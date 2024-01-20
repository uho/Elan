/*
   File: dcg.h
   Defines some global definitions

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

   CVS ID: "$Id: dcg.h,v 1.23 2012/12/22 16:19:44 marcs Exp $"
*/
#ifndef IncDcg
#define IncDcg

/*
   Define the 64 bit ints and constants
*/
#if defined(WIN32) && !defined(__MINGW32__)

/* WIN32 with MS Visual C */
typedef __int32 int32;
typedef __int64 int64;
typedef unsigned int u_int;
typedef unsigned __int32 u_int32;
typedef unsigned __int64 u_int64;
#define int64_const(x) (x##i64)
#define u_int64_const(x) (x##ui64)

/* Includes for MS Visual C */
#include <limits.h>
#include <sys/types.h>
#include <windows.h>
#include <io.h>

/* Define macros to indicate what we can include */
#define STDC_HEADERS
#define HAVE_MALLOC_H
#define HAVE_STRING_H
#define DIR_SEP '\\'
#define DIR_SEP_STR "/"
#define MAXPATHLEN 260
#define strcasecmp stricmp

#else

/* Unix with gcc */
#include <sys/types.h>
#include <sys/param.h>
#if defined(__STRICT_ANSI__) || defined(__MINGW32__)
/* usually present in sys/types.h */
typedef unsigned int u_int;
#endif /* defined(__STRICT_ANSI__) || defined(__MINGW32__) */

#if HAVE_STDINT_H
#include <stdint.h>
typedef int32_t int32;
typedef int64_t int64;
typedef uint32_t u_int32;
typedef uint64_t u_int64;
#else /* Usual gcc conventions */
typedef int int32;
typedef long long int int64;
typedef unsigned int u_int32;
typedef unsigned long long int u_int64;
#endif /* HAVE_STDINT_H */

#define int64_const(x) (x##LL)
#define u_int64_const(x) (x##ULL)
#define DIR_SEP '/'
#define DIR_SEP_STR "/"

#endif /* defined(WIN32) && !defined(__MINGW32__) */

/* Introduce other types */
typedef double real;
typedef void *vptr;

#define int_nil 0
#define u_int_nil 0
#define int32_nil 0
#define int64_nil int64_const(0)
#define u_int32_nil 0
#define u_int64_nil u_int64_const(0)
#define real_nil 0.0
#define char_nil '\0'
#define vptr_nil ((vptr) NULL)

#define detach_int(old) 
#define detach_u_int(old)
#define detach_int32(old) 
#define detach_u_int32(old)
#define detach_int64(old) 
#define detach_u_int64(old)
#define detach_real(old) 
#define detach_char(old) 
#define detach_vptr(old)

#define rdup_int(old)  (old)
#define rdup_u_int(old) (old)
#define rdup_int32(old)  (old)
#define rdup_u_int32(old) (old)
#define rdup_int64(old)  (old)
#define rdup_u_int64(old) (old)
#define rdup_real(old)  (old)
#define rdup_char(old)  (old)
#define rdup_vptr(old) (old)

#define cmp_int(a,b)     (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_u_int(a,b)   (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_int32(a,b)   (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_u_int32(a,b) (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_int64(a,b)   (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_u_int64(a,b) (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_real(a,b)    (((a)<(b))?-1:((a)>(b))?1:0)
#define cmp_char(a,b)    (((a)<(b))?-1:((a)>(b))?1:0)

#define equal_int(a,b)     ((a) == (b))
#define equal_u_int(a,b)   ((a) == (b))
#define equal_int32(a,b)   ((a) == (b))
#define equal_u_int32(a,b) ((a) == (b))
#define equal_int64(a,b)   ((a) == (b))
#define equal_u_int64(a,b) ((a) == (b))
#define equal_real(a,b)    ((a) == (b))
#define equal_char(a,b)    ((a) == (b))
#define equal_vptr(a,b)    ((a) == (b))

#define check_not_freed_int(a)
#define check_not_freed_u_int(a)
#define check_not_freed_int32(a)
#define check_not_freed_u_int32(a)
#define check_not_freed_int64(a)
#define check_not_freed_u_int64(a)
#define check_not_freed_real(a)
#define check_not_freed_char(a)
#define check_not_freed_vptr(a) dcg_check_not_freed(vptr)

#endif /* IncDcg */
