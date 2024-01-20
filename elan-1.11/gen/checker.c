/*
   File: checker.c
   Does identification, type and access checking 

   CVS ID: "$Id: checker.c,v 1.9 2011/08/13 15:58:54 marcs Exp $"
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
#include "type_table.h"
#include "evaluate.h"
#include "ident.h"
#include "checker.h"
#include "contsens.h"

/*
   Error reporting
*/
static void checker_error (int line, int column, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  if (!line && !column) dcg_error (0, "%s in file %s", buf, contsens_fname);
  else dcg_error (0, "%s in file %s, line %d, col %d", buf, contsens_fname, line, column);
}

static void typing_error (node n, type dtype, type otype, type *rtype)
{ string otext = convert_type_text (otype);
  string dtext = convert_type_text (dtype);
  checker_error (n -> line, n -> column, "can't convert type %s to type %s", otext, dtext);
  *rtype = t_error;
}

/*
   Type coercion
*/
static void try_coerce_access (node n, e_access dacc, e_access oacc, e_access *racc);
static void try_coerce_type (node n, type dtype, type otype, type *rtype)
	{ pdecl_list oparms, dparms;
	  type ortype, drtype;

	  /* Equal types are ok */
	  if (equal_type (dtype, otype)) { *rtype = otype; return; };

	  /* Check for error propagation */
	  if (equal_type (dtype, t_error)) { *rtype = t_error; return; };
	  if (equal_type (otype, t_error)) { *rtype = t_error; return; };

	  /* Check for any generic demanded types */
	  if (equal_type (dtype, t_any_non_void))
	     { if (equal_type (otype, t_void))
	          typing_error (n, dtype, otype, rtype);
	       else *rtype = otype;
	       return;
	     };
	  if (equal_type (dtype, t_any_object))
	     { if (equal_type (otype, t_void))
		  typing_error (n, dtype, otype, rtype);
	       else *rtype = otype;
	       return;
	     };
	  if (equal_type (dtype, t_any_row))
	     { if (otype -> tag != TAGRow)
	          typing_error (n, dtype, otype, rtype);
	       else *rtype = otype;
	       return;
	     };
	  if (equal_type (dtype, t_any_struct))
	     { if (otype -> tag != TAGStruct)
	          typing_error (n, dtype, otype, rtype);
	       else *rtype = otype;
	       return;
	     };
	  if ((dtype -> tag == TAGUnresolved) && feebly_coercable (dtype, otype))
	     { *rtype = otype; return; };
	  if (is_a_procedure_type (dtype, &dparms, &drtype) &&
	      is_a_procedure_type (otype, &oparms, &ortype))
	     { if (dparms -> size != oparms -> size)
		  typing_error (n, dtype, otype, rtype);
	       else
	          { pdecl_list rparms = init_pdecl_list (dparms -> size);
	            type rrtype;
		    int ix;
		    try_coerce_type (n, drtype, ortype, &rrtype);
		    if (rrtype -> tag == TAGError) { *rtype = t_error; return; };
		    for (ix = 0; ix < dparms -> size; ix++)
		       { pdecl dparm = dparms -> array[ix];
			 pdecl oparm = oparms -> array[ix];
			 type rparmt;
			 e_access racc;
			 try_coerce_type (n, dparm -> ptype, oparm -> ptype, &rparmt);
			 try_coerce_access (n, dparm -> pacc, oparm -> pacc, &racc);
			 app_pdecl_list (rparms, new_pdecl (rparmt, racc));
		       };
		    *rtype = new_Proc_type (rparms, rrtype);
		  };
	       return;
	     };

	  /* check for coercions */
	  if (feebly_coercable (otype, dtype)) { *rtype = dtype; return; };
	  if (feebly_coercable (dtype, otype)) { *rtype = otype; return; };
	  if (equal_type (dtype, t_void)) { *rtype = t_void; return; };
	  if (equal_type (dtype, t_unknown)) { *rtype = otype; return; };
	  typing_error (n, dtype, otype, rtype);
	}

/*
   Type balancing
   equal_type -> equivalent_type?
*/
static type try_balance_types (type dtype, type ltype)
	{ pdecl_list dparms, lparms;
	  type drtype, lrtype;
	  if (equal_type (dtype, ltype)) return (ltype);
	  if (equal_type (ltype, t_error)) return (dtype);
	  if (equal_type (dtype, t_error)) return (ltype);
	  if (equal_type (dtype, t_any_object)) return (ltype);
	  if (equal_type (dtype, t_unknown)) return (ltype);
	  if (equal_type (dtype, t_any_row)) return (ltype);
	  if (equal_type (dtype, t_any_struct)) return (ltype);
	  if (is_a_procedure_type (dtype, &dparms, &drtype) &&
	      is_a_procedure_type (ltype, &lparms, &lrtype) &&
	      actual_match_formal_parms (dparms, lparms))
	     { type brtype = try_balance_types (drtype, lrtype);
	       return (new_Proc_type (lparms, brtype));
	     };
	  dcg_internal_error ("try_balance_types");
	  return (type_nil);
	}

/* only for the refinement (appl and Leave) check */
/*
   type inference, dtype = Any_non_void, reftype = determined went wrong,
   patched, but may not be final solution
*/
static type balance_refinement_type (node appl, decl id, type dtype)
	{ type reftype = id -> typ;
	  if (equal_type (dtype, reftype)) return (reftype);
	  else if (equal_type (dtype, t_error)) reftype = t_error;
	  else if (equal_type (reftype, t_error)) return (reftype);
	  else if (equal_type (dtype, t_unknown)) return (reftype);
	  else if (equal_type (reftype, t_unknown)) reftype = dtype;
	  else if (equal_type (dtype, t_any_object) &&
		   !equal_type (reftype, t_void)) return (reftype);
	  else if (equal_type (dtype, t_any_row)) return (reftype);
	  else if (equal_type (dtype, t_any_struct)) return (reftype);
	  else if (equal_type (dtype, t_any_non_void))
	    { if (equal_type (reftype, t_void))
	        { checker_error (appl -> line, appl -> column,
				 "can't balance type of refinement %s", id -> name);
		  reftype = t_error;
		};
	      return (reftype);
	    }
	  else
	     { checker_error (appl -> line, appl -> column,
			      "can't balance type of refinement %s", id -> name);
	       dcg_wlog ("ref type = %s\ndtype = %s", convert_type_text (reftype),
		         convert_type_text (dtype));
	       reftype = t_error;
	     };
	  id -> typ = reftype;
	  return (reftype);
	};

/*
   Access coercion
*/
static string convert_access_text (e_access acc)
	{ switch (acc)
	     { case acc_any:	return ("<ANY>");
	       case acc_const:	return ("CONST");
	       case acc_var:	return ("VAR");
	       case acc_proc:	return ("PROC");
	       case acc_error:	return ("<ERROR>");
	       default: dcg_bad_tag (acc, "convert_access_text");
	     };
	  return (NULL);
	};

/*
   Coerce table:
   off  A  C  V  E
     A  A  C  A  E
   d C  C  C  C  E
   e V  V  E* V  E
   m E  E  E  E  E
*/
static void try_coerce_access (node n, e_access dacc, e_access oacc, e_access *racc)
	{ *racc = acc_error;
	  if (oacc == acc_error) return;
	  switch (dacc)
	     { case acc_any:
		  if (oacc == acc_const) *racc = acc_const;
		  else if (oacc == acc_proc) *racc = acc_proc;
		  else *racc = acc_any;
		  break;
	       case acc_const: *racc = acc_const; break;
	       case acc_var:
		  if (oacc == acc_const)
	    	     checker_error (n -> line, n -> column,
				    "can't convert CONST access to VAR access");
		  else *racc = acc_var;
		  break;
	       case acc_proc:
		  if (oacc != acc_proc)
		     checker_error (n -> line, n -> column,
				    "can't convert %s access to PROC access",
				    convert_access_text (oacc));
		  else *racc = acc_proc;
		  break;
	       default: dcg_bad_tag (dacc, "try_coerce_access");
	     };
	};

/*
   Balance table:
   off  A  C  V  P  E
     A  A  C  V  P  A
   d C  -  C  -  -  C
   e V  -  -  V  -  V
   m P  -  -  -  P  P
     E  E  E  E  E  E
*/
static e_access try_balance_access (e_access dacc, e_access lacc)
	{ if (dacc == lacc) return (lacc);
	  if (lacc == acc_error) return (dacc);
	  if (dacc == acc_error) return (acc_error);
	  if ((dacc == acc_any) && (lacc == acc_const)) return (acc_const);
	  dcg_internal_error ("try_balance_access");
	  return (acc_error);
	};

/* Note only for refinement appls and Leaves */
static e_access balance_refinement_access (node appl, decl id, e_access dacc)
	{ e_access refacc = id -> Refinement.acc;
	  if (equal_e_access (dacc, refacc)) return (refacc);
	  else if (equal_e_access (dacc, acc_error)) refacc = acc_error;
	  else if (equal_e_access (refacc, acc_error)) return (refacc);
	  else if (equal_e_access (dacc, acc_any)) return (refacc);
	  else if (equal_e_access (refacc, acc_any)) refacc = dacc;
	  else
	     { checker_error (appl -> line, appl -> column,
			 "can't balance access of refinement %s", appl -> Ref_appl.name);
	       refacc = acc_error;
	     };
	  id -> Refinement.acc = refacc;
	  return (refacc);
	};

/*
   The actual checking
*/
static void check_node (node n, type dtype, e_access dacc, type *rtype, e_access *racc);
void check_paragraph (node_list par, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ int size = par -> size;
	  e_access vacc;
	  type vtype;
	  int ix;
	  for (ix = 0; ix < size - 1; ix++)
	     check_node (par -> array[ix], t_void, acc_const, &vtype, &vacc);
	  if (!size) dcg_internal_error ("check_paragraph");
	  check_node (par -> array[size - 1], dtype, dacc, rtype, racc);
	};

/*
   Check the initialization
*/
static void check_initialization (node init, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ decl d = identify_object_decl (init -> Init.name, init);
	  e_access rhsacc;
	  type rhstype;
	  if (d != decl_nil)
	     { init -> Init.id = d;
	       check_node (init -> Init.src, d -> typ, acc_const, &rhstype, &rhsacc);
	     }
	  else check_node (init -> Init.src, t_any_object, acc_const, &rhstype, &rhsacc);
	  try_coerce_type (init, dtype, t_void, rtype);
	  *racc = acc_const;
	};

/*
   Check the assignment
   improve for error recovery
*/
static void check_assignment (node ass, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ type lhstype, rhstype;
	  e_access lhsacc, rhsacc;
	  check_node (ass -> Assign.dest, t_any_object, acc_var, &lhstype, &lhsacc);
	  if (!equal_type (lhstype, t_error))
	     check_node (ass -> Assign.src, lhstype, acc_const, &rhstype, &rhsacc);
	  else check_node (ass -> Assign.src, t_any_object, acc_const, &rhstype, &rhsacc);
	  try_coerce_type (ass, dtype, t_void, rtype);
	  *racc = acc_const;
	};

/*
   Check the conditional
*/
static void check_conditional (node cond, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ type enqtype, ltype, etype, ftype;
	  e_access enqacc, lacc, eacc, facc;
	  check_node (cond -> Cond.enq, t_bool, acc_const, &enqtype, &enqacc);
	  check_paragraph (cond -> Cond.thenp, dtype, dacc, &ltype, &lacc);
	  etype = try_balance_types (dtype, ltype);
	  eacc = try_balance_access (dacc, lacc);
	  check_paragraph (cond -> Cond.elsep, etype, eacc, &ftype, &facc);
	  try_coerce_type (cond, dtype, ftype, rtype);
	  *racc = try_balance_access (eacc, facc);
	  cond -> Cond.typ = *rtype;
	};

/*
   nog toevoegen code op overeenkomende/uitsluitende cases
*/
static int_list evaluate_case_labels (node_list lbls)
	{ int_list new = init_int_list (lbls -> size);
	  int ix;
	  for (ix = 0; ix < lbls -> size; ix++)
	     { int val = evaluate_intval (lbls -> array[ix]);
	       app_int_list (new, val);
	     };
	  return (new);
	};

static void check_case_part (case_part cp, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ cp -> els = evaluate_case_labels (cp -> lbls);
	  check_paragraph (cp -> lcod, dtype, dacc, rtype, racc);
	};

static void check_case_parts (case_part_list cps, type dtype, e_access dacc,
			      type *rtype, e_access *racc)
	{ type ltype = dtype;
	  e_access lacc = dacc;
	  type rltype;
	  e_access rlacc;
	  int ix;
	  for (ix = 0; ix < cps -> size; ix++)
	     { check_case_part (cps -> array[ix], ltype, lacc, &rltype, &rlacc);
	       ltype = try_balance_types (dtype, rltype);
	       lacc = try_balance_access (dacc, rlacc);
	     };
	  *rtype = ltype;
	  *racc = lacc;
	};

/*
   Check the numerical choice clause
*/
static void check_numerical_choice (node num, type dtype, e_access dacc,
				    type *rtype, e_access *racc)
	{ type enqtype, cptype, othtype, ftype;
	  e_access enqacc, cpacc, othacc, facc;
	  check_node (num -> Num.enq, t_int, acc_const, &enqtype, &enqacc);
	  check_case_parts (num -> Num.cps, dtype, dacc, &cptype, &cpacc);
	  othtype = try_balance_types (dtype, cptype);
	  othacc = try_balance_access (dacc, cpacc);
	  check_paragraph (num -> Num.oth, othtype, othacc, &ftype, &facc);
	  try_coerce_type (num, dtype, ftype, rtype);
	  *racc = try_balance_access (othacc, facc);
	  num -> Num.typ = *rtype;
	};

/*
   Check the repetition
*/
static void check_repetition (node rep, type dtype, type *rtype, e_access *racc)
	{ type gtype;
	  e_access gacc;
	  if (strlen (rep -> For.lvar))
	     { decl d = identify_object_decl (rep -> For.lvar, rep);
	       if (d != decl_nil)
		  { rep -> For.id = d;
		    try_coerce_type (rep, t_int, d -> typ, &gtype);
		    try_coerce_access (rep, acc_var, access_from_decl (d), &gacc);
		  };
	     };
	  check_node (rep -> For.from, t_int, acc_const, &gtype, &gacc);
	  check_node (rep -> For.to, t_int, acc_const, &gtype, &gacc);
	  check_node (rep -> For.wenq, t_bool, acc_const, &gtype, &gacc);
	  check_paragraph (rep -> For.body, t_void, acc_const, &gtype, &gacc);
	  check_node (rep -> For.uenq, t_bool, acc_const, &gtype, &gacc);
	  try_coerce_type (rep, dtype, t_void, rtype);
	  *racc = acc_const;
	};

/*
   Check the simple repetition
*/
static void check_while (node wh, type dtype, type *rtype, e_access *racc)
	{ type gtype;
	  e_access gacc;
	  check_node (wh -> While.wenq, t_bool, acc_const, &gtype, &gacc);
	  check_paragraph (wh -> While.body, t_void, acc_const, &gtype, &gacc);
	  check_node (wh -> While.uenq, t_bool, acc_const, &gtype, &gacc);
	  try_coerce_type (wh, dtype, t_void, rtype);
	  *racc = acc_const;
	};

/*
   Check the LEAVE
*/
static void check_terminator (node lv, type dtype, type *rtype, e_access *racc)
	{ decl id = lv -> Leave.id;
	  e_access dacc, wacc;
	  type wtype; 
	  
	  if ((id -> tag == TAGProc_decl) || (id -> tag == TAGOp_decl)) dacc = acc_const;
	  else dacc = access_from_decl (id);
	  check_node (lv -> Leave.with, id -> typ, dacc, &wtype, &wacc);
	  if (id -> tag == TAGRefinement)
	     { (void) balance_refinement_type (lv, id, wtype);
	       (void) balance_refinement_access (lv, id, wacc);
	     };
	  try_coerce_type (lv, dtype, t_void, rtype);
	  *racc = acc_const;
	};

/*
   Check a dynamic call

   First the argument types are collected to provide a provisional signature
   When the identified procedure is a single identified name the tag of the
   node is changed into a fixed call
*/
static void check_dyn_call (node dcall, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ pdecl_list parms = new_pdecl_list ();
	  node_list args = dcall -> Dyn_call.args;
	  node proc_node = dcall -> Dyn_call.proc;
	  type ptype, ret_ptype, atype;
	  e_access acc;
	  int ix;

	  /* Collect the argument types to identify the appropriate procedure */
	  for (ix = 0; ix < args -> size; ix++)
	     { check_node (args -> array[ix], t_any_non_void, acc_any, &atype, &acc);
	       app_pdecl_list (parms, new_pdecl (attach_type (atype), acc_any));
	     };

	  /*
	     Construct the demanded type as proc type with unknown return type
	     Identification must indicate what the return type will become
	  */
	  ptype = new_Proc_type (parms, new_Unknown ());
	  check_node (proc_node, ptype, acc_proc, &ret_ptype, &acc);

	  /* Finish up */
	  try_coerce_access (dcall, dacc, acc_const, racc);
	  if (ret_ptype -> tag == TAGProc_type)
	     { parms = ret_ptype -> Proc_type.parms;
	       if (parms -> size != args -> size) dcg_internal_error ("check_dyn_call");
	       for (ix = 0; ix < args -> size; ix++)
		  { pdecl pd = parms -> array[ix];
		    check_node (args -> array[ix], pd -> ptype, pd -> pacc, &atype, &acc);
		  };
	     }
	  else
	     { *rtype = t_error;
	       return;
	     };

	  /* Optimize the dynamic call away if able to do so (99% of the cases...) */
	  if (proc_node -> tag == TAGAppl)
	     { decl id = proc_node -> Appl.id;
	       if (id -> tag == TAGProc_decl)
		  { dcall -> tag = TAGCall;
		    dcall -> Call.pname = proc_node -> Appl.name;
		    dcall -> Call.id = id;
		    dcall -> Call.args = args;
		  };
	     };
	  try_coerce_type (dcall, dtype, ret_ptype -> Proc_type.rtype, rtype);
	};

/*
   Check a call

   First the argument types are collected to provide a provisional signature

   Betere error recovery als er slechts een proc/def geidentificeerd wordt
   op basis van de naam. 
*/
static void check_call (node call, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ type_list atypes = new_type_list ();
	  node_list args = call -> Call.args;
	  decl_list fpars;
	  e_access acc;
	  type atype;
	  decl pd;
	  int ix;

	  /* Collect the argument types to identify the appropriate procedure */
	  for (ix = 0; ix < args -> size; ix++)
	     { check_node (args -> array[ix], t_any_non_void, acc_const, &atype, &acc);
	       app_type_list (atypes, attach_type (atype));
	     };
	  pd = identify_proc_decl (call, atypes);

	  /* Check access, recover if no identification */
	  try_coerce_access (call, dacc, acc_const, racc);
	  if (pd == decl_nil)
	     { detach_type_list (&atypes);
	       *rtype = t_error;
	       return;
	     };
	  call -> Call.id = pd;

	  /* Check arguments again; now with type and access demanded by proc */
	  fpars = pd -> Proc_decl.fpars;
	  for (ix = 0; ix < fpars -> size; ix++)
	     { decl fp = fpars -> array[ix];
	       check_node (args -> array[ix], fp -> typ, fp -> Formal.acc, &atype, &acc);
	     };
	  detach_type_list (&atypes);
	  try_coerce_type (call, dtype, pd -> typ, rtype);
	};

/*
   Check a row display
*/
static void check_row_display (node dpy, type dtype, type *rtype)
	{ node_list elts = dpy -> Display.elts;
	  int len = elts -> size;
	  e_access eacc;
	  type etype;
	  int ix;

	  /* Check if the display has the correct size */
	  if (dtype -> Row.sz != len)
	     checker_error (dpy -> line, dpy -> column,
			    "Display size is not equal to number of elements in ROW type");

	  /* We check at least the minimum overlap */
	  if (dtype -> Row.sz < len) len = dtype -> Row.sz;
	  dpy -> Display.typ = dtype;
	  for (ix = 0; ix < len; ix++)
	     check_node (elts -> array[ix], dtype -> Row.elt, acc_const, &etype, &eacc);
	  *rtype = dtype;
	};

/*
   Check a structure display
*/
static void check_structure_display (node dpy, type dtype, type *rtype)
	{ field_list flds = dtype -> Struct.flds;
	  node_list elts = dpy -> Display.elts;
	  int len = elts -> size;
	  e_access facc;
	  type ftype;
	  int ix;

	  /* Check if the display has the correct size */
	  if (flds -> size != len)
	     checker_error (dpy -> line, dpy -> column,
			    "Display size is not equal to number of fields in STRUCT type");

	  /* We check at least the minimum overlap */
	  if (flds -> size < len) len = flds -> size;
	  dpy -> Display.typ = dtype;
	  for (ix = 0; ix < len; ix++)
	     check_node (elts -> array[ix], flds -> array[ix] -> ftype, acc_const, &ftype, &facc);
	  *rtype = dtype;
	};

/*
   Check a display whose type is a priori unknown
*/
static void check_anytype_display (node dpy, type dtype, type *rtype)
	{ node_list elts = dpy -> Display.elts;
	  type_list flds = new_type_list ();
	  e_access facc;
	  type ftype, ntype;
	  int ix;

	  /* Check the nodes for a non void type */
	  for (ix = 0; ix < elts -> size; ix++)
	     { check_node (elts -> array[ix], t_any_object, acc_const, &ftype, &facc);
	       app_type_list (flds, ftype);
	     };

	  /* Construct the display type: we must still be able to resolve it */
	  ntype = new_Display_type (flds);
	  try_coerce_type (dpy, dtype, ntype, rtype);
	};

static void check_display (node dpy, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ switch (dtype -> tag)
	     { case TAGRow: check_row_display (dpy, dtype, rtype); break;
	       case TAGStruct: check_structure_display (dpy, dtype, rtype); break;
	       default: check_anytype_display (dpy, dtype, rtype);
	     }; 
	  try_coerce_access (dpy, dacc, acc_const, racc);
	};

/*
   Check a subscription
*/
static void check_subscription (node sub, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ type rowtype, idxtype;
	  e_access idxacc;
	  *rtype = t_error;
	  check_node (sub -> Sub.arr, t_any_row, dacc, &rowtype, racc);
	  check_node (sub -> Sub.index, t_int, acc_const, &idxtype, &idxacc);
	  if (rowtype == t_error) return;
	  if (rowtype -> tag != TAGRow)
	     dcg_internal_error ("check_subscription");
	  sub -> Sub.rtype = rowtype;
	  try_coerce_type (sub, dtype, rowtype -> Row.elt, rtype);
	};

/*
   Check a selection
*/
static void check_selection (node sel, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ field fld;
	  type structtype;
	  *rtype = t_error;
	  check_node (sel -> Select.str, t_any_struct, dacc, &structtype, racc);
	  if (structtype == t_error) return;
	  if (structtype -> tag != TAGStruct)
	     dcg_internal_error ("check_selection");
	  fld = identify_selector (sel -> Select.sel, sel, structtype);
	  if (fld == field_nil) return;
	  sel -> Select.stype = structtype;
	  sel -> Select.fld = fld;
	  try_coerce_type (sel, dtype, fld -> ftype, rtype);
	};

/*
   Check an abstractor
*/
static void check_abstractor (node abs, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ decl d = identify_type_name (abs -> Abstr.tname, abs -> line, abs -> column);
	  e_access argacc;
	  type argtype;
	  *rtype = t_error;
	  try_coerce_access (abs, dacc, acc_const, racc);
	  if (d == decl_nil)
	     { check_node (abs -> Abstr.arg, t_any_object, dacc, &argtype, &argacc);
	       return;
	     };
	  if (d -> tag != TAGType)
	     { checker_error (abs -> line, abs -> column,
			      "abstractor %s is not the name of a TYPE",
			      abs -> Abstr.tname);
	       check_node (abs -> Abstr.arg, t_any_object, dacc, &argtype, &argacc);
	       return;
	     };
	  abs -> Abstr.id = d;
	  check_node (abs -> Abstr.arg, d -> Type.concr, dacc, &argtype, &argacc);
	  try_coerce_type (abs, dtype, d -> typ, rtype);
	};

/*
   Check a concretizer
*/
static void check_concretizer (node conc, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ type argtype;
	  decl td;
	  *rtype = t_error;
	  check_node (conc -> Concr.arg, t_any_object, dacc, &argtype, racc);
	  if (argtype == t_error) return;
	  if (argtype -> tag != TAGTname)
	     { checker_error (conc -> line, conc -> column,
			      "argument of concretizer has not an abstract type");
	       return;
	     };
	  td = argtype -> Tname.id;
	  if (td == decl_nil)
	     dcg_internal_error ("check_concretizer");
	  if (td -> tag != TAGType)
	     { checker_error (conc -> line, conc -> column,
			      "argument of concretizer has not an abstract type");
	       return;
	     };
	  conc -> Concr.id = td;
	  try_coerce_type (conc, dtype, td -> Type.concr, rtype);
	};

/*
   Check a dyadic operator
*/
static void check_dyop (node dyop, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ decl d, fpar1, fpar2;
	  type atype1, atype2;
	  e_access acc;
	  *rtype = t_error;
	  check_node (dyop -> Dyop.arg1, t_any_non_void, acc_const, &atype1, &acc);
	  check_node (dyop -> Dyop.arg2, t_any_non_void, acc_const, &atype2, &acc);
	  d = identify_dyop_decl (dyop, atype1, atype2);
	  try_coerce_access (dyop, dacc, acc_const, racc);
	  if (d == decl_nil) return;
	  dyop -> Dyop.id = d;
	  fpar1 = d -> Op_decl.fpars -> array[0];
	  fpar2 = d -> Op_decl.fpars -> array[1];
	  check_node (dyop -> Dyop.arg1, fpar1 -> typ, fpar1 -> Formal.acc, &atype1, &acc);
	  check_node (dyop -> Dyop.arg2, fpar2 -> typ, fpar2 -> Formal.acc, &atype2, &acc);
	  try_coerce_type (dyop, dtype, d -> typ, rtype);
	};

/*
   Check a monadic operator
*/
static void check_monop (node monop, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ decl d, fpar;
	  e_access acc;
	  type atype;
	  *rtype = t_error;
          check_node (monop -> Monop.arg, t_any_non_void, acc_const, &atype, &acc);
	  d = identify_monop_decl (monop, atype);
	  try_coerce_access (monop, dacc, acc_const, racc);
	  if (d == decl_nil) return;
	  monop -> Monop.id = d;
	  fpar = d -> Op_decl.fpars -> array[0];
	  check_node (monop -> Monop.arg, fpar -> typ, fpar -> Formal.acc, &atype, &acc);
	  try_coerce_type (monop, dtype, d -> typ, rtype);
	};

/*
   Check an identifier application
*/
static void check_ident_appl (node appl, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ string name = appl -> Appl.name;
	  decl d;

	  /*
	     Do the primary identification
	     We return on an error or a generic identification
	  */
	  *rtype = t_error;
	  *racc = acc_error;
	  d = identify_small_name (name, appl, dtype, rtype);
	  if (d == decl_nil) return;	

	  /* We have a correct identification, now check access and deproceduring */
	  switch (d -> tag)
	     { case TAGObject_decl:
	       case TAGFormal:
		  { appl -> Appl.id = d;
		    if ((d -> typ -> tag != TAGProc_type) || (dtype -> tag == TAGProc_type))
		       { try_coerce_access (appl, dacc, access_from_decl (d), racc); 
	  	         try_coerce_type (appl, dtype, d -> typ, rtype);
		       }
		    else
		       { /* We must deprocedure a procedure parameter */
			 type proc_type = d -> typ;
			 node_list zero_args = new_node_list ();
			 node id_appl = new_Appl (appl -> line, appl -> column, name);
			 if (proc_type -> Proc_type.parms -> size != 0)
			    dcg_internal_error ("check_ident_appl");
			 id_appl -> Appl.id = d;
		         appl -> tag = TAGDyn_call;
			 appl -> Dyn_call.proc = id_appl;
			 appl -> Dyn_call.args = zero_args;
			 try_coerce_access (appl, dacc, acc_const, racc);
			 try_coerce_type (appl, dtype, proc_type -> Proc_type.rtype, rtype);
		       };
		  }; break;
	       case TAGSynonym_value:
		  { /* construct denoter for this value */
		    value val = attach_value (d -> Synonym_value.val);
		    appl -> tag = TAGDenoter;
		    appl -> Denoter.val = val;
		    detach_string (&name);
	  	    try_coerce_access (appl, dacc, acc_const, racc); 
		    try_coerce_type (appl, dtype, d -> typ, rtype);
		  }; break;
	       case TAGProc_decl:
		  { if (dtype -> tag == TAGProc_type)
		       { type ptype = type_from_decl (d);
			 appl -> Appl.id = d;
			 try_coerce_access (appl, dacc, acc_proc, racc);
			 try_coerce_type (appl, dtype, ptype, rtype);
		       }
		    else if (!d -> Proc_decl.fpars -> size)
		       { /* deproceduring coercion, construct dynamic call */
			 node_list zero_args = new_node_list ();
			 appl -> tag = TAGCall;
			 appl -> Call.pname = name;
			 appl -> Call.id = d;
			 appl -> Call.args = zero_args;
		         try_coerce_type (appl, dtype, d -> typ, rtype);
	  	         try_coerce_access (appl, dacc, acc_const, racc); 
		       }
		    else checker_error (appl -> line, appl -> column, 
				        "can't coerce PROC %s into an object", name);
		  }; break;
	       default: dcg_bad_tag (d -> tag, "check_ident_appl");
	     };
	};

/*
   Check a refinement application
*/
static void check_ref_appl (node appl, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ decl ref = appl -> Ref_appl.id;
	  e_access bacc = acc_error;
	  type btype = t_error;

	  if (ref -> Refinement.stat == type_checked)
	     { btype = balance_refinement_type (appl, ref, dtype);
	       bacc = balance_refinement_access (appl, ref, dacc);
	     }
	  else if (ref -> Refinement.stat == ref_checked)
	     { check_paragraph (ref -> Refinement.rcode, t_unknown, acc_any, &btype, &bacc);
	       btype = balance_refinement_type (appl, ref, btype);
	       bacc = balance_refinement_access (appl, ref, bacc);
	     }
	  else dcg_internal_error ("check_ref_appl");
	  ref -> Refinement.stat = type_checked;
	  try_coerce_type (appl, dtype, btype, rtype);
	  try_coerce_access (appl, dacc, bacc, racc);
	};

/*
   Check NIL
*/
static void check_nil (node nil, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ if (is_a_niltype (dtype)) nil -> Nil.typ = dtype;
	  else
	     { string dtext = convert_type_text (dtype);
	       checker_error (nil -> line, nil -> column,
			      "can't create NIL value for type %s", dtext);
	     };
	  try_coerce_access (nil, dacc, acc_const, racc);
	  *rtype = dtype;
	};

/*
   Check a denoter
*/
static void check_denoter (node den, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ type otype = type_from_value (den -> Denoter.val);
	  try_coerce_access (den, dacc, acc_const, racc);
	  try_coerce_type (den, dtype, otype, rtype);
	};

/*
   Check SKIP
*/
static void check_skip (node skip, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ try_coerce_type (skip, dtype, t_void, rtype);
	  *racc = acc_const;
	};

/*
   Node dispatcher
*/
static void check_node (node n, type dtype, e_access dacc, type *rtype, e_access *racc)
	{ switch (n -> tag)
	     { case TAGInit:     check_initialization (n, dtype, dacc, rtype, racc); break;
	       case TAGAssign:   check_assignment (n, dtype, dacc, rtype, racc); break;
	       case TAGCond:     check_conditional (n, dtype, dacc, rtype, racc); break;
	       case TAGNum:	 check_numerical_choice (n, dtype, dacc, rtype, racc); break;
	       case TAGFor:      check_repetition (n, dtype, rtype, racc); break;
	       case TAGWhile:    check_while (n, dtype, rtype, racc); break;
	       case TAGLeave:	 check_terminator (n, dtype, rtype, racc); break;
	       case TAGDyn_call: check_dyn_call (n, dtype, dacc, rtype, racc); break;
	       case TAGCall:     check_call (n, dtype, dacc, rtype, racc); break;
	       case TAGDisplay:	 check_display (n, dtype, dacc, rtype, racc); break;
	       case TAGSub:	 check_subscription (n, dtype, dacc, rtype, racc); break;
	       case TAGSelect:	 check_selection (n, dtype, dacc, rtype, racc); break;
	       case TAGAbstr:	 check_abstractor (n, dtype, dacc, rtype, racc); break;
	       case TAGConcr:	 check_concretizer (n, dtype, dacc, rtype, racc); break;
	       case TAGDyop:     check_dyop (n, dtype, dacc, rtype, racc); break;
	       case TAGMonop:    check_monop (n, dtype, dacc, rtype, racc); break;
	       case TAGRef_appl: check_ref_appl (n, dtype, dacc, rtype, racc); break;
	       case TAGAppl:     check_ident_appl (n, dtype, dacc, rtype, racc); break;
	       case TAGNil:	 check_nil (n, dtype, dacc, rtype, racc); break;
	       case TAGDenoter:  check_denoter (n, dtype, dacc, rtype, racc); break;
	       case TAGDefault:  *rtype = dtype; break;
	       case TAGSkip:     check_skip (n, dtype, dacc, rtype, racc); break;
	       default: dcg_bad_tag (n -> tag, "check_node");
	     };
	};
