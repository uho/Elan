/*
   File: ast_utils.c
   Defines some access functions on ast structures

   CVS ID: "$Id: ast_utils.c,v 1.6 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "type_table.h"

type type_from_decl (decl d)
{ pdecl_list parms;
  decl_list fpars;
  int ix;

  switch (d -> tag)
    { case TAGProc_decl: fpars = d -> Proc_decl.fpars; break;
      case TAGOp_decl: fpars = d -> Op_decl.fpars; break;
      default: return (d -> typ);
    };
  parms = init_pdecl_list (fpars -> size);
  for (ix = 0; ix < fpars -> size; ix++)
    { decl formal = fpars -> array[ix];
      pdecl pd = new_pdecl (attach_type (formal -> typ), formal -> Formal.acc);
      app_pdecl_list (parms, pd);
    };
  return (new_Proc_type (parms, attach_type (d -> typ)));
}

type type_from_value (value v)
{ switch (v -> tag)
    { case TAGIval: return (t_int);
      case TAGBval: return (t_bool);
      case TAGRval: return (t_real);
      case TAGTval: return (t_text);
      case TAGErrval: return (t_error);
      default: dcg_bad_tag (v -> tag, "type_from_value");
    };
  return (type_nil);
}

e_access access_from_decl (decl d)
{ switch (d -> tag)
    { case TAGFormal:		return (d -> Formal.acc);
      case TAGObject_decl:	return (d -> Object_decl.acc);
      case TAGProc_decl:
      case TAGOp_decl:		return (acc_proc);
      case TAGSynonym_value:	return (acc_const);
      case TAGRefinement:	return (d -> Refinement.acc);
      default: dcg_bad_tag (d -> tag, "access_from_decl");
    };
  return (acc_error);
}
