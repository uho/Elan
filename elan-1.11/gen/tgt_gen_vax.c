/*
   File: tgt_gen_vax.c
   Generates VAX11 machine code

   CVS ID: "$Id: tgt_gen_vax.c,v 1.8 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "options.h"
#include "elan_ast.h"
#include "elan_imc.h"
#include "imc_utils.h"
#include "type_table.h"
#include "tgt_gen.h"
#include "tgt_gen_common.h"
#include "tgt_gen_vax.h"

/*
   VAX11 properties
   Basically:
      sizeof (VOID) = 0;
      sizeof (INT) = sizeof (<ADDR>) = 4;
      sizeof (REAL) = 8;
*/
int vax_size_from_type (type t)
{ switch (t -> tag)
    { case TAGTname:
	{ decl id = t -> Tname.id;
	  if (id -> tag != TAGType)
	    dcg_internal_error ("vax_size_from_type");
	  return (vax_size_from_type (id -> Type.concr));
	};
      case TAGVoid: return (0);
      case TAGInt:
      case TAGBool:
      case TAGAddr:
      case TAGText:
      case TAGFile:
      case TAGNiltype:
      case TAGRow:
      case TAGStruct:
      case TAGProc_type: return (4);
      case TAGReal: return (8);
      default: dcg_bad_tag (t -> tag, "vax_size_from_type");
    };
  return (0);
}

int vax_regsize_from_arg (pdecl pd)
{ switch (pd -> pacc)
    { case acc_var:
      case acc_proc: return (1);
      default: return (vax_size_from_type (pd -> ptype) / 4);
    };
}

/*
   VAX11 allocation routines
*/
static int needs_reflexive (tuple first)
{ tuple ptr = first;
  do
    { if (ptr -> dst -> tag == TAGReflexive)
	return (1);
    }
  while (ptr != first);
  return (0);
}

/*
   We allocate from [%fp] downward
   Note locals have offset < 0 with respect to fp.
   Note that we only align to longword boundaries.
*/
static int allocate_locals (int_list locals)
{ int offset = 0;
  int ix;

  /* Loop over locals, fill in size and offset */
  for (ix = 0; ix < locals -> size; ix++)
    { int vnr = locals -> array[ix];
      var opd = im_vars -> array[vnr];
      int size = vax_size_from_type (opd -> vtype);
      opd -> vsize = size;
      if (opd -> vglob) continue;
      offset -= size;
      opd -> offset = offset;
    };

  return (offset);
}

/*
   Locate proc arguments from 4(%ap)
   Note arguments have offsets > 0 with respect to ap
*/
static void locate_args (decl_list args)
{ int offset = 4;
  int ix;
  for (ix = 0; ix < args -> size; ix++)
    { decl arg = args -> array[ix];
      int vnr = arg -> Formal.vnr;
      var opd = im_vars -> array[vnr];
      opd -> offset = offset;
      if (opd -> vref) offset += 4;
      else offset += vax_size_from_type (opd -> vtype);
    };
}

/*
   Allocate and code global vars
*/
static void code_global_variables (int_list globals)
{ int ix;
  for (ix = 0; ix < globals -> size; ix++)
    { int vnr = globals -> array[ix];
      var opd = im_vars -> array[vnr];
      type vtype = opd -> vtype;
      int vstabs = type_table -> array[vtype -> tnr] -> stabs_nr;
      int size = opd -> vsize;

      if (!opd -> vglob) continue;
      code_line ("\t.local\tG%d\t/* %s */", vnr, opd -> vname);
      code_line ("\t.comm\tG%d,%d,%d", vnr, size, size);
      if (generate_stabs)
        code_line ("\t.stabs\t\"%s:S%d,\",40,0,0,G%d", opd -> vname, vstabs, vnr);
    };
}

/*
   Coding of operands
   Generally the registers are used as followe

   %sp: stack pointer
   %fp: frame pointer
   %ap: argument pointer
*/
static void code_operand (oprd opd)
{ switch (opd -> tag)
    { case TAGIconst: code_string ("$%d", opd -> Iconst.ival); break;
      case TAGRconst: code_string ("$0d%g", opd -> Rconst.rval); break;
      case TAGReflexive: code_string ("*(%%sp)"); break;
      case TAGRts_nil: code_string ("$1"); break;
      case TAGVar:
	{ var obj = im_vars -> array[opd -> Var.vnr];
	  int offset = obj -> offset;
	  if (obj -> vref) code_string ("*");
	  if (obj -> vglob) code_string ("G%d", opd -> Var.vnr);
	  else if (offset > 0) code_string ("%d(%%ap)", offset);
	  else code_string ("%d(%%fp)", offset);
	}; break;
      case TAGLab:
      case TAGRts:
      case TAGNop: dcg_internal_error ("code_operand");
      default: dcg_bad_tag (opd -> tag, "code_operand");
    };
}

static void code_1_oprd_instr (string instr, oprd opd)
{ code_string ("\t%s\t", instr);
  code_operand (opd);
  code_newline ();
}

static void code_2_oprd_instr (string instr, oprd opd1, oprd opd2)
{ code_string ("\t%s\t", instr);
  code_operand (opd1);
  code_string (",");
  code_operand (opd2);
  code_newline ();
}
#define code_2_oprd_tuple(instr,t) code_2_oprd_instr (instr, t -> op1, t -> dst)

static void code_3_oprd_instr (string instr, oprd opd1, oprd opd2, oprd dest)
{ code_string ("\t%s\t", instr);
  code_operand (opd1);
  code_string (",");
  code_operand (opd2);
  code_string (",");
  code_operand (dest);
  code_newline ();
}

/* Note the order of operands in the following macro */
#define code_3_oprd_tuple(instr,t) code_3_oprd_instr (instr, t -> op2, t -> op1, t -> dst)

/*
   Code load operand is used to transfer an operand to a register (pair)
*/
static void code_load_operand (string dd, oprd opd, int regno)
{ switch (opd -> tag)
    { case TAGIconst:
      case TAGRconst:
      case TAGVar:
      case TAGRts_nil:
      case TAGReflexive:
	{ code_string ("\tmov%s\t", dd);
	  code_operand (opd);
	  code_line (",%%r%d", regno);
	}; break;
      case TAGTconst:
	{ int nr = new_const_label ();
	  code_line ("\tmovab\t.CC%d,%%r%d", nr, regno);
	  code_line ("\t.section\t.rodata");
	  code_line ("\t.align\t4");
	  code_line ("\t.short\t-1");
	  code_line ("\t.short\t-1");
	  code_string (".CC%d:\t.asciz\t", nr);
	  code_quoted_string (opd -> Tconst.tval);
	  code_line ("\n\t.text");
	}; break;
      case TAGLab:
      case TAGRts:
      case TAGNop: dcg_internal_error ("code_load_operand");
      default: dcg_bad_tag (opd -> tag, "code_load_operand");
    };
}

static void code_load_address (oprd opd, int regno)
{ switch (opd -> tag)
    { case TAGReflexive:
      case TAGRts_nil:
      case TAGIconst:
      case TAGTconst:
      case TAGLab:
      case TAGRts:
      case TAGNop: dcg_internal_error ("code_load_address");
      case TAGVar:
	{ var obj = im_vars -> array[opd -> Var.vnr];
	  int offset = obj -> offset;
	  if (obj -> vglob)
	    code_line ("\tmovab\tG%d,%%r%d", opd -> Var.vnr, regno);
	  else if (obj -> vref)
	    code_line ("\tmovl\t%d(%%ap),%%r%d", offset, regno);
	  else if (offset > 0)
	    code_line ("\taddl3\t$%d,%%ap,%%r%d", offset, regno);
	  else code_line ("\tsubl3\t$%d,%%fp,%%r%d", -offset, regno);
	}; break;
      default: dcg_bad_tag (opd -> tag, "code_load_address");
    };
}

static void code_store_operand (string dd, oprd opd, int regno)
{ switch (opd -> tag)
    { case TAGVar:
      case TAGReflexive:
	{ code_string ("\tmov%s\t%%r%d,", dd, regno);
	  code_operand (opd);
	  code_newline ();
	}; break;
      case TAGNop: break;
      default: dcg_bad_tag (opd -> tag, "code_store_operand");
    };
}

/*
   Code intro
*/
static void code_typestabs_entries (type_info info)
{ int stabs_nr = info -> stabs_nr;
  type t = info -> typ;
  switch (t -> tag)
    { case TAGRow:
	{ type elt_typ = t -> Row.elt;
	  type_info elt_info = type_table -> array[elt_typ -> tnr];
	  code_line ("\t.stabs\t\":t%d=*%d\",128,0,0,0", stabs_nr, elt_info -> stabs_nr);
	}; break;
      case TAGStruct:
	{ field_list flds = t -> Struct.flds;
	  int ix;
	  code_string ("\t.stabs\t\":t%d=s%d", stabs_nr - 1, indirect_size_from_type (t));
	  for (ix = 0; ix < flds -> size; ix++)
	    { field fld = flds -> array[ix];
	      type ftype = fld -> ftype;
	      type_info ftype_info = type_table -> array[ftype -> tnr];
	      code_string ("%s:%d,%d,%d;", fld -> fname, ftype_info -> stabs_nr,
			   8 * fld -> offset, 8 * vax_size_from_type (ftype));
	    };
	  code_line (";\",128,0,0,0");
	  code_line ("\t.stabs\t\":t%d=*%d\",128,0,0,0", stabs_nr, stabs_nr - 1);
	}; break;
      case TAGTname:
	{ decl id = t -> Tname.id;
	  if (id -> tag == TAGType)
	    { type rtyp = id -> Type.concr;
	      type_info rinfo = type_table -> array[rtyp -> tnr];
	      code_line ("\t.stabs\t\"%s:t%d=%d\",128,0,0,0",
			 t -> Tname.tname, stabs_nr, rinfo -> stabs_nr);
	    };
	};
      default: break;
    };
}

static void code_intro ()
{ int ix;
  if (!generate_stabs) return;
  code_line ("\t/* specify anonymous C types */");
  code_line ("\t.stabs\t\":t1=r1;-2147483648;2147483647;\",128,0,0,0");	/* int */
  code_line ("\t.stabs\t\":t2=r2;0;127;\",128,0,0,0");	/* char */
  code_line ("\t.stabs\t\":t3=r1;8;0;\",128,0,0,0");	/* double */
  code_line ("\t.stabs\t\":t4=4\",128,0,0,0");		/* void */
  code_line ("\t.stabs\t\":t5=*2\",128,0,0,0");		/* char * */
  code_line ("\t.stabs\t\":t6=*5\",128,0,0,0");		/* char ** */
  code_line ("\t.stabs\t\":t7=*4\",128,0,0,0");		/* void * */
  code_string ("\t.stabs\t\":t8=s16stream:7,0,32;fname:5,32,32;");
  code_line ("opened:1,64,32;dir:1,96,32;;\",128,0,0,0");

  code_line ("\n\t/* specify ELAN types */");
  code_line ("\t.stabs\t\"INT:t9=r1;-2147483647;2147483647;\",128,0,0,0");
  code_line ("\t.stabs\t\"BOOL:t10=r1;0;1;\",128,0,0,0");
  code_line ("\t.stabs\t\"REAL:t11=r1;8;0;\",128,0,0,0");
  code_line ("\t.stabs\t\"TEXT:t12=*2\",128,0,0,0");
  code_line ("\t.stabs\t\"FILE:t13=*8\",128,0,0,0");

  code_line ("\n\t/* specify coded ELAN types */");
  for (ix = nr_precoded_types; ix < type_table -> size; ix++)
    code_typestabs_entries (type_table -> array[ix]);
}

/*
   Code program entry
*/
static void code_start (proc p, tuple t)
{ int offset = allocate_locals (p -> locals);
  code_global_variables (p -> locals);
  if (needs_reflexive (t)) offset -= 4;

  /* Code program entry and call to initialize rts */
  code_line ("\t.text");
  code_line ("\t.align\t1");
  code_line ("\t.globl\tmain");
  if (generate_stabs)
    { code_line ("\t.stabs\t\"_main:F1\",36,0,0,main");
      code_line ("\t.stabs\t\"argc:p1\",160,0,0,4");
      code_line ("\t.stabs\t\"argv:p6\",160,0,0,8");
    };
  code_line ("\t.type\tmain,@function");
  code_line ("main:\t.word\t0");
  if (offset) code_line ("\tsubl2\t$%d,%%sp", -offset);
  code_line ("\tpushl\t8(%%ap)");		/* argv @ 8(%ap) */
  code_line ("\tpushl\t4(%%ap)");		/* argc @ 4(%ap) */
  code_line ("\tcalls\t$2,rts_init");
}

/*
   Code program end
*/
static void code_end ()
{ int lnr = new_local_label ();
  code_line ("\tcalls\t$0,rts_finish");
  code_line ("\tsubl2\t%%r0,%%r0");
  code_line ("\tret");
  code_line (".Lfe%d:\t.size\tmain,.Lfe%d-main", lnr, lnr);
}


/*
   Code args stabs
*/
static void code_parameter_stabs_entry (decl arg, int pnr)
{ int vnr = arg -> Formal.vnr;
  var opd = im_vars -> array[vnr];
  type ftyp = arg -> typ;
  type_info finfo = type_table -> array[ftyp -> tnr];
  string facc_info = (arg -> Formal.acc == acc_var)?"v":"p";
  code_line ("\t.stabs\t\"%s:%s%d\",160,0,0,%d",
	     arg -> name, facc_info, finfo -> stabs_nr, opd -> offset);
}

/*
   Code procedure entry
*/
static void code_enter (proc p, tuple t)
{ int offset, ix;

  locate_args (p -> args);
  offset = allocate_locals (p -> locals);
  if (needs_reflexive (t)) offset -= 4;
  code_line ("\t.align\t1");
  if (generate_stabs)
    { type rtype = p -> rtyp;
      type_info rinfo = type_table -> array[rtype -> tnr];
      code_line ("\t.stabs\t\"%s:f%d\",36,0,0,P%d", p -> pname, rinfo -> stabs_nr, p -> pnr);
      for (ix = 0; ix < p -> args -> size; ix++)
	code_parameter_stabs_entry (p -> args -> array[ix], ix + 1);
    };
  code_line ("\t.type\tP%d,@function", p -> pnr);
  code_line ("P%d:\t.word\t0", p -> pnr);
  if (offset) code_line ("\tsubl2\t$%d,%%sp", -offset);
}

/*
   Code procedure exit
*/
static void code_leave (proc p, tuple t)
{ int lnr = new_local_label ();
  type rtype = p -> rtyp;

  /* load return value */
  if (is_a_floating_type (rtype))
    code_load_operand ("d", t -> op2, 0);
  else if (t -> op2 -> tag != TAGNop)
    code_load_operand ("l", t -> op2, 0);

  /* discard frame and return */
  code_line ("\tret");
  code_line (".Lfe%d:\t.size\tP%d,.Lfe%d-P%d", lnr, p -> pnr, lnr, p -> pnr);
}

/*
   Coding of procedure calls
   Note that the call will remove the parameters from the stack
*/

/*
   Prepare call: not needed on vax
*/

/*
   Code load word parameter
   [ lpar, parm, argno, -- ] 
*/
static void code_load_parameter (tuple t)
{ type oprdtype = type_from_operand (t -> op1);
  int is_real = is_a_floating_type (oprdtype);
  if (t -> op1 -> tag == TAGTconst)
    { code_load_operand ("l", t -> op1, 1);
      code_line ("\tpushl\t%%r1");
    }
  else code_1_oprd_instr ((is_real)?"pushd":"pushl", t -> op1);
}

/*
   Code call, store result and discard arguments
   [ call, proc, nr_args, dst ]
*/
static void code_call (tuple t)
{ type desttype = type_from_operand (t -> dst);
  int is_real = is_a_floating_type (desttype);
  int nargs;
  if (t -> op2 -> tag != TAGIconst)
     dcg_bad_tag (t -> op2 -> tag, "code_call");
  nargs = t -> op2 -> Iconst.ival;
  if (t -> dst -> tag == TAGReflexive)
     code_line ("\tmovl\t(%%sp),%d(%%sp)", nargs * 4);

  code_string ("\tcalls\t$%d,", nargs);
  code_called_proc (t -> op1);		/* code call */
  code_newline ();

  code_store_operand ((is_real)?"d":"l", t -> dst, 0);
}

/*
   Code jump
   [ jmp, --, --, dst ]
*/
static void code_jump (tuple t)
{ code_line ("\tjbr\t.BB%d", t -> dst -> Block.bnr);
}

/*
   Code jump if nil
   [ jmp_if_nil, oprd, --, dst ]
*/
static void code_jump_if_nil (tuple t)
{ code_string ("cmpl\t$1,");
  code_operand (t -> op1);
  code_newline ();
  code_line ("\tjeql\t.BB%d", t -> dst -> Block.bnr);
}

/*
   Code jump if null
   [ jmp_if_null, oprd, --, dst ]
*/
static void code_jump_if_null (tuple t)
{ code_string ("tstl\t");
  code_operand (t -> op1);
  code_newline ();
  code_line ("\tjeql\t.BB%d", t -> dst -> Block.bnr);
}

/*
   Code jump if condition
   [ jump_if_int_less_than_int, oprd1, oprd2, dst ]
*/
static void code_cond_jump (tuple t, string opc)
{ code_2_oprd_instr ("cmpl", t -> op1, t -> op2);
  code_line ("\tj%s\t.BB%d", opc, t -> dst -> Block.bnr);
}

/*
   Coding of garbage collection calls
*/

/*
   Allocate space for row/struct
   [ allocate, space in bytes, --, dest ]
*/
static void code_allocate (tuple t)
{ code_1_oprd_instr ("pushl", t -> op1);	/* push nr bytes */
  code_line ("\tcalls\t$1,rts_malloc");		/* call the runtime system */
  code_store_operand ("l", t -> dst, 0);	/* store result */
}

/*
   Set guard on var object and possibly create space for it
   [ guard, oprd, space_in_bytes, dest ]
*/
static void code_guard (tuple t)
{ code_1_oprd_instr ("pushl", t -> op2);	/* push nr bytes */
  code_1_oprd_instr ("pushl", t -> op1);	/* load address */
  code_line ("\tcalls\t$1,rts_guard");		/* guard value */
  code_store_operand ("l", t -> dst, 0);	/* store result away */
}

/*
   Attach object
   [ att&, oprd, --, dest/--/reflexive ]
*/
static void code_attach_object (tuple t)
{ code_load_address (t -> op1, 1);		/* load argument */
  /* check for reflexive destination */
  if (t -> dst -> tag == TAGReflexive)
    code_line ("\tmovl\t%%r1(%%sp)");
  code_line ("\tpushl\t%%r1");			/* push address of arg */
  code_line ("\tcalls\t$1,rts_attach");		/* attach value */
  code_store_operand ("l", t -> dst, 0);	/* store result away */
}

/*
   Attach object pointed to by oprd + offset
   [ att_off, addr, offset, dest/--/reflexive ]
*/
static void code_attach_offset (tuple t)
{ int fixed_offset = 0;				/* if fixed offset, default is 0 */
  code_load_operand ("l", t -> op1, 1);		/* load address */
  switch (t -> op2 -> tag)
    { case TAGIconst: fixed_offset = t -> op2 -> Iconst.ival;
      case TAGNop:
  	if (fixed_offset) code_line ("\taddl2\t$%d,%%r1", fixed_offset);
	break;
      default:
	code_string ("\taddl2\t");
	code_operand (t -> op2);
	code_line (",%%r1");
    };

  /* check for reflexive destination */
  if (t -> dst -> tag == TAGReflexive)
    code_line ("\tmovl\t%%r1,(%%sp)");
  code_line ("\tpushl\t%%r1");			/* push address of arg */
  code_line ("\tcalls\t$1,rts_attach");		/* attach value */
  code_store_operand ("l", t -> dst, 0);	/* special code to handle the return */
}

/*
   Detach variable. Note that rts_detach is called by reference
   [ det&_adr, oprd, --, -- ]
*/
static void code_detach_object (tuple t)
{ code_1_oprd_instr ("pushab", t -> op1);	/* push adr of object */
  code_line ("\tcalls\t$1,rts_detach");		/* detach value */
}

/*
   Detach object pointed to by oprd + offset
   [ det_off, addr, offset, -- ]
*/
static void code_detach_offset (tuple t)
{ int fixed_offset = 0;				/* if fixed offset, default is 0 */
  code_load_operand ("l", t -> op1, 1);		/* load address */
  switch (t -> op2 -> tag)
    { case TAGIconst: fixed_offset = t -> op2 -> Iconst.ival;
      case TAGNop:
  	if (fixed_offset) code_line ("\taddl2\t$%d,%%r1", fixed_offset);
	break;
      default:
	code_string ("\taddl2\t");
	code_operand (t -> op2);
	code_line (",%%r1");
    };
  code_line ("\tpushl\t%%r1");			/* push address of arg */
  code_line ("\tcalls\t$1,rts_detach");		/* detach value */
}

static void code_predetach_object (tuple t)
{ code_1_oprd_instr ("pushab", t -> op1);	/* push adr of object */
  code_line ("\tcalls\t$1,rts_predetach");	/* predetach value */
  code_store_operand ("l", t -> dst, 0);	/* store result away */
}

/*
   Coding of copy actions
*/

static void code_copy_adr (tuple t)
{ if (t -> op1 -> tag != TAGTconst)
    code_2_oprd_tuple ("movl", t);
  else
    { code_load_operand ("l", t -> op1, 1);
      code_store_operand ("l", t -> dst, 1);
    };
}

static void code_copy_indirect (string dd, tuple t)
{ code_load_operand (dd, t -> op1, 2);		/* load operand */
  code_load_operand ("l", t -> dst, 1);		/* load target address */
  code_line ("\tmov%s\t%%r2,(%%r1)", dd);	/* store it away */
}

static void code_indirect_copy (string dd, tuple t)
{ code_load_operand ("l", t -> op1, 1);		/* load source address */
  code_line ("\tmov%s\t(%%r1),%%r2", dd);	/* load operand */
  code_store_operand (dd, t -> dst, 2);		/* store it away */
}

/*
   Rts support actions
*/

/*
   Code test for nil
   [ is_nil, oprd, --, dest]
*/
static void code_is_nil (tuple t)
{ int lnr = new_local_label ();
  code_line ("\tmovl\t$1,%%r1");		/* preload TRUE */
  code_string ("\tcmpl\t$1,");			/* is it nil */
  code_operand (t -> op1);
  code_newline ();
  code_line ("\tbeql\t.LL%d", lnr);		/* Ok if so */
  code_line ("\tsubl2\t%%r1,%%r1");		/* make FALSE */
  code_string (".LL%d:", lnr);
  code_store_operand ("l", t -> dst, 1);	/* store it away */
}

/*
   Code to add an address and an offset
   [ A+I, adr_oprd, int_oprd, dest ]
*/
static void code_addr_plus_offset (tuple t)
{ code_load_operand ("l", t -> op1, 1);
  code_line ("\ttstl\t%%r1");
  code_line ("\tjeql\trts_offset_from_0");
  if (t -> op2 -> tag == TAGIconst)
    { int fixed_offset = t -> op2 -> Iconst.ival;
      if (fixed_offset)
	code_line ("\taddl2\t$%d,%%r1", fixed_offset);
    }
  else
    { code_string ("\taddl2\t");
      code_operand (t -> op2);
      code_line (",%%r1");
    };
  code_store_operand ("l", t -> dst, 1);
}

/*
   Code to adjust the row index and check its bounds
   [ row_idx, oprd, upb, dest ]
*/
static void code_row_index (tuple t)
{ code_load_operand ("l", t -> op1, 1);
  code_line ("\tdecl\t%%r1");
  code_line ("\tjlss\trts_index_out_of_range");
  code_store_operand ("l", t -> dst, 1);
  code_string ("\tcmpl\t%%r1,");
  code_operand (t -> op2);
  code_newline ();
  code_line ("\tjgeq\trts_index_out_of_range");
}

/*
   code integer ops
   [ int op, oprd1, oprd2, dst ]
*/
static void code_int_mod (tuple t, int mod)
{ code_load_operand ("l", t -> op1, 2);
  code_load_operand ("l", t -> op2, 4);
  code_line ("\tdivl3\t%%r4,%%r2,%%r6");
  code_line ("\tmull2\t%%r4,%%r6");
  code_line ("\tsubl2\t%%r6,%%r2");
  code_store_operand ("l", t -> dst, 2);
}

static void code_negate (tuple t)
{ code_string ("\tsubl3\t");
  code_operand (t -> op1);
  code_string (",$0,");
  code_operand (t -> dst);
  code_newline ();
}

static void code_negate_real (tuple t)
{ code_string ("\tsubd3\t");
  code_operand (t -> op1);
  code_string (",$0d0.0,");
  code_operand (t -> dst);
  code_newline ();
}

static void code_compare (int fl, tuple t, string tcc)
{ int lnr = new_local_label ();
  code_line ("\tmovl\t$1,%%r1");		/* preload TRUE */
  code_2_oprd_instr ((fl)?"cmpd":"cmpl", t -> op1, t -> op2);
  code_line ("\tj%s\t.LL%d", tcc, lnr);
  code_line ("\tsubl2\t%%r1,%%r1");		/* make FALSE */
  code_string (".LL%d:", lnr);
  code_store_operand ("l", t -> dst, 1);
}

static void code_and (tuple t)
{ code_string ("\tmcoml\t");
  code_operand (t -> op2);
  code_line (",%%r1");
  code_string ("\tbicl3\t%%r1,");
  code_operand (t -> op1);
  code_string (",");
  code_operand (t -> dst);
  code_newline ();
}

static void code_not (tuple t)
{ code_string ("\txorl3\t$1,");
  code_operand (t -> op1);
  code_string (",");
  code_operand (t -> dst);
  code_newline ();
}

static void code_sline (proc p, tuple t)
{ int lnr;
  if (!generate_stabs) return;
  if (t -> op1 -> tag != TAGIconst) dcg_internal_error ("code_sline");
  lnr = new_local_label ();
  if (p -> pnr)
    code_line ("\t.stabn\t68,0,%d,.Lline%d-P%d", t -> op1 -> Iconst.ival, lnr, p -> pnr);
  else code_line ("\t.stabn\t68,0,%d,.Lline%d-main", t -> op1 -> Iconst.ival, lnr);
  code_line (".Lline%d:", lnr);
}

/*
   The coder dispatcher
*/
static void code_tuple (proc p, tuple t)
{ switch (t -> opc)
    { /* General */
      case start_program:	   code_start (p, t); break;
      case end_program:		   code_end (p); break;

      /* Procedure calls */
      case prepare_call:	   break;
      case load_parameter:	   code_load_parameter (t); break;
      case load_address_parameter: code_1_oprd_instr ("pushab", t -> op1); break;
      case call_procedure:
      case call_attach_procedure:
      case call_detach_procedure:
      case call_guard_procedure:   code_call (t); break;

      /* Procedure entry and return */
      case enter_procedure:	   code_enter (p, t); break;
      case leave_procedure:	   code_leave (p, t); break;

      /* Jumps */
      case jump: 		   code_jump (t); break;
      case jump_if_nil:		   code_jump_if_nil (t); break;
      case jump_if_null:	   code_jump_if_null (t); break;
      case jump_if_addr_equal_addr:
      case jump_if_int_equal_int:	  code_cond_jump (t, "eql"); break;
      case jump_if_int_notequal_int:	  code_cond_jump (t, "neq"); break;
      case jump_if_int_less_than_int: 	  code_cond_jump (t, "lss"); break;
      case jump_if_int_less_equal_int:    code_cond_jump (t, "leq"); break;
      case jump_if_int_greater_than_int:  code_cond_jump (t, "gtr"); break;
      case jump_if_int_greater_equal_int: code_cond_jump (t, "geq"); break;

      /* Garbage collection */
      case undefine:		   code_1_oprd_instr ("clrl", t -> dst); break;
      case allocate:		   code_allocate (t); break;
      case guard:		   code_guard (t); break;
      case attach_adr:		   code_attach_object (t); break;
      case attach_offset:	   code_attach_offset (t); break;
      case detach_adr:		   code_detach_object (t); break;
      case detach_offset:	   code_detach_offset (t); break;
      case predetach_adr:	   code_predetach_object (t); break;

      /* Copying stuff */
      case copy_adr_of: 	   code_2_oprd_tuple ("movab", t); break;
      case copy_int:		   code_2_oprd_tuple ("movl", t); break;
      case copy_adr:		   code_copy_adr (t); break;
      case copy_int_indirect:
      case copy_adr_indirect:	   code_copy_indirect ("l", t); break;
      case copy_indirect_int:
      case copy_indirect_adr:	   code_indirect_copy ("l", t); break;
      case copy_real:		   code_2_oprd_tuple ("movd", t); break;
      case copy_real_indirect:	   code_copy_indirect ("d", t); break;
      case copy_indirect_real:	   code_indirect_copy ("d", t); break;

      /* Rts support ops */
      case is_nil:		   code_is_nil (t); break;
      case addr_plus_offset:	   code_addr_plus_offset (t); break;
      case row_index:		   code_row_index (t); break;

      /* Integer ops */
      case int_plus_int:	   code_3_oprd_tuple ("addl3", t); break;
      case int_minus_int:	   code_3_oprd_tuple ("subl3", t); break;
      case int_times_int:	   code_3_oprd_tuple ("mull3", t); break;
      case int_div_int:		   code_3_oprd_tuple ("divl3", t); break;
      case int_mod_int:		   code_int_mod (t, 1); break;
      case negate_int:		   code_negate (t); break;
      case int_equal_int:	   code_compare (0, t, "eql"); break;
      case int_notequal_int:	   code_compare (0, t, "neq"); break;
      case int_less_than_int: 	   code_compare (0, t, "lss"); break;
      case int_less_equal_int:     code_compare (0, t, "leq"); break;
      case int_greater_than_int:   code_compare (0, t, "gtr"); break;
      case int_greater_equal_int:  code_compare (0, t, "geq"); break;

      /* Real ops */
      case real_plus_real:	   code_3_oprd_tuple ("addd3", t); break;
      case real_minus_real:	   code_3_oprd_tuple ("subd3", t); break;
      case real_times_real:	   code_3_oprd_tuple ("muld3", t); break;
      case real_divide_real:	   code_3_oprd_tuple ("divd3", t); break;
      case negate_real:		   code_negate_real (t); break;
      case real_equal_real:	   code_compare (1, t, "eql"); break;
      case real_notequal_real:     code_compare (1, t, "neq"); break;
      case real_less_than_real:    code_compare (1, t, "lss"); break;
      case real_less_equal_real:   code_compare (1, t, "leq"); break;
      case real_greater_than_real: code_compare (1, t, "gtr"); break;
      case real_greater_equal_real:code_compare (1, t, "geq"); break;

      /* Bool ops */
      case bool_and_bool: 	   code_and (t); break;
      case bool_or_bool: 	   code_3_oprd_tuple ("bisl3", t); break;
      case not_bool: 		   code_not (t); break;

      /* Text ops */
      /* Conversions */
      case int_to_real:		   code_2_oprd_tuple ("cvtld", t); break;

      /* Miscellaneous */
      case sline:		   code_sline (p, t);
      case comment:
      case nop:			   break;

      default: dcg_bad_tag (t -> opc, "code_tuple"); 
    };
}

/*
   Later on, we will have to extend this part for basic blocks
   Note add param in machdep for local label prefix
*/
/*
   Code basic block and procedure
*/
static void code_segment (proc p, block b)
{ tuple ptr = b -> anchor;
  code_block_label (b);
  do
    { code_comment_tuple (ptr);
      code_tuple (p, ptr);
      ptr = ptr -> next;
    }
  while (ptr != b -> anchor);
}

static void code_procedure (proc p)
{ block bptr;
  code_line ("\n/* %s */", p -> cname);
  for (bptr = p -> init_block; bptr != block_nil; bptr = bptr -> direct_succ)
    code_segment (p, bptr);
}

void vax_code_generation ()
{ int ix;
  dcg_hint ("      generating vax11 machine code...");
  code_intro ();
  for (ix = 0; ix < im_procs -> size; ix++)
    code_procedure (im_procs -> array[ix]);
}
