/*
   File: analyze.h
   Analyzes the datastructure definitions

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

   CVSID: "$Id: analyze.h,v 1.5 2008/06/28 13:03:45 marcs Exp $"
*/
#ifndef IncAnalyze
#define IncAnalyze

/* libdcg includes */
#include <dcg.h>

/* exported functions */
void analyze ();
void analyze_ex_names (string_list ex_names);

#endif /* IncAnalyze */
