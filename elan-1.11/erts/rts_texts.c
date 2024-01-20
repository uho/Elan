/*
   File: rts_texts.c
   Provides text engine

   CVS ID: "$Id: rts_texts.c,v 1.7 2005/04/06 18:55:28 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_texts.h"

/* Support routine to create new texts */
char *rts_new_text (char *a)
	{ char *new;
	  if (a == NULL)
	     rts_error ("rts_new_text called with NULL argument");
	  new = rts_malloc (strlen (a) + 1);
	  strcpy (new, a);
	  return (new);
	};

/* TEXT PROC ascii (INT a) */
char *rts_ascii_int (int nr)
	{ char *new;
	  if ((nr < 0) || (nr > 255))
	     rts_error ("TEXT PROC ascii (INT a) called with value %d", nr);
	  new = rts_malloc (2);
	  new[0] = (char) (nr & 0377);
	  new[1] = '\0';
	  return (new);
	};

/* INT PROC ascii (TEXT a) */
int rts_ascii_text (char *a)
	{ if (a == NULL)
	     rts_error ("INT PROC ascii (TEXT a) called with uninitialized value");
	  return (*a);
	};

/* TEXT OP + (TEXT a,b) */
char *rts_text_plus_text (char *a, char *b)
	{ char *new;
	  char *dptr;
	  char *sptr;
	  if ((a == NULL) || (b == NULL))
	     rts_error ("TEXT OP + (TEXT a,b) called with uninitialized value");
	  new = rts_malloc (strlen (a) + strlen (b) + 1);	/* include '\0' */
	  for (dptr = new, sptr = a; *sptr; dptr++, sptr++) *dptr = *sptr;
	  for (sptr = b; *sptr; dptr++, sptr++) *dptr = *sptr;
	  *dptr = '\0';
	  return (new);
	};

/* TEXT OP * (INT n, TEXT t) */
char *rts_int_times_text (int a, char *b)
	{ char *new;
	  char *dptr;
	  char *sptr;
	  int ix;
	  if (b == NULL)
	     rts_error ("TEXT OP * (INT i, TEXT t) called with uninitialized value");
	  if (a < 0) rts_error ("TEXT OP * (INT i, TEXT t) called with negative multiplicand");
	  new = rts_malloc (a * strlen (b) + 1);
	  for (ix = 0, dptr = new; ix < a; ix++)
	     for (sptr = b; *sptr; dptr++, sptr++) *dptr = *sptr;
	  *dptr = '\0';
	  return (new);
	};

/* INT PROC length (TEXT t) */
/* INT OP len (TEXT t) */
int rts_length_text (char *a)
	{ if (a == NULL) rts_error
		("PROC length (TEXT a) called with uninitialized value");
	  return (strlen (a));
	};

/* TEXT OP SUB (TEXT a, INT b) */
char *rts_text_sub_int (char *a, int idx)
	{ char *new;
	  int len;
	  if (a == NULL) rts_error
		("OP SUB (TEXT a, INT b) called with uninitialized value");
	  len = strlen (a);
	  if ((idx < 1) || (idx > len))
	     { new = rts_malloc (1);
	       new[0] = '\0';
	     }
	  else
	     { new = rts_malloc (2);
	       new[0] = a[idx - 1];		/* Stupid Elan convention */
	       new[1] = '\0';
	     };
	  return (new);
	};

/* INT PROC pos (TEXT t, TEXT pat) */
int rts_pos_text_text (char *a, char *b)
	{ char *res;
	  if ((a == NULL) || (b == NULL))
	     rts_error ("PROC pos (TEXT t,pat) called with uninitialized value");
	  res = strstr (a,b);
	  if (res == NULL) return (0);
	  return (res - a + 1);
	};

/* TEXT PROC subtext (TEXT a, INT from, to) */
char *rts_subtext_text_int_int (char *a, int from, int to)
	{ char *new;
	  int afrom, ato, ix;
	  int len;
	  if (a == NULL) rts_error
	       ("subtext (TEXT a, INT from, to) called with unitialized value");
	  len = strlen (a);
	  afrom = 1;
	  ato = len;
	  if (from > 1) afrom = from;
	  if (to < len) ato = to;
	  if (ato < afrom)
	     { new = rts_malloc (1);
	       new[0] = '\0';
	     }
	  else /* Stupid Elan convention */
	     { new = rts_malloc (ato - afrom + 2);
	       for (ix = 0; ix < ato - afrom + 2; ix++)
		  new[ix] = a[afrom + ix - 1];
	       new [ato - afrom + 1] = '\0';
	     };
	  return (new);
	};

/* BOOL OP =  (TEXT a,b) */
int rts_text_equal_text (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("OP = (TEXT a,b) called with uninitialized value");
	  if (strcmp (a,b) == 0) return (1);
	  return (0);
	};

/* BOOL OP <> (TEXT a,b) */
int rts_text_notequal_text (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("OP <> (TEXT a,b) called with uninitialized value");
	  if (strcmp (a,b) != 0) return (1);
	  return (0);
	};

/* BOOL OP <  (TEXT a,b) */
int rts_text_less_than_text (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("OP < (TEXT a,b) called with uninitialized value");
	  if (strcmp (a,b) < 0) return (1);
	  return (0);
	};

/* BOOL OP <= (TEXT a,b) */
int rts_text_less_equal_text (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("OP <= (TEXT a,b) called with uninitialized value");
	  if (strcmp (a,b) <= 0) return (1);
	  return (0);
	};

/* BOOL OP >  (TEXT a,b) */
int rts_text_greater_than_text (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("OP > (TEXT a,b) called with uninitialized value");
	  if (strcmp (a,b) > 0) return (1);
	  return (0);
	};

/* BOOL OP >= (TEXT a,b) */
int rts_text_greater_equal_text (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("OP >= (TEXT a,b) called with uninitialized value");
	  if (strcmp (a,b) >= 0) return (1);
	  return (0);
	};

/* Text comparison */
int rts_text_compare (char *a, char *b)
	{ if ((a == NULL) || (b == NULL))
	     rts_error ("Text comparison operator called with uninitialized value");
	  return (strcmp (a,b));
	};
