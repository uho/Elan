/*
   File: tgt_gen.c
   Calls the correct machine code generator to generate the target code

   CVS ID: "$Id: tgt_gen.c,v 1.8 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_imc.h"
#include "tgt_gen_common.h"
#include "tgt_gen_machdep.h"
#include "tgt_gen_intelx86.h"
#include "tgt_gen_amd64.h"
#include "tgt_gen_sparc.h"
#include "tgt_gen_alpha.h"
#include "tgt_gen_vax.h"
#include "tgt_gen.h"
#include "options.h"

static void dispatch_code_generation ()
{ switch (tcpu)
    { case TGT_SPARC:	  sparc_code_generation (); break;
      case TGT_INTELx86:  intelx86_code_generation (); break;
      case TGT_AMD64:	  amd64_code_generation (); break;
      case TGT_ALPHA:	  alpha_code_generation (); break;
      case TGT_VAX:	  vax_code_generation (); break;
      default: dcg_bad_tag (tcpu, "dispatch_code_generation");
    };
}

int size_from_type (type t)
{ switch (tcpu)
    { case TGT_SPARC:	  return (sparc_size_from_type (t)); break;
      case TGT_INTELx86:  return (intelx86_size_from_type (t)); break;
      case TGT_AMD64:	  return (amd64_size_from_type (t)); break;
      case TGT_ALPHA:	  return (alpha_size_from_type (t)); break;
      case TGT_VAX:	  return (vax_size_from_type (t)); break;
      default: dcg_bad_tag (tcpu, "size_from_type");
    };
  return (0);
};

int regsize_from_arg (pdecl pd)
{ switch (tcpu)
    { case TGT_SPARC:	  return (sparc_regsize_from_arg (pd)); break;
      case TGT_INTELx86:  return (intelx86_regsize_from_arg (pd)); break;
      case TGT_AMD64:	  return (amd64_regsize_from_arg (pd)); break;
      case TGT_ALPHA:	  return (alpha_regsize_from_arg (pd)); break;
      case TGT_VAX:	  return (vax_regsize_from_arg (pd)); break;
      default: dcg_bad_tag (tcpu, "regsize_from_arg");
    };
  return (0);
};

/*
   Routine to calculate the size of the indirect object
*/
int indirect_size_from_type (type t)
{ switch (t -> tag)
    { case TAGRow:
	return (t -> Row.sz * size_from_type (t -> Row.elt));
      case TAGStruct:
	{ field_list flds = t -> Struct.flds;
	  field last = flds -> array[flds -> size - 1];
	  return (last -> offset + size_from_type (last -> ftype));
	};
      default: dcg_bad_tag (t -> tag, "indirect_size_from_type");
    };
  return (0);
};

void target_code_generation ()
{ dcg_warning (0, "   generating machine code...");
  init_common_code_generation ();
  open_assembler_file (basename);
  code_preamble (basename);
  dispatch_code_generation ();
  close_assembler_file ();
};
