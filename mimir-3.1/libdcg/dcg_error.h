/*
   File: dcg_error.h
   Defines routines for diagnostic messages

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

   CVS ID: "$Id: dcg_error.h,v 1.12 2012/01/11 15:43:01 marcs Exp $"
*/
#ifndef IncDcgError
#define IncDcgError

/* Standard includes */
#include <stdio.h>

/* Local includes */
#include <dcg.h>

/* Exported variables */
extern int debug;
extern int verbose;
extern int full_verbose;

/* Exported logging functions */
void dcg_exit (int error);
void dcg_panic (char *format, ...);
void dcg_abort (char *location, char *format, ...);
void dcg_bug (char *location, char *format, ...);
void dcg_error (int prefix, char *format, ...);
void dcg_warning (int prefix, char *format, ...);
void dcg_hint (char *format, ...);
void dcg_wlog (char *format, ...);
void dcg_eprint (char *format, ...);

/* Other exported functions */
void dcg_init_error (FILE *f, char *tname, void (*onexit)());
void dcg_internal_error (char *location);
void dcg_bad_tag (int tag, char *location);
void dcg_panic_if_errors ();
FILE *dcg_error_file ();

#endif /* IncDcgError */
