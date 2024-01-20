/*
   File: dcg_dstring.c
   Handles dynamic strings needed in the runtime system

   Copyright 2009-2011 Radboud University of Nijmegen

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

   CVS ID: "$Id: "
*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* local includes */
#include "dcg.h"
#include "dcg_error.h"
#include "dcg_alloc.h"
#include "dcg_string.h"
#include "dcg_dstring.h"

/* A dynamic string is an opaque struct ref */
struct dcg_dstring_rec
{ size_t room;
  size_t len;
  char *str;
};

/*
   Initialize and free a dynamic string
*/
dstring dcg_pool_init_dstring (pool my_pool, size_t room)
{ dstring ds = (dstring) dcg_pool_malloc (my_pool, sizeof (struct dcg_dstring_rec));
  if (room < 8L) room = 8L;		/* smallest allocation */
  ds -> room = room;
  ds -> len = 0;
  ds -> str = (char *) dcg_pool_malloc (my_pool, room);
  ds -> str[0] = '\0';
  return (ds);
}

void dcg_pool_free_dstring (pool my_pool, dstring *ds)
{ if (ds == NULL) return;
  dcg_pool_detach (my_pool, (void **) &((*ds) -> str));
  dcg_pool_detach (my_pool, (void **) ds);
}

/*
   Deallocate the dynamic string and return its contents
*/
char *dcg_pool_finish_dstring (pool my_pool, dstring *ds)
{ /* Check validity of dynamic string */
  char *new_str;
  if (ds == NULL)
    dcg_abort ("dcg_pool_finish_dstring", "called with NULL arg");

  /* Copy to new string and release */
  new_str = dcg_pool_new_string (my_pool, (*ds) -> str);
  dcg_pool_free_dstring (my_pool, ds);
  return (new_str);
}

/*
   Character append
*/
void dcg_pool_append_dstring_c (pool my_pool, dstring ds, char ch)
{ /* Check validity of dynamic string */
  if (ds == NULL)
    dcg_abort ("abs_append_dstring_c", "called with NULL arg");

  /* Check if the character would fit */
  if (ds -> len + 1 == ds -> room)
    { /* Double the amount of space for every realloc */
      size_t new_room = ds -> room * 2;
      dcg_pool_realloc (my_pool, (void **) &ds -> str, new_room);
      ds -> room = new_room;
    };

  /* Add the character */
  ds -> str[ds -> len++] = ch;
  ds -> str[ds -> len] = '\0';
}

/*
   Dynamic string concatenation
*/
void dcg_pool_append_dstring_n (pool my_pool, dstring ds, const char *str, size_t len)
{ /* Check validity of dynamic string */
  size_t old_len, new_len;
  if ((ds == NULL) || (str == NULL))
    dcg_abort ("dcg_append_dstring_n", "called with NULL args");

  /* Check if the space would fit */
  old_len = ds -> len;
  new_len = old_len + len + 1;
  if (new_len >= ds -> room)
    { size_t new_room = 2 * (new_len);
      dcg_pool_realloc (my_pool, (void **) &ds -> str, new_room);
      ds -> room = new_room;
    };

  /* Copy the string at the end */
  strncpy (ds -> str + old_len, str, len);
  ds -> len = old_len + len;
}

void dcg_pool_append_dstring (pool my_pool, dstring ds, const char *str)
{ size_t len = strlen (str);
  dcg_pool_append_dstring_n (my_pool, ds, str, len);
}

/*
   Dynamic string concatenation with a var args
*/
#define MAX_FMT_LEN 65536
void dcg_pool_sprintfa_dstring (pool my_pool, dstring ds, char *format, ...)
{ char buf[MAX_FMT_LEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsnprintf (buf, MAX_FMT_LEN, format, arg_ptr);
  va_end (arg_ptr);
  dcg_pool_append_dstring (my_pool, ds, buf);
}

void dcg_sprintfa_dstring (dstring ds, char *format, ...)
{ char buf[MAX_FMT_LEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsnprintf (buf, MAX_FMT_LEN, format, arg_ptr);
  va_end (arg_ptr);
  dcg_pool_append_dstring (my_default_pool, ds, buf);
}

/*
   Outputting dynamic strings
*/
void dcg_fprint_dstring (FILE *out, dstring ds)
{ fputs (ds -> str, out);
}
