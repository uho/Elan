/*
   File: dcg_alloc.c
   Provides basic allocation routines and garbage collection
   while lessening the overhead caused by malloc and free

   Copyright (C) 2008-2011 Marc Seutter

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_alloc.c,v 1.17 2012/12/22 16:19:44 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* standard includes */
#include <stdio.h>
#include <string.h>

/* Check whether to include malloc or stdlib.h */
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#else
#include <stdlib.h>
#endif

/* local includes */
#include "dcg_error.h"
#include "dcg_alloc.h"

/*
   We are going to combine the pooled memory allocation devised by Olaf Seibert
   for the AGFL project with the one already used by dcg with the following
   modifications.

   Memory is allocated from big page aligned chunks (dcg already used big
   chunks, but they were kept internal in the memory allocator). From these
   chunks, objects may be allocated in three different ways, namely as

   a) struct aligned managed objects, having a ref count and an object length
      preceding the actual object i.e. looking like

      int32	ref_count;		ref count < 0 denotes constant
      u_int32	obj_len;		object length in bytes
      char	object[obj_len];	actual object being pointed at

      For the smaller of these objects (<= 1KB), free lists are maintained.
      In compilers and especially the EAG3 runtime system, small objects are
      continuously recycled. For larger objects (but not too large, see remark
      below), a separate free list is maintained that should aim at recombining
      adjacent free objects (Think of TEXT affixes that accumulate the result
      of a parse). When one of the free lists is empty upon a request, the large
      free list is first checked before trying to allocate from the managed pool.

   b) struct aligned unmanaged object, whose object length is a multiple of
      sizeof (void *). Since they are unmanaged, they can only get freed
      upon the release of the pool.

   c) unaligned unmanaged char *objects, only to be freed at the release of
      a pool.

   The typical use of these latter two object types is for the construction and
   loading of trie structures needed to maintain vocabularies.

   You should note one thing in particular though: whenever a managed requested object
   size is bigger than a quarter of the chunk size, we delegate the administration
   to the raw allocator. Unmanaged or unaligned object requests of this size are
   forbidden.
*/

/*
   Raw allocation to start with
*/
void *dcg_raw_malloc (size_t size)
{ void *new_ptr = malloc (size);
  if (new_ptr == NULL) dcg_panic ("could not malloc %ld bytes", (long) size);
#ifdef MEMCHECK
  if (debug) dcg_wlog ("Allocated %ld bytes at %p", (long) size, new_ptr);
#endif
  return (new_ptr);
}

void dcg_raw_free (void *ptr)
{
#ifdef MEMCHECK
  if (debug) dcg_wlog ("Freed memory chunk at %p", ptr);
#endif
  free (ptr);
}

/*
   Basically a pool block is a large chunk of memory whose size is a power of 2
   times the basic page size, so that it gets malloced on a page boundary (FreeBSD)
   or from a private anonymous mapping using mmap (glibc malloc). It is unknown
   (at least not described at MSDN) whether MS Windows has any similar optimization.

   After allocation the pool block administration is written at the start of the pool
   It is linked to its predecessor block in this pool as well as the pool administration
   of the whole pool.
*/
typedef struct dcg_pool_block_rec *pool_block;
struct dcg_pool_block_rec
{ char *mem;			/* Pointing to free memory */
  size_t avail;			/* Size still available in bytes */
  pool_block prev;		/* Previously allocated block */
  pool my_pool;			/* To locate the pool */
};

/*
   The pool itself must contain quite a lot of administration
   Note that we need 3 pool blocks for the 3 regimes of allocation
   All pools constitute a double linked list: hence we are able to
   reach all our managed memory from the default pool.

   All our managed objects have lengths that are multiples of 8.
*/
#define ANCHORS_SIZE 128
struct dcg_pool_rec
{ /* Pool administration */
  char *name;			/* Name of the pool: keep this one static */
  pool prev;			/* Previous pool */
  pool next;			/* Next pool */
  size_t block_size;		/* Size of pool blocks */
  size_t total_allocated;	/* Total of memory in pool blocks */

  /* Managed administration */
  pool_block managed;		/* Managed blocks */
  void *large_free_list;	/* Free list for large objects */
  void *anchors[ANCHORS_SIZE];	/* Anchors for free lists */
  int allocated[ANCHORS_SIZE];	/* Allocated for this size */
  int freed[ANCHORS_SIZE];	/* Currently on free list */
  
  /* Unmanaged and unaligned administration */
  pool_block unmanaged;		/* Unmanaged blocks */
  pool_block unaligned;		/* Unaligned blocks */
};
 
/*
   Pool initialization and release
   The first pool to be allocated becomes the default pool
*/
pool my_default_pool = NULL;
#define DEFAULT_SIZE (1 << 20)
pool dcg_pool_init (char *name, size_t block_size)
{ pool my_pool = (pool) dcg_raw_malloc (sizeof (struct dcg_pool_rec));
  int ix;

  my_pool -> name = name;
  if (my_default_pool == NULL)
    { my_pool -> next = my_pool;
      my_pool -> prev = my_pool;
      my_default_pool = my_pool;
    }
  else
    { my_pool -> next = my_default_pool -> next;
      my_pool -> prev = my_default_pool;
      my_pool -> next -> prev = my_pool;
      my_default_pool -> next = my_pool;
    };

  if (block_size) my_pool -> block_size = block_size;
  else my_pool -> block_size = DEFAULT_SIZE;
  my_pool -> total_allocated = 0;
  my_pool -> managed = NULL;
  for (ix = 0; ix < ANCHORS_SIZE; ix++)
    { my_pool -> anchors[ix] = NULL;
      my_pool -> allocated[ix] = 0;
      my_pool -> freed[ix] = 0;
    };
  my_pool -> large_free_list = NULL;
  my_pool -> unmanaged = NULL;
  my_pool -> unaligned = NULL;
  return (my_pool);
}

static void release_pool_blocks (pool_block chain)
{ pool_block cur = chain;
  while (cur != NULL)
    { pool_block prev = cur -> prev;
      dcg_raw_free (cur);
      cur = prev;
    };
}

void dcg_pool_release (pool old_pool)
{ if (old_pool == NULL)
    dcg_abort ("dcg_pool_release", "trying to release NULL pool");
  if (old_pool == my_default_pool)
    { if ((old_pool -> next != old_pool) || (old_pool -> prev != old_pool))
	dcg_abort ("dcg_pool_release",
		   "The default pool must be the last memory pool to be released");
      my_default_pool = NULL;
    }
  else
    { old_pool -> prev -> next = old_pool -> next;
      old_pool -> next -> prev = old_pool -> prev;
    };
  release_pool_blocks (old_pool -> managed);
  release_pool_blocks (old_pool -> unmanaged);
  release_pool_blocks (old_pool -> unaligned);
  dcg_raw_free (old_pool);
}

/*
   Pool block allocation and verification
*/
static void allocate_pool_block (pool my_pool, pool_block *prev)
{ pool_block new_block = (pool_block) dcg_raw_malloc (my_pool -> block_size);
  my_pool -> total_allocated += my_pool -> block_size;

  new_block -> mem = (char *)(new_block + 1);	/* Note: pointer arithmetic */
  new_block -> avail = my_pool -> block_size - sizeof (*new_block);
  new_block -> prev = *prev;
  new_block -> my_pool = my_pool;
  *prev = new_block;
}

static void verify_managed_address (pool_block managed, void *optr)
{ unsigned long oadr = (unsigned long) optr;
  pool_block bptr = managed;
  while (bptr != NULL)
    { unsigned long bstart = (unsigned long)(bptr + 1);	/* Pointer arithmetic */
      unsigned long bend = (unsigned long)(bptr -> mem);
      if ((bstart <= oadr) && (oadr < bend))
	return;
      bptr = bptr -> prev;
    };
  dcg_abort ("verify_managed_address", "found non managed address %p", optr);
}

/*
   dcg_pool_malloc will allocate a fresh object either from the free lists, then
   from the large objects free list and if all else fails from the current pool
*/
#define ADMIN_SIZE 2 * sizeof (u_int32)
static void *try_default_to_raw_malloc (pool my_pool, size_t object_len)
{ /* If we need something bigger than 1/8th of the blocksize, we will not handle it */
  u_int32 *bptr;
  size_t comb_len = object_len + ADMIN_SIZE;
  if (comb_len < ((my_pool -> block_size) >> 3))
    return (NULL);

  bptr = (u_int32 *) dcg_raw_malloc (comb_len);
  return ((void *)(bptr + 2));		/* Pointer arithmetic */
}

static void *try_pick_from_free_list_anchors (pool my_pool, size_t object_len, size_t *ridx)
{ size_t picked_len;
  void *new_ptr;

  /* Calculate free list index and check if we can take it from the free lists */
  size_t idx = (object_len >> 3) - 1;
  *ridx = idx;
  if (idx >= ANCHORS_SIZE) return (NULL);

#ifdef MEMCHECK
  if (((my_pool -> anchors[idx] == NULL) && (my_pool -> freed[idx] != 0)) ||
      ((my_pool -> anchors[idx] != NULL) && (my_pool -> freed[idx] == 0)))
    dcg_abort ("dcg_pool_detach", "free list administration mismatch");
#endif

  new_ptr = my_pool -> anchors[idx];
  if (new_ptr == NULL) return (NULL);
  my_pool -> freed[idx]--;

  /* Object can be taken: check that it has the same length */
  my_pool -> anchors[idx] = *(void **) new_ptr;
  picked_len = (size_t)(((u_int32 *) new_ptr)[-1]);

  if (picked_len != object_len)
    dcg_abort ("try_pick_from_free_list_anchors",
	       "Found object of wrong length %ld, expected %ld",
	       (long) picked_len, (long) object_len);
  return (new_ptr);
}

static void *try_pick_from_large_free_list (pool my_pool, size_t object_len)
{ size_t large_len = object_len + 16L;
  void **cur_ptr = &my_pool -> large_free_list;
  while ((*cur_ptr) != NULL)
    { void *current = *cur_ptr;
      size_t cur_obj_len = (size_t)(((u_int32 *) current)[-1]);

      /* Check if the current object has an exact matching length: if so recycle */
      if (cur_obj_len == object_len)
	{ *cur_ptr = *(void **) current;
	  return (current);
	};

      /*
	 If the current object is sufficiently larger i.e. we can split in the requested size
         plus the extra overhead of an extra smaller piece, we will do so
      */
      if (cur_obj_len >= large_len)
	{ /* We can split */
	  size_t new_size = cur_obj_len - object_len - ADMIN_SIZE;
	  size_t new_idx = (new_size >> 3) - 1;
	  void *new_cur_obj_ptr = (void *)((char *) current + object_len + ADMIN_SIZE);
	  ((u_int32 *) new_cur_obj_ptr)[-1] = (u_int32) new_size;
	  ((u_int32 *) new_cur_obj_ptr)[-2] = 0;
	  ((u_int32 *) current)[-1] = (u_int32) object_len;
	  if (new_idx < ANCHORS_SIZE)
	    { /* Recycle small piece through the free lists */
	      *(void **) new_cur_obj_ptr = my_pool -> anchors[new_idx];
	      my_pool -> anchors[new_idx] = new_cur_obj_ptr;
	      *cur_ptr = *(void **) current;
	    }
	  else
	    { *(void **) new_cur_obj_ptr = *(void **) current;
	      *cur_ptr = new_cur_obj_ptr;
	    };
	  return (current);
	};

      cur_ptr = (void **) current;
    };

  return (NULL);
}

static void *try_pick_from_available_pool_space (pool_block chain, size_t total_len)
{ pool_block cur = chain;
  while (cur != NULL)
    { if (cur -> avail > total_len)
	{ void *new_ptr = (void *) cur -> mem;
	  cur -> mem += total_len;
	  cur -> avail -= total_len;
	  return (new_ptr);
	};
      cur = cur -> prev;
    };
  return (NULL);
}

static void *try_pick_from_pool (pool my_pool, size_t object_len)
{ /* Check the object size: if it is too large we default to the raw malloc */
  size_t total_len = object_len + ADMIN_SIZE;
  size_t idx;
  void *new_ptr = try_default_to_raw_malloc (my_pool, object_len);
  if (new_ptr != NULL) return (new_ptr);

  /* Check the free list anchors */
  new_ptr = try_pick_from_free_list_anchors (my_pool, object_len, &idx);
  if (new_ptr != NULL) return (new_ptr);

  /* Check the large free list administration */
  new_ptr = try_pick_from_large_free_list (my_pool, object_len);
  if (new_ptr != NULL)
    { if (idx < ANCHORS_SIZE) my_pool -> allocated[idx]++;
      return (new_ptr);
    };

  /* Check the remaining available pool space */
  new_ptr = try_pick_from_available_pool_space (my_pool -> managed, total_len);
  if (new_ptr != NULL)
    { if (idx < ANCHORS_SIZE) my_pool -> allocated[idx]++;
      return ((void *)((char *) new_ptr + ADMIN_SIZE));
    };

  /* Final resort: allocate a new block and take it from there */
  allocate_pool_block (my_pool, &my_pool -> managed);
  new_ptr = try_pick_from_available_pool_space (my_pool -> managed, total_len); 
  if (new_ptr != NULL)
    { if (idx < ANCHORS_SIZE) my_pool -> allocated[idx]++;
      return ((void *)((char *) new_ptr + ADMIN_SIZE));
    };

  /* We should not be able to get here */
  dcg_internal_error ("try_pick_from_pool");
  return (NULL);
}

void *dcg_pool_malloc (pool my_pool, size_t size)
{ size_t object_len;
  void *new_ptr;

  /* Check the pool and align the size */
  if (my_pool == NULL)
    dcg_abort ("dcg_pool_malloc", "using NULL pool");
  if (size == 0)
    dcg_abort ("dcg_pool_malloc", "trying to allocate 0 bytes");
  object_len = (size + 7L) & (~7L);

  /* Pick object from available sources */
  new_ptr = try_pick_from_pool (my_pool, object_len);
  if (new_ptr == NULL)
    dcg_abort ("dcg_pool_malloc", "could not allocate object of size %ld in pool %s",
	       object_len, my_pool -> name);

  /* Initialize object */
  memset (new_ptr, 0, object_len);		/* Clear memory */
  ((u_int32 *) new_ptr)[-1] = object_len;	/* Remember the size */
  ((int32 *) new_ptr)[-2] = 1;			/* We are freshly born */
  return (new_ptr);
}

/*
   Dcg_pool_detach is the opposite of dcg_pool_malloc. First it will try and lower
   the reference count of the indirect argument while nulling the pointer the
   argument is refering to. When the reference count drops to 0, the object
   is recycled. Depending on the size of the freed object, it may end up in
   one of the free lists or be relinquished to the general malloc administration.
*/
void dcg_pool_detach (pool my_pool, void **ptr)
{ void **ins_ptr;				/* Insert pointer */
  void *optr = *ptr;				/* pick up ptr to object */
  int32 *ref_ptr = (int32 *) optr;		/* gain access to ref count */
  size_t object_len, comb_len, idx;
  void *nptr;

  /* lose ref to object */
  if (my_pool == NULL)
    dcg_abort ("dcg_pool_detach", "using NULL pool");
  if (optr == NULL) return;			/* uninitialized object */
  *ptr = NULL;					/* lose reference */

  /* check reference count */
  if (ref_ptr[-2] < 0) return;			/* constant object, done */
  if (ref_ptr[-2] == 0)
    dcg_abort ("dcg_pool_detach", "freeing a free object");
  ref_ptr[-2] -= 1;				/* decrement ref count */
  if (ref_ptr[-2] != 0) return;			/* if not 0, done */

  /* Pick up object length to decide how to deallocate */
  object_len = (size_t)(((u_int32 *) optr)[-1]);
  comb_len = object_len + ADMIN_SIZE;

  /* Very huge objects go to the raw free */
  if (comb_len >= ((my_pool -> block_size) >> 3))
    { dcg_raw_free (&ref_ptr[-2]);
      return;
    };

#ifdef MEMCHECK
  verify_managed_address (my_pool -> managed, optr);
#endif

  /* Very small ones to the free lists */
  idx = (object_len >> 3) - 1;
  if (idx < ANCHORS_SIZE)
    {
#ifdef MEMCHECK
      if (((my_pool -> anchors[idx] == NULL) && (my_pool -> freed[idx] != 0)) ||
	  ((my_pool -> anchors[idx] != NULL) && (my_pool -> freed[idx] == 0)))
	dcg_abort ("dcg_pool_detach", "free list administration mismatch");
#endif
      *(void **)optr = my_pool -> anchors[idx];
      my_pool -> anchors[idx] = optr;
      my_pool -> freed[idx]++;
      return;
    }

  /*
     We must deallocate to the large free list. Locate the right
     insertion point: we keep the list sorted by address, so that
     we are able to coalesce two blocks if they are adjacent
  */
  ins_ptr = &my_pool -> large_free_list;
  while (*ins_ptr != NULL)
    { if ((unsigned long) optr < (unsigned long) *ins_ptr)
	break;
      ins_ptr = (void **) (*ins_ptr);
    };

  /* First insert */
  *(void **)optr = *ins_ptr;
  *ins_ptr = optr;

  /* Check to coalesce */
  nptr = (void *) ((char *) optr + comb_len);
  if (nptr != *(void **) optr) return;		/* No coalescing possible */

  /* Coalesce the two blocks into one */
  *(void **) optr = *(void **) nptr;
  (((u_int32 *) optr)[-1]) += ((((u_int32 *) nptr)[-1]) + ADMIN_SIZE);
}

/*
   dcg_pool_unmanaged_malloc will allocate memory from the unmanaged pool block list:
   aligned but kept alive until the whole pool is released
*/
void *dcg_pool_unmanaged_malloc (pool my_pool, size_t size)
{ size_t object_len;
  void *new_ptr;

  /* Check and align the size */
  if (size == 0)
    dcg_abort ("dcg_pool_unmanaged_malloc", "trying to allocate 0 bytes");
  object_len = (size + 7L) & (~7L);

  /* Check for impossible large sizes: anything this big must use the managed scheme */
  if (object_len >= ((my_pool -> block_size) >> 3))
    dcg_abort ("dcg_pool_unmanaged_malloc", "trying to allocate %ld bytes", (long) object_len);

  /* Check the remaining available pool space */
  new_ptr = try_pick_from_available_pool_space (my_pool -> unmanaged, object_len);
  if (new_ptr != NULL) return (new_ptr);

  /* Final resort: allocate a new pool and take it from there */
  allocate_pool_block (my_pool, &my_pool -> unmanaged);
  return (try_pick_from_available_pool_space (my_pool -> unmanaged, object_len)); 
}

/*
   dcg_pool_unaligned_malloc will allocate memory from the unmanaged pool block list:
   aligned but kept alive until the whole pool is released
*/
char *dcg_pool_unaligned_malloc (pool my_pool, size_t size)
{ /* Check the size */
  char *new_ptr;
  if (size == 0)
    dcg_abort ("dcg_pool_unmanaged_malloc", "trying to allocate 0 bytes");

  /* Check for impossible large sizes: anything this big must use the managed scheme */
  if (size >= ((my_pool -> block_size) >> 3))
    dcg_abort ("dcg_pool_unaligned_malloc", "trying to allocate %ld bytes", (long) size);

  /* Check the remaining available pool space */
  new_ptr = (char *) try_pick_from_available_pool_space (my_pool -> unaligned, size);
  if (new_ptr != NULL) return (new_ptr);

  /* Final resort: allocate a new pool and take it from there */
  allocate_pool_block (my_pool, &my_pool -> unaligned);
  return ((char *) try_pick_from_available_pool_space (my_pool -> unaligned, size)); 
}

/*
   dcg_pool_stats will give a report about the pool usage
*/
void dcg_pool_stats (pool my_pool)
{ int ix;
  dcg_wlog ("Total allocated in pool %s is %d", my_pool -> name, my_pool -> total_allocated);
  dcg_wlog ("Free list administration");
  dcg_wlog ("Index\t  Size\t\tAlloc\tFree");
  for (ix = 0; ix < ANCHORS_SIZE; ix++)
    dcg_wlog ("%2d\t%3d (+8)\t%d\t%d", ix, (ix << 3) + 8,
	      my_pool -> allocated[ix], my_pool -> freed[ix]);
}

/*
   Derived pool allocation schemes. Note that dcg_realloc and dcg_recalloc have
   a different calling convention than the usual realloc because of the need
   to detach the original.
*/
void *dcg_pool_calloc (pool my_pool, size_t nr, size_t size)
{ return (dcg_pool_malloc (my_pool, nr * size));	/* as you might expect */
}

void dcg_pool_realloc (pool my_pool, void **ptr, size_t size)
{ void *optr = *ptr;					/* pick up ptr to object */
  size_t osize;
  void *nptr;
  int32 refc;

  /* check arguments */
  if (optr == NULL)
    dcg_abort ("dcg_pool_realloc", "trying to reallocate non existing object");
  if (size == 0)
    dcg_abort ("dcg_pool_realloc", "trying to reallocate %d bytes", size);

  /* check reference count */
  refc = ((int32 *) optr)[-2];
  if (refc < 0)
    dcg_abort ("dcg_pool_realloc", "trying to reallocate a constant object");
  else if (refc == 0)
    dcg_abort ("dcg_pool_realloc", "trying to reallocate a free object");
  else if (refc > 1)
    dcg_abort ("dcg_pool_realloc", "trying to reallocate a shared object");

  /* if requested size smaller than resident size, done with */
  osize = (size_t) ((u_int32 *) optr)[-1];
  if (size < osize) return;
  
  /* Allocate and copy */
  nptr = dcg_pool_malloc (my_pool, size);
  memcpy (nptr, optr, osize);

  /* detach old, set pointer to new */
  dcg_pool_detach (my_pool, ptr);
  *ptr = nptr;
}


void dcg_pool_recalloc (pool my_pool, void **ptr, size_t nr, size_t size)
{ dcg_pool_realloc (my_pool, ptr, nr * size);         /* as you might expect */
}

/*
   dcg_attach increments the reference count of the argument
*/
void *dcg_attach (void *ptr)
{ int32 *ref_ptr = (int32 *) ptr;		/* gain access to ref count */
  if (ptr == NULL)				/* check argument */
    dcg_abort ("dcg_attach", "trying to attach a non existing object");
  if (ref_ptr[-2] < 0) return (ptr);		/* constant, return pointer */
  if (ref_ptr[-2] == 0)
    dcg_abort ("dcg_attach", "trying to attach a free object");
  ref_ptr[-2] += 1;				/* increment ref count */
  return (ptr);					/* return pointer */
}

/*
   dcg_predetach tries to decrement the reference count of the indirect
   argument while nulling the pointer the argument is pointing to.
   If the reference count would drop to 0, it returns the pointer
   the object, signifying that its subfields should be detached
   before detaching the actual object. It returns NULL otherwise.
*/
void *dcg_predetach (void **ptr)
{ void *optr = *ptr;				/* pick up ptr to object */
  int32 *ref_ptr = (int32 *) optr;		/* gain access to ref count */

  /* lose reference */
  if (optr == NULL) return (NULL);		/* uninitialized object */
  *ptr = NULL;					/* lose reference */

  /* ref count < 0 */
  if (ref_ptr[-2] < 0) return (NULL);		/* is it a constant, done */
  if (ref_ptr[-2] == 0)
    dcg_abort ("dcg_predetach", "trying to predetach a free object");

  /* ref count > 1 */
  if (ref_ptr[-2] > 1)				/* detach is safe */
    { ref_ptr[-2] -= 1;				/* decrement ref count */
      return (NULL);				/* also done */
    };

  /* signal the caller to get rid of the subfield(s) before detaching */
  return (optr);
}

/*
   dcg_cknonshared verifies that its argument is a valid (non constant) object
   and that it is not shared (i.e. its refcount equals 1). dcg_cknonshared is
   intended to be called by list modification routines.
*/
void dcg_cknonshared (void *ptr)
{ int32 refc;
  if (ptr == NULL)
    dcg_abort ("dcg_cknonshared", "Illegal modification of non existing object");
  refc = ((int32 *) ptr)[-2];
  if (refc < 0)
    dcg_abort ("dcg_cknonshared", "Illegal modification of constant object");
  if (!refc)
    dcg_abort ("dcg_cknonshared", "Illegal modification of a free object");
  if (refc > 1)
    dcg_abort ("dcg_cknonshared", "Illegal modification of shared object");
}

/*
   dcg_mkconstant sets the reference count of an existing object to -1
   i.e. it sets the life time of the object to infinite
*/

void dcg_mkconstant (void *ptr)
{ if (ptr == NULL)
    dcg_abort ("dcg_mkconstant", "Illegal constantification of non existing object");
  ((int32 *) ptr)[-2] = -1;
  /* optional, push ptr to object on a local string list */
}

/*
   dcg_check_not_freed checks that its argument has a ref count != 0
*/
void dcg_check_not_freed (void *ptr)
{ int32 refc;
  if (ptr == NULL) return;	/* Null pointer is allright */
  refc = ((int32 *) ptr)[-2];
  if (!refc)
    dcg_error (0, "Found object that has been freed whilst it should not have been");
}
