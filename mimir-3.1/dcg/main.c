/*
   File: main.c
   Defines the main program of 'dcg'

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

   CVS ID: "$Id: main.c,v 1.18 2011/09/05 19:55:33 marcs Exp $"
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
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_version.h>
#include <dcg_fileutil.h>

/* local includes */
#include "dcg_code.h"
#include "dcg_lex.h"
#include "dcg_parser2.h"
#include "coder.h"
#include "xform_coder.h"
#include "analyze.h"
#include "options.h"

/*
   Loading, saving and dumping internal datastructures
*/
#define dcg_tool "DCG"
#define dcg_kind "DataStructures"
static void try_load_data_structures (char *basename)
{ BinFile bf;
  char fname[MAXPATHLEN + 1];
  sprintf (fname, "%s.sav", basename);
  bf = dcg_bin_fopen (fname, "r", 1);
  dcg_bin_verify_header (bf, dcg_tool, dcg_kind, DCG_VERSION);
  load_def_list (bf, &all_defs);
  load_stat_list (bf, &all_stats);
  load_type_list (bf, &imp_types);
  dcg_bin_verify_eof (bf);
  dcg_bin_fclose (&bf);
}

static void dump_data_structures ()
{ fprintf (stderr, "Dump of internal data structures:\n\n");
  pp_def_list (stderr, all_defs);
  pp_stat_list (stderr, all_stats);
  pp_type_list (stderr, imp_types);
}

static void save_data_structures (char *basename)
{ BinFile bf;
  char fname[MAXPATHLEN + 1];
  sprintf (fname, "%s.sav", basename);
  bf = dcg_bin_fopen (fname, "w", 1);
  dcg_bin_save_header (bf, dcg_tool, dcg_kind, DCG_VERSION);
  save_def_list (bf, all_defs);
  save_stat_list (bf, all_stats);
  save_type_list (bf, imp_types);
  dcg_bin_save_eof (bf);
  dcg_bin_fclose (&bf);
}

/*
   Main code
*/
int main (int argc, char **argv)
{ /* Initialization */
  char basename[MAXPATHLEN + 1];
  dcg_init_error (stderr, "dcg", NULL);
  dcg_init_alloc ("dcg", 0);
  init_options ();
  scan_options (&argc, argv);
  scan_basename (argc, argv, basename);
  try_report_version ();

  /* Load or parse and analyze the input */
  if (loading_flag) try_load_data_structures (basename);
  else parse (basename, include_path);
  analyze ();

  /* Check whether to dump or save */
  if (full_verbose) dump_data_structures ();
  if (saving_flag) save_data_structures (basename);

  /* Check for extra transformer analysis */
  if (xformer_flag)
    { analyze_ex_names (ex_names);
      code_xformer (basename, ex_names, xformer_pname, xformer_ptype);
    };

  /* All is ready to code, now do it */
  if (coding_flag) code (basename);
#ifdef DEBUG
  if (debug) dcg_report_alloc ();
#endif
  return (0);
}
