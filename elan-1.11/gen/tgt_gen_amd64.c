/*
   File: tgt_gen_amd64.c
   Generates Amd64 machine code

   CVS ID: "$Id: tgt_gen_amd64.c,v 1.3 2011/08/13 15:58:54 marcs Exp $"
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
#include "tgt_gen_amd64.h"

/*
   AMD64 properties
   Basically:
      sizeof (VOID) = 0;
      sizeof (INT) = 4;
      sizeof (REAL) = sizeof (void *) = 8;
   Different calling convention
      %rbp, %rbx should be callee saved
      %rdi, %rsi, %rdx, %rcx, %r8, %r9 are used for integer and pointer parameters
      %xmm0 upto %xmm7 are used for double (REAL) parameters
      All other parameters are passed by the stack (8 byte aligned)
      The stack pointer %rsp must always be 8 byte aligned
*/
int amd64_size_from_type (type t)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType)
		       dcg_internal_error ("amd64_size_from_type");
		    return (amd64_size_from_type (id -> Type.concr));
		  };
	       case TAGVoid: return (0);
	       case TAGInt:
	       case TAGBool: return (4);
	       case TAGAddr:
	       case TAGText:
	       case TAGFile:
	       case TAGNiltype:
	       case TAGRow:
	       case TAGStruct:
	       case TAGProc_type:
	       case TAGReal: return (8);
	       default: dcg_bad_tag (t -> tag, "amd64_size_from_type");
	     };
	  return (0);
	};

int amd64_regsize_from_arg (pdecl pd)
	{ return (1);			/* always one */
	};

/*
   Amd64 coding routines
*/

/*
   We allocate from -8(%fp) (reserved for %rbx) downward. 
   Note that we must also allocate the save areas for the parameters.

   Since we only have two alignments, we first allocate double word objects
   and then all single word objects. Finally ensure that the frame and stack
   pointer is double word aligned
*/
static int allocate_locals (int_list locals, int offset)
	{ /* Loop over locals, fill in size, allocate doubles */
	  int ix;
	  for (ix = 0; ix < locals -> size; ix++)
	     { int vnr = locals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       int size = amd64_size_from_type (opd -> vtype);
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

	  /* make sure %rsp is 16 byte aligned */
	  if (offset & 4) offset -= 4;
	  if (offset & 8) offset -= 8;
	  return (offset);
	};

/*
   Locate proc arguments and code the saving of the arguments
*/
static int locate_args (decl_list args)
	{ int offset = -8;
	  int extra_offset = 8;
	  int nr_of_int_class = 0;
	  int nr_of_real_class = 0;
	  int ix;
	  for (ix = 0; ix < args -> size; ix++)
	     { decl arg = args -> array[ix];
	       int vnr = arg -> Formal.vnr;
	       var opd = im_vars -> array[vnr];
	       type vtype = opd -> vtype;
	       int *clptr = &nr_of_int_class;
	       int clmax = 6;
	       if ((vtype -> tag == TAGReal) && (!opd -> vref))
		 { clptr = &nr_of_real_class;
		   clmax = 8;
		 };
	       *clptr += 1;
	       if (*clptr > clmax)
	       	 { opd -> offset = extra_offset;
		   extra_offset += 8;
		 }
	       else
	         { offset -= 8;
	           opd -> offset = offset;
	         };
	     };
	  return (offset);
	};

/*
   Allocate and code global vars
*/
static void code_global_variables (int_list globals)
	{ int ix;
	  for (ix = 0; ix < globals -> size; ix++)
	     { int vnr = globals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       /* type vtype = opd -> vtype;
	          int vstabs = type_table -> array[vtype -> tnr] -> stabs_nr;
	       */
	       int size = opd -> vsize;
	       
	       if (!opd -> vglob) continue;
	       code_line ("\t.local\tG%d\t/* %s */", vnr, opd -> vname);
	       code_line ("\t.comm\tG%d,%d,%d", vnr, size, size);
	       /* if (generate_stabs)
		   code_line ("\t.stabs\t\"%s:S%d,\",40,0,0,G%d", opd -> vname, vstabs, vnr);
	       */
	     };
	};

/*
   Coding of operands
   Generally the registers are used as followed:

   %rsp: stack pointer
   %rbp: frame pointer
   %rax: general register
   %rbx: general address register (always free for indirection)
*/

/*
   Code load operand is used to transfer int/addr operands to a register
   The string 'dd' is used to indicate the kind of operand 
   "l" for int, "q" for 64 bit int or address, "sd" for doubles
*/
static void code_load_operand (oprd opd, string dd, string reg)
	{ switch (opd -> tag)
	     { case TAGIconst:
		  code_line ("\tmovl\t$%d,%%%s", opd -> Iconst.ival, reg);
		  break;
	       case TAGTconst:
		  { int nr = new_const_label ();
		    code_line ("\t.section\t.rodata");
		    code_line ("\t.align\t4");
		    code_line ("\t.short\t-1");
		    code_line ("\t.short\t-1");
		    code_string ("CC%d:\t.asciz\t", nr);
		    code_quoted_string (opd -> Tconst.tval);
		    code_line ("\n\t.text");
		    code_line ("\tleaq\tCC%d,%%%s", nr, reg);
		  }; break;
	       case TAGVar:	/* Check size VAR */
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    if (obj -> vglob)
		       code_line ("\tmov%s\tG%d,%%%s", dd, opd -> Var.vnr, reg);
		    else if (obj -> vref)
		       { code_line ("\tmovq\t%d(%%rbp),%%rbx", obj -> offset);
			 code_line ("\tmov%s\t(%%rbx),%%%s", dd, reg);
		       }
		    else code_line ("\tmov%s\t%d(%%rbp),%%%s", dd, obj -> offset, reg);
		  }; break;
	       case TAGRts_nil:
		  code_line ("\tmovq\t$1,%%%s", reg);
		  break;
	       case TAGRconst:
		  { int nr = new_const_label ();
		    code_line ("\t.section\t.rodata");
		    code_line ("\t.align\t8");
		    code_line ("CC%d:\t.double\t0e%e", nr, opd -> Rconst.rval);
		    code_line ("\t.text");
		    code_line ("\tmov%s\tCC%d,%%%s", dd, nr, reg);
		  }; break;
	       case TAGReflexive:
	       case TAGLab:
	       case TAGRts:
	       case TAGNop: dcg_internal_error ("code_load_operand");
	       default: dcg_bad_tag (opd -> tag, "code_load_operand");
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
		  { code_string ("\tleaq\t");
		    code_called_proc (opd);
		    code_line (",%%%s", reg);
		  }; break;
	       case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    if (obj -> vglob)
		       code_line ("\tleaq\tG%d,%%%s", opd -> Var.vnr, reg);
		    else if (obj -> vref)
		       code_line ("\tmovq\t%d(%%rbp),%%%s", obj -> offset, reg);
		    else code_line ("\tleaq\t%d(%%rbp),%%%s", obj -> offset, reg);
		  }; break;
	       default: dcg_bad_tag (opd -> tag, "code_load_address");
	     };
	};

/* may use %rbx on storing VAR/Reflexive parameters */
static void code_store_operand (oprd opd, string dd, string reg)
	{ var obj;
	  switch (opd -> tag)
	    { case TAGVar:
		{ obj = im_vars -> array[opd -> Var.vnr];
		  if (obj -> vglob)
		    code_line ("\tmov%s\t%%%s,G%d", dd, reg, opd -> Var.vnr);
		  else if (obj -> vref)
		    { code_line ("\tmovq\t%d(%%rbp),%%rbx", obj -> offset);
		      code_line ("\tmov%s\t%%%s,(%%rbx)", dd, reg);
		    }
		  else code_line ("\tmov%s\t%%%s,%d(%%rbp)", dd, reg, obj -> offset);
		}; break;
	      case TAGReflexive:
		code_line ("\tmovq\t%%%s,(%%rbx)", reg);	/* overwrite @pointer */
	      case TAGNop: break;
	      default: dcg_bad_tag (opd -> tag, "code_store_operand");
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
				      8 * fld -> offset, 8 * amd64_size_from_type (ftype));
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
   Note that it is not needed to allocate local variables
*/
static void code_start (proc p)
	{ int offset = allocate_locals (p -> locals, -8);
	  code_global_variables (p -> locals);

	  /* Code program entry and call to initialize rts */
	  code_line ("\t.text");
	  code_line ("\t.align\t16");
	  code_string ("\t.globl\t");
	  code_proc_label ("main");
	  code_newline ();
	  code_line ("\t.type\tmain,@function");
	  if (generate_stabs)
	     { code_line ("\t.stabs\t\"main:F1\",36,0,0,main");
	       /*
	       code_line ("\t.stabs\t\"argc:p1\",160,0,0,8");
	       code_line ("\t.stabs\t\"argv:p6\",160,0,0,12");
	       */
	     };
	  code_proc_label ("main");
	  code_line (":\tpushq\t%%rbp");
	  code_line ("\tmovq\t%%rsp,%%rbp");
	  code_line ("\tsubq\t$%d,%%rsp", -offset);
	  code_line ("\tmovq\t%%rbx,-8(%%rbp)");	/* %rbx is callee saved at -8 (%rbp) */

	  /* Call runtime system to initialize */	/* argc is in %rdi, argv in %rsi */
	  code_string ("\tcall\t");			/* code call to init rts */
	  code_proc_label ("rts_init");
	  code_newline ();
	};

/*
   Code program end
*/
static void code_end ()
	{ int lnr = new_local_label ();
	  code_string ("\tcall\t");			/* code call to finish rts */
	  code_proc_label ("rts_finish");
	  code_newline ();
	  code_line ("\tmovq\t-8(%%rbp),%%rbx");	/* Reload old %rbx */
	  code_line ("\tmovq\t%%rbp,%%rsp");
	  code_line ("\tpopq\t%%rbp");			/* Reload old fp */
	  code_line ("\txorq\t%%rax,%%rax");
	  code_line ("\tret");
	  code_line (".Lfe%d:\t.size\tmain,.Lfe%d-main", lnr, lnr);
	};

/*
   Code the saving of args and their possible stabs entry
   First code the arrays that code the order of parameters
*/
static string sd_regs[8] = { "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7" };
static string q_regs[6]  = { "rdi", "rsi", "rdx", "rcx", "r8", "r9" };
static string l_regs[6]  = { "edi", "esi", "edx", "ecx", "r8", "r9" };

/*
static void code_parameter_stabs_entry (decl arg, int pnr)
	{ int vnr = arg -> Formal.vnr;
	  var opd = im_vars -> array[vnr];
	  type ftyp = arg -> typ;
	  type_info finfo = type_table -> array[ftyp -> tnr];
	  string facc_info = (arg -> Formal.acc == acc_var)?"v":"p";
	  code_line ("\t.stabs\t\"%s:%s%d\",160,0,0,%d",
		     arg -> name, facc_info, finfo -> stabs_nr, opd -> offset);
	};
*/

static void code_save_args (decl_list args)
	{ int nr_of_int_opds = 0;
	  int nr_of_real_opds = 0;
	  int ix;
	  for (ix = 0; ix < args -> size; ix++)
	    { decl arg = args -> array[ix];
	      int vnr = arg -> Formal.vnr;
	      var opd = im_vars -> array[vnr];
	      type vtype = opd -> vtype;
	      int size = amd64_size_from_type (vtype);
	      if (opd -> vref) size = 8;
	      if ((vtype -> tag == TAGReal) && !(opd -> vref))
	        { if (nr_of_real_opds < 8)
		    code_line ("\tmovsd\t%%%s,%d(%%rbp)", sd_regs[nr_of_real_opds], opd -> offset);
		  nr_of_real_opds++;
		}
	      else if (nr_of_int_opds < 6)
	        { if (size & 4)
		    code_line ("\tmovl\t%%%s,%d(%%rbp)", l_regs[nr_of_int_opds], opd -> offset);
		  else code_line ("\tmovq\t%%%s,%d(%%rbp)", q_regs[nr_of_int_opds], opd -> offset);
		  nr_of_int_opds++;
		};
	    };
	};

/*
   Code procedure entry
*/
static void code_enter (proc p, tuple t)
	{ int offset = locate_args (p -> args);
	  offset = allocate_locals (p -> locals, offset);
	  code_line ("\t.align\t16");
	  code_line ("\t.type\tP%d,@function", p -> pnr);
	  /* if (generate_stabs)
	     { int ix;
	       type_info rinfo = type_table -> array[rtype -> tnr];
	       code_line ("\t.stabs\t\"%s:f%d\",36,0,0,P%d",
			  p -> pname, rinfo -> stabs_nr, p -> pnr);
	       for (ix = 0; ix < p -> args -> size; ix++)
		  code_parameter_stabs_entry (p -> args -> array[ix], ix + 1);
	     };
	  */
	  code_line ("P%d:\tpushq\t%%rbp", p -> pnr);
	  code_line ("\tmovq\t%%rsp,%%rbp");
	  code_line ("\tsubq\t$%d,%%rsp", -offset);
	  code_line ("\tmovq\t%%rbx,-8(%%rbp)");	/* %rbx is callee saved at -8 (%rbp) */
	  code_save_args (p -> args);
	};

/*
   Code procedure exit
*/
static void code_leave (proc p, tuple t)
	{ int lnr = new_local_label ();
	  type rtype = p -> rtyp;
	  int rsize = amd64_size_from_type (rtype);

	  /* load return value */
	  if (is_a_floating_type (rtype))
	    code_load_operand (t -> op2, "sd", "xmm0");
	  else if (t -> op2 -> tag != TAGNop)
	    { if (rsize & 8) code_load_operand (t -> op2, "q", "rax");
	      else code_load_operand (t -> op2, "l", "eax");
	    };

	  /* discard frame and return */
	  code_line ("\tmovq\t-8(%%rbp),%%rbx");	/* %rbx is callee saved at -8 (%rbp) */
	  code_line ("\tmovq\t%%rbp,%%rsp");
	  code_line ("\tpopq\t%%rbp");
	  code_line ("\tret");
	  code_line (".Lfe%d:\t.size\tP%d,.Lfe%d-P%d", lnr, p -> pnr, lnr, p -> pnr);
	};

/*
   Coding of procedure calls
   Note that the finish call removes the parameters from the stack
*/

/*
   Prepare call:
   Count the number of integer/pointer parameters and real parameters
   The first 6 in the integer/pointer class and the first 8 in the real class
   are passed by register; the remainder should be passed by stack.
   [ pcall, nr_args, Types:pdl, -- ]
*/
static void calculate_parameter_classes (proc p)
	{ pdecl_list pds = p -> tmp_pdecls;
	  int ix;
	  p -> tmp1 = 0;	/* integer or pointer class */
	  p -> tmp2 = 0;	/* real class */
	  for (ix = 0; ix < pds -> size; ix++)
	    { pdecl pd = pds -> array[ix];
	      if ((pd -> ptype -> tag == TAGReal) && (pd -> pacc == acc_const))
		p -> tmp2 += 1;
	      else p -> tmp1 += 1;
	    };
	};

static int calculate_extra_stack_space (proc p)
	{ int extra = 0;
	  if (p -> tmp1 > 6) extra += (p -> tmp1 - 6) * 8;
	  if (p -> tmp2 > 8) extra += (p -> tmp2 - 8) * 8;
	  return (extra);
	};

static void code_prepare_call (proc p, tuple t)
	{ int extra;
	  if (t -> op2 -> tag != TAGTypes)
	    dcg_bad_tag (t -> op2 -> tag, "code_prepare_call");
	  p -> tmp_pdecls = t -> op2 -> Types.atypes;
	  calculate_parameter_classes (p);
	  extra = calculate_extra_stack_space (p);
	  if (extra) code_line ("\tsubq\t$%d,%%rsp", extra);
	};

/*
   Coding of subroutine parameter loading and result storing
   Note that if the parameter is a Nop, nothing gets coded

   Code load word parameter
   [ lpar, parm, argno, -- ] 
*/
static void code_load_parameter (proc p, tuple t)
	{ oprd opd = t -> op1;
	  type opd_type = type_from_operand (opd);
	  int size = amd64_size_from_type (opd_type);
	  if (opd -> tag == TAGNop)
	    dcg_bad_tag (opd -> tag, "code_load_parameter");
	  else if (is_a_floating_type (opd_type))
	    { /* Reduce the number of remaining parameters */
	      p -> tmp2--;
	      if (p -> tmp2 >= 8)
		{ /* Push the operand */
		  code_line ("\tsubq\t$8,%%rsp");
		  code_load_operand (opd, "sd", "xmm0");
		  code_line ("\tmovsd\t%%xmm0,(%%rsp)");
		}
	      else code_load_operand (opd, "sd", sd_regs[p -> tmp2]);
	    }
	  else /* not a floating point operand */
	    { p -> tmp1--;
	      if (p -> tmp1 >= 6)
		{ /* Push the operand */
		  code_line ("\tsubq\t$8,%%rsp");
		  if (size & 4)
		    { code_load_operand (opd, "l", "eax");
		      code_line ("\tmovl\t%%eax,(%%rsp)");
		    }
		  else
		    { code_load_operand (opd, "q", "rax");
		      code_line ("\tmovq\t%%rax,(%%rsp)");
		    }
		}
	      else if (size & 4)
		/* Code in argument regs */
		code_load_operand (opd, "l", l_regs[p -> tmp1]);
	      else code_load_operand (opd, "q", q_regs[p -> tmp1]);
	    };
	};

static void code_store_result (oprd dest)
	{ type desttype = type_from_operand (dest);
	  int size = size_from_type (desttype);
	  if (is_a_floating_type (desttype)) code_store_operand (dest, "sd", "xmm0");
	  else if (size & 4) code_store_operand (dest, "l", "eax");
	  else code_store_operand (dest, "q", "rax");
	};

/*
   Code load address of parameter
   [ lpar&, parm, argno, --]
*/
static void code_load_address_parameter (proc p, tuple t)
	{ p -> tmp1--;
	  if (p -> tmp1 >= 6)
	    { /* Push the operand */
	      code_line ("\tsubq\t$8,%%rsp");
	      code_load_address (t -> op1, "rbx");
	      code_line ("\tmovq\t%%rbx,(%%rsp)");
	    }
	  else code_load_address (t -> op1, q_regs[p -> tmp1]);
	};

/*
   Code call to runtime system
*/
static void code_rts_call (tuple t, string rts_name)
	{ type op1_type = type_from_operand (t -> op1);
	  type op2_type = type_from_operand (t -> op2);
	  int op1_size = amd64_size_from_type (op1_type);
	  int op2_size = amd64_size_from_type (op2_type);
	  int nr_int_opds = 0;
	  int nr_real_opds = 0;

	  /* Check what to do with operand 1 */
	  if (t -> op1 -> tag == TAGNop) ;
	  else if (is_a_floating_type (op1_type))
	    { code_load_operand (t -> op1, "sd", "xmm0");
	      nr_real_opds++;
	    }
	  else
	    { if (op1_size & 4) code_load_operand (t -> op1, "l", "edi");
	      else code_load_operand (t -> op1, "q", "rdi");
	      nr_int_opds++;
	    };

	  /* Check what to do with operand 2 */
	  if (t -> op2 -> tag == TAGNop) ;
	  else if (is_a_floating_type (op2_type))
	    code_load_operand (t -> op2, "sd", sd_regs [nr_real_opds]);
	  else if (op2_size & 4)
	    code_load_operand (t -> op2, "l", l_regs [nr_int_opds]);
	  else code_load_operand (t -> op2, "q", q_regs [nr_int_opds]);

	  /* Code the actual call, store the result and discard parameters */
	  code_string ("\tcall\t");
	  code_proc_label (rts_name);
	  code_newline ();
	  code_store_result (t -> dst);
	};

/*
   Code call, store result and discard arguments
   [ call, proc, nr_args, dst ]
*/
static void code_actual_call (oprd opd)
	{ if (opd -> tag == TAGVar)
	    { code_load_operand (opd, "q", "rbx");
	      code_line ("\tcall\t*%%rbx");
	    }
	  else
	    { code_string ("\tcall\t");
	      code_called_proc (opd);
	      code_newline ();
	    };
	};

static void code_call (proc p, tuple t)
	{ int extra;
	  if (t -> dst -> tag == TAGReflexive)
	    code_line ("\tmovq\t%%rdi,%%rbx");
	  code_actual_call (t -> op1);
	  code_store_result (t -> dst);
	  calculate_parameter_classes (p);
	  extra = calculate_extra_stack_space (p);
	  if (extra) code_line ("\taddq\t$%d,%%rsp", extra);
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
	{ code_load_operand (t -> op1, "q", "rax");
	  code_line ("\tdecq\t%%rax");
	  code_string ("\tjz\t");
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Code jump if null
   [ jmp_if_null, oprd, --, dst ]
*/
static void code_jump_if_null (tuple t)
	{ code_load_operand (t -> op1, "q", "rax");
	  code_line ("\tandq\t%%rax,%%rax");
	  code_string ("\tjz\t");
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Code jump if condition
   [ jump_if_int_less_than_int, oprd1, oprd2, dst ]
*/
static void code_cond_jump (tuple t, string opc)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_load_operand (t -> op2, "l", "ecx");
	  code_line ("\tsubl\t%%ecx,%%eax");
	  code_string ("\tj%s\t", opc);
	  code_jump_target (t -> dst);
	  code_newline ();
	};

/*
   Coding of garbage collection calls
*/
static void code_undefine (tuple t)
	{ code_line ("\txorq\t%%rax,%%rax");
	  code_store_operand (t -> dst, "q", "rax");
	};

/*
   Allocate space for row/struct
   [ allocate, space in bytes, --, dest ]
   Check alignment / calling convention
*/
static void code_allocate (tuple t)
	{ code_load_operand (t -> op1, "l", "edi");
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_malloc");
	  code_newline ();
	  code_store_operand (t -> dst, "q", "rax");	/* Store returned address */
	};

/*
   Set guard on var object and possibly create space for it
   [ guard, oprd, space_in_words, dest ]
*/
static void code_guard (tuple t)
	{ code_load_operand (t -> op2, "l", "esi");	/* load nr bytes */
	  code_load_operand (t -> op1, "q", "rdi");	/* load address */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_guard");		/* to guard the value */
	  code_newline ();
	  code_store_operand (t -> dst, "q", "rax");	/* store result away */
	};

/*
   Attach object
   [ att&, oprd, --, dest/--/reflexive ]
*/
static void code_attach_object (tuple t)
	{ code_load_address (t -> op1, "rdi");		/* load argument */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_attach");		/* to attach value */
	  code_newline ();
	  code_store_operand (t -> dst, "q", "rax");	/* special code to handle the return */
	};

/*
   Attach object pointed to by oprd + offset
   [ att_off, addr, offset, dest/--/reflexive ]
*/
static void code_attach_offset (tuple t)
	{ int fixed_offset = 0;				/* if fixed offset, default is 0 */
	  code_load_operand (t -> op1, "q", "rdi");	/* load address */
	  switch (t -> op2 -> tag)
	    { case TAGIconst: fixed_offset = t -> op2 -> Iconst.ival;
	      case TAGNop:
	  	if (fixed_offset) code_line ("\taddq\t$%d,%%rdi", fixed_offset);
		break;
	      default:
	        code_load_operand (t -> op2, "l", "eax");
		/* Check: Zero.extend eax -> rax */
	        code_line ("\taddq\t%%rax,%%rdi");
	    };
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_attach");		/* to attach value */
	  code_newline ();
	  code_store_operand (t -> dst, "q", "rax");	/* special code to handle the return */
	};

/*
   Detach variable. Note that rts_detach is called by reference
   [ det&_adr, oprd, --, -- ]
*/
static void code_detach_object (tuple t)
	{ code_load_address (t -> op1, "rdi");		/* load addres */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_detach");		/* to detach value */
	  code_newline ();
	};

/*
   Detach object pointed to by oprd + offset
   [ det_off, addr, offset, -- ]
*/
static void code_detach_offset (tuple t)
	{ int fixed_offset = 0;				/* if fixed offset, default is 0 */
	  code_load_operand (t -> op1, "q", "rdi");	/* load address */
	  switch (t -> op2 -> tag)
	    { case TAGIconst: fixed_offset = t -> op2 -> Iconst.ival;
	      case TAGNop:
	  	if (fixed_offset) code_line ("\taddq\t$%d,%%rdi", fixed_offset);
		break;
	      default:
	        code_load_operand (t -> op2, "l", "eax");
		/* Check sign extend eax -> rax */
	        code_line ("\taddq\t%%rax,%%rdi");
	    };
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_detach");		/* to detach value */
	  code_newline ();
	};

static void code_predetach_object (tuple t)
	{ code_load_address (t -> op1, "rdi");		/* load address */
	  code_string ("\tcall\t");			/* call the runtime system */
	  code_proc_label ("rts_predetach");		/* to predetach value */
	  code_newline ();
	  code_store_operand (t -> dst, "q", "rax");	/* store result away */
	};

/*
   Coding of copy actions
*/

/*
   Code copy address of
   [ :=&, oprd, --, dst ]
*/
static void code_copy_adr_of (tuple t)
	{ code_load_address (t -> op1, "rdx");		/* load address */
	  code_store_operand (t -> dst, "q", "rdx");	/* store it away */
	};

/*
   Code copy/distinguish int/addr
   [ :=I, oprd, --, dst ], [ :=A, oprd, --, dst ]
*/
static void code_copy_int (tuple t)
	{ code_load_operand (t -> op1, "l", "eax");	/* load operand */
	  code_store_operand (t -> dst, "l", "eax");	/* store it away */
	};

static void code_copy_int_indirect (tuple t)
	{ code_load_operand (t -> op1, "l", "eax");	/* load operand */
	  code_load_operand (t -> dst, "q", "rbx");	/* load target address */
	  code_line ("\tmovl\t%%eax,(%%rbx)");		/* store it away */
	};

static void code_copy_indirect_int (tuple t)
	{ code_load_operand (t -> op1, "q", "rbx");	/* load source address */
	  code_line ("\tmovl\t(%%rbx),%%eax");		/* load operand */
	  code_store_operand (t -> dst, "l", "eax");	/* store it away */
	};

static void code_copy_adr (tuple t)
	{ code_load_operand (t -> op1, "q", "rax");	/* load operand */
	  code_store_operand (t -> dst, "q", "rax");	/* store it away */
	};

static void code_copy_adr_indirect (tuple t)
	{ code_load_operand (t -> op1, "q", "rax");	/* load operand */
	  code_load_operand (t -> dst, "q", "rbx");	/* load target address */
	  code_line ("\tmovq\t%%rax,(%%rbx)");		/* store it away */
	};

static void code_copy_indirect_adr (tuple t)
	{ code_load_operand (t -> op1, "q", "rbx");	/* load source address */
	  code_line ("\tmovq\t(%%rbx),%%rax");		/* load operand */
	  code_store_operand (t -> dst, "q", "rax");	/* store it away */
	};

static void code_copy_real (tuple t)
	{ code_load_operand (t -> op1, "sd", "xmm0");
	  code_store_operand (t -> dst, "sd", "xmm0");
	};

static void code_copy_real_indirect (tuple t)
	{ code_load_operand (t -> op1, "sd", "xmm0");
	  code_load_operand (t -> dst, "q", "rbx");
	  code_line ("\tmovsd\t%%xmm0,(%%rbx)");
	};

static void code_copy_indirect_real (tuple t)
	{ code_load_operand (t -> op1, "q", "rbx");
	  code_line ("\tmovsd\t(%%rbx),%%xmm0");
	  code_store_operand (t -> dst, "sd", "xmm0");
	};

/*
   Rts support actions
*/

/*
   Code test for nil
   [ is_nil, oprd, --, dest]
*/
static void code_is_nil (tuple t)
	{ code_load_operand (t -> op1, "q", "rax");
	  code_line ("\tsubl\t%%ecx,%%ecx");
	  code_line ("\tdecq\t%%rax");
	  code_line ("\tsetz\t%%cl");
	  code_store_operand (t -> dst, "l", "ecx");
	};

/*
   Code to add an address and an offset
   [ A+I, adr_oprd, int_oprd, dest ]
*/
static void code_addr_plus_offset (tuple t)
	{ code_load_operand (t -> op1, "q", "rbx");
	  code_line ("\torq\t%%rbx,%%rbx");
	  code_string ("\tjz\t");
	  code_proc_label ("rts_offset_from_0");
	  code_newline ();
	  if (t -> op2 -> tag == TAGIconst)
	    { int fixed_offset = t -> op2 -> Iconst.ival;
	      if (fixed_offset)
	        code_line ("\taddq\t$%d,%%rbx", fixed_offset);
	    }
	  else
	    { code_load_operand (t -> op2, "l", "eax");
	      /* Add sign extension */
	      code_line ("\taddq\t%%rax,%%rbx");
	    };
	  code_store_operand (t -> dst, "q", "rbx");
	};

/*
   Code to adjust the row index and check its bounds
   [ row_idx, oprd, upb, dest ]
*/
static void code_row_index (tuple t)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_line ("\tdecl\t%%eax");
	  code_string ("\tjs\t");
	  code_proc_label ("rts_index_out_of_range");
	  code_newline ();
	  code_store_operand (t -> dst, "l", "eax");
	  if (t -> op2 -> tag == TAGIconst)
	     code_line ("\tsubl\t$%d,%%eax", t -> op2 -> Iconst.ival);
	  else
	     { code_load_operand (t -> op2, "l", "ecx");
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
	{ code_load_operand (t -> op1, "l", "eax");
	  code_load_operand (t -> op2, "l", "ecx");
	  code_line ("\t%s\t%%ecx,%%eax", opc);
	  code_store_operand (t -> dst, "l", "eax");
	};

static void code_real_op (tuple t, string opc)
	{ code_load_operand (t -> op1, "sd", "xmm0");
	  code_load_operand (t -> op2, "sd", "xmm1");
	  code_line ("\t%ssd\t%%xmm1,%%xmm0", opc);
	  code_store_operand (t -> dst, "sd", "xmm0");
	};

static void code_int_divmod (tuple t, int mod)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_line ("\tcdq");	/* may be cltd */
	  code_load_operand (t -> op2, "l", "ecx");
	  code_line ("\tidivl\t%%ecx");
	  code_store_operand (t -> dst, "l", (mod)?"edx":"eax");
	};

static void code_negate (tuple t)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_line ("\tnegl\t%%eax");
	  code_store_operand (t -> dst, "l", "eax");
	};

static void code_int_to_real (tuple t)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_line ("\tcvtsi2sd\t%%eax,%%xmm0");
	  code_store_operand (t -> dst, "sd", "xmm0");
	};

static void code_negate_real (tuple t)
	{ code_line ("\tmovl\t$0,%%eax");
	  code_line ("\tcvtsi2sd\t%%eax,%%xmm0");
	  code_load_operand (t -> op1, "sd", "xmm1");
	  code_line ("\tsubsd\t%%xmm1,%%xmm0");
	  code_store_operand (t -> dst, "sd", "xmm0");
	};

static void code_int_cmp (tuple t, string tcc)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_load_operand (t -> op2, "l", "edx");
	  code_line ("\tsubl\t%%ecx,%%ecx");
	  code_line ("\tsubl\t%%edx,%%eax");
	  code_line ("\tset%s\t%%cl", tcc);
	  code_store_operand (t -> dst, "l", "ecx");
	};

static void code_real_cmp (tuple t, string tcc)
	{ code_load_operand (t -> op1, "sd", "xmm0");
	  code_load_operand (t -> op2, "sd", "xmm1");
	  code_line ("\tsubl\t%%ecx,%%ecx");
	  code_line ("\tcomisd\t%%xmm1,%%xmm0");
	  code_line ("\tset%s\t%%cl", tcc);
	  code_store_operand (t -> dst, "l", "ecx");
	};

static void code_not (tuple t)
	{ code_load_operand (t -> op1, "l", "eax");
	  code_line ("\tnotl\t%%eax");
	  code_line ("\tandl\t$1,%%eax");
	  code_store_operand (t -> dst, "l", "eax");
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
	       case prepare_call:	   code_prepare_call (p, t); break;
	       case load_parameter:	   code_load_parameter (p, t); break;
	       case load_address_parameter:code_load_address_parameter (p, t); break;
	       case call_procedure:
	       case call_attach_procedure:
	       case call_detach_procedure:
	       case call_guard_procedure:  code_call (p, t); break;

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
	       case copy_int:		   code_copy_int (t); break;
	       case copy_adr:		   code_copy_adr (t); break;
	       case copy_int_indirect:	   code_copy_int_indirect (t); break;
	       case copy_adr_indirect:	   code_copy_adr_indirect (t); break;
	       case copy_indirect_int:	   code_copy_indirect_int (t); break;
	       case copy_indirect_adr:	   code_copy_indirect_adr (t); break;
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
	       case real_plus_real:	   code_real_op (t, "add"); break;
	       case real_minus_real:	   code_real_op (t, "sub"); break;
	       case real_times_real:	   code_real_op (t, "mul"); break;
	       case real_divide_real:	   code_real_op (t, "div"); break;
	       case real_pow_int:	   code_rts_call (t, "rts_real_pow_int"); break;
	       case negate_real:	   code_negate_real (t); break;
	       case real_equal_real:	   code_real_cmp (t, "e"); break;
	       case real_notequal_real:    code_real_cmp (t, "ne"); break;
	       case real_less_than_real:   code_real_cmp (t, "l"); break;
	       case real_less_equal_real:  code_real_cmp (t, "le"); break;
	       case real_greater_than_real:  code_real_cmp (t, "g"); break;
	       case real_greater_equal_real: code_real_cmp (t, "ge"); break;

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

void amd64_code_generation ()
	{ int ix;
	  dcg_hint ("      generating amd64 machine code...");
	  code_intro ();
	  for (ix = 0; ix < im_procs -> size; ix++)
	     code_procedure (im_procs -> array[ix]);
	};
