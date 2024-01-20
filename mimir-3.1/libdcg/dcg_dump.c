/*
   File: dcg_dump.c
   Defines basic transput routines

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

   CVS ID: "$Id: dcg_dump.c,v 1.3 2011/09/27 19:00:29 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* standard includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* local includes */
#include "dcg.h"
#include "dcg_string.h"
#include "dcg_dump.h"

/*
   Estimate printing of standard types
*/
int est_int (int x)
{ char buf[32];
#ifdef WIN32
  _snprintf (buf, 32, "%d", x);
#else 
  snprintf (buf, 32, "%d", x);
#endif
  return (strlen (buf));
}

int est_u_int (u_int x)
{ char buf[32];
#ifdef WIN32
  _snprintf (buf, 32, "%u", x);
#else
  snprintf (buf, 32, "%u", x);
#endif
  return (strlen (buf));
}

int est_int64 (int64 x)
{ char buf[32];
#ifdef WIN32
  _snprintf (buf, 32, "0x%08x%08xLL", (int) (x >> 32), (int) x);
#else
  snprintf (buf, 32, "0x%08x%08xLL", (int) (x >> 32), (int) x);
#endif
  return (strlen (buf));
}

int est_u_int64 (u_int64 x)
{ char buf[32];
#ifdef WIN32
  _snprintf (buf, 32, "0x%08x%08xULL", (u_int)(x >> 32), (u_int) x);
#else
  snprintf (buf, 32, "0x%08x%08xULL", (u_int)(x >> 32), (u_int) x);
#endif
  return (strlen (buf));
}

int est_real (real x)
{ char buf[32];
#ifdef WIN32
  _snprintf (buf, 32, "%g", x);
#else
  snprintf (buf, 32, "%g", x);
#endif
  return (strlen (buf));
}

int est_string (string x)
{ if (x == string_nil) return (2);
  return (strlen (x) + 2);
}

void pppindent (FILE *f, int ind)
{ int i;
  for (i = 0; i < ind/8; i++) fputc ('\t', f);
  for (i = 0; i < ind%8; i++) fputc (' ', f);
}

void pppdelim (FILE *f, int horiz, int ind, char x)
{ if ((x == ')') || (x == ']') || (x == '}') || (x == '>'))
    { if (!horiz)
	{ fputc ('\n', f);
	  pppindent (f, ind);
	}
      else fputc (' ', f);
      fputc (x, f);
    }
  else
    { fputc (x, f);
      if (!horiz)
	{ fputc ('\n', f);
	  pppindent (f, ind + 2);
	}
       else fputc (' ', f);
    };
}
