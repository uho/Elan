/*
   File: ident.h
   Identifies declarations, checks for double declarations

   CVS ID: "$Id: ident.h,v 1.5 2005/05/19 15:48:41 marcs Exp $"
*/
#ifndef IncIdent
#define IncIdent

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"
#include "decl_tree.h"

/* exported code */
decl try_identify_refinement (string rname, decl_tree surr_env);
void pre_check_double_decls (decl_list decls);
void check_double_decls (decl_list decls);
void export_decl_list (string tag, decl_list defs);

/* exported identification code */
field identify_selector (string selector, node n, type strtype);
decl identify_small_name (string tag, node n, type dtype, type *rtype);
decl identify_synonym_value (string tag, node n);
decl identify_object_decl (string tag, node n);
decl identify_type_name (string tag, int line, int column);
decl identify_proc_decl (node call, type_list atypes);
decl identify_monop_decl (node monop, type atype);
decl identify_dyop_decl (node dyop, type atype1, type atype2);

#endif /* IncIdent */
