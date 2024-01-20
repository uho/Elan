/*
   File: ident.c
   Identifies declarations
   Checks for double declarations

   CVS ID: "$Id: ident.c,v 1.10 2011/08/13 15:58:54 marcs Exp $"
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

/* local includes */
#include "elan_ast.h"
#include "ast_utils.h"
#include "decl_tree.h"
#include "type_table.h"
#include "symbol_table.h"
#include "contsens.h"
#include "ident.h"

static void ident_error (int line, int column, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  dcg_error (0, "%s in file %s, line %d, col %d", buf, contsens_fname, line, column);
}

static void ident_error_in_node (node n, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  dcg_error (0, "%s in file %s, line %d, col %d", buf, contsens_fname, n -> line, n -> column);
}

static void decl_error (decl d, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  dcg_error (0, "%s in file %s, line %d, col %d", buf, contsens_fname, d -> line, d -> column);
};

/*
   Try identify refinement in current direct surrounding environment
*/
decl try_identify_refinement (string rname, decl_tree surr_env)
{ decl_list dl = lookup_decl_tree (surr_env, rname);
  int ix;
  if (dl == decl_list_nil) return (decl_nil);
  for (ix = 0; ix < dl -> size; ix++)
    { decl d = dl -> array[ix];
      if (d -> tag == TAGRefinement) return (d);
    };
  return (decl_nil);
}

/*
   Test conflict between decls with same tag
*/
static int decl_kinds_conflict (decl d1, decl d2)
{ switch (d1 -> tag)
    { case TAGFormal:
      case TAGObject_decl:
      case TAGRefinement:
      case TAGSynonym_value:
      case TAGSynonym_type:
      case TAGType: return (1);
      case TAGProc_decl: return (d2 -> tag != TAGProc_decl);
      case TAGOp_decl:	  return (d2 -> tag != TAGOp_decl);
      default: dcg_bad_tag (d1 -> tag, "decl_kinds_conflict");
    };
  return (0);
}

/*
   Check double declarations on basis of declaration kind (tag)
   The argument is a list of decls with the same tag.
*/
void pre_check_double_decls (decl_list decls)
	{ int ix;
	  for (ix = 0; ix < decls -> size; ix++)
	     { decl d = decls -> array[ix];
	       int iy;
	       for (iy = 0; iy < ix; iy++)
		  if (decl_kinds_conflict (d, decls -> array[iy]))
		     { decl_error (d, "multiple declaration of '%s'", d -> name);
		       break;
		     };
	     };
	};

/*
   Routines to solve conflict resulting from generics
*/
static int formal_has_same_type (decl fpar1, decl fpar2)
	{ return (equivalent_type (fpar1 -> typ, fpar2 -> typ) ||
		  feebly_coercable (fpar1 -> typ, fpar2 -> typ) ||
		  feebly_coercable (fpar2 -> typ, fpar1 -> typ));
	};

static int formals_have_same_type (decl_list fpars1, decl_list fpars2)
	{ int ix;
	  if (fpars1 -> size != fpars2 -> size) return (0);
	  for (ix = 0; ix < fpars1 -> size; ix++)
	     if (!formal_has_same_type (fpars1 -> array[ix], fpars2 -> array[ix]))
		return (0);
	  return (1);
	};

/*
   Test for conflicting decls in the current decl_list
*/
static int decls_conflict (decl d, decl d2)
	{ switch (d -> tag)
	     { case TAGFormal:
	       case TAGObject_decl:
	       case TAGRefinement:
	       case TAGSynonym_type:
	       case TAGType: return (1);
	       case TAGProc_decl:
		  if (d2 -> tag != TAGProc_decl) return (1);
		  else return (formals_have_same_type (d -> Proc_decl.fpars,
						       d2 -> Proc_decl.fpars));
	       case TAGOp_decl:
		  if (d2 -> tag != TAGOp_decl) return (1);
		  else return (formals_have_same_type (d -> Op_decl.fpars,
						       d2 -> Op_decl.fpars));
	       default: dcg_bad_tag (d -> tag, "decls_conflict");
	     };
	  return (0);
	};

/*
   Check double declarations when types have been resolved
*/
void check_double_decls (decl_list decls)
	{ int ix;
	  for (ix = 0; ix < decls -> size; ix++)
	     { decl d = decls -> array[ix];
	       int iy;
	       for (iy = 0; iy < ix; iy++)
		  if (decls_conflict (d, decls -> array[iy]))
		     { decl_error (d, "multiple declaration of '%s'", d -> name);
		       break;
		     };
	     };
	};

/*
   Export the declaration list belonging to tag into the
   extended scope. If a declaration already exists in the
   extended scope with the same profile, it is deleted.
   The exported one is then merged into the extended scope.
*/
static void export_into_extended_scope (decl_list ext_scope, decl def)
	{ int ix;
	  for (ix = 0; ix < ext_scope -> size; ix++)
	     { decl entry = ext_scope -> array[ix];
	       if (decls_conflict (entry, def))
		  { /* We must replace the old entry by the new one */
		    detach_decl (&entry);
		    ext_scope -> array[ix] = attach_decl (def);
		    contsens_warning (0, 0, "export of %s replaces previous definition",
				      def -> name);
		    return;
		  }
	     };

	  /* We definitely have a new entry for the scope */
	  app_decl_list (ext_scope, attach_decl (def));
	};

void export_decl_list (string tag, decl_list defs)
	{ decl_list ext_scope = try_make_extended_scope_entry (tag);
	  int ix;
	  for (ix = 0; ix < defs -> size; ix++)
	     export_into_extended_scope (ext_scope, defs -> array[ix]);
	};

/*
   Routines to solve generic identification
*/
static int formal_matches_actual (decl fpar, type atype)
	{ return (equivalent_type (fpar -> typ, atype) ||
		  feebly_coercable (atype, fpar -> typ) ||
		  feebly_coercable (fpar -> typ, atype));
	};

static int formals_match_actuals (decl_list fpars, type_list atypes)
	{ int ix;
	  if (fpars -> size != atypes -> size) return (0);
	  for (ix = 0; ix < fpars -> size; ix++)
	     if (!formal_matches_actual (fpars -> array[ix], atypes -> array[ix]))
		return (0);
	  return (1);
	};

static int formals_match_pdecls (decl_list fpars, pdecl_list parms)
	{ int ix;
	  if (fpars -> size != parms -> size) return (0);
	  for (ix = 0; ix < fpars -> size; ix++)
	     if (!formal_matches_actual (fpars -> array[ix], parms -> array[ix] -> ptype))
		return (0);
	  return (1);
	};

static int decl_matches_procedure_type (decl d, type dtype)
	{ switch (d -> tag)
	     { case TAGFormal: return (actual_matches_formal_type (dtype, d -> typ));
	       case TAGProc_decl:
		  { decl_list fpars = d -> Proc_decl.fpars;
		    if (!formals_match_pdecls (fpars, dtype -> Proc_type.parms))
		       return (0);
		    else if (dtype -> Proc_type.rtype -> tag == TAGUnknown)
		       return (1);
		    else return (actual_matches_formal_type (dtype -> Proc_type.rtype, d -> typ));
		  };
	       case TAGObject_decl:
	       case TAGRefinement:
	       case TAGSynonym_value: break;
	       default: dcg_bad_tag (d -> tag, "decl_matches_procedure_type");
	     };
	  return (0);
	};

field identify_selector (string selector, node n, type strtype)
	{ field_list flds;
	  int ix;
	  if (strtype -> tag != TAGStruct)
	     dcg_internal_error ("identify_selector");
	  flds = strtype -> Struct.flds;
	  for (ix = 0; ix < flds -> size; ix++)
	     { field fld = flds -> array[ix];
	       if (streq (fld -> fname, selector))
		  return (fld);
	     };
	  ident_error_in_node (n, "can't identify selector %s", selector);
	  return (field_nil);
	};
	
/*
   Identify single small name in current environment
   The result may be an object, synonym value or procedure
*/
static decl identify_generic_decl (symbol_stack stack, string tag, node n, type *rtype)
	{ type_list tl = type_list_nil;
	  while (stack != symbol_stack_nil)
	     { decl_list decls = stack -> decls;
	       int ix;
	       if (decls -> size) tl = init_type_list (decls -> size);
	       for (ix = 0; ix < decls -> size; ix++)
		  { decl d = decls -> array[ix];
		    switch (d -> tag)
		       { case TAGProc_decl: app_type_list (tl, type_from_decl (d)); break;
		         case TAGFormal:
			 case TAGObject_decl:
			 case TAGSynonym_value: app_type_list (tl, d -> typ);
		         case TAGRefinement: break;
			 default: dcg_bad_tag (d -> tag, "identify_generic_decl");
		       };
		  };
	       if (tl != type_list_nil)
		  { *rtype = new_Unresolved (tl);
		    return (decl_nil);
		  };
	       stack = stack -> tail;
	     };
	  ident_error_in_node (n, "can't identify small name %s", tag);
	  return (decl_nil);
	};

static decl identify_procedure_decl (symbol_stack stack, string tag, node n, type dtype,
				     type *rtype)
	{ decl found = decl_nil;
	  while (stack != symbol_stack_nil)
	     { decl_list decls = stack -> decls;
	       int ix;
	       for (ix = 0; ix < decls -> size; ix++)
		  { decl d = decls -> array[ix];
		    if (!decl_matches_procedure_type (d, dtype)) continue;
		    if (found != decl_nil)
		       { ident_error_in_node (n, "multiple identification of procedure %s", tag);
			 return (decl_nil);
		       };
		    found = d;
		  };
	       if (found)
		  { *rtype = found -> typ;
		    return (found);
		  };
	       stack = stack -> tail;
	     };
	  ident_error_in_node (n, "can't identify procedure %s", tag);
	  return (decl_nil);
	};

static decl identify_object_or_value (symbol_stack stack, string tag, node n, type *rtype)
	{ decl found = decl_nil;
	  while (stack != symbol_stack_nil)
	     { decl_list decls = stack -> decls;
	       int ix;
	       for (ix = 0; ix < decls -> size; ix++)
		  { decl d = decls -> array[ix];
		    switch (d -> tag)
		       { case TAGObject_decl:
			 case TAGFormal:
			 case TAGSynonym_value: break;
			 case TAGProc_decl:
			    if ((d -> tag == TAGProc_decl) && d -> Proc_decl.fpars -> size)
			       continue;
			    break;
			 case TAGRefinement: continue;
			 default: dcg_bad_tag (d -> tag, "identify_object_or_value");
		       };
		    if (found != decl_nil)
		       { ident_error_in_node (n, "multiple identification of procedure %s", tag);
			 return (decl_nil);
		       };
		    found = d;
		  };
	       if (found)
		  { *rtype = found -> typ;
		    return (found);
		  };
	       stack = stack -> tail;
	     };
	  ident_error_in_node (n, "can't identify small name %s", tag);
	  return (decl_nil);
	};

decl identify_small_name (string tag, node n, type dtype, type *rtype)
	{ symbol_stack stack = lookup_symbol_stack (tag);
	  if (stack == symbol_stack_nil)
	     { ident_error_in_node (n, "can't identify small name %s", tag);
	       *rtype = t_error;
	       return (decl_nil);
	     };

	  /* We have identified at least one name */
	  if (dtype -> tag == TAGAny_non_void)
	     return (identify_generic_decl (stack, tag, n, rtype));
	  else if (dtype -> tag == TAGProc_type)
	     return (identify_procedure_decl (stack, tag, n, dtype, rtype));
	  else return (identify_object_or_value (stack, tag, n, rtype));
	};

/*
   Identify synonym value in current environment
   Synonym values may not conflict with refinements, procedures and objects
*/
decl identify_synonym_value (string tag, node n)
	{ decl_list decls = lookup_symbol (tag);
	  decl d;
	  if (decls == decl_list_nil)
	     { ident_error_in_node (n, "can't identify synonym value %s", tag);
	       return (decl_nil);
	     };
	  d = decls -> array[0];
	  switch (d -> tag)
	     { case TAGObject_decl:
	       case TAGFormal:
		  ident_error_in_node (n, "can't coerce object %s into a denoter", tag);
		  break;
	       case TAGProc_decl:
		  ident_error_in_node (n, "can't coerce proc %s into a denoter", tag);
		  break;
	       case TAGSynonym_value: return (d);
	       case TAGRefinement:
		  ident_error_in_node (n, "can't coerce refinement %s into a denoter", tag);
		  break;
	       default: dcg_bad_tag (d -> tag, "identify_synonym_value");
	     };
	  return (decl_nil);
	};

/*
   Identify type name in current environment
   Types may not conflict with operators
*/
decl identify_type_name (string tag, int line, int column)
	{ decl_list decls = lookup_symbol (tag);
	  decl d;
	  if (decls == decl_list_nil)
	     { ident_error (line, column, "can't identify type %s", tag);
	       return (decl_nil);
	     };
	  d = decls -> array[0];
	  switch (d -> tag)
	     { case TAGSynonym_type:
	       case TAGType: return (d);
	       case TAGOp_decl:
		  ident_error (line, column, "can't coerce operator %s into a type", tag);
		  break;
	       default: dcg_bad_tag (d -> tag, "identify_type_name");
	     };
	  return (decl_nil);
	};

/*
   Identify object in current decl_list
   Objects may not conflict with refinements and procedures
*/
decl identify_object_decl (string tag, node n)
	{ decl_list decls = lookup_symbol (tag);
	  decl d;
	  if (decls == decl_list_nil)
	     { ident_error_in_node (n, "can't identify object %s", tag);
	       return (decl_nil);
	     };
	  d = decls -> array[0];
	  switch (d -> tag)
	     { case TAGObject_decl:
	       case TAGFormal: return (d);
	       case TAGProc_decl:
		  ident_error_in_node (n, "can't coerce proc %s into an object", tag);
		  break;
	       case TAGSynonym_value:
		  ident_error_in_node (n, "can't coerce denoter %s into an object", tag);
		  break;
	       case TAGRefinement:
		  ident_error_in_node (n, "can't coerce refinement %s into an object", tag);
		  break;
	       default: dcg_bad_tag (d -> tag, "identify_object_decl");
	     };
	  return (decl_nil);
	};

/*
   Identify procedure
   Update error recovery wanneer er precies een proc is die matcht
   Procedures may not conflict with refinements or objects
*/
decl identify_proc_decl (node call, type_list atypes)
	{ string pname = call -> Call.pname;
	  symbol_stack stack = lookup_symbol_stack (pname);
	  int ix;
	  if (stack == symbol_stack_nil)
	     { ident_error_in_node (call, "can't identify proc %s", pname);
	       return (decl_nil);
	     };
	  for (ix = 0; ix < atypes -> size; ix++)
	     if (equivalent_type (atypes -> array[ix], t_error))
		return (decl_nil);
	  while (stack != symbol_stack_nil)
	     { decl_list decls = stack -> decls;
	       for (ix = 0; ix < decls -> size; ix++)
		  { decl d = decls -> array[ix];
		    switch (d -> tag)
		       { case TAGObject_decl:
			 case TAGFormal:
			    { ident_error_in_node (call,
				"can't coerce object %s into a proc", pname);
			      return (decl_nil);
			    };
			 case TAGProc_decl:
			    { decl_list fpars = d -> Proc_decl.fpars;
			      if (formals_match_actuals (fpars, atypes))
				 return (d);
			    }; break;
			 case TAGRefinement:
			    { ident_error_in_node (call,
				"can't coerce refinement %s into a proc", pname);
			      return (decl_nil);
			    };
			 default: dcg_bad_tag (d -> tag, "identify_proc_decl");
		       };
		  };
	       stack = stack -> tail;
	     };
	  ident_error_in_node (call, "can't identify proc %s", pname);
	  return (decl_nil);
	};

/*
   Identify monadic operator
*/
decl identify_monop_decl (node monop, type atype)
	{ string mop = monop -> Monop.mop;
	  symbol_stack stack = lookup_symbol_stack (mop);
	  if (stack == symbol_stack_nil)
	     { ident_error_in_node (monop, "can't identify monadic operator %s", mop);
	       return (decl_nil);
	     };
	  if (equivalent_type (atype, t_error))
	     return (decl_nil);
	  while (stack != symbol_stack_nil)
	     { decl_list decls = stack -> decls;
	       int ix;
	       for (ix = 0; ix < decls -> size; ix++)
		  { decl d = decls -> array[ix];
		    switch (d -> tag)
		       { case TAGOp_decl:
			    if (d -> Op_decl.fpars -> size == 1)
			       { decl fpar = d -> Op_decl.fpars -> array[0];
			         if (formal_matches_actual (fpar, atype))
				    return (d);
			       }; break;
			 case TAGType:
			 case TAGSynonym_type:
			    { ident_error_in_node (monop,
				"can't coerce type %s into a monadic operator", mop);
			      return (decl_nil);
			    };
			 default: dcg_bad_tag (d -> tag, "identify_monop_decl");
		       };
		  };
	       stack = stack -> tail;
	     };
	  ident_error_in_node (monop, "can't identify monadic operator %s", mop);
	  return (decl_nil);
	};

/*
   Identify dyadic operator
*/
decl identify_dyop_decl (node dyop, type atype1, type atype2)
	{ string dop = dyop -> Dyop.dop;
	  symbol_stack stack = lookup_symbol_stack (dop);
	  if (stack == symbol_stack_nil)
	     { ident_error_in_node (dyop, "can't identify dyadic operator %s", dop);
	       return (decl_nil);
	     };
	  if (equivalent_type (atype1, t_error) || equivalent_type (atype2, t_error))
	     return (decl_nil);
	  while (stack != symbol_stack_nil)
	     { decl_list decls = stack -> decls;
	       int ix;
	       for (ix = 0; ix < decls -> size; ix++)
		  { decl d = decls -> array[ix];
		    switch (d -> tag)
		       { case TAGOp_decl:
			    if (d -> Op_decl.fpars -> size == 2)
			       { decl fpar1 = d -> Op_decl.fpars -> array[0];
			         decl fpar2 = d -> Op_decl.fpars -> array[1];
			         if (formal_matches_actual (fpar1, atype1) &&
			             formal_matches_actual (fpar2, atype2))
			            return (d);
			       }; break;
			 case TAGType:
			 case TAGSynonym_type:
			    { ident_error_in_node (dyop,
				"can't coerce type %s into a dyadic operator", dop);
			      return (decl_nil);
			    };
			 default:  dcg_bad_tag (d -> tag, "identify_dyop_decl");
		       };
		  };
	       stack = stack -> tail;
	     };
	  ident_error_in_node (dyop, "can't identify dyadic operator %s", dop);
	  return (decl_nil);
	};

