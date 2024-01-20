/*
   File: rts_test.c
   Tests the elan runtime system

   CVS ID: "$Id: rts_test.c,v 1.3 2005/02/10 12:53:07 marcs Exp $"
*/

/* global includes */
#include <stdio.h>

/* local include */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_texts.h"
#include "rts_ints.h"

int main (int argc, char **argv)
	{ rts_init (argc, argv);
	  fprintf (stderr, "(-7) ** 3 = %d\n", rts_int_pow_int (-7,3));
	  return (0);
	};
