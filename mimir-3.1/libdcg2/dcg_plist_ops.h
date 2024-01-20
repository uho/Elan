/*
   File: dcg_plists_ops.h
   Defines some general operations we usually need.

   Copyright (C) 2011 Marc Seutter

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

   CVS ID: "$Id: dcg_plist_ops.h,v 1.7 2011/08/13 12:50:07 marcs Exp $"
*/
#ifndef IncDcgPlistsOps
#define IncDcgPlistsOps

/* libdcg includes */
#include <dcg.h>
#include <dcg_string.h>

/* exported operations */
int is_on_int_list (int_list il, int i);
int is_on_string_list (string_list sl, string s);
void add_uniquely_to_int_list (int_list il, int i);
void add_uniquely_to_string_list (string_list sl, string s);

#endif /* IncDcgPlistsOps */
