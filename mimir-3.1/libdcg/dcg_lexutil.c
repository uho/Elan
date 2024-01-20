/*
   File: dcg_lexutil.c
   Defines some utilities for the lexical analyzer

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

   CVS ID: "$Id: dcg_lexutil.c,v 1.13 2011/09/02 09:58:00 marcs Exp $"
*/

/* standard includes */
#include <stdio.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

/* local includes */
#include "dcg.h"
#include "dcg_error.h"
#include "dcg_arith.h"
#include "dcg_string.h"
#include "dcg_lexutil.h"

string dcg_convert_string (char *text, int handle_escapes)
{ char buffer[MAXSTRLEN];
  char *sptr, *dptr;
  for (sptr = text+1, dptr = buffer; 1; sptr++, dptr++)
    if (!*sptr) break;
    else if (*sptr == '"')
      { sptr++;
	if (*sptr == '"') *dptr = '"';
	else break;
      }
    else if (handle_escapes && (*sptr == '\\'))
      { sptr++;
	switch (*sptr)
	  { case '"': *dptr = '"'; break;
	    case 'E':
	    case 'e': *dptr = '\033'; break;
	    case 'n': *dptr = '\n'; break;
	    case 'r': *dptr = '\r'; break;
	    case 't': *dptr = '\t'; break;
	    case '\\': *dptr = '\\'; break;
	    default: dcg_error (1, "unknown escape sequence '\\%c'", *sptr);
	  };
      }
    else *dptr = *sptr;
  *dptr = '\0';
  return (new_string (buffer));
}

string dcg_convert_identifier (char *text, int to_low)
{ char buffer[MAXSTRLEN];
  char *sptr, *dptr;
  for (sptr = text, dptr = buffer; *sptr; sptr++, dptr++)
    if (isupper (*sptr) && to_low) *dptr = tolower (*sptr);
    else *dptr = *sptr;
  *dptr = '\0';
  return (new_string (buffer));
}

static int convert_hexdigit (char c)
{ if (('0' <= c) && (c <= '9')) return (c - '0');
  else if (('A' <= c) && (c <= 'F')) return (c - 'A' + 10);
  else if (('a' <= c) && (c <= 'f')) return (c - 'a' + 10);
  else return (0);
}

int dcg_convert_integer (char *text, int base)
{ char *sptr = text;
  int value = 0;
  do { value = value * base + convert_hexdigit (*sptr++); }
  while (isalnum (*sptr));
  return (value);
}

real dcg_convert_real (char *text)
{ real value = 0.0;
  int had_comma = 0;
  int exp = 0;
  int i;
  for (i = 0;
       i < (int) strlen(text) && (text[i] != 'e') && (text[i] != 'E');
       i++)
    if (text[i] == '.') had_comma = 1;
    else
      { value = 10.0 * value + convert_hexdigit (text[i]);
	exp -= had_comma;
      };
  if ((text[i] == 'e') || (text[i] == 'E'))
    { int sec_exp = 0;
      int sec_sign = 0;
      i++;
      if (text[i] == '-') { sec_sign = 1; i++; }
      else if (text[i] == '+') i++;
      for ( ; i < (int) strlen (text) && isalnum (text[i]); i++)
	sec_exp = sec_exp * 10 + convert_hexdigit (text[i]);
      if (sec_sign) exp -= sec_exp;
      else exp += sec_exp;
    };
  value *= power10 (exp);
  return (value);
}
