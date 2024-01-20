/*
   File: tgt_gen_sparc.c
   Generates Sparc machine code

   CVS ID: "$Id: tgt_gen_sparc.c,v 1.12 2011/08/13 15:58:54 marcs Exp $"
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
#include "tgt_gen_sparc.h"

/*
   Sparc < V9 characteristics
   Basically:
      sizeof (VOID) = 0;
      sizeof (INT) = sizeof (void *) = 4;
      sizeof (REAL) = 8;
*/
int sparc_size_from_type (type t)
	{ switch (t -> tag)
	     { case TAGTname:
		  { decl id = t -> Tname.id;
		    if (id -> tag != TAGType)
		       dcg_internal_error ("sparc_size_from_type");
		    return (sparc_size_from_type (id -> Type.concr));
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
	       default: dcg_bad_tag (t -> tag, "sparc_size_from_type");
	     };
	  return (0);
	};

int sparc_regsize_from_arg (pdecl pd)
	{ switch (pd -> pacc)
	     { case acc_var:
	       case acc_proc: return (1);		/* until V9 */
	       default: return (sparc_size_from_type (pd -> ptype) / 4);
	     };
	};

/*
   Sparc coding routines
*/

/*
   Loop over segment to check for the longest call
*/
static int longest_call_in_segment (tuple first)
	{ tuple ptr = first;
	  int max = 6;		/* Default frame */
	  do
	     { if (ptr -> opc == prepare_call)
		  { int lmax;
		    if (ptr -> op1 -> tag != TAGIconst)
		       dcg_bad_tag (ptr -> op1 -> tag, "longest_call_in_segment");
		    lmax = ptr -> op1 -> Iconst.ival;
		    if (lmax > max) max = lmax;
		  };
	       ptr = ptr -> next;
	     }
	  while (ptr != first);
	  return (max);
	};

static int longest_call (proc p)
	{ block bptr;
	  int max = 0;
	  for (bptr = p -> init_block; bptr != block_nil; bptr = bptr -> direct_succ)
	    { int lmax = longest_call_in_segment (bptr -> anchor);
	      if (lmax > max) max = lmax;
	    };
	  if (debug && (max > 6))
	    dcg_hint ("      longest call needed %d arguments", max);
	  return (max);
	}

/*
   Allocate space for arguments
   Space for arguments is allocated from [%fp+68] and then upward
   unless the argument is a const double. In this case a double word
   aligned local is needed as well.
*/
static int allocate_arguments (decl_list args)
	{ int local_offset = 0;
	  int arg_offset = 68;
	  int local_alloc = 0;
	  int ix;
	  for (ix = 0; ix < args -> size; ix++)
	     { decl arg = args -> array[ix];
	       int vnr = arg -> Formal.vnr;
	       var opd = im_vars -> array[vnr];
	       if (!opd -> vref && (sparc_size_from_type (opd -> vtype) == 8))
		  { local_offset -= 8;
		    local_alloc += 2;
		    opd -> offset = local_offset;
		    opd -> vsize = 8;
		  }
	       else
		  { opd -> offset = arg_offset;
		    opd -> vsize = 4;
		    arg_offset += 4;
		  };
	       if (debug)
		 dcg_hint ("         gave %sarg %s of size %d offset %d",
			(opd -> vref)?"var ":"", opd -> vname, opd -> vsize, opd -> offset);
	     };
	  return (local_alloc);
	};

/*
   We allocate from [%fp-4*alloc] downward, since some space may already have
   been allocated to double word arguments. To proceed, we first allocate double
   word objects and then all single word objects. Finally we realign the frame to
   double word boundaries.
*/
static int allocate_locals (int_list locals, int alloc)
	{ int local_offset = -alloc * 4;
	  int local_alloc = alloc;
	  int ix;

	  /* Loop over locals, fill in size, allocate doubles */
	  for (ix = 0; ix < locals -> size; ix++)
	     { int vnr = locals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       int size = sparc_size_from_type (opd -> vtype);
	       opd -> vsize = size;
	       if (size & 4) continue;
	       if (opd -> vglob) continue;
	       local_offset -= 8;
	       local_alloc += 2;
	       opd -> offset = local_offset;
	     };
	  for (ix = 0; ix < locals -> size; ix++)
	     { int vnr = locals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       if (opd -> vglob) continue;
	       if (opd -> vsize == 4)
		  { local_offset -= 4;
		    local_alloc++;
		    opd -> offset = local_offset;
		  };
	       if (debug) dcg_hint ("         gave %s of size %d offset %d",
				    opd -> vname, opd -> vsize, opd -> offset);
	     };

	  /* make sure frame is double word aligned */
	  if (local_alloc & 1) local_alloc++;
	  return (local_alloc);
	};

/*
   Code global vars
*/
static void code_global_variables (int_list globals)
	{ int ix;
	  if (old_os) code_line ("\t.data");
	  for (ix = 0; ix < globals -> size; ix++)
	     { int vnr = globals -> array[ix];
	       var opd = im_vars -> array[vnr];
	       int size = opd -> vsize;
	       if (!opd -> vglob) continue;
	       if (old_os)
		  { code_line ("\t.align\t%d", size);
		    code_line ("G%d:\t.skip\t%d\t/* %s */", vnr, size, opd -> vname);
		  }
	       else
		  { code_line ("\t.local\tG%d\t/* %s */", vnr, opd -> vname);
		    code_line ("\t.comm\tG%d,%d,%d", vnr, size, size);
		  };
	     };
	  if (old_os) code_line ("\t.text");
	};

/*
   Coding of operands
*/
static void code_load_integer_constant (int con, string regtype, int regnr)
	{ if ((-4096 <= con) && (con < 4096))
	     code_line ("\tor\t%%g0,%d,%%%s%d", con, regtype, regnr);
	  else
	     { code_line ("\tsethi\t%%hi(%d),%%%s%d", con, regtype, regnr);
	       code_line ("\tor\t%%%s%d,%%lo(%d),%%%s%d", regtype, regnr,
			  con, regtype, regnr);
	     };
	};

static void code_load_integer_constant_to_float (int con, int regnr)
	{ int nr = new_const_label ();
	  code_line ("\t.section\t.rodata");
	  code_line ("\t.align\t4");
	  code_line ("\t.word\t%d", con);
	  code_line ("\t.text");
	  code_line ("\tsethi\t%%hi(CC%d),%%l7", nr);
	  code_line ("\tldf\t[%%l7+%%lo(CC%d)],%%f%d", nr, regnr); 
	};

/*
   Code the loading of an operand
   REAL and TEXT constants are allocated in the read only data segment
*/
static void code_load_operand (oprd opd, string dd, string regtype, int regnr)
	{ switch (opd -> tag)
	     { case TAGIconst:
		  code_load_integer_constant (opd -> Iconst.ival, regtype, regnr);
		  break;
	       case TAGRconst:
		  { int nr = new_const_label ();
		    code_line ("\t.section\t.rodata");
		    code_line ("\t.align\t8");
		    code_line ("CC%d:\t.double\t0r%e", nr, opd -> Rconst.rval);
		    code_line ("\t.text");
		    code_line ("\tsethi\t%%hi(CC%d),%%l7", nr);
		    code_line ("\tld%s\t[%%l7+%%lo(CC%d)],%%%s%d", dd, nr, regtype, regnr);
		  }; break;
	       case TAGTconst:
		  { int nr = new_const_label ();
		    code_line ("\t.section\t.rodata");
		    code_line ("\t.align\t4");
		    code_line ("\t.half\t-1");
		    code_line ("\t.half\t-1");
		    code_string ("CC%d:\t.asciz\t", nr);
		    code_quoted_string (opd -> Tconst.tval);
		    code_line ("\n\t.text");
		    code_line ("\tsethi\t%%hi(CC%d),%%%s%d", nr, regtype, regnr);
		    code_line ("\tor\t%%%s%d,%%lo(CC%d),%%%s%d",
					regtype, regnr, nr, regtype, regnr);
		  }; break;
	       case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    int offset = obj -> offset;
		    if (obj -> vglob)
		       { code_line ("\tsethi\t%%hi(G%d),%%l7", opd -> Var.vnr);
			 code_line ("\tld%s\t[%%l7+%%lo(G%d)],%%%s%d",
				    dd, opd -> Var.vnr, regtype, regnr);
		       }
		    else if (obj -> vref)
		       { /* reference argument, can't have too large offset */
			 code_line ("\tld\t[%%fp+%d],%%l7", offset);
			 code_line ("\tld%s\t[%%l7],%%%s%d", dd, regtype, regnr);
		       }
		    else if (iabs (offset) >= 4096)
		       { code_load_integer_constant (-offset, "l", 7);
		         code_line ("\tsub\t%%fp,%%l7,%%l7");
			 code_line ("\tld%s\t[%%l7],%%%s%d", dd, regtype, regnr);
		       }
		    else if (offset < 0)
		       code_line ("\tld%s\t[%%fp%d],%%%s%d", dd, offset, regtype, regnr);
		    else code_line ("\tld%s\t[%%fp+%d],%%%s%d", dd, offset, regtype, regnr);
		  }; break;
	       case TAGRts_nil:
		  code_load_integer_constant (1, regtype, regnr);
		  break;
	       case TAGLab:
	       case TAGRts:
	       case TAGNop: dcg_internal_error ("code_load_operand");
	       default: dcg_bad_tag (opd -> tag, "code_load_operand");
	     };
	};

static void code_load_address (oprd opd, string regtype, int regnr)
	{ switch (opd -> tag)
	     { case TAGRts_nil:
	       case TAGIconst:
	       case TAGRconst:
	       case TAGTconst:
	       case TAGLab:
	       case TAGNop: dcg_internal_error ("code_load_address");
	       case TAGRts:
		  { code_line ("\tsethi\t%%hi(%s),%%%s%d", opd -> Rts.proc, regtype, regnr);
		    code_line ("\tor\t%%%s%d,%%lo(%s),%%%s%d",
			       regtype, regnr, opd -> Rts.proc, regtype, regnr);
		  }; break;
	       case TAGProc:
		  { code_line ("\tsethi\t%%hi(P%d),%%%s%d", opd -> Proc.pnr, regtype, regnr);
		    code_line ("\tor\t%%%s%d,%%lo(P%d),%%%s%d",
			       regtype, regnr, opd -> Proc.pnr, regtype, regnr);
		  }; break;
	       case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr];
		    int offset = obj -> offset;
		    if (obj -> vglob)
		       { code_line ("\tsethi\t%%hi(G%d),%%%s%d", opd -> Var.vnr, regtype, regnr);
			 code_line ("\tor\t%%%s%d,%%lo(G%d),%%%s%d",
				    regtype, regnr, opd -> Var.vnr, regtype, regnr);
		       }
		    else if (obj -> vref)
		       /* reference argument, can't have too large offset */
		       code_line ("\tld\t[%%fp+%d],%%%s%d", offset, regtype, regnr);
		    else if (iabs (offset) >= 4096)
		       { code_load_integer_constant (-offset, "l", 7); /* into l7 */
			 code_line ("\tsub\t%%fp,%%l7,%%%s%d",
				    regtype, regnr);
		       }
		    else if (offset < 0)
		       code_line ("\tsub\t%%fp,%d,%%%s%d", -offset, regtype, regnr);
		    else
		       code_line ("\tadd\t%%fp,%d,%%%s%d", offset, regtype, regnr);
		  }; break;
	       default: dcg_bad_tag (opd -> tag, "code_load_address");
	     };
	};

static void code_store_operand (oprd opd, string dd, string regtype, int regnr)
	{ switch (opd -> tag)
	     { case TAGVar:
		  { var obj = im_vars -> array[opd -> Var.vnr]; 
		    int offset = obj -> offset;
		    if (obj -> vglob)
		       { code_line ("\tsethi\t%%hi(G%d),%%l7", opd -> Var.vnr);
			 code_line ("\tst%s\t%%%s%d,[%%l7+%%lo(G%d)]", dd,
				    regtype, regnr, opd -> Var.vnr);
		       }
		    else if (obj -> vref)
		       { /* reference argument, can't have too large offset */
			 code_line ("\tld\t[%%fp+%d],%%l7", offset);
			 code_line ("\tst%s\t%%%s%d,[%%l7]", dd, regtype, regnr);
		       }
		    else if (iabs (offset) >= 4096)
		       { code_load_integer_constant (-offset, "l", 7);
		         code_line ("\tsub\t%%fp,%%l7,%%l7");
			 code_line ("\tst%s\t%%%s%d,[%%l7]", dd,
				    regtype, regnr);
		       }
		    else if (offset < 0)
		       code_line ("\tst%s\t%%%s%d,[%%fp%d]",
				    dd, regtype, regnr, offset);
		    else
		       code_line ("\tst%s\t%%%s%d,[%%fp+%d]",
				    dd, regtype, regnr, offset);
		  }; break;
	       case TAGReflexive:
		  { code_line ("\tld\t[%%sp+64],%%l7");
		    code_line ("\tst%s\t%%%s%d,[%%l7]", dd, regtype, regnr);
		  };
	       case TAGNop: break;
	       case TAGIconst:
	       case TAGRconst:
	       case TAGTconst:
	       case TAGLab:
	       case TAGRts: dcg_internal_error ("code_store_operand");
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
				      8 * fld -> offset, 8 * sparc_size_from_type (ftype));
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
static void code_start (proc p, tuple t)
	{ /* Allocate space for double args, locals and save area */
	  int max_call, local_alloc;
	  if (debug)
	     dcg_hint ("      allocating stack space for main");
	  max_call = longest_call (p);
	  local_alloc = allocate_arguments (p -> args);	/* argc, argv */
	  if (debug && local_alloc)
	     dcg_hint ("      allocated %d bytes for arguments", local_alloc * 4);
	  local_alloc = allocate_locals (p -> locals, local_alloc);
	  if (debug)
	     dcg_hint ("      allocated %d bytes for locals", local_alloc * 4);
	  local_alloc += 17 + max_call;			/* 16 + 1 + max */
	  if (local_alloc & 1) local_alloc++;		/* align double */
	  if (debug)
	     dcg_hint ("      allocated %d bytes for stack frame", local_alloc * 4);

	  /* Code program header */
	  code_global_variables (p -> locals);
	  code_line ("\t.text");
	  code_line ("\t.align\t4");
	  code_string ("\t.globl\t");
	  code_proc_label ("main");
	  code_newline ();
	  code_string ("\t.type\t");
	  code_proc_label ("main");
	  code_line (",#function");
	  if (generate_stabs)
	     { /* probably also ok for SunOS */
	       code_string ("\t.stabs\t\"_main:F1\",36,0,0,");
	       code_proc_label ("main");
	       code_line ("\n\t.stabs\t\"argc:p1\",160,0,0,68");
	       code_line ("\t.stabs\t\"argv:p6\",160,0,0,72");
	     };

	  /* Code program entry and call to initialize rts */
	  code_proc_label ("main");
	  code_string (":");
	  if (local_alloc * 4 >= 4096)
	     { code_load_integer_constant (-local_alloc * 4, "l", 7);
	       code_line ("\tsave\t%%sp,%%l7,%%sp");
	     }
	  else code_line ("\tsave\t%%sp,-%d,%%sp", local_alloc * 4);
	  if (generate_stabs)
	     { code_line ("\tst\t%%i0,[%%fp+68]");	/* argc @ 68 */
	       code_line ("\tst\t%%i1,[%%fp+72]");	/* argv @ 72 */
	     };
	  code_line ("\tor\t%%i0,%%g0,%%o0");		/* copy argc and argv */
	  code_line ("\tor\t%%i1,%%g0,%%o1");
	  code_string ("\tcall\t");
	  code_proc_label ("rts_init");
	  code_line ("\n\tnop");			/* delay slot */
	};

/*
   Code program end
*/
static void code_end ()
	{ int lnr = new_local_label ();
	  code_string ("\tcall\t");
	  code_proc_label ("rts_finish");
	  code_line ("\n\tnop");			/* delay slot */
	  code_line ("\tor\t%%g0,%%g0,%%o0");
	  code_line ("\tret");
	  code_line ("\trestore");			/* delay slot */
	  code_string (".Lfe%d:\t.size\t", lnr);
	  code_proc_label ("main");
	  code_string (",.Lfe%d-", lnr);
	  code_proc_label ("main");
	  code_newline ();
	};

/*
   Code procedure entry
*/
static void code_store_double_argument_part (int ir_nr, int offset, int part)
	{ if (ir_nr < 6)
	     code_line ("\tst\t%%i%d,[%%fp%d]", ir_nr, offset + 4 * part);
	  else
	     { code_line ("\tld\t[%%fp+%d],%%l7", 68 + 4 * ir_nr);
	       code_line ("\tst\t%%l7,[%%fp%d]", ir_nr, offset + 4 * part);
	     };
	};

static void code_store_arguments (decl_list decls)
	{ int ir_nr, ix;
	  for (ix = 0, ir_nr = 0; (ix < decls -> size); ix++)
	     { decl arg = decls -> array[ix];
	       int vnr = arg -> Formal.vnr;
	       var opd = im_vars -> array[vnr];
	       int offset = opd -> offset;
	       if (!opd -> vref && (opd -> vsize == 8))
		  { /* double argument in ir_nr and ir_nr + 1 */
		    /* double arguments are allocated first, so their offset is low */
		    code_store_double_argument_part (ir_nr, offset, 0);
		    code_store_double_argument_part (ir_nr + 1, offset, 1);
		    ir_nr += 2;
		  }
	       else if (ir_nr < 6)
		  { /* single word argument in input reg */
		    code_line ("\tst\t%%i%d,[%%fp+%d]", ir_nr, offset);
		    ir_nr++;
		  }
	       else ir_nr++;
	     };
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

static void code_enter (proc p, tuple t)
	{ /* Pick up procedure */
	  int pnr, local_alloc, max_call, ix;
	  if (t -> op1 -> tag != TAGProc)
	     dcg_bad_tag (t -> op1 -> tag, "code_enter");
	  pnr = t -> op1 -> Proc.pnr;

	  /* Allocate space for double args, locals and save area */
	  if (debug)
	     dcg_hint ("      allocating stack space for P%d", pnr);
	  max_call = longest_call (p);
	  local_alloc = allocate_arguments (p -> args);
	  if (debug && local_alloc)
	     dcg_hint ("      allocated %d bytes for arguments", local_alloc * 4);
	  local_alloc = allocate_locals (p -> locals, local_alloc);
	  if (debug)
	     dcg_hint ("      allocated %d bytes for locals", local_alloc * 4);
	  local_alloc += 17 + max_call;			/* 16 + 1 + max */
	  if (local_alloc & 1) local_alloc++;
	  if (debug)
	     dcg_hint ("      allocated %d bytes for stack frame", local_alloc * 4);

	  /* Code procedure header for gdb */
	  code_line ("\t.align\t4");
	  code_line ("\t.type\tP%d,#function", pnr);
	  if (generate_stabs)
	     { type rtype = p -> rtyp;
	       type_info rinfo = type_table -> array[rtype -> tnr];
	       code_line ("\t.stabs\t\"%s:f%d\",36,0,0,P%d",
			  p -> pname, rinfo -> stabs_nr, pnr);
	       for (ix = 0; ix < p -> args -> size; ix++)
		  code_parameter_stabs_entry (p -> args -> array[ix], ix + 1);
	     };
	  code_string ("P%d:", pnr);
	  if (local_alloc * 4 >= 4096)
	     { code_load_integer_constant (-local_alloc * 4, "l", 7);
	       code_line ("\tsave\t%%sp,%%l7,%%sp");
	     }
	  else code_line ("\tsave\t%%sp,-%d,%%sp", local_alloc * 4);

	  /* argument storing */
	  code_store_arguments (p -> args);
	};

/*
   Code procedure leave
*/
static void code_leave (tuple t)
	{ /* Pick up procedure number */
	  int lnr = new_local_label ();
	  type rtype;
	  int pnr;

	  if (t -> op1 -> tag != TAGProc)
	     dcg_bad_tag (t -> op1 -> tag, "code_leave");
	  pnr = t -> op1 -> Proc.pnr;
	  rtype = im_procs -> array[pnr] -> rtyp;

	  /* return value loading */
	  if (is_a_floating_type (rtype))
	     code_load_operand (t -> op2, "d", "f", 0);
	  else if (t -> op2 -> tag != TAGNop)
	     code_load_operand (t -> op2, "", "i", 0);

	  /* Code procedure exit and function size for gdb */
	  code_line ("\tret");
	  code_line ("\trestore");
	  code_line (".Lfe%d:\t.size\tP%d,.Lfe%d-P%d", lnr, pnr, lnr, pnr);
	};

/*
   Coding routines to support the C calling conventions
*/

/*
   Prepare call: No code on sparc
   [ pcall, nr_args, --, -- ]
*/
static void code_prepare_call (tuple t)
	{
	};

/*
   Store a single (32 bit) reg into a parameter
*/
static void code_store_single_parameter (string reg, int argno)
	{ if (argno < 6)
	     code_line ("\tor\t%%%s,%%g0,%%o%d", reg, argno);
	  else code_line ("\tst\t%%%s,[%%sp+%d]", reg, 68 + 6 * argno);
	};

/*
   Code the storing of a function call result
   Note if dest is a Nop, nothing gets coded
*/
static void code_store_result (oprd dest)
	{ /* Check the type of destination */
	  type desttype = type_from_operand (dest);
	  if (is_a_floating_type (desttype))		/* if double result */
	     code_store_operand (dest, "d", "f", 0);	/* store %f0 */
	  else code_store_operand (dest, "", "o", 0);	/* else store %o0 */
	};

/*
   Code the loading of a call parameter
   Note if the operand is a Nop, nothing gets coded
*/
static void code_load_parameter (oprd opd, int argno)
	{ type oprdtype = type_from_operand (opd);
	  if (opd -> tag == TAGNop) return;
	  else if (is_a_floating_type (oprdtype))
	     { code_load_operand (opd, "d", "l", 0);
	       code_store_single_parameter ("l0", argno);
	       code_store_single_parameter ("l1", argno + 1);
	     }
	  else if (argno < 6)	/* local optimization */
	     code_load_operand (opd, "", "o", argno);
	  else
	     { code_load_operand (opd, "", "l", 0);
	       code_store_single_parameter ("l0", argno);
	     };
	};

/*
   Code load parameter
   [ lpar, parm, argno, -- ]
*/
static void code_tuple_load_parameter (tuple t)
	{ int argno;
	  if (t -> op2 -> tag != TAGIconst)
	     dcg_internal_error ("code_load_parameter");
	  argno = t -> op2 -> Iconst.ival;
	  code_load_parameter (t -> op1, argno);
	};

/*
   Code call to runtime system
*/
static void code_rts_call (tuple t, string rts_name, int loads, int store)
	{ /* Check if we need two or one reg for operand 1 */
	  int argno2 = 1;
	  if ((t -> op1 -> tag != TAGNop) &&
	      is_a_floating_type (type_from_operand (t -> op1))) argno2 = 2;

	  /* Load the operands into the o regs */
	  code_load_parameter (t -> op2, argno2);
	  code_load_parameter (t -> op1, 0);

	  /* Code the call */
	  code_string ("\tcall\t");
	  code_proc_label (rts_name);
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_result (t -> dst);
	};

/*
   Code load address of parameter
   [ lpar&, parm, argno, --]
*/
static void code_load_address_parameter (tuple t)
	{ int argno;
	  if (t -> op2 -> tag != TAGIconst)
	     dcg_internal_error ("code_load_address_parameter");
	  argno = t -> op2 -> Iconst.ival;

	  if (argno < 6)
	     code_load_address (t -> op1, "o", argno);
	  else
	     { code_load_address (t -> op1, "l", 0);
	       code_store_single_parameter ("l0", argno);
	     };
	};

/*
   Code call and store result
   [ call, proc, --, dst ]
*/
static void code_actual_call (oprd opd)
	{ if (opd -> tag == TAGVar)
	     { code_load_operand (opd, "", "l", 0);
	       code_line ("\tcall\t%%l0,0");
	     }
	  else
	     { code_string ("\tcall\t");
	       code_called_proc (opd);
	     };
	  code_line ("\n\tnop");			/* delay slot */
	};

static void code_call (tuple t)
	{ if (t -> dst -> tag == TAGReflexive)
	     code_line ("\tst\t%%o0,[%%sp+64]\n");
	  code_actual_call (t -> op1);
	  code_store_result (t -> dst);
	};

/*
   Code jump
   [ jmp, --, --, dst ]
*/
static void code_jump (tuple t)
	{ code_string ("\tba\t");
	  code_jump_target (t -> dst);
	  code_line ("\n\tnop");			/* delay slot */
	};

/*
   Code jump if nil
   [ jmp_if_nil, oprd, --, dst ]
*/
static void code_jump_if_nil (tuple t)
        { code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\tsubcc\t%%l0,1,%%g0");
	  code_string ("\tbe\t");
	  code_jump_target (t -> dst);
	  code_line ("\n\tnop");			/* delay slot */
	};

/*
   Code jump if null
   [ jmp_if_null, oprd, --, dst ]
*/
static void code_jump_if_null (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\torcc\t%%l0,%%g0,%%l0");
	  code_string ("\tbe\t");
	  code_jump_target (t -> dst);
	  code_line ("\n\tnop");			/* delay slot */
	};

/*
   Code jump if condition
   [ jump_if_int_less_than_int, oprd1, oprd2, dst ]
*/
static void code_cond_jump (tuple t, string opc)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_load_operand (t -> op2, "", "l", 1);
	  code_line ("\tsubcc\t%%l0,%%l1,%%g0");
	  code_string ("\tb%s\t", opc);
	  code_jump_target (t -> dst);
	  code_line ("\n\tnop");			/* delay slot */
	};

/*
   Coding routines for garbage collection
*/

/* [ undefine, --, --, dest ] */
static void code_undefine (tuple t)
	{ code_store_operand (t -> dst, "", "g", 0);
	};

/*
   Allocate space for row/struct
   [ allocate, space in bytes, --, dest ]
*/
static void code_allocate (tuple t)
	{ /* call rts_malloc and store result */
	  code_load_operand (t -> op1, "", "o", 0);
	  code_string ("\tcall\t");
	  code_proc_label ("rts_malloc");
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_operand (t -> dst, "", "o", 0);
	};

/*
   Set guard on var object and possibly create space for it
   [ guard, oprd, space in bytes, dest ]
*/
static void code_guard (tuple t)
	{ /* load size and oprd */
	  code_load_operand (t -> op1, "", "o", 0);
	  code_load_operand (t -> op2, "", "o", 1);
	  
	  /* call rts_guard and store result */
	  code_string ("\tcall\t");
	  code_proc_label ("rts_guard");
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_operand (t -> dst, "", "o", 0);
	};

/*
   Attach object
   [ att&, oprd, --, dest/--/reflexive ]
*/
static void code_attach_object (tuple t)
	{ code_load_address (t -> op1, "o", 0);		/* load arg in o0 */
	  if (t -> dst -> tag == TAGReflexive)
	     code_line ("\tst\t%%o0,[%%sp+64]\n");
	  code_string ("\tcall\t");
	  code_proc_label ("rts_attach");
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_operand (t -> dst, "", "o", 0);
	};

/*
   Attach object pointed to by oprd + (byte) offset
   [ att_off, addr, offset, dest/--/reflexive ]
*/
static void code_attach_offset (tuple t)
	{ code_load_operand (t -> op1, "", "o", 0);
	  switch (t -> op2 -> tag)
	     { case TAGNop: break;
	       case TAGIconst:
		  { int fixed_offset = t -> op2 -> Iconst.ival;
		    if (!fixed_offset) break;
		    if ((0 < fixed_offset) && (fixed_offset < 4096))
		       { code_line ("\tadd\t%%o0,%d,%%o0", fixed_offset);
			 break;
		       };
		  };
	       default:
		  { code_load_operand (t -> op2, "", "l", 0);
		    code_line ("\tadd\t%%l0,%%o0,%%o0");
		  };
	     };	    
	  if (t -> dst -> tag == TAGReflexive)
	     code_line ("\tst\t%%o0,[%%sp+64]\n");
	  code_string ("\tcall\t");
	  code_proc_label ("rts_attach");
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_operand (t -> dst, "", "o", 0);
	};

/*
   Detach variable. Note that rts_detach is called by reference
   [ det&, oprd, --, -- ]
*/
static void code_detach_object (tuple t)
	{ code_load_address (t -> op1, "o", 0);		/* load arg in o0 */
	  code_string ("\tcall\t");
	  code_proc_label ("rts_detach");
	  code_line ("\n\tnop");			/* branch slot */
	};

/*
   Attach object pointed to by oprd + (byte) offset
   [ det_off, addr, offset, -- ]
*/
static void code_detach_offset (tuple t)
	{ code_load_operand (t -> op1, "", "o", 0);
	  switch (t -> op2 -> tag)
	     { case TAGNop: break;
	       case TAGIconst:
		  { int fixed_offset = t -> op2 -> Iconst.ival;
		    if (!fixed_offset) break;
		    if ((0 < fixed_offset) && (fixed_offset < 4096))
		       { code_line ("\tadd\t%%o0,%d,%%o0", fixed_offset);
			 break;
		       };
		  };
	       default:
		  { code_load_operand (t -> op2, "", "l", 0);
		    code_line ("\tadd\t%%l0,%%o0,%%o0");
		  };
	     };	    
	  code_string ("\tcall\t");
	  code_proc_label ("rts_detach");
	  code_line ("\n\tnop");			/* branch slot */
	};

/*
   Predetach object. If the value returned by predetach is not NULL,
   the object contains subfields that should be detached first
   [ predet&, oprd, --, dest ]
*/
static void code_predetach_object (tuple t)
	{ code_load_address (t -> op1, "o", 0);		/* load arg in o0 */
	  code_string ("\tcall\t");
	  code_proc_label ("rts_predetach");
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_operand (t -> dst, "", "o", 0);
	};

/*
   Rts support actions
*/

/*
   Code test for nil
   [ is_nil, oprd, --, dest]
*/
static void code_is_nil (tuple t)
	{ int label = new_local_label ();
	  code_line ("\tor\t%%g0,%%g0,%%l1");
	  code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\tsubcc\t%%l0,1,%%l0");
	  code_line ("\tbe,a\tLL%d", label);
	  code_line ("\tor\t%%g0,1,%%l1");
	  code_string ("LL%d:", label);
	  code_store_operand (t -> dst, "", "l", 1);
	};

/*
   Code to add an address and an offset which will be scaled by 4
   [ A+I, adr_oprd, int_oprd, dest ]
*/
static void code_addr_plus_offset (tuple t)
	{ code_load_operand (t -> op1, "", "l", 1);
	  code_line ("\torcc\t%%g0,%%l1,%%g0");
	  code_string ("\tbe\t");
	  code_proc_label ("rts_offset_from_0");
	  code_line ("\n\tnop");			/* branch slot */
	  switch (t -> op2 -> tag)
	     { case TAGNop: break;
	       case TAGIconst:
		  { int fixed_offset = t -> op2 -> Iconst.ival;
		    if (!fixed_offset) break;
		    if ((0 < fixed_offset) && (fixed_offset < 4096))
		       { code_line ("\tadd\t%%l1,%d,%%l1", fixed_offset);
			 break;
		       };
		  };
	       default:
		  { code_load_operand (t -> op2, "", "l", 0);
		    code_line ("\tadd\t%%l1,%%l0,%%l1");
		  };
	     };	    
	  code_store_operand (t -> dst, "", "l", 1);
	};

/*
   Code to adjust the row index and check its bounds
   [ row_idx, oprd, upb, dest ]
*/
static void code_row_index (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\tsubcc\t%%l0,1,%%l0");		/* rows start at 1 */
	  code_string ("\tbneg\t");
	  code_proc_label ("rts_index_out_of_range");
	  code_line ("\n\tnop");			/* branch slot */
	  if ((t -> op2 -> tag == TAGIconst) &&
	      (-4096 <= t -> op2 -> Iconst.ival) &&
	      (t -> op2 -> Iconst.ival < 4096))
	     code_line ("\tsubcc\t%%l0,%d,%%g0", t -> op2 -> Iconst.ival);
	  else
	     { code_load_operand (t -> op2, "", "l", 1);
	       code_line ("\tsubcc\t%%l0,%%l1,%%g0");
	     }
	  code_string ("\tbge\t");
	  code_proc_label ("rts_index_out_of_range");
	  code_line ("\n\tnop");			/* branch slot */
	  code_store_operand (t -> dst, "", "l", 0);
	};

/*
   Coding of copy actions
*/

/*
   Copy address of
   [ :=&, oprd, --, dst ]
*/
static void code_copy_adr_of (tuple t)
	{ code_load_address (t -> op1, "l", 0);		/* load address */
	  code_store_operand (t -> dst, "", "l", 0);	/* store it away */
	};

static void code_copy (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);	/* load operand */
	  code_store_operand (t -> dst, "", "l", 0);	/* store it away */
	};

static void code_copy_indirect (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);	/* load operand */
	  code_load_operand (t -> dst, "", "l", 1);
	  code_line ("\tst\t%%l0,[%%l1]");		/* store it away */
	};

static void code_indirect_copy (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\tld\t[%%l0],%%l0");		/* load operand */
	  code_store_operand (t -> dst, "", "l", 0);	/* store it away */
	};

static void code_copy_real (tuple t)
	{ code_load_operand (t -> op1, "d", "f", 0);	/* load operand */
	  code_store_operand (t -> dst, "d", "f", 0);	/* store it away */
	};

static void code_copy_real_indirect (tuple t)
	{ code_load_operand (t -> op1, "d", "f", 0);	/* load operand */
	  code_load_operand (t -> dst, "", "l", 1);
	  code_line ("\tstd\t%%f0,[%%l1]");		/* store it away */
	};

static void code_copy_indirect_real (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\tldd\t[%%l0],%%f0");		/* load operand */
	  code_store_operand (t -> dst, "d", "f", 0);	/* store it away */
	};

static void code_int_op (tuple t, string opc)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_load_operand (t -> op2, "", "l", 1);
	  code_line ("\t%s\t%%l0,%%l1,%%l2", opc);
	  code_store_operand (t -> dst, "", "l", 2);
	};

static void code_int_div (tuple t, int divmod)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_load_operand (t -> op2, "", "l", 1);
	  code_line ("\tsra\t%%l0,31,%%l2");
	  code_line ("\twr\t%%g0,%%l2,%%y");
	  code_line ("\tsdiv\t%%l0,%%l1,%%l2");
	  if (divmod)
	     { code_line ("\tsmul\t%%l1,%%l2,%%l2");
	       code_line ("\tsub\t%%l0,%%l2,%%l2");
	     };
	  code_store_operand (t -> dst, "", "l", 2);
	};
	  
static void code_real_op (tuple t, string opc)
	{ code_load_operand (t -> op1, "d", "f", 0);
	  code_load_operand (t -> op2, "d", "f", 2);
	  code_line ("\t%s\t%%f0,%%f2,%%f4", opc);
	  code_store_operand (t -> dst, "d", "f", 4);
	};

static void code_negate (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\tsub\t%%g0,%%l0,%%l0");
	  code_store_operand (t -> dst, "", "l", 0);
	};

static void code_int_to_real (tuple t)
	{ if (t -> op1 -> tag == TAGIconst)
	     code_load_integer_constant_to_float (t -> op1 -> Iconst.ival, 0);
	  else code_load_operand (t -> op1, "", "f", 0);
	  code_line ("\tfitod\t%%f0,%%f0");
	  code_store_operand (t -> dst, "d", "f", 0);
	};

static void code_negate_real (tuple t)
	{ code_load_operand (t -> op1, "d", "f", 0);
	  code_line ("\tfnegs\t%%f0,%%f0");
	  code_store_operand (t -> dst, "d", "f", 0);
	};

static void code_int_cmp (tuple t, string opc)
	{ int label = new_local_label ();
	  code_load_operand (t -> op1, "", "l", 0);
	  code_load_operand (t -> op2, "", "l", 1);
	  code_line ("\tor\t%%g0,%%g0,%%l2");
	  code_line ("\tsubcc\t%%l0,%%l1,%%g0");
	  code_line ("\t%s,a\tLL%d", opc, label);
	  code_line ("\tor\t%%g0,1,%%l2");
	  code_string ("LL%d:", label);
	  code_store_operand (t -> dst, "", "l", 2);
	};

static void code_real_cmp (tuple t, string opc)
	{ int label = new_local_label ();
	  code_load_operand (t -> op1, "d", "f", 0);
	  code_load_operand (t -> op2, "d", "f", 2);
	  code_line ("\tor\t%%g0,%%g0,%%l2");
	  code_line ("\tfcmped\t%%f0,%%f2");
	  code_line ("\tnop");				/* transfer delay slot */
	  code_line ("\t%s,a\tLL%d", opc, label);
	  code_line ("\tor\t%%g0,1,%%l2");
	  code_string ("LL%d:", label);
	  code_store_operand (t -> dst, "", "l", 2);
	};

static void code_not (tuple t)
	{ code_load_operand (t -> op1, "", "l", 0);
	  code_line ("\txnor\t%%l0,%%g0,%%l2");
	  code_line ("\tand\t%%l2,1,%%l2");
	  code_store_operand (t -> dst, "", "l", 2);
	};

static void code_sline (proc p, tuple t)
	{ int lnr;
	  if (!generate_stabs) return;
	  if (t -> op1 -> tag != TAGIconst)
	     dcg_internal_error ("code_sline");
	  lnr = new_local_label ();
	  if (old_os)
	     code_line ("\t.stabn\t68,0,%d,Lline%d", t -> op1 -> Iconst.ival, lnr, p -> pnr);
	  else if (p -> pnr)
	     code_line ("\t.stabn\t68,0,%d,.Lline%d-P%d", t -> op1 -> Iconst.ival, lnr, p -> pnr);
	  else code_line ("\t.stabn\t68,0,%d,.Lline%d-main", t -> op1 -> Iconst.ival, lnr);
	  code_line ("%sLline%d:", (old_os)?"":".", lnr);
	};

/*
   The tuple dispatcher
*/
static void code_tuple (proc p, tuple t)
	{ switch (t -> opc)
	     { /* General */
	       case start_program:	     code_start (p, t); break;
	       case end_program:	     code_end (); break;

	       /* Procedure calls */
	       case prepare_call:	     code_prepare_call (t); break;
	       case load_parameter:	     code_tuple_load_parameter (t); break;
	       case load_address_parameter:  code_load_address_parameter (t); break;
	       case call_procedure:
	       case call_attach_procedure:
	       case call_detach_procedure:
	       case call_guard_procedure:    code_call (t); break;

	       /* Procedure entry and return */
	       case enter_procedure:	     code_enter (p, t); break;
	       case leave_procedure:	     code_leave (t); break;

	       /* Jumps */
	       case jump: 		     code_jump (t); break;
	       case jump_if_nil:	     code_jump_if_nil (t); break;
	       case jump_if_null:	     code_jump_if_null (t); break;
	       case jump_if_addr_equal_addr:	   code_cond_jump (t, "e"); break;
	       case jump_if_int_equal_int:	   code_cond_jump (t, "e"); break;
	       case jump_if_int_notequal_int:	   code_cond_jump (t, "ne"); break;
	       case jump_if_int_less_than_int: 	   code_cond_jump (t, "l"); break;
	       case jump_if_int_less_equal_int:    code_cond_jump (t, "le"); break;
	       case jump_if_int_greater_than_int:  code_cond_jump (t, "g"); break;
	       case jump_if_int_greater_equal_int: code_cond_jump (t, "ge"); break;

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
	       case copy_int:
	       case copy_adr:		     code_copy (t); break;
	       case copy_int_indirect:
	       case copy_adr_indirect:       code_copy_indirect (t); break;
	       case copy_indirect_int:
	       case copy_indirect_adr:	     code_indirect_copy (t); break;
	       case copy_real:		     code_copy_real (t); break;
	       case copy_real_indirect:	     code_copy_real_indirect (t); break;
	       case copy_indirect_real:	     code_copy_indirect_real (t); break;

	       /* Index op */
	       case is_nil:		     code_is_nil (t); break;
	       case addr_plus_offset:	     code_addr_plus_offset (t); break;
	       case row_index:		     code_row_index (t); break;

	       /* Integer ops */
	       case int_plus_int:	     code_int_op (t, "add"); break;
	       case int_minus_int:	     code_int_op (t, "sub"); break;
	       case int_times_int:	     code_int_op (t, "smul"); break;
	       case int_div_int:	     code_int_div (t, 0); break;
	       case int_mod_int:	     code_int_div (t, 1); break;
	       case int_pow_int:	     code_rts_call (t, "rts_int_pow_int", 2, 1); break;
	       case negate_int:		     code_negate (t); break;
	       case int_equal_int: 	     code_int_cmp (t, "be"); break;
	       case int_notequal_int: 	     code_int_cmp (t, "bne"); break;
	       case int_less_than_int: 	     code_int_cmp (t, "bl"); break;
	       case int_less_equal_int:      code_int_cmp (t, "ble"); break;
	       case int_greater_than_int:    code_int_cmp (t, "bg"); break;
	       case int_greater_equal_int:   code_int_cmp (t, "bge"); break;

	       /* Real ops */
	       case real_plus_real:	     code_real_op (t, "faddd"); break;
	       case real_minus_real:	     code_real_op (t, "fsubd"); break;
	       case real_times_real:	     code_real_op (t, "fmuld"); break;
	       case real_divide_real:	     code_real_op (t, "fdivd"); break;
	       case real_pow_int:	     code_rts_call (t, "rts_real_pow_int", 2, 1); break;
	       case negate_real:	     code_negate_real (t); break;
	       case real_equal_real: 	     code_real_cmp (t, "fbe"); break;
	       case real_notequal_real:      code_real_cmp (t, "fbne"); break;
	       case real_less_than_real:     code_real_cmp (t, "fbl"); break;
	       case real_less_equal_real:    code_real_cmp (t, "fble"); break;
	       case real_greater_than_real:  code_real_cmp (t, "fbg"); break;
	       case real_greater_equal_real: code_real_cmp (t, "fbge"); break;

	       /* Bool ops */
	       case bool_and_bool: 	     code_int_op (t, "and"); break;
	       case bool_or_bool: 	     code_int_op (t, "or"); break;
	       case bool_xor_bool: 	     code_int_op (t, "xor"); break;
	       case not_bool: 		     code_not (t); break;

	       /* Text ops */
	       case text_equal_text:	   code_rts_call (t, "rts_text_equal_text", 2, 1); break;
	       case text_notequal_text:	   code_rts_call (t, "rts_text_notequal_text", 2, 1); break;
	       case text_less_than_text:  
		  code_rts_call (t, "rts_text_less_than_text", 2, 1); break;
	       case text_less_equal_text:
		  code_rts_call (t, "rts_text_less_equal_text", 2, 1); break;
	       case text_greater_than_text:
		  code_rts_call (t, "rts_text_greater_than_text", 2, 1); break;
	       case text_greater_equal_text:
		  code_rts_call (t, "rts_text_greater_equal_text", 2, 1); break;

	       /* Conversions */
	       case int_to_real:	     code_int_to_real (t); break;

	       /* Miscellaneous */
	       case sline:		     code_sline (p, t);
	       case comment:
	       case nop:		     break;
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

void sparc_code_generation ()
	{ int ix;
	  dcg_hint ("      generating sparc machine code...");
	  code_intro ();
	  for (ix = 0; ix < im_procs -> size; ix++)  
	     code_procedure (im_procs -> array[ix]);
	};
