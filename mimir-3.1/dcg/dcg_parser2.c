/*
   File: dcg_parser2.c
   Parses all files indicated by the argument string

   Copyright (C) 2008 Marc Seutter

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

   CVS ID: "$Id: dcg_parser2.c,v 1.11 2011/08/13 12:57:03 marcs Exp $"
*/

/* standard includes */
#include <stdio.h>
#include <string.h>

/* Conditional inclusion to define MAXPATHLEN */
#ifndef WIN32
#include <sys/param.h>
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>

/* local includes */
#include "dcg_code.h"
#include "dcg_lex.h"
#include "dcg_parser2.h"

static FILE *try_open (char *basename)
{ FILE *new;
  char fname[MAXPATHLEN + 1];
  sprintf (fname, "%s.dcg", basename);
  if ((new = fopen (fname, "r")))
     dcg_hint ("parsing %s...", fname);
  else
     { sprintf (fname, "%s", basename);
       if ((new = fopen (fname, "r")))       
	  dcg_hint ("parsing %s...", fname);
       else dcg_panic ("could not open %s", fname);
     };
  return (new);
}

static void parse_imported_definitions (string iname, string_list include_path)
{ FILE *new;
  char fname[MAXPATHLEN + 1];
  int ix;
  for (ix = 0; ix < include_path -> size; ix++)
    { sprintf (fname, "%s/%s.def", include_path -> array[ix], iname);
      if ((new = fopen (fname, "r")))
        { dcg_hint ("parsing definitions from %s.def...", iname);
	  init_lex (new);
	  pdcg_parse ();
	  fclose (new);
	  return;
	};
    };
  dcg_panic ("could not locate %s[.def]", iname);
}

#define STDDEF "dcg"
void parse (char *basename, string_list include_path)
{ FILE *f = try_open (basename);
  int ix;
  imp_types = new_type_list ();
  all_defs = new_def_list ();
  all_stats = new_stat_list ();
  init_lex (f);
  pdcg_parse ();
  fclose (f);
  for (ix = 0; ix < all_stats -> size; ix++)
    { stat s = all_stats -> array[ix];
      if (s -> tag == TAGImport)
        parse_imported_definitions (s -> Import.imp, include_path);
    };
  parse_imported_definitions (STDDEF, include_path);
}
