/*
   File: pre_checker.c
   This module builds packet and routine declaration trees,
   prechecks refinements, identifies refinement applications,
   resolves LEAVE constructs and prechecks the trees for
   certain double declarations, which could bother subsequent
   check passes.

   CVS ID: "$Id: pre_checker.c,v 1.7 2011/08/13 15:58:54 marcs Exp $"
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
#include "options.h"
#include "elan_ast.h"
#include "decl_tree.h"
#include "ast_utils.h"
#include "ident.h"
#include "contsens.h"
#include "pre_checker.h"

/* error administration */
static void pre_check_error (int line, int column, char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  if (!line && !column) dcg_error (0, "%s in file %s", buf, contsens_fname);
  else dcg_error (0, "%s in file %s, line %d, col %d", buf, contsens_fname, line, column);
}

/*
   Precheck nodes traversing the abstract syntax tree
*/
static void pre_check_refinement (decl ref, decl_tree *surr_env, decl_list alg_stack);
static void pre_check_case_parts (case_part_list cps, decl_tree *surr_env, decl_list alg_stack);
static void pre_check_case_part (case_part cp, decl_tree *surr_env, decl_list alg_stack);
static void pre_check_node (node n, decl_tree *surr_env, decl_list alg_stack);
static void pre_check_node_list (node_list par, decl_tree *surr_env, decl_list alg_stack)
{ int ix;
  for (ix = 0; ix < par -> size; ix++)
    pre_check_node (par -> array[ix], surr_env, alg_stack);
};

static void pre_check_initialization (node init, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (init -> Init.src, surr_env, alg_stack);
}

static void pre_check_assignment (node ass, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (ass -> Assign.dest, surr_env, alg_stack);
  pre_check_node (ass -> Assign.src, surr_env, alg_stack);
}

static void pre_check_conditional (node cond, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (cond -> Cond.enq, surr_env, alg_stack);
  pre_check_node_list (cond -> Cond.thenp, surr_env, alg_stack);
  pre_check_node_list (cond -> Cond.elsep, surr_env, alg_stack);
}

static void pre_check_numerical_choice (node num, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (num -> Num.enq, surr_env, alg_stack);
  pre_check_case_parts (num -> Num.cps, surr_env, alg_stack);
  pre_check_node_list (num -> Num.oth, surr_env, alg_stack);
}

static void pre_check_case_parts (case_part_list cps, decl_tree *surr_env, decl_list alg_stack)
{ int ix;
  for (ix = 0; ix < cps -> size; ix++)
    pre_check_case_part (cps -> array[ix], surr_env, alg_stack);
}

static void pre_check_case_part (case_part cp, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node_list (cp -> lbls, surr_env, alg_stack);
  pre_check_node_list (cp -> lcod, surr_env, alg_stack);
}

static void pre_check_repetition (node rep, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (rep -> For.from, surr_env, alg_stack);
  pre_check_node (rep -> For.to, surr_env, alg_stack);
  pre_check_node (rep -> For.wenq, surr_env, alg_stack);
  pre_check_node_list (rep -> For.body, surr_env, alg_stack);
  pre_check_node (rep -> For.uenq, surr_env, alg_stack);
}

static void pre_check_while (node wh, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (wh -> While.wenq, surr_env, alg_stack);
  pre_check_node_list (wh -> While.body, surr_env, alg_stack);
  pre_check_node (wh -> While.uenq, surr_env, alg_stack);
}

/*
   Pre check a LEAVE clause.
   Note that we only leave a reference to the left algorithm
   in the node, since there may be more than one way this
   node could have been reached
*/
static void check_leavability (node lv, decl_list alg_stack)
{ int ix;
  for (ix = 0; ix < alg_stack -> size; ix++)
    { decl d = alg_stack -> array[ix];
      if (streq (lv -> Leave.alg, d -> name))
	{ if (lv -> Leave.id == decl_nil) lv -> Leave.id = d;
	  else if (lv -> Leave.id != d)
	    dcg_internal_error ("check_leavability");
	};
     };
  if (lv -> Leave.id == decl_nil)
    pre_check_error (lv -> line, lv -> column, "can't leave algorithm %s", lv -> Leave.alg);
}

static void pre_check_leave (node lv, decl_tree *surr_env, decl_list alg_stack)
{ check_leavability (lv, alg_stack);
  pre_check_node (lv -> Leave.with, surr_env, alg_stack);
}

static void pre_check_dyn_call (node dcall, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (dcall -> Dyn_call.proc, surr_env, alg_stack);
  pre_check_node_list (dcall -> Dyn_call.args, surr_env, alg_stack);
}

static void pre_check_call (node call, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node_list (call -> Call.args, surr_env, alg_stack);
}

static void pre_check_display (node dpy, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node_list (dpy -> Display.elts, surr_env, alg_stack);
}

static void pre_check_subscription (node sub, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (sub -> Sub.arr, surr_env, alg_stack);
  pre_check_node (sub -> Sub.index, surr_env, alg_stack);
}

static void pre_check_selection (node sel, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (sel -> Select.str, surr_env, alg_stack);
}

static void pre_check_abstraction (node abs, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (abs -> Abstr.arg, surr_env, alg_stack);
}

static void pre_check_concretizer (node concr, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (concr -> Concr.arg, surr_env, alg_stack);
}

static void pre_check_dyop (node dyop, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (dyop -> Dyop.arg1, surr_env, alg_stack);
  pre_check_node (dyop -> Dyop.arg2, surr_env, alg_stack);
}

static void pre_check_monop (node monop, decl_tree *surr_env, decl_list alg_stack)
{ pre_check_node (monop -> Monop.arg, surr_env, alg_stack);
}

/*
   Precheck identifier applications and distinguish refinement applications
   from other identifier application.
*/
static void pre_check_ident_appl (node appl, decl_tree *surr_env, decl_list alg_stack)
{ string rname = appl -> Appl.name;
  decl d = try_identify_refinement (rname, *surr_env);
  if (d == decl_nil) return;

  /* change node kind and fill in parts */
  appl -> tag = TAGRef_appl;
  appl -> Ref_appl.name = rname;
  appl -> Ref_appl.id = d;
  pre_check_refinement (d, surr_env, alg_stack);
}

static void pre_check_ref_appl (node appl, decl_tree *surr_env, decl_list alg_stack)
{ string rname = appl -> Ref_appl.name;
  decl d = try_identify_refinement (rname, *surr_env);
  if (d == decl_nil) dcg_internal_error ("pre_check_ref_appl");

  /* fill in parts */
  appl -> Ref_appl.id = d;
  pre_check_refinement (d, surr_env, alg_stack);
}

/*
   Precheck node dispatcher
*/
static void pre_check_node (node n, decl_tree *surr_env, decl_list alg_stack)
{ switch (n -> tag)
    { case TAGInit:	pre_check_initialization (n, surr_env, alg_stack); break;
      case TAGAssign:	pre_check_assignment (n, surr_env, alg_stack); break;
      case TAGCond:	pre_check_conditional (n, surr_env, alg_stack); break;
      case TAGNum:	pre_check_numerical_choice (n, surr_env, alg_stack); break;
      case TAGFor:	pre_check_repetition (n, surr_env, alg_stack); break;
      case TAGWhile:	pre_check_while (n, surr_env, alg_stack); break;
      case TAGLeave:	pre_check_leave (n, surr_env, alg_stack); break;
      case TAGDyn_call:	pre_check_dyn_call (n, surr_env, alg_stack); break;
      case TAGCall:	pre_check_call (n, surr_env, alg_stack); break;
      case TAGDisplay:	pre_check_display (n, surr_env, alg_stack); break;
      case TAGSub:	pre_check_subscription (n, surr_env, alg_stack); break;
      case TAGSelect:	pre_check_selection (n, surr_env, alg_stack); break;
      case TAGAbstr:	pre_check_abstraction (n, surr_env, alg_stack); break;
      case TAGConcr:	pre_check_concretizer (n, surr_env, alg_stack); break;
      case TAGDyop:	pre_check_dyop (n, surr_env, alg_stack); break;
      case TAGMonop:	pre_check_monop (n, surr_env, alg_stack); break;
      case TAGAppl:	pre_check_ident_appl (n, surr_env, alg_stack); break;
      case TAGRef_appl:	pre_check_ref_appl (n, surr_env, alg_stack);
      case TAGNil:
      case TAGDenoter: 
      case TAGDefault:
      case TAGSkip:	break;
      default: dcg_bad_tag (n -> tag, "pre_check_node");
    };
}

/*
   Pre check a refinement definition
*/
static void pre_check_refinement (decl ref, decl_tree *surr_env, decl_list alg_stack)
{ switch (ref -> Refinement.stat)
    { case not_checked:
	{ /*
	     Mark this refinement as being checked, add it to the
	     algoritm stack, add his locals to the environment and
	     (recursively) check its code.
	  */
	  enter_decls_into_tree (surr_env, ref -> Refinement.locals);
	  ref -> Refinement.stat = being_checked;
	  app_decl_list (alg_stack, ref);
	  pre_check_node_list (ref -> Refinement.rcode, surr_env, alg_stack);
	  del_decl_list (alg_stack, alg_stack -> size - 1);

	  /* if refinement still marked as being checked it is ok */
	  if (ref -> Refinement.stat == being_checked)
	    ref -> Refinement.stat = ref_checked;
	}; break;
      case ref_checked:
	{ /* if already checked, check again for LEAVEs */
	  app_decl_list (alg_stack, ref);
	  pre_check_node_list (ref -> Refinement.rcode, surr_env, alg_stack);
	  del_decl_list (alg_stack, alg_stack -> size - 1);
	}; break;
      case being_checked:
	{ /* if ref being checked, it is recursive */
	  pre_check_error (ref -> line, ref -> column, "refinement %s is recursive", ref -> name);
	  ref -> Refinement.stat = recursive;
	};
      case recursive: break;
      default: dcg_bad_tag (ref -> Refinement.stat, "pre_check_refinement");
    };
}

/*
   Pre check a declaration tree for double declarations
*/
static void pre_check_double_decls_in_tree (decl_tree dt)
{ if (dt == decl_tree_nil) return;
  pre_check_double_decls_in_tree (dt -> left);
  pre_check_double_decls (dt -> decls);
  pre_check_double_decls_in_tree (dt -> right);
}

/*
   Pre check a routine (a user declared OP or PROC)
   First create a stack entry for the routine since we are able
   to LEAVE the routine. We then add all local declarations to
   the routine's declaration tree and finish by checking the
   routine code.
*/
static void pre_check_routine (decl pd, decl_tree *ldecls, body bdy)
{ decl_list locals = bdy -> Routine.locals;
  decl_list alg_stack = new_decl_list ();
  int ix;

  app_decl_list (alg_stack, attach_decl (pd));
  for (ix = 0; ix < locals -> size; ix++)
    { decl ld = locals -> array[ix];
      enter_decl_tree (ldecls, ld);
      switch (ld -> tag)
	{ case TAGFormal:
	  case TAGObject_decl:
	  case TAGSynonym_value:
	  case TAGRefinement: break;
	  default: dcg_bad_tag (ld -> tag, "pre_check_routine");
	};
    };
  pre_check_node_list (bdy -> Routine.rcode, ldecls, alg_stack);
  detach_decl_list (&alg_stack);
  bdy -> Routine.rdecls = *ldecls;
}

/*
   Pre check PROC and OP declarations
*/
static void pre_check_proc_decl (decl pd)
{ decl_tree ldecls = decl_tree_nil;
  body bdy = pd -> Proc_decl.bdy;
  enter_decls_into_tree (&ldecls, pd -> Proc_decl.fpars);
  if (bdy -> tag == TAGRoutine)
    pre_check_routine (pd, &ldecls, pd -> Proc_decl.bdy);
  pre_check_double_decls_in_tree (ldecls);
  if (bdy -> tag != TAGRoutine) detach_decl_tree (&ldecls);
}

static void pre_check_op_decl (decl pd)
{ decl_tree ldecls = decl_tree_nil;
  body bdy = pd -> Op_decl.bdy;
  enter_decls_into_tree (&ldecls, pd -> Op_decl.fpars);
  if (bdy -> tag == TAGRoutine)
    pre_check_routine (pd, &ldecls, pd -> Op_decl.bdy);
  pre_check_double_decls_in_tree (ldecls);
  if (bdy -> tag != TAGRoutine) detach_decl_tree (&ldecls);
}

/*
   Precheck the packet
   First enter all packet declarations into the declaration tree
   If the packet contains a refinement add an application of the
   first refinement to the packet code. The code the continues
   by prechecking the packet code. During the check a stack is 
   maintained to reflect the refinement application hierarchy,
   needed to solve LEAVE-clauses. Finally the declaration tree
   is checked for conflicting double declarations, although
   not fully: we need the type analysis to check for conflicting
   routine declarations.
*/
void pre_check_packet (packet pk)
{ decl_list pdecls = pk -> pdecls;
  decl_list alg_stack = new_decl_list ();
  node fref = node_nil;
  int ix;
  for (ix = 0; ix < pdecls -> size; ix++)
    { decl pd = pdecls -> array[ix];
      enter_decl_tree (&pk -> pkdecls, pd);
      switch (pd -> tag)
	{ case TAGProc_decl:	pre_check_proc_decl (pd); break;
	  case TAGOp_decl: 	pre_check_op_decl (pd); break;
	  case TAGRefinement:
	    if (fref == node_nil)
	      fref = new_Ref_appl (pd -> line, pd -> column, attach_string (pd -> name));
	  case TAGObject_decl:
	  case TAGSynonym_value:
	  case TAGSynonym_type:
	  case TAGType:
	    break;
	  default: dcg_bad_tag (pd -> tag, "pre_check_packet");
	};
    };
  if (fref != node_nil)
    app_node_list (pk -> pcode, fref);
  else if (!pk -> pcode -> size)
    app_node_list (pk -> pcode, new_Skip (0, 0));
  pre_check_node_list (pk -> pcode, &pk -> pkdecls, alg_stack);
  pre_check_double_decls_in_tree (pk -> pkdecls);
}
