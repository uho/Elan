/*
   File: elan_imc.c
   Generated on Wed Oct  5 12:44:35 2011
*/

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_dump.h>
#include <dcg_binfile.h>

/* local includes */
#include "elan_imc.h"

/* Allocate new proc_list */
proc_list init_proc_list (int room)
{ proc_list new = (proc_list) dcg_malloc (sizeof (struct str_proc_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (proc *) dcg_calloc (safe_room, sizeof (proc));
  return (new);
}

/* Allocate new tuple_list */
tuple_list init_tuple_list (int room)
{ tuple_list new = (tuple_list) dcg_malloc (sizeof (struct str_tuple_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (tuple *) dcg_calloc (safe_room, sizeof (tuple));
  return (new);
}

/* Allocate new block_list */
block_list init_block_list (int room)
{ block_list new = (block_list) dcg_malloc (sizeof (struct str_block_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (block *) dcg_calloc (safe_room, sizeof (block));
  return (new);
}

/* Allocate new oprd_list */
oprd_list init_oprd_list (int room)
{ oprd_list new = (oprd_list) dcg_malloc (sizeof (struct str_oprd_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (oprd *) dcg_calloc (safe_room, sizeof (oprd));
  return (new);
}

/* Allocate new var_list */
var_list init_var_list (int room)
{ var_list new = (var_list) dcg_malloc (sizeof (struct str_var_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (var *) dcg_calloc (safe_room, sizeof (var));
  return (new);
}

/* Allocate new refadm_list */
refadm_list init_refadm_list (int room)
{ refadm_list new = (refadm_list) dcg_malloc (sizeof (struct str_refadm_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (refadm *) dcg_calloc (safe_room, sizeof (refadm));
  return (new);
}

/* Announce to use 'room' chunks for proc_list */
void room_proc_list (proc_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (proc));
  l -> room = room;
}

/* Announce to use 'room' chunks for tuple_list */
void room_tuple_list (tuple_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (tuple));
  l -> room = room;
}

/* Announce to use 'room' chunks for block_list */
void room_block_list (block_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (block));
  l -> room = room;
}

/* Announce to use 'room' chunks for oprd_list */
void room_oprd_list (oprd_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (oprd));
  l -> room = room;
}

/* Announce to use 'room' chunks for var_list */
void room_var_list (var_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (var));
  l -> room = room;
}

/* Announce to use 'room' chunks for refadm_list */
void room_refadm_list (refadm_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (refadm));
  l -> room = room;
}

/* Allocate new proc record */
proc new_proc (string pname, string cname, type rtyp, decl_list args, body cbody)
{ proc new = (proc) dcg_malloc (sizeof (struct str_proc));
  new -> pname = pname;
  new -> cname = cname;
  new -> rtyp = rtyp;
  new -> args = args;
  new -> cbody = cbody;
  new -> anchor = tuple_nil;
  new -> locals = int_list_nil;
  new -> count = int_nil;
  new -> init_block = block_nil;
  new -> tmp_pdecls = pdecl_list_nil;
  new -> tmp1 = int_nil;
  new -> tmp2 = int_nil;
  new -> pnr = int_nil;
  return (new);
};

/* Allocate new tuple record */
tuple new_tuple (opcode opc, oprd op1, oprd op2, oprd dst)
{ tuple new = (tuple) dcg_malloc (sizeof (struct str_tuple));
  new -> opc = opc;
  new -> op1 = op1;
  new -> op2 = op2;
  new -> dst = dst;
  new -> ad = int_nil;
  new -> next = tuple_nil;
  new -> prev = tuple_nil;
  return (new);
};

/* Allocate new block record */
block new_block (int bnr)
{ block new = (block) dcg_malloc (sizeof (struct str_block));
  new -> bnr = bnr;
  new -> direct_succ = block_nil;
  new -> jump_succ = block_nil;
  new -> pred = block_list_nil;
  new -> anchor = tuple_nil;
  return (new);
};

/* Allocate new oprd record for constructor Tconst */
oprd new_Tconst (string tval)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGTconst;
  new -> Tconst.tval = tval;
  return (new);
}

/* Allocate new oprd record for constructor Rconst */
oprd new_Rconst (real rval)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGRconst;
  new -> Rconst.rval = rval;
  return (new);
}

/* Allocate new oprd record for constructor Iconst */
oprd new_Iconst (int ival)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGIconst;
  new -> Iconst.ival = ival;
  return (new);
}

/* Allocate new oprd record for constructor Proc */
oprd new_Proc (int pnr)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGProc;
  new -> Proc.pnr = pnr;
  return (new);
}

/* Allocate new oprd record for constructor Rts */
oprd new_Rts (string proc)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGRts;
  new -> Rts.proc = proc;
  return (new);
}

/* Allocate new oprd record for constructor Var */
oprd new_Var (int vnr)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGVar;
  new -> Var.vnr = vnr;
  return (new);
}

/* Allocate new oprd record for constructor Lab */
oprd new_Lab (int lab)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGLab;
  new -> Lab.lab = lab;
  return (new);
}

/* Allocate new oprd record for constructor Block */
oprd new_Block (int bnr)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGBlock;
  new -> Block.bnr = bnr;
  return (new);
}

/* Allocate new oprd record for constructor Types */
oprd new_Types (pdecl_list atypes)
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGTypes;
  new -> Types.atypes = atypes;
  return (new);
}

/* Allocate new oprd record for constructor Reflexive */
oprd new_Reflexive ()
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGReflexive;
  return (new);
}

/* Allocate new oprd record for constructor Rts_nil */
oprd new_Rts_nil ()
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGRts_nil;
  return (new);
}

/* Allocate new oprd record for constructor Nop */
oprd new_Nop ()
{ oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  new -> tag = TAGNop;
  return (new);
}

/* Allocate new var record */
var new_var (string vname, type vtype, int vglob, int vref, int vused)
{ var new = (var) dcg_malloc (sizeof (struct str_var));
  new -> vname = vname;
  new -> vtype = vtype;
  new -> vglob = vglob;
  new -> vref = vref;
  new -> vused = vused;
  new -> vsize = int_nil;
  new -> offset = int_nil;
  return (new);
};

/* Allocate new refadm record */
refadm new_refadm (decl algorithm, xkind kind, oprd result, int_list active_temps)
{ refadm new = (refadm) dcg_malloc (sizeof (struct str_refadm));
  new -> algorithm = algorithm;
  new -> kind = kind;
  new -> result = result;
  new -> active_temps = active_temps;
  new -> lab = int_nil;
  new -> trc = int_nil;
  return (new);
};

/* Recursively detach a proc */
void detach_proc (proc *optr)
{ proc old = (proc) dcg_predetach ((void **) optr);
  if (old == proc_nil) return;
  detach_string (&(old -> pname));
  detach_string (&(old -> cname));
  detach_type (&(old -> rtyp));
  detach_decl_list (&(old -> args));
  detach_body (&(old -> cbody));
  dcg_detach ((void **) &old);
}

/* Recursively detach a tuple */
void detach_tuple (tuple *optr)
{ tuple old = (tuple) dcg_predetach ((void **) optr);
  if (old == tuple_nil) return;
  detach_opcode (&(old -> opc));
  detach_oprd (&(old -> op1));
  detach_oprd (&(old -> op2));
  detach_oprd (&(old -> dst));
  dcg_detach ((void **) &old);
}

/* Recursively detach a block */
void detach_block (block *optr)
{ block old = (block) dcg_predetach ((void **) optr);
  if (old == block_nil) return;
  detach_int (&(old -> bnr));
  dcg_detach ((void **) &old);
}

/* Recursively detach a oprd */
void detach_oprd (oprd *optr)
{ oprd old = (oprd) dcg_predetach ((void **) optr);
  if (old == oprd_nil) return;
  switch (old -> tag)
    { case TAGTconst:
	detach_string (&(old -> Tconst.tval));
	break;
      case TAGRconst:
	detach_real (&(old -> Rconst.rval));
	break;
      case TAGIconst:
	detach_int (&(old -> Iconst.ival));
	break;
      case TAGProc:
	detach_int (&(old -> Proc.pnr));
	break;
      case TAGRts:
	detach_string (&(old -> Rts.proc));
	break;
      case TAGVar:
	detach_int (&(old -> Var.vnr));
	break;
      case TAGLab:
	detach_int (&(old -> Lab.lab));
	break;
      case TAGBlock:
	detach_int (&(old -> Block.bnr));
	break;
      case TAGTypes:
	detach_pdecl_list (&(old -> Types.atypes));
	break;
      case TAGReflexive:
	break;
      case TAGRts_nil:
	break;
      case TAGNop:
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_oprd");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a var */
void detach_var (var *optr)
{ var old = (var) dcg_predetach ((void **) optr);
  if (old == var_nil) return;
  detach_string (&(old -> vname));
  detach_type (&(old -> vtype));
  detach_int (&(old -> vglob));
  detach_int (&(old -> vref));
  detach_int (&(old -> vused));
  dcg_detach ((void **) &old);
}

/* Recursively detach a refadm */
void detach_refadm (refadm *optr)
{ refadm old = (refadm) dcg_predetach ((void **) optr);
  if (old == refadm_nil) return;
  detach_decl (&(old -> algorithm));
  detach_xkind (&(old -> kind));
  detach_oprd (&(old -> result));
  detach_int_list (&(old -> active_temps));
  dcg_detach ((void **) &old);
}

/* Recursively detach a proc_list */
void detach_proc_list (proc_list *lp)
{ int ix;
  proc_list old = (proc_list) dcg_predetach ((void **) lp);
  if (old == proc_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_proc (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a tuple_list */
void detach_tuple_list (tuple_list *lp)
{ int ix;
  tuple_list old = (tuple_list) dcg_predetach ((void **) lp);
  if (old == tuple_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_tuple (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a block_list */
void detach_block_list (block_list *lp)
{ int ix;
  block_list old = (block_list) dcg_predetach ((void **) lp);
  if (old == block_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_block (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a oprd_list */
void detach_oprd_list (oprd_list *lp)
{ int ix;
  oprd_list old = (oprd_list) dcg_predetach ((void **) lp);
  if (old == oprd_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_oprd (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a var_list */
void detach_var_list (var_list *lp)
{ int ix;
  var_list old = (var_list) dcg_predetach ((void **) lp);
  if (old == var_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_var (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a refadm_list */
void detach_refadm_list (refadm_list *lp)
{ int ix;
  refadm_list old = (refadm_list) dcg_predetach ((void **) lp);
  if (old == refadm_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_refadm (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a proc_list without detaching elements */
void nonrec_detach_proc_list (proc_list *lp)
{ proc_list old = (proc_list) dcg_predetach ((void **) lp);
  if (old == proc_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a tuple_list without detaching elements */
void nonrec_detach_tuple_list (tuple_list *lp)
{ tuple_list old = (tuple_list) dcg_predetach ((void **) lp);
  if (old == tuple_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a block_list without detaching elements */
void nonrec_detach_block_list (block_list *lp)
{ block_list old = (block_list) dcg_predetach ((void **) lp);
  if (old == block_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a oprd_list without detaching elements */
void nonrec_detach_oprd_list (oprd_list *lp)
{ oprd_list old = (oprd_list) dcg_predetach ((void **) lp);
  if (old == oprd_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a var_list without detaching elements */
void nonrec_detach_var_list (var_list *lp)
{ var_list old = (var_list) dcg_predetach ((void **) lp);
  if (old == var_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a refadm_list without detaching elements */
void nonrec_detach_refadm_list (refadm_list *lp)
{ refadm_list old = (refadm_list) dcg_predetach ((void **) lp);
  if (old == refadm_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Append element to proc_list */
proc_list append_proc_list (proc_list l, proc el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_proc_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to tuple_list */
tuple_list append_tuple_list (tuple_list l, tuple el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_tuple_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to block_list */
block_list append_block_list (block_list l, block el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_block_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to oprd_list */
oprd_list append_oprd_list (oprd_list l, oprd el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_oprd_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to var_list */
var_list append_var_list (var_list l, var el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_var_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to refadm_list */
refadm_list append_refadm_list (refadm_list l, refadm el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_refadm_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Concatenate two proc_lists */
proc_list concat_proc_list (proc_list l1, proc_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_proc_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two tuple_lists */
tuple_list concat_tuple_list (tuple_list l1, tuple_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_tuple_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two block_lists */
block_list concat_block_list (block_list l1, block_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_block_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two oprd_lists */
oprd_list concat_oprd_list (oprd_list l1, oprd_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_oprd_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two var_lists */
var_list concat_var_list (var_list l1, var_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_var_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two refadm_lists */
refadm_list concat_refadm_list (refadm_list l1, refadm_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_refadm_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Insert element into proc_list at pos 'pos' */
proc_list insert_proc_list (proc_list l, int pos, proc el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_proc_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_proc_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into tuple_list at pos 'pos' */
tuple_list insert_tuple_list (tuple_list l, int pos, tuple el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_tuple_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_tuple_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into block_list at pos 'pos' */
block_list insert_block_list (block_list l, int pos, block el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_block_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_block_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into oprd_list at pos 'pos' */
oprd_list insert_oprd_list (oprd_list l, int pos, oprd el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_oprd_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_oprd_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into var_list at pos 'pos' */
var_list insert_var_list (var_list l, int pos, var el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_var_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_var_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into refadm_list at pos 'pos' */
refadm_list insert_refadm_list (refadm_list l, int pos, refadm el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_refadm_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_refadm_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Delete element from proc_list at pos 'pos' */
proc_list delete_proc_list (proc_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_proc_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from tuple_list at pos 'pos' */
tuple_list delete_tuple_list (tuple_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_tuple_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from block_list at pos 'pos' */
block_list delete_block_list (block_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_block_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from oprd_list at pos 'pos' */
oprd_list delete_oprd_list (oprd_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_oprd_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from var_list at pos 'pos' */
var_list delete_var_list (var_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_var_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from refadm_list at pos 'pos' */
refadm_list delete_refadm_list (refadm_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_refadm_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Comparison test for a proc */
int cmp_proc (proc a, proc b)
{ int s;
  if (a == b) return (0);
  if (a == proc_nil) return (-1);
  if (b == proc_nil) return (1);
  if ((s = cmp_string (a -> pname, b -> pname))) return (s);
  if ((s = cmp_string (a -> cname, b -> cname))) return (s);
  if ((s = cmp_type (a -> rtyp, b -> rtyp))) return (s);
  if ((s = cmp_decl_list (a -> args, b -> args))) return (s);
  if ((s = cmp_body (a -> cbody, b -> cbody))) return (s);
  return (0);
}

/* Comparison test for a tuple */
int cmp_tuple (tuple a, tuple b)
{ int s;
  if (a == b) return (0);
  if (a == tuple_nil) return (-1);
  if (b == tuple_nil) return (1);
  if ((s = cmp_opcode (a -> opc, b -> opc))) return (s);
  if ((s = cmp_oprd (a -> op1, b -> op1))) return (s);
  if ((s = cmp_oprd (a -> op2, b -> op2))) return (s);
  if ((s = cmp_oprd (a -> dst, b -> dst))) return (s);
  return (0);
}

/* Comparison test for a block */
int cmp_block (block a, block b)
{ int s;
  if (a == b) return (0);
  if (a == block_nil) return (-1);
  if (b == block_nil) return (1);
  if ((s = cmp_int (a -> bnr, b -> bnr))) return (s);
  return (0);
}

/* Comparison test for a oprd */
int cmp_oprd (oprd a, oprd b)
{ int s;
  if (a == b) return (0);
  if (a == oprd_nil) return (-1);
  if (b == oprd_nil) return (1);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGTconst:
	if ((s = cmp_string (a -> Tconst.tval, b -> Tconst.tval))) return (s);
	break;
      case TAGRconst:
	if ((s = cmp_real (a -> Rconst.rval, b -> Rconst.rval))) return (s);
	break;
      case TAGIconst:
	if ((s = cmp_int (a -> Iconst.ival, b -> Iconst.ival))) return (s);
	break;
      case TAGProc:
	if ((s = cmp_int (a -> Proc.pnr, b -> Proc.pnr))) return (s);
	break;
      case TAGRts:
	if ((s = cmp_string (a -> Rts.proc, b -> Rts.proc))) return (s);
	break;
      case TAGVar:
	if ((s = cmp_int (a -> Var.vnr, b -> Var.vnr))) return (s);
	break;
      case TAGLab:
	if ((s = cmp_int (a -> Lab.lab, b -> Lab.lab))) return (s);
	break;
      case TAGBlock:
	if ((s = cmp_int (a -> Block.bnr, b -> Block.bnr))) return (s);
	break;
      case TAGTypes:
	if ((s = cmp_pdecl_list (a -> Types.atypes, b -> Types.atypes))) return (s);
	break;
      case TAGReflexive:
	break;
      case TAGRts_nil:
	break;
      case TAGNop:
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_oprd");
    };
  return (0);
}

/* Comparison test for a var */
int cmp_var (var a, var b)
{ int s;
  if (a == b) return (0);
  if (a == var_nil) return (-1);
  if (b == var_nil) return (1);
  if ((s = cmp_string (a -> vname, b -> vname))) return (s);
  if ((s = cmp_type (a -> vtype, b -> vtype))) return (s);
  if ((s = cmp_int (a -> vglob, b -> vglob))) return (s);
  if ((s = cmp_int (a -> vref, b -> vref))) return (s);
  if ((s = cmp_int (a -> vused, b -> vused))) return (s);
  return (0);
}

/* Comparison test for a refadm */
int cmp_refadm (refadm a, refadm b)
{ int s;
  if (a == b) return (0);
  if (a == refadm_nil) return (-1);
  if (b == refadm_nil) return (1);
  if ((s = cmp_decl (a -> algorithm, b -> algorithm))) return (s);
  if ((s = cmp_xkind (a -> kind, b -> kind))) return (s);
  if ((s = cmp_oprd (a -> result, b -> result))) return (s);
  if ((s = cmp_int_list (a -> active_temps, b -> active_temps))) return (s);
  return (0);
}

/* Comparison test for a proc_list */
int cmp_proc_list (proc_list a, proc_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == proc_list_nil) return (-1);
  if (b == proc_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_proc (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a tuple_list */
int cmp_tuple_list (tuple_list a, tuple_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == tuple_list_nil) return (-1);
  if (b == tuple_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_tuple (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a block_list */
int cmp_block_list (block_list a, block_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == block_list_nil) return (-1);
  if (b == block_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_block (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a oprd_list */
int cmp_oprd_list (oprd_list a, oprd_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == oprd_list_nil) return (-1);
  if (b == oprd_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_oprd (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a var_list */
int cmp_var_list (var_list a, var_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == var_list_nil) return (-1);
  if (b == var_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_var (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a refadm_list */
int cmp_refadm_list (refadm_list a, refadm_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == refadm_list_nil) return (-1);
  if (b == refadm_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_refadm (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Estimate printing a proc */
int est_proc (proc old)
{ int size = 0;
  if (old == proc_nil) return (2);
  size += est_string (old -> pname) + 2;
  size += est_string (old -> cname) + 2;
  size += est_type (old -> rtyp) + 2;
  size += est_decl_list (old -> args) + 2;
  size += est_body (old -> cbody) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate printing a tuple */
int est_tuple (tuple old)
{ int size = 0;
  if (old == tuple_nil) return (2);
  size += est_opcode (old -> opc) + 2;
  size += est_oprd (old -> op1) + 2;
  size += est_oprd (old -> op2) + 2;
  size += est_oprd (old -> dst) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate printing a block */
int est_block (block old)
{ int size = 0;
  if (old == block_nil) return (2);
  size += est_int (old -> bnr) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate printing a oprd */
int est_oprd (oprd old)
{ int size = 0;
  if (old == oprd_nil) return (2);
  switch (old -> tag)
    { case TAGTconst:
	size += 8;
	size += est_string (old -> Tconst.tval) + 2;
	break;
      case TAGRconst:
	size += 8;
	size += est_real (old -> Rconst.rval) + 2;
	break;
      case TAGIconst:
	size += 8;
	size += est_int (old -> Iconst.ival) + 2;
	break;
      case TAGProc:
	size += 6;
	size += est_int (old -> Proc.pnr) + 2;
	break;
      case TAGRts:
	size += 5;
	size += est_string (old -> Rts.proc) + 2;
	break;
      case TAGVar:
	size += 5;
	size += est_int (old -> Var.vnr) + 2;
	break;
      case TAGLab:
	size += 5;
	size += est_int (old -> Lab.lab) + 2;
	break;
      case TAGBlock:
	size += 7;
	size += est_int (old -> Block.bnr) + 2;
	break;
      case TAGTypes:
	size += 7;
	size += est_pdecl_list (old -> Types.atypes) + 2;
	break;
      case TAGReflexive:
	size += 11;
	break;
      case TAGRts_nil:
	size += 9;
	break;
      case TAGNop:
	size += 5;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_oprd");
    };
  return (size + 2);
}

/* Estimate printing a var */
int est_var (var old)
{ int size = 0;
  if (old == var_nil) return (2);
  size += est_string (old -> vname) + 2;
  size += est_type (old -> vtype) + 2;
  size += est_int (old -> vglob) + 2;
  size += est_int (old -> vref) + 2;
  size += est_int (old -> vused) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate printing a refadm */
int est_refadm (refadm old)
{ int size = 0;
  if (old == refadm_nil) return (2);
  size += est_decl (old -> algorithm) + 2;
  size += est_xkind (old -> kind) + 2;
  size += est_oprd (old -> result) + 2;
  size += est_int_list (old -> active_temps) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate a xkind */
int est_xkind (xkind old)
{ switch (old)
    { case k_value: return (7);
      case k_addr: return (6);
      case k_oprd: return (6);
      case k_void: return (6);
      default:
	dcg_bad_tag (old, "est_xkind");
    };
  return (0);
}

/* Estimate a opcode */
int est_opcode (opcode old)
{ switch (old)
    { case start_program: return (13);
      case end_program: return (11);
      case prepare_call: return (12);
      case load_parameter: return (14);
      case load_address_parameter: return (22);
      case call_procedure: return (14);
      case call_attach_procedure: return (21);
      case call_detach_procedure: return (21);
      case call_guard_procedure: return (20);
      case enter_procedure: return (15);
      case leave_procedure: return (15);
      case jump: return (4);
      case jump_if_nil: return (11);
      case jump_if_null: return (12);
      case jump_if_addr_equal_addr: return (23);
      case jump_if_int_equal_int: return (21);
      case jump_if_int_notequal_int: return (24);
      case jump_if_int_less_than_int: return (25);
      case jump_if_int_less_equal_int: return (26);
      case jump_if_int_greater_than_int: return (28);
      case jump_if_int_greater_equal_int: return (29);
      case undefine: return (8);
      case allocate: return (8);
      case guard: return (5);
      case attach_adr: return (10);
      case attach_offset: return (13);
      case detach_adr: return (10);
      case detach_offset: return (13);
      case predetach_adr: return (13);
      case copy_adr_of: return (11);
      case copy_int: return (8);
      case copy_int_indirect: return (17);
      case copy_indirect_int: return (17);
      case copy_adr: return (8);
      case copy_adr_indirect: return (17);
      case copy_indirect_adr: return (17);
      case copy_real: return (9);
      case copy_real_indirect: return (18);
      case copy_indirect_real: return (18);
      case is_nil: return (6);
      case addr_plus_offset: return (16);
      case row_index: return (9);
      case int_plus_int: return (12);
      case int_minus_int: return (13);
      case int_times_int: return (13);
      case int_div_int: return (11);
      case int_mod_int: return (11);
      case int_pow_int: return (11);
      case negate_int: return (10);
      case int_equal_int: return (13);
      case int_notequal_int: return (16);
      case int_less_than_int: return (17);
      case int_less_equal_int: return (18);
      case int_greater_than_int: return (20);
      case int_greater_equal_int: return (21);
      case bool_and_bool: return (13);
      case bool_or_bool: return (12);
      case bool_xor_bool: return (13);
      case not_bool: return (8);
      case real_plus_real: return (14);
      case real_minus_real: return (15);
      case real_times_real: return (15);
      case real_divide_real: return (16);
      case real_pow_int: return (12);
      case negate_real: return (11);
      case real_equal_real: return (15);
      case real_notequal_real: return (18);
      case real_less_than_real: return (19);
      case real_less_equal_real: return (20);
      case real_greater_than_real: return (22);
      case real_greater_equal_real: return (23);
      case text_plus_text: return (14);
      case int_times_text: return (14);
      case text_equal_text: return (15);
      case text_notequal_text: return (18);
      case text_less_than_text: return (19);
      case text_less_equal_text: return (20);
      case text_greater_than_text: return (22);
      case text_greater_equal_text: return (23);
      case int_to_real: return (11);
      case sline: return (5);
      case comment: return (7);
      case nop: return (3);
      default:
	dcg_bad_tag (old, "est_opcode");
    };
  return (0);
}

/* Estimate printing a proc_list */
int est_proc_list (proc_list old)
{ int ix;
  int size = 0;
  if (old == proc_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_proc (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a tuple_list */
int est_tuple_list (tuple_list old)
{ int ix;
  int size = 0;
  if (old == tuple_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_tuple (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a block_list */
int est_block_list (block_list old)
{ int ix;
  int size = 0;
  if (old == block_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_block (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a oprd_list */
int est_oprd_list (oprd_list old)
{ int ix;
  int size = 0;
  if (old == oprd_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_oprd (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a var_list */
int est_var_list (var_list old)
{ int ix;
  int size = 0;
  if (old == var_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_var (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a refadm_list */
int est_refadm_list (refadm_list old)
{ int ix;
  int size = 0;
  if (old == refadm_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_refadm (old -> array[ix]) + 2;
  return (size + 2);
}

/* Pretty print a proc */
void ppp_proc (FILE *f, int horiz, int ind, proc old)
{ int mhoriz;
  if (old == proc_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_proc (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> pname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string (f, mhoriz, ind + 2, old -> cname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_type (f, mhoriz, ind + 2, old -> rtyp);
  pppdelim (f, mhoriz, ind, ',');
  ppp_decl_list (f, mhoriz, ind + 2, old -> args);
  pppdelim (f, mhoriz, ind, ',');
  ppp_body (f, mhoriz, ind + 2, old -> cbody);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> anchor);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> locals);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> count);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> init_block);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> tmp_pdecls);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> tmp1);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> tmp2);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> pnr);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a tuple */
void ppp_tuple (FILE *f, int horiz, int ind, tuple old)
{ int mhoriz;
  if (old == tuple_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_tuple (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_opcode (f, mhoriz, ind + 2, old -> opc);
  pppdelim (f, mhoriz, ind, ',');
  ppp_oprd (f, mhoriz, ind + 2, old -> op1);
  pppdelim (f, mhoriz, ind, ',');
  ppp_oprd (f, mhoriz, ind + 2, old -> op2);
  pppdelim (f, mhoriz, ind, ',');
  ppp_oprd (f, mhoriz, ind + 2, old -> dst);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> ad);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> next);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> prev);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a block */
void ppp_block (FILE *f, int horiz, int ind, block old)
{ int mhoriz;
  if (old == block_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_block (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_int (f, mhoriz, ind + 2, old -> bnr);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> direct_succ);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> jump_succ);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> pred);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> anchor);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a oprd */
void ppp_oprd (FILE *f, int horiz, int ind, oprd old)
{ int mhoriz;
  if (old == oprd_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_oprd (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  switch (old -> tag)
    { case TAGTconst:
	pppstring (f, "Tconst");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Tconst.tval);
	break;
      case TAGRconst:
	pppstring (f, "Rconst");
	pppdelim (f, mhoriz, ind, ',');
	ppp_real (f, mhoriz, ind + 2, old -> Rconst.rval);
	break;
      case TAGIconst:
	pppstring (f, "Iconst");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Iconst.ival);
	break;
      case TAGProc:
	pppstring (f, "Proc");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Proc.pnr);
	break;
      case TAGRts:
	pppstring (f, "Rts");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Rts.proc);
	break;
      case TAGVar:
	pppstring (f, "Var");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Var.vnr);
	break;
      case TAGLab:
	pppstring (f, "Lab");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Lab.lab);
	break;
      case TAGBlock:
	pppstring (f, "Block");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Block.bnr);
	break;
      case TAGTypes:
	pppstring (f, "Types");
	pppdelim (f, mhoriz, ind, ',');
	ppp_pdecl_list (f, mhoriz, ind + 2, old -> Types.atypes);
	break;
      case TAGReflexive:
	pppstring (f, "Reflexive");
	break;
      case TAGRts_nil:
	pppstring (f, "Rts_nil");
	break;
      case TAGNop:
	pppstring (f, "Nop");
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_oprd");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a var */
void ppp_var (FILE *f, int horiz, int ind, var old)
{ int mhoriz;
  if (old == var_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_var (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> vname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_type (f, mhoriz, ind + 2, old -> vtype);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int (f, mhoriz, ind + 2, old -> vglob);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int (f, mhoriz, ind + 2, old -> vref);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int (f, mhoriz, ind + 2, old -> vused);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> vsize);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> offset);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a refadm */
void ppp_refadm (FILE *f, int horiz, int ind, refadm old)
{ int mhoriz;
  if (old == refadm_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_refadm (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_decl (f, mhoriz, ind + 2, old -> algorithm);
  pppdelim (f, mhoriz, ind, ',');
  ppp_xkind (f, mhoriz, ind + 2, old -> kind);
  pppdelim (f, mhoriz, ind, ',');
  ppp_oprd (f, mhoriz, ind + 2, old -> result);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int_list (f, mhoriz, ind + 2, old -> active_temps);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> lab);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> trc);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a xkind */
void ppp_xkind (FILE *f, int horiz, int ind, xkind old)
{ switch (old)
     { case k_value:
	pppstring (f, "k_value");
	break;
       case k_addr:
	pppstring (f, "k_addr");
	break;
       case k_oprd:
	pppstring (f, "k_oprd");
	break;
       case k_void:
	pppstring (f, "k_void");
	break;
      default:
	dcg_bad_tag (old, "ppp_xkind");
    };
}

/* Pretty print a opcode */
void ppp_opcode (FILE *f, int horiz, int ind, opcode old)
{ switch (old)
     { case start_program:
	pppstring (f, "start_program");
	break;
       case end_program:
	pppstring (f, "end_program");
	break;
       case prepare_call:
	pppstring (f, "prepare_call");
	break;
       case load_parameter:
	pppstring (f, "load_parameter");
	break;
       case load_address_parameter:
	pppstring (f, "load_address_parameter");
	break;
       case call_procedure:
	pppstring (f, "call_procedure");
	break;
       case call_attach_procedure:
	pppstring (f, "call_attach_procedure");
	break;
       case call_detach_procedure:
	pppstring (f, "call_detach_procedure");
	break;
       case call_guard_procedure:
	pppstring (f, "call_guard_procedure");
	break;
       case enter_procedure:
	pppstring (f, "enter_procedure");
	break;
       case leave_procedure:
	pppstring (f, "leave_procedure");
	break;
       case jump:
	pppstring (f, "jump");
	break;
       case jump_if_nil:
	pppstring (f, "jump_if_nil");
	break;
       case jump_if_null:
	pppstring (f, "jump_if_null");
	break;
       case jump_if_addr_equal_addr:
	pppstring (f, "jump_if_addr_equal_addr");
	break;
       case jump_if_int_equal_int:
	pppstring (f, "jump_if_int_equal_int");
	break;
       case jump_if_int_notequal_int:
	pppstring (f, "jump_if_int_notequal_int");
	break;
       case jump_if_int_less_than_int:
	pppstring (f, "jump_if_int_less_than_int");
	break;
       case jump_if_int_less_equal_int:
	pppstring (f, "jump_if_int_less_equal_int");
	break;
       case jump_if_int_greater_than_int:
	pppstring (f, "jump_if_int_greater_than_int");
	break;
       case jump_if_int_greater_equal_int:
	pppstring (f, "jump_if_int_greater_equal_int");
	break;
       case undefine:
	pppstring (f, "undefine");
	break;
       case allocate:
	pppstring (f, "allocate");
	break;
       case guard:
	pppstring (f, "guard");
	break;
       case attach_adr:
	pppstring (f, "attach_adr");
	break;
       case attach_offset:
	pppstring (f, "attach_offset");
	break;
       case detach_adr:
	pppstring (f, "detach_adr");
	break;
       case detach_offset:
	pppstring (f, "detach_offset");
	break;
       case predetach_adr:
	pppstring (f, "predetach_adr");
	break;
       case copy_adr_of:
	pppstring (f, "copy_adr_of");
	break;
       case copy_int:
	pppstring (f, "copy_int");
	break;
       case copy_int_indirect:
	pppstring (f, "copy_int_indirect");
	break;
       case copy_indirect_int:
	pppstring (f, "copy_indirect_int");
	break;
       case copy_adr:
	pppstring (f, "copy_adr");
	break;
       case copy_adr_indirect:
	pppstring (f, "copy_adr_indirect");
	break;
       case copy_indirect_adr:
	pppstring (f, "copy_indirect_adr");
	break;
       case copy_real:
	pppstring (f, "copy_real");
	break;
       case copy_real_indirect:
	pppstring (f, "copy_real_indirect");
	break;
       case copy_indirect_real:
	pppstring (f, "copy_indirect_real");
	break;
       case is_nil:
	pppstring (f, "is_nil");
	break;
       case addr_plus_offset:
	pppstring (f, "addr_plus_offset");
	break;
       case row_index:
	pppstring (f, "row_index");
	break;
       case int_plus_int:
	pppstring (f, "int_plus_int");
	break;
       case int_minus_int:
	pppstring (f, "int_minus_int");
	break;
       case int_times_int:
	pppstring (f, "int_times_int");
	break;
       case int_div_int:
	pppstring (f, "int_div_int");
	break;
       case int_mod_int:
	pppstring (f, "int_mod_int");
	break;
       case int_pow_int:
	pppstring (f, "int_pow_int");
	break;
       case negate_int:
	pppstring (f, "negate_int");
	break;
       case int_equal_int:
	pppstring (f, "int_equal_int");
	break;
       case int_notequal_int:
	pppstring (f, "int_notequal_int");
	break;
       case int_less_than_int:
	pppstring (f, "int_less_than_int");
	break;
       case int_less_equal_int:
	pppstring (f, "int_less_equal_int");
	break;
       case int_greater_than_int:
	pppstring (f, "int_greater_than_int");
	break;
       case int_greater_equal_int:
	pppstring (f, "int_greater_equal_int");
	break;
       case bool_and_bool:
	pppstring (f, "bool_and_bool");
	break;
       case bool_or_bool:
	pppstring (f, "bool_or_bool");
	break;
       case bool_xor_bool:
	pppstring (f, "bool_xor_bool");
	break;
       case not_bool:
	pppstring (f, "not_bool");
	break;
       case real_plus_real:
	pppstring (f, "real_plus_real");
	break;
       case real_minus_real:
	pppstring (f, "real_minus_real");
	break;
       case real_times_real:
	pppstring (f, "real_times_real");
	break;
       case real_divide_real:
	pppstring (f, "real_divide_real");
	break;
       case real_pow_int:
	pppstring (f, "real_pow_int");
	break;
       case negate_real:
	pppstring (f, "negate_real");
	break;
       case real_equal_real:
	pppstring (f, "real_equal_real");
	break;
       case real_notequal_real:
	pppstring (f, "real_notequal_real");
	break;
       case real_less_than_real:
	pppstring (f, "real_less_than_real");
	break;
       case real_less_equal_real:
	pppstring (f, "real_less_equal_real");
	break;
       case real_greater_than_real:
	pppstring (f, "real_greater_than_real");
	break;
       case real_greater_equal_real:
	pppstring (f, "real_greater_equal_real");
	break;
       case text_plus_text:
	pppstring (f, "text_plus_text");
	break;
       case int_times_text:
	pppstring (f, "int_times_text");
	break;
       case text_equal_text:
	pppstring (f, "text_equal_text");
	break;
       case text_notequal_text:
	pppstring (f, "text_notequal_text");
	break;
       case text_less_than_text:
	pppstring (f, "text_less_than_text");
	break;
       case text_less_equal_text:
	pppstring (f, "text_less_equal_text");
	break;
       case text_greater_than_text:
	pppstring (f, "text_greater_than_text");
	break;
       case text_greater_equal_text:
	pppstring (f, "text_greater_equal_text");
	break;
       case int_to_real:
	pppstring (f, "int_to_real");
	break;
       case sline:
	pppstring (f, "sline");
	break;
       case comment:
	pppstring (f, "comment");
	break;
       case nop:
	pppstring (f, "nop");
	break;
      default:
	dcg_bad_tag (old, "ppp_opcode");
    };
}

/* Pretty print a proc_list */
void ppp_proc_list (FILE *f, int horiz, int ind, proc_list old)
{ int ix, mhoriz;
  if (old == proc_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_proc_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_proc (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a tuple_list */
void ppp_tuple_list (FILE *f, int horiz, int ind, tuple_list old)
{ int ix, mhoriz;
  if (old == tuple_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_tuple_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_tuple (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a block_list */
void ppp_block_list (FILE *f, int horiz, int ind, block_list old)
{ int ix, mhoriz;
  if (old == block_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_block_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_block (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a oprd_list */
void ppp_oprd_list (FILE *f, int horiz, int ind, oprd_list old)
{ int ix, mhoriz;
  if (old == oprd_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_oprd_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_oprd (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a var_list */
void ppp_var_list (FILE *f, int horiz, int ind, var_list old)
{ int ix, mhoriz;
  if (old == var_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_var_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_var (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a refadm_list */
void ppp_refadm_list (FILE *f, int horiz, int ind, refadm_list old)
{ int ix, mhoriz;
  if (old == refadm_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_refadm_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_refadm (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Recursively save a proc */
void save_proc (BinFile bf, proc old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> pname);
  save_string (bf, old -> cname);
  save_type (bf, old -> rtyp);
  save_decl_list (bf, old -> args);
  save_body (bf, old -> cbody);
}

/* Recursively save a tuple */
void save_tuple (BinFile bf, tuple old)
{ /* save fixed fields before variant ones */
  save_opcode (bf, old -> opc);
  save_oprd (bf, old -> op1);
  save_oprd (bf, old -> op2);
  save_oprd (bf, old -> dst);
}

/* Recursively save a block */
void save_block (BinFile bf, block old)
{ /* save fixed fields before variant ones */
  save_int (bf, old -> bnr);
}

/* Recursively save a oprd */
void save_oprd (BinFile bf, oprd old)
{ /* save fixed fields before variant ones */
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGTconst:
	save_string (bf, old -> Tconst.tval);
	break;
      case TAGRconst:
	save_real (bf, old -> Rconst.rval);
	break;
      case TAGIconst:
	save_int (bf, old -> Iconst.ival);
	break;
      case TAGProc:
	save_int (bf, old -> Proc.pnr);
	break;
      case TAGRts:
	save_string (bf, old -> Rts.proc);
	break;
      case TAGVar:
	save_int (bf, old -> Var.vnr);
	break;
      case TAGLab:
	save_int (bf, old -> Lab.lab);
	break;
      case TAGBlock:
	save_int (bf, old -> Block.bnr);
	break;
      case TAGTypes:
	save_pdecl_list (bf, old -> Types.atypes);
	break;
      case TAGReflexive:
	break;
      case TAGRts_nil:
	break;
      case TAGNop:
	break;
      default:
	dcg_bad_tag (old -> tag, "save_oprd");
    };
}

/* Recursively save a var */
void save_var (BinFile bf, var old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> vname);
  save_type (bf, old -> vtype);
  save_int (bf, old -> vglob);
  save_int (bf, old -> vref);
  save_int (bf, old -> vused);
}

/* Recursively save a refadm */
void save_refadm (BinFile bf, refadm old)
{ /* save fixed fields before variant ones */
  save_decl (bf, old -> algorithm);
  save_xkind (bf, old -> kind);
  save_oprd (bf, old -> result);
  save_int_list (bf, old -> active_temps);
}

/* Recursively save a proc_list */
void save_proc_list (BinFile bf, proc_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_proc (bf, l -> array[ix]);
}

/* Recursively save a tuple_list */
void save_tuple_list (BinFile bf, tuple_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_tuple (bf, l -> array[ix]);
}

/* Recursively save a block_list */
void save_block_list (BinFile bf, block_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_block (bf, l -> array[ix]);
}

/* Recursively save a oprd_list */
void save_oprd_list (BinFile bf, oprd_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_oprd (bf, l -> array[ix]);
}

/* Recursively save a var_list */
void save_var_list (BinFile bf, var_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_var (bf, l -> array[ix]);
}

/* Recursively save a refadm_list */
void save_refadm_list (BinFile bf, refadm_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_refadm (bf, l -> array[ix]);
}

/* Recursively load a proc */
void load_proc (BinFile bf, proc *x)
{ /* load fixed fields before variant ones */
  proc new = (proc) dcg_malloc (sizeof (struct str_proc));
  load_string (bf, &new -> pname);
  load_string (bf, &new -> cname);
  load_type (bf, &new -> rtyp);
  load_decl_list (bf, &new -> args);
  load_body (bf, &new -> cbody);
  new -> anchor = tuple_nil;
  new -> locals = int_list_nil;
  new -> count = int_nil;
  new -> init_block = block_nil;
  new -> tmp_pdecls = pdecl_list_nil;
  new -> tmp1 = int_nil;
  new -> tmp2 = int_nil;
  new -> pnr = int_nil;
  *x = new;
}

/* Recursively load a tuple */
void load_tuple (BinFile bf, tuple *x)
{ /* load fixed fields before variant ones */
  tuple new = (tuple) dcg_malloc (sizeof (struct str_tuple));
  load_opcode (bf, &new -> opc);
  load_oprd (bf, &new -> op1);
  load_oprd (bf, &new -> op2);
  load_oprd (bf, &new -> dst);
  new -> ad = int_nil;
  new -> next = tuple_nil;
  new -> prev = tuple_nil;
  *x = new;
}

/* Recursively load a block */
void load_block (BinFile bf, block *x)
{ /* load fixed fields before variant ones */
  block new = (block) dcg_malloc (sizeof (struct str_block));
  load_int (bf, &new -> bnr);
  new -> direct_succ = block_nil;
  new -> jump_succ = block_nil;
  new -> pred = block_list_nil;
  new -> anchor = tuple_nil;
  *x = new;
}

/* Recursively load a oprd */
void load_oprd (BinFile bf, oprd *x)
{ /* load fixed fields before variant ones */
  oprd new = (oprd) dcg_malloc (sizeof (struct str_oprd));
  char ch;
  load_char (bf, &ch);
  new -> tag = (tags_oprd) ch;
  switch (new -> tag)
    { case TAGTconst:
	load_string (bf, &new -> Tconst.tval);
	break;
      case TAGRconst:
	load_real (bf, &new -> Rconst.rval);
	break;
      case TAGIconst:
	load_int (bf, &new -> Iconst.ival);
	break;
      case TAGProc:
	load_int (bf, &new -> Proc.pnr);
	break;
      case TAGRts:
	load_string (bf, &new -> Rts.proc);
	break;
      case TAGVar:
	load_int (bf, &new -> Var.vnr);
	break;
      case TAGLab:
	load_int (bf, &new -> Lab.lab);
	break;
      case TAGBlock:
	load_int (bf, &new -> Block.bnr);
	break;
      case TAGTypes:
	load_pdecl_list (bf, &new -> Types.atypes);
	break;
      case TAGReflexive:
	break;
      case TAGRts_nil:
	break;
      case TAGNop:
	break;
      default:
	dcg_bad_tag (new -> tag, "load_oprd");
    };
  *x = new;
}

/* Recursively load a var */
void load_var (BinFile bf, var *x)
{ /* load fixed fields before variant ones */
  var new = (var) dcg_malloc (sizeof (struct str_var));
  load_string (bf, &new -> vname);
  load_type (bf, &new -> vtype);
  load_int (bf, &new -> vglob);
  load_int (bf, &new -> vref);
  load_int (bf, &new -> vused);
  new -> vsize = int_nil;
  new -> offset = int_nil;
  *x = new;
}

/* Recursively load a refadm */
void load_refadm (BinFile bf, refadm *x)
{ /* load fixed fields before variant ones */
  refadm new = (refadm) dcg_malloc (sizeof (struct str_refadm));
  load_decl (bf, &new -> algorithm);
  load_xkind (bf, &new -> kind);
  load_oprd (bf, &new -> result);
  load_int_list (bf, &new -> active_temps);
  new -> lab = int_nil;
  new -> trc = int_nil;
  *x = new;
}

/* Load a xkind */
void load_xkind (BinFile bf, xkind *x)
{ char ch;
  load_char (bf, &ch);
  *x = (xkind) ch;
}

/* Load a opcode */
void load_opcode (BinFile bf, opcode *x)
{ char ch;
  load_char (bf, &ch);
  *x = (opcode) ch;
}

/* Recursively load a proc_list */
void load_proc_list (BinFile bf, proc_list *l)
{ int ix, size;
  proc_list new;
  loadsize (bf, &size);
  new = init_proc_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_proc (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a tuple_list */
void load_tuple_list (BinFile bf, tuple_list *l)
{ int ix, size;
  tuple_list new;
  loadsize (bf, &size);
  new = init_tuple_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_tuple (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a block_list */
void load_block_list (BinFile bf, block_list *l)
{ int ix, size;
  block_list new;
  loadsize (bf, &size);
  new = init_block_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_block (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a oprd_list */
void load_oprd_list (BinFile bf, oprd_list *l)
{ int ix, size;
  oprd_list new;
  loadsize (bf, &size);
  new = init_oprd_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_oprd (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a var_list */
void load_var_list (BinFile bf, var_list *l)
{ int ix, size;
  var_list new;
  loadsize (bf, &size);
  new = init_var_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_var (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a refadm_list */
void load_refadm_list (BinFile bf, refadm_list *l)
{ int ix, size;
  refadm_list new;
  loadsize (bf, &size);
  new = init_refadm_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_refadm (bf, &new -> array[ix]);
  *l = new;
}

