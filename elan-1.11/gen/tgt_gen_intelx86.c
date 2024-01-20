/*
   File: tgt_gen_intelx86.c
   Generates Intelx86 machine code

   CVS ID: "$Id: tgt_gen_intelx86.c,v 1.12 2011/08/13 15:58:54 marcs Exp $"
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
#include "tgt_gen_machdep.h"
#include "tgt_gen_common.h"
#include "tgt_gen_intelx86.h"

/*
   Intelx86 properties
   Basically:
      sizeof (VOID) = 0;
      sizeof (INT) = sizeof (void *) = 4;
      sizeof (REAL) = 8;
*/
int intelx86_size_from_type (type t)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType)
		       dcg_internal_error ("intelx86_size_from_type");
		    return (intelx86_size_from_type (id -> Type.concr));
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
	       default: dcg_bad_tag (t -> tag, "intelx86_size_from_type");
	     };
	  return (0);
	};

int intelx86_regsize_from_arg (pdecl pd)
	{ switch (pd -> pacc)
	     { case acc_var:
	       case acc_proc: return (1);
	       default: return (intelx86_size_from_type (pd -> ptype) / 4);
	     };
	};

/*
   Intelx86 coding routines
*/
/*
   We allocate from [%fp] downward
   Since we only have two alignments, we first allocate double word objects
   and then all single word objects. Finally make the frame double word aligned
*/
static int allocate_locals (int_list locals)
	{ int offset = 0;
	  int ix;

	  /* Loop over locals, fill in size, allocate doubles */
	  for (ix = 0; ix < locals -> size; ix++)
	     { int vnr = locals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       int size = intelx86_size_from_type (opd -> vtype);
	       opd -> vsize = size;
	       if (size & 4) continue;
	       if (opd -> vglob) continue;
	       offset -= 8;
	       opd -> offset = offset;
	     };
	  for (ix = 0; ix < locals -> size; ix++)
	     { int vnr = locals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       if (opd -> vglob) continue;
	       if (opd -> vsize == 4)
		  { offset -= 4;
		    opd -> offset = offset;
		  };
	       if (debug) dcg_hint ("      gave %s offset %d", opd -> vname, opd -> offset);
	     };

	  /* make sure %esp is double word aligned */
	  if (offset & 4) offset -= 4;
	  return (offset);
	};

/*
   Locate proc arguments
*/
static void locate_args (decl_list args)
	{ int offset = 8;
	  int ix;
	  for (ix = 0; ix < args -> size; ix++)
	     { decl arg = args -> array[ix];
	       int vnr = arg -> Formal.vnr;
	       var opd = im_vars -> array[vnr];
	       opd -> offset = offset;
	       if (opd -> vref) offset += 4;
	       else offset += intelx86_size_from_type (opd -> vtype);
	     };
	};

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
	       if (old_os)	/* MINGW32 */
		  { code_line ("\t.align\t%d", size);
		    code_line ("\t.comm\tG%d,%d\t/* %s */", vnr, size, opd -> vname);
		  }
	       else
		  { code_line ("\t.local\tG%d\t/* %s */", vnr, opd -> vname);
		    code_line ("\t.comm\tG%d,%d,%d", vnr, size, size);
		  };
	       if (generate_stabs)
		  code_line ("\t.stabs\t\"%s:S%d,\",40,0,0,G%d", opd -> vname, vstabs, vnr);
	     };
	};

/*
   Coding of operands
   Generally the registers are used as followed:

   %esp: stack pointer
   %ebp: frame pointer
   %eax: general register
   %ebx: general address register (always free for indirection)
   %ecx: general data register
   %edx: auxilary register and used for multi register arithmetic 
   Still free but perhaps useful: %esi, %edi
*/

/*
   Code load word operand is used to transfer int/addr operands to a register
*/
static void code_load_word_operand (oprd opd, string reg)
	{ switch (opd -> tag)
	     { case TAGIconst:
		  code_line ("\tmovl\t$%d,%%%s", opd -> Iconst.ival, reg);
		  break;
	       case TAGTconst:
		  { int nr = new_const_label ();
		    code_line ("\tleal\tCC%d,%%%s", nr, reg);
		    code_line ("\t.section\t.rodata");
		    code_line ("\t.align\t4");
		    code_line ("\t.short\t-1");
		    code_line ("\t.short\t-1");
		    code_string ("CC%d:\t.asciz\t", nr);
		    code_quoted_string (opd -> Tconst.tval);
		    code_line ("\n\t.text");
		  }; break;
	       case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    if (obj -> vglob)
		       code_line ("\tmovl\tG%d,%%%s", opd -> Var.vnr, reg);
		    else if (obj -> vref)
		       { code_line ("\tmovl\t%d(%%ebp),%%ebx", obj -> offset);
			 code_line ("\tmovl\t(%%ebx),%%%s", reg);
		       }
		    else code_line ("\tmovl\t%d(%%ebp),%%%s", obj -> offset, reg);
		  }; break;
	       case TAGRts_nil:
		  code_line ("\tmovl\t$1,%%%s", reg);
		  break;
	       case TAGReflexive:
	       case TAGRconst:
	       case TAGLab:
	       case TAGRts:
	       case TAGNop: dcg_internal_error ("code_load_word_operand");
	       default: dcg_bad_tag (opd -> tag, "code_load_word_operand");
	     };
	};

/* may use %ebx on storing VAR parameters */
static void code_instr_and_real_operand (string instr, oprd opd)
	{ switch (opd -> tag)
	     { case TAGIconst:
		  code_line ("\t%s\t$%d", instr, opd -> Iconst.ival);
		  break;
	       case TAGRconst:
		  { int nr = new_const_label ();
		    code_line ("\t.section\t.rodata");
		    code_line ("\t.align\t8");
		    code_line ("CC%d:\t.double\t0e%e", nr, opd -> Rconst.rval);
		    code_line ("\t.text");
		    code_line ("\t%s\tCC%d", instr, nr);
		  }; break;
	       case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    if (obj -> vglob)
		       code_line ("\t%s\tG%d", instr, opd -> Var.vnr);
		    else if (obj -> vref)
		       { code_line ("\tmovl\t%d(%%ebp),%%ebx", obj -> offset);
			 code_line ("\t%s\t(%%ebx)", instr);
		       }
		    else code_line ("\t%s\t%d(%%ebp)", instr, obj -> offset);
		  }; break;
	       case TAGReflexive:
	       case TAGRts_nil:
	       case TAGTconst:
	       case TAGLab:
	       case TAGRts:
	       case TAGNop: dcg_internal_error ("code_instr_and_real_operand");
	       default: dcg_bad_tag (opd -> tag, "code_instr_and_real_operand");
	     };
	};

static void code_load_address (oprd opd, string reg)
	{ switch (opd -> tag)
	     { case TAGReflexive:
	       case TAGRts_nil:
	       case TAGIconst:
	       case TAGTconst:
	       case TAGLab:
	       case TAGNop: dcg_internal_error ("code_load_address");
	       case TAGRts:
	       case TAGProc:
		  { code_string ("\tleal\t");
		    code_called_proc (opd);
		    code_line (",%%%s", reg);
		  }; break;
	       case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    if (obj -> vglob)
		       code_line ("\tleal\tG%d,%%%s", opd -> Var.vnr, reg);
		    else if (obj -> vref)
		       code_line ("\tmovl\t%d(%%ebp),%%%s", obj -> offset, reg);
		    else code_line ("\tleal\t%d(%%ebp),%%%s", obj -> offset, reg);
		  }; break;
	       default: dcg_bad_tag (opd -> tag, "code_load_address");
	     };
	};

/* may use %ebx on storing VAR/Reflexive parameters */
static void code_store_word_operand (oprd opd, string reg)
	{ var obj;
	  switch (opd -> tag)
	     { case TAGVar:
		  { obj = im_vars -> array[opd -> Var.vnr];
		    if (obj -> vglob)
		       code_line ("\tmovl\t%%%s,G%d", reg, opd -> Var.vnr);
		    else if (obj -> vref)
		       { code_line ("\tmovl\t%d(%%ebp),%%ebx", obj -> offset);
		         code_line ("\tmovl\t%%%s,(%%ebx)", reg);
		       }
		    else code_line ("\tmovl\t%%%s,%d(%%ebp)", reg, obj -> offset);
		  }; break;
	       case TAGReflexive:
		  { code_line ("\tmovl\t(%%esp),%%ebx");	/* pick up pointer to arg */
		    code_line ("\tmovl\t%%%s,(%%ebx)", reg);	/* overwrite @pointer */
		  };
	       case TAGNop: break;
	       default: dcg_bad_tag (opd -> tag, "code_store_word_operand");
	     };
	};

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
		    code_string ("\t.stabs\t\":t%d=s%d", stabs_nr - 1,
				 indirect_size_from_type (t));
		    for (ix = 0; ix < flds -> size; ix++)
		       { field fld = flds -> array[ix];
			 type ftype = fld -> ftype;
			 type_info ftype_info = type_table -> array[ftype -> tnr];
		         code_string ("%s:%d,%d,%d;", fld -> fname, ftype_info -> stabs_nr,
				      8 * fld -> offset, 8 * intelx86_size_from_type (ftype));
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
	};

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
	};

/*
   Code program entry
*/
static void code_start (proc p)
	{ int offset = allocate_locals (p -> locals);
	  code_global_variables (p -> locals);

	  /* Code program entry and call to initialize rts */
	  code_line ("\t.text");
	  code_line ("\t.align\t16");
	  code_string ("\t.globl\t");
	  code_proc_label ("main");
	  code_newline ();
	  if (!old_os)	/* No Mingw32 */
	     code_line ("\t.type\tmain,@function");
	  if (generate_stabs)
	     { code_line ("\t.stabs\t\"_main:F1\",36,0,0,main");
	       code_line ("\t.stabs\t\"argc:p1\",160,0,0,8");
	       code_line ("\t.stabs\t\"argv:p6\",160,0,0,12");
	     };
	  code_proc_label ("main");
	  code_line (":\tpushl\t%%ebp");
	  code_line ("\tmovl\t%%esp,%%ebp");
	  if (offset) code_line ("\tsubl\t$%d,%%esp", -offset);
	  code_line ("\tpushl\t%%ebx");
	  code_line ("\tpushl\t%%esi");
	  code_line ("\tpushl\t%%edi");
	  code_line ("\tpushl\t12(%%ebp)");		/* argv @ 12 + 12 */
	  code_line ("\tpushl\t8(%%ebp)");		/* argc @ 8 + 12 */
	  code_string ("\tcall\t");			/* code call to init rts */
	  code_proc_label ("rts_init");
	  code_newline ();
	  code_line ("\taddl\t$8,%%esp");
	};

/*
   Code program end
*/
static void code_end ()
	{ int lnr = new_local_label ();
	  code_string ("\tcall\t");			/* code call to finish rts */
	  code_proc_label ("rts_finish");
	  code_newline ();
	  code_line ("\tpopl\t%%edi");
	  code_line ("\tpopl\t%%esi");
	  code_line ("\tpopl\t%%ebx");
	  code_line ("\tmovl\t%%ebp,%%esp");
	  code_line ("\tpopl\t%%ebp");			/* Reload old fp */
	  code_line ("\txorl\t%%eax,%%eax");
	  code_line ("\tret");
	  if (!old_os)	/* No Mingw32 */
	     code_line (".Lfe%d:\t.size\tmain,.Lfe%d-main", lnr, lnr);
	};

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
	};

/*
   Code procedure entry
*/
static void code_enter (proc p, tuple t)
	{ int offset, ix;

	  locate_args (p -> args);
	  offset = allocate_locals (p -> locals);
	  code_line ("\t.align\t16");
	  if (!old_os)	/* No Mingw32 */
	     code_line ("\t.type\tP%d,@function", p -> pnr);
	  if (generate_stabs)
	     { type rtype = p -> rtyp;
	       type_info rinfo = type_table -> array[rtype -> tnr];
	       code_line ("\t.stabs\t\"%s:f%d\",36,0,0,P%d",
			  p -> pname, rinfo -> stabs_nr, p -> pnr);
	       for (ix = 0; ix < p -> args -> size; ix++)
		  code_parameter_stabs_entry (p -> args -> array[ix], ix + 1);
	     };
	  code_line ("P%d:\tpushl\t%%ebp", p -> pnr);
	  code_line ("\tmovl\t%%esp,%%ebp");
	  if (offset) code_line ("\tsubl\t$%d,%%esp", -offset);
	};

/*
   Code procedure exit
*/
static void code_leave (proc p, tuple t)
	{ int lnr = new_local_label ();
	  type rtype = p -> rtyp;

	  /* load return value */
	  if (is_a_floating_type (rtype))
	     code_instr_and_real_operand ("fldl", t -> op2);
	  else if (t -> op2 -> tag != TAGNop)
	     code_load_word_operand (t -> op2, "eax");

	  /* discard frame and return */
	  code_line ("\tmovl\t%%ebp,%%esp");
	  code_line ("\tpopl\t%%ebp");
	  code_line ("\tret");
	  if (!old_os)	/* No Mingw32 */
	     code_line (".Lfe%d:\t.size\tP%d,.Lfe%d-P%d", lnr, p -> pnr, lnr, p -> pnr);
	};

/*
   Coding of procedure calls
   Note that the call removes the parameters from the stack
*/

/*
   Prepare call: on the intel make sure that %sp will be
   double word aligned upon procedure entry.
   [ pcall, nr_args, --, -- ]
*/
static void code_prepare_call (tuple t)
	{ int nr;
	  if (t -> op1 -> tag != TAGIconst)
	     dcg_bad_tag (t -> op1 -> tag, "code_prepare_call");
	  nr = t -> op1 -> Iconst.ival;

	  if (nr & 1)
	     code_line ("\tsubl\t$4,%%esp");
	};

/*
   Coding of subroutine parameter loading and result storing
   Note that if the parameter is a Nop, nothing gets coded
*/
static void code_load_parameter (oprd opd)
	{ type oprdtype = type_from_operand (opd);
	  if (opd -> tag == TAGNop) return;
	  else if (is_a_floating_type (oprdtype))
	     { code_line ("\tsubl\t$8,%%esp");
	       code_instr_and_real_operand ("fldl", opd);
	       code_line ("\tfstpl\t(%%esp)");
	     }
	  else /* single word operand */
	     { code_load_word_operand (opd, "eax");
	       code_line ("\tpushl\t%%eax");
	     };
	};

static void code_store_result (oprd dest)
	{ type desttype = type_from_operand (dest);
	  if (is_a_floating_type (desttype))
	     code_instr_and_real_operand ("fstpl", dest);
	  else code_store_word_operand (dest, "eax");
	};

/*
   Code load word parameter
   [ lpar, parm, argno, -- ] 
*/
static void code_tuple_load_parameter (tuple t)
	{ code_load_parameter (t -> op1);
	};

/*
   Code load address of parameter
   [ lpar&, parm, argno, --]
*/
static void code_load_address_parameter (tuple t)
	{ code_load_address (t -> op1, "ebx");
	  code_line ("\tpushl\t%%ebx");
	};

/*
   Code call to runtime system
*/
static void code_rts_call (tuple t, string rts_name)
	{ /* Determine space needed by arguments */
	  int nr = 0;

	  /* Count nr of words for operand 2 */
	  if (t -> op2 -> tag == TAGNop) ;
	  else if (is_a_floating_type (type_from_operand (t -> op2))) nr += 2;
	  else nr++;

	  /* Count nr of words for operand 2 */
	  if (t -> op1 -> tag == TAGNop) ;
	  else if (is_a_floating_type (type_from_operand (t -> op1))) nr += 2;
	  else nr++;

	  /* Code stack frame alignment and push the parameters */
	  if (nr & 1)
	     { code_line ("\tsubl\t$4,%%esp");
	       nr++;
	     };

	  /* Code the loading of the arguments */
	  code_load_parameter (t -> op2);
	  code_load_parameter (t -> op1);

	  /* Code the actual call, store the result and discard parameters */
	  code_string ("\tcall\t");
	  code_proc_label (rts_name);
	  code_newline ();
	  code_store_result (t -> dst);
	  if (nr) code_line ("\taddl\t$%d,%%esp", nr * 4);
	};

/*
   Code call, store result and discard arguments
   [ call, proc, nr_args, dst ]
*/
static void code_actual_call (oprd opd)
	{ if (opd -> tag == TAGVar)
	     { code_load_word_operand (opd, "ebx");
	       code_line ("\tcall\t*%%ebx");
	     }
	  else
	     { code_string ("\tcall\t");
	       code_called_proc (opd);
	       code_newline ();
	     };
	};

static void code_call (tuple t)
	{ int nr;
	  if (t -> op2 -> tag != TAGIconst)
	     dcg_bad_tag (t -> op2 -> tag, "code_call");
	  nr = t -> op2 -> Iconst.ival;		/* nr of words for args */
	  if (nr & 1) nr++;			/* align to double word */
	  code_actual_call (t -> op1);
	  code_store_result (t -> dst);
	  if (nr) code_line ("\taddl\t$%d,%%esp", nr * 4);
	};

/*
   Code jump
   [ jmp, --, --, dst ]
*/
static void code_jump (tuple t)
	{ code_string ("\tjmp\t");
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Code jump if nil
   [ jmp_if_nil, oprd, --, dst ]
*/
static void code_jump_if_nil (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tdec\t%%eax");
	  code_string ("\tjz\t");
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Code jump if null
   [ jmp_if_null, oprd, --, dst ]
*/
static void code_jump_if_null (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tandl\t%%eax,%%eax");
	  code_string ("\tjz\t");
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Code jump if condition
   [ jump_if_int_less_than_int, oprd1, oprd2, dst ]
*/
static void code_cond_jump (tuple t, string opc)
	{ code_load_word_operand (t -> op1, "eax");
	  code_load_word_operand (t -> op2, "ecx");
	  code_line ("\tsubl\t%%ecx,%%eax");
	  code_string ("\tj%s\t", opc);
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Coding of garbage collection calls
*/
static void code_undefine (tuple t)
	{ code_line ("\tsubl\t%%eax,%%eax");
	  code_store_word_operand (t -> dst, "eax");
	};

/*
   Allocate space for row/struct
   [ allocate, space in bytes, --, dest ]
*/
static void code_allocate (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tsubl\t$4,%%esp");		/* keep esp double aligned */
	  code_line ("\tpushl\t%%eax");
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_malloc");
	  code_newline ();
	  code_line ("\taddl\t$8,%%esp");		/* discard arg and gap */
	  code_store_word_operand (t -> dst, "eax");
	};

/*
   Set guard on var object and possibly create space for it
   [ guard, oprd, space_in_words, dest ]
*/
static void code_guard (tuple t)
	{ code_load_word_operand (t -> op2, "eax");	/* load nr bytes */
	  code_load_word_operand (t -> op1, "ebx");	/* load address */
	  code_line ("\tpushl\t%%eax");			/* push args */
	  code_line ("\tpushl\t%%ebx");
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_guard");		/* to guard the value */
	  code_newline ();
	  code_line ("\taddl\t$8,%%esp");		/* discard args */
	  code_store_word_operand (t -> dst, "eax");	/* store result away */
	};

/*
   Attach object
   [ att&, oprd, --, dest/--/reflexive ]
*/
static void code_attach_object (tuple t)
	{ code_load_address (t -> op1, "ebx");		/* load argument */
	  code_line ("\tsubl\t$4,%%esp");		/* keep esp double aligned */
	  code_line ("\tpushl\t%%ebx");			/* push address of arg */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_attach");		/* to attach value */
	  code_newline ();
	  code_store_word_operand (t -> dst, "eax");	/* special code to handle the return */
	  code_line ("\taddl\t$8,%%esp");		/* discard arg and gap */
	};

/*
   Attach object pointed to by oprd + offset
   [ att_off, addr, offset, dest/--/reflexive ]
*/
static void code_attach_offset (tuple t)
	{ int fixed_offset = 0;				/* if fixed offset, default is 0 */
	  code_load_word_operand (t -> op1, "ebx");	/* load address */
	  switch (t -> op2 -> tag)
	     { case TAGIconst: fixed_offset = t -> op2 -> Iconst.ival;
	       case TAGNop:
	  	  if (fixed_offset) code_line ("\taddl\t$%d,%%ebx", fixed_offset);
		  break;
	       default:
	          code_load_word_operand (t -> op2, "eax");
	          code_line ("\taddl\t%%eax,%%ebx");
	     };
	  code_line ("\tsubl\t$4,%%esp");		/* keep esp double aligned */
	  code_line ("\tpushl\t%%ebx");			/* push address of arg */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_attach");		/* to attach value */
	  code_newline ();
	  code_store_word_operand (t -> dst, "eax");	/* special code to handle the return */
	  code_line ("\taddl\t$8,%%esp");		/* discard arg and gap */
	};

/*
   Detach variable. Note that rts_detach is called by reference
   [ det&_adr, oprd, --, -- ]
*/
static void code_detach_object (tuple t)
	{ code_load_address (t -> op1, "ebx");		/* load addres */
	  code_line ("\tsubl\t$4,%%esp");		/* keep esp double aligned */
	  code_line ("\tpushl\t%%ebx");			/* push address of arg */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_detach");		/* to detach value */
	  code_newline ();
	  code_line ("\taddl\t$8,%%esp");		/* discard arg and gap */
	};

/*
   Detach object pointed to by oprd + offset
   [ det_off, addr, offset, -- ]
*/
static void code_detach_offset (tuple t)
	{ int fixed_offset = 0;				/* if fixed offset, default is 0 */
	  code_load_word_operand (t -> op1, "ebx");	/* load address */
	  switch (t -> op2 -> tag)
	     { case TAGIconst: fixed_offset = t -> op2 -> Iconst.ival;
	       case TAGNop:
	  	  if (fixed_offset) code_line ("\taddl\t$%d,%%ebx", fixed_offset);
		  break;
	       default:
	          code_load_word_operand (t -> op2, "eax");
	          code_line ("\taddl\t%%eax,%%ebx");
	     };
	  code_line ("\tsubl\t$4,%%esp");		/* keep esp double aligned */
	  code_line ("\tpushl\t%%ebx");			/* push address of arg */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_detach");		/* to detach value */
	  code_newline ();
	  code_line ("\taddl\t$8,%%esp");		/* discard arg and gap */
	};

static void code_predetach_object (tuple t)
	{ code_load_address (t -> op1, "ebx");		/* load address */
	  code_line ("\tsubl\t$4,%%esp");		/* keep esp double aligned */
	  code_line ("\tpushl\t%%ebx");
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_predetach");		/* to predetach value */
	  code_newline ();
	  code_line ("\taddl\t$8,%%esp");		/* discard arg and gap */
	  code_store_word_operand (t -> dst, "eax");	/* store result away */
	};

/*
   Coding of copy actions
*/

/*
   Code copy address of
   [ :=&, oprd, --, dst ]
*/
static void code_copy_adr_of (tuple t)
	{ code_load_address (t -> op1, "edx");		/* load address */
	  code_store_word_operand (t -> dst, "edx");	/* store it away */
	};

/*
   Code copy
   [ :=I, oprd, --, dst ], [ :=A, oprd, --, dst ]
*/
static void code_copy (tuple t)
	{ code_load_word_operand (t -> op1, "eax");	/* load operand */
	  code_store_word_operand (t -> dst, "eax");	/* store it away */
	};

static void code_copy_indirect (tuple t)
	{ code_load_word_operand (t -> op1, "eax");	/* load operand */
	  code_load_word_operand (t -> dst, "ebx");	/* load target address */
	  code_line ("\tmovl\t%%eax,(%%ebx)");		/* store it away */
	};

static void code_indirect_copy (tuple t)
	{ code_load_word_operand (t -> op1, "ebx");	/* load source address */
	  code_line ("\tmovl\t(%%ebx),%%eax");		/* load operand */
	  code_store_word_operand (t -> dst, "eax");	/* store it away */
	};

static void code_copy_real (tuple t)
	{ code_instr_and_real_operand ("fldl", t -> op1);
	  code_instr_and_real_operand ("fstpl", t -> dst);
	};

static void code_copy_real_indirect (tuple t)
	{ code_instr_and_real_operand ("fldl", t -> op1);
	  code_load_word_operand (t -> dst, "ebx");
	  code_line ("\tfstpl\t(%%ebx)");
	};

static void code_copy_indirect_real (tuple t)
	{ code_load_word_operand (t -> op1, "ebx");
	  code_line ("\tfldl\t(%%ebx)");
	  code_instr_and_real_operand ("fstpl", t -> dst);
	};

/*
   Rts support actions
*/

/*
   Code test for nil
   [ is_nil, oprd, --, dest]
*/
static void code_is_nil (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tsubl\t%%ecx,%%ecx");
	  code_line ("\tdec\t%%eax");
	  code_line ("\tsetz\t%%cl");
	  code_store_word_operand (t -> dst, "ecx");
	};

/*
   Code to add an address and an offset
   [ A+I, adr_oprd, int_oprd, dest ]
*/
static void code_addr_plus_offset (tuple t)
	{ code_load_word_operand (t -> op1, "ebx");
	  code_line ("\torl\t%%ebx,%%ebx");
	  code_string ("\tjz\t");
	  code_proc_label ("rts_offset_from_0");
	  code_newline ();
	  if (t -> op2 -> tag == TAGIconst)
	     { int fixed_offset = t -> op2 -> Iconst.ival;
	       if (fixed_offset)
	          code_line ("\taddl\t$%d,%%ebx", fixed_offset);
	     }
	  else
	     { code_load_word_operand (t -> op2, "eax");
	       code_line ("\taddl\t%%eax,%%ebx");
	     };
	  code_store_word_operand (t -> dst, "ebx");
	};

/*
   Code to adjust the row index and check its bounds
   [ row_idx, oprd, upb, dest ]
*/
static void code_row_index (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tdec\t%%eax");
	  code_string ("\tjs\t");
	  code_proc_label ("rts_index_out_of_range");
	  code_newline ();
	  code_store_word_operand (t -> dst, "eax");
	  if (t -> op2 -> tag == TAGIconst)
	     code_line ("\tsubl\t$%d,%%eax", t -> op2 -> Iconst.ival);
	  else
	     { code_load_word_operand (t -> op2, "ecx");
	       code_line ("\tsubl\t%%ecx,%%eax");
	     };
	  code_string ("\tjge\t");
	  code_proc_label ("rts_index_out_of_range");
	  code_newline ();
	};

/*
   code integer ops
   [ int op, oprd1, oprd2, dst ]
*/
static void code_int_op (tuple t, string opc)
	{ code_load_word_operand (t -> op1, "eax");
	  code_load_word_operand (t -> op2, "ecx");
	  code_line ("\t%s\t%%ecx,%%eax", opc);
	  code_store_word_operand (t -> dst, "eax");
	};

static void code_real_op (tuple t, string opc)
	{ code_instr_and_real_operand ("fldl", t -> op1);
	  code_instr_and_real_operand (opc, t -> op2);
	  code_instr_and_real_operand ("fstpl", t -> dst);
	};

static void code_int_divmod (tuple t, int mod)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tcdq");
	  code_load_word_operand (t -> op2, "ecx");
	  code_line ("\tidivl\t%%ecx");
	  code_store_word_operand (t -> dst, (mod)?"edx":"eax");
	};

static void code_negate (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tneg\t%%eax");
	  code_store_word_operand (t -> dst, "eax");
	};

static void code_int_to_real (tuple t)
	{ if (t -> op1 -> tag == TAGIconst)
	     { oprd ropd = new_Rconst ((double) t -> op1 -> Iconst.ival);
	       code_instr_and_real_operand ("fldl", ropd);
	       detach_oprd (&ropd);
	     }
	  else code_instr_and_real_operand ("fildl", t -> op1);
	  code_instr_and_real_operand ("fstpl", t -> dst);
	};

static void code_negate_real (tuple t)
	{ code_instr_and_real_operand ("fldl", t -> op1);
	  code_line ("\tfchs");
	  code_instr_and_real_operand ("fstpl", t -> dst);
	};

static void code_int_cmp (tuple t, string tcc)
	{ code_load_word_operand (t -> op1, "eax");
	  code_load_word_operand (t -> op2, "edx");
	  code_line ("\tsubl\t%%ecx,%%ecx");
	  code_line ("\tsubl\t%%edx,%%eax");
	  code_line ("\tset%s\t%%cl", tcc);
	  code_store_word_operand (t -> dst, "ecx");
	};

static void code_real_cmp (tuple t, int pat, string tcc)
	{ code_instr_and_real_operand ("fldl", t -> op1);
	  code_instr_and_real_operand ("fcompl", t -> op2);
	  code_line ("\tfstsw\t%%ax");
	  code_line ("\tsubl\t%%ecx,%%ecx");
	  code_line ("\ttestw\t$0x%04x,%%ax", pat);
	  code_line ("\tset%s\t%%cl", tcc);
	  code_store_word_operand (t -> dst, "ecx");
	};

static void code_not (tuple t)
	{ code_load_word_operand (t -> op1, "eax");
	  code_line ("\tnotl\t%%eax");
	  code_line ("\tandl\t$1,%%eax");
	  code_store_word_operand (t -> dst, "eax");
	};

static void code_sline (proc p, tuple t)
	{ int lnr;
	  if (!generate_stabs) return;
	  if (t -> op1 -> tag != TAGIconst)
	     dcg_internal_error ("code_sline");
	  lnr = new_local_label ();
	  if (p -> pnr)
	     code_line ("\t.stabn\t68,0,%d,.Lline%d-P%d", t -> op1 -> Iconst.ival, lnr, p -> pnr);
	  else code_line ("\t.stabn\t68,0,%d,.Lline%d-main", t -> op1 -> Iconst.ival, lnr);
	  code_line (".Lline%d:", lnr);
	};

/*
   The coder dispatcher
*/
static void code_tuple (proc p, tuple t)
	{ switch (t -> opc)
	     { /* General */
	       case start_program:	   code_start (p); break;
	       case end_program:	   code_end (); break;

	       /* Procedure calls */
	       case prepare_call:	   code_prepare_call (t); break;
	       case load_parameter:	   code_tuple_load_parameter (t); break;
	       case load_address_parameter:code_load_address_parameter (t); break;
	       case call_procedure:
	       case call_attach_procedure:
	       case call_detach_procedure:
	       case call_guard_procedure:  code_call (t); break;

	       /* Procedure entry and return */
	       case enter_procedure:	   code_enter (p, t); break;
	       case leave_procedure:	   code_leave (p, t); break;

	       /* Jumps */
	       case jump: 		   code_jump (t); break;
	       case jump_if_nil:	   code_jump_if_nil (t); break;
	       case jump_if_null:	   code_jump_if_null (t); break;
	       case jump_if_addr_equal_addr:	   code_cond_jump (t, "e"); break;
	       case jump_if_int_equal_int:	   code_cond_jump (t, "e"); break;
	       case jump_if_int_notequal_int:	   code_cond_jump (t, "ne"); break;
	       case jump_if_int_less_than_int: 	   code_cond_jump (t, "l"); break;
	       case jump_if_int_less_equal_int:    code_cond_jump (t, "le"); break;
	       case jump_if_int_greater_than_int:  code_cond_jump (t, "g"); break;
	       case jump_if_int_greater_equal_int: code_cond_jump (t, "ge"); break;

	       /* Garbage collection */
	       case undefine:		   code_undefine (t); break;
	       case allocate:		   code_allocate (t); break;
	       case guard:		   code_guard (t); break;
	       case attach_adr:		   code_attach_object (t); break;
	       case attach_offset:	   code_attach_offset (t); break;
	       case detach_adr:		   code_detach_object (t); break;
	       case detach_offset:	   code_detach_offset (t); break;
	       case predetach_adr:	   code_predetach_object (t); break;

	       /* Copying stuff */
	       case copy_adr_of: 	   code_copy_adr_of (t); break;
	       case copy_int:
	       case copy_adr:		   code_copy (t); break;
	       case copy_int_indirect:
	       case copy_adr_indirect:	   code_copy_indirect (t); break;
	       case copy_indirect_int:
	       case copy_indirect_adr:	   code_indirect_copy (t); break;
	       case copy_real:		   code_copy_real (t); break;
	       case copy_real_indirect:	   code_copy_real_indirect (t); break;
	       case copy_indirect_real:	   code_copy_indirect_real (t); break;

	       /* Rts support ops */
	       case is_nil:		   code_is_nil (t); break;
	       case addr_plus_offset:	   code_addr_plus_offset (t); break;
	       case row_index:		   code_row_index (t); break;

	       /* Integer ops */
	       case int_plus_int:	   code_int_op (t, "addl"); break;
	       case int_minus_int:	   code_int_op (t, "subl"); break;
	       case int_times_int:	   code_int_op (t, "imull"); break;
	       case int_div_int:	   code_int_divmod (t, 0); break;
	       case int_mod_int:	   code_int_divmod (t, 1); break;
	       case int_pow_int:	   code_rts_call (t, "rts_int_pow_int"); break;
	       case negate_int:		   code_negate (t); break;
	       case int_equal_int:	   code_int_cmp (t, "e"); break;
	       case int_notequal_int:	   code_int_cmp (t, "ne"); break;
	       case int_less_than_int: 	   code_int_cmp (t, "l"); break;
	       case int_less_equal_int:    code_int_cmp (t, "le"); break;
	       case int_greater_than_int:  code_int_cmp (t, "g"); break;
	       case int_greater_equal_int: code_int_cmp (t, "ge"); break;

	       /* Real ops */
	       case real_plus_real:	   code_real_op (t, "faddl"); break;
	       case real_minus_real:	   code_real_op (t, "fsubl"); break;
	       case real_times_real:	   code_real_op (t, "fmull"); break;
	       case real_divide_real:	   code_real_op (t, "fdivl"); break;
	       case real_pow_int:	   code_rts_call (t, "rts_real_pow_int"); break;
	       case negate_real:	   code_negate_real (t); break;
	       case real_equal_real:	   code_real_cmp (t, 0x4000, "nz"); break;
	       case real_notequal_real:    code_real_cmp (t, 0x4000, "z"); break;
	       case real_less_than_real:   code_real_cmp (t, 0x0100, "nz"); break;
	       case real_less_equal_real:  code_real_cmp (t, 0x4100, "nz"); break;
	       case real_greater_than_real:  code_real_cmp (t, 0x4100, "z"); break;
	       case real_greater_equal_real: code_real_cmp (t, 0x0100, "z"); break;

	       /* Bool ops */
	       case bool_and_bool: 	   code_int_op (t, "andl"); break;
	       case bool_or_bool: 	   code_int_op (t, "orl"); break;
	       case bool_xor_bool: 	   code_int_op (t, "xorl"); break;
	       case not_bool: 		   code_not (t); break;

	       /* Text ops */
	       case text_equal_text:	   code_rts_call (t, "rts_text_equal_text"); break;
	       case text_notequal_text:	   code_rts_call (t, "rts_text_notequal_text"); break;
	       case text_less_than_text:   code_rts_call (t, "rts_text_less_than_text"); break;
	       case text_less_equal_text:  code_rts_call (t, "rts_text_less_equal_text"); break;
	       case text_greater_than_text:
		  code_rts_call (t, "rts_text_greater_than_text"); break;
	       case text_greater_equal_text:
		  code_rts_call (t, "rts_text_greater_equal_text"); break;

	       /* Conversions */
	       case int_to_real:	   code_int_to_real (t); break;

	       /* Miscellaneous */
	       case sline:		   code_sline (p, t);
	       case comment:
	       case nop:		   break;

	       default: dcg_bad_tag (t -> opc, "code_tuple"); 
	     };
	};

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
	};

static void code_procedure (proc p)
	{ block bptr;
	  code_line ("\n/* %s */", p -> cname);
	  for (bptr = p -> init_block; bptr != block_nil; bptr = bptr -> direct_succ)
	     code_segment (p, bptr);
	};

void intelx86_code_generation ()
	{ int ix;
	  dcg_hint ("      generating intelx86 machine code...");
	  code_intro ();
	  for (ix = 0; ix < im_procs -> size; ix++)
	     code_procedure (im_procs -> array[ix]);
	};
