/*
   File: imc_utils.c
   Defines general utilities for the intermediate code
   as well as a number of global variables which are used
   during intermediate code generation

   CVS ID: "$Id: imc_utils.c,v 1.13 2011/09/02 12:50:29 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "elan_imc.h"
#include "type_table.h"
#include "imc_utils.h"

/* exported variables for codegenerations, if needed.... */
tuple_list im_labels;
proc_list im_procs;
var_list im_vars;

/* private administration */
static block_list all_blocks;
static tuple_list all_tuples;
static tuple free_tuples;
static tuple last_tuple, next_tuple;
static string no_oprd_repr;
static string reflexive_repr;
static oprd no_oprd_atom;
static oprd reflexive_atom;
static int vnr_lw_mark;
static int nr_temps;

static tuple new_clean_tuple ()
{ /* allocate from free list or new */
  tuple new;
  if (free_tuples != tuple_nil)
     { new = free_tuples;
       free_tuples = next_tuple -> next;
     }
  else
     { new = new_tuple (nop, oprd_nil, oprd_nil, oprd_nil);
       app_tuple_list (all_tuples, new);
     };

  /* fill in fields as in self ref */
  new -> opc = nop;
  new -> op1 = oprd_nil;
  new -> op2 = oprd_nil;
  new -> dst = oprd_nil;
  new -> ad = 0;
  new -> next = new;
  new -> prev = new;

  return (new);
};

/* Note that var i != $ti */
void init_imc ()
{ im_labels = new_tuple_list ();
  app_tuple_list (im_labels, tuple_nil);	/* L0 is not used */
  im_procs = new_proc_list ();
  im_vars = new_var_list ();
  app_var_list (im_vars, var_nil);		/* var0 is not used */
  free_tuples = tuple_nil;
  all_tuples = new_tuple_list ();
  all_blocks = new_block_list ();
  app_block_list (all_blocks, block_nil);	/* BB0 is not used */
  no_oprd_repr = new_string ("--");
  no_oprd_atom = new_Nop ();
  reflexive_repr = new_string ("(sp)");
  reflexive_atom = new_Reflexive ();
  nr_temps = 0;
  next_tuple = new_clean_tuple ();
  last_tuple = tuple_nil;
};

oprd no_oprd ()
{ return (attach_oprd (no_oprd_atom));
};

oprd reflexive ()
{ return (attach_oprd (reflexive_atom));
};

/*
   Proc administration
*/
int allocate_proc (string pname, string cname, type rtyp, decl_list args, body cbody)
{ proc p = new_proc (pname, cname, rtyp, args, cbody);
  int pnr = im_procs -> size;
  app_proc_list (im_procs, p);
  p -> pnr = pnr;
  return (pnr);
};

#define PERMANENT_CALL_COUNT 1000000
void increment_call_count (int pnr)
{ int *cptr;
  if ((pnr < 0) || (pnr > im_procs -> size))
     dcg_internal_error ("increment_call_count");
  cptr = &im_procs -> array[pnr] -> count;
  if (*cptr == PERMANENT_CALL_COUNT) return;
  *cptr += 1;
};

void set_permanent_call_count (int pnr)
{ if ((pnr < 0) || (pnr > im_procs -> size))
     dcg_internal_error ("increment_call_count");
  im_procs -> array[pnr] -> count = PERMANENT_CALL_COUNT;
};

/*
   Variable and argument administration
*/
int allocate_variable (string vname, type vtype, int global, int vref)
{ var obj = new_var (attach_string (vname), attach_type (vtype), global, vref, 1);
  int vnr = im_vars -> size;
  app_var_list (im_vars, obj);
  return (vnr);
};

void set_vnr_lw_mark ()
{ vnr_lw_mark = im_vars -> size;
}

void release_variable (int vnr)
{ if ((vnr < vnr_lw_mark) || (vnr >= im_vars -> size))
    dcg_internal_error ("release_variable");
  im_vars -> array[vnr] -> vused = 0;
}

int create_variable (decl id, int global)
{ int vnr = allocate_variable (id -> name, id -> typ, global, 0);
  if (id -> tag != TAGObject_decl)
    dcg_internal_error ("create_global_variable");

  id -> Object_decl.vnr = vnr;
  return (vnr);
}

int pick_variable (decl id)
{ int vnr = 0;
  switch (id -> tag)
    { case TAGObject_decl: vnr = id -> Object_decl.vnr; break;
      case TAGFormal: vnr = id -> Formal.vnr; break;
      default: dcg_bad_tag (id -> tag, "pick_variable");
    };
  if (!vnr) dcg_internal_error ("pick_variable");
  return (vnr);
}

int create_argument (string name, type t, int vref)
{ int vnr = allocate_variable (name, t, 0, vref);
  return (vnr);
}

void create_uproc_args (decl_list dl)
{ int ix;
  for (ix = 0; ix < dl -> size; ix++)
    { decl d = dl -> array[ix];
      d -> Formal.vnr = create_argument (d -> name, d -> typ, (d -> Formal.acc == acc_var));
    };
}

int allocate_temporary (type t)
{ string tname;
  int vnr;
  for (vnr = vnr_lw_mark; vnr < im_vars -> size; vnr++)
    { var v = im_vars -> array[vnr];
      if (v -> vused) continue;
      if (equal_type (v -> vtype, t))
	{ dcg_hint ("      reusing %s", v -> vname);
	  v -> vused = 1;
	  return (vnr);
	};
    };
  tname = dcg_new_fmtd_string ("$t%d", nr_temps++);
  vnr = allocate_variable (tname, t, 0, 0);
  detach_string (&tname);
  return (vnr);
}

/*
   Label administration
*/
int new_label ()
{ int new = im_labels -> size;
  app_tuple_list (im_labels, tuple_nil);
  return (new);
}

void set_label (int label)
{ im_labels -> array[label] = next_tuple;
}

/*
   Tuple administration
   Note that we always maintain a next tuple, so that we may
   pick up references to it (see set_label) before generating it

   Furthermore all tuples belonging to the main program or one
   procedure are tied together as a doubly linked list.
*/
tuple get_next_location ()
{ return (next_tuple);
}

static tuple enter_tuple (opcode opc, oprd op1, oprd op2, oprd dst)
{ /* copy next in current, alloc next */
  last_tuple = next_tuple;
  next_tuple = new_clean_tuple ();

  /* fill in fields as in self ref */
  last_tuple -> opc = opc;
  last_tuple -> op1 = op1;
  last_tuple -> op2 = op2;
  last_tuple -> dst = dst;
  last_tuple -> ad = 0;
  last_tuple -> next = last_tuple;
  last_tuple -> prev = last_tuple;

  return (last_tuple);
}

/*
   Generate the first tuple for the main program or a procedure
   and hook it up as the anchor of the proc structure.
   Note that opc can only be start_program or enter_procedure
*/
void gen_start_tuple (proc p, opcode opc, int proc_idx)
{ p -> anchor = enter_tuple (opc, new_Proc (proc_idx), no_oprd (), no_oprd ());
}

void gen_tuple (opcode opc, oprd op1, oprd op2, oprd dst)
{ tuple anchor = last_tuple;
  tuple new = enter_tuple (opc, op1, op2, dst);
  if (anchor == tuple_nil)
    dcg_internal_error ("gen_tuple");

  /* now insert after anchor */
  new -> next = anchor -> next;
  anchor -> next = new;
  new -> prev = anchor;
  new -> next -> prev = new;
};

void insert_tuple (tuple loc, opcode opc, oprd op1, oprd op2, oprd dst)
{ /* check if loc is the next to be generated */
  if (loc != next_tuple)
    { /*
	 loc has been generated earlier and contains valid code
         copy its code to the new tuple and insert it after loc
	 then overwrite loc with the new fields. By copying the
	 label administration remains intact.
      */
      tuple new = new_clean_tuple ();
      new -> opc = loc -> opc;
      new -> op1 = loc -> op1;
      new -> op2 = loc -> op2;
      new -> dst = loc -> dst;
      new -> ad = loc -> ad;
      new -> next = loc -> next;
      loc -> next = new;
      new -> prev = loc;
      new -> next -> prev = new;
      loc -> opc = opc;
      loc -> op1 = op1;
      loc -> op2 = op2;
      loc -> dst = dst;
      loc -> ad = 0;
      if (loc == last_tuple) last_tuple = new;
    }
  else gen_tuple (opc, op1, op2, dst);
}

static int is_a_jump (tuple t)
{ switch (t -> opc)
    { case jump:
      case jump_if_nil:
      case jump_if_null:
      case jump_if_addr_equal_addr:
      case jump_if_int_equal_int:
      case jump_if_int_notequal_int:
      case jump_if_int_less_than_int:
      case jump_if_int_less_equal_int:
      case jump_if_int_greater_than_int:
      case jump_if_int_greater_equal_int: return (1);
      default: return (0);
    };
}

static int is_basic_block_end (tuple t)
{ if (is_a_jump (t)) return (1);
  switch (t -> opc)
    { case end_program:
      case leave_procedure: return (1);
      default: return (0);
    };
}

void register_locals_with_proc (proc p)
{ int vnre = im_vars -> size;
  int vnrs = vnr_lw_mark;
  int ix;
  int_list locals = init_int_list (vnre - vnrs);

  for (ix = vnrs; ix < vnre; ix++) app_int_list (locals, ix);
  p -> locals = locals;
}

/*
   Basic block and flow graph construction 
*/
static void add_unique_block (block_list l, block b)
{ int ix;
  for (ix = 0; ix < l -> size; ix++)
    if (b == l -> array[ix]) return;
  app_block_list (l, b);
}

static block make_block_header (tuple t)
{ block nbl;
  int bnr;
  if (t -> ad)
    return (all_blocks -> array [t -> ad]);
  bnr = all_blocks -> size;
  nbl = new_block (bnr);
  app_block_list (all_blocks, nbl);
  nbl -> pred = new_block_list ();
  nbl -> anchor = t;
  t -> ad = bnr;
  return (nbl);
}

static void mark_block_headers (proc p)
{ tuple ptr = p -> anchor;
  p -> init_block = make_block_header (ptr);
  do
    { if (is_a_jump (ptr))
	{ oprd dst = ptr -> dst;
	  tuple jump_tgt;
	  block jump_blk;
	  int label;

	  /* Consistency check */
	  if (dst -> tag != TAGLab)
	    dcg_internal_error ("mark_block_headers");
	  label = dst -> Lab.lab;

	  /* Make the target and the successor a block header */
	  jump_tgt = im_labels -> array[label];
	  jump_blk = make_block_header (jump_tgt);
	  detach_oprd (&ptr -> dst);
	  ptr -> dst = new_Block (jump_blk -> bnr);
	  ptr = ptr -> next;
	  if (ptr != p -> anchor)
	    (void) make_block_header (ptr);
	}
      else ptr = ptr -> next;
    }
  while (ptr != p -> anchor);
}

static void collect_basic_blocks (proc p)
{ tuple hdr = p -> anchor;
  block curr = all_blocks -> array[hdr -> ad];
  tuple ptr = hdr;
  do
    { tuple succ_t = ptr -> next;
      int bbend = is_basic_block_end (ptr);
      if ((succ_t != p -> anchor) && (succ_t -> ad)) bbend = 1;
      if (bbend)
	{ tuple succ_t = ptr -> next;
	  block succ_blk;
	  curr -> anchor -> prev = ptr;
	  ptr -> next = curr -> anchor;

	  /* Check if we have a jump, if so lookup jump block */
	  if (is_a_jump (ptr))
	    { block jump_blk = all_blocks -> array[ptr -> dst -> Block.bnr];
	      curr -> jump_succ = jump_blk;
	      add_unique_block (jump_blk -> pred, curr);
	    };

	  /* Lookup successor block, if present */
	  if (succ_t == p -> anchor) return;
	  if (!succ_t -> ad)
	    dcg_internal_error ("collect_basic_blocks");
	  succ_blk = all_blocks -> array[succ_t -> ad];
	  curr -> direct_succ = succ_blk;
	  add_unique_block (succ_blk -> pred, curr);
	  curr = succ_blk;
	  ptr = succ_t;
	}
       else ptr = ptr -> next;
    }
  while (ptr != p -> anchor);
  dcg_internal_error ("collect_basic_blocks");
}

void make_basic_blocks (proc p)
{ mark_block_headers (p);
  collect_basic_blocks (p);
}

/* Access utilities */
type type_from_operand (oprd op)
{ switch (op -> tag)
     { case TAGTconst: return (t_text);
       case TAGRconst: return (t_real);
       case TAGIconst: return (t_int);
       case TAGProc: return (t_addr);
       case TAGRts: return (t_addr);
       case TAGVar:
	 { var v = im_vars -> array[op -> Var.vnr];
	   return (v -> vtype);
	 };
       case TAGLab: return (t_addr);
       case TAGBlock: return (t_addr);
       case TAGReflexive: return (t_addr);
       case TAGRts_nil: return (t_addr);
       case TAGNop: break;
       default: dcg_bad_tag (op -> tag, "type_from_operand");
     };
  return (t_void);
}

/* Conversion utilities */
string convert_opcode_text (opcode opc)
{ switch (opc)
     { /* General */
       case start_program:	     return ("start");
       case end_program:	     return ("end");

       /* Procedure calls */
       case prepare_call:	     return ("pcall");
       case load_parameter:	     return ("lpar");
       case load_address_parameter:  return ("lpar&");
       case call_procedure:	     return ("call");
       case call_attach_procedure:   return ("call_att");
       case call_detach_procedure:   return ("call_det");
       case call_guard_procedure:    return ("call_grd");
       case enter_procedure:	     return ("enter");
       case leave_procedure:	     return ("leave");

       /* Jumps */
       case jump:		     return ("jmp");
       case jump_if_nil:	     return ("jif_nil");
       case jump_if_null:	     return ("jif_null");
       case jump_if_addr_equal_addr:       return ("jif_==A");
       case jump_if_int_equal_int:         return ("jif_==I");
       case jump_if_int_notequal_int:      return ("jif_!=I");
       case jump_if_int_less_than_int:     return ("jif_<I");
       case jump_if_int_less_equal_int:    return ("jif_<=I");
       case jump_if_int_greater_than_int:  return ("jif_>I");
       case jump_if_int_greater_equal_int: return ("jif_>=I");

       /* Garbage collection */
       case undefine:		     return ("undef");
       case allocate:		     return ("allocate");
       case guard:		     return ("guard");
       case attach_adr:		     return ("att&");
       case attach_offset:	     return ("att_off");
       case detach_adr:	             return ("det&");
       case detach_offset:	     return ("det_off");
       case predetach_adr:	     return ("predet&");

       /* Copy group */
       case copy_adr_of:	     return (":=&");
       case copy_int:		     return (":=I");
       case copy_int_indirect:	     return ("*:=I");
       case copy_indirect_int:	     return (":=*I");
       case copy_adr:		     return (":=A");
       case copy_adr_indirect:	     return ("*:=A");
       case copy_indirect_adr:	     return (":=*A");
       case copy_real:		     return (":=R");
       case copy_real_indirect:	     return ("*:=R");
       case copy_indirect_real:	     return (":=*R");

       /* Rts support ops */
       case is_nil:		     return ("is_nil");
       case addr_plus_offset:	     return ("A+I");
       case row_index:		     return ("row_idx");

       /* Integer ops */
       case int_plus_int:	     return ("+I");
       case int_minus_int:	     return ("-I");
       case int_times_int:	     return ("*I");
       case int_div_int:	     return ("div");
       case int_mod_int:	     return ("mod");
       case int_pow_int:	     return ("**I");
       case negate_int:		     return ("-I");
       case int_equal_int:	     return ("==I");
       case int_notequal_int:	     return ("!=I");
       case int_less_than_int:	     return ("<I");
       case int_less_equal_int:	     return ("<=I");
       case int_greater_than_int:    return (">I");
       case int_greater_equal_int:   return (">=I");

       /* Bool ops */
       case bool_and_bool:	     return ("&&");
       case bool_or_bool:	     return ("||");
       case bool_xor_bool:	     return ("^^");
       case not_bool:		     return ("!");

       /* Real ops */
       case real_plus_real:	     return ("+R");
       case real_minus_real:	     return ("-R");
       case real_times_real:	     return ("*R");
       case real_divide_real:	     return ("/R");
       case real_pow_int:	     return ("**RI");
       case negate_real:	     return ("-R");
       case real_equal_real:	     return ("==R");
       case real_notequal_real:	     return ("!=R");
       case real_less_than_real:     return ("<R");
       case real_less_equal_real:    return ("<=R");
       case real_greater_than_real:  return (">R");
       case real_greater_equal_real: return (">=R");

       /* Text ops */
       case text_plus_text:	     return ("+T");
       case int_times_text:	     return ("*T");
       case text_equal_text:	     return ("==T");
       case text_notequal_text:	     return ("!=T");
       case text_less_than_text:     return ("<T");
       case text_less_equal_text:    return ("<=T");
       case text_greater_than_text:  return (">T");
       case text_greater_equal_text: return (">=T");

       /* Conversions */
       case int_to_real:	     return ("I2R");

       /* Miscellaneous */
       case sline:		     return ("sline");
       case comment:		     return ("comment");
       case nop:		     return ("nop");
       default: dcg_bad_tag (opc, "convert_opcode_text");
     };
  return (string_nil);
}

opcode convert_text_opcode (string s)
{ if (streq (s, "is_nil"))			 return (is_nil);
  else if (streq (s, "copy_int"))		 return (copy_int);
  else if (streq (s, "int_plus_int"))		 return (int_plus_int);
  else if (streq (s, "int_minus_int"))		 return (int_minus_int);
  else if (streq (s, "int_times_int"))		 return (int_times_int);
  else if (streq (s, "int_div_int"))		 return (int_div_int);
  else if (streq (s, "int_mod_int"))		 return (int_mod_int);
  else if (streq (s, "int_pow_int"))		 return (int_pow_int);
  else if (streq (s, "negate_int"))		 return (negate_int);
  else if (streq (s, "int_equal_int"))		 return (int_equal_int);
  else if (streq (s, "int_notequal_int"))	 return (int_notequal_int);
  else if (streq (s, "int_less_than_int"))	 return (int_less_than_int);
  else if (streq (s, "int_less_equal_int"))	 return (int_less_equal_int);
  else if (streq (s, "int_greater_than_int"))	 return (int_greater_than_int);
  else if (streq (s, "int_greater_equal_int"))	 return (int_greater_equal_int);
  else if (streq (s, "bool_and_bool"))		 return (bool_and_bool);
  else if (streq (s, "bool_or_bool"))		 return (bool_or_bool);
  else if (streq (s, "bool_xor_bool"))		 return (bool_xor_bool);
  else if (streq (s, "not_bool"))		 return (not_bool);
  else if (streq (s, "real_plus_real"))		 return (real_plus_real);
  else if (streq (s, "real_minus_real"))	 return (real_minus_real);
  else if (streq (s, "real_times_real"))	 return (real_times_real);
  else if (streq (s, "real_divide_real"))	 return (real_divide_real);
  else if (streq (s, "real_pow_int"))		 return (real_pow_int);
  else if (streq (s, "negate_real")) 		 return (negate_real);
  else if (streq (s, "real_equal_real"))	 return (real_equal_real);
  else if (streq (s, "real_notequal_real"))	 return (real_notequal_real);
  else if (streq (s, "real_less_than_real"))	 return (real_less_than_real);
  else if (streq (s, "real_less_equal_real"))	 return (real_less_equal_real);
  else if (streq (s, "real_greater_than_real"))  return (real_greater_than_real);
  else if (streq (s, "real_greater_equal_real")) return (real_greater_equal_real);
  else if (streq (s, "text_plus_text"))		 return (text_plus_text);
  else if (streq (s, "int_times_text"))		 return (int_times_text);
  else if (streq (s, "text_equal_text"))	 return (text_equal_text);
  else if (streq (s, "text_notequal_text"))	 return (text_notequal_text);
  else if (streq (s, "text_less_than_text"))	 return (text_less_than_text);
  else if (streq (s, "text_less_equal_text"))	 return (text_less_equal_text);
  else if (streq (s, "text_greater_than_text"))  return (text_greater_than_text);
  else if (streq (s, "text_greater_equal_text")) return (text_greater_equal_text);
  else if (streq (s, "int_to_real")) 		 return (int_to_real);
  else dcg_error (1, "can not convert '%s' into opcode", s);
  return (nop);
}

static string convert_string_quoted_text (string str)
{ char buf[MAXSTRLEN];
  char *dptr = buf;
  char *sptr;
  *dptr++ = '#';
  *dptr++ = '"';
  for (sptr = str; *sptr; sptr++, dptr++)
    { char ch = *sptr;
      if (ch < ' ')
	{ sprintf (dptr, "\\%03o", ch);
	  dptr += 3;
	}
      else if ((ch == '*') && (*(sptr + 1) == '/'))
	{ *dptr++ = '*';
	  *dptr = '\\';
	}
      else *dptr = *sptr;
    };
  *dptr++ = '"';
  *dptr = '\0';
  return (dcg_new_fmtd_string (buf));
}

string convert_operand_text (oprd op)
{ switch (op -> tag)
    { case TAGTconst:
	return (convert_string_quoted_text (op -> Tconst.tval));
      case TAGRconst:
	return (dcg_new_fmtd_string ("#%.1e", op -> Rconst.rval));
      case TAGIconst:
	return (dcg_new_fmtd_string ("#%d", op -> Iconst.ival));
	break;
      case TAGProc: return (dcg_new_fmtd_string ("P%d", op -> Proc.pnr));
      case TAGRts: return (attach_string (op -> Rts.proc));
      case TAGVar:
	{ var v = im_vars -> array[op -> Var.vnr];
	  string vname = v -> vname;
	  if (v -> vglob) return (dcg_new_fmtd_string ("G_%s", vname));
	  else if (v -> vref) return (dcg_new_fmtd_string ("@%s", vname));
	  else return (attach_string (vname));
	};
      case TAGLab: /* Note obsolete */
	return (dcg_new_fmtd_string ("L%d", op -> Lab.lab));
      case TAGBlock:
	return (dcg_new_fmtd_string ("BB%d", op -> Block.bnr));
      case TAGTypes:
	return (dcg_new_fmtd_string ("{TL%d}", op -> Types.atypes -> size));
      case TAGReflexive: return (attach_string (reflexive_repr));
      case TAGRts_nil: return (new_string ("#Nil"));
      case TAGNop: return (attach_string (no_oprd_repr));
      default: dcg_bad_tag (op -> tag, "convert_operand_text");
    };
  return (string_nil);
}

/* Assumption: there is at least one tuple to dump */
static void dump_tuple_segment (tuple anchor)
{ tuple ptr = anchor;
  do
     { string opc = convert_opcode_text (ptr -> opc);
       string oprd1 = convert_operand_text (ptr -> op1); 
       string oprd2 = convert_operand_text (ptr -> op2); 
       string dst = convert_operand_text (ptr -> dst);
       if (ptr -> ad)
	 dcg_eprint ("BB%d:", ptr -> ad);
       dcg_eprint ("\t[ %s, %s, %s, %s ]\n", opc, oprd1, oprd2, dst);
       detach_string (&oprd1);
       detach_string (&oprd2);
       detach_string (&dst);
       ptr = ptr -> next;
     }
  while (ptr != anchor);
}

void dump_imc_program ()
{ int ix;
  dcg_wlog ("Dump of intermediate code:");
  for (ix = 0; ix < im_procs -> size; ix++)
    { proc p = im_procs -> array[ix];
      block bptr;
      dcg_eprint ("/* %s */\n", p -> pname);
      dcg_eprint ("P%d:\n", ix);
      for (bptr = p -> init_block; bptr != block_nil; bptr = bptr -> direct_succ)
        dump_tuple_segment (bptr -> anchor);
    };
}

void try_report_imc ()
{ int nprocs = im_procs -> size - 1;
  dcg_hint ("      collected %d procedure%s, %d basic blocks, %d tuples",
	    nprocs, (nprocs == 1)?"":"s", all_blocks -> size, all_tuples -> size - 1);
}
