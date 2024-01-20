/*
   File: elan_imc.h
   Generated on Wed Oct  5 12:44:35 2011
*/
#ifndef IncElan_imc
#define IncElan_imc

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_binfile.h>

/* Introduce record type definitions */
typedef struct str_proc *proc;
typedef struct str_tuple *tuple;
typedef struct str_block *block;
typedef struct str_oprd *oprd;
typedef struct str_var *var;
typedef struct str_refadm *refadm;

/* Introduce type_list definitions */
typedef struct str_proc_list *proc_list;
typedef struct str_tuple_list *tuple_list;
typedef struct str_block_list *block_list;
typedef struct str_oprd_list *oprd_list;
typedef struct str_var_list *var_list;
typedef struct str_refadm_list *refadm_list;

/* Introduce enumeration types */
typedef enum
{ k_value, k_addr, k_oprd,
  k_void
} xkind;

typedef enum
{ start_program, end_program, prepare_call,
  load_parameter, load_address_parameter, call_procedure,
  call_attach_procedure, call_detach_procedure, call_guard_procedure,
  enter_procedure, leave_procedure, jump,
  jump_if_nil, jump_if_null, jump_if_addr_equal_addr,
  jump_if_int_equal_int, jump_if_int_notequal_int, jump_if_int_less_than_int,
  jump_if_int_less_equal_int, jump_if_int_greater_than_int, jump_if_int_greater_equal_int,
  undefine, allocate, guard,
  attach_adr, attach_offset, detach_adr,
  detach_offset, predetach_adr, copy_adr_of,
  copy_int, copy_int_indirect, copy_indirect_int,
  copy_adr, copy_adr_indirect, copy_indirect_adr,
  copy_real, copy_real_indirect, copy_indirect_real,
  is_nil, addr_plus_offset, row_index,
  int_plus_int, int_minus_int, int_times_int,
  int_div_int, int_mod_int, int_pow_int,
  negate_int, int_equal_int, int_notequal_int,
  int_less_than_int, int_less_equal_int, int_greater_than_int,
  int_greater_equal_int, bool_and_bool, bool_or_bool,
  bool_xor_bool, not_bool, real_plus_real,
  real_minus_real, real_times_real, real_divide_real,
  real_pow_int, negate_real, real_equal_real,
  real_notequal_real, real_less_than_real, real_less_equal_real,
  real_greater_than_real, real_greater_equal_real, text_plus_text,
  int_times_text, text_equal_text, text_notequal_text,
  text_less_than_text, text_less_equal_text, text_greater_than_text,
  text_greater_equal_text, int_to_real, sline,
  comment, nop
} opcode;

/* Include imported defs */
#include <dcg_plist.h>
#include <elan_ast.h>

/* Introduce record nils */
#define proc_nil ((proc) NULL)
#define tuple_nil ((tuple) NULL)
#define block_nil ((block) NULL)
#define oprd_nil ((oprd) NULL)
#define var_nil ((var) NULL)
#define refadm_nil ((refadm) NULL)

/* Introduce type_list nils */
#define proc_list_nil ((proc_list) NULL)
#define tuple_list_nil ((tuple_list) NULL)
#define block_list_nil ((block_list) NULL)
#define oprd_list_nil ((oprd_list) NULL)
#define var_list_nil ((var_list) NULL)
#define refadm_list_nil ((refadm_list) NULL)

/* Introduce enumeration nils */
#define xkind_nil ((xkind) 0)
#define opcode_nil ((opcode) 0)

/* introduce constructors */
#define Tconst u.ue_Tconst
#define Rconst u.ue_Rconst
#define Iconst u.ue_Iconst
#define Proc u.ue_Proc
#define Rts u.ue_Rts
#define Var u.ue_Var
#define Lab u.ue_Lab
#define Block u.ue_Block
#define Types u.ue_Types
#define Reflexive u.ue_Reflexive
#define Rts_nil u.ue_Rts_nil
#define Nop u.ue_Nop

/* Introduce constructor tags */
typedef enum en_tags_oprd
{ TAGTconst,
  TAGRconst,
  TAGIconst,
  TAGProc,
  TAGRts,
  TAGVar,
  TAGLab,
  TAGBlock,
  TAGTypes,
  TAGReflexive,
  TAGRts_nil,
  TAGNop
} tags_oprd;

/* Introduce structure for constructor Tconst */
typedef struct str_Tconst
{ string tval;
} C_Tconst;

/* Introduce structure for constructor Rconst */
typedef struct str_Rconst
{ real rval;
} C_Rconst;

/* Introduce structure for constructor Iconst */
typedef struct str_Iconst
{ int ival;
} C_Iconst;

/* Introduce structure for constructor Proc */
typedef struct str_Proc
{ int pnr;
} C_Proc;

/* Introduce structure for constructor Rts */
typedef struct str_Rts
{ string proc;
} C_Rts;

/* Introduce structure for constructor Var */
typedef struct str_Var
{ int vnr;
} C_Var;

/* Introduce structure for constructor Lab */
typedef struct str_Lab
{ int lab;
} C_Lab;

/* Introduce structure for constructor Block */
typedef struct str_Block
{ int bnr;
} C_Block;

/* Introduce structure for constructor Types */
typedef struct str_Types
{ pdecl_list atypes;
} C_Types;

/* Introduce structure of type proc */
struct str_proc
{ string pname;
  string cname;
  type rtyp;
  decl_list args;
  body cbody;
  tuple anchor;
  int_list locals;
  int count;
  block init_block;
  pdecl_list tmp_pdecls;
  int tmp1;
  int tmp2;
  int pnr;
};

/* Introduce structure of type tuple */
struct str_tuple
{ opcode opc;
  oprd op1;
  oprd op2;
  oprd dst;
  int ad;
  tuple next;
  tuple prev;
};

/* Introduce structure of type block */
struct str_block
{ int bnr;
  block direct_succ;
  block jump_succ;
  block_list pred;
  tuple anchor;
};

/* Introduce structure of type oprd */
struct str_oprd
{ tags_oprd tag;
  union uni_oprd
    { C_Tconst ue_Tconst;
      C_Rconst ue_Rconst;
      C_Iconst ue_Iconst;
      C_Proc ue_Proc;
      C_Rts ue_Rts;
      C_Var ue_Var;
      C_Lab ue_Lab;
      C_Block ue_Block;
      C_Types ue_Types;
    } u;
};

/* Introduce structure of type var */
struct str_var
{ string vname;
  type vtype;
  int vglob;
  int vref;
  int vused;
  int vsize;
  int offset;
};

/* Introduce structure of type refadm */
struct str_refadm
{ decl algorithm;
  xkind kind;
  oprd result;
  int_list active_temps;
  int lab;
  int trc;
};

/* Introduce structure of type proc_list */
struct str_proc_list
{ int size;
  int room;
  proc *array;
};

/* Introduce structure of type tuple_list */
struct str_tuple_list
{ int size;
  int room;
  tuple *array;
};

/* Introduce structure of type block_list */
struct str_block_list
{ int size;
  int room;
  block *array;
};

/* Introduce structure of type oprd_list */
struct str_oprd_list
{ int size;
  int room;
  oprd *array;
};

/* Introduce structure of type var_list */
struct str_var_list
{ int size;
  int room;
  var *array;
};

/* Introduce structure of type refadm_list */
struct str_refadm_list
{ int size;
  int room;
  refadm *array;
};

/* Introduce list allocation routines */
proc_list init_proc_list (int room);
tuple_list init_tuple_list (int room);
block_list init_block_list (int room);
oprd_list init_oprd_list (int room);
var_list init_var_list (int room);
refadm_list init_refadm_list (int room);

/* Introduce room allocation routines */
void room_proc_list (proc_list l, int room);
void room_tuple_list (tuple_list l, int room);
void room_block_list (block_list l, int room);
void room_oprd_list (oprd_list l, int room);
void room_var_list (var_list l, int room);
void room_refadm_list (refadm_list l, int room);

/* Introduce list allocation shorthands */
#define new_proc_list() init_proc_list(2)
#define new_tuple_list() init_tuple_list(2)
#define new_block_list() init_block_list(2)
#define new_oprd_list() init_oprd_list(2)
#define new_var_list() init_var_list(2)
#define new_refadm_list() init_refadm_list(2)

/* Introduce type allocation routines */
proc new_proc (string pname, string cname, type rtyp, decl_list args, body cbody);
tuple new_tuple (opcode opc, oprd op1, oprd op2, oprd dst);
block new_block (int bnr);
oprd new_Tconst (string tval);
oprd new_Rconst (real rval);
oprd new_Iconst (int ival);
oprd new_Proc (int pnr);
oprd new_Rts (string proc);
oprd new_Var (int vnr);
oprd new_Lab (int lab);
oprd new_Block (int bnr);
oprd new_Types (pdecl_list atypes);
oprd new_Reflexive ();
oprd new_Rts_nil ();
oprd new_Nop ();
var new_var (string vname, type vtype, int vglob, int vref, int vused);
refadm new_refadm (decl algorithm, xkind kind, oprd result, int_list active_temps);

/* Introduce attaching of types */
#define attach_proc(orig)(proc) dcg_attach ((char *) orig)
#define att_proc(orig)(void) dcg_attach ((char *) orig)
#define attach_tuple(orig)(tuple) dcg_attach ((char *) orig)
#define att_tuple(orig)(void) dcg_attach ((char *) orig)
#define attach_block(orig)(block) dcg_attach ((char *) orig)
#define att_block(orig)(void) dcg_attach ((char *) orig)
#define attach_oprd(orig)(oprd) dcg_attach ((char *) orig)
#define att_oprd(orig)(void) dcg_attach ((char *) orig)
#define attach_var(orig)(var) dcg_attach ((char *) orig)
#define att_var(orig)(void) dcg_attach ((char *) orig)
#define attach_refadm(orig)(refadm) dcg_attach ((char *) orig)
#define att_refadm(orig)(void) dcg_attach ((char *) orig)
#define attach_xkind(orig) (orig)
#define att_xkind(orig)
#define attach_opcode(orig) (orig)
#define att_opcode(orig)

/* Introduce attaching of lists */
#define attach_proc_list(orig) (proc_list) dcg_attach ((char *) orig)
#define att_proc_list(orig) (void) dcg_attach ((char *) orig)
#define attach_tuple_list(orig) (tuple_list) dcg_attach ((char *) orig)
#define att_tuple_list(orig) (void) dcg_attach ((char *) orig)
#define attach_block_list(orig) (block_list) dcg_attach ((char *) orig)
#define att_block_list(orig) (void) dcg_attach ((char *) orig)
#define attach_oprd_list(orig) (oprd_list) dcg_attach ((char *) orig)
#define att_oprd_list(orig) (void) dcg_attach ((char *) orig)
#define attach_var_list(orig) (var_list) dcg_attach ((char *) orig)
#define att_var_list(orig) (void) dcg_attach ((char *) orig)
#define attach_refadm_list(orig) (refadm_list) dcg_attach ((char *) orig)
#define att_refadm_list(orig) (void) dcg_attach ((char *) orig)

/* Introduce detaching of enumeration types */
#define detach_xkind(old)
#define det_xkind(old)
#define detach_opcode(old)
#define det_opcode(old)

/* Introduce detaching of record types */
void detach_proc (proc *optr);
#define det_proc(optr) detach_proc(optr)
void detach_tuple (tuple *optr);
#define det_tuple(optr) detach_tuple(optr)
void detach_block (block *optr);
#define det_block(optr) detach_block(optr)
void detach_oprd (oprd *optr);
#define det_oprd(optr) detach_oprd(optr)
void detach_var (var *optr);
#define det_var(optr) detach_var(optr)
void detach_refadm (refadm *optr);
#define det_refadm(optr) detach_refadm(optr)

/* Introduce detaching of lists */
void detach_proc_list (proc_list *lp);
void detach_tuple_list (tuple_list *lp);
void detach_block_list (block_list *lp);
void detach_oprd_list (oprd_list *lp);
void detach_var_list (var_list *lp);
void detach_refadm_list (refadm_list *lp);

/* Introduce nonrecursive detaching of lists */
void nonrec_detach_proc_list (proc_list *lp);
void nonrec_detach_tuple_list (tuple_list *lp);
void nonrec_detach_block_list (block_list *lp);
void nonrec_detach_oprd_list (oprd_list *lp);
void nonrec_detach_var_list (var_list *lp);
void nonrec_detach_refadm_list (refadm_list *lp);

/* Introduce list append routines */
proc_list append_proc_list (proc_list l, proc el);
tuple_list append_tuple_list (tuple_list l, tuple el);
block_list append_block_list (block_list l, block el);
oprd_list append_oprd_list (oprd_list l, oprd el);
var_list append_var_list (var_list l, var el);
refadm_list append_refadm_list (refadm_list l, refadm el);

/* Introduce void shorthands for list append routines */
#define app_proc_list(l,el) (void) append_proc_list (l,el)
#define app_tuple_list(l,el) (void) append_tuple_list (l,el)
#define app_block_list(l,el) (void) append_block_list (l,el)
#define app_oprd_list(l,el) (void) append_oprd_list (l,el)
#define app_var_list(l,el) (void) append_var_list (l,el)
#define app_refadm_list(l,el) (void) append_refadm_list (l,el)

/* Introduce list concat routines */
proc_list concat_proc_list (proc_list l1, proc_list l2);
tuple_list concat_tuple_list (tuple_list l1, tuple_list l2);
block_list concat_block_list (block_list l1, block_list l2);
oprd_list concat_oprd_list (oprd_list l1, oprd_list l2);
var_list concat_var_list (var_list l1, var_list l2);
refadm_list concat_refadm_list (refadm_list l1, refadm_list l2);

/* Introduce void shorthands for list concat routines */
#define conc_proc_list(l1,l2) (void) concat_proc_list (l1,l2)
#define conc_tuple_list(l1,l2) (void) concat_tuple_list (l1,l2)
#define conc_block_list(l1,l2) (void) concat_block_list (l1,l2)
#define conc_oprd_list(l1,l2) (void) concat_oprd_list (l1,l2)
#define conc_var_list(l1,l2) (void) concat_var_list (l1,l2)
#define conc_refadm_list(l1,l2) (void) concat_refadm_list (l1,l2)

/* Introduce list insertion routines */
proc_list insert_proc_list (proc_list l, int pos, proc el);
tuple_list insert_tuple_list (tuple_list l, int pos, tuple el);
block_list insert_block_list (block_list l, int pos, block el);
oprd_list insert_oprd_list (oprd_list l, int pos, oprd el);
var_list insert_var_list (var_list l, int pos, var el);
refadm_list insert_refadm_list (refadm_list l, int pos, refadm el);

/* Introduce void shorthands for list insertion routines */
#define ins_proc_list(l,pos,el) (void) insert_proc_list (l,pos,el)
#define ins_tuple_list(l,pos,el) (void) insert_tuple_list (l,pos,el)
#define ins_block_list(l,pos,el) (void) insert_block_list (l,pos,el)
#define ins_oprd_list(l,pos,el) (void) insert_oprd_list (l,pos,el)
#define ins_var_list(l,pos,el) (void) insert_var_list (l,pos,el)
#define ins_refadm_list(l,pos,el) (void) insert_refadm_list (l,pos,el)

/* Introduce list deletion routines */
proc_list delete_proc_list (proc_list l, int pos);
tuple_list delete_tuple_list (tuple_list l, int pos);
block_list delete_block_list (block_list l, int pos);
oprd_list delete_oprd_list (oprd_list l, int pos);
var_list delete_var_list (var_list l, int pos);
refadm_list delete_refadm_list (refadm_list l, int pos);

/* Introduce void shorthands for list deletion routines */
#define del_proc_list(l,pos) (void) delete_proc_list (l,pos)
#define del_tuple_list(l,pos) (void) delete_tuple_list (l,pos)
#define del_block_list(l,pos) (void) delete_block_list (l,pos)
#define del_oprd_list(l,pos) (void) delete_oprd_list (l,pos)
#define del_var_list(l,pos) (void) delete_var_list (l,pos)
#define del_refadm_list(l,pos) (void) delete_refadm_list (l,pos)

/* Equality test for types */
int cmp_proc (proc a, proc b);
int cmp_tuple (tuple a, tuple b);
int cmp_block (block a, block b);
int cmp_oprd (oprd a, oprd b);
int cmp_var (var a, var b);
int cmp_refadm (refadm a, refadm b);
#define cmp_xkind(a,b) (cmp_int ((int)(a),(int)(b)))
#define cmp_opcode(a,b) (cmp_int ((int)(a),(int)(b)))

/* Comparison tests for lists */
int cmp_proc_list (proc_list a, proc_list b);
int cmp_tuple_list (tuple_list a, tuple_list b);
int cmp_block_list (block_list a, block_list b);
int cmp_oprd_list (oprd_list a, oprd_list b);
int cmp_var_list (var_list a, var_list b);
int cmp_refadm_list (refadm_list a, refadm_list b);

/* Equality shorthands for types */
#define equal_proc(a,b) !cmp_proc ((a),(b))
#define equal_tuple(a,b) !cmp_tuple ((a),(b))
#define equal_block(a,b) !cmp_block ((a),(b))
#define equal_oprd(a,b) !cmp_oprd ((a),(b))
#define equal_var(a,b) !cmp_var ((a),(b))
#define equal_refadm(a,b) !cmp_refadm ((a),(b))
#define equal_xkind(a,b) !cmp_xkind ((a),(b))
#define equal_opcode(a,b) !cmp_opcode ((a),(b))

/* Equality shorthands for lists */
#define equal_proc_list(a,b) !cmp_proc_list((a),(b))
#define equal_tuple_list(a,b) !cmp_tuple_list((a),(b))
#define equal_block_list(a,b) !cmp_block_list((a),(b))
#define equal_oprd_list(a,b) !cmp_oprd_list((a),(b))
#define equal_var_list(a,b) !cmp_var_list((a),(b))
#define equal_refadm_list(a,b) !cmp_refadm_list((a),(b))

/* Estimating printing of types */
int est_proc (proc old);
int est_tuple (tuple old);
int est_block (block old);
int est_oprd (oprd old);
int est_var (var old);
int est_refadm (refadm old);
int est_xkind (xkind old);
int est_opcode (opcode old);

/* Estimate printing of lists */
int est_proc_list (proc_list old);
int est_tuple_list (tuple_list old);
int est_block_list (block_list old);
int est_oprd_list (oprd_list old);
int est_var_list (var_list old);
int est_refadm_list (refadm_list old);

void ppp_proc (FILE *f, int horiz, int ind, proc old);
void ppp_tuple (FILE *f, int horiz, int ind, tuple old);
void ppp_block (FILE *f, int horiz, int ind, block old);
void ppp_oprd (FILE *f, int horiz, int ind, oprd old);
void ppp_var (FILE *f, int horiz, int ind, var old);
void ppp_refadm (FILE *f, int horiz, int ind, refadm old);
void ppp_xkind (FILE *f, int horiz, int ind, xkind old);
void ppp_opcode (FILE *f, int horiz, int ind, opcode old);

/* Introduce pretty printing of lists */
void ppp_proc_list (FILE *f, int horiz, int ind, proc_list old);
void ppp_tuple_list (FILE *f, int horiz, int ind, tuple_list old);
void ppp_block_list (FILE *f, int horiz, int ind, block_list old);
void ppp_oprd_list (FILE *f, int horiz, int ind, oprd_list old);
void ppp_var_list (FILE *f, int horiz, int ind, var_list old);
void ppp_refadm_list (FILE *f, int horiz, int ind, refadm_list old);

/* Introduce pretty printing shorthands */
#define pp_proc(f,x) ppp_proc (f,0,0,x)
#define pp_tuple(f,x) ppp_tuple (f,0,0,x)
#define pp_block(f,x) ppp_block (f,0,0,x)
#define pp_oprd(f,x) ppp_oprd (f,0,0,x)
#define pp_var(f,x) ppp_var (f,0,0,x)
#define pp_refadm(f,x) ppp_refadm (f,0,0,x)
#define pp_xkind(f,x) ppp_xkind (f,0,0,x)
#define pp_opcode(f,x) ppp_opcode (f,0,0,x)

/* Introduce pretty printing shorthands for lists */
#define pp_proc_list(f,x) ppp_proc_list (f,0,0,x)
#define pp_tuple_list(f,x) ppp_tuple_list (f,0,0,x)
#define pp_block_list(f,x) ppp_block_list (f,0,0,x)
#define pp_oprd_list(f,x) ppp_oprd_list (f,0,0,x)
#define pp_var_list(f,x) ppp_var_list (f,0,0,x)
#define pp_refadm_list(f,x) ppp_refadm_list (f,0,0,x)

/* Introduce saving of types */
#define save_xkind(bf,x) save_char(bf, (char) x)
#define save_opcode(bf,x) save_char(bf, (char) x)
void save_proc (BinFile bf, proc old);
void save_tuple (BinFile bf, tuple old);
void save_block (BinFile bf, block old);
void save_oprd (BinFile bf, oprd old);
void save_var (BinFile bf, var old);
void save_refadm (BinFile bf, refadm old);

/* Introduce saving of lists */
void save_proc_list (BinFile bf, proc_list l);
void save_tuple_list (BinFile bf, tuple_list l);
void save_block_list (BinFile bf, block_list l);
void save_oprd_list (BinFile bf, oprd_list l);
void save_var_list (BinFile bf, var_list l);
void save_refadm_list (BinFile bf, refadm_list l);

/* Introduce loading of types */
void load_proc (BinFile bf, proc *x);
void load_tuple (BinFile bf, tuple *x);
void load_block (BinFile bf, block *x);
void load_oprd (BinFile bf, oprd *x);
void load_var (BinFile bf, var *x);
void load_refadm (BinFile bf, refadm *x);
void load_xkind (BinFile bf, xkind *x);
void load_opcode (BinFile bf, opcode *x);

/* Introduce loading of lists */
void load_proc_list (BinFile bf, proc_list *l);
void load_tuple_list (BinFile bf, tuple_list *l);
void load_block_list (BinFile bf, block_list *l);
void load_oprd_list (BinFile bf, oprd_list *l);
void load_var_list (BinFile bf, var_list *l);
void load_refadm_list (BinFile bf, refadm_list *l);

#endif /* IncElan_imc */
