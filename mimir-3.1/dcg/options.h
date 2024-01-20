/*
   File: options.h
   Defines flags and parameters of the datastructure generator

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

   CVS ID: "$Id: options.h,v 1.5 2008/06/28 13:03:45 marcs Exp $"
*/
#ifndef IncOptions
#define IncOptions

/* libdcg includes */
#include <dcg.h>
#include <dcg_string.h>

/* local include */
#include "dcg_code.h"

/* Exported flags */
extern int coding_flag;
extern int saving_flag;
extern int loading_flag;
extern int xformer_flag;
extern int code_transput;
extern int code_rdup;
extern int code_extra_tags;
extern int code_pretty_print;

/* Exported parameters and search paths */
extern string_list ex_names;
extern string_list include_path;
extern string xformer_ptype;
extern string xformer_pname;
extern string basic_types;

/* Exported routines to parse the command line and report version */
void init_options ();
void scan_options (int *argc, char **argv);
void scan_basename (int argc, char **argv, char *basename);
void try_report_version ();

#endif /* IncOptions */
