/*
   File: dcg_lex.l
   Defines a lexical analyzer for the datastructure definition files

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

   CVS ID: "$Id: dcg_lex.l,v 1.11 2011/08/13 12:57:03 marcs Exp $"
*/

%{
/* Standard includes */
#include <stdio.h>
#include <stdlib.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>
#include <dcg_lexutil.h>

/* local includes */
#include "dcg_code.h"
#include "dcg_parser.h"		/* y.tab.h */
#include "dcg_lex.h"

/* Keep gcc -Wall happy */
extern int isatty (int desc);
static void increment_line_number ();

/* We do not use UNPUT */
#define YY_NO_UNPUT
%}

DG		[0-9]
LC		[a-z]
UC		[A-Z]
LG		{DG}|{LC}|\_

%option noyywrap
%option prefix="pdcg_"
%%

{LC}{LG}*	{ pdcg_lval.string = new_string (yytext);
		  return (identifier);
		}
{UC}{LG}*	{ pdcg_lval.string = new_string (yytext);
		  return (constructor);
		}
\{		return left_brace;
\}		return right_brace;
\(		return left_parenthesis;
\)		return right_parenthesis;
\[		return left_square;
\]		return right_square;
:		return colon;
,		return comma;
\;		return semicolon;
=		return equals;
>		return grtr;
\?		return question_mark;
\|		return choice;
\+		return plus;
\-		return minus;
\"[^\n\"]*(\"\"[^\n\"]*)*\" { pdcg_lval.string = dcg_convert_string (yytext, 1);
			      return (string_literal);
			    }
\#[^!\n][^\n]*	;
[ \t\015]	;
\#\n		{ increment_line_number (); }
\n		{ increment_line_number (); }
.		return lex_error_token;
%%
/*
   Initialization
*/
static int line_number;
void init_lex (FILE *in)
	{ pdcg_restart (in);
	  line_number = 1;
	};

/*
   Error recovery (of some kind)
*/
static void increment_line_number ()
	{ line_number++;
	};

void pdcg_error (char *msg)
	{ dcg_panic ("line %d: %s", line_number, msg);
	};
