/*
   File: imc_utils.h
   Defines general utilities for the intermediate code as well as
   a number of global variables which are used during intermediate code generation

   CVS ID: "$Id: imc_utils.h,v 1.5 2005/05/19 15:48:41 marcs Exp $"
*/
#ifndef IncImcUtils
#define IncImcUtils

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported global vars */
extern tuple_list im_labels;
extern proc_list im_procs;
extern var_list im_vars;
extern oprd reflexive ();
extern oprd no_oprd ();

/* init and finish */
void init_imc ();
void try_report_imc ();
/* export void finish_imc (); */

/* coding administration */
int allocate_proc (string pname, string cname, type rtyp, decl_list args, body cbody);
void set_permanent_call_count (int pnr);
void increment_call_count (int pnr);

int create_variable (decl id, int global);
int create_argument (string name, type t, int vref);
void create_uproc_args (decl_list dl);
int pick_variable (decl id);
void set_vnr_lw_mark ();
int allocate_temporary (type t);
void release_variable (int vnr);

int new_label ();
void set_label (int label);

tuple get_next_location ();
void gen_start_tuple (proc p, opcode opc, int proc_idx);
void gen_tuple (opcode opc, oprd op1, oprd op2, oprd dst);
void insert_tuple (tuple loc, opcode opc, oprd op1, oprd op2, oprd dst);

void register_locals_with_proc (proc p);
void make_basic_blocks (proc p);
void unify_labels (proc p);

/* access on operands */
type type_from_operand (oprd op);

/* conversion utilities */
string convert_opcode_text (opcode opc);
string convert_operand_text (oprd op);
opcode convert_text_opcode (string s);

/* dumping */
void dump_imc_program ();

#endif /* IncImcUtils */
