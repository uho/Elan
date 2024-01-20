/*
   File: rts_error.h
   Defines abnormal exits from running Elan programs

   CVS ID: "$Id: rts_error.c,v 1.3 2005/02/10 12:53:06 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* local includes */
#include "rts_term.h"

/* temporarily until it gets initialized by init_rts () */
int debug = 0;

#define MAXERRLEN 1024
void rts_panic (char *format, ...)
	{ char buf[MAXERRLEN + 1];
	  va_list arg_ptr;
	  va_start (arg_ptr, format);
	  vsprintf (buf, format, arg_ptr);
	  va_end (arg_ptr);
	  rts_finish_termio ();
	  fprintf (stderr, "\nRts panic: %s\n", buf);
	  exit (4);
	};

void rts_error (char *format, ...)
	{ char buf[MAXERRLEN + 1];
	  va_list arg_ptr;
	  va_start (arg_ptr, format);
	  vsprintf (buf, format, arg_ptr);
	  va_end (arg_ptr);
	  rts_finish_termio ();
	  fprintf (stderr, "\nRts error: %s\n", buf);
	  exit (4);
	};

void rts_warning (char *format, ...)
	{ char buf[MAXERRLEN + 1];
	  va_list arg_ptr;
	  va_start (arg_ptr, format);
	  vsprintf (buf, format, arg_ptr);
	  va_end (arg_ptr);
	  rts_put_text ("\nRts warning: ");
	  rts_put_text (buf);
	  rts_put_text ("\n");
	};

void rts_log (char *format, ...)
	{ char buf[MAXERRLEN + 1];
	  va_list arg_ptr;
	  va_start (arg_ptr, format);
	  vsprintf (buf, format, arg_ptr);
	  va_end (arg_ptr);
	  rts_put_text (buf);
	  rts_put_text ("\n");
	};

/* Special exits to be called from generated code */
void rts_index_out_of_range ()
	{ rts_error ("Row index out of range");
	};

void rts_offset_from_0 ()
	{ rts_error ("Accessing the internals of an uninitialized object");
	};
