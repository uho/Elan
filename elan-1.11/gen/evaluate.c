/*
   File: evaluate.c
   Evaluates static expressions
   Checks synonym value definitions

   CVS ID: "$Id: evaluate.c,v 1.4 2011/08/13 15:58:54 marcs Exp $"
*/


/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "ast_utils.h"
#include "type_table.h"
#include "ident.h"
#include "evaluate.h"
#include "contsens.h"

static value errval;
static value no_node_eval (node n, string cons)
{ contsens_error (n -> line, n -> column, "can't statically evaluate %s");
  return (attach_value (errval));
}

static value evaluate_synonym_value (decl d);
static value evaluate_ident (node n)
{ decl d = identify_synonym_value (n -> Appl.name, n);
  if (d == decl_nil) return (attach_value (errval));
  return (evaluate_synonym_value (d));
}

static value evaluate_denoter (node n)
{ return (attach_value (n -> Denoter.val));
}

static value evaluate_node (node n)
{ switch (n -> tag)
    { case TAGInit:	return (no_node_eval (n, "initialization"));
      case TAGAssign:	return (no_node_eval (n, "assignation"));
      case TAGCond:	return (no_node_eval (n, "conditional"));
      case TAGNum:	return (no_node_eval (n, "numerical choice"));
      case TAGFor:
      case TAGWhile:	return (no_node_eval (n, "repetition"));
      case TAGLeave:	return (no_node_eval (n, "terminator"));
      case TAGCall:	return (no_node_eval (n, "call yet"));
      case TAGDisplay:	return (no_node_eval (n, "display"));
      case TAGSub:	return (no_node_eval (n, "subscription"));
      case TAGSelect:	return (no_node_eval (n, "selection"));
      case TAGAbstr:	return (no_node_eval (n, "abstractor"));
      case TAGConcr:	return (no_node_eval (n, "concretizer"));
      case TAGDyop:	return (no_node_eval (n, "dyadic operator yet"));
      case TAGMonop:	return (no_node_eval (n, "monadic operator yet"));
      case TAGRef_appl:	return (no_node_eval (n, "refinement application"));
      case TAGAppl:	return (evaluate_ident (n));
      case TAGNil:	return (no_node_eval (n, "nil"));
      case TAGSkip:	return (no_node_eval (n, "empty paragraph"));
      case TAGDenoter:	return (evaluate_denoter (n));
      default: dcg_bad_tag (n -> tag, "evaluate_node");
    };
  return (value_nil);
}

int evaluate_intval (node n)
{ value val = evaluate_node (n);
  int ival = -1;
  switch (val -> tag)
    { case TAGIval: ival = val -> Ival.i;
      case TAGErrval: break;
      case TAGBval:
      case TAGRval:
      case TAGTval:
	contsens_error (n -> line, n -> column, "can't convert %s value to INT",
			convert_type_text (type_from_value (val)));
	break;
      default: dcg_bad_tag (val -> tag, "evaluate_intval");
    };
  detach_value (&val);
  return (ival);
}

static value evaluate_synonym_value (decl d)
{ /* if already checked, done with it */
  if (d -> Synonym_value.stat == type_checked)
    return (attach_value (d -> Synonym_value.val));

  /* if being checked, it is recursive */
  if (d -> Synonym_value.stat == being_checked)
    { contsens_error (d -> line, d -> column, "LET declaration of %s is recursive", d -> name); 
      d -> typ = attach_type (t_error);
      d -> Synonym_value.val = attach_value (errval);
      d -> Synonym_value.stat = type_checked;
      return (attach_value (errval));
    };

  /* mark declaration as being checked, check rhs */
  d -> Synonym_value.stat = being_checked;
  d -> Synonym_value.val = evaluate_node (d -> Synonym_value.rhs);

  /* if still marked as being checked, no recursion, mark checked */
  if (d -> Synonym_value.stat == being_checked)
    { d -> typ = attach_type (type_from_value (d -> Synonym_value.val));
      d -> Synonym_value.stat = type_checked;
    };

  return (attach_value (d -> Synonym_value.val));
}

void evaluate_synonym_values (decl_list dl)
{ int ix;
  errval = new_Errval ();
  for (ix = 0; ix < dl -> size; ix++)
    { decl d = dl -> array[ix];
      if (d -> tag == TAGSynonym_value)
	{ value val = evaluate_synonym_value (d);
	  detach_value (&val);
	};
    };
}
