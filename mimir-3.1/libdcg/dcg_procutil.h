/*
   File: dcg_procutil.h
   Defines routines to spawn subprocesses. For the moment we only
   need code that spawns a subprocess and then waits for completion.
   As soon as we need asynchronous process communication we will add it.

   Copyright 2011 M. Seutter

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

   CVS ID: "$Id: dcg_procutil.h,v 1.1 2011/10/14 11:19:10 marcs Exp $"
*/
#ifndef IncDcgProcutil
#define IncDcgProcutil

#ifdef __cplusplus
extern "C" {
#endif

int dcg_spawn_and_wait (char *argv[]);

#ifdef __cplusplus
}
#endif
#endif /* IncDcgProcutil */
