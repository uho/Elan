/*
   File: xform_coder.h
   Codes a datastructure transformer

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

   CVS ID: "$Id: xform_coder.h,v 1.6 2008/06/28 13:03:45 marcs Exp $"
*/
#ifndef IncXformCoder
#define IncXformCoder

/* libdcg includes */
#include <dcg.h>

/* local includes */
#include "dcg_code.h"

/* exported transformer */
void code_xformer (char *basename, string_list ex_names, string pname, string ptype);

#endif /* IncXformCoder */
