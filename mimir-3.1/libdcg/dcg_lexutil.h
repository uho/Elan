/*
   File: dcg_lexutil.h
   Defines some general utilities for lexical analyzers

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

   CVS ID: "$Id: dcg_lexutil.h,v 1.11 2011/09/07 08:46:59 marcs Exp $"
*/
#ifndef IncDcgLexUtil
#define IncDcgLexUtil

/* Local includes */
#include <dcg.h>
#include <dcg_string.h>

/* Exported code */
string dcg_convert_string (char *text, int handle_escapes);
string dcg_convert_identifier (char *text, int to_low);
int dcg_convert_integer (char *text, int base);
real dcg_convert_real (char *text);

#endif /* IncDcgLexUtil */
