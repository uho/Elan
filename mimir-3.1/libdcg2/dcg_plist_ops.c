/*
   File: dcg_plist_ops.c
   Defines some general operations we sometime need for these particular types.

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

   CVS ID: "$Id: dcg_plist_ops.c,v 1.9 2011/09/03 19:38:36 marcs Exp $"
*/

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "dcg_plist.h"
#include "dcg_plist_ops.h"

/* Check if arg is on int_list */
int is_on_int_list (int_list il, int i)
{ int ix;
  if (il == int_list_nil)
    dcg_internal_error ("is_on_int_list");
  for (ix = 0; ix < il -> size; ix++)
    if (il -> array[ix] == i) return (1);
  return (0);
};

/* Check if arg is on string_list */
int is_on_string_list (string_list sl, string s)
{ int ix;
  if (sl == string_list_nil)
    dcg_internal_error ("is_on_string_list");
  for (ix = 0; ix < sl -> size; ix++)
    if (equal_string (sl -> array[ix], s)) return (1);
  return (0);
};

/* Add int to int_list if not already present */
void add_uniquely_to_int_list (int_list il, int i)
{ if (il == int_list_nil)
    dcg_internal_error ("add_uniquely_to_int_list");
  if (is_on_int_list (il, i)) return;
  app_int_list (il, i);
};

/* Add string to string_list if not already present and increment its refcount */
void add_uniquely_to_string_list (string_list sl, string s)
{ if ((sl == string_list_nil) || (s == string_nil))
    dcg_internal_error ("add_uniquely_to_string_list");
  if (is_on_string_list (sl, s)) return;
  app_string_list (sl, attach_string (s));
};
