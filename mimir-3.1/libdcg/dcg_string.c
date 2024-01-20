/*
   File: dcg_string.c
   Defines text operations
   Optionally provides routines to share string space

   Copyright (C) 2008-2011 Marc Seutter

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

   CVS ID: "$Id: dcg_string.c,v 1.11 2011/09/02 09:58:00 marcs Exp $"
*/

/* standard includes */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* local includes */
#include "dcg.h"
#include "dcg_error.h"
#include "dcg_alloc.h"
#include "dcg_string.h"

string dcg_pool_new_string (pool my_pool, char *t)
{ string new_str;
  if (t == NULL)
    dcg_abort ("dcg_pool_new_string", "called with NULL argument");
  new_str = dcg_pool_malloc (my_pool, strlen (t) + 1);
  strcpy (new_str, t);
  return (new_str);
}

string dcg_pool_new_unaligned_string (pool my_pool, char *t)
{ string new_str;
  if (t == NULL)
    dcg_abort ("dcg_new_pool_unaligned_string", "called with NULL argument");
  new_str = dcg_pool_unaligned_malloc (my_pool, strlen (t) + 1);
  strcpy (new_str, t);
  return (new_str);
}

#define MAXSTRLEN 65536
string dcg_pool_new_fmtd_string (pool my_pool, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsnprintf (buf, MAXSTRLEN, format, arg_ptr);
  va_end (arg_ptr);
  return (dcg_pool_new_string (my_pool, buf));
}

string dcg_pool_concat_string (pool my_pool, string t1, string t2)
{ string new_str;
  if ((t1 == NULL) || (t2 == NULL))
    dcg_abort ("dcg_pool_concat_string", "called with NULL argument");
  new_str = dcg_pool_malloc (my_pool, strlen (t1) + strlen (t2) + 1);
  strcpy (new_str, t1);
  strcat (new_str, t2);
  return (new_str);
}

string dcg_new_fmtd_string (char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsnprintf (buf, MAXSTRLEN, format, arg_ptr);
  va_end (arg_ptr);
  return (dcg_pool_new_string (my_default_pool, buf));
}

