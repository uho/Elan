/*
   File: dcg_alloc.h
   Provides basic allocation routines and garbage collection
   while lessening the overhead caused by malloc and free.
   Note that an initial call to init_alloc () is required.

   Copyright (C) 2008 Marc Seutter

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

   CVS ID: "$Id: dcg_alloc.h,v 1.14 2012/12/22 16:19:44 marcs Exp $"
*/
#ifndef IncDcgAlloc
#define IncDcgAlloc

/* Standard includes */
#include <stdlib.h>

/* Local includes */
#include <dcg.h>

/* Raw allocation */
void *dcg_raw_malloc (size_t size);
void dcg_raw_free (void *ptr);

/* Pooled allocation: a pool is an opaque struct ref */
typedef struct dcg_pool_rec *pool;

/* Basic pooled allocation */
pool dcg_pool_init (char *name, size_t block_size);
void dcg_pool_release (pool old_pool);

void *dcg_pool_malloc (pool my_pool, size_t size);
void dcg_pool_detach (pool my_pool, void **ptr);
void *dcg_pool_unmanaged_malloc (pool my_pool, size_t size);
char *dcg_pool_unaligned_malloc (pool my_pool, size_t size);
void dcg_pool_stats (pool my_pool);

/*
   Derived pooled allocation: note that dcg_pool_realloc and dcg_pool_recalloc
   have a calling convention that differs from the usual realloc
*/
void *dcg_pool_calloc (pool my_pool, size_t nr, size_t size);
void dcg_pool_realloc (pool my_pool, void **ptr, size_t size);
void dcg_pool_recalloc (pool my_pool, void **ptr, size_t nr, size_t size);

/* Among the pools, one pool is more equal than the others */
extern pool my_default_pool;
#define dcg_init_alloc(name, size) (void)dcg_pool_init(name, size)
#define dcg_finish_alloc() dcg_pool_release(my_default_pool)
#define dcg_report_alloc() dcg_pool_stats(my_default_pool)

/* Pooled allocation using the default pool */
#define dcg_malloc(size) 		dcg_pool_malloc (my_default_pool, size)
#define dcg_detach(ptr) 		dcg_pool_detach (my_default_pool, ptr)
#define dcg_unmanaged_malloc(size)	dcg_pool_unmanaged_malloc (my_default_pool, size)		
#define dcg_unaligned_malloc(size)	dcg_pool_unaligned_malloc (my_default_pool, size)		

#define dcg_calloc(nr, size) 		dcg_pool_calloc (my_default_pool, nr, size)
#define dcg_realloc(ptr, size) 		dcg_pool_realloc (my_default_pool, ptr, size)
#define dcg_recalloc(ptr, nr, size)	dcg_pool_recalloc (my_default_pool, ptr, nr, size)

/* Pool independent memory management operations */
void *dcg_attach (void *ptr);
void *dcg_predetach (void **ptr);
void dcg_cknonshared (void *ptr);
void dcg_mkconstant (void *ptr);

/* Convenience macros */
#define nonrec_pool_detach(pool, ptr)	dcg_pool_detach (pool, (void **) ptr)
#define nonrec_detach(ptr)		dcg_pool_detach (my_default_pool, (void **) ptr)

/* To check freed parts */
void dcg_check_not_freed (void *ptr);

#endif /* IncDcgAlloc */
