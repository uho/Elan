/*
   File: dcg_parser2.h
   Defines the actual export by the parser

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

   CVS ID: "$Id: dcg_parser2.h,v 1.7 2008/06/28 13:03:45 marcs Exp $"
*/
#ifndef IncDcgParser2
#define IncDcgParser2

/* libdcg includes */
#include <dcg.h>

/* exported variables */
extern def_list all_defs;
extern stat_list all_stats;
extern type_list imp_types;

/* exported functions */
int pdcg_parse ();
void parse (char *basename, string_list include_path);

#endif /* IncDcgParser2 */
