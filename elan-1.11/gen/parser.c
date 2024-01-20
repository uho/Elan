/*
   File: parser.c
   Defines a parser for ELAN

   CVS ID: "$Id: parser.c,v 1.12 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>
#include <dcg_plist.h>
#include <dcg_plist_ops.h>

/* local includes */
#include "options.h"
#include "elan_ast.h"
#include "type_table.h"
#include "lexer.h"
#include "parser.h"

/* error administration */
static void parser_error (int lin, int col, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  dcg_error (0, "%s in file %s, line %d, col %d", buf, this_fname, lin, col);
}

/*
   match_or_skip_tokens will skip tokens until it has skipped the match_symbol
   or the lookahead symbol is in the set of sync_symbols
*/
static symbol empty_set[] = { UNDEFINED };
static void match_or_skip_tokens (symbol match_symbol, symbol *sync_symbols)
	{ while (1)
	     { int ix;
	       if ((match_symbol != UNDEFINED) && (this_symbol == match_symbol))
		  { read_token ();
		    return;
		  };
	       if (this_symbol == EOF_SYMBOL) return;	/* ultimate heuristic */
	       for (ix = 0; sync_symbols[ix] != UNDEFINED; ix++)
		  if (this_symbol == sync_symbols[ix]) return;
	       read_token ();
	     };
	};
#define skip_tokens(sync_symbols) match_or_skip_tokens (UNDEFINED,sync_symbols)

static int is_token (symbol sy)
	{ if (this_symbol != sy) return (0);
	  read_token ();
	  return (1);
	};

static void should_be_token (symbol sy, char *kind, symbol *sync_symbols)
	{ if (is_token (sy)) return;
	  parser_error (this_line, this_column, "%s expected", kind);
	  match_or_skip_tokens (sy, sync_symbols);
	};

/*
   Routines to handle the LL(3) lookahead
*/
static int this_token_is (symbol sy)
	{ return (this_symbol == sy);
	};

static int next_token_is (symbol sy)
	{ return (next_symbol == sy);
	};

static int next2_token_is (symbol sy)
	{ return (next2_symbol == sy);
	};

/*
   Routines for syntactic categories with some attributed value
*/
static int is_small_name (string *rval)
	{ string sval = this_sval;
	  if (is_token (SMALL_NAME))
	     { *rval = sval;
	       return (1);
	     };
	  return (0);
	};

static void should_be_small_name (string *rval, symbol *sync_symbols)
	{ if (is_small_name (rval)) return;
	  parser_error (this_line, this_column, "small name expected");
	  match_or_skip_tokens (SMALL_NAME, sync_symbols);
	  *rval = string_nil;
	};

static int is_capital_name (string *rval)
	{ string sval = this_sval;
	  if (is_token (CAPITAL_NAME))
	     { *rval = sval;
	       return (1);
	     };
	  return (0);
	};

static void should_be_text_denotation (string *rval, symbol *sync_symbols)
	{ string sval = this_sval;
	  if (is_token (TEXT_DENOTER))
	     { *rval = sval;
	       return;
	     }
	  parser_error (this_line, this_column, "text denotation expected");
	  match_or_skip_tokens (TEXT_DENOTER, sync_symbols);
	  *rval = string_nil;
	};

static void should_be_eof ()
	{ if (this_symbol == EOF_SYMBOL) return;
	  parser_error (this_line, this_column, "end of file expected");
	  skip_tokens (empty_set);
	};

/*
   Forward declare the parsing procedures.
   In this way we can code along the grammar
*/
static int is_packet_head (string *packet_name, string_list *defines);
static void may_be_export_interface (string_list *defines);
static void should_be_export_name_list (string_list defines);
static void may_be_packet_body (string_list puses, string_list puselibs,
				decl_list pdecls, node_list pcode);
static void may_be_packet_paragraph (string_list puses, string_list puselibs,
				     decl_list pdecls, node_list pcode);
static int is_packet_unit (string_list puses, string_list puselibs, 
			   decl_list pdecls, node_list pcode);
static void should_be_packet_tail (string packet_name);
static int is_import_declaration (string_list puses, string_list puselibs);
static void should_be_packet_name_list (string_list puses);
static int is_imported_name (string_list puses);
static void should_be_imported_name (string_list puses);
static int is_closed_declaration (decl_list pdecls);
static void should_be_rest_closed_declaration (int line, int column, type rtyp, decl *rdcl);
static int is_rest_procedure_declaration (int line, int column, type rtyp, decl *rdcl);
static int is_rest_procedure_head (string *pname, pragmat_list *curr, decl_list *fpars);
static void should_be_procedure_tail (string pname);
static int is_rest_operator_declaration (int line, int column, type rtyp, decl *rdcl);
static int is_rest_operator_head (string *oname, pragmat_list *curr, decl_list *fpars);
static void should_be_operator_tail (string oname);
static void may_be_routine_body (body *bdy);
static void may_be_formal_parameter_spec_pack (decl_list *fpars);
static int is_formal_parameter_spec_pack (decl_list *fpars);
static void should_be_formal_parameter_spec_pack (decl_list *fpars);
static void should_be_formal_parameter_spec_list (decl_list fpars);
static int is_formal_parameter_spec (decl_list fpars);
static void should_be_formal_parameter_spec (decl_list fpars);
static void may_be_refinement_sequence (decl_list decls);
static void should_be_refinement_sequence (decl_list decls, int routine);
static int is_refinement (decl_list decls);
static void may_be_paragraph (decl_list decls, node_list *paragraph);
static int is_paragraph (decl_list decls, node_list paragraph);
static int is_unit (decl_list decls, node_list paragraph);
static void should_be_unit (decl_list decls, node_list paragraph);
static int is_basic_declaration (decl_list decls, node_list paragraph);
static int is_object_declaration (decl_list decls, node_list paragraph);
static int object_declarer_allowed ();
static void should_be_object_association_list (decl_list decls, node_list paragraph,
					       type otyp, e_access oacc);
static int is_object_association (decl_list decls, node_list paragraph,
				  type otyp, e_access oacc);
static void should_be_object_association (decl_list decls, node_list paragraph,
				          type otyp, e_access oacc);
static void may_be_initialization (decl_list decls, node_list paragraph, string oname);
static int is_initialization (decl_list decls, node_list paragraph, string oname);
static void should_be_initialization (decl_list decls, node_list paragraph, string oname);
static int is_synonym_declaration (decl_list decls);
static void should_be_synonym_association_list (decl_list decls);
static int is_synonym_association (decl_list decls);
static void should_be_synonym_association (decl_list decls);
static int is_synonym_value_association (decl_list decls);
static void should_be_type_association_list (decl_list pdecls);
static int is_type_association (decl_list pdecls, int strong);
static void should_be_type_association (decl_list pdecls, int strong);
static int is_formal_declarer (type *t, e_access *acc);
static void should_be_formal_declarer (type *t, e_access *acc);
static void may_be_parameter_declarer_pack (pdecl_list *pdecls);
static void should_be_parameter_declarer_list (pdecl_list pdecls);
static int is_object_declarer (type *t, e_access *acc);
static int is_type_declarer (type *t);
static void should_be_type_declarer (type *t);
static int is_elementary_type_declarer (type *t);
static int is_concrete_type_declarer (type *t);
static int is_abstract_type_declarer (type *t);
static int is_composed_type_declarer (type *t);
static int is_row_declarer (type *t);
static void should_be_cardinality (node *crd);
static int is_struct_declarer (type *t);
static void should_be_field_specification_list (field_list fields);
static int is_field_specification (field_list fields);
static void should_be_field_specification (field_list fields);
static void may_be_access_declarer (e_access *acc);
static int is_access_declarer (e_access *acc);
static int is_expression (decl_list decls, node *expr);
static void should_be_expression (decl_list decls, node *expr);
static int is_priority_ii_formula (decl_list decls, node *form);
static void should_be_priority_ii_formula (decl_list decls, node *form);
static int is_priority_ii_operator (string *dop);
static int is_priority_iii_formula (decl_list decls, node *form);
static void should_be_priority_iii_formula (decl_list decls, node *form);
static int is_priority_iii_operator (string *dop);
static int is_priority_iv_formula (decl_list decls, node *form);
static void should_be_priority_iv_formula (decl_list decls, node *form);
static int is_priority_iv_operator (string *dop);
static int is_priority_v_formula (decl_list decls, node *form);
static void should_be_priority_v_formula (decl_list decls, node *form);
static int is_priority_v_operator (string *dop);
static int is_priority_vi_formula (decl_list decls, node *form);
static void should_be_priority_vi_formula (decl_list decls, node *form);
static int is_priority_vi_operator (string *dop);
static int is_priority_vii_formula (decl_list decls, node *form);
static void should_be_priority_vii_formula (decl_list decls, node *form);
static int is_priority_vii_operator (string *dop);
static int is_priority_viii_formula (decl_list decls, node *form);
static void should_be_priority_viii_formula (decl_list decls, node *form);
static int is_priority_viii_operator (string *dop);
static int is_priority_ix_formula (decl_list decls, node *form);
static void should_be_priority_ix_formula (decl_list decls, node *form);
static int is_priority_ix_operator (string *dop);
static int is_monadic_formula (decl_list decls, node *mon);
static void should_be_monadic_formula (decl_list decls, node *mon);
static int is_monadic_operator (string *mop);
static int is_primary (decl_list decls, node *prim);
static int is_rest_subscription (decl_list decls, node *prim);
static int is_rest_selection (node *prim);
static int is_rest_call (decl_list decls, node *prim);
static int is_primary_prefix (decl_list decls, node *prim);
static int is_enclosed_expression (decl_list decls, node *prim);
static int is_display (decl_list decls, node *prim);
static int is_abstractor (decl_list decls, node *prim);
static int is_concretizer (decl_list decls, node *prim);
static int is_nil (node *den);
static int is_identifier_application (node *prim);
static int is_conditional_choice (decl_list decls, node *cond);
static void should_be_condition (decl_list decls, node *expr);
static void should_be_then_part (decl_list decls, node_list *thenp);
static void may_be_else_part (decl_list decls, node_list *elsep);
static int is_else_part (decl_list decls, node_list *elsep);
static int is_numerical_choice (decl_list decls, node *num);
static int is_case_part_sequence (decl_list decls, case_part_list *parts);
static void should_be_case_part_sequence (decl_list decls, case_part_list *parts);
static int is_case_part (decl_list decls, case_part *parts);
static int is_case_label_list (node_list *clab);
static void should_be_case_label_list (node_list *clab);
static int is_case_label (node *clab);
static void should_be_case_label (node *clab);
static void may_be_otherwise_part (decl_list decls, node_list *others);
static int is_repetition (decl_list decls, node *rep);
static int is_for_part (string *lvar);
static int is_from_part (decl_list decls, node *from);
static int is_direction_part (decl_list decls, int *dir, node *to);
static int is_while_part (decl_list decls, node *cond);
static void may_be_until_part (decl_list decls, node *cond);
static int is_terminator (decl_list decls, node *term);
static void may_be_result_option (decl_list decls, node *result);
static int is_export_name (string *name);
static void should_be_export_name (string *name, symbol* sync_set);
static void should_be_algorithm_name (string *name, symbol *sync_set);
static int is_operator_name (string *name);
static void should_be_operator_name (string *name, symbol *sync_set);
static int is_special_name (string *name);
static int is_denoter (node *den);
static int is_integer_denoter (node *den);
static int is_boolean_denoter (node *den);
static int is_real_denoter (node *den);
static int is_text_denoter (node *den);

/*
   3: Programs and preludes

   a) elan program:
	packet sequence option, main packet.

   b) packet:
	packet head, packet body, packet tail.

   c) main packet:
        packet paragraph option, refinement sequence.

   FIRST (packet) = PACKET
*/
static int is_packet (packet *pk)
	{ string pname;
	  string_list defines;
	  if (is_packet_head (&pname, &defines))
	     { string_list puses = new_string_list ();
	       string_list puselibs = new_string_list ();
	       decl_list pdecls = new_decl_list ();
	       node_list pcode = new_node_list ();
	       may_be_packet_body (puses, puselibs, pdecls, pcode);
	       should_be_packet_tail (pname);
	       should_be_eof ();
	       *pk = new_packet (this_fname, pname, defines, puses, puselibs, pdecls, pcode);
 	       return (1);
	     };
	  return (0);
	};

void should_be_packet (string pname, packet *pk)
	{ if (!is_packet (pk))
	     parser_error (this_line, this_column, "packet expected");
	  else if (!streq (pname, (*pk) -> pname))
	     dcg_error (0, "package name %s and package source filename %s do not match",
		        (*pk) -> pname, this_fname);
	};

void should_be_main_packet (packet *pk)
	{ string_list puses = new_string_list ();
	  string_list puselibs = new_string_list ();
	  decl_list pdecls = new_decl_list ();
	  node_list pcode = new_node_list ();
	  may_be_packet_paragraph (puses, puselibs, pdecls, pcode);
	  should_be_refinement_sequence (pdecls, 0);
	  should_be_eof ();
	  *pk = new_packet (this_fname, new_string (""), new_string_list (), puses,
			    puselibs, pdecls, pcode);
	}

/*
   d) packet head:
	 packet token, packet name, export interface option, colon token.

   e) export interface:
	 defines token, export name list.

   Rule for export name list

   FIRST (packet head) = PACKET
   FIRST (export interface) = EXPORT

   FOLLOW (packet name) = EXPORT | COLON

   We synchronize on EXPORT, COLON or FIRST (packet body)
   SYNC (packet name) = EXPORT | COLON | PROC | OP | TYPE | LET | USES | USES_LIBRARY |
			INT | BOOL | REAL | TEXT | ROW | STRUCT |
			CAPITAL_NAME | SMALL_NAME | ENDPACKET
*/
static symbol packet_head_sync_set[] =
	{ DEFINES_SYMBOL, COLON_SYMBOL, PROC_SYMBOL, OP_SYMBOL, TYPE_SYMBOL,
	  LET_SYMBOL, USES_SYMBOL, USES_LIBRARY_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL,
	  TEXT_SYMBOL, ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, SMALL_NAME,
	  ENDPACKET_SYMBOL, UNDEFINED
	};
static symbol packet_head_sync_set2[] =
	{ COLON_SYMBOL, PROC_SYMBOL, OP_SYMBOL, TYPE_SYMBOL,
	  LET_SYMBOL, USES_SYMBOL, USES_LIBRARY_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL,
	  TEXT_SYMBOL, ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, SMALL_NAME,
	  ENDPACKET_SYMBOL, UNDEFINED
	};
static symbol packet_head_sync_set3[] =
	{ COMMA_SYMBOL, COLON_SYMBOL, PROC_SYMBOL, OP_SYMBOL, TYPE_SYMBOL,
	  LET_SYMBOL, USES_SYMBOL, USES_LIBRARY_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL,
	  TEXT_SYMBOL, ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, SMALL_NAME,
	  ENDPACKET_SYMBOL, UNDEFINED
	};
static symbol packet_paragraph_sync_set[] =
	{ PROC_SYMBOL, OP_SYMBOL, TYPE_SYMBOL, LET_SYMBOL, USES_SYMBOL, USES_LIBRARY_SYMBOL, 
	  INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL, ROW_SYMBOL, STRUCT_SYMBOL,
	  CAPITAL_NAME, SMALL_NAME, ENDPACKET_SYMBOL, UNDEFINED
	};
static int is_packet_head (string *packet_name, string_list *defines)
	{ if (is_token (PACKET_SYMBOL))
	     { should_be_small_name (packet_name, packet_head_sync_set);
	       may_be_export_interface (defines);
	       should_be_token (COLON_SYMBOL, ":", packet_paragraph_sync_set);
	       return (1);
	     };
	  return (0);
	};

static void may_be_export_interface (string_list *defines)
	{ *defines = new_string_list ();
	  if (is_token (DEFINES_SYMBOL))
	     should_be_export_name_list (*defines);
	};

static int is_export_name_list (string_list defines)
	{ string define;
	  if (is_export_name (&define))
	     { app_string_list (defines, define);
	       while (is_token (COMMA_SYMBOL))
		  { should_be_export_name (&define, packet_head_sync_set3);
		    app_string_list (defines, define);
	          };
	       return (1);
	     };
	  return (0);
	};

static void should_be_export_name_list (string_list defines)
	{ if (is_export_name_list (defines)) return;
	  parser_error (this_line, this_column, "export name expected");
	  skip_tokens (packet_head_sync_set2);
	};

/*
   f) packet body:
	 packet paragraph option, refinement sequence option.

   g) packet paragraph:
	 packet unit, semicolon token, packet paragraph option.

   FIRST (packet body) = PROC | OP | TYPE | LET | USES | USES_LIBRARY | INT | BOOL | REAL |
   			 TEXT | ROW | STRUCT | CAPITAL_NAME | SMALL_NAME
   FOLLOW (packet body) = ENDPACKET

   FIRST (packet paragraph) = 
   FIRST (packet unit) = PROC | OP | TYPE | LET | USES | USES_LIBRARY |
			 INT | BOOL | REAL | TEXT | ROW | STRUCT | CAPITAL_NAME 
   SYNC (packet paragraph) = PROC | OP | TYPE | LET | USES | USES_LIBRARY |
			     INT | BOOL | REAL | TEXT | ROW | STRUCT |
			     CAPITAL_NAME | SMALL_NAME | ENDPACKET
*/
static void may_be_packet_body (string_list puses, string_list puselibs,
				decl_list pdecls, node_list pcode)
	{ may_be_packet_paragraph (puses, puselibs, pdecls, pcode);
	  may_be_refinement_sequence (pdecls);
	};

static void may_be_packet_paragraph (string_list puses, string_list puselibs,
				     decl_list pdecls, node_list pcode)
	{ while (is_packet_unit (puses, puselibs, pdecls, pcode))
	     should_be_token (SEMICOLON_SYMBOL, ";", packet_paragraph_sync_set);
	};

/*
   h) packet unit:
	 basic declaration;
	 closed declaration;
	 import declaration.
*/
static int is_packet_unit (string_list puses, string_list puselibs,
			   decl_list pdecls, node_list pcode)
	{ if (is_closed_declaration (pdecls)) return (1);
	  if (is_basic_declaration (pdecls, pcode)) return (1);
	  if (is_import_declaration (puses, puselibs)) return (1);
	  return (0);
	};

/*
   i) packet tail:
	 end packet token, packet name option, semicolon token.

   FIRST (packet tail) = ENDPACKET

   Since a packet is a compilation unit, the only sync set is end of file
   FOLLOW (packet tail) = EOF 
   SYNC (packet tail) = EOF
*/
static int is_packet_tail (string packet_name)
	{ if (is_token (ENDPACKET_SYMBOL))
	     { string name2;
	       if (is_small_name (&name2) && !streq (packet_name, name2))
		  parser_error (this_line, this_column, "packet name mismatch");
	       should_be_token (SEMICOLON_SYMBOL, ";", empty_set);
	       return (1);
	     };
	  return (0);
	};

static void should_be_packet_tail (string packet_name)
	{ if (is_packet_tail (packet_name)) return;
	  parser_error (this_line, this_column, "ENDPACKET expected");
	  skip_tokens (empty_set);
	};

/*
   j) import declaration:
         uses token, packet name list, semicolon token;
	 uses library token, text denoter, semicolon token.

   k) packet name:
	 small name.

   FIRST (import declaration) = USES | USES_LIBRARY

   SYNC (import declaration) = SEMICOLON | PROC | OP | TYPE | LET | USES | USES_LIBRARY |
			       INT | BOOL | REAL | TEXT | ROW | STRUCT | CAPITAL_NAME | SMALL_NAME
*/
static symbol import_sync_set[] =
	{ SEMICOLON_SYMBOL, PROC_SYMBOL, OP_SYMBOL, TYPE_SYMBOL, LET_SYMBOL,
	  USES_SYMBOL, USES_LIBRARY_SYMBOL, INT_SYMBOL,
	  BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL, ROW_SYMBOL, STRUCT_SYMBOL,
	  CAPITAL_NAME, SMALL_NAME, ENDPACKET_SYMBOL, UNDEFINED
	};
static symbol import_sync_set2[] =
	{ SEMICOLON_SYMBOL, PROC_SYMBOL, OP_SYMBOL, TYPE_SYMBOL, LET_SYMBOL,
	  USES_SYMBOL, USES_LIBRARY_SYMBOL, INT_SYMBOL,
	  BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL, ROW_SYMBOL, STRUCT_SYMBOL,
	  CAPITAL_NAME, SMALL_NAME, ENDPACKET_SYMBOL, UNDEFINED
	};

/* set default location of X11 library path */
#ifndef X11LIBDIR
#define X11LIBDIR "/usr/X11R6/lib"
#endif

static int is_import_declaration (string_list puses, string_list puselibs)
	{ if (is_token (USES_SYMBOL))
	     { should_be_packet_name_list (puses);
	       return (1);
	     }
	  else if (is_token (USES_LIBRARY_SYMBOL))
	     { string libname;
	       should_be_text_denotation (&libname, import_sync_set);
	       add_uniquely_to_string_list (puselibs, libname);
	       detach_string (&libname);
	       return (1);
	     };
	  return (0);
	};

static int is_packet_name_list (string_list puses)
	{ if (is_imported_name (puses))
	     { while (is_token (COMMA_SYMBOL))
		  should_be_imported_name (puses);
	       return (1);
	     };
	  return (0);
	};

static void should_be_packet_name_list (string_list puses)
	{ if (is_packet_name_list (puses)) return;
	  parser_error (this_line, this_column, "packet name expected");
	  skip_tokens (import_sync_set);
	};

static int is_imported_name (string_list puses)
	{ string packet_name;
	  if (is_small_name (&packet_name))
	     { if (iris_compatibility && streq (packet_name, "std"))
		  return (1);
	       app_string_list (puses, packet_name);
	       return (1);
	     };
	  return (0);
	};

static void should_be_imported_name (string_list puses)
	{ if (is_imported_name (puses)) return;
	  parser_error (this_line, this_column, "packet name expected");
	  match_or_skip_tokens (SMALL_NAME, import_sync_set2);
	};

/*
   4 Procedure and operator declarations
  
   a) procedure declaration
         procedure head, routine body, procedure tail.
  
   b) procedure head:
         result option, proc token, procedure name,
         formal parameter spec pack, colon token.
  
   c) procedure tail:
         endproc token, procedure name option.
  
   d) operator declaration:
         operator head, routine body, operator tail.
  
   e) operator head:
         result option, op token, operator name,
         formal parameter spec pack, colon token.
  
   f) operator tail:
         endop token, operater name option.
  
   g) routine body:
         internal token, text denotation;
         external token, text denotation;
	 routine body tail.

   h) routine body tail:
	 paragraph, period token, refinement sequence;
	 paragraph option.
  
   i) result:
         type declarer.
  
   Procedure head and operator head share a common prefix.
   We leftfactorize the prefix and find that operators with one or
   two parameters also share a common prefix and that we have to
   leftfactorize these too. It is for this reason that the code
   for closed declaration is also moved to this location.

   FIRST (procedure declaration) = INT | BOOL | REAL | TEXT | ROW | STRUCT |
   				   CAPITAL_NAME | PROC
   FIRST (operator declaration) = INT | BOOL | REAL | TEXT | ROW | STRUCT |
   				  CAPITAL_NAME | OP
*/
static int is_closed_declaration (decl_list pdecls)
	{ int line = this_line;
	  int column = this_column;
	  type rtyp;
	  decl rdcl;
	  if (is_token (TYPE_SYMBOL))
	     { should_be_type_association_list (pdecls);
	       return (1);
	     }
	  else if (is_type_declarer (&rtyp))
	     { should_be_rest_closed_declaration (line, column, rtyp, &rdcl);
	       app_decl_list (pdecls, rdcl);
	       return (1);
	     }
	  else if (is_rest_procedure_declaration (line, column, attach_type (t_void), &rdcl))
	     { app_decl_list (pdecls, rdcl);
	       return (1);
	     }
	  else if (is_rest_operator_declaration (line, column, attach_type (t_void), &rdcl))
	     { app_decl_list (pdecls, rdcl);
	       return (1);
	     };
	  return (0);
	};

/*
   SYNC (rest_closed_decl_sync)
*/
static symbol rest_closed_decl_sync_set[] =
	{ ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  SEMICOLON_SYMBOL, SMALL_NAME, UNDEFINED
	};
static void should_be_rest_closed_declaration (int line, int column, type rtyp, decl *rdcl)
	{ if (is_rest_procedure_declaration (line, column, rtyp, rdcl)) return;
	  if (is_rest_operator_declaration (line, column, rtyp, rdcl)) return;
	  parser_error (this_line, this_column, "PROC or OP expected");
	  skip_tokens (rest_closed_decl_sync_set);
	  *rdcl = decl_nil;
	};

static int is_rest_procedure_declaration (int line, int column, type rtyp, decl *rdcl)
	{ pragmat_list curr;
	  decl_list fpars;
	  string pname;
	  body bdy;
	  if (is_rest_procedure_head (&pname, &curr, &fpars))
	     { may_be_routine_body (&bdy);
	       should_be_procedure_tail (pname);
	       *rdcl = new_Proc_decl (line, column, rtyp, pname, curr, fpars, bdy);
	       return (1);
	     };
	  return (0);
	};

/*
   SYNC (rest_procedure_head) = OPEN | COLON | FIRST (unit) |
				closing symbols
*/
static symbol rest_routine_head_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  SEMICOLON_SYMBOL, PERIOD_SYMBOL, COLON_SYMBOL, UNDEFINED
	};
static int is_rest_procedure_head (string *pname, pragmat_list *curr, decl_list *fpars)
	{ if (is_token (PROC_SYMBOL))
	     { *curr = read_and_reset_pragmats ();
	       should_be_small_name (pname, rest_routine_head_sync_set);
	       may_be_formal_parameter_spec_pack (fpars);
	       should_be_token (COLON_SYMBOL, ":", rest_routine_head_sync_set);
	       return (1);
	     };
	  return (0);
	};

static symbol rest_routine_tail_sync_set[] =
	{ ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  SEMICOLON_SYMBOL, SMALL_NAME, UNDEFINED
	};
static void should_be_procedure_tail (string pname)
	{ string pname_tail;
	  int line, column;
	  should_be_token (ENDPROC_SYMBOL, "ENDPROC", rest_routine_tail_sync_set);
	  line = this_line;
	  column = this_column;
	  if (!is_small_name (&pname_tail)) return;
	  if (pname == string_nil) return;
	  if (streq (pname, pname_tail)) return;
	  parser_error (line, column, "Procedure name mismatch");
	};

static int is_rest_operator_declaration (int line, int column, type rtyp, decl *rdcl)
	{ pragmat_list curr;
	  decl_list fpars;
	  string oname;
	  body bdy;
	  if (is_rest_operator_head (&oname, &curr, &fpars))
	     { may_be_routine_body (&bdy);
	       should_be_operator_tail (oname);
	       *rdcl = new_Op_decl (line, column, rtyp, oname, curr, fpars, bdy);
	       return (1);
	     };
	  return (0);
	};

static int is_rest_operator_head (string *oname, pragmat_list *curr, decl_list *fpars)
	{ if (is_token (OP_SYMBOL))
	     { int line, column;
	       *curr = read_and_reset_pragmats ();
	       should_be_operator_name (oname, rest_routine_head_sync_set);
	       line = this_line;
	       column = this_column;
	       should_be_formal_parameter_spec_pack (fpars);
	       if ((*fpars) -> size > 2) parser_error (line, column,
			"An operator can not have more than 2 parameters");
	       should_be_token (COLON_SYMBOL, ":", rest_routine_head_sync_set);
	       return (1);
	     };
	  return (0);
	};

static void should_be_operator_tail (string oname)
	{ string oname_tail;
	  int line, column;
	  should_be_token (ENDOP_SYMBOL, "ENDOP", rest_routine_tail_sync_set);
	  line = this_line;
	  column = this_column;
	  if (!is_operator_name (&oname_tail)) return;
	  if (oname == string_nil) return;
	  if (streq (oname, oname_tail)) return;
	  parser_error (line, column, "Operator name mismatch");
	};

static void may_be_routine_body (body *bdy)
	{ string sval;
	  if (is_token (INTERNAL_SYMBOL))
	     { should_be_text_denotation (&sval, rest_routine_tail_sync_set);
	       *bdy = new_Internal (sval);
	     }
	  else if (is_token (EXTERNAL_SYMBOL))
	     { should_be_text_denotation (&sval, rest_routine_tail_sync_set);
	       *bdy = new_External (sval);
	     }
	  else
	     { decl_list locals = new_decl_list ();
	       node_list rcode = new_node_list ();
	       if (is_paragraph (locals, rcode))
		  { if (is_token (PERIOD_SYMBOL))
		       should_be_refinement_sequence (locals, 1);
		  }
	       else
		  { int line = this_line;
	  	    int column = this_column;
		    app_node_list (rcode, new_Skip (line, column));
		  };
	       *bdy = new_Routine (locals, rcode);
	       if (analyzing_main) (*bdy) -> Routine.trc = 1;
	     };
	};

/*
   Rules for 'formal parameter spec pack'
         and 'formal parameter spec list'
*/
static void may_be_formal_parameter_spec_pack (decl_list *fpars)
	{ if (is_formal_parameter_spec_pack (fpars));
	  else *fpars = new_decl_list ();
	}

static symbol formal_parameter_spec_pack_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  COLON_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL, UNDEFINED
	};
static int is_formal_parameter_spec_pack (decl_list *fpars)
	{ if (is_token (OPEN_SYMBOL))
	     { *fpars = new_decl_list ();
	       should_be_formal_parameter_spec_list (*fpars);
	       should_be_token (CLOSE_SYMBOL, ")", formal_parameter_spec_pack_sync_set);
	       return (1);
	     };
	  return (0);
	};

static void should_be_formal_parameter_spec_pack (decl_list *fpars)
	{ if (is_formal_parameter_spec_pack (fpars)) return;
	  parser_error (this_line, this_column, "formal parameter pack expected");
	  skip_tokens (formal_parameter_spec_pack_sync_set);
	  *fpars = new_decl_list ();
	};

static int is_formal_parameter_spec_list (decl_list fpars)
	{ if (is_formal_parameter_spec (fpars))
	     { while (is_token (COMMA_SYMBOL))
		  should_be_formal_parameter_spec (fpars);
	       return (1);
	     };
	  return (0);
	};

static symbol formal_parameter_spec_list_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, COMMA_SYMBOL,
	  COLON_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL, UNDEFINED
	};
static void should_be_formal_parameter_spec_list (decl_list fpars)
	{ if (is_formal_parameter_spec_list (fpars)) return;
	  parser_error (this_line, this_column, "formal parameter expected");
	  skip_tokens (formal_parameter_spec_list_sync_set);
	};

/*
   k) formal parameter spec:
         formal declarer, parameter name list.

   FIRST (formal parameter spec) =
   FIRST (formal parameter) = INT | BOOL | REAL | TEXT | ROW | STRUCT | CAPITAL_NAME | PROC
*/
static int is_formal_parameter_spec (decl_list fpars)
	{ int column = this_column;
	  int line = this_line;
	  e_access facc;
	  string pname;
	  type ftyp;
	  if (is_formal_declarer (&ftyp, &facc))
	     { pragmat_list curr = read_and_reset_pragmats ();
	       should_be_small_name (&pname, formal_parameter_spec_list_sync_set);
	       app_decl_list (fpars, new_Formal (line, column, ftyp, pname, curr, facc));
	       while (next_token_is (SMALL_NAME) && is_token (COMMA_SYMBOL))
		  { pragmat_list curr = read_and_reset_pragmats ();
		    should_be_small_name (&pname, empty_set);	/* sic */
		    app_decl_list (fpars, new_Formal (line, column, attach_type (ftyp),
						      pname, curr, facc));
		  };
	       return (1);
	     };
	  return (0);
	};

static void should_be_formal_parameter_spec (decl_list fpars)
	{ if (is_formal_parameter_spec (fpars)) return;
	  parser_error (this_line, this_column, "formal parameter expected");
	  skip_tokens (formal_parameter_spec_list_sync_set);
	};

/*
   5: Refinements, paragraphs and units
  
   a) refinement:
 	 refinement name, colon token, paragraph option, period token.
  
   b) paragraph:
         paragraph, semicolon token, unit;
         unit.
  
   c) unit:
         basic declaration;
         expression.

   We have to remove the left recursion from rule b in the usual way.
   Note that we check the refinement header with lookahead.

   FIRST (refinement) = SMALL_NAME
   FIRST (paragraph) =
   FIRST (unit) = LET | INT | BOOL | REAL | TEXT | ROW | STRUCT | CAPITAL_NAME |
		  IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                  WHILE | REP | LEAVE | OPEN | SUB |
		  CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		  INTEGER_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER

   FOLLOW (refinement sequence) = ENDPACKET | ENDPROC | ENDOP | EOF
   For better error recovery, we are going to skip until we see
   one of the symbols in the follow set or until we see another
   refinement header (refinement name:)
*/
static void may_be_refinement_sequence (decl_list decls)
	{ while (1)
	     { int recovered = 0;
	       while (is_refinement (decls));
	       if (this_symbol == ENDPACKET_SYMBOL) return;
	       parser_error (this_line, this_column, "end of packet expected");
	       while (!recovered)
	          switch (this_symbol)
		     { case ENDPACKET_SYMBOL:
		       case EOF_SYMBOL: return;
		       case SMALL_NAME:
			  if (next_token_is (COLON_SYMBOL))
			     { recovered = 1;
			       break;
			     };
		       default: read_token ();
		     };
	     };
	};

static void should_be_refinement_sequence (decl_list decls, int routine)
	{ if (!is_refinement (decls))
	     parser_error (this_line, this_column, "refinement expected");
	  while (1)
	     { int recovered = 0;
	       while (is_refinement (decls));
	       if (routine && ((this_symbol == ENDPROC_SYMBOL) || (this_symbol == ENDOP_SYMBOL)))
		  return;
	       if (!routine && (this_symbol == EOF_SYMBOL)) return;
	       parser_error (this_line, this_column, "end of %s expected",
			     (routine)?"routine":"main program");
	       while (!recovered)
	          switch (this_symbol)
		     { case ENDPROC_SYMBOL:
		       case ENDOP_SYMBOL:
			  if (routine) return;
			  else read_token ();
			  break;
		       case EOF_SYMBOL: return;
		       case SMALL_NAME:
			  if (next_token_is (COLON_SYMBOL)) recovered = 1;
			  else read_token ();
			  break;
		       default: read_token ();
		     };
	     };
	};

static symbol refinement_sync_set[] =
	{ ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  SMALL_NAME, UNDEFINED
	};
static int is_refinement (decl_list decls)
	{ int line = this_line;
	  int column = this_column;
	  string ref_name;
	  if (next_token_is (COLON_SYMBOL) && is_small_name (&ref_name))
	     { pragmat_list curr = read_and_reset_pragmats ();
	       decl_list ldecls = new_decl_list ();
	       node_list paragraph;
	       decl ref;
	       should_be_token (COLON_SYMBOL, ":", empty_set);	/* sic */
	       may_be_paragraph (ldecls, &paragraph);
	       should_be_token (PERIOD_SYMBOL, ".", refinement_sync_set);
	       ref = new_Refinement (line, column, attach_type (t_unknown), ref_name, curr,
				     acc_any, ldecls, paragraph);
	       if (analyzing_main) ref -> Refinement.trc = 1;
	       app_decl_list (decls, ref);
	       return (1);
	     };
	  return (0);
	};

/*
   FOLLOW (paragraph) = PERIOD | ENDPROC | ENDOP | ELSE | ELIF | FI |
			CASE | OTHERWISE | ENDSELECT | UNTIL | ENDREP
*/
static symbol paragraph_sync_set[] =
	{ SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  ELSE_SYMBOL, ELIF_SYMBOL, ENDIF_SYMBOL,
	  CASE_SYMBOL, OTHERWISE_SYMBOL, ENDSELECT_SYMBOL,
	  UNTIL_SYMBOL, ENDREP_SYMBOL, UNDEFINED
	};
static void may_be_paragraph (decl_list decls, node_list *paragraph)
	{ int line = this_line;
	  int column = this_column;
	  *paragraph = new_node_list ();
	  if (is_paragraph (decls, *paragraph)) return;
	  app_node_list (*paragraph, new_Skip (line, column));
	};

static int is_paragraph (decl_list decls, node_list paragraph)
	{ if (is_unit (decls, paragraph))
	     { while (is_token (SEMICOLON_SYMBOL))
		  should_be_unit (decls, paragraph);
	       return (1);
	     };
	  return (0);
	};

static int is_unit (decl_list decls, node_list paragraph)
	{ node unit;
	  if (is_basic_declaration (decls, paragraph))
	     return (1);
	  else if (is_expression (decls, &unit))
	     { app_node_list (paragraph, unit);
	       return (1);
	     };
	  return (0);
	};

static void should_be_unit (decl_list decls, node_list paragraph)
	{ if (is_unit (decls, paragraph)) return;
	  parser_error (this_line, this_column, "unit expected");
	  skip_tokens (paragraph_sync_set);
	};

/*
   6 Declarations and declarers

   a) closed declaration:
	 procedure declaration;
	 operator declaration;
	 type declaration.

   b) basic declaration:
	 object declaration;
	 synonym declaration.

   FIRST (closed declaration) = PROC | OP | TYPE | INT | BOOL | REAL |
				TEXT | ROW | STRUCT | CAPITAL_NAME
   FIRST (basic declaration) = LET | INT | BOOL | REAL |
			       TEXT | ROW | STRUCT | CAPITAL_NAME
*/
static int is_basic_declaration (decl_list decls, node_list paragraph)
	{ return (is_object_declaration (decls, paragraph) ||
	          is_synonym_declaration (decls));
	};

/*
   6.1 Object declarations
  
   a) object declaration:
	 object declarer, object association list.

   Rules for 'object association list'

   LL(3) lookahead is needed at this point to distinguish between object
   declarations, abstractors and monadic formulas

   FIRST (object declaration) = INT | BOOL | REAL | TEXT | ROW | STRUCT |
				CAPITAL_NAME
*/
static int is_object_declaration (decl_list decls, node_list paragraph)
	{ type otyp;
	  e_access oacc;
	  if (object_declarer_allowed () && is_object_declarer (&otyp, &oacc))
	     { should_be_object_association_list (decls, paragraph, otyp, oacc);
	       detach_type (&otyp);
	       return (1);
	     };
	  return (0);
	};

static int object_declarer_allowed ()
	{ if (!this_token_is (CAPITAL_NAME)) return (1);
	  if (next_token_is (CONST_SYMBOL)) return (1);
	  if (next_token_is (VAR_SYMBOL)) return (1);
	  if (!next_token_is (SMALL_NAME)) return (0);
	  if (next2_token_is (INITIAL_SYMBOL)) return (1);
	  if (next2_token_is (COMMA_SYMBOL)) return (1);
	  return (0);
	};

static int is_object_association_list (decl_list decls, node_list paragraph,
				       type otyp, e_access oacc)
	{ if (is_object_association (decls, paragraph, otyp, oacc))
	     { while (is_token (COMMA_SYMBOL))
		  should_be_object_association (decls, paragraph, otyp, oacc);
	       return (1);
	     };
	  return (0);
	};

static symbol object_association_list_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_object_association_list (decl_list decls, node_list paragraph,
					       type otyp, e_access oacc)
	{ if (is_object_association_list (decls, paragraph, otyp, oacc)) return;
	  parser_error (this_line, this_column, "object association expected");
	  skip_tokens (object_association_list_sync_set);
	};

/*
   b) object association:
	 object name, initialization option.
  
   c) initialization:
         initial token, expression.

   FIRST (object association) = SMALL_NAME
   FIRST (initialization) = INITIAL_SYMBOL
*/
static int is_object_association (decl_list decls, node_list paragraph,
				  type otyp, e_access oacc)
	{ int column = this_column;
	  int line = this_line;
	  string oname;
	  if (is_small_name (&oname))
	     { pragmat_list curr = read_and_reset_pragmats ();
	       decl dcl = new_Object_decl (line, column, attach_type (otyp), oname, curr, oacc);
	       app_decl_list (decls, dcl);
	       if (oacc == acc_const) should_be_initialization (decls, paragraph, oname);
	       else may_be_initialization (decls, paragraph, oname);
	       return (1);
	     };
	  return (0);
	};

static symbol object_association_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_object_association (decl_list decls, node_list paragraph,
				          type otyp, e_access oacc)
	{ if (is_object_association (decls, paragraph, otyp, oacc)) return;
	  parser_error (this_line, this_column, "object association expected");
	  skip_tokens (object_association_sync_set);
	};

static void may_be_initialization (decl_list decls, node_list paragraph, string oname)
	{ int line = this_line;
	  int column = this_column;
	  if (is_initialization (decls, paragraph, oname)) return;
	  else app_node_list (paragraph, new_Skip (line, column));
	};

static int is_initialization (decl_list decls, node_list paragraph, string oname)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (INITIAL_SYMBOL))
	     { node init, rhs;
	       should_be_expression (decls, &rhs);
	       init = new_Init (line, column, oname, rhs);
	       app_node_list (paragraph, init);
	       return (1);
	     };
	  return (0);
	};

static void should_be_initialization (decl_list decls, node_list paragraph,
				       string oname)
	{ if (is_initialization (decls, paragraph, oname)) return;
	  parser_error (this_line, this_column, "initialization expected");
	  skip_tokens (object_association_sync_set);
	};

/*
   6.2 Synonym declarations

   a) synonym declaration:
	 let token, synonym association list.

   b) synonym association:
	 synonym value association;
	 synonym type association.

   c) synonym value assocation:
	 synonym value name, equal token, denoter.

   d) synonym type association:
	 synonym type name, equal token, type declarer.

   Rules for synonym association list

   FIRST (synonym declaration) = LET
   FIRST (synonym association) = SMALL_NAME | CAPITAL_NAME
   FIRST (synonym value association) = SMALL_NAME
   FIRST (synonym type association) = CAPITAL_NAME
*/
static int is_synonym_declaration (decl_list decls)
	{ if (is_token (LET_SYMBOL))
	     { should_be_synonym_association_list (decls);
	       return (1);
	     }
	  return (0);
	};

static int is_synonym_association_list (decl_list decls)
	{ if (is_synonym_association (decls))
	     { while (is_token (COMMA_SYMBOL))
		  should_be_synonym_association (decls);
	       return (1);
	     };
	  return (0);
	};

static symbol synonym_association_list_sync_set [] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};

static void should_be_synonym_association_list (decl_list decls)
	{ if (is_synonym_association_list (decls)) return;
	  parser_error (this_line, this_column, "synonym association expected");
	  skip_tokens (synonym_association_list_sync_set);
	};

static int is_synonym_association (decl_list decls)
	{ if (is_type_association (decls, 0)) return (1);
	  else if (is_synonym_value_association (decls)) return (1);
	  return (0);
	};

static symbol synonym_association_sync_set [] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};

static void should_be_synonym_association (decl_list decls)
	{ if (is_synonym_association (decls)) return;
	  parser_error (this_line, this_column, "synonym association expected");
	  skip_tokens (synonym_association_sync_set);
	};

static symbol synonym_value_association_sync_set [] =
	{ CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};

static int is_synonym_value_association (decl_list decls)
	{ int column = this_column;
	  int line = this_line;
	  string vname;
	  node rhs;
	  if (is_small_name (&vname))
	     { pragmat_list curr = read_and_reset_pragmats ();
	       decl ndcl;
	       should_be_token (EQUAL_SYMBOL, "=", synonym_value_association_sync_set);
	       should_be_expression (decls, &rhs);
	       ndcl = new_Synonym_value (line, column, attach_type (t_unknown), vname, curr, rhs);
	       app_decl_list (decls, ndcl);
	       return (1);
	     };
	  return (0);
	};

/*
   6.3 Type declarations

   a) type declaration:
	 type token, type association list.

   b) type association:
	 type name, equal token, type declarer.

   Since a type declaration is also a closed declaration,
   the reading of its first token is moved there. Furthermore
   the syntax of the synonym type association is indistinguishable
   from that of the type association. Hence, their parsing routines
   are unified.

   FIRST (type declaration) = TYPE
   FIRST (type association) = CAPITAL_NAME
*/
static int is_type_association_list (decl_list pdecls)
	{ if (is_type_association (pdecls, 1))
	     { while (is_token (COMMA_SYMBOL))
		  should_be_type_association (pdecls, 1);
	       return (1);
	     };
	  return (0);
	};

static symbol type_association_list_sync_set [] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, 
	  PROC_SYMBOL, OP_SYMBOL,
	  SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};

static void should_be_type_association_list (decl_list pdecls)
	{ if (is_type_association_list (pdecls)) return;
	  parser_error (this_line, this_column, "type association expected");
	  skip_tokens (type_association_list_sync_set);
	};

static symbol type_association_sync_set [] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, 
	  PROC_SYMBOL, OP_SYMBOL, SMALL_NAME,
	  COMMA_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};

static int is_type_association (decl_list pdecls, int strong)
	{ int column = this_column;
	  int line = this_line;
	  string bname;
	  if (is_capital_name (&bname))
	     { pragmat_list curr = read_and_reset_pragmats ();
	       type rhs_type;
	       decl ndcl;
	       should_be_token (EQUAL_SYMBOL, "=", type_association_sync_set);
	       should_be_type_declarer (&rhs_type);
	       if (strong) ndcl = new_Type (line, column, rhs_type, bname, curr);
	       else ndcl = new_Synonym_type (line, column, rhs_type, bname, curr);
	       app_decl_list (pdecls, ndcl);
	       return (1);
	     };
	  return (0);
	};

static void should_be_type_association (decl_list pdecls, int strong)
	{ if (is_type_association (pdecls, strong)) return;
	  parser_error (this_line, this_column, "type association expected");
	  skip_tokens (type_association_sync_set);
	};

/*
   6.4 Declarers

   a) formal declarer:
	 object declarer;
	 procedure declarer.

   b) procedure declarer:
	 result option, proc token, parameter declarer pack option.

   c) parameter declarer:
	 formal declarer.
  
   FIRST (formal declarer) = INT | BOOL | REAL | TEXT | ROW | STRUCT | CAPITAL_NAME | PROC

   Note that we have to left factorize for rule a.
*/

static int is_formal_declarer (type *t, e_access *acc)
	{ pdecl_list pdecls;
	  type rtype;
	  if (is_type_declarer (&rtype))
	     { if (is_token (PROC_SYMBOL))
		  { may_be_parameter_declarer_pack (&pdecls);
		    *t = new_Proc_type (pdecls, rtype);
		    *acc = acc_proc;
		  }
	       else
	          { *t = rtype;
		    may_be_access_declarer (acc);
		  };
	       return (1);
	     }
	  else if (is_token (PROC_SYMBOL))
	     { may_be_parameter_declarer_pack (&pdecls);
	       rtype = attach_type (t_void);
	       *t = new_Proc_type (pdecls, rtype);
	       *acc = acc_proc;
	       return (1);
	     };
	  return (0);
	};

static symbol formal_declarer_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, COMMA_SYMBOL,
	  COLON_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL, UNDEFINED
	};
static void should_be_formal_declarer (type *t, e_access *acc)
	{ if (is_formal_declarer (t, acc)) return;
	  parser_error (this_line, this_column, "formal declarer expected");
	  skip_tokens (formal_declarer_sync_set);
	};

/*
   Rules for parameter declarer pack and parameter declarer list
*/
static symbol parameter_declarer_pack_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  COLON_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL, UNDEFINED
	};
static void may_be_parameter_declarer_pack (pdecl_list *pdecls)
	{ *pdecls = new_pdecl_list ();
	  if (is_token (OPEN_SYMBOL))
	     { should_be_parameter_declarer_list (*pdecls);
	       should_be_token (CLOSE_SYMBOL, ")", parameter_declarer_pack_sync_set);
	     };
	};

static int is_parameter_declarer_list (pdecl_list pdecls)
	{ type ptype;
	  e_access pacc;
	  if (is_formal_declarer (&ptype, &pacc))
	     { pdecl pd = new_pdecl (ptype, pacc);
	       app_pdecl_list (pdecls, pd);
	       while (is_token (COMMA_SYMBOL))
		  { should_be_formal_declarer (&ptype, &pacc);
		    pd = new_pdecl (ptype, pacc);
	            app_pdecl_list (pdecls, pd);
		  };
	       return (1);
	     };
	  return (0);
	};

static symbol parameter_declarer_list_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER, CLOSE_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL,
	  COLON_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL, UNDEFINED
	};
static void should_be_parameter_declarer_list (pdecl_list pdecls)
	{ if (is_parameter_declarer_list (pdecls)) return;
	  parser_error (this_line, this_column, "parameter declarer list expected");
	  skip_tokens (parameter_declarer_list_sync_set);
	};

/*
   d) object declarer:
	 type declarer, access declarer option.

   e) type declarer:
         elementary type declarer;
	 composed type declarer.
  
   f) elementary type declarer:
	 concrete type declarer;
	 abstract type declarer.

   g) concrete type declarer:
         int token;
         bool token;
	 real token;
         text token.

   h) abstract type declarer:
	 type name.

   i) composed type declarer:
	 row declarer;
	 struct declarer.


   FIRST (object declarer) = 
   FIRST (field specification) =
   FIRST (type declarer) = INT | BOOL | REAL | TEXT | ROW | STRUCT | CAPITAL_NAME

   FOLLOW (type declarer) = PROC | OP | VAR | CONST | SMALL_NAME
*/
static int is_object_declarer (type *t, e_access *acc)
	{ if (is_type_declarer (t))
	     { may_be_access_declarer (acc);
	       return (1);
	     };
	  return (0);
	};

static int is_type_declarer (type *t)
	{ if (is_elementary_type_declarer (t)) return (1);
	  else if (is_composed_type_declarer (t)) return (1);
	  return (0);
	};

static symbol type_declarer_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, 
	  PROC_SYMBOL, OP_SYMBOL, SMALL_NAME,
	  COMMA_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_type_declarer (type *t)
	{ if (is_type_declarer (t)) return;
	  parser_error (this_line, this_column, "type declarer expected");
	  skip_tokens (type_declarer_sync_set);
	  *t = type_nil;
	};

static int is_elementary_type_declarer (type *t)
	{ if (is_concrete_type_declarer (t)) return (1);
	  return (is_abstract_type_declarer (t));
	};

static int is_concrete_type_declarer (type *t)
	{ if (is_token (INT_SYMBOL))
	     { *t = attach_type (t_int); return (1); }
	  else if (is_token (BOOL_SYMBOL))
	     { *t = attach_type (t_bool); return (1); }
	  else if (is_token (REAL_SYMBOL))
	     { *t = attach_type (t_real); return (1); }
	  else if (is_token (TEXT_SYMBOL))
	     { *t = attach_type (t_text); return (1); }
	  else if (is_token (INTERNAL_SYMBOL))
	     { int line = this_line;
	       int column = this_column;
	       string sval;
	       should_be_text_denotation (&sval, type_declarer_sync_set);
	       /*
		  Currently there are only three internal types we directly match
	       */
	       if (streq (sval, "addr")) *t = attach_type (t_addr);
	       else if (streq (sval, "file")) *t = attach_type (t_file);
	       else if (streq (sval, "niltype")) *t = attach_type (t_niltype);
	       else
		  { parser_error (line, column, "unknown internal type %s", sval);
		    *t = attach_type (t_error);
		  };
	       return (1);
	     };
	  return (0);
	};

static int is_abstract_type_declarer (type *t)
	{ int line = this_line;
	  int column = this_column;
	  string s;
	  if (is_capital_name (&s))
	     { *t = new_Proto_tname (line, column, s);
	       return (1);
	     };
	  return (0);
	};

static int is_composed_type_declarer (type *t)
	{ if (is_row_declarer (t)) return (1);
	  else if (is_struct_declarer (t)) return (1);
	  return (0);
	};

/*
   h) row declarer:
	 row token, cardinality, type declarer.

   i) cardinality:
	 integer denoter;		# substituted
	 synonym value name.

   FOLLOW (cardinality) =
   FIRST (type declarer)
*/
static int is_row_declarer (type *t)
	{ if (is_token (ROW_SYMBOL))
	     { node sz; 
	       type elt;
	       should_be_cardinality (&sz);
	       should_be_type_declarer (&elt);
	       *t = new_Proto_row (sz, elt);
	       return (1);
	     };
	  return (0);
	};

static int is_cardinality (node *crd)
	{ if (is_integer_denoter (crd)) return (1);
	  else if (is_identifier_application (crd)) return (1);
	  return (0);
	};

static void should_be_cardinality (node *crd)
	{ if (is_cardinality (crd)) return;
	  parser_error (this_line, this_column, "cardinality expected");
	  skip_tokens (type_declarer_sync_set);
	  *crd = node_nil;
	};

/*
   j) struct declarer:
	 struct token, field specification pack.

   rules for field_specifiation_list
*/
static symbol struct_declarer_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, 
	  PROC_SYMBOL, OP_SYMBOL, SMALL_NAME,
	  CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_struct_declarer (type *t)
	{ if (is_token (STRUCT_SYMBOL))
	     { field_list fields = new_field_list ();
	       should_be_token (OPEN_SYMBOL, "(", struct_declarer_sync_set);
	       should_be_field_specification_list (fields);
	       should_be_token (CLOSE_SYMBOL, ")", struct_declarer_sync_set);
	       *t = new_Struct (fields);
	       return (1);
	     };
	  return (0);
	};

static int is_field_specification_list (field_list fields)
	{ if (is_field_specification (fields))
	     { while (is_token (COMMA_SYMBOL))
		  should_be_field_specification (fields);
	       return (1);
	     };
	  return (0);
	};

static symbol field_specification_list_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, 
	  PROC_SYMBOL, OP_SYMBOL, SMALL_NAME,
	  CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_field_specification_list (field_list fields)
	{ if (is_field_specification_list (fields)) return;
	  parser_error (this_line, this_column, "field expected");
	  skip_tokens (field_specification_list_sync_set);
	};

/*
   k) field specification:
	 type declarer, field name list.
*/
static symbol field_specification_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, 
	  PROC_SYMBOL, OP_SYMBOL, SMALL_NAME,
	  COMMA_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_field_specification (field_list fields)
	{ type ftype;
	  if (is_type_declarer (&ftype))
	     { string fname;
	       should_be_small_name (&fname, field_specification_sync_set);
	       app_field_list (fields, new_field (ftype, fname));
	       while (next_token_is (SMALL_NAME) && is_token (COMMA_SYMBOL))
		  { should_be_small_name (&fname, empty_set); /* sic */
	            app_field_list (fields, new_field (attach_type (ftype), fname));
		  }; 
	       return (1);
	     };
	  return (0);
	};

static void should_be_field_specification (field_list fields)
	{ if (is_field_specification (fields)) return;
	  parser_error (this_line, this_column, "field expected");
	  skip_tokens (field_specification_sync_set);
	};

/*
   l) access declarer:
         const token;
         var token.

   FIRST (access declarer) = INT | VAR
*/
static void may_be_access_declarer (e_access *acc)
	{ if (is_access_declarer (acc)) return;
	  *acc = acc_const;
	};

static int is_access_declarer (e_access *acc)
	{ if (is_token (CONST_SYMBOL))
	     { *acc = acc_const;
	       return (1);
	     }
	  else if (is_token (VAR_SYMBOL))
	     { *acc = acc_var;
	       return (1);
	     };
	  return (0);
	};

/*
   7 Expressions

   Rules for expression list
  
   a) expression:
         assignment;
         priority ii formula.
  
   7.1 Assignments:
  
   a) assignment:
         destination, becomes token, source.
  
   b) destination:
         priority ii formula:
  
   c) source:
         priority ii formula.
  
   Again we have the problem of a common prefix. We solve it in
   the usual way.

   FIRST (expression_list) =
   FIRST (expression) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static symbol expression_list_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_expression_list (decl_list decls, node_list *exprs)
	{ node expr;
	  if (is_expression (decls, &expr))
	     { *exprs = new_node_list ();
	       app_node_list (*exprs, expr);
	       while (is_token (COMMA_SYMBOL))
		  { should_be_expression (decls, &expr);
		    app_node_list (*exprs, expr);
		  };
	       return (1);
	     };
	  return (0);
	};

static void should_be_expression_list (decl_list decls, node_list *exprs)
	{ if (is_expression_list (decls, exprs)) return;
	  parser_error (this_line, this_column, "expression expected");
	  skip_tokens (expression_list_sync_set);
	  *exprs = node_list_nil;
	};

static int is_expression (decl_list decls, node *expr)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_ii_formula (decls, expr))
	     { if (is_token (BECOMES_SYMBOL))
		  { node rhs;
		    should_be_priority_ii_formula (decls, &rhs);
		    *expr = new_Assign (line, column, *expr, rhs);
		    return (1);
		  };
	       return (1);
	     };
	  return (0);
	};

static symbol expression_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_expression (decl_list decls, node *expr)
	{ if (is_expression (decls, expr)) return;
	  parser_error (this_line, this_column, "expression expected");
	  skip_tokens (expression_sync_set);
	  *expr = node_nil;
	};

/*
   7.2 Formulas
  
   a) priority ii formula:
	 priority ii formula, priority ii operator, priority iii formula;
	 priority iii formula.

   b) priority ii operator:
	 capital name.

   FIRST (priority ii formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_ii_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_iii_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_ii_operator (&dop))
		  { should_be_priority_iii_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static symbol priority_ii_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_priority_ii_formula (decl_list decls, node *form)
	{ if (is_priority_ii_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 2 formula expected");
	  skip_tokens (priority_ii_formula_sync_set);
	  *form = node_nil;
	};

static int is_priority_ii_operator (string *dop)
	{ return (is_capital_name (dop));
	};

/*
   c) priority iii formula:
	 priority iii formula, priority iii operator, priority iv formula;
	 priority iv formula.

   d) priority iii operator:
	 or token;
	 xor token.

   FIRST (priority iii formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_iii_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_iv_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_iii_operator (&dop))
		  { should_be_priority_iv_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static void should_be_priority_iii_formula (decl_list decls, node *form)
	{ if (is_priority_iii_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 3 formula expected");
	  skip_tokens (priority_ii_formula_sync_set); /* is same as iii */
	  *form = node_nil;
	};

static int is_priority_iii_operator (string *dop)
	{ if (is_token (OR_SYMBOL))
	     { *dop = new_string ("OR"); return (1); }
	  else if (is_token (XOR_SYMBOL))
	     { *dop = new_string ("XOR"); return (1); }
	  return (0);
	};

/*
   e) priority iv formula:
	 priority iv formula, priority iv operator, priority v formula;
	 priority v formula.

   f) priority iv operator:
	 and token.

   FIRST (priority iv formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_iv_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_v_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_iv_operator (&dop))
		  { should_be_priority_v_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static symbol priority_iv_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, 
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_priority_iv_formula (decl_list decls, node *form)
	{ if (is_priority_iv_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 4 formula expected");
	  skip_tokens (priority_iv_formula_sync_set);
	  *form = node_nil;
	};

static int is_priority_iv_operator (string *dop)
	{ if (is_token (AND_SYMBOL))
	     { *dop = new_string ("AND"); return (1); }
	  return (0);
	};

/*
   g) priority v formula:
	 priority v formula, priority v operator, priority vi formula;
	 priority vi formula.

   h) priority v operator:
	 equal token;
	 not equal token.

   FIRST (priority v formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_v_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_vi_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_v_operator (&dop))
		  { should_be_priority_vi_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static symbol priority_v_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_priority_v_formula (decl_list decls, node *form)
	{ if (is_priority_v_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 5 formula expected");
	  skip_tokens (priority_v_formula_sync_set);
	  *form = node_nil;
	};

static int is_priority_v_operator (string *dop)
	{ if (is_token (EQUAL_SYMBOL))
	     { *dop = new_string ("="); return (1); }
	  else if (is_token (NOT_EQUAL_SYMBOL))
	     { *dop = new_string ("<>"); return (1); }
	  return (0);
	};

/*
   i) priority vi formula:
	 priority vi formula, priority vi operator, priority vii formula;
	 priority vii formula.

   j) priority vi operator:
	 less than token;
	 less equal token;
	 greater than token;
	 greater equal token.

   FIRST (priority vi formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_vi_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_vii_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_vi_operator (&dop))
		  { should_be_priority_vii_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static symbol priority_vi_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_priority_vi_formula (decl_list decls, node *form)
	{ if (is_priority_vi_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 6 formula expected");
	  skip_tokens (priority_vi_formula_sync_set);
	  *form = node_nil;
	};

static int is_priority_vi_operator (string *dop)
	{ if (is_token (LESS_THAN_SYMBOL))
	     { *dop = new_string ("<"); return (1); }
	  else if (is_token (LESS_EQUAL_SYMBOL))
	     { *dop = new_string ("<="); return (1); }
	  else if (is_token (GREATER_THAN_SYMBOL))
	     { *dop = new_string (">"); return (1); }
	  else if (is_token (GREATER_EQUAL_SYMBOL))
	     { *dop = new_string (">="); return (1); }
	  return (0);
	};

/*
   k) priority vii formula:
	 priority vii formula, priority vii operator, priority viii formula;
	 priority viii formula.

   l) priority vii operator:
	 plus token;
	 minus token.

   FIRST (priority vii formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_vii_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_viii_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_vii_operator (&dop))
		  { should_be_priority_viii_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static symbol priority_vii_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_priority_vii_formula (decl_list decls, node *form)
	{ if (is_priority_vii_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 7 formula expected");
	  skip_tokens (priority_vii_formula_sync_set);
	  *form = node_nil;
	};

static int is_priority_vii_operator (string *dop)
	{ if (is_token (PLUS_SYMBOL))
	     { *dop = new_string ("+"); return (1); }
	  else if (is_token (MINUS_SYMBOL))
	     { *dop = new_string ("-"); return (1); }
	  return (0);
	};


/*
   m) priority viii formula:
	 priority viii formula, priority viii operator, priority ix formula;
	 priority ix formula.

   n) priority viii operator:
	 asterix token;
	 divides token;
	 div token;
	 mod token.

   FIRST (priority viii formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_viii_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_priority_ix_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_viii_operator (&dop))
		  { should_be_priority_ix_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static void should_be_priority_viii_formula (decl_list decls, node *form)
	{ if (is_priority_viii_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 8 formula expected");
	  skip_tokens (priority_vii_formula_sync_set);	/* same as vii */
	  *form = node_nil;
	};

static int is_priority_viii_operator (string *dop)
	{ if (is_token (ASTERIX_SYMBOL))
	     { *dop = new_string ("*"); return (1); }
	  else if (is_token (DIVIDES_SYMBOL))
	     { *dop = new_string ("/"); return (1); }
	  else if (is_token (DIV_SYMBOL))
	     { *dop = new_string ("DIV"); return (1); }
	  else if (is_token (MOD_SYMBOL))
	     { *dop = new_string ("MOD"); return (1); }
	  return (0);
	};

/*
   o) priority ix formula:
	 priority ix formula, priority ix operator, monadic formula;
	 monadic formula.

   p) priority ix operator:
	 obelix token.

   FIRST (priority ix formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER
*/
static int is_priority_ix_formula (decl_list decls, node *form)
	{ int line = this_line;
	  int column = this_column;
	  if (is_monadic_formula (decls, form))
	     { string dop;
	       node arg2;
	       while (is_priority_ix_operator (&dop))
		  { should_be_monadic_formula (decls, &arg2);
		    *form = new_Dyop (line, column, dop, *form, arg2);
	          };
	       return (1);
	     };
	  return (0);
	};

static symbol priority_ix_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME, INTEGER_DENOTER, TRUE_SYMBOL,
	  FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_priority_ix_formula (decl_list decls, node *form)
	{ if (is_priority_ix_formula (decls, form)) return;
	  parser_error (this_line, this_column, "priority 9 formula expected");
	  skip_tokens (priority_ix_formula_sync_set);
	  *form = node_nil;
	};

static int is_priority_ix_operator (string *dop)
	{ if (is_token (OBELIX_SYMBOL))
	    { *dop = new_string ("**"); return (1); }
	  return (0);
	};

/*
   q) monadic formula:
	 monadic operator, monadic formula;
	 primary.

   FIRST (monadic formula) =
		IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		CONCR | NIL | PLUS | NOT | MINUS | SMALL_NAME |
		INT_DENOTER | TRUE | FALSE | REAL_DENOTER | TEXT_DENOTER

*/
static int is_monadic_formula (decl_list decls, node *mon)
	{ int line = this_line;
	  int column = this_column;
	  string mop;
	  if (is_monadic_operator (&mop))
	     { node arg;
	       should_be_monadic_formula (decls, &arg);
	       *mon = new_Monop (line, column, mop, arg);
	       return (1);
	     }
	  else if (is_primary (decls, mon)) return (1);
	  return (0);
	};

static symbol monadic_formula_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_monadic_formula (decl_list decls, node *mon)
	{ if (is_monadic_formula (decls, mon)) return;
	  parser_error (this_line, this_column, "monadic formula expected");
	  skip_tokens (monadic_formula_sync_set);
	  *mon = node_nil;
	};

/*
   r) monadic operator:
	 capital name;
	 plus token;
	 minus token;
	 not token.

   When checking for the capital name, check syntax collision with the abstractor
   Note order of LL(2) check
*/
static int is_monadic_operator (string *mop)
	{ if (!next_token_is (COLON_SYMBOL) && is_capital_name (mop))
	     return (1);
	  else if (is_token (PLUS_SYMBOL))
	     { *mop = new_string ("+"); return (1); }
	  else if (is_token (MINUS_SYMBOL))
	     { *mop = new_string ("-"); return (1); }
	  else if (is_token (NOT_SYMBOL))
	     { *mop = new_string ("NOT"); return (1); };
	  return (0);
	};

/*
   7.3 Primaries and calls
  
   a) primary:
	 conditional choice;
	 numerical choice;
         repetition;
	 terminator;
	 enclosed expression;
	 display;
	 subscription;
	 selection;
	 abstractor;
	 concretizer;
	 nil;
         call;
         denoter;
         identifier application.
  
   b) enclosed expression:
	 open token, expression, close token.

   c) display:
	 sub token, expression list, bus token.

   d) subscription:
	 primary, sub token, expression, bus token.

   e) selection:
	 primary, period token, field name.

   f) abstractor:
	 capital name, colon token, primary.

   g) concretizer:
	 concr token, primary.

   h) call:
         primary, actual parameter list pack option.
  
   i) actual parameter:
         expression.
  
   Again we have to leftfactorize

   FIRST (primary) = IF | SELECT | FOR | FROM | UPTO | DOWNTO |
                     WHILE | REP | LEAVE | OPEN | SUB | CAPITAL_NAME |
		     CONCR | NIL | SMALL_NAME | INT_DENOTER | TRUE | FALSE |
		     REAL_DENOTER | TEXT_DENOTER

   FIRST (enclosed expression) = OPEN
   FIRST (display) = SUB
   FIRST (abstractor) = CAPITAL_NAME
   FIRST (concretizer) = CONCR
   FIRST (nil) = NIL
   FIRST (call) = SMALL_NAME
   FIRST (actual parameter) = FIRST (expression) =
*/
static int is_primary (decl_list decls, node *prim)
	{ if (is_primary_prefix (decls, prim))
	     { while (is_rest_subscription (decls, prim) ||
		      is_rest_selection (prim) ||
		      is_rest_call (decls, prim));
	       return (1);
	     };
	  return (0);
	};

/* Note hier vergeet je de directe follow set */
static symbol primary_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  THEN_SYMBOL, ELIF_SYMBOL, ELSE_SYMBOL, ENDIF_SYMBOL,
	  CASE_SYMBOL, ENDSELECT_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_rest_subscription (decl_list decls, node *prim)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (SUB_SYMBOL))
	     { node index;
	       should_be_expression (decls, &index);
	       should_be_token (BUS_SYMBOL, "]", primary_sync_set);
	       *prim = new_Sub (line, column, *prim, index);
	       return (1);
	     };
	  return (0);
	};

static int is_rest_selection (node *prim)
	{ int line = this_line;
	  int column = this_column;
	  if (next_token_is (SMALL_NAME) && !next2_token_is (COLON_SYMBOL) &&
	      is_token (PERIOD_SYMBOL))
	     { string fname;
	       should_be_small_name (&fname, primary_sync_set);
	       *prim = new_Select (line, column, *prim, fname);
	       return (1);
	     };
	  return (0);
	};

static int is_rest_call (decl_list decls, node *prim)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (OPEN_SYMBOL))
	     { node_list args;
	       should_be_expression_list (decls, &args);
	       should_be_token (CLOSE_SYMBOL, ")", primary_sync_set);
	       *prim = new_Dyn_call (line, column, *prim, args);
	       return (1);
	     };
	  return (0);
	};

static int is_primary_prefix (decl_list decls, node *prim)
	{ if (is_conditional_choice (decls, prim)) return (1);
	  else if (is_numerical_choice (decls, prim)) return (1);
	  else if (is_repetition (decls, prim)) return (1);
	  else if (is_terminator (decls, prim)) return (1);
	  else if (is_enclosed_expression (decls, prim)) return (1);
	  else if (is_display (decls, prim)) return (1);
	  else if (is_abstractor (decls, prim)) return (1);
	  else if (is_concretizer (decls, prim)) return (1);
	  else if (is_nil (prim)) return (1);
	  else if (is_denoter (prim)) return (1);
	  else if (is_identifier_application (prim)) return (1);
	  return (0);
	};

static void should_be_primary_prefix (decl_list decls, node *prim)
	{ if (is_primary_prefix (decls, prim)) return;
	  parser_error (this_line, this_column, "primary expected");
	  skip_tokens (primary_sync_set);
	  *prim = node_nil;
	};

static int is_enclosed_expression (decl_list decls, node *prim)
	{ if (is_token (OPEN_SYMBOL))
	     { should_be_expression (decls, prim);
	       should_be_token (CLOSE_SYMBOL, ")", primary_sync_set);
	       return (1);
	     };
	  return (0);
	};

static int is_display (decl_list decls, node *prim)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (SUB_SYMBOL))
	     { node_list elts;
	       should_be_expression_list (decls, &elts);
	       should_be_token (BUS_SYMBOL, "]", primary_sync_set);
	       *prim = new_Display (line, column, elts);
	       return (1);
	     };
	  return (0);
	};

static int is_abstractor (decl_list decls, node *prim)
	{ string tname;
	  int line = this_line;
	  int column = this_column;
	  if (is_capital_name (&tname))
	     { node cprim;
	       should_be_token (COLON_SYMBOL, ":", primary_sync_set);
	       should_be_primary_prefix (decls, &cprim);
	       *prim = new_Abstr (line, column, tname, cprim);
	       return (1);
	     };
	  return (0);
	};

static int is_concretizer (decl_list decls, node *prim)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (CONCR_SYMBOL))
	     { node aprim;
	       should_be_primary_prefix (decls, &aprim);
	       *prim = new_Concr (line, column, aprim);
	       return (1);
	     };
	  return (0);
	};

static int is_nil (node *den)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (NIL_SYMBOL))
	     { *den = new_Nil (line, column);
	       return (1);
	     };
	  return (0);
	};

static int is_identifier_application (node *prim)
	{ int line = this_line;
	  int column = this_column;
	  string name;
	  if (is_small_name (&name))
	     { /*
		  Note that the identifier application can still be
		  a procedure or refinement application
	       */
	       *prim = new_Appl (line, column, name);
	       return (1);
	     };
	  return (0);
	};

/*
   9 Control constructs

   9.1 The Conditional Clause

   a) conditional choice:
	 if token, condition, then part, else part option, end if token.

   b) condition:
	 expression.

   c) then part:
	 then token, paragraph option.

   d) else part:
	 else token, paragraph option;
	 elif token, condition, then part, else part option.

   FIRST (conditional choice) = IF
   FIRST (condition) = FIRST (expression) = a lot
   FIRST (then part) = THEN
   FIRST (else part) = ELSE | ELIF
*/
static int is_conditional_choice (decl_list decls, node *cond)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (IF_SYMBOL))
	     { node enq;
	       node_list thenp, elsep;
	       should_be_condition (decls, &enq);
	       should_be_then_part (decls, &thenp);
	       may_be_else_part (decls, &elsep);
	       should_be_token (ENDIF_SYMBOL, "FI", primary_sync_set);
	       *cond = new_Cond (line, column, enq, thenp, elsep);
	       return (1);
	     };
	  return (0);
	};

static int is_condition (decl_list decls, node *expr)
	{ return (is_expression (decls, expr));
	};

static symbol condition_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  THEN_SYMBOL, REP_SYMBOL, CASE_SYMBOL, 
	  ENDIF_SYMBOL, ENDSELECT_SYMBOL, ENDREP_SYMBOL,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_condition (decl_list decls, node *expr)
	{ if (is_condition (decls, expr)) return;
	  parser_error (this_line, this_column, "condition expected");
	  skip_tokens (condition_sync_set);
	  *expr = node_nil;
	};

static int is_then_part (decl_list decls, node_list *thenp)
	{ if (is_token (THEN_SYMBOL))
	     { may_be_paragraph (decls, thenp);
	       return (1);
	     };
	  return (0);
	};

static symbol then_part_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  ELSE_SYMBOL, ELIF_SYMBOL, ENDIF_SYMBOL,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static void should_be_then_part (decl_list decls, node_list *thenp)
	{ if (is_then_part (decls, thenp)) return;
	  parser_error (this_line, this_column, "then part expected");
	  skip_tokens (then_part_sync_set);
	  *thenp = node_list_nil;
	};

static void may_be_else_part (decl_list decls, node_list *elsep)
	{ int line = this_line;
	  int column = this_column;
	  if (is_else_part (decls, elsep)) return;
	  *elsep = new_node_list ();
	  app_node_list (*elsep, new_Skip (line, column));
	};

static int is_else_part (decl_list decls, node_list *relsep)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (ELSE_SYMBOL))
	     { may_be_paragraph (decls, relsep);
	       return (1);
	     }
	  else if (is_token (ELIF_SYMBOL))
	     { node enq;
	       node_list thenp, elsep;
	       should_be_condition (decls, &enq);
	       should_be_then_part (decls, &thenp);
	       may_be_else_part (decls, &elsep);
	       *relsep = new_node_list ();
	       app_node_list (*relsep, new_Cond (line, column, enq, thenp, elsep));
	       return (1);
	     };
	  return (0);
	};

/*
   9.2 The Numerical Choice Clause

   a) numerical choice:
	 select token, expression, of token, case part sequence,
	    otherwise part option, end select token.

   b) case part:
	 case token, case label list, colon token, paragraph.

   c) case label:
	 integer denoter;		# substituted
	 synonym value name.

   d) otherwise part:
	 paragraph option.

   FIRST (numerical choice) = SELECT
   FIRST (case part) = CASE
   FIRST (case label) = INT_DENOTER | SMALL_NAME
   FIRST (otherwise part) = OTHERWISE
*/
static symbol numerical_choice_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  CASE_SYMBOL, OTHERWISE_SYMBOL, ENDSELECT_SYMBOL,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_numerical_choice (decl_list decls, node *num)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (SELECT_SYMBOL))
	     { node enq;
	       case_part_list parts;
	       node_list oth;
	       should_be_expression (decls, &enq);
	       should_be_token (OF_SYMBOL, "OF", numerical_choice_sync_set);
	       should_be_case_part_sequence (decls, &parts);
	       may_be_otherwise_part (decls, &oth);
	       should_be_token (ENDSELECT_SYMBOL, "ENDSELECT", primary_sync_set);
	       *num = new_Num (line, column, enq, parts, oth);
	       return (1);
	     };
	  return (0);
	};

static int is_case_part_sequence (decl_list decls, case_part_list *parts)
	{ case_part cp;
	  if (is_case_part (decls, &cp))
	     { *parts = new_case_part_list ();
	       app_case_part_list (*parts, cp);
	       while (is_case_part (decls, &cp))
		  app_case_part_list (*parts, cp);
	       return (1);
	     };
	  return (0);
	};

static void should_be_case_part_sequence (decl_list decls, case_part_list *parts)
	{ if (is_case_part_sequence (decls, parts)) return;
	  parser_error (this_line, this_column, "case parts expected");
	  skip_tokens (numerical_choice_sync_set);
	  *parts = case_part_list_nil;
	};

static int is_case_part (decl_list decls, case_part *cp)
	{ if (is_token (CASE_SYMBOL))
	     { node_list clabs;
	       node_list lcod;
	       should_be_case_label_list (&clabs);
	       should_be_token (COLON_SYMBOL, ":", primary_sync_set);
	       may_be_paragraph (decls, &lcod);
	       *cp = new_case_part (clabs, lcod);
	       return (1);
	     };
	  return (0);
	};

static int is_case_label_list (node_list *clabs)
	{ node clab;
	  if (is_case_label (&clab))
	     { *clabs = new_node_list ();
	       app_node_list (*clabs, clab);
	       while (is_token (COMMA_SYMBOL))
		  { should_be_case_label (&clab);
		    app_node_list (*clabs, clab);
		  };
	       return (1);
	     };
	  return (0);
	};

static void should_be_case_label_list (node_list *clabs)
	{ if (is_case_label_list (clabs)) return;
	  parser_error (this_line, this_column, "case labels expected");
	  skip_tokens (numerical_choice_sync_set);
	  *clabs = node_list_nil;
	};

static int is_case_label (node *clab)
	{ if (is_integer_denoter (clab)) return (1);
	  else if (is_identifier_application (clab)) return (1);
	  return (0);
	};

static void should_be_case_label (node *clab)
	{ if (is_case_label (clab)) return;
	  parser_error (this_line, this_column, "case label expected");
	  skip_tokens (numerical_choice_sync_set);
	  *clab = node_nil;
	};

static void may_be_otherwise_part (decl_list decls, node_list *others)
	{ if (is_token (OTHERWISE_SYMBOL))
	     { may_be_paragraph (decls, others);
	       return;
	     }
	  *others = new_node_list ();
	  app_node_list (*others, new_Skip (this_line, this_column));
	};

/*
   9.3 The Repetition
  
   a) repetition:
	 for part option, from part option, direction part option,
	    while part option, rep token, paragraph,
	    until part option, endrep token.
  
   b) for part:
	 for token, small name.

   c) from part:
	 from token, expression.

   d) direction part:
	 upto token, expression;
	 downto token, expression.

   e) while part:
         while token, condition.
  
   f) until part:
         until token, condition.

   FIRST (repetition) = FOR | FROM | UPTO | DOWNTO |
			WHILE | REP
   FIRST (for part) = FOR
   FIRST (from part) = FROM
   FIRST (direction part) = UPTO | DOWNTO
   FIRST (while part) = WHILE
   FIRST (until part) = UNTIL  
*/
static int is_repetition (decl_list decls, node *rep)
	{ int line = this_line;
	  int column = this_column;
	  string lvar;
	  node from, to, wenq, uenq;
	  int dir, had_fp, had_wp;
	  node_list body;

	  /* parse loop variable part */
	  had_fp = is_for_part (&lvar);
	  if (is_from_part (decls, &from)) had_fp = 1;
	  if (is_direction_part (decls, &dir, &to)) had_fp = 1;

	  /* parse while part */
	  had_wp = is_while_part (decls, &wenq);

	  /* parse rep token if present or required */
	  if (had_fp || had_wp)
	     should_be_token (REP_SYMBOL, "REP", primary_sync_set);
	  else if (!is_token (REP_SYMBOL))
	     { /* rfre_node (from);
	       rfre_node (to);
	       rfre_node (wenq); */
	       return (0);
	     };

	  /* parse loop paragraph and until part */
	  may_be_paragraph (decls, &body);
	  may_be_until_part (decls, &uenq);
	  should_be_token (ENDREP_SYMBOL, "ENDREP", primary_sync_set);

	  /* create node */
	  if (had_fp)
	     *rep = new_For (line, column, lvar, dir, from, to, wenq, body, uenq);
	  else *rep = new_While (line, column, wenq, body, uenq);
	  return (1);
	};

static symbol for_part_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_for_part (string *lvar)
	{ if (is_token (FOR_SYMBOL))
	     { should_be_small_name (lvar, for_part_sync_set);
	       return (1);
	     }
	  else *lvar = new_string ("");
	  return (0);
	};

static int is_from_part (decl_list decls, node *from)
	{ if (is_token (FROM_SYMBOL))
	     { should_be_expression (decls, from);
	       return (1);
	     };
	  *from = new_Default (this_line, this_column);
	  return (0);
	};

static int is_direction_part (decl_list decls, int *dir, node *to)
	{ if (is_token (UPTO_SYMBOL))
	     { *dir = 0;
	       should_be_expression (decls, to);
	       return (1);
	     }
	  else if (is_token (DOWNTO_SYMBOL))
	     { *dir = 1;
	       should_be_expression (decls, to);
	       return (1);
	     }
	  *dir = 0;
	  *to = new_Default (this_line, this_column);
	  return (0);
	};

static int is_while_part (decl_list decls, node *cond)
	{ if (is_token (WHILE_SYMBOL))
	     { should_be_condition (decls, cond);
	       return (1);
	     };
	  *cond = new_Default (this_line, this_column);
	  return (0);
	};

static void may_be_until_part (decl_list decls, node *cond)
	{ if (is_token (UNTIL_SYMBOL))
	     should_be_condition (decls, cond);
	  else *cond = new_Default (this_line, this_column);
	};

/*
   9.4 The Terminator
  
   a) terminator:
	 leave token, algorithm name, premature result option.

   b) premature result:
	 with token, expression.

   FIRST (terminator) = LEAVE
   FIRST (premature result) = WITH
*/
static symbol leave_sync_set[] =
	{ LET_SYMBOL, INT_SYMBOL, BOOL_SYMBOL, REAL_SYMBOL, TEXT_SYMBOL,
	  ROW_SYMBOL, STRUCT_SYMBOL, CAPITAL_NAME, IF_SYMBOL, SELECT_SYMBOL,
	  BECOMES_SYMBOL, OR_SYMBOL, XOR_SYMBOL, AND_SYMBOL, OBELIX_SYMBOL,
	  EQUAL_SYMBOL, NOT_EQUAL_SYMBOL, LESS_THAN_SYMBOL,
	  LESS_EQUAL_SYMBOL, GREATER_THAN_SYMBOL, GREATER_EQUAL_SYMBOL,
	  ASTERIX_SYMBOL, DIVIDES_SYMBOL, DIV_SYMBOL, MOD_SYMBOL,
	  FOR_SYMBOL, FROM_SYMBOL, UPTO_SYMBOL, DOWNTO_SYMBOL,
	  WHILE_SYMBOL, REP_SYMBOL, LEAVE_SYMBOL, OPEN_SYMBOL, OF_SYMBOL,
	  SUB_SYMBOL, CONCR_SYMBOL, NIL_SYMBOL, PLUS_SYMBOL,
	  MINUS_SYMBOL, NOT_SYMBOL, WITH_SYMBOL, SMALL_NAME,
	  INTEGER_DENOTER, TRUE_SYMBOL, FALSE_SYMBOL, REAL_DENOTER, TEXT_DENOTER,
	  COMMA_SYMBOL, BUS_SYMBOL, CLOSE_SYMBOL, SEMICOLON_SYMBOL, PERIOD_SYMBOL,
	  ENDPACKET_SYMBOL, ENDPROC_SYMBOL, ENDOP_SYMBOL, UNDEFINED
	};
static int is_terminator (decl_list decls, node *term)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (LEAVE_SYMBOL))
	     { string aname;
	       node result;
	       should_be_algorithm_name (&aname, leave_sync_set);
	       may_be_result_option (decls, &result);
	       *term = new_Leave (line, column, aname, result);
	       return (1);
	     };
	  return (0);
	};

static void may_be_result_option (decl_list decls, node *result)
	{ if (is_token (WITH_SYMBOL))
	     should_be_expression (decls, result);
	  else *result = new_Skip (this_line, this_column);
	};

/*
   8: Names
  
   a) export name:
	 small name;
	 capital name;
	 special name.

   FIRST (export name) = OR | XOR | AND | EQUAL | NOT_EQUAL |
			 LESS_THAN | LESS_EQUAL |
			 GREATER_THAN | GREATER_EQUAL |
			 PLUS | MINUS | ASTERIX | OBELIX |
			 DIV | MOD | DIVIDES | NOT |
			 CAPITAL_NAME | SMALL_NAME
*/
static int is_export_name (string *name)
	{ if (is_small_name (name)) return (1);
	  else if (is_capital_name (name)) return (1);
	  return (is_special_name (name));
	};

static void should_be_export_name (string *name, symbol *sync_set)
	{ if (is_export_name (name)) return;
	  parser_error (this_line, this_column, "export name expected");
	  skip_tokens (sync_set);
	  *name = string_nil;
	};

/*
   b) algorithm name:
	 small name;
	 capital name;
	 special name.

   FIRST (algorithm name) = OR | XOR | AND | EQUAL | NOT_EQUAL |
			    LESS_THAN | LESS_EQUAL |
			    GREATER_THAN | GREATER_EQUAL |
			    PLUS | MINUS | ASTERIX | OBELIX |
			    DIV | MOD | DIVIDES | NOT |
			    CAPITAL_NAME | SMALL_NAME
*/
static int is_algorithm_name (string *name)
	{ if (is_small_name (name)) return (1);
	  else if (is_capital_name (name)) return (1);
	  return (is_special_name (name));
	};

static void should_be_algorithm_name (string *name, symbol *sync_set)
	{ if (is_algorithm_name (name)) return;
	  parser_error (this_line, this_column, "algorithm name expected");
	  skip_tokens (sync_set);
	  *name = string_nil;
	};
  
/*
   f) operator name:
         capital name;
         special name.

   FIRST (operator name) = OR | XOR | AND | EQUAL | NOT_EQUAL |
			   LESS_THAN | LESS_EQUAL |
			   GREATER_THAN | GREATER_EQUAL |
			   PLUS | MINUS | ASTERIX | OBELIX |
			   DIV | MOD | DIVIDES | NOT |
			   CAPITAL_NAME

*/
static int is_operator_name (string *name)
	{ if (is_capital_name (name)) return (1);
	  return (is_special_name (name));
	};

static void should_be_operator_name (string *name, symbol *sync_symbols)
	{ if (is_operator_name (name)) return;
	  parser_error (this_line, this_column, "operator name expected");
	  skip_tokens (sync_symbols);
	};

/*
   g) special name:
         or token;
	 xor token;
         and token;
         equal token;
         not equal token;
         less than token;
         less equal token;
         greater than token;
         greater equal token;
         plus token;
         minus token;
         asterix token;
	 div token;
	 mod token;
         divides token;
	 obelix token;
         not token.

   FIRST (special name) = OR | XOR | AND | EQUAL | NOT_EQUAL |
			  LESS_THAN | LESS_EQUAL |
			  GREATER_THAN | GREATER_EQUAL |
			  PLUS | MINUS | ASTERIX | OBELIX |
			  DIV | MOD | DIVIDES | NOT
*/
static int is_special_name (string *name)
	{ if (is_token (OR_SYMBOL))
	     { *name = new_string ("OR"); return (1); }
	  else if (is_token (XOR_SYMBOL))
	     { *name = new_string ("XOR"); return (1); }
	  else if (is_token (AND_SYMBOL))
	     { *name = new_string ("AND"); return (1); }
	  else if (is_token (EQUAL_SYMBOL))
	     { *name = new_string ("="); return (1); }
	  else if (is_token (NOT_EQUAL_SYMBOL))
	     { *name = new_string ("<>"); return (1); }
	  else if (is_token (LESS_THAN_SYMBOL))
	     { *name = new_string ("<"); return (1); }
	  else if (is_token (LESS_EQUAL_SYMBOL))
	     { *name = new_string ("<="); return (1); }
	  else if (is_token (GREATER_THAN_SYMBOL))
	     { *name = new_string (">"); return (1); }
	  else if (is_token (GREATER_EQUAL_SYMBOL))
	     { *name = new_string (">="); return (1); }
	  else if (is_token (PLUS_SYMBOL))
	     { *name = new_string ("+"); return (1); }
	  else if (is_token (MINUS_SYMBOL))
	     { *name = new_string ("-"); return (1); }
	  else if (is_token (ASTERIX_SYMBOL))
	     { *name = new_string ("*"); return (1); }
	  else if (is_token (DIV_SYMBOL))
	     { *name = new_string ("DIV"); return (1); }
	  else if (is_token (MOD_SYMBOL))
	     { *name = new_string ("MOD"); return (1); }
	  else if (is_token (DIVIDES_SYMBOL))
	     { *name = new_string ("/"); return (1); }
	  else if (is_token (OBELIX_SYMBOL))
	     { *name = new_string ("**"); return (1); }
	  else if (is_token (NOT_SYMBOL))
	     { *name = new_string ("NOT"); return (1); };
	  return (0);
	};

/*
   9: Denoters
  
   a) denoter:
         integer denotation;
         boolean denotation;
	 real denotation;
         text denotation.
  
   d) boolean denotation:
         true symbol;
         false symbol.

   e) nil denoter:
	 nil symbol.

   FIRST (denoter) = INT_DENOTER | TRUE | FALSE | REAL_DENOTER |
		     TEXT_DENOTER
*/
static int is_denoter (node *den)
	{ if (is_integer_denoter (den)) return (1);
	  else if (is_boolean_denoter (den)) return (1);
	  else if (is_real_denoter (den)) return (1);
	  else if (is_text_denoter (den)) return (1);
	  return (0);
	};

static int is_integer_denoter (node *den)
	{ int nval = this_nval;
	  int line = this_line;
	  int column = this_column;
	  if (is_token (INTEGER_DENOTER))
	     { *den = new_Denoter (line, column, new_Ival (nval));
	       return (1);
	     };
	  return (0);
	};

static int is_boolean_denoter (node *den)
	{ int line = this_line;
	  int column = this_column;
	  if (is_token (TRUE_SYMBOL))
	     { *den = new_Denoter (line, column, new_Bval (1));
	       return (1);
	     }
	  else if (is_token (FALSE_SYMBOL))
	     { *den = new_Denoter (line, column, new_Bval (0));
	       return (1);
	     };
	  return (0);
	};

static int is_real_denoter (node *den)
	{ real rval = this_rval;
	  int line = this_line;
	  int column = this_column;
	  if (is_token (REAL_DENOTER))
	     { *den = new_Denoter (line, column, new_Rval (rval));
	       return (1);
	     };
	  return (0);
	};

static int is_text_denoter (node *den)
	{ string sval = this_sval;
	  int line = this_line;
	  int column = this_column;
	  if (is_token (TEXT_DENOTER))
	     { *den = new_Denoter (line, column, new_Tval (sval));
	       return (1);
	     };
	  return (0);
	};

