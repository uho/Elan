/*
   File: warshall.c
   Implements Warshalls algorithm
  
   CVS ID: "$Id: warshall.c,v 1.3 2005/02/25 20:08:37 marcs Exp $"
*/

/* general includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_alloc.h>

/* local includes */
#include "warshall.h"

char *warshall (int n, char *rel)
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
		   result[i * n + j] = result[i * n + j] ||
			(result[i * n + k] && result [k * n + j]);

	  return (result);
	};
