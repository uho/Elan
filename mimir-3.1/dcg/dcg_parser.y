/*
   File: dcg.y
   Defines the grammar of the datastructure definition file

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

   CVS ID: "$Id: dcg_parser.y,v 1.7 2008/06/28 13:03:45 marcs Exp $"
*/

/* First includes */
%{
/* Global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>

/* local includes */
#include "dcg_lex.h"
#include "dcg_code.h"
#include "dcg_parser2.h"
#define YYERROR_VERBOSE 1

def_list all_defs;
stat_list all_stats;
type_list imp_types;
%}

/*
   Types
*/
%union {
	int value;			/* int is a reserved word in C */
	string string;
	string_list string_list;
	def definition;
	def_list definition_list;
	stat statement;
	stat_list statement_list;
	field field;
	field_list field_list;
	vfield vfield;
	vfield_list vfield_list;
	type type;
	type_list type_list;
}

/*
   Tokens
*/

/* general tokens */
%token <string> identifier
%token <string> string_literal
%token <string> constructor

/* punctuation tokens */
%token left_brace
%token right_brace
%token left_parenthesis
%token right_parenthesis
%token left_square
%token right_square
%token colon
%token comma
%token semicolon
%token equals
%token choice
%token grtr
%token question_mark
%token plus
%token minus
/* error tokens */
%token lex_error_token

/* rule typing */
%type <field_list> field_sequence
%type <field> field
%type <vfield_list> variant_fields_option
%type <vfield_list> variant_fields
%type <vfield> variant_field
%type <field_list> field_parts
%type <field> field_part
%type <type_list> type_list
%type <type> type
%type <string_list> identifier_list
%type <value> traversal_option
/* starting rule */
%start rules

%%
/*
   Syntax rules
*/
rules:
	  rules declaration
	| rules statement
	|
	; 

declaration:
	  primitive_declaration	
	| enumeration_declaration
	| record_declaration
	;

enumeration_declaration:
	  identifier equals
	  left_brace identifier_list right_brace
	  semicolon
	  { app_def_list (all_defs, new_Enum ($1,$4)) }
	;

primitive_declaration:
	  grtr type semicolon	
	  { app_type_list (imp_types, $2);
	    if ($2 -> tag == TAGTname)
	       app_def_list (all_defs, new_Primitive ($2 -> Tname.tname)) }
	;

record_declaration:
	  identifier equals left_parenthesis
	  field_sequence variant_fields_option
	  right_parenthesis semicolon
	  { app_def_list (all_defs, new_Record ($1,$4,$5)) }
	;

statement:
	  use_statement
	| import_statement
	;

use_statement:
	  question_mark type_list semicolon
	  { app_stat_list (all_stats, new_Use ($2)) }
	;

import_statement:
	  plus string_literal semicolon
	  { app_stat_list (all_stats, new_Import ($2)) }
	;

field_sequence:
	  field_sequence field		{ $$ = append_field_list ($1,$2) }
	|					{ $$ = new_field_list () }
	;

field:
	  traversal_option identifier colon type semicolon
	  { $$ = new_field ($2,$4,$1) }
	;

variant_fields_option:
	  variant_fields semicolon		{ $$ = $1 }
	|					{ $$ = new_vfield_list () }
	;

variant_fields:
	  variant_fields choice variant_field
	  { $$ = append_vfield_list ($1,$3) }
	| variant_field
	  { $$ = new_vfield_list (); app_vfield_list ($$,$1) }
	;

variant_field:
	  constructor field_parts		{ $$ = new_vfield ($1,$2) }
	;

field_parts:
	  field_parts field_part	{ $$ = append_field_list ($1,$2) }
	|					{ $$ = new_field_list () }
	;

field_part:
	  traversal_option identifier colon type
	  { $$ = new_field ($2,$4,$1) }
	;

type_list:
	  type_list comma type	{ $$ = append_type_list ($1, $3) }
	| type		{ $$ = new_type_list (); app_type_list ($$, $1) }
	;

type:
	  identifier				{ $$ = new_Tname ($1) }
	| left_square type right_square		{ $$ = new_Tlist ($2) }
	;

identifier_list:
	 identifier_list comma identifier { $$ = append_string_list ($1, $3) }
       | identifier	{ $$ = new_string_list (); app_string_list ($$, $1) }
       ;

traversal_option:
	 minus		{ $$ = 0 }
	|		{ $$ = 1 }
	; 
