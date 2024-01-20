/*
   File: dcg_arith.c
   Defines arithmetic support

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

   CVS ID: "$Id: dcg_arith.c,v 1.8 2011/08/13 16:01:48 marcs Exp $"
*/

/* local includes */
#include "dcg.h"
#include "dcg_arith.h"

int dcg_ilog2 (int nr)
{ int i = 0;
  while ((1<<i) < nr) i++;
  return (i);
}

int dcg_ilog10 (int n)
{ int i, z = 1;
  for (i=0; z<n; i++) z *= 10;
  return (i);
}

int dcg_ipower (int nr, int exp)
{ int z = 1, b0 = nr, n0 = exp;
  while (n0!=0)
    { if (odd (n0)) z *= b0;
      n0 >>= 1;
      b0 *= b0;
    };
  return (z);
}

int dcg_imod (int l, int r)
{ int q = l / r;
  if (q < 0) q--;
  return (l - q * r);
}

int dcg_irem (int l, int r)
{ int q = l / r;
  return (l - q * r);
}
