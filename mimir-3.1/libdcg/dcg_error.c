/*
   File: dcg_error.c
   Defines routines for diagnostic messages

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

   CVS ID: "$Id: dcg_error.c,v 1.11 2012/01/11 15:43:01 marcs Exp $"
*/

/* standard includes */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

/* local includes */
#include "dcg.h"
#include "dcg_error.h"

/* Public variables to set from command line parsing */
int debug = 0;
int verbose = 0;	/* Can be combined into a verbosity level */
int full_verbose = 0;

/* Note: these variables must be initialized before using any dcg code */
static FILE* error_file;
static char *tool_name;
static int error_count;
static void (*my_onexit)();

void dcg_init_error (FILE *f, char *tname, void (*onexit)())
{ verbose = 0;
  full_verbose = 0;
  error_file = f;
  error_count = 0;
  tool_name = tname;
  my_onexit = onexit;
}

void dcg_exit (int error)
{ exit (error);
}

void dcg_panic (char *format, ...)
{ va_list arg_ptr;

  fprintf (error_file, "Fatal error: ");
  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\n%s aborting\n", tool_name);
  if (my_onexit != NULL) my_onexit ();
  dcg_exit (4);
}

void dcg_abort (char *location, char *format, ...)
{ va_list arg_ptr;

  fprintf (error_file, "Abort in function %s of %s: ", location, tool_name);
  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\n");
  if (my_onexit != NULL) my_onexit ();
  dcg_exit (1);
}

void dcg_bug (char *location, char *format, ...)
{ va_list arg_ptr;

  fprintf (error_file, "Abort in function %s of %s: ", location, tool_name);
  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\nPlease contact marcs@cs.ru.nl\n");
  if (my_onexit != NULL) my_onexit ();
  dcg_exit (1);
}

void dcg_error (int prefix, char *format, ...)
{ va_list arg_ptr;

  if (prefix)
    fprintf (error_file, "Error: ");

  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\n");
  fflush (error_file);
  error_count ++;
}

void dcg_warning (int prefix, char *format, ...)
{ va_list arg_ptr;

  if (!verbose)
    return;

  if (prefix)
    fprintf (error_file, "Warning: ");

  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\n");
  fflush (error_file);
}

void dcg_hint (char *format, ...)
{ va_list arg_ptr;

  if (!full_verbose)
    return;

  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\n");
  fflush (error_file);
}

void dcg_wlog (char *format, ...)
{ va_list arg_ptr;

  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);

  fprintf (error_file, "\n");
  fflush (error_file);
}

void dcg_eprint (char *format, ...)
{ va_list arg_ptr;

  va_start (arg_ptr, format);
  vfprintf (error_file, format, arg_ptr);
  va_end (arg_ptr);
  fflush (error_file);
}

FILE *dcg_error_file ()
{ return (error_file);
}

void dcg_panic_if_errors ()
{ if (!error_count) return;
  dcg_panic ("can not continue due to errors");
}

void dcg_bad_tag (int tag, char *routine)
{ dcg_panic ("bad tag %d occurred in function %s of %s", tag, routine, tool_name);
}

void dcg_internal_error (char *routine)
{ dcg_panic ("internal consistency check failed in function %s of %s", routine, tool_name);
}
