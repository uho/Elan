/*
   File: dcg_arith.h
   Defines some arithmetic support

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

   CVS ID: "$Id: dcg_arith.h,v 1.9 2011/08/13 16:01:48 marcs Exp $"
*/
#ifndef IncDcgArith
#define IncDcgArith

/* include math */
#include <math.h>

/* exclude min and max for WIN32 */
#ifndef WIN32
#define max(a,b) (((a)<(b))?(b):(a))
#define min(a,b) (((a)>(b))?(b):(a))
#endif

#define odd(a) ((a)&1)
#define even(a) (!((a)&1))
#define iabs(a) (((a)<0)?(-(a)):(a))
#define rabs(a) (((a)<0.0)?(-(a)):(a))

#define power10(x) pow(10.0, ((real) x))
#define ipower2(a) (1<<(a))
#define ipower10(a) (dcg_ipower(10,a));

int dcg_ilog2 (int nr);
int dcg_ilog10 (int nr);
int dcg_ipower (int nr, int exp);
int dcg_imod (int l, int r);
int dcg_irem (int l, int r);

#endif /* IncDcgArith */
