/*
   File: dcg_dstring.h
   Handles dynamic strings

   Copyright 2009-2012 Radboud University of Nijmegen

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_dstring.h,v 1.4 2012/12/27 16:45:40 marcs Exp $"
*/
#ifndef IncDcgDstring
#define IncDcgDstring

/* Local includes */
#include <dcg.h>
#include <dcg_alloc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* A dynamic string is an opaque struct ref */
typedef struct dcg_dstring_rec *dstring;

/* Exported routines */
dstring dcg_pool_init_dstring	(pool my_pool, size_t room);
char *dcg_pool_finish_dstring	(pool my_pool, dstring *ds);
void dcg_pool_free_dstring	(pool my_pool, dstring *ds);
void dcg_pool_append_dstring_c	(pool my_pool, dstring ds, char ch);
void dcg_pool_append_dstring_n	(pool my_pool, dstring ds, const char *str, size_t len2);
void dcg_pool_append_dstring	(pool my_pool, dstring ds, const char *str);
void dcg_pool_sprintfa_dstring	(pool my_pool, dstring ds, char *format, ...);

/* Shorthands using the default pool */
#define dcg_init_dstring(room)		 dcg_pool_init_dstring (my_default_pool, room)
#define dcg_free_dstring(ds)		 dcg_pool_free_dstring (my_default_pool, ds)
#define dcg_append_dstring_c(ds,ch)	 dcg_pool_append_dstring_c (my_default_pool, ds, ch)
#define dcg_append_dstring_n(ds,str,len) dcg_pool_append_dstring_n (my_default_pool, ds, str, len)
#define dcg_append_dstring(ds,str)	 dcg_pool_append_dstring (my_default_pool, ds, str)
#define dcg_finish_dstring(ds)		 dcg_pool_finish_dstring (my_default_pool, ds)

/* One specialized for the default pool */
void dcg_sprintfa_dstring (dstring ds, char *format, ...);

/* To output dynamic strings */
void dcg_fprint_dstring (FILE *out, dstring ds);

#ifdef __cplusplus
}
#endif
#endif /* IncDcgDstring */
