/*
   File: contsens.c
   Does context dependent analysis

   CVS ID: "$Id: contsens.c,v 1.13 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "ast_utils.h"
#include "options.h"
#include "decl_tree.h"
#include "type_table.h"
#include "symbol_table.h"
#include "ident.h"
#include "checker.h"
#include "evaluate.h"
#include "pre_checker.h"

/* error administration */
string contsens_fname;
void contsens_error (int line, int column, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  if (!line && !column) dcg_error (0, "%s in file %s", buf, contsens_fname);
  else dcg_error (0, "%s in file %s, line %d, col %d", buf, contsens_fname, line, column);
}

void contsens_warning (int line, int column, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  if (!line && !column) dcg_warning (0, "%s in file %s", buf, contsens_fname);
  else dcg_warning (0, "%s in file %s, line %d, col %d", buf, contsens_fname, line, column);
}

/*
   Various recursive checks and actions on the local
   packet or routine declaration tree.
*/
static void push_local_declarations (decl_tree t)
{ if (t == decl_tree_nil) return;
  push_decls_into_symbol_table (t -> tag, t -> decls);
  push_local_declarations (t -> left);
  push_local_declarations (t -> right);
}

static void evaluate_synonym_values_in_tree (decl_tree t)
{ if (t == decl_tree_nil) return;
  evaluate_synonym_values_in_tree (t -> left);
  evaluate_synonym_values (t -> decls);
  evaluate_synonym_values_in_tree (t -> right);
}

static void unify_decl_types_in_tree (decl_tree t)
{ if (t == decl_tree_nil) return;
  unify_decl_types_in_tree (t -> left);
  unify_decl_types (t -> decls);
  unify_decl_types_in_tree (t -> right);
}

static void check_double_decls_in_tree (decl_tree t)
{ if (t == decl_tree_nil) return;
  check_double_decls_in_tree (t -> left);
  check_double_decls (t -> decls);
  check_double_decls_in_tree (t -> right);
}

static void pop_local_declarations (decl_tree t)
{ if (t == decl_tree_nil) return;
  pop_decls_from_symbol_table (t -> tag);
  pop_local_declarations (t -> left);
  pop_local_declarations (t -> right);
}

/*
   Export the defines list (if present) into the extended scope
*/
static int is_exportable (decl d)
{ switch (d -> tag)
    { case TAGSynonym_value:
      case TAGSynonym_type:
      case TAGType:
      case TAGProc_decl:
      case TAGOp_decl: return (1);
      case TAGRefinement: return (0);
      case TAGObject_decl:
	return (d -> Object_decl.acc == acc_const);
      default: dcg_bad_tag (d -> tag, "is_exportable");
    };
  return (0);
}

static void export_all_defs (decl_tree t)
{ decl_list new;
  int ix;
  if (t == decl_tree_nil) return;
  export_all_defs (t -> left);
  new = new_decl_list ();
  for (ix = 0; ix < t -> decls -> size; ix++)
    { decl d = t -> decls -> array[ix];
      if (is_exportable (d))
	app_decl_list (new, attach_decl (d));
    };
  if (new -> size) export_decl_list (t -> tag, new);
  detach_decl_list (&new);
  export_all_defs (t -> right);
}

#define def_error1 "no matching declaration of %s in defines list"
#define def_error2 "can't export %s into extended scope"
static void export_defines (string_list defs, decl_tree tree)
{ if (!defs -> size) export_all_defs (tree);
  else
    { int ix;
      for (ix = 0; ix < defs -> size; ix++)
	{ string tag = defs -> array[ix];
	  decl_list decls = lookup_decl_tree (tree, tag);
	  decl_list new;
	  int iy;
	  if (decls == decl_list_nil)
	    { contsens_error (0, 0, def_error1, tag);
	      continue;
	    };
	  new = new_decl_list ();
	  for (iy = 0; iy < decls -> size; iy++)
	    { decl d = decls -> array[iy];
	      if (is_exportable (d))
	        app_decl_list (new, attach_decl (d));
	    };
	  if (new -> size) export_decl_list (tag, new);
	  else contsens_error (0, 0, def_error2, tag);
	};
    };
}

/*
   Check PROC and OP declarations
*/
static void check_routine_body (decl pd, body bdy)
{ e_access racc;
  type rtype;
  if (bdy -> tag != TAGRoutine) return;
  push_local_declarations (bdy -> Routine.rdecls);
  evaluate_synonym_values_in_tree (bdy -> Routine.rdecls);
  evaluate_type_declarations (bdy -> Routine.locals);
  unify_decl_types_in_tree (bdy -> Routine.rdecls);
  check_double_decls_in_tree (bdy -> Routine.rdecls);
  check_paragraph (bdy -> Routine.rcode, pd -> typ, acc_const, &rtype, &racc);
  pop_local_declarations (bdy -> Routine.rdecls);
}

static void check_routine_declarations (decl_list pdecls)
{ int ix;
  for (ix = 0; ix < pdecls -> size; ix++)
    { decl pd = pdecls -> array[ix];
      switch (pd -> tag)
	{ case TAGProc_decl:  check_routine_body (pd, pd -> Proc_decl.bdy); break;
	  case TAGOp_decl: 	check_routine_body (pd, pd -> Op_decl.bdy); break;
	  case TAGObject_decl:
	  case TAGRefinement:
	  case TAGSynonym_value:
	  case TAGSynonym_type:
	  case TAGType: break;
	  default: dcg_bad_tag (pd -> tag, "check_routine_declarations");
        };
    };
}

/*
   Check a packet
*/
static void check_packet (packet pk)
{ e_access racc;
  type rtype;
  if (strlen (pk -> pname))
    dcg_warning (0, "   checking packet %s...", pk -> pname);
  else dcg_warning (0, "   checking main program...");
  contsens_fname = attach_string (pk -> fname);
  pre_check_packet (pk);
  dcg_panic_if_errors ();
  push_local_declarations (pk -> pkdecls);
  evaluate_synonym_values_in_tree (pk -> pkdecls);
  evaluate_type_declarations (pk -> pdecls);
  unify_decl_types_in_tree (pk -> pkdecls);
  check_double_decls_in_tree (pk -> pkdecls);
  check_paragraph (pk -> pcode, t_void, acc_const, &rtype, &racc);
  check_routine_declarations (pk -> pdecls);
  pop_local_declarations (pk -> pkdecls);
  if (strlen (pk -> pname))
    export_defines (pk -> pdefines, pk -> pkdecls);
  if (dump_checker)
    { dcg_wlog ("   Dump of packet %s after checking:", pk -> pname);
      pp_packet (stderr, pk);
      dcg_wlog ("");
    };
  detach_string (&contsens_fname);
  dcg_panic_if_errors ();
}

/*
   Check each packet (including the main program) in USE order
*/
void context_sensitive_analysis (packet_list all_pks)
{ int ix;
  dcg_warning (0, "   checking the program...");
  init_symbol_table ();
  for (ix = 0; ix < all_pks -> size; ix++)
    check_packet (all_pks -> array[ix]);
}
