/*
   File: dcg_dump.h
   Defines pretty printing routines to dump data structures

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

   CVS ID: "$Id: dcg_dump.h,v 1.2 2011/09/07 08:46:59 marcs Exp $"
*/
#ifndef IncDcgDump
#define IncDcgDump

/* Standard includes */
#include <stdlib.h>
#include <stdio.h>

/* Local includes */
#include <dcg.h>
#include <dcg_string.h>

/* Estimate printing of standard types */
#define MAXWIDTH 80
#define PTRWIDTH (sizeof (vptr) + 8)
int est_int (int x);
int est_u_int (u_int x);
int est_int64 (int64 x);
int est_u_int64 (u_int64 x);
int est_real (real x);
int est_string (string x);
#define est_char(x) 3
#define est_vptr(x) PTRWIDTH
#define est_int32 est_int
#define est_u_int32 est_u_int

/* Define support for pretty printing */
void pppindent (FILE *f, int ind);
void pppdelim (FILE *f, int horiz, int ind, char x);
#define pppstring(f,x) fputs(x,f)

/* Define pretty printing of standard types */
#define ppp_int(f,hor,ind,x) fprintf (f,"%d",x)
#define ppp_u_int(f,hor,ind,x) fprintf (f,"%u",x)
#define ppp_int32(f,hor,ind,x) fprintf (f,"%d",x)
#define ppp_u_int32(f,hor,ind,x) fprintf (f,"%u",x)
#define ppp_int64(f,hor,ind,x) { fprintf (f, "0x%08x", ((int) (x >> 32))); \
				 fprintf (f, "%08xLL", ((int) x)); }
#define ppp_u_int64(f,hor,ind,x) { fprintf (f, "0x%08x", ((int) (x >> 32))); \
				   fprintf (f, "%08xULL", ((int) x)); }
#define ppp_real(f,hor,ind,x) fprintf (f,"%g",x)
#define ppp_string(f,hor,ind,x) fprintf (f,"\"%s\"",x)
#define ppp_char(f,hor,ind,x) fprintf (f,"'%c'",x)
#define ppp_vptr(f,hor,ind,x) fprintf (f, "--> %p", x)

#define pp_char(f,ind,x) fprintf (f,"'%c'",x)
#define pp_int(f,ind,x) fprintf (f,"%d",x)
#define pp_u_int(f,ind,x) fprintf (f,"%u",x)
#define pp_int32(f,ind,x) fprintf (f,"%d",x)
#define pp_u_int32(f,ind,x) fprintf (f,"%u",x)
#define pp_int64(f,ind,x) { fprintf (f, "0x%08x", ((int) (x >> 32)); \
			    fprintf (f, "%08xLL", ((int) x)); }
#define pp_u_int64(f,ind,x) { fprintf (f, "0x%08x", ((int) (x >> 32)); \
			      fprintf (f, "%08xULL", ((int) x)); }
#define pp_real(f,ind,x) fprintf (f,"%g",x)
#define pp_string(f,ind,x) fprintf (f,"\"%s\"",x)
#define pp_vptr(f,ind,x) ppp_vptr (f,0,0,x)

#endif /* IncDcgDump */
