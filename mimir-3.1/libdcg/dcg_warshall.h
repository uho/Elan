/*
   File: dcg_warshall.h
   Implements Warshalls algorithm
   Added to the library as we use it too often in our compilers
  
   Copyright (C) 2008-2011 Marc Seutter

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

   CVS ID: "$Id: dcg_warshall.h,v 1.1 2011/09/03 19:38:33 marcs Exp $"
*/
#ifndef IncDcgWarshall
#define IncDcgWarshall

/* exported algorithm */
char *dcg_warshall (int n, char *rel);

#endif /* IncDcgWarshall */
