/*
   File: dcg_string.h
   Defines string operations

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

   CVS ID: "$Id: dcg_string.h,v 1.16 2012/12/22 16:19:44 marcs Exp $"
*/
#ifndef IncDcgString
#define IncDcgString

/* include string for ops */
#include <string.h>

/* Local includes */
#include <dcg.h>
#include <dcg_alloc.h>

/* Introduce type string (to become text) */
#define MAXSTRLEN 65536
typedef char *string;
#define string_nil ((string) NULL)
#define streq(s1,s2) (strcmp((s1),(s2)) == 0)
#define strlt(s1,s2) (strcmp((s1),(s2)) < 0)
#define streq_nocase(s1,s2) (strcasecmp((s1),(s2)) == 0)

/* Introduce attach/detach ops */
#define attach_string(t) dcg_attach(t)
#define att_string(t) (void) dcg_attach(t)
#define rdup_string(t) dcg_attach(t)
#define detach_string(t) dcg_detach((void **) t)
#define check_not_freed_string(t) dcg_check_not_freed(t)

/* Introduce support ops */
string dcg_pool_new_string (pool my_pool, char *t);
string dcg_pool_new_unaligned_string (pool my_pool, char *t);
string dcg_pool_new_fmtd_string (pool my_pool, char *format, ...);
string dcg_pool_concat_string (pool my_pool, string t1, string t2);
string dcg_new_fmtd_string (char *format, ...);

/* Introduce shorthands through macros */
#define new_string(t) dcg_pool_new_string (my_default_pool, t)
#define new_unaligned_string(t) dcg_pool_new_unaligned_string (my_default_pool, t)
#define cmp_string(t1,t2) strcmp((t1),(t2))
#define equal_string(t1,t2) streq(t1,t2)

#endif /* IncDcgString */
