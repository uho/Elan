/*
   File: tgt_gen_alpha.c
   Generates Alpha machine code

   CVS ID: "$Id: tgt_gen_alpha.c,v 1.7 2011/08/13 15:58:54 marcs Exp $"
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
#include <dcg_arith.h>
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
#include "tgt_gen_alpha.h"

/*
   Alpha characteristics
   Basically:
      sizeof (VOID) = 0;
      sizeof (INT) = 4;
      sizeof (<ADDR>) = sizeof (REAL) = 8;
*/
int alpha_size_from_type (type t)
{ switch (t -> tag)
    { case TAGTname:
	{ decl id = t -> Tname.id;
	  if (id -> tag != TAGType)
	    dcg_internal_error ("alpha_size_from_type");
	  return (alpha_size_from_type (id -> Type.concr));
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
      default: dcg_bad_tag (t -> tag, "alpha_size_from_type");
    };
  return (0);
}

int alpha_regsize_from_arg (pdecl pd)
{ return (1);				/* always one */
}

/*
   Alpha coding routines
*/

/*
   Loop over segment to check for the longest call outgoing allocation
   since we have to allocate space in the local stack frame to allow
   calls with more than 6 arguments. If we detect a reflexive destination
   we ensure that one extra argument is allocated.

   We should do this pass before coding any procedure/program
*/
static int longest_call_alloc_in_segment (tuple first)
{ tuple ptr = first;
  int max = 6;			/* 6 arguments are passed by register */
  do
    { if (ptr -> opc == prepare_call)
	{ int lmax;
	  if (ptr -> op1 -> tag != TAGIconst)
	    dcg_bad_tag (ptr -> op1 -> tag, "longest_call_alloc");
	  lmax = ptr -> op1 -> Iconst.ival;
	  if (lmax > max) max = lmax;
	}
      else if ((ptr -> dst -> tag == TAGReflexive) && (max < 7))
	max = 7;
      ptr = ptr -> next;
    }
  while (ptr != first);
  if (max <= 6) return (0);	/* if max <= 6 no extra stack space needed */
  if (max & 1) max++;		/* allocate double octaword alignment */
  return ((max - 6) * 2);	/* every (even int) argument takes a double */
}

static int longest_call_alloc (proc p)
{ block bptr;
  int max = 0;
  for (bptr = p -> init_block; bptr != block_nil; bptr = bptr -> direct_succ)
    { int lmax = longest_call_alloc_in_segment (bptr -> anchor);
      if (lmax > max) max = lmax;
    };
  return (max);
}

/*
   Allocate space for arguments and locals. To save space we allocate
   space both for the incoming register arguments as well as for the
   locals. 
*/
static int allocate_inargs_and_locals (decl_list args, int_list locals, int extra_alloc)
{ int local_alloc = extra_alloc + 4;		/* save RA and FP: 2 for each */
  int local_offset = local_alloc * 4;
  int ix;

  /* Allocate for double register arguments */
  for (ix = 0; (ix < args -> size) && (ix < 6); ix++)
    { decl arg = args -> array[ix];
      int vnr = arg -> Formal.vnr;
      var opd = im_vars -> array[vnr];
      if (opd -> vref || (alpha_size_from_type (opd -> vtype) == 8))
	{ opd -> vsize = 8;
	  opd -> offset = local_offset;
	  local_offset += 8;
	  local_alloc += 2;
	}
      else opd -> vsize = 4;
    };

  /* Allocate for double locals */
  for (ix = 0; ix < locals -> size; ix++)
    { int vnr = locals -> array[ix];
      var opd = im_vars -> array[vnr];
      int size = alpha_size_from_type (opd -> vtype);
      opd -> vsize = size;
      if (size & 4) continue;
      if (opd -> vglob) continue;
      opd -> offset = local_offset;
      local_offset += 8;
      local_alloc += 2;
    };

  /* Allocate for int register arguments */
  for (ix = 0; (ix < args -> size) && (ix < 6); ix++)
    { decl arg = args -> array[ix];
      int vnr = arg -> Formal.vnr;
      var opd = im_vars -> array[vnr];
      if (opd -> vsize == 4)
	{ opd -> offset = local_offset;
	  local_offset += 4;
	  local_alloc += 1;
	};
    };

  /* Allocate for int locals */
  for (ix = 0; ix < locals -> size; ix++)
    { int vnr = locals -> array[ix];
      var opd = im_vars -> array[vnr];
      if (opd -> vglob) continue;
      if (opd -> vsize == 4)
	{ opd -> offset = local_offset;
	  local_offset += 4;
	  local_alloc += 1;
	};
    };
  if (local_alloc & 1)
    { local_offset += 4;
      local_alloc++;
    };

  /*
     Now that we know the layout of the local frame, we still have
     to assign offsets to the incoming arguments beyond the sixth
     Note that they are always given octawords offsets.
  */
  for (ix = 6; ix < args -> size; ix++)
    { decl arg = args -> array[ix];
      int vnr = arg -> Formal.vnr;
      var opd = im_vars -> array[vnr];
      if (opd -> vref || (alpha_size_from_type (opd -> vtype) == 8))
	opd -> vsize = 8;
      else opd -> vsize = 4;
      opd -> offset = local_offset;
      local_offset += 8;
    };

  return (local_alloc);
};

/*
   Code global vars
*/
static void code_global_variables (int_list globals)
{ int ix;
  if (!globals -> size) return;
  code_line ("\t/* global variables */");
  code_line ("\t.section\t.sbss,\"aw\"");
  for (ix = 0; ix < globals -> size; ix++)
    { int vnr = globals -> array[ix];
      var opd = im_vars -> array[vnr];
      int size = opd -> vsize;
      if (!opd -> vglob) continue;
      code_line ("\t.type\tG%d,@object\t/* %s */", vnr, opd -> vname);
      code_line ("\t.size\tG%d,%d", vnr, size);
      code_line ("\t.align\t%d", (size == 4)?2:3);
      code_line ("G%d:\t.zero\t%d", vnr, size);
    };
  code_line ("\t.text");
};

/*
   Loading of integers
*/
static void code_load_integer_constant (int con, string regtype, int regnr)
{ if ((-32768 <= con) && (con < 32768))
    code_line ("\tlda\t$%s%d,%d", regtype, regnr, con);
  else
    { code_line ("\tldah\t$%s%d", regtype, regnr, con >> 16);
      if (con & 0xffff)
	code_line ("\tlda\t$%s%d,%d(%s%d)", regtype, regnr, con & 0xffff, regtype, regnr);
    };
};

/*
   Coding of operands
   Conventions: $r8 is always used for temporary addresses
*/
static void code_load_operand (oprd opd, string dd, string regtype, int regnr)
{ switch (opd -> tag)
    { case TAGIconst:
	code_load_integer_constant (opd -> Iconst.ival, regtype, regnr);
	break;
      case TAGRconst:
	{ int nr = new_const_label ();
	  code_line ("\tlda\t$r8,$CC%d", nr);
	  code_line ("\tld%s\t$%s%d,0($r8)", dd, regtype, regnr);
	  code_line ("\t.section\t.rodata");
	  code_line ("\t.align\t3");
	  code_line ("$CC%d:\t.t_floating\t%e", nr, opd -> Rconst.rval);
	  code_line ("\t.text");
	}; break;
      case TAGTconst:
	{ int nr = new_const_label ();
	  code_line ("\tlda\t$%s%d,$CC%d",regtype, regnr, nr);
	  code_line ("\t.section\t.rodata");
	  code_line ("\t.align\t2");
	  code_line ("\t.short\t-1");
	  code_line ("\t.short\t-1");
	  code_string ("$CC%d:\t.asciz\t", nr);
	  code_quoted_string (opd -> Tconst.tval);
	  code_line ("\n\t.text");
	}; break;
      case TAGVar:
	{ var obj = im_vars -> array[opd -> Var.vnr];
	  int offset = obj -> offset;
	  if (obj -> vglob)
	    { code_line ("\tlda\t$r8,G%d", opd -> Var.vnr);
	      code_line ("\tld%s\t$%s%d,0($r8)", dd, regtype, regnr);
	    }
	  else if (iabs (offset) >= 32768)
	    { code_line ("\tldah\t$r8,%d($fp)", offset >> 16);
	      if (obj -> vref)
		{ code_line ("\tldq\t$r8,%d($r8)", offset & 0xffff);
		  code_line ("\tld%s\t$%s%d,0($r8)", dd, regtype, regnr);
		}
	      else code_line ("\tld%s\t$%s%d,%d($r8)", dd, regtype, regnr, offset & 0xffff);
	    }
	  else if (obj -> vref)
	    { code_line ("\tldq\t$r8,%d($fp)", offset);
	      code_line ("\tld%s\t$%s%d,0($r8)", dd, regtype, regnr);
	    }
	  else code_line ("\tld%s\t$%s%d,%d($fp)", dd, regtype, regnr, offset);
	}; break;
      case TAGRts_nil:
	code_load_integer_constant (1, regtype, regnr);
	break;
      case TAGBlock:
      case TAGLab:
      case TAGRts:
      case TAGNop: dcg_internal_error ("code_load_operand");
      default: dcg_bad_tag (opd -> tag, "code_load_operand");
    };
}

static void code_load_address (oprd opd, int regnr)
{ switch (opd -> tag)
    { case TAGRts_nil:
      case TAGIconst:
      case TAGRconst:
      case TAGTconst:
      case TAGBlock:
      case TAGLab:
      case TAGRts:
      case TAGNop: dcg_internal_error ("code_load_address");
      case TAGVar:
	{ var obj = im_vars -> array[opd -> Var.vnr];
	  int offset = obj -> offset;
	  if (obj -> vglob)
	    code_line ("\tlda\t$r%d,G%d", regnr, opd -> Var.vnr);
	  else if (iabs (offset) >= 32768)
	    { code_line ("\tldah\t$r8,%d($fp)", offset >> 16);
	      if (obj -> vref)
	        code_line ("\tldq\t$r%d,%d($r8)", regnr, offset & 0xffff);
	      else code_line ("\tlda\t$r%d,%d($r8)", regnr, offset & 0xffff);
	    }
          else if (obj -> vref)
            code_line ("\tldq\t$r%d,%d($fp)", regnr, offset);
	  else code_line ("\tlda\t$r%d,%d($fp)", regnr, offset);
	}; break;
      default: dcg_bad_tag (opd -> tag, "code_load_address");
    };
}

static void code_store_operand (oprd opd, string dd, string regtype, int regnr)
{ switch (opd -> tag)
    { case TAGVar:
	{ var obj = im_vars -> array[opd -> Var.vnr]; 
	  int offset = obj -> offset;
	  if (obj -> vglob)
	    { code_line ("\tlda\t$r8,G%d", opd -> Var.vnr);
	      code_line ("\tst%s\t$%s%d,0($r8)", dd, regtype, regnr);
	    }
	  else if (iabs (offset) >= 32768)
	    { code_line ("\tldah\t$r8,%d($fp)", offset >> 16);
	      if (obj -> vref)
		{ code_line ("\tldq\t$r8,%d($r8)", offset & 0xffff);
		  code_line ("\tst%s\t$%s%d,0($r8)", dd, regtype, regnr);
		}
	      else code_line ("\tst%s\t$%s%d,%d($r8)", dd, regtype, regnr, offset & 0xffff);
	    }
	  else if (obj -> vref)
	    { code_line ("\tldq\t$r8,%d($fp)", offset);
	      code_line ("\tst%s\t$%s%d,0($r8)", dd, regtype, regnr);
	    }
	  else code_line ("\tst%s\t$%s%d,%d($fp)", dd, regtype, regnr, offset);
	}; break;
      case TAGReflexive:
	{ code_line ("\tldq\t$r8,0($fp)");
	  code_line ("\tst%s\t$%s%d,0($r8)", dd, regtype, regnr);
	};
      case TAGNop: break;
      case TAGIconst:
      case TAGRconst:
      case TAGTconst:
      case TAGBlock:
      case TAGLab:
      case TAGRts: dcg_internal_error ("code_store_operand");
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
			   8 * fld -> offset, 8 * alpha_size_from_type (ftype));
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
  code_line ("\t.set\tnoat");
  code_line ("\t.set\tnoreorder");
  if (!generate_stabs) return;
  code_line ("\n\t/* specify anonymous C types */");
  code_line ("\t.stabs\t\":t1=r1;-2147483648;2147483647;\",128,0,0,0");	/* int */
  code_line ("\t.stabs\t\":t2=r2;0;127;\",128,0,0,0");	/* char */
  code_line ("\t.stabs\t\":t3=r1;8;0;\",128,0,0,0");	/* double */
  code_line ("\t.stabs\t\":t4=4\",128,0,0,0");		/* void */
  code_line ("\t.stabs\t\":t5=*2\",128,0,0,0");		/* char * */
  code_line ("\t.stabs\t\":t6=*5\",128,0,0,0");		/* char ** */
  code_line ("\t.stabs\t\":t7=*4\",128,0,0,0");		/* void * */
  code_string ("\t.stabs\t\":t8=s24stream:7,0,32;fname:5,64,32;");
  code_line ("opened:1,128,32;dir:1,160,32;;\",128,0,0,0");

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
{ /*
     Allocate space for double args, locals and save area
     Note that the RA and old FP are saved at extra_alloc and
     extra_alloc + 8 resp. in the local stack frame
  */
  int extra_alloc, local_alloc;
  extra_alloc = longest_call_alloc (p);
  if (generate_stabs && (extra_alloc <= 4)) extra_alloc = 4;
  local_alloc = allocate_inargs_and_locals (p -> args, p -> locals, extra_alloc);
  p -> tmp1 = extra_alloc;
  p -> tmp2 = local_alloc;

  /* Code program header */
  code_global_variables (p -> locals);
  code_line ("\t.text");
  code_line ("\t.align\t5");
  code_line ("\t.globl\tmain");
  code_line ("\t.ent\tmain");
  if (generate_stabs)
    { code_line ("\t.stabs\t\"main:F1\",36,0,0,main");
      code_line ("\n\t.stabs\t\"argc:p1\",160,0,0,0");
      code_line ("\t.stabs\t\"argv:p6\",160,0,0,8");
    };

  /* Code program entry */
  code_line ("main:\t.frame\t$fp,%d,$r26,0", local_alloc * 4);
  code_line ("\t.mask\t0x4008000,-%d", (local_alloc - extra_alloc) * 4);
  code_line ("\tldgp\t$gp,0($r27)");		/* setup GOT pointer */
  code_line ("$main..ng:");

  /* Allocate stack space and setup frame pointer */
  if (local_alloc >= 8192)
    code_line ("\tldah\t$sp,-%d($sp)", local_alloc >> 14);
  code_line ("\tlda\t$sp,-%d($sp)", local_alloc * 4);
  code_line ("\tstq\t$r26,%d($sp)", extra_alloc * 4);	/* save return address */
  code_line ("\tstq\t$fp,%d($sp)", extra_alloc * 4 + 8);	/* save old fp */
  code_line ("\tmov\t$sp,$fp");				/* copy sp into new fp */
  code_line ("\t.prologue\t1");
  if (generate_stabs)					/* store args for gdb */
    { code_line ("\tstl\t$r16,0($fp)");
      code_line ("\tstq\t$r17,8($fp)");
    };

  /* Code call to initialize rts */
  code_line ("\tjsr\t$r26,rts_init");		/* call rts_init */
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
}

/*
   Code program end
*/
static void code_end (proc p)
{ int extra_alloc = p -> tmp1;
  int local_alloc = p -> tmp2;

  /* Code call to finialize rts */
  code_line ("\tjsr\trts_finish");		/* call rts_finish */
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */

  /* Deallocate stack space */
  code_line ("\tmov\t$fp,$sp");				/* copy fp into sp */
  code_line ("\tldq\t$r26,%d($sp)", extra_alloc * 4);	/* load return address */
  code_line ("\tldq\t$fp,%d($sp)", extra_alloc * 4 + 8);	/* load old fp */
  if (local_alloc >= 8192)
     code_line ("\tldah\t$sp,%d($sp)", local_alloc >> 14);
  code_line ("\tlda\t$sp,%d($sp)", local_alloc * 4);	/* deallocate stack */

  /* return to caller */
  code_line ("\tmov\t$r31,$r0");		/* return code is 0 */
  code_line ("\tret\t$r31,($r26),1");

  /* code gdb info */
  code_line ("\t.end\tmain");
}

/*
   Code procedure entry
   Note that the incoming arguments are stored as near to the $fp
   as possible. Currently we assume that the offset is less than 32K
*/
static void code_store_arguments (decl_list decls)
{ int ix;
  for (ix = 0; (ix < decls -> size) && (ix < 6); ix++)
    { decl arg = decls -> array[ix];
      int vnr = arg -> Formal.vnr;
      var opd = im_vars -> array[vnr];
      int offset = opd -> offset;
      if (opd -> vref)			/* an address */
	code_line ("\tstq\t$%d,%d($fp)", 16 + ix, offset);
      else if (opd -> vsize == 4)
	code_line ("\tstl\t$%d,%d($fp)", 16 + ix, offset);
      else if (is_a_floating_type (opd -> vtype))
	code_line ("\tstt\t$f%d,%d($fp)", 16 + ix, offset);
      else code_line ("\tstq\t$%d,%d($fp)", 16 + ix, offset);
    };
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
  code_line ("\t.stabs\t\"%s:%s%d\",160,0,0,%d", arg -> name, facc_info,
	     finfo -> stabs_nr, opd -> offset);
}

static void code_enter (proc p, tuple t)
{ /*
     Allocate space for double args, locals and save area
     Note that the RA and old FP are saved at extra_alloc 
     extra_alloc + 8 in the local stack frame
  */
  int extra_alloc = longest_call_alloc (p);
  int local_alloc = allocate_inargs_and_locals (p -> args, p -> locals, extra_alloc);
  int ix;
  p -> tmp1 = extra_alloc;
  p -> tmp2 = local_alloc;

  /* Code procedure header for gdb */
  code_line ("\t.text");
  code_line ("\t.align\t5");
  code_line ("\t.ent\tP%d", p -> pnr);
  if (generate_stabs)
    { type rtype = p -> rtyp;
      type_info rinfo = type_table -> array[rtype -> tnr];
      code_line ("\t.stabs\t\"%s:f%d\",36,0,0,P%d", p -> pname, rinfo -> stabs_nr, p -> pnr);
      for (ix = 0; ix < p -> args -> size; ix++)
	code_parameter_stabs_entry (p -> args -> array[ix], ix + 1);
    };

  /* Code program entry */
  code_line ("P%d:\t.frame\t$fp,%d,$r26,0", p -> pnr, local_alloc * 4);
  code_line ("\t.mask\t0x4008000,-%d", (local_alloc - extra_alloc) * 4);
  code_line ("\tldgp\t$r29,0($r27)");		/* setup GOT pointer */
  code_line ("$P%d..ng:", p -> pnr);

  /* Allocate stack space and setup frame pointer */
  if (local_alloc >= 8192)
    code_line ("\tldah\t$sp,-%d($sp)", local_alloc >> 14);
  code_line ("\tlda\t$sp,-%d($sp)", local_alloc * 4);
  code_line ("\tstq\t$r26,%d($sp)", extra_alloc * 4);	/* save return address */
  code_line ("\tstq\t$fp,%d($sp)", extra_alloc * 4 + 8);	/* save old fp */
  code_line ("\tmov\t$sp,$fp");				/* copy sp into new fp */
  code_line ("\t.prologue\t1");

  /* argument storing */
  code_store_arguments (p -> args);
}

/*
   Code procedure leave
*/
static void code_leave (proc p, tuple t)
{ int extra_alloc = p -> tmp1;
  int local_alloc = p -> tmp2;
  type rtype = p -> rtyp;
  int rsize = alpha_size_from_type (rtype);

  /* load return value in r0 or f0 */
  if (is_a_floating_type (rtype)) code_load_operand (t -> op2, "t", "f", 0);
  else if (rsize & 4) code_load_operand (t -> op2, "l", "r", 0);
  else if (rsize & 8) code_load_operand (t -> op2, "q", "r", 0);

  /* Discard stack frame */
  code_line ("\tmov\t$fp,$sp");				/* copy fp into sp */
  code_line ("\tldq\t$r26,%d($sp)", extra_alloc * 4);	/* load return address */
  code_line ("\tldq\t$fp,%d($sp)", extra_alloc * 4 + 8);	/* load old fp */
  if (local_alloc >= 8192)
    code_line ("\tldah\t$sp,%d($sp)", local_alloc >> 14);
  code_line ("\tlda\t$sp,%d($sp)", local_alloc * 4);	/* deallocate stack */

  /* return to caller */
  code_line ("\tret\t$r31,($r26),1");

  /* code gdb info */
  code_line ("\t.end\tP%d", p -> pnr);
}

/*
   Coding routines to support the C calling conventions
*/

/*
   Prepare call: No code on sparc
   [ pcall, nr_args, --, -- ]
*/
static void code_prepare_call (tuple t)
{
}

static void code_store_single_parameter (oprd op1, string type, string reg, int argno)
{ if (argno < 6)
    code_load_operand (op1, type, reg, 16 + argno);
  else
    { code_load_operand (op1, type, reg, 1);
      code_line ("\tst%s\t$%s1,%d($sp)", type, reg, (argno - 6) * 8);
    };
}

/*
   Code the loading of procedure parameters and storing of procedure results
*/
static void code_load_parameter (oprd opd, int argno)
{ type oprdtype = type_from_operand (opd);
  int oprdsize = alpha_size_from_type (oprdtype);
  if (!oprdsize) return;
  else if (is_a_floating_type (oprdtype))
    code_store_single_parameter (opd, "t", "f", argno);
  else if (oprdsize & 4) 
    code_store_single_parameter (opd, "l", "r", argno);
  else code_store_single_parameter (opd, "q", "r", argno);
}

static void code_store_result (oprd dest)
{ type desttype = type_from_operand (dest);
  int destsize = alpha_size_from_type (desttype);
  if (!destsize) return;
  else if (destsize & 4)
    code_store_operand (dest, "l", "r", 0);
  else if (is_a_floating_type (desttype))
    code_store_operand (dest, "t", "f", 0);
  else code_store_operand (dest, "q", "r", 0);
}

/*
   Code load (real) parameter
   [ lpar(r), parm, argno, -- ]
*/
static void code_tuple_load_parameter (tuple t)
{ int argno;
  if (t -> op2 -> tag != TAGIconst)
    dcg_internal_error ("code_tuple_load_parameter");
  argno = t -> op2 -> Iconst.ival;
  code_load_parameter (t -> op1, argno);
}

/*
   Code load address of parameter
   [ lpar&, parm, argno, --]
*/
static void code_load_address_parameter (tuple t)
{ int argno;
  if (t -> op2 -> tag != TAGIconst)
    dcg_internal_error ("code_load_address_parameter");
  argno = t -> op2 -> Iconst.ival;
  if (argno < 6) code_load_address (t -> op1, 16 + argno);
  else
    { code_load_address (t -> op1, 1);
      code_line ("\tstq\t$r1,%d($sp)", (argno - 6) * 8);
    };
}

/*
   Code runtime system call
*/
static void code_rts_call (tuple t, string rts_name)
{ /* Load the operands into the regs */
  code_load_parameter (t -> op2, 1);
  code_load_parameter (t -> op1, 0);

  /* Code the call */
  code_line ("\tjsr\t$r26,%s", rts_name);
  code_line ("\tldgp\t$gp,0($r26)");	/* restore GOT pointer */
  code_store_result (t -> dst);
}

/*
   Code call and store result
   [ call (r), proc, --, dst ]
*/
static void code_call (tuple t)
{ if (t -> dst -> tag == TAGReflexive)
    code_line ("\tstq\t$r16,0($sp)");
  
  /* We do not call code_called_proc because of alphaness */
  switch (t -> op1 -> tag)
    { case TAGProc:
	code_line ("\tjsr\t$r26,P%d", t -> op1 -> Proc.pnr);
	break;
      case TAGRts:
	code_line ("\tjsr\t$r26,%s", t -> op1 -> Rts.proc);
	break;
      default: dcg_bad_tag (t -> op1 -> tag, "code_call");
    };
  code_line ("\tldgp\t$gp,0($r26)");	/* restore GOT pointer */
  code_store_result (t -> dst);
}

/*
   Code jump
   [ jmp, --, --, dst ]
*/
static void code_jump (tuple t)
{ if (t -> dst -> tag != TAGBlock)
    dcg_internal_error ("code_jump");
  code_line ("\tbr\t$r31,$BB%d", t -> dst -> Block.bnr);
}

/*
   Code jump if nil
   [ jmp_if_nil, oprd, --, dst ]
*/
static void code_jump_if_nil (tuple t)
{ code_load_operand (t -> op1, "q", "r", 1);
  code_line ("\tsubq\t$r1,1,$r1");
  code_line ("\tbeq\t$r1,$BB%d", t -> dst -> Block.bnr);
}

/*
   Code jump if null
   [ jmp_if_null, oprd, --, dst ]
*/
static void code_jump_if_null (tuple t)
{ code_load_operand (t -> op1, "q", "r", 1);
  code_line ("\tbeq\t$r1,$BB%d", t -> dst -> Block.bnr);
}

/*
   Code jump if condition
   [ jump_if_int_less_than_int, oprd1, oprd2, dst ]
*/
static void code_cond_jump (tuple t, string dd, string opc, int tcc)
{ code_load_operand (t -> op1, dd, "r", 1);
  code_load_operand (t -> op2, dd, "r", 2);
  code_line ("\tcmp%s\t$r1,$r2,$r3", opc);
  code_line ("\tb%s\t$r3,$BB%d", ((tcc)?"ne":"eq"), t -> dst -> Block.bnr);
}

/*
   Coding routines for garbage collection
*/

/* [ undefine, --, --, dest ] */
static void code_undefine (tuple t)
{ code_store_operand (t -> dst, "q", "r", 31);
}

/*
   Allocate space for row/struct
   [ allocate, space in bytes, --, dest ]
*/
static void code_allocate (tuple t)
{ code_load_operand (t -> op1, "q", "r", 16);
  code_line ("\tjsr\t$r26,rts_malloc");
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
  code_store_operand (t -> dst, "q", "r", 0);
}

/*
   Set guard on var object and possibly create space for it
   [ guard, oprd, space in bytes, dest ]
*/
static void code_guard (tuple t)
{ /* load the size and oprd */
  code_load_operand (t -> op2, "l", "r", 17);
  code_load_operand (t -> op1, "q", "r", 16);
  
  /* call rts_guard and store result */
  code_line ("\tjsr\t$r26,rts_guard");
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
  code_store_operand (t -> dst, "q", "r", 0);
}

/*
   Attach object
   [ att&, oprd, --, dest/--/reflexive ]
*/
static void code_attach_object (tuple t)
{ code_load_address (t -> op1, 16);		/* load arg in o0 */
  if (t -> dst -> tag == TAGReflexive)
    code_line ("\tstq\t$r16,0($sp)");
  code_line ("\tjsr\t$r26,rts_attach");		/* call rts_attach */
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
  code_store_operand (t -> dst, "q", "r", 0);
}

/*
   Attach object pointed to by oprd + offset
   [ att_off, addr, offset, dest/--/reflexive ]
*/
static void code_attach_offset (tuple t)
{ code_load_operand (t -> op1, "q", "r", 16);
  switch (t -> op2 -> tag)
    { case TAGNop: break;
      case TAGIconst:
	{ int fixed_offset = t -> op2 -> Iconst.ival;
	  if (!fixed_offset) break;
	  if ((-32768 <= fixed_offset) && (fixed_offset < 32768))
	     { code_line ("\taddq\t$r16,%d,$r16", fixed_offset);
	       break;
	     };
	};
      default:
	{ code_load_operand (t -> op2, "l", "r", 1);
	  code_line ("\taddq\t$r16,$r1,$r16");
	};
     };	    

  /* check for reflexive destination */
  if (t -> dst -> tag == TAGReflexive)
    code_line ("\tstq\t$r16,0($sp)");
  code_line ("\tjsr\t$r26,rts_attach");
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
  code_store_operand (t -> dst, "q", "r", 0);
}

/*
   Detach variable. Note that rts_detach is called by reference
   [ det&, oprd, --, -- ]
*/
static void code_detach_object (tuple t)
{ code_load_address (t -> op1, 16);		/* load arg in o0 */
  code_line ("\tjsr\t$r26,rts_detach");		/* call rts_detach */
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
}

/*
   Attach object pointed to by oprd + offset
   [ det_off, addr, offset, -- ]
*/
static void code_detach_offset (tuple t)
{ code_load_operand (t -> op1, "q", "r", 16);
  switch (t -> op2 -> tag)
    { case TAGNop: break;
      case TAGIconst:
	{ int fixed_offset = t -> op2 -> Iconst.ival;
	  if (!fixed_offset) break;
	  if ((-32768 <= fixed_offset) && (fixed_offset < 32768))
	    { code_line ("\taddq\t$r16,%d,$r16", fixed_offset);
	      break;
	    };
	};
      default:
	{ code_load_operand (t -> op2, "l", "r", 1);
	  code_line ("\taddq\t$r16,$r1,$r16");
	};
     };	    
  code_line ("\tjsr\t$r26,rts_detach");		/* call rts_detach */
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
}

/*
   Predetach object. If the value returned by predetach is not NULL,
   the object contains subfields that should be detached first
   [ predet&, oprd, --, dest ]
*/
static void code_predetach_object (tuple t)
{ code_load_address (t -> op1, 16);		/* load arg in r16 */
  code_line ("\tjsr\t$r26,rts_predetach");	/* call rts_predetach */
  code_line ("\tldgp\t$gp,0($r26)");		/* restore GOT pointer */
  code_store_operand (t -> dst, "q", "r", 0);	/* store result */
}

/*
   Rts support actions
*/

/*
   Code test for nil
   [ is_nil, oprd, --, dest]
*/
static void code_is_nil (tuple t)
{ code_load_operand (t -> op1, "q", "r", 1);
  code_line ("\tlda\t$r2,1");
  code_line ("\tcmpeq\t$r1,$r2,$r3");
  code_store_operand (t -> dst, "l", "r", 3);
}

/*
   Code to add an address and an offset
   [ A+I, adr_oprd, int_oprd, dest ]
*/
static void code_addr_plus_offset (tuple t)
{ int lnr = new_local_label ();
  code_load_operand (t -> op1, "q", "r", 1);
  code_line ("\tbne\t$r1,$LL%d", lnr);
  code_line ("\tjsr\t$26,rts_offset_from_0");
  code_string ("$LL%d:", lnr);
  switch (t -> op2 -> tag)
    { case TAGNop: break;
      case TAGIconst:
        { int fixed_offset = t -> op2 -> Iconst.ival;
	  if (!fixed_offset) break;
	  if ((-32768 <= fixed_offset) && (fixed_offset < 32768))
	    { code_line ("\taddq\t$r1,%d,$r1", fixed_offset);
	      break;
            };
	};
      default:
	{ code_load_operand (t -> op2, "l", "r", 2);
	  code_line ("\taddq\t$r1,$r2,$r1");
	};
     };	    
  code_store_operand (t -> dst, "q", "r", 1);
}

/*
   Code to adjust the row index and check its bounds
   [ row_idx, oprd, upb, dest ]
*/
static void code_row_index (tuple t)
{ int lnr1 = new_local_label ();
  int lnr2 = new_local_label ();
  code_load_operand (t -> op1, "l", "r", 1);
  code_line ("\tsubl\t$r1,1,$r1");		/* rows start at 1 */
  code_line ("\tbge\t$r1,$LL%d", lnr1);
  code_line ("\tjsr\t$r26,rts_index_out_of_range");
  code_string ("$LL%d:", lnr1);
  if ((t -> op2 -> tag == TAGIconst) &&
      (-32768 <= t -> op2 -> Iconst.ival) && (t -> op2 -> Iconst.ival < 32768))
    code_line ("\tsubl\t$r1,%d,$r2", t -> op2 -> Iconst.ival);
  else
    { code_load_operand (t -> op2, "l", "r", 2);
      code_line ("\tsubl\t$r1,$r2,$r1");
    }
  code_line ("\tblt\t$r2,$LL%d", lnr2);
  code_line ("\tjsr\t$r26,rts_index_out_of_range");
  code_string ("$LL%d:", lnr2);
  code_store_operand (t -> dst, "l", "r", 1);
}

/*
   Coding of copy actions
*/

/*
   Copy address of
   [ :=&, oprd, --, dst ]
*/
static void code_copy_adr_of (tuple t)
{ code_load_address (t -> op1, 1);		/* load address */
  code_store_operand (t -> dst, "q", "r", 1);	/* store it away */
}

static void code_copy (tuple t, string type)
{ code_load_operand (t -> op1, type, "r", 1);	/* load operand */
  code_store_operand (t -> dst, type, "r", 1);	/* store it away */
}

static void code_copy_indirect (tuple t, string type)
{ code_load_operand (t -> op1, type, "r", 1);	/* load operand */
  code_load_operand (t -> dst, "q", "r", 2);	/* load dest address */
  code_line ("\tst%s\t$r1,0($r2)", type);	/* store it away */
}

static void code_indirect_copy (tuple t, string type)
{ code_load_operand (t -> op1, "q", "r", 1);	/* load src address */
  code_line ("\tld%s\t$r2,0($r1)", type);	/* load operand */
  code_store_operand (t -> dst, type, "r", 2);	/* store it away */
}

static void code_copy_real (tuple t)
{ code_load_operand (t -> op1, "t", "f", 1);	/* load operand */
  code_store_operand (t -> dst, "t", "f", 1);	/* store it away */
}

static void code_copy_real_indirect (tuple t)
{ code_load_operand (t -> op1, "t", "f", 1);	/* load operand */
  code_load_operand (t -> dst, "q", "r", 1);	/* load dest address */
  code_line ("\tstt\t$f1,0($r1)");		/* store it away */
}

static void code_copy_indirect_real (tuple t)
{ code_load_operand (t -> op1, "q", "r", 1);	/* load src address */
  code_line ("\tldt\t$f1,0($r1)");		/* load operand */
  code_store_operand (t -> dst, "t", "f", 1);	/* store it away */
}

/*
   Integer operations
*/
static void code_int_op (tuple t, string opc)
{ code_load_operand (t -> op1, "l", "r", 1);
  code_load_operand (t -> op2, "l", "r", 2);
  code_line ("\t%s\t$r1,$r2,$r3", opc);
  code_store_operand (t -> dst, "l", "r", 3);
}

/* Specific alpha dependency: old Alphas do not have a div/rem */
static void code_int_div (tuple t, int divmod)
{ if (divmod) code_int_op (t, "reml");
  else code_int_op (t, "divl");
}

static void code_negate (tuple t)
{ code_load_operand (t -> op1, "l", "r", 1);
  code_line ("\tsubl\t$r31,$r1,$r1");
  code_store_operand (t -> dst, "l", "r", 1);
}

static void code_int_cmp (tuple t, string opc, int args_rev, int comp)
{ code_load_operand (t -> op1, "l", "r", (args_rev)?2:1);
  code_load_operand (t -> op2, "l", "r", (args_rev)?1:2);
  code_line ("\tcmp%s\t$r1,$r2,$r3", opc);
  if (comp) code_line ("\txor\t$r3,1,$r3");
  code_store_operand (t -> dst, "l", "r", 3);
}

static void code_real_op (tuple t, string opc)
{ code_load_operand (t -> op1, "t", "f", 1);
  code_load_operand (t -> op2, "t", "f", 2);
  code_line ("\t%s\t$f1,$f2,$f3", opc);
  code_store_operand (t -> dst, "t", "f", 3);
}

static void code_int_to_real (tuple t)
{ code_load_operand (t -> op1, "s", "f", 1);
  code_line ("\tcvtlq\t$f1,$f1");
  code_line ("\tcvtqt\t$f1,$f1");
  code_store_operand (t -> dst, "t", "f", 1);
}

static void code_negate_real (tuple t)
{ code_load_operand (t -> op1, "t", "f", 1);
  code_line ("\tsubt\t$f31,$f1,$f1");
  code_store_operand (t -> dst, "t", "f", 1);
}

static void code_real_cmp (tuple t, string opc)
{ int label = new_local_label ();
  code_load_operand (t -> op1, "t", "f", 1);
  code_load_operand (t -> op2, "t", "f", 2);
  code_line ("\tlda\t$r1,1");
  code_line ("\tsubt\t$f1,$f2,$f3");
  code_line ("\tfb%s\t$f3,$LL%d", opc, label);
  code_line ("\tsubl\t$r1,$r1,$r1");
  code_string ("$LL%d:", label);
  code_store_operand (t -> dst, "l", "r", 1);
}

static void code_not (tuple t)
{ code_load_operand (t -> op1, "l", "r", 1);
  code_line ("\txor\t$r1,1,$r1");
  code_store_operand (t -> dst, "l", "r", 1);
}

static void code_sline (tuple t)
{ int lnr;
  if (!generate_stabs) return;
  if (t -> op1 -> tag != TAGIconst)
    dcg_internal_error ("code_sline");
  lnr = new_local_label ();
  code_line ("$LLINE%d:", lnr);
  code_line ("\t.stabn\t68,0,%d,$LLINE%d", t -> op1 -> Iconst.ival, lnr);
}

/*
   The tuple dispatcher
*/
static void code_tuple (proc p, tuple t)
{ switch (t -> opc)
    { /* General */
      case start_program:	     code_start (p, t); break;
      case end_program:		     code_end (p); break;

      /* Procedure calls */
      case prepare_call:	     code_prepare_call (t); break;
      case load_parameter:	     code_tuple_load_parameter (t); break;
      case load_address_parameter:   code_load_address_parameter (t); break;
      case call_procedure:
      case call_attach_procedure:
      case call_detach_procedure:
      case call_guard_procedure:     code_call (t); break;

      /* Procedure entry and return */
      case enter_procedure:	     code_enter (p, t); break;
      case leave_procedure:	     code_leave (p, t); break;

      /* Jumps */
      case jump: 		     code_jump (t); break;
      case jump_if_nil:		     code_jump_if_nil (t); break;
      case jump_if_null:	     code_jump_if_null (t); break;
      case jump_if_addr_equal_addr:	  code_cond_jump (t, "q", "eq", 1); break;
      case jump_if_int_equal_int:	  code_cond_jump (t, "l", "eq", 1); break;
      case jump_if_int_notequal_int:	  code_cond_jump (t, "l", "eq", 0); break;
      case jump_if_int_less_than_int: 	  code_cond_jump (t, "l", "lt", 1); break;
      case jump_if_int_less_equal_int:    code_cond_jump (t, "l", "le", 1); break;
      case jump_if_int_greater_than_int:  code_cond_jump (t, "l", "le", 0); break;
      case jump_if_int_greater_equal_int: code_cond_jump (t, "l", "lt", 0); break;

      /* Garbage collection */
      case undefine:		     code_undefine (t); break;
      case allocate:		     code_allocate (t); break;
      case guard:		     code_guard (t); break;
      case attach_adr:		     code_attach_object (t); break;
      case attach_offset:	     code_attach_offset (t); break;
      case detach_adr:		     code_detach_object (t); break;
      case detach_offset:	     code_detach_offset (t); break;
      case predetach_adr:	     code_predetach_object (t); break;

      /* Copying stuff */
      case copy_adr_of: 	     code_copy_adr_of (t); break;
      case copy_int:		     code_copy (t, "l"); break;
      case copy_adr:		     code_copy (t, "q"); break;
      case copy_int_indirect:	     code_copy_indirect (t, "l"); break;
      case copy_adr_indirect:	     code_copy_indirect (t, "q"); break;
      case copy_indirect_int:	     code_indirect_copy (t, "l"); break;
      case copy_indirect_adr:	     code_indirect_copy (t, "q"); break;
      case copy_real:		     code_copy_real (t); break;
      case copy_real_indirect:	     code_copy_real_indirect (t); break;
      case copy_indirect_real:	     code_copy_indirect_real (t); break;

      /* Index op */
      case is_nil:		     code_is_nil (t); break;
      case addr_plus_offset:	     code_addr_plus_offset (t); break;
      case row_index:		     code_row_index (t); break;

      /* Integer ops */
      case int_plus_int:	     code_int_op (t, "addl"); break;
      case int_minus_int:	     code_int_op (t, "subl"); break;
      case int_times_int:	     code_int_op (t, "mull"); break;
      case int_div_int:		     code_int_div (t, 0); break;
      case int_mod_int:		     code_int_div (t, 1); break;
      case int_pow_int:		     code_rts_call (t, "rts_int_pow_int"); break;
      case negate_int:		     code_negate (t); break;
      case int_equal_int: 	     code_int_cmp (t, "eq", 0, 0); break;
      case int_notequal_int: 	     code_int_cmp (t, "eq", 0, 1); break;
      case int_less_than_int: 	     code_int_cmp (t, "lt", 0, 0); break;
      case int_less_equal_int:       code_int_cmp (t, "le", 0, 0); break;
      case int_greater_than_int:     code_int_cmp (t, "lt", 1, 0); break;
      case int_greater_equal_int:    code_int_cmp (t, "le", 1, 0); break;

      /* Real ops */
      case real_plus_real:	     code_real_op (t, "addt"); break;
      case real_minus_real:	     code_real_op (t, "subt"); break;
      case real_times_real:	     code_real_op (t, "mult"); break;
      case real_divide_real:	     code_real_op (t, "divt"); break;
      case real_pow_int:	     code_rts_call (t, "rts_real_pow_int"); break;
      case negate_real:		     code_negate_real (t); break;
      case real_equal_real: 	     code_real_cmp (t, "eq"); break;
      case real_notequal_real:       code_real_cmp (t, "ne"); break;
      case real_less_than_real:      code_real_cmp (t, "lt"); break;
      case real_less_equal_real:     code_real_cmp (t, "le"); break;
      case real_greater_than_real:   code_real_cmp (t, "gt"); break;
      case real_greater_equal_real:  code_real_cmp (t, "ge"); break;

      /* Bool ops */
      case bool_and_bool: 	     code_int_op (t, "and"); break;
      case bool_or_bool: 	     code_int_op (t, "bis"); break;
      case bool_xor_bool:	     code_int_op (t, "xor"); break;
      case not_bool: 		     code_not (t); break;

      /* Text operations */
      case text_equal_text:	     code_rts_call (t, "rts_text_equal_text"); break;
      case text_notequal_text:	     code_rts_call (t, "rts_text_notequal_text"); break;
      case text_less_than_text:      code_rts_call (t, "rts_text_less_than_text"); break;
      case text_less_equal_text:     code_rts_call (t, "rts_text_less_equal_text"); break;
      case text_greater_than_text:   code_rts_call (t, "rts_text_greater_than_text"); break;
      case text_greater_equal_text:  code_rts_call (t, "rts_text_greater_equal_text"); break;

      /* Conversions */
      case int_to_real:		     code_int_to_real (t); break;

      /* miscellaneous */
      case sline:		     code_sline (t); break;
      case comment:
      case nop:			     break;

      default: dcg_bad_tag (t -> opc, "code_tuple"); 
    };
}

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

void alpha_code_generation ()
{ int ix;
  dcg_hint ("      generating alpha machine code...");
  code_intro ();
  for (ix = 0; ix < im_procs -> size; ix++)  
    code_procedure (im_procs -> array[ix]);
}
