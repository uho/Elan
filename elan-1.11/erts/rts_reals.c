/*
   File: rts_reals.c
   Provides real engine

   CVS ID: "$Id: rts_reals.c,v 1.4 2005/04/06 18:55:28 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* global includes */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#ifdef HAVE_FLOAT_H
#include <float.h>
#endif

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_texts.h"
#include "rts_reals.h"

/*
   Define DBL_MAX and DBL_EPSILON according to IEEE754
   if float.h could not do that for us 
*/
#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623157E+308
#endif
#ifndef DBL_EPSILON
#define DBL_EPSILON 2.2204460492503131E-16
#endif

/* REAL PROC maxreal */
real rts_maxreal ()
	{ return (DBL_MAX);
	};

/* REAL PROC smallreal */
real rts_smallreal ()
	{ return (DBL_EPSILON);
	};

/* must still correct for rounding direction */
/* INT PROC trunc (REAL a) */
int rts_trunc_real_to_int (real a)
	{ return ((int) a);
	};

/* INT PROC round (REAL a) */
int rts_round_real_to_int (real a)
	{ return ((int) (a + 0.5));
	};

/* Unoptimized REAL OP ** (REAL a, INT b) */
real rts_real_pow_int (real a, int b)
	{ real z = 1;
	  if (b < 0) return (1.0/rts_real_pow_int (a,-b));
	  while (b > 0)
	     { if (b & 1) z = z * a;
	       a = a * a;
	       b = b >> 1;
	     };
	  return (z);
	};

/* TEXT PROC text (REAL r) */
char *rts_real_to_text (double r)
	{ char buf[64];
	  int len;
	  char *new;
	  sprintf (buf, "%e", r);
	  len = strlen (buf);
	  new = rts_malloc (len + 1);		/* include '\0' */
	  strcpy (new, buf);
	  return (new);
	};

/* REAL PROC real (TEXT a, BOOL VAR conv ok) */
real rts_text_to_real (char *text, int *ok)
	{ char *ptr = text;
	  real value = 0.0;
	  int exp = 0;

	  /* initially we fail */
	  *ok = 0;

	  /* check for valid text */
	  if (ptr == NULL)
	     rts_error ("REAL PROC text (TEXT a, BOOL VAR ok) called with uninitialized value");

	  /* skip leading spaces */
	  while (isspace (*ptr)) ptr++;

	  /* first non space must be a digit */
	  if (!isdigit (*ptr)) return (value);

	  /* convert digits */
	  while (isdigit (*ptr))
	     value = 10.0 * value + (real) ((*ptr++) - '0');

	  /* if decimal point, check for following digit and convert */
	  if ((*ptr) == '.')
	     { ptr++;
	       if (!isdigit (*ptr)) return (value);
	       while (isdigit (*ptr))
		  { value = 10.0 * value + (real) ((*ptr++) - '0');
		    exp--;
		  };
	     };

	  /* check for exponent */
	  if (((*ptr) == 'e') || ((*ptr) == 'E'))
	     { int sec_exp = 0;
	       int sec_sign = 0;
	       ptr++;

	       /* check for sign of exponent */
	       if ((*ptr) == '-') { ptr++; sec_sign = 1; }
	       else if ((*ptr) == '+') ptr++;

	       if (!isdigit (*ptr)) return (value);
	       while (isdigit (*ptr))
		  sec_exp = 10 * sec_exp + ((*ptr++) - '0');

	       if (sec_sign) exp -= sec_exp;
	       else exp += sec_exp;
	     };

	  /* at end of denotation only spaces are ok */
	  while (isspace (*ptr)) ptr++;

	  /* if at end of text, conversion is ok */
	  if (!(*ptr)) *ok = 1;
	  return (value * rts_real_pow_int (10.0, exp));
	};

