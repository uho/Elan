/*
   File: rts_ints.c
   Provides integer engine.

   CVS ID: "$Id: rts_ints.c,v 1.4 2005/04/06 18:55:27 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* global includes */
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif

/* local includes */
#include "rts_error.h"
#include "rts_ints.h"

/* define INT_MAX if limits.h could not do that for us */
#ifndef INT_MAX
#define INT_MAX 0x7fffffff
#endif

/* INT PROC maxint */
int rts_maxint ()
	{ return (INT_MAX);
	};

/* Unoptimized INT OP ** (INT a, b) */
int rts_int_pow_int (int a, int b)
	{ int z = 1;
	  if (b < 0)
	     rts_error ("OP ** (INT a, b) called with negative exponent");
	  while (b > 0)
	     { if (b & 1) z = z * a;
	       a = a * a;
	       b = b >> 1;
	     };
	  return (z);
	};
