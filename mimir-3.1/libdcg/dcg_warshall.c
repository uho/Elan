/*
   File: dcg_warshall.c
   Implements Warshalls algorithm
   Added to the library as we use it too often in our compilers
  
   Copyright (C) 2008-2011 Marc Seutter

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

   CVS ID: "$Id: dcg_warshall.c,v 1.1 2011/09/03 19:38:33 marcs Exp $"
*/

/* general includes */
#include <stdio.h>

/* local includes */
#include "dcg.h"
#include "dcg_alloc.h"
#include "dcg_warshall.h"

char *dcg_warshall (int n, char *rel)
{ /* Allocate array for result relation */
  char *result = (char *) dcg_calloc (n*n, sizeof (char));
  int i,j,k;

  /* Copy the original relation into the result */
  for (i=0; i<n; i++)
    for (j=0; j<n; j++) result[i * n + j] = rel[i * n + j];

  /* Take closure */
  for (k=0; k<n; k++)
    for (i=0; i<n; i++)
      for (j=0; j<n; j++)
	result[i * n + j] = result[i * n + j] || (result[i * n + k] && result [k * n + j]);

  return (result);
}
