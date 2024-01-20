/*
   File: rts_alloc.c
   Provides basic allocation routines and garbage collection

   CVS ID: "$Id: rts_alloc.c,v 1.5 2005/04/06 18:55:27 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* Check if we have SIZEOF_CHAR_P */
#ifndef SIZEOF_CHAR_P
#define SIZEOF_CHAR_P 8
#endif

/* standard includes */
#include <stdio.h>

/* Check whether to include malloc or stdlib.h */
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif

/* Check if configured to use threads */
#ifdef USE_THREADS
#include <pthread.h>
#endif

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"

/* When configured to use threads, reserve a lock */
#ifdef USE_THREADS
static pthread_mutex_t alloc_mutex;
#endif

/*
   Elan heap objects (such as TEXTs) will be have the following layout:

      unsigned short ref_count;	ref count & (1 << 15) denotes constant
				ref count & (1 << 14) denotes guarded object
      unsigned short obj_len;	object length in bytes
      char object[obj_len];	actual object,
				padded to double word +4 alignment
   
   Since we plan to do our own memory management (and garbage collection),
   we will maintain free lists for object sizes that are small.
   Our own object overhead is 4 bytes (on Intel or Sparc). Since we do
   not want the overhead caused by malloc (minimum size allocated is
   16 bytes), we allocate the memory for small objects from malloced chunks
   of 64KB, maintained as a simple linked list. For large objects we directly
   call malloc and free.

   The uninitialized object is represented by the NULL pointer.
   The NIL object is represented by the NULL pointer + 1.
*/
#define constant_bit	(8 * sizeof (short) - 1)
#define guard_bit	(8 * sizeof (short) - 2)
#define constant_mask	(1 << constant_bit)
#define guard_mask	(1 << guard_bit)
#define count_mask	((1 << guard_bit) - 1)
#define ELAN_NULL	((char *) NULL)
#define ELAN_NIL	((char *) 1)
#define MAX_REFCOUNT	((1 << guard_bit) - 1)

/*
   Define code for controlled malloc and free
*/
#define ctl_free(ptr) free (ptr)
static char *ctl_malloc (int size)
	{ char *new = (char *) malloc ((unsigned) size);
	  if (new == NULL)
	     rts_panic ("could not malloc %d bytes", size);
#ifdef DEBUG
          if (debug)
             rts_log ("Allocated %d bytes at %p", size, (void *) new);
#endif
	  return (new);
	};

#define ADM_SIZE (2 * sizeof (short))
#define CHUNK_SIZE 65536
static char *current_chunk;
static char *heap_ptr;
static void allocate_chunk ()
	{ char *new_chunk = ctl_malloc (CHUNK_SIZE);
	  int offset = (sizeof (char *) + ADM_SIZE + 7) & ~7;
	  * (char **) new_chunk = current_chunk;
	  current_chunk = new_chunk;
	  heap_ptr = new_chunk + offset;
#ifdef DEBUG
	  if (debug)
	     rts_log ("Allocating chunk from %p to %p",
		      (void *) new_chunk, (void *) (new_chunk + CHUNK_SIZE));
#endif
	};

static void verify_managed_address (char *optr)
	{ char *chunk_ptr = * ((char **) current_chunk);
	  unsigned long oint = (unsigned long) optr;
	  if (((unsigned long) current_chunk < oint) && (oint < (unsigned long) heap_ptr)) return;
	  while (chunk_ptr != NULL)
	     { if (((unsigned long) chunk_ptr < oint) &&
		   (oint < (unsigned long) (chunk_ptr + CHUNK_SIZE))) return;
	       chunk_ptr = * ((char **) chunk_ptr);
	     };
	  rts_panic ("found non managed address, %p", (void *) optr);
	};

/*
   The following counts keep track of proper use of the allocator
*/
#define ANCHORS_SIZE 256
static char *anchors[ANCHORS_SIZE];
static int requested[ANCHORS_SIZE];
static int returned[ANCHORS_SIZE];
static int allocated[ANCHORS_SIZE];
static int huge_allocs, huge_frees;
static char *allocate_from_chunk (int offset)
	{ char *new;
	  int diff;
	  diff = (int) (heap_ptr - current_chunk);
	  if (diff + offset > CHUNK_SIZE)
	     { /* what we need does not fit in current chunk */
	       /* if what remains is large enough, add to free lists */
	       int rem_offset = CHUNK_SIZE - diff;	/* Note always multiple of 8 */
	       int rem_objlen = rem_offset - ADM_SIZE;
	       if (rem_objlen >= SIZEOF_CHAR_P)
	          { /* add the remainder to the free lists */
		    int idx = rem_objlen >> 3;
		    ((unsigned short *) heap_ptr)[-1] = (unsigned short) rem_objlen;
		    ((unsigned short *) heap_ptr)[-2] = (unsigned short) 0;
		    * (char **) heap_ptr = anchors[idx];
		    anchors[idx] = heap_ptr;
		    allocated[idx]++;			/* Note as allocation */
		  };
	       allocate_chunk ();
	     };
	  new = heap_ptr;
	  heap_ptr = heap_ptr + offset;
	  ((unsigned short *) new)[-1] = (unsigned short) (offset - ADM_SIZE);
	  ((unsigned short *) new)[-2] = (unsigned short) 0;
	  return (new);
	};

/*
   rts_malloc will allocate a fresh object either from the free list
   pool or allocate a new one from the current chunk
*/
#define MAX_SIZE (1 << (8 * sizeof (short)))
static char *protected_malloc (int size)
	{ int msize, offset, objlen, idx;
	  char *new, *ptr;

	  /* check size to fit in an unsigned short */
	  if ((size <= 0) || (size >= MAX_SIZE))
	       rts_panic ("trying to allocate %d bytes", size);

	  /* make sure size is in units of 4 bytes
	     and large enough to hold a single pointer */
	  msize = (size + 3) & ~3;	
#if SIZEOF_CHAR_P > 4
	  if (msize < SIZEOF_CHAR_P) msize = SIZEOF_CHAR_P;
#endif
	  /* calculate (double aligned) offset, object length */
	  offset = (msize + ADM_SIZE + 7) & ~7;
	  objlen = offset - ADM_SIZE;
	  if (objlen >= MAX_SIZE)
	     rts_panic ("trying to allocate %d bytes", size);

	  /* calculate free list index and allocate the object */
	  idx = objlen >> 3;
	  if (idx > ANCHORS_SIZE - 1)			/* if large object */
	     { new = ctl_malloc (offset + 8) + 8;	/* then use libc malloc */
	       huge_allocs++;
	     }
	  else
	     { requested[idx]++;			/* Note request */
	       if ((new = anchors[idx]) != NULL)	/* if present on free lists */
		  anchors[idx] = * (char **) new;	/* then reuse object */
	       else
		  { new = allocate_from_chunk (offset);
		    allocated[idx]++;			/* Note allocation */
		  };
	     };

	  /* clear the object, fill in object length and initial refcount */
	  for (ptr = new; ptr < new + objlen; ptr++) *ptr = 0;
	  ((unsigned short *) new)[-1] = (unsigned short) objlen;
	  ((unsigned short *) new)[-2] = (unsigned short) 1;
	  return (new);
	};

char *rts_malloc (int size)
	{ char *new_object;
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&alloc_mutex))
	     rts_panic ("Could not lock mutex in memory allocator: rts_malloc");
#endif
	  new_object = protected_malloc (size);  
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&alloc_mutex))
	     rts_panic ("Could not unlock mutex in memory allocator: rts_malloc");
#endif
	  return (new_object);
	};
/*
   Detach decrements the reference count of the indirect argument
   while nulling the pointer the argument is pointing to.
   If the reference count drops to 0, the object is recycled
*/
static void protected_detach (char **ptr)
	{ unsigned short *refc_ptr;
	  unsigned short refc;
	  char *optr;
	  int idx;

	  /* check if arg is null */
	  if (ptr == NULL)
	     rts_panic ("rts_detach called with NULL arg");
	  optr = *ptr;				/* pick up ptr to object */;

	  /* lose ref to object */
	  if (optr == NULL) return;		/* uninitialized object */
	  *ptr = NULL;				/* lose reference */
	  if (optr == ELAN_NIL) return;		/* NIL object */

	  /* locate reference count and check it */
	  refc_ptr = (unsigned short *) (optr - ADM_SIZE);
	  refc = *refc_ptr;			/* pick up ref count */
	  if (refc & constant_mask) return;	/* constant, done with */
	  if (refc == 0)			/* bad if ref count = 0 */
	     rts_panic ("Freeing a free object");
	  if (refc != 1)
	     { /* detach is safe, decrement the ref count */
	       /* if ref count becomes 1, drop the guard bit */
	       if (((refc - 1) & count_mask) == 1) *refc_ptr = 1;
	       else *refc_ptr = refc - 1;
	       return;
	     };

	  /* the object may be freed */
	  idx = ((int)(refc_ptr[1])) >> 3;	/* convert length into list idx */
	  if (idx > ANCHORS_SIZE - 1)		/* if large object */
	     { ctl_free (optr - 8);		/* then use libc free */
	       huge_frees++;			/* note free */
	     }
	  else
	     { if (debug) verify_managed_address (optr);
	       * (char **) optr = anchors[idx];	/* else insert into free list */
	       anchors[idx] = optr;
	       returned[idx]++;
	     };
	};

void rts_detach (char **ptr)
	{
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&alloc_mutex))
	     rts_panic ("Could not lock mutex in memory allocator: rts_detach");
#endif
	  protected_detach (ptr);
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&alloc_mutex))
	     rts_panic ("Could not unlock mutex in memory allocator: rts_detach");
#endif
	};

/*
   Predetach tries to decrement the reference count of the indirect
   argument while nulling the pointer the argument is pointing to.
   If the reference count would drop to 0, it returns the pointer to
   the object, signifying that its subfields should be detached
   before detaching the actual object. It returns NULL otherwise.
*/
static char *protected_predetach (char **ptr)
	{ unsigned short *refc_ptr;
	  unsigned short refc;
	  char *optr;

	  /* check if arg is null */
	  if (ptr == NULL)
	     rts_panic ("rts_predetach called with NULL arg");
	  optr = *ptr;				/* pick up ptr to object */;

	  /* check special objects */
	  if (optr == NULL) return (NULL);	/* uninitialized object */
	  *ptr = NULL;				/* lose reference */
	  if (optr == ELAN_NIL) return (NULL);	/* Nil object is ok */

	  /* locate reference count and check it */
	  refc_ptr = (unsigned short *) (optr - ADM_SIZE);
	  refc = *refc_ptr;			/* pick up ref count */
	  if (refc & constant_mask)		/* constant, done with */
	     return (NULL);
	  if (refc == 0)			/* bad if ref count = 0 */
	     rts_panic ("Freeing a free object");

	  /* if ref count equals 1, signal the caller to get
	     rid of the subfield(s) before detaching */
	  if (refc == 1) return (optr);

	  /* detach is safe, decrement the ref count */
	  /* if ref count becomes 1, drop the guard bit */
	  if (((refc - 1) & count_mask) == 1) *refc_ptr = 1;
	  else *refc_ptr = refc - 1;
	  return (NULL);
	};

char *rts_predetach (char **ptr)
	{ char *old_object;
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&alloc_mutex))
	     rts_panic ("Could not lock mutex in memory allocator: rts_predetach");
#endif
	  old_object = protected_predetach (ptr);  
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&alloc_mutex))
	     rts_panic ("Could not unlock mutex in memory allocator: rts_predetach");
#endif
	  return (old_object);
	};

/*
   Attach tries to increment the reference count of the argument.
   This argument is passed by reference to enable a possible overwrite
   of the argument by the return value.

   If succesfull, attach returns its argument.
   However, if the object is guarded it creates a copy of the object 
   and returns a pointer to the newly created copy.

   Nog doen: check op MAX_REFCOUNT
*/
static char *protected_attach (char **ptr)
	{ unsigned short *refc_ptr;
	  unsigned short refc;
	  char *optr;
	  char *new;
	  char *sptr;
	  char *dptr;
	  int len;

	  /* check if arg is null */
	  if (ptr == NULL)
	     rts_panic ("rts_attach called with NULL arg");
	  optr = *ptr;				/* pick up ptr to object */;

	  /* check special objects */
	  if (optr == NULL)			/* Uninitialized object */
	     rts_error ("Attempt to copy an uninitialized value");
	  if (optr == ELAN_NIL) return (optr);	/* Nil object is ok */

	  /* locate ref count */
	  refc_ptr = (unsigned short *)(optr - ADM_SIZE);
	  refc = *refc_ptr;			/* pick up ref count */
	  if (refc & constant_mask)		/* is it a constant, done */
	     return (optr);
	  if (refc == 0)			/* bad if ref count = 0 */
	     rts_panic ("Attaching a free object");
	  
	  if (!(refc & guard_mask))		/* if not guarded */
	     { *refc_ptr = refc + 1;		/* increase ref count */
	       return (optr);			/* done, all is well */
	     };
	
	  /* copy the object */
	  len = ((int)(refc_ptr[1]));		/* pickup length */
	  new = protected_malloc (len);		/* allocate new object */
	  for (sptr = optr, dptr = new; sptr < optr + len; sptr++, dptr++)
	     *dptr = *sptr;

	  /* return the copied object */
	  return (new);
	};

char *rts_attach (char **ptr)
	{ char *new_object;
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&alloc_mutex))
	     rts_panic ("Could not lock mutex in memory allocator: rts_attach");
#endif
	  new_object = protected_attach (ptr);  
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&alloc_mutex))
	     rts_panic ("Could not unlock mutex in memory allocator: rts_attach");
#endif
	  return (new_object);
	};

/*
   Guard tries to increment the reference count of the first arg
   while setting the guard bit, indicating that the object will
   remain in VAR access (therefore may not be shared) until the
   reference is removed. If no object is pointed to by the first arg
   it will dynamically create an object of the correct size.

   When the object is already shared it will create a private copy
   of the object and return a pointer to the newly created object.
   In other cases it will return NULL indicating that the subfields
   need not be attached
*/
static char *protected_guard (char **ptr, int size)
	{ unsigned short *refc_ptr;
	  unsigned short refc;
	  char *optr;
	  char *new;
	  char *sptr;
	  char *dptr;

	  /* check if arg is null */
	  if (ptr == NULL)
	     rts_panic ("rts_guard called with NULL arg");
	  optr = *ptr;				/* pick up ptr to object */;

	  /* check special objects: Uninitialized or NIL */
	  if (optr == ELAN_NIL) return (NULL);
	  else if (optr == NULL)
	     { /* create object */
	       optr = protected_malloc (size);
	       *ptr = optr;
	     };

	  /* locate reference count and check it */
	  refc_ptr = (unsigned short *) (optr - ADM_SIZE);
	  refc = *refc_ptr;			/* pick up reference count */
	  if (refc & constant_mask)		/* bad if it is constant */
	     rts_panic ("Modifying internals of a constant");
	  if (refc == 0)			/* bad if ref count = 0 */
	     rts_panic ("Modifying internals of a free object");

	  /* if refcount = 1, make it 2 and set guard */
	  if (refc == 1)			/* ref count 1 */
	     { *refc_ptr = (unsigned short) 2 | guard_mask;
	       return (NULL);
	     };

	  /* if already guarded, just increase ref count and be done */
	  if (refc & guard_mask)
	     { *refc_ptr = refc + 1;
	       return (NULL);
	     };

	  /* Not guarded but refcount is bigger than 1, copy the object */
	  new = protected_malloc (size);
	  for (sptr = optr, dptr = new; sptr < optr + size; sptr++, dptr++)
	     *dptr = *sptr;

	  /* set guard bit on new object and indicate we have copied */
	  refc_ptr = (unsigned short *) (new - ADM_SIZE);
	  *refc_ptr = (unsigned short) 2 | guard_mask;
	  *ptr = new;
	  return (new);
	};

char *rts_guard (char **ptr, int size)
	{ char *var_object;
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&alloc_mutex))
	     rts_panic ("Could not lock mutex in memory allocator: rts_guard");
#endif
	  var_object = protected_guard (ptr, size);  
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&alloc_mutex))
	     rts_panic ("Could not unlock mutex in memory allocator: rts_guard");
#endif
	  return (var_object);
	};

/*
   Initialize the garbage collection administration
*/
void rts_init_gc ()
	{ int ix;
	  current_chunk = NULL;
	  huge_allocs = 0;
	  huge_frees = 0;
	  for (ix = 0; ix < ANCHORS_SIZE; ix++)
	     { anchors[ix] = NULL;
	       allocated[ix] = 0;
	       requested[ix] = 0;
	       returned[ix] = 0;
	     };
	  allocate_chunk ();

	  /* If configured to use threads, init mutex */
#ifdef USE_THREADS
	  if (pthread_mutex_init (&alloc_mutex, NULL))
	     rts_panic ("Could not initialize mutex in memory allocator");
#endif
	};

/*
   first routines for msck: memory system consistency check
*/
static int free_count[ANCHORS_SIZE];
static void check_free_lists ()
	{ int ix;
	  for (ix = 0; ix < ANCHORS_SIZE; ix++)
	     { int objlen = 0;
	       int count = 0;
	       char *ptr = anchors[ix];
	       while (ptr != NULL)
		  { int refcount = (int)(((short *) ptr)[-2]);
		    int len = (int)(((unsigned short *) ptr)[-1]);
		    if (refcount)
		       rts_panic ("found free object of length %d with refcount %d",
				  len, refcount);
		    if (len >> 3 != ix)
		       rts_panic ("found free object of length %d on queue %d", len, ix);
		    if (!objlen) objlen = len;
		    else if (len != objlen)
		       rts_panic ("found objects of size %d and size %d on queue %d",
				  len, objlen, ix);
		    count++;
		    verify_managed_address (ptr);
		    ptr = *((char **) ptr);
		  };
	       free_count[ix] = count;
	     };
	};

#define rep_string \
"queue %d: allocs = %d, %d on free list, requests = %d, frees = %d"
static void make_report ()
	{ int ix;
	  rts_log ("Memory report:");
	  for (ix = 0; ix < ANCHORS_SIZE; ix++)
	     { int alloced = allocated[ix];
	       if (alloced)
		  rts_log (rep_string, ix, alloced, free_count[ix], requested[ix], returned[ix]);
	     };
	  rts_log ("%d huge allocs, %d huge frees", huge_allocs, huge_frees);
	};

void report_alloc ()
	{ check_free_lists ();
	  make_report ();
	};
