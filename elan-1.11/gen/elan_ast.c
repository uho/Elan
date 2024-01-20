/*
   File: elan_ast.c
   Generated on Wed Oct  5 12:44:33 2011
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
#include "elan_ast.h"

/* Allocate new packet_list */
packet_list init_packet_list (int room)
{ packet_list new = (packet_list) dcg_malloc (sizeof (struct str_packet_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (packet *) dcg_calloc (safe_room, sizeof (packet));
  return (new);
}

/* Allocate new node_list */
node_list init_node_list (int room)
{ node_list new = (node_list) dcg_malloc (sizeof (struct str_node_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (node *) dcg_calloc (safe_room, sizeof (node));
  return (new);
}

/* Allocate new case_part_list */
case_part_list init_case_part_list (int room)
{ case_part_list new = (case_part_list) dcg_malloc (sizeof (struct str_case_part_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (case_part *) dcg_calloc (safe_room, sizeof (case_part));
  return (new);
}

/* Allocate new decl_list */
decl_list init_decl_list (int room)
{ decl_list new = (decl_list) dcg_malloc (sizeof (struct str_decl_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (decl *) dcg_calloc (safe_room, sizeof (decl));
  return (new);
}

/* Allocate new type_list */
type_list init_type_list (int room)
{ type_list new = (type_list) dcg_malloc (sizeof (struct str_type_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (type *) dcg_calloc (safe_room, sizeof (type));
  return (new);
}

/* Allocate new pdecl_list */
pdecl_list init_pdecl_list (int room)
{ pdecl_list new = (pdecl_list) dcg_malloc (sizeof (struct str_pdecl_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (pdecl *) dcg_calloc (safe_room, sizeof (pdecl));
  return (new);
}

/* Allocate new field_list */
field_list init_field_list (int room)
{ field_list new = (field_list) dcg_malloc (sizeof (struct str_field_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (field *) dcg_calloc (safe_room, sizeof (field));
  return (new);
}

/* Allocate new type_info_list */
type_info_list init_type_info_list (int room)
{ type_info_list new = (type_info_list) dcg_malloc (sizeof (struct str_type_info_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (type_info *) dcg_calloc (safe_room, sizeof (type_info));
  return (new);
}

/* Allocate new pragmat_list */
pragmat_list init_pragmat_list (int room)
{ pragmat_list new = (pragmat_list) dcg_malloc (sizeof (struct str_pragmat_list));
  int safe_room = (room < 2)?2:room;
  new -> size = 0;
  new -> room = safe_room;
  new -> array = (pragmat *) dcg_calloc (safe_room, sizeof (pragmat));
  return (new);
}

/* Announce to use 'room' chunks for packet_list */
void room_packet_list (packet_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (packet));
  l -> room = room;
}

/* Announce to use 'room' chunks for node_list */
void room_node_list (node_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (node));
  l -> room = room;
}

/* Announce to use 'room' chunks for case_part_list */
void room_case_part_list (case_part_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (case_part));
  l -> room = room;
}

/* Announce to use 'room' chunks for decl_list */
void room_decl_list (decl_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (decl));
  l -> room = room;
}

/* Announce to use 'room' chunks for type_list */
void room_type_list (type_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (type));
  l -> room = room;
}

/* Announce to use 'room' chunks for pdecl_list */
void room_pdecl_list (pdecl_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (pdecl));
  l -> room = room;
}

/* Announce to use 'room' chunks for field_list */
void room_field_list (field_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (field));
  l -> room = room;
}

/* Announce to use 'room' chunks for type_info_list */
void room_type_info_list (type_info_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (type_info));
  l -> room = room;
}

/* Announce to use 'room' chunks for pragmat_list */
void room_pragmat_list (pragmat_list l, int room)
{ if (room <= l -> room) return;
  dcg_recalloc ((void **) &l -> array, room, sizeof (pragmat));
  l -> room = room;
}

/* Allocate new packet record */
packet new_packet (string fname, string pname, string_list pdefines, string_list puses, string_list puselibs, decl_list pdecls, node_list pcode)
{ packet new = (packet) dcg_malloc (sizeof (struct str_packet));
  new -> fname = fname;
  new -> pname = pname;
  new -> pdefines = pdefines;
  new -> puses = puses;
  new -> puselibs = puselibs;
  new -> pdecls = pdecls;
  new -> pcode = pcode;
  new -> pkdecls = decl_tree_nil;
  return (new);
};

/* Allocate new node record for constructor Init */
node new_Init (int line, int column,
		string name, node src)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGInit;
  new -> Init.name = name;
  new -> Init.id = decl_nil;
  new -> Init.src = src;
  return (new);
}

/* Allocate new node record for constructor Assign */
node new_Assign (int line, int column,
		node dest, node src)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGAssign;
  new -> Assign.dest = dest;
  new -> Assign.src = src;
  return (new);
}

/* Allocate new node record for constructor Cond */
node new_Cond (int line, int column,
		node enq, node_list thenp, node_list elsep)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGCond;
  new -> Cond.enq = enq;
  new -> Cond.thenp = thenp;
  new -> Cond.elsep = elsep;
  new -> Cond.typ = type_nil;
  return (new);
}

/* Allocate new node record for constructor Num */
node new_Num (int line, int column,
		node enq, case_part_list cps, node_list oth)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGNum;
  new -> Num.enq = enq;
  new -> Num.cps = cps;
  new -> Num.oth = oth;
  new -> Num.typ = type_nil;
  return (new);
}

/* Allocate new node record for constructor For */
node new_For (int line, int column,
		string lvar, int dir, node from, node to, node wenq, node_list body, node uenq)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGFor;
  new -> For.lvar = lvar;
  new -> For.id = decl_nil;
  new -> For.dir = dir;
  new -> For.from = from;
  new -> For.to = to;
  new -> For.wenq = wenq;
  new -> For.body = body;
  new -> For.uenq = uenq;
  return (new);
}

/* Allocate new node record for constructor While */
node new_While (int line, int column,
		node wenq, node_list body, node uenq)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGWhile;
  new -> While.wenq = wenq;
  new -> While.body = body;
  new -> While.uenq = uenq;
  return (new);
}

/* Allocate new node record for constructor Leave */
node new_Leave (int line, int column,
		string alg, node with)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGLeave;
  new -> Leave.alg = alg;
  new -> Leave.id = decl_nil;
  new -> Leave.with = with;
  return (new);
}

/* Allocate new node record for constructor Dyn_call */
node new_Dyn_call (int line, int column,
		node proc, node_list args)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGDyn_call;
  new -> Dyn_call.proc = proc;
  new -> Dyn_call.args = args;
  return (new);
}

/* Allocate new node record for constructor Call */
node new_Call (int line, int column,
		string pname, node_list args)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGCall;
  new -> Call.pname = pname;
  new -> Call.id = decl_nil;
  new -> Call.args = args;
  return (new);
}

/* Allocate new node record for constructor Display */
node new_Display (int line, int column,
		node_list elts)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGDisplay;
  new -> Display.elts = elts;
  new -> Display.typ = type_nil;
  return (new);
}

/* Allocate new node record for constructor Sub */
node new_Sub (int line, int column,
		node arr, node index)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGSub;
  new -> Sub.arr = arr;
  new -> Sub.index = index;
  new -> Sub.rtype = type_nil;
  return (new);
}

/* Allocate new node record for constructor Select */
node new_Select (int line, int column,
		node str, string sel)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGSelect;
  new -> Select.str = str;
  new -> Select.sel = sel;
  new -> Select.stype = type_nil;
  new -> Select.fld = field_nil;
  return (new);
}

/* Allocate new node record for constructor Abstr */
node new_Abstr (int line, int column,
		string tname, node arg)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGAbstr;
  new -> Abstr.tname = tname;
  new -> Abstr.id = decl_nil;
  new -> Abstr.arg = arg;
  return (new);
}

/* Allocate new node record for constructor Concr */
node new_Concr (int line, int column,
		node arg)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGConcr;
  new -> Concr.arg = arg;
  new -> Concr.id = decl_nil;
  return (new);
}

/* Allocate new node record for constructor Dyop */
node new_Dyop (int line, int column,
		string dop, node arg1, node arg2)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGDyop;
  new -> Dyop.dop = dop;
  new -> Dyop.id = decl_nil;
  new -> Dyop.arg1 = arg1;
  new -> Dyop.arg2 = arg2;
  return (new);
}

/* Allocate new node record for constructor Monop */
node new_Monop (int line, int column,
		string mop, node arg)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGMonop;
  new -> Monop.mop = mop;
  new -> Monop.id = decl_nil;
  new -> Monop.arg = arg;
  return (new);
}

/* Allocate new node record for constructor Ref_appl */
node new_Ref_appl (int line, int column,
		string name)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGRef_appl;
  new -> Ref_appl.name = name;
  new -> Ref_appl.id = decl_nil;
  return (new);
}

/* Allocate new node record for constructor Appl */
node new_Appl (int line, int column,
		string name)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGAppl;
  new -> Appl.name = name;
  new -> Appl.id = decl_nil;
  return (new);
}

/* Allocate new node record for constructor Nil */
node new_Nil (int line, int column)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGNil;
  new -> Nil.typ = type_nil;
  return (new);
}

/* Allocate new node record for constructor Denoter */
node new_Denoter (int line, int column,
		value val)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGDenoter;
  new -> Denoter.val = val;
  return (new);
}

/* Allocate new node record for constructor Skip */
node new_Skip (int line, int column)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGSkip;
  return (new);
}

/* Allocate new node record for constructor Default */
node new_Default (int line, int column)
{ node new = (node) dcg_malloc (sizeof (struct str_node));
  new -> line = line;
  new -> column = column;
  new -> tag = TAGDefault;
  return (new);
}

/* Allocate new case_part record */
case_part new_case_part (node_list lbls, node_list lcod)
{ case_part new = (case_part) dcg_malloc (sizeof (struct str_case_part));
  new -> lbls = lbls;
  new -> lcod = lcod;
  new -> els = int_list_nil;
  return (new);
};

/* Allocate new decl record for constructor Formal */
decl new_Formal (int line, int column, type typ, string name, pragmat_list prs,
		e_access acc)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGFormal;
  new -> Formal.acc = acc;
  new -> Formal.vnr = int_nil;
  return (new);
}

/* Allocate new decl record for constructor Object_decl */
decl new_Object_decl (int line, int column, type typ, string name, pragmat_list prs,
		e_access acc)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGObject_decl;
  new -> Object_decl.acc = acc;
  new -> Object_decl.vnr = int_nil;
  return (new);
}

/* Allocate new decl record for constructor Proc_decl */
decl new_Proc_decl (int line, int column, type typ, string name, pragmat_list prs,
		decl_list fpars, body bdy)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGProc_decl;
  new -> Proc_decl.fpars = fpars;
  new -> Proc_decl.bdy = bdy;
  return (new);
}

/* Allocate new decl record for constructor Op_decl */
decl new_Op_decl (int line, int column, type typ, string name, pragmat_list prs,
		decl_list fpars, body bdy)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGOp_decl;
  new -> Op_decl.fpars = fpars;
  new -> Op_decl.bdy = bdy;
  return (new);
}

/* Allocate new decl record for constructor Refinement */
decl new_Refinement (int line, int column, type typ, string name, pragmat_list prs,
		e_access acc, decl_list locals, node_list rcode)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGRefinement;
  new -> Refinement.acc = acc;
  new -> Refinement.locals = locals;
  new -> Refinement.rcode = rcode;
  new -> Refinement.stat = status_nil;
  new -> Refinement.trc = int_nil;
  return (new);
}

/* Allocate new decl record for constructor Synonym_value */
decl new_Synonym_value (int line, int column, type typ, string name, pragmat_list prs,
		node rhs)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGSynonym_value;
  new -> Synonym_value.rhs = rhs;
  new -> Synonym_value.val = value_nil;
  new -> Synonym_value.stat = status_nil;
  new -> Synonym_value.rec = int_nil;
  return (new);
}

/* Allocate new decl record for constructor Synonym_type */
decl new_Synonym_type (int line, int column, type typ, string name, pragmat_list prs)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGSynonym_type;
  new -> Synonym_type.stat = status_nil;
  new -> Synonym_type.rec = int_nil;
  return (new);
}

/* Allocate new decl record for constructor Type */
decl new_Type (int line, int column, type typ, string name, pragmat_list prs)
{ decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  new -> line = line;
  new -> column = column;
  new -> typ = typ;
  new -> name = name;
  new -> prs = prs;
  new -> tag = TAGType;
  new -> Type.stat = status_nil;
  new -> Type.rec = int_nil;
  new -> Type.concr = type_nil;
  return (new);
}

/* Allocate new body record for constructor Internal */
body new_Internal (string tname)
{ body new = (body) dcg_malloc (sizeof (struct str_body));
  new -> tag = TAGInternal;
  new -> Internal.tname = tname;
  return (new);
}

/* Allocate new body record for constructor External */
body new_External (string ename)
{ body new = (body) dcg_malloc (sizeof (struct str_body));
  new -> tag = TAGExternal;
  new -> External.ename = ename;
  return (new);
}

/* Allocate new body record for constructor Routine */
body new_Routine (decl_list locals, node_list rcode)
{ body new = (body) dcg_malloc (sizeof (struct str_body));
  new -> tag = TAGRoutine;
  new -> Routine.locals = locals;
  new -> Routine.rcode = rcode;
  new -> Routine.rdecls = decl_tree_nil;
  new -> Routine.pnr = int_nil;
  new -> Routine.trc = int_nil;
  return (new);
}

/* Allocate new body record for constructor Attach */
body new_Attach (type t)
{ body new = (body) dcg_malloc (sizeof (struct str_body));
  new -> tag = TAGAttach;
  new -> Attach.t = t;
  return (new);
}

/* Allocate new body record for constructor Detach */
body new_Detach (type t)
{ body new = (body) dcg_malloc (sizeof (struct str_body));
  new -> tag = TAGDetach;
  new -> Detach.t = t;
  return (new);
}

/* Allocate new body record for constructor Guard */
body new_Guard (type t)
{ body new = (body) dcg_malloc (sizeof (struct str_body));
  new -> tag = TAGGuard;
  new -> Guard.t = t;
  return (new);
}

/* Allocate new value record for constructor Ival */
value new_Ival (int i)
{ value new = (value) dcg_malloc (sizeof (struct str_value));
  new -> tag = TAGIval;
  new -> Ival.i = i;
  return (new);
}

/* Allocate new value record for constructor Bval */
value new_Bval (int b)
{ value new = (value) dcg_malloc (sizeof (struct str_value));
  new -> tag = TAGBval;
  new -> Bval.b = b;
  return (new);
}

/* Allocate new value record for constructor Rval */
value new_Rval (real r)
{ value new = (value) dcg_malloc (sizeof (struct str_value));
  new -> tag = TAGRval;
  new -> Rval.r = r;
  return (new);
}

/* Allocate new value record for constructor Tval */
value new_Tval (string t)
{ value new = (value) dcg_malloc (sizeof (struct str_value));
  new -> tag = TAGTval;
  new -> Tval.t = t;
  return (new);
}

/* Allocate new value record for constructor Errval */
value new_Errval ()
{ value new = (value) dcg_malloc (sizeof (struct str_value));
  new -> tag = TAGErrval;
  return (new);
}

/* Allocate new type record for constructor Unknown */
type new_Unknown ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGUnknown;
  return (new);
}

/* Allocate new type record for constructor Error */
type new_Error ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGError;
  return (new);
}

/* Allocate new type record for constructor Any_non_void */
type new_Any_non_void ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGAny_non_void;
  return (new);
}

/* Allocate new type record for constructor Any_row */
type new_Any_row ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGAny_row;
  return (new);
}

/* Allocate new type record for constructor Any_struct */
type new_Any_struct ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGAny_struct;
  return (new);
}

/* Allocate new type record for constructor Any_object */
type new_Any_object ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGAny_object;
  return (new);
}

/* Allocate new type record for constructor Addr */
type new_Addr ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGAddr;
  return (new);
}

/* Allocate new type record for constructor Int */
type new_Int ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGInt;
  return (new);
}

/* Allocate new type record for constructor Bool */
type new_Bool ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGBool;
  return (new);
}

/* Allocate new type record for constructor Real */
type new_Real ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGReal;
  return (new);
}

/* Allocate new type record for constructor Text */
type new_Text ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGText;
  return (new);
}

/* Allocate new type record for constructor Void */
type new_Void ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGVoid;
  return (new);
}

/* Allocate new type record for constructor File */
type new_File ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGFile;
  return (new);
}

/* Allocate new type record for constructor Niltype */
type new_Niltype ()
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGNiltype;
  return (new);
}

/* Allocate new type record for constructor Proto_row */
type new_Proto_row (node sz, type elt)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGProto_row;
  new -> Proto_row.sz = sz;
  new -> Proto_row.elt = elt;
  return (new);
}

/* Allocate new type record for constructor Row */
type new_Row (int sz, type elt)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGRow;
  new -> Row.sz = sz;
  new -> Row.elt = elt;
  return (new);
}

/* Allocate new type record for constructor Struct */
type new_Struct (field_list flds)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGStruct;
  new -> Struct.flds = flds;
  return (new);
}

/* Allocate new type record for constructor Union */
type new_Union (field_list uflds)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGUnion;
  new -> Union.uflds = uflds;
  return (new);
}

/* Allocate new type record for constructor Proc_type */
type new_Proc_type (pdecl_list parms, type rtype)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGProc_type;
  new -> Proc_type.parms = parms;
  new -> Proc_type.rtype = rtype;
  return (new);
}

/* Allocate new type record for constructor Tname */
type new_Tname (string tname)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGTname;
  new -> Tname.tname = tname;
  new -> Tname.id = decl_nil;
  return (new);
}

/* Allocate new type record for constructor Proto_tname */
type new_Proto_tname (int line, int column, string tname)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGProto_tname;
  new -> Proto_tname.line = line;
  new -> Proto_tname.column = column;
  new -> Proto_tname.tname = tname;
  return (new);
}

/* Allocate new type record for constructor Display_type */
type new_Display_type (type_list flds)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGDisplay_type;
  new -> Display_type.flds = flds;
  return (new);
}

/* Allocate new type record for constructor Unresolved */
type new_Unresolved (type_list typs)
{ type new = (type) dcg_malloc (sizeof (struct str_type));
  new -> tnr = int_nil;
  new -> tag = TAGUnresolved;
  new -> Unresolved.typs = typs;
  return (new);
}

/* Allocate new pdecl record */
pdecl new_pdecl (type ptype, e_access pacc)
{ pdecl new = (pdecl) dcg_malloc (sizeof (struct str_pdecl));
  new -> ptype = ptype;
  new -> pacc = pacc;
  return (new);
};

/* Allocate new field record */
field new_field (type ftype, string fname)
{ field new = (field) dcg_malloc (sizeof (struct str_field));
  new -> ftype = ftype;
  new -> fname = fname;
  new -> offset = int_nil;
  return (new);
};

/* Allocate new type_info record */
type_info new_type_info (string text, type typ)
{ type_info new = (type_info) dcg_malloc (sizeof (struct str_type_info));
  new -> text = text;
  new -> typ = typ;
  new -> attach_nr = int_nil;
  new -> detach_nr = int_nil;
  new -> guard_nr = int_nil;
  new -> stabs_nr = int_nil;
  return (new);
};

/* Recursively detach a packet */
void detach_packet (packet *optr)
{ packet old = (packet) dcg_predetach ((void **) optr);
  if (old == packet_nil) return;
  detach_string (&(old -> fname));
  detach_string (&(old -> pname));
  detach_string_list (&(old -> pdefines));
  detach_string_list (&(old -> puses));
  detach_string_list (&(old -> puselibs));
  detach_decl_list (&(old -> pdecls));
  detach_node_list (&(old -> pcode));
  dcg_detach ((void **) &old);
}

/* Recursively detach a node */
void detach_node (node *optr)
{ node old = (node) dcg_predetach ((void **) optr);
  if (old == node_nil) return;
  detach_int (&(old -> line));
  detach_int (&(old -> column));
  switch (old -> tag)
    { case TAGInit:
	detach_string (&(old -> Init.name));
	detach_node (&(old -> Init.src));
	break;
      case TAGAssign:
	detach_node (&(old -> Assign.dest));
	detach_node (&(old -> Assign.src));
	break;
      case TAGCond:
	detach_node (&(old -> Cond.enq));
	detach_node_list (&(old -> Cond.thenp));
	detach_node_list (&(old -> Cond.elsep));
	break;
      case TAGNum:
	detach_node (&(old -> Num.enq));
	detach_case_part_list (&(old -> Num.cps));
	detach_node_list (&(old -> Num.oth));
	break;
      case TAGFor:
	detach_string (&(old -> For.lvar));
	detach_int (&(old -> For.dir));
	detach_node (&(old -> For.from));
	detach_node (&(old -> For.to));
	detach_node (&(old -> For.wenq));
	detach_node_list (&(old -> For.body));
	detach_node (&(old -> For.uenq));
	break;
      case TAGWhile:
	detach_node (&(old -> While.wenq));
	detach_node_list (&(old -> While.body));
	detach_node (&(old -> While.uenq));
	break;
      case TAGLeave:
	detach_string (&(old -> Leave.alg));
	detach_node (&(old -> Leave.with));
	break;
      case TAGDyn_call:
	detach_node (&(old -> Dyn_call.proc));
	detach_node_list (&(old -> Dyn_call.args));
	break;
      case TAGCall:
	detach_string (&(old -> Call.pname));
	detach_node_list (&(old -> Call.args));
	break;
      case TAGDisplay:
	detach_node_list (&(old -> Display.elts));
	break;
      case TAGSub:
	detach_node (&(old -> Sub.arr));
	detach_node (&(old -> Sub.index));
	break;
      case TAGSelect:
	detach_node (&(old -> Select.str));
	detach_string (&(old -> Select.sel));
	break;
      case TAGAbstr:
	detach_string (&(old -> Abstr.tname));
	detach_node (&(old -> Abstr.arg));
	break;
      case TAGConcr:
	detach_node (&(old -> Concr.arg));
	break;
      case TAGDyop:
	detach_string (&(old -> Dyop.dop));
	detach_node (&(old -> Dyop.arg1));
	detach_node (&(old -> Dyop.arg2));
	break;
      case TAGMonop:
	detach_string (&(old -> Monop.mop));
	detach_node (&(old -> Monop.arg));
	break;
      case TAGRef_appl:
	detach_string (&(old -> Ref_appl.name));
	break;
      case TAGAppl:
	detach_string (&(old -> Appl.name));
	break;
      case TAGNil:
	break;
      case TAGDenoter:
	detach_value (&(old -> Denoter.val));
	break;
      case TAGSkip:
	break;
      case TAGDefault:
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_node");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a case_part */
void detach_case_part (case_part *optr)
{ case_part old = (case_part) dcg_predetach ((void **) optr);
  if (old == case_part_nil) return;
  detach_node_list (&(old -> lbls));
  detach_node_list (&(old -> lcod));
  dcg_detach ((void **) &old);
}

/* Recursively detach a decl */
void detach_decl (decl *optr)
{ decl old = (decl) dcg_predetach ((void **) optr);
  if (old == decl_nil) return;
  detach_int (&(old -> line));
  detach_int (&(old -> column));
  detach_type (&(old -> typ));
  detach_string (&(old -> name));
  detach_pragmat_list (&(old -> prs));
  switch (old -> tag)
    { case TAGFormal:
	detach_e_access (&(old -> Formal.acc));
	break;
      case TAGObject_decl:
	detach_e_access (&(old -> Object_decl.acc));
	break;
      case TAGProc_decl:
	detach_decl_list (&(old -> Proc_decl.fpars));
	detach_body (&(old -> Proc_decl.bdy));
	break;
      case TAGOp_decl:
	detach_decl_list (&(old -> Op_decl.fpars));
	detach_body (&(old -> Op_decl.bdy));
	break;
      case TAGRefinement:
	detach_e_access (&(old -> Refinement.acc));
	detach_decl_list (&(old -> Refinement.locals));
	detach_node_list (&(old -> Refinement.rcode));
	break;
      case TAGSynonym_value:
	detach_node (&(old -> Synonym_value.rhs));
	break;
      case TAGSynonym_type:
	break;
      case TAGType:
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_decl");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a body */
void detach_body (body *optr)
{ body old = (body) dcg_predetach ((void **) optr);
  if (old == body_nil) return;
  switch (old -> tag)
    { case TAGInternal:
	detach_string (&(old -> Internal.tname));
	break;
      case TAGExternal:
	detach_string (&(old -> External.ename));
	break;
      case TAGRoutine:
	detach_decl_list (&(old -> Routine.locals));
	detach_node_list (&(old -> Routine.rcode));
	break;
      case TAGAttach:
	detach_type (&(old -> Attach.t));
	break;
      case TAGDetach:
	detach_type (&(old -> Detach.t));
	break;
      case TAGGuard:
	detach_type (&(old -> Guard.t));
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_body");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a value */
void detach_value (value *optr)
{ value old = (value) dcg_predetach ((void **) optr);
  if (old == value_nil) return;
  switch (old -> tag)
    { case TAGIval:
	detach_int (&(old -> Ival.i));
	break;
      case TAGBval:
	detach_int (&(old -> Bval.b));
	break;
      case TAGRval:
	detach_real (&(old -> Rval.r));
	break;
      case TAGTval:
	detach_string (&(old -> Tval.t));
	break;
      case TAGErrval:
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_value");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a type */
void detach_type (type *optr)
{ type old = (type) dcg_predetach ((void **) optr);
  if (old == type_nil) return;
  switch (old -> tag)
    { case TAGUnknown:
	break;
      case TAGError:
	break;
      case TAGAny_non_void:
	break;
      case TAGAny_row:
	break;
      case TAGAny_struct:
	break;
      case TAGAny_object:
	break;
      case TAGAddr:
	break;
      case TAGInt:
	break;
      case TAGBool:
	break;
      case TAGReal:
	break;
      case TAGText:
	break;
      case TAGVoid:
	break;
      case TAGFile:
	break;
      case TAGNiltype:
	break;
      case TAGProto_row:
	detach_node (&(old -> Proto_row.sz));
	detach_type (&(old -> Proto_row.elt));
	break;
      case TAGRow:
	detach_int (&(old -> Row.sz));
	detach_type (&(old -> Row.elt));
	break;
      case TAGStruct:
	detach_field_list (&(old -> Struct.flds));
	break;
      case TAGUnion:
	detach_field_list (&(old -> Union.uflds));
	break;
      case TAGProc_type:
	detach_pdecl_list (&(old -> Proc_type.parms));
	detach_type (&(old -> Proc_type.rtype));
	break;
      case TAGTname:
	detach_string (&(old -> Tname.tname));
	break;
      case TAGProto_tname:
	detach_int (&(old -> Proto_tname.line));
	detach_int (&(old -> Proto_tname.column));
	detach_string (&(old -> Proto_tname.tname));
	break;
      case TAGDisplay_type:
	detach_type_list (&(old -> Display_type.flds));
	break;
      case TAGUnresolved:
	detach_type_list (&(old -> Unresolved.typs));
	break;
      default:
	dcg_bad_tag ((int) old -> tag, "detach_type");
    };
  dcg_detach ((void **) &old);
}

/* Recursively detach a pdecl */
void detach_pdecl (pdecl *optr)
{ pdecl old = (pdecl) dcg_predetach ((void **) optr);
  if (old == pdecl_nil) return;
  detach_type (&(old -> ptype));
  detach_e_access (&(old -> pacc));
  dcg_detach ((void **) &old);
}

/* Recursively detach a field */
void detach_field (field *optr)
{ field old = (field) dcg_predetach ((void **) optr);
  if (old == field_nil) return;
  detach_type (&(old -> ftype));
  detach_string (&(old -> fname));
  dcg_detach ((void **) &old);
}

/* Recursively detach a type_info */
void detach_type_info (type_info *optr)
{ type_info old = (type_info) dcg_predetach ((void **) optr);
  if (old == type_info_nil) return;
  detach_string (&(old -> text));
  detach_type (&(old -> typ));
  dcg_detach ((void **) &old);
}

/* Recursively detach a packet_list */
void detach_packet_list (packet_list *lp)
{ int ix;
  packet_list old = (packet_list) dcg_predetach ((void **) lp);
  if (old == packet_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_packet (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a node_list */
void detach_node_list (node_list *lp)
{ int ix;
  node_list old = (node_list) dcg_predetach ((void **) lp);
  if (old == node_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_node (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a case_part_list */
void detach_case_part_list (case_part_list *lp)
{ int ix;
  case_part_list old = (case_part_list) dcg_predetach ((void **) lp);
  if (old == case_part_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_case_part (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a decl_list */
void detach_decl_list (decl_list *lp)
{ int ix;
  decl_list old = (decl_list) dcg_predetach ((void **) lp);
  if (old == decl_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_decl (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a type_list */
void detach_type_list (type_list *lp)
{ int ix;
  type_list old = (type_list) dcg_predetach ((void **) lp);
  if (old == type_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_type (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a pdecl_list */
void detach_pdecl_list (pdecl_list *lp)
{ int ix;
  pdecl_list old = (pdecl_list) dcg_predetach ((void **) lp);
  if (old == pdecl_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_pdecl (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a field_list */
void detach_field_list (field_list *lp)
{ int ix;
  field_list old = (field_list) dcg_predetach ((void **) lp);
  if (old == field_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_field (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a type_info_list */
void detach_type_info_list (type_info_list *lp)
{ int ix;
  type_info_list old = (type_info_list) dcg_predetach ((void **) lp);
  if (old == type_info_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_type_info (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Recursively detach a pragmat_list */
void detach_pragmat_list (pragmat_list *lp)
{ int ix;
  pragmat_list old = (pragmat_list) dcg_predetach ((void **) lp);
  if (old == pragmat_list_nil) return;
  for (ix = 0; ix < old -> size; ix++)
    detach_pragmat (&(old -> array[ix]));
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a packet_list without detaching elements */
void nonrec_detach_packet_list (packet_list *lp)
{ packet_list old = (packet_list) dcg_predetach ((void **) lp);
  if (old == packet_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a node_list without detaching elements */
void nonrec_detach_node_list (node_list *lp)
{ node_list old = (node_list) dcg_predetach ((void **) lp);
  if (old == node_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a case_part_list without detaching elements */
void nonrec_detach_case_part_list (case_part_list *lp)
{ case_part_list old = (case_part_list) dcg_predetach ((void **) lp);
  if (old == case_part_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a decl_list without detaching elements */
void nonrec_detach_decl_list (decl_list *lp)
{ decl_list old = (decl_list) dcg_predetach ((void **) lp);
  if (old == decl_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a type_list without detaching elements */
void nonrec_detach_type_list (type_list *lp)
{ type_list old = (type_list) dcg_predetach ((void **) lp);
  if (old == type_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a pdecl_list without detaching elements */
void nonrec_detach_pdecl_list (pdecl_list *lp)
{ pdecl_list old = (pdecl_list) dcg_predetach ((void **) lp);
  if (old == pdecl_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a field_list without detaching elements */
void nonrec_detach_field_list (field_list *lp)
{ field_list old = (field_list) dcg_predetach ((void **) lp);
  if (old == field_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a type_info_list without detaching elements */
void nonrec_detach_type_info_list (type_info_list *lp)
{ type_info_list old = (type_info_list) dcg_predetach ((void **) lp);
  if (old == type_info_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Detach a pragmat_list without detaching elements */
void nonrec_detach_pragmat_list (pragmat_list *lp)
{ pragmat_list old = (pragmat_list) dcg_predetach ((void **) lp);
  if (old == pragmat_list_nil) return;
  dcg_detach ((void **) &(old -> array));
  dcg_detach ((void **) &old);
}

/* Append element to packet_list */
packet_list append_packet_list (packet_list l, packet el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_packet_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to node_list */
node_list append_node_list (node_list l, node el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_node_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to case_part_list */
case_part_list append_case_part_list (case_part_list l, case_part el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_case_part_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to decl_list */
decl_list append_decl_list (decl_list l, decl el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_decl_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to type_list */
type_list append_type_list (type_list l, type el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_type_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to pdecl_list */
pdecl_list append_pdecl_list (pdecl_list l, pdecl el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_pdecl_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to field_list */
field_list append_field_list (field_list l, field el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_field_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to type_info_list */
type_info_list append_type_info_list (type_info_list l, type_info el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_type_info_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Append element to pragmat_list */
pragmat_list append_pragmat_list (pragmat_list l, pragmat el)
{ dcg_cknonshared ((char *) l);
  if (l -> size == l -> room)
    room_pragmat_list (l, l -> size << 1);
  l -> array[l -> size] = el;
  l -> size++;
  return (l);
}

/* Concatenate two packet_lists */
packet_list concat_packet_list (packet_list l1, packet_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_packet_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two node_lists */
node_list concat_node_list (node_list l1, node_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_node_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two case_part_lists */
case_part_list concat_case_part_list (case_part_list l1, case_part_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_case_part_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two decl_lists */
decl_list concat_decl_list (decl_list l1, decl_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_decl_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two type_lists */
type_list concat_type_list (type_list l1, type_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_type_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two pdecl_lists */
pdecl_list concat_pdecl_list (pdecl_list l1, pdecl_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_pdecl_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two field_lists */
field_list concat_field_list (field_list l1, field_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_field_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two type_info_lists */
type_info_list concat_type_info_list (type_info_list l1, type_info_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_type_info_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Concatenate two pragmat_lists */
pragmat_list concat_pragmat_list (pragmat_list l1, pragmat_list l2)
{ int ix;
  dcg_cknonshared ((char *) l1);
  room_pragmat_list (l1, l1 -> size + l2 -> size);
  for (ix = 0; ix < l2 -> size; ix++)
    l1 -> array[l1 -> size + ix] = l2 -> array[ix];
  l1 -> size += l2 -> size;
  return (l1);
}

/* Insert element into packet_list at pos 'pos' */
packet_list insert_packet_list (packet_list l, int pos, packet el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_packet_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_packet_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into node_list at pos 'pos' */
node_list insert_node_list (node_list l, int pos, node el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_node_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_node_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into case_part_list at pos 'pos' */
case_part_list insert_case_part_list (case_part_list l, int pos, case_part el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_case_part_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_case_part_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into decl_list at pos 'pos' */
decl_list insert_decl_list (decl_list l, int pos, decl el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_decl_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_decl_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into type_list at pos 'pos' */
type_list insert_type_list (type_list l, int pos, type el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_type_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_type_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into pdecl_list at pos 'pos' */
pdecl_list insert_pdecl_list (pdecl_list l, int pos, pdecl el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_pdecl_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_pdecl_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into field_list at pos 'pos' */
field_list insert_field_list (field_list l, int pos, field el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_field_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_field_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into type_info_list at pos 'pos' */
type_info_list insert_type_info_list (type_info_list l, int pos, type_info el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_type_info_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_type_info_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Insert element into pragmat_list at pos 'pos' */
pragmat_list insert_pragmat_list (pragmat_list l, int pos, pragmat el)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos > l -> size))
    dcg_panic ("insert_pragmat_list: position %d out of range", pos);
  if (l -> size == l -> room)
    room_pragmat_list (l, l -> size << 1);
  for (ix = l -> size; pos < ix; ix--)
    l -> array[ix] = l -> array[ix-1];
  l -> array[pos] = el;
  l -> size++;
  return (l);
}

/* Delete element from packet_list at pos 'pos' */
packet_list delete_packet_list (packet_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_packet_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from node_list at pos 'pos' */
node_list delete_node_list (node_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_node_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from case_part_list at pos 'pos' */
case_part_list delete_case_part_list (case_part_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_case_part_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from decl_list at pos 'pos' */
decl_list delete_decl_list (decl_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_decl_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from type_list at pos 'pos' */
type_list delete_type_list (type_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_type_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from pdecl_list at pos 'pos' */
pdecl_list delete_pdecl_list (pdecl_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_pdecl_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from field_list at pos 'pos' */
field_list delete_field_list (field_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_field_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from type_info_list at pos 'pos' */
type_info_list delete_type_info_list (type_info_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_type_info_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Delete element from pragmat_list at pos 'pos' */
pragmat_list delete_pragmat_list (pragmat_list l, int pos)
{ int ix;
  dcg_cknonshared ((char *) l);
  if ((pos < 0) || (pos >= l -> size))
    dcg_panic ("delete_pragmat_list: position %d out of range", pos);
  for (ix = pos; ix < l -> size - 1; ix++)
    l -> array[ix] = l -> array[ix+1];
  l -> size--;
  return (l);
}

/* Comparison test for a packet */
int cmp_packet (packet a, packet b)
{ int s;
  if (a == b) return (0);
  if (a == packet_nil) return (-1);
  if (b == packet_nil) return (1);
  if ((s = cmp_string (a -> fname, b -> fname))) return (s);
  if ((s = cmp_string (a -> pname, b -> pname))) return (s);
  if ((s = cmp_string_list (a -> pdefines, b -> pdefines))) return (s);
  if ((s = cmp_string_list (a -> puses, b -> puses))) return (s);
  if ((s = cmp_string_list (a -> puselibs, b -> puselibs))) return (s);
  if ((s = cmp_decl_list (a -> pdecls, b -> pdecls))) return (s);
  if ((s = cmp_node_list (a -> pcode, b -> pcode))) return (s);
  return (0);
}

/* Comparison test for a node */
int cmp_node (node a, node b)
{ int s;
  if (a == b) return (0);
  if (a == node_nil) return (-1);
  if (b == node_nil) return (1);
  if ((s = cmp_int (a -> line, b -> line))) return (s);
  if ((s = cmp_int (a -> column, b -> column))) return (s);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGInit:
	if ((s = cmp_string (a -> Init.name, b -> Init.name))) return (s);
	if ((s = cmp_node (a -> Init.src, b -> Init.src))) return (s);
	break;
      case TAGAssign:
	if ((s = cmp_node (a -> Assign.dest, b -> Assign.dest))) return (s);
	if ((s = cmp_node (a -> Assign.src, b -> Assign.src))) return (s);
	break;
      case TAGCond:
	if ((s = cmp_node (a -> Cond.enq, b -> Cond.enq))) return (s);
	if ((s = cmp_node_list (a -> Cond.thenp, b -> Cond.thenp))) return (s);
	if ((s = cmp_node_list (a -> Cond.elsep, b -> Cond.elsep))) return (s);
	break;
      case TAGNum:
	if ((s = cmp_node (a -> Num.enq, b -> Num.enq))) return (s);
	if ((s = cmp_case_part_list (a -> Num.cps, b -> Num.cps))) return (s);
	if ((s = cmp_node_list (a -> Num.oth, b -> Num.oth))) return (s);
	break;
      case TAGFor:
	if ((s = cmp_string (a -> For.lvar, b -> For.lvar))) return (s);
	if ((s = cmp_int (a -> For.dir, b -> For.dir))) return (s);
	if ((s = cmp_node (a -> For.from, b -> For.from))) return (s);
	if ((s = cmp_node (a -> For.to, b -> For.to))) return (s);
	if ((s = cmp_node (a -> For.wenq, b -> For.wenq))) return (s);
	if ((s = cmp_node_list (a -> For.body, b -> For.body))) return (s);
	if ((s = cmp_node (a -> For.uenq, b -> For.uenq))) return (s);
	break;
      case TAGWhile:
	if ((s = cmp_node (a -> While.wenq, b -> While.wenq))) return (s);
	if ((s = cmp_node_list (a -> While.body, b -> While.body))) return (s);
	if ((s = cmp_node (a -> While.uenq, b -> While.uenq))) return (s);
	break;
      case TAGLeave:
	if ((s = cmp_string (a -> Leave.alg, b -> Leave.alg))) return (s);
	if ((s = cmp_node (a -> Leave.with, b -> Leave.with))) return (s);
	break;
      case TAGDyn_call:
	if ((s = cmp_node (a -> Dyn_call.proc, b -> Dyn_call.proc))) return (s);
	if ((s = cmp_node_list (a -> Dyn_call.args, b -> Dyn_call.args))) return (s);
	break;
      case TAGCall:
	if ((s = cmp_string (a -> Call.pname, b -> Call.pname))) return (s);
	if ((s = cmp_node_list (a -> Call.args, b -> Call.args))) return (s);
	break;
      case TAGDisplay:
	if ((s = cmp_node_list (a -> Display.elts, b -> Display.elts))) return (s);
	break;
      case TAGSub:
	if ((s = cmp_node (a -> Sub.arr, b -> Sub.arr))) return (s);
	if ((s = cmp_node (a -> Sub.index, b -> Sub.index))) return (s);
	break;
      case TAGSelect:
	if ((s = cmp_node (a -> Select.str, b -> Select.str))) return (s);
	if ((s = cmp_string (a -> Select.sel, b -> Select.sel))) return (s);
	break;
      case TAGAbstr:
	if ((s = cmp_string (a -> Abstr.tname, b -> Abstr.tname))) return (s);
	if ((s = cmp_node (a -> Abstr.arg, b -> Abstr.arg))) return (s);
	break;
      case TAGConcr:
	if ((s = cmp_node (a -> Concr.arg, b -> Concr.arg))) return (s);
	break;
      case TAGDyop:
	if ((s = cmp_string (a -> Dyop.dop, b -> Dyop.dop))) return (s);
	if ((s = cmp_node (a -> Dyop.arg1, b -> Dyop.arg1))) return (s);
	if ((s = cmp_node (a -> Dyop.arg2, b -> Dyop.arg2))) return (s);
	break;
      case TAGMonop:
	if ((s = cmp_string (a -> Monop.mop, b -> Monop.mop))) return (s);
	if ((s = cmp_node (a -> Monop.arg, b -> Monop.arg))) return (s);
	break;
      case TAGRef_appl:
	if ((s = cmp_string (a -> Ref_appl.name, b -> Ref_appl.name))) return (s);
	break;
      case TAGAppl:
	if ((s = cmp_string (a -> Appl.name, b -> Appl.name))) return (s);
	break;
      case TAGNil:
	break;
      case TAGDenoter:
	if ((s = cmp_value (a -> Denoter.val, b -> Denoter.val))) return (s);
	break;
      case TAGSkip:
	break;
      case TAGDefault:
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_node");
    };
  return (0);
}

/* Comparison test for a case_part */
int cmp_case_part (case_part a, case_part b)
{ int s;
  if (a == b) return (0);
  if (a == case_part_nil) return (-1);
  if (b == case_part_nil) return (1);
  if ((s = cmp_node_list (a -> lbls, b -> lbls))) return (s);
  if ((s = cmp_node_list (a -> lcod, b -> lcod))) return (s);
  return (0);
}

/* Comparison test for a decl */
int cmp_decl (decl a, decl b)
{ int s;
  if (a == b) return (0);
  if (a == decl_nil) return (-1);
  if (b == decl_nil) return (1);
  if ((s = cmp_int (a -> line, b -> line))) return (s);
  if ((s = cmp_int (a -> column, b -> column))) return (s);
  if ((s = cmp_type (a -> typ, b -> typ))) return (s);
  if ((s = cmp_string (a -> name, b -> name))) return (s);
  if ((s = cmp_pragmat_list (a -> prs, b -> prs))) return (s);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGFormal:
	if ((s = cmp_e_access (a -> Formal.acc, b -> Formal.acc))) return (s);
	break;
      case TAGObject_decl:
	if ((s = cmp_e_access (a -> Object_decl.acc, b -> Object_decl.acc))) return (s);
	break;
      case TAGProc_decl:
	if ((s = cmp_decl_list (a -> Proc_decl.fpars, b -> Proc_decl.fpars))) return (s);
	if ((s = cmp_body (a -> Proc_decl.bdy, b -> Proc_decl.bdy))) return (s);
	break;
      case TAGOp_decl:
	if ((s = cmp_decl_list (a -> Op_decl.fpars, b -> Op_decl.fpars))) return (s);
	if ((s = cmp_body (a -> Op_decl.bdy, b -> Op_decl.bdy))) return (s);
	break;
      case TAGRefinement:
	if ((s = cmp_e_access (a -> Refinement.acc, b -> Refinement.acc))) return (s);
	if ((s = cmp_decl_list (a -> Refinement.locals, b -> Refinement.locals))) return (s);
	if ((s = cmp_node_list (a -> Refinement.rcode, b -> Refinement.rcode))) return (s);
	break;
      case TAGSynonym_value:
	if ((s = cmp_node (a -> Synonym_value.rhs, b -> Synonym_value.rhs))) return (s);
	break;
      case TAGSynonym_type:
	break;
      case TAGType:
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_decl");
    };
  return (0);
}

/* Comparison test for a body */
int cmp_body (body a, body b)
{ int s;
  if (a == b) return (0);
  if (a == body_nil) return (-1);
  if (b == body_nil) return (1);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGInternal:
	if ((s = cmp_string (a -> Internal.tname, b -> Internal.tname))) return (s);
	break;
      case TAGExternal:
	if ((s = cmp_string (a -> External.ename, b -> External.ename))) return (s);
	break;
      case TAGRoutine:
	if ((s = cmp_decl_list (a -> Routine.locals, b -> Routine.locals))) return (s);
	if ((s = cmp_node_list (a -> Routine.rcode, b -> Routine.rcode))) return (s);
	break;
      case TAGAttach:
	if ((s = cmp_type (a -> Attach.t, b -> Attach.t))) return (s);
	break;
      case TAGDetach:
	if ((s = cmp_type (a -> Detach.t, b -> Detach.t))) return (s);
	break;
      case TAGGuard:
	if ((s = cmp_type (a -> Guard.t, b -> Guard.t))) return (s);
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_body");
    };
  return (0);
}

/* Comparison test for a value */
int cmp_value (value a, value b)
{ int s;
  if (a == b) return (0);
  if (a == value_nil) return (-1);
  if (b == value_nil) return (1);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGIval:
	if ((s = cmp_int (a -> Ival.i, b -> Ival.i))) return (s);
	break;
      case TAGBval:
	if ((s = cmp_int (a -> Bval.b, b -> Bval.b))) return (s);
	break;
      case TAGRval:
	if ((s = cmp_real (a -> Rval.r, b -> Rval.r))) return (s);
	break;
      case TAGTval:
	if ((s = cmp_string (a -> Tval.t, b -> Tval.t))) return (s);
	break;
      case TAGErrval:
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_value");
    };
  return (0);
}

/* Comparison test for a type */
int cmp_type (type a, type b)
{ int s;
  if (a == b) return (0);
  if (a == type_nil) return (-1);
  if (b == type_nil) return (1);
  if ((s = cmp_int ((int)(a -> tag), (int)(b -> tag)))) return (s);
  switch (a -> tag)
    { case TAGUnknown:
	break;
      case TAGError:
	break;
      case TAGAny_non_void:
	break;
      case TAGAny_row:
	break;
      case TAGAny_struct:
	break;
      case TAGAny_object:
	break;
      case TAGAddr:
	break;
      case TAGInt:
	break;
      case TAGBool:
	break;
      case TAGReal:
	break;
      case TAGText:
	break;
      case TAGVoid:
	break;
      case TAGFile:
	break;
      case TAGNiltype:
	break;
      case TAGProto_row:
	if ((s = cmp_node (a -> Proto_row.sz, b -> Proto_row.sz))) return (s);
	if ((s = cmp_type (a -> Proto_row.elt, b -> Proto_row.elt))) return (s);
	break;
      case TAGRow:
	if ((s = cmp_int (a -> Row.sz, b -> Row.sz))) return (s);
	if ((s = cmp_type (a -> Row.elt, b -> Row.elt))) return (s);
	break;
      case TAGStruct:
	if ((s = cmp_field_list (a -> Struct.flds, b -> Struct.flds))) return (s);
	break;
      case TAGUnion:
	if ((s = cmp_field_list (a -> Union.uflds, b -> Union.uflds))) return (s);
	break;
      case TAGProc_type:
	if ((s = cmp_pdecl_list (a -> Proc_type.parms, b -> Proc_type.parms))) return (s);
	if ((s = cmp_type (a -> Proc_type.rtype, b -> Proc_type.rtype))) return (s);
	break;
      case TAGTname:
	if ((s = cmp_string (a -> Tname.tname, b -> Tname.tname))) return (s);
	break;
      case TAGProto_tname:
	if ((s = cmp_int (a -> Proto_tname.line, b -> Proto_tname.line))) return (s);
	if ((s = cmp_int (a -> Proto_tname.column, b -> Proto_tname.column))) return (s);
	if ((s = cmp_string (a -> Proto_tname.tname, b -> Proto_tname.tname))) return (s);
	break;
      case TAGDisplay_type:
	if ((s = cmp_type_list (a -> Display_type.flds, b -> Display_type.flds))) return (s);
	break;
      case TAGUnresolved:
	if ((s = cmp_type_list (a -> Unresolved.typs, b -> Unresolved.typs))) return (s);
	break;
      default:
	dcg_bad_tag (a -> tag, "cmp_type");
    };
  return (0);
}

/* Comparison test for a pdecl */
int cmp_pdecl (pdecl a, pdecl b)
{ int s;
  if (a == b) return (0);
  if (a == pdecl_nil) return (-1);
  if (b == pdecl_nil) return (1);
  if ((s = cmp_type (a -> ptype, b -> ptype))) return (s);
  if ((s = cmp_e_access (a -> pacc, b -> pacc))) return (s);
  return (0);
}

/* Comparison test for a field */
int cmp_field (field a, field b)
{ int s;
  if (a == b) return (0);
  if (a == field_nil) return (-1);
  if (b == field_nil) return (1);
  if ((s = cmp_type (a -> ftype, b -> ftype))) return (s);
  if ((s = cmp_string (a -> fname, b -> fname))) return (s);
  return (0);
}

/* Comparison test for a type_info */
int cmp_type_info (type_info a, type_info b)
{ int s;
  if (a == b) return (0);
  if (a == type_info_nil) return (-1);
  if (b == type_info_nil) return (1);
  if ((s = cmp_string (a -> text, b -> text))) return (s);
  if ((s = cmp_type (a -> typ, b -> typ))) return (s);
  return (0);
}

/* Comparison test for a packet_list */
int cmp_packet_list (packet_list a, packet_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == packet_list_nil) return (-1);
  if (b == packet_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_packet (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a node_list */
int cmp_node_list (node_list a, node_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == node_list_nil) return (-1);
  if (b == node_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_node (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a case_part_list */
int cmp_case_part_list (case_part_list a, case_part_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == case_part_list_nil) return (-1);
  if (b == case_part_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_case_part (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a decl_list */
int cmp_decl_list (decl_list a, decl_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == decl_list_nil) return (-1);
  if (b == decl_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_decl (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a type_list */
int cmp_type_list (type_list a, type_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == type_list_nil) return (-1);
  if (b == type_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_type (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a pdecl_list */
int cmp_pdecl_list (pdecl_list a, pdecl_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == pdecl_list_nil) return (-1);
  if (b == pdecl_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_pdecl (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a field_list */
int cmp_field_list (field_list a, field_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == field_list_nil) return (-1);
  if (b == field_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_field (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a type_info_list */
int cmp_type_info_list (type_info_list a, type_info_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == type_info_list_nil) return (-1);
  if (b == type_info_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_type_info (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Comparison test for a pragmat_list */
int cmp_pragmat_list (pragmat_list a, pragmat_list b)
{ int ix, stat, size;
  if (a == b) return (0);
  if (a == pragmat_list_nil) return (-1);
  if (b == pragmat_list_nil) return (1);
  size = a -> size;
  if (b -> size < size) size = b -> size;
  for (ix = 0; ix < size; ix++)
    if ((stat = cmp_pragmat (a -> array[ix], b -> array[ix])))
      return (stat);
  return (cmp_int (a -> size, b -> size));
}

/* Estimate printing a packet */
int est_packet (packet old)
{ int size = 0;
  if (old == packet_nil) return (2);
  size += est_string (old -> fname) + 2;
  size += est_string (old -> pname) + 2;
  size += est_string_list (old -> pdefines) + 2;
  size += est_string_list (old -> puses) + 2;
  size += est_string_list (old -> puselibs) + 2;
  size += est_decl_list (old -> pdecls) + 2;
  size += est_node_list (old -> pcode) + 2;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate printing a node */
int est_node (node old)
{ int size = 0;
  if (old == node_nil) return (2);
  size += est_int (old -> line) + 2;
  size += est_int (old -> column) + 2;
  switch (old -> tag)
    { case TAGInit:
	size += 6;
	size += est_string (old -> Init.name) + 2;
	size += PTRWIDTH;
	size += est_node (old -> Init.src) + 2;
	break;
      case TAGAssign:
	size += 8;
	size += est_node (old -> Assign.dest) + 2;
	size += est_node (old -> Assign.src) + 2;
	break;
      case TAGCond:
	size += 6;
	size += est_node (old -> Cond.enq) + 2;
	size += est_node_list (old -> Cond.thenp) + 2;
	size += est_node_list (old -> Cond.elsep) + 2;
	size += PTRWIDTH;
	break;
      case TAGNum:
	size += 5;
	size += est_node (old -> Num.enq) + 2;
	size += est_case_part_list (old -> Num.cps) + 2;
	size += est_node_list (old -> Num.oth) + 2;
	size += PTRWIDTH;
	break;
      case TAGFor:
	size += 5;
	size += est_string (old -> For.lvar) + 2;
	size += PTRWIDTH;
	size += est_int (old -> For.dir) + 2;
	size += est_node (old -> For.from) + 2;
	size += est_node (old -> For.to) + 2;
	size += est_node (old -> For.wenq) + 2;
	size += est_node_list (old -> For.body) + 2;
	size += est_node (old -> For.uenq) + 2;
	break;
      case TAGWhile:
	size += 7;
	size += est_node (old -> While.wenq) + 2;
	size += est_node_list (old -> While.body) + 2;
	size += est_node (old -> While.uenq) + 2;
	break;
      case TAGLeave:
	size += 7;
	size += est_string (old -> Leave.alg) + 2;
	size += PTRWIDTH;
	size += est_node (old -> Leave.with) + 2;
	break;
      case TAGDyn_call:
	size += 10;
	size += est_node (old -> Dyn_call.proc) + 2;
	size += est_node_list (old -> Dyn_call.args) + 2;
	break;
      case TAGCall:
	size += 6;
	size += est_string (old -> Call.pname) + 2;
	size += PTRWIDTH;
	size += est_node_list (old -> Call.args) + 2;
	break;
      case TAGDisplay:
	size += 9;
	size += est_node_list (old -> Display.elts) + 2;
	size += PTRWIDTH;
	break;
      case TAGSub:
	size += 5;
	size += est_node (old -> Sub.arr) + 2;
	size += est_node (old -> Sub.index) + 2;
	size += PTRWIDTH;
	break;
      case TAGSelect:
	size += 8;
	size += est_node (old -> Select.str) + 2;
	size += est_string (old -> Select.sel) + 2;
	size += PTRWIDTH;
	size += PTRWIDTH;
	break;
      case TAGAbstr:
	size += 7;
	size += est_string (old -> Abstr.tname) + 2;
	size += PTRWIDTH;
	size += est_node (old -> Abstr.arg) + 2;
	break;
      case TAGConcr:
	size += 7;
	size += est_node (old -> Concr.arg) + 2;
	size += PTRWIDTH;
	break;
      case TAGDyop:
	size += 6;
	size += est_string (old -> Dyop.dop) + 2;
	size += PTRWIDTH;
	size += est_node (old -> Dyop.arg1) + 2;
	size += est_node (old -> Dyop.arg2) + 2;
	break;
      case TAGMonop:
	size += 7;
	size += est_string (old -> Monop.mop) + 2;
	size += PTRWIDTH;
	size += est_node (old -> Monop.arg) + 2;
	break;
      case TAGRef_appl:
	size += 10;
	size += est_string (old -> Ref_appl.name) + 2;
	size += PTRWIDTH;
	break;
      case TAGAppl:
	size += 6;
	size += est_string (old -> Appl.name) + 2;
	size += PTRWIDTH;
	break;
      case TAGNil:
	size += 5;
	size += PTRWIDTH;
	break;
      case TAGDenoter:
	size += 9;
	size += est_value (old -> Denoter.val) + 2;
	break;
      case TAGSkip:
	size += 6;
	break;
      case TAGDefault:
	size += 9;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_node");
    };
  return (size + 2);
}

/* Estimate printing a case_part */
int est_case_part (case_part old)
{ int size = 0;
  if (old == case_part_nil) return (2);
  size += est_node_list (old -> lbls) + 2;
  size += est_node_list (old -> lcod) + 2;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate printing a decl */
int est_decl (decl old)
{ int size = 0;
  if (old == decl_nil) return (2);
  size += est_int (old -> line) + 2;
  size += est_int (old -> column) + 2;
  size += est_type (old -> typ) + 2;
  size += est_string (old -> name) + 2;
  size += est_pragmat_list (old -> prs) + 2;
  switch (old -> tag)
    { case TAGFormal:
	size += 8;
	size += est_e_access (old -> Formal.acc) + 2;
	size += PTRWIDTH;
	break;
      case TAGObject_decl:
	size += 13;
	size += est_e_access (old -> Object_decl.acc) + 2;
	size += PTRWIDTH;
	break;
      case TAGProc_decl:
	size += 11;
	size += est_decl_list (old -> Proc_decl.fpars) + 2;
	size += est_body (old -> Proc_decl.bdy) + 2;
	break;
      case TAGOp_decl:
	size += 9;
	size += est_decl_list (old -> Op_decl.fpars) + 2;
	size += est_body (old -> Op_decl.bdy) + 2;
	break;
      case TAGRefinement:
	size += 12;
	size += est_e_access (old -> Refinement.acc) + 2;
	size += est_decl_list (old -> Refinement.locals) + 2;
	size += est_node_list (old -> Refinement.rcode) + 2;
	size += PTRWIDTH;
	size += PTRWIDTH;
	break;
      case TAGSynonym_value:
	size += 15;
	size += est_node (old -> Synonym_value.rhs) + 2;
	size += PTRWIDTH;
	size += PTRWIDTH;
	size += PTRWIDTH;
	break;
      case TAGSynonym_type:
	size += 14;
	size += PTRWIDTH;
	size += PTRWIDTH;
	break;
      case TAGType:
	size += 6;
	size += PTRWIDTH;
	size += PTRWIDTH;
	size += PTRWIDTH;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_decl");
    };
  return (size + 2);
}

/* Estimate printing a body */
int est_body (body old)
{ int size = 0;
  if (old == body_nil) return (2);
  switch (old -> tag)
    { case TAGInternal:
	size += 10;
	size += est_string (old -> Internal.tname) + 2;
	break;
      case TAGExternal:
	size += 10;
	size += est_string (old -> External.ename) + 2;
	break;
      case TAGRoutine:
	size += 9;
	size += est_decl_list (old -> Routine.locals) + 2;
	size += est_node_list (old -> Routine.rcode) + 2;
	size += PTRWIDTH;
	size += PTRWIDTH;
	size += PTRWIDTH;
	break;
      case TAGAttach:
	size += 8;
	size += est_type (old -> Attach.t) + 2;
	break;
      case TAGDetach:
	size += 8;
	size += est_type (old -> Detach.t) + 2;
	break;
      case TAGGuard:
	size += 7;
	size += est_type (old -> Guard.t) + 2;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_body");
    };
  return (size + 2);
}

/* Estimate printing a value */
int est_value (value old)
{ int size = 0;
  if (old == value_nil) return (2);
  switch (old -> tag)
    { case TAGIval:
	size += 6;
	size += est_int (old -> Ival.i) + 2;
	break;
      case TAGBval:
	size += 6;
	size += est_int (old -> Bval.b) + 2;
	break;
      case TAGRval:
	size += 6;
	size += est_real (old -> Rval.r) + 2;
	break;
      case TAGTval:
	size += 6;
	size += est_string (old -> Tval.t) + 2;
	break;
      case TAGErrval:
	size += 8;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_value");
    };
  return (size + 2);
}

/* Estimate printing a type */
int est_type (type old)
{ int size = 0;
  if (old == type_nil) return (2);
  size += PTRWIDTH;
  switch (old -> tag)
    { case TAGUnknown:
	size += 9;
	break;
      case TAGError:
	size += 7;
	break;
      case TAGAny_non_void:
	size += 14;
	break;
      case TAGAny_row:
	size += 9;
	break;
      case TAGAny_struct:
	size += 12;
	break;
      case TAGAny_object:
	size += 12;
	break;
      case TAGAddr:
	size += 6;
	break;
      case TAGInt:
	size += 5;
	break;
      case TAGBool:
	size += 6;
	break;
      case TAGReal:
	size += 6;
	break;
      case TAGText:
	size += 6;
	break;
      case TAGVoid:
	size += 6;
	break;
      case TAGFile:
	size += 6;
	break;
      case TAGNiltype:
	size += 9;
	break;
      case TAGProto_row:
	size += 11;
	size += est_node (old -> Proto_row.sz) + 2;
	size += est_type (old -> Proto_row.elt) + 2;
	break;
      case TAGRow:
	size += 5;
	size += est_int (old -> Row.sz) + 2;
	size += est_type (old -> Row.elt) + 2;
	break;
      case TAGStruct:
	size += 8;
	size += est_field_list (old -> Struct.flds) + 2;
	break;
      case TAGUnion:
	size += 7;
	size += est_field_list (old -> Union.uflds) + 2;
	break;
      case TAGProc_type:
	size += 11;
	size += est_pdecl_list (old -> Proc_type.parms) + 2;
	size += est_type (old -> Proc_type.rtype) + 2;
	break;
      case TAGTname:
	size += 7;
	size += est_string (old -> Tname.tname) + 2;
	size += PTRWIDTH;
	break;
      case TAGProto_tname:
	size += 13;
	size += est_int (old -> Proto_tname.line) + 2;
	size += est_int (old -> Proto_tname.column) + 2;
	size += est_string (old -> Proto_tname.tname) + 2;
	break;
      case TAGDisplay_type:
	size += 14;
	size += est_type_list (old -> Display_type.flds) + 2;
	break;
      case TAGUnresolved:
	size += 12;
	size += est_type_list (old -> Unresolved.typs) + 2;
	break;
      default:
	dcg_bad_tag (old -> tag, "est_type");
    };
  return (size + 2);
}

/* Estimate printing a pdecl */
int est_pdecl (pdecl old)
{ int size = 0;
  if (old == pdecl_nil) return (2);
  size += est_type (old -> ptype) + 2;
  size += est_e_access (old -> pacc) + 2;
  return (size + 2);
}

/* Estimate printing a field */
int est_field (field old)
{ int size = 0;
  if (old == field_nil) return (2);
  size += est_type (old -> ftype) + 2;
  size += est_string (old -> fname) + 2;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate a e_access */
int est_e_access (e_access old)
{ switch (old)
    { case acc_any: return (7);
      case acc_const: return (9);
      case acc_var: return (7);
      case acc_proc: return (8);
      case acc_error: return (9);
      default:
	dcg_bad_tag (old, "est_e_access");
    };
  return (0);
}

/* Estimate a status */
int est_status (status old)
{ switch (old)
    { case not_checked: return (11);
      case being_checked: return (13);
      case recursive: return (9);
      case ref_checked: return (11);
      case type_checked: return (12);
      default:
	dcg_bad_tag (old, "est_status");
    };
  return (0);
}

/* Estimate printing a type_info */
int est_type_info (type_info old)
{ int size = 0;
  if (old == type_info_nil) return (2);
  size += est_string (old -> text) + 2;
  size += est_type (old -> typ) + 2;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  size += PTRWIDTH;
  return (size + 2);
}

/* Estimate a pragmat */
int est_pragmat (pragmat old)
{ switch (old)
    { case no_pragmat: return (10);
      case nonreentrant: return (12);
      default:
	dcg_bad_tag (old, "est_pragmat");
    };
  return (0);
}

/* Estimate printing a packet_list */
int est_packet_list (packet_list old)
{ int ix;
  int size = 0;
  if (old == packet_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_packet (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a node_list */
int est_node_list (node_list old)
{ int ix;
  int size = 0;
  if (old == node_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_node (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a case_part_list */
int est_case_part_list (case_part_list old)
{ int ix;
  int size = 0;
  if (old == case_part_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_case_part (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a decl_list */
int est_decl_list (decl_list old)
{ int ix;
  int size = 0;
  if (old == decl_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_decl (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a type_list */
int est_type_list (type_list old)
{ int ix;
  int size = 0;
  if (old == type_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_type (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a pdecl_list */
int est_pdecl_list (pdecl_list old)
{ int ix;
  int size = 0;
  if (old == pdecl_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_pdecl (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a field_list */
int est_field_list (field_list old)
{ int ix;
  int size = 0;
  if (old == field_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_field (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a type_info_list */
int est_type_info_list (type_info_list old)
{ int ix;
  int size = 0;
  if (old == type_info_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_type_info (old -> array[ix]) + 2;
  return (size + 2);
}

/* Estimate printing a pragmat_list */
int est_pragmat_list (pragmat_list old)
{ int ix;
  int size = 0;
  if (old == pragmat_list_nil) return (2);
  if (!old -> size) return (2);
  for (ix = 0; ix < old -> size; ix++)
    size += est_pragmat (old -> array[ix]) + 2;
  return (size + 2);
}

/* Pretty print a packet */
void ppp_packet (FILE *f, int horiz, int ind, packet old)
{ int mhoriz;
  if (old == packet_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_packet (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> fname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string (f, mhoriz, ind + 2, old -> pname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string_list (f, mhoriz, ind + 2, old -> pdefines);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string_list (f, mhoriz, ind + 2, old -> puses);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string_list (f, mhoriz, ind + 2, old -> puselibs);
  pppdelim (f, mhoriz, ind, ',');
  ppp_decl_list (f, mhoriz, ind + 2, old -> pdecls);
  pppdelim (f, mhoriz, ind, ',');
  ppp_node_list (f, mhoriz, ind + 2, old -> pcode);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> pkdecls);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a node */
void ppp_node (FILE *f, int horiz, int ind, node old)
{ int mhoriz;
  if (old == node_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_node (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_int (f, mhoriz, ind + 2, old -> line);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int (f, mhoriz, ind + 2, old -> column);
  pppdelim (f, mhoriz, ind, ',');
  switch (old -> tag)
    { case TAGInit:
	pppstring (f, "Init");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Init.name);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Init.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Init.src);
	break;
      case TAGAssign:
	pppstring (f, "Assign");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Assign.dest);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Assign.src);
	break;
      case TAGCond:
	pppstring (f, "Cond");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Cond.enq);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Cond.thenp);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Cond.elsep);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Cond.typ);
	break;
      case TAGNum:
	pppstring (f, "Num");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Num.enq);
	pppdelim (f, mhoriz, ind, ',');
	ppp_case_part_list (f, mhoriz, ind + 2, old -> Num.cps);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Num.oth);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Num.typ);
	break;
      case TAGFor:
	pppstring (f, "For");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> For.lvar);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> For.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> For.dir);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> For.from);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> For.to);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> For.wenq);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> For.body);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> For.uenq);
	break;
      case TAGWhile:
	pppstring (f, "While");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> While.wenq);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> While.body);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> While.uenq);
	break;
      case TAGLeave:
	pppstring (f, "Leave");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Leave.alg);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Leave.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Leave.with);
	break;
      case TAGDyn_call:
	pppstring (f, "Dyn_call");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Dyn_call.proc);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Dyn_call.args);
	break;
      case TAGCall:
	pppstring (f, "Call");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Call.pname);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Call.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Call.args);
	break;
      case TAGDisplay:
	pppstring (f, "Display");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Display.elts);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Display.typ);
	break;
      case TAGSub:
	pppstring (f, "Sub");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Sub.arr);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Sub.index);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Sub.rtype);
	break;
      case TAGSelect:
	pppstring (f, "Select");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Select.str);
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Select.sel);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Select.stype);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Select.fld);
	break;
      case TAGAbstr:
	pppstring (f, "Abstr");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Abstr.tname);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Abstr.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Abstr.arg);
	break;
      case TAGConcr:
	pppstring (f, "Concr");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Concr.arg);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Concr.id);
	break;
      case TAGDyop:
	pppstring (f, "Dyop");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Dyop.dop);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Dyop.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Dyop.arg1);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Dyop.arg2);
	break;
      case TAGMonop:
	pppstring (f, "Monop");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Monop.mop);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Monop.id);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Monop.arg);
	break;
      case TAGRef_appl:
	pppstring (f, "Ref_appl");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Ref_appl.name);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Ref_appl.id);
	break;
      case TAGAppl:
	pppstring (f, "Appl");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Appl.name);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Appl.id);
	break;
      case TAGNil:
	pppstring (f, "Nil");
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Nil.typ);
	break;
      case TAGDenoter:
	pppstring (f, "Denoter");
	pppdelim (f, mhoriz, ind, ',');
	ppp_value (f, mhoriz, ind + 2, old -> Denoter.val);
	break;
      case TAGSkip:
	pppstring (f, "Skip");
	break;
      case TAGDefault:
	pppstring (f, "Default");
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_node");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a case_part */
void ppp_case_part (FILE *f, int horiz, int ind, case_part old)
{ int mhoriz;
  if (old == case_part_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_case_part (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_node_list (f, mhoriz, ind + 2, old -> lbls);
  pppdelim (f, mhoriz, ind, ',');
  ppp_node_list (f, mhoriz, ind + 2, old -> lcod);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> els);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a decl */
void ppp_decl (FILE *f, int horiz, int ind, decl old)
{ int mhoriz;
  if (old == decl_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_decl (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_int (f, mhoriz, ind + 2, old -> line);
  pppdelim (f, mhoriz, ind, ',');
  ppp_int (f, mhoriz, ind + 2, old -> column);
  pppdelim (f, mhoriz, ind, ',');
  ppp_type (f, mhoriz, ind + 2, old -> typ);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string (f, mhoriz, ind + 2, old -> name);
  pppdelim (f, mhoriz, ind, ',');
  ppp_pragmat_list (f, mhoriz, ind + 2, old -> prs);
  pppdelim (f, mhoriz, ind, ',');
  switch (old -> tag)
    { case TAGFormal:
	pppstring (f, "Formal");
	pppdelim (f, mhoriz, ind, ',');
	ppp_e_access (f, mhoriz, ind + 2, old -> Formal.acc);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Formal.vnr);
	break;
      case TAGObject_decl:
	pppstring (f, "Object_decl");
	pppdelim (f, mhoriz, ind, ',');
	ppp_e_access (f, mhoriz, ind + 2, old -> Object_decl.acc);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Object_decl.vnr);
	break;
      case TAGProc_decl:
	pppstring (f, "Proc_decl");
	pppdelim (f, mhoriz, ind, ',');
	ppp_decl_list (f, mhoriz, ind + 2, old -> Proc_decl.fpars);
	pppdelim (f, mhoriz, ind, ',');
	ppp_body (f, mhoriz, ind + 2, old -> Proc_decl.bdy);
	break;
      case TAGOp_decl:
	pppstring (f, "Op_decl");
	pppdelim (f, mhoriz, ind, ',');
	ppp_decl_list (f, mhoriz, ind + 2, old -> Op_decl.fpars);
	pppdelim (f, mhoriz, ind, ',');
	ppp_body (f, mhoriz, ind + 2, old -> Op_decl.bdy);
	break;
      case TAGRefinement:
	pppstring (f, "Refinement");
	pppdelim (f, mhoriz, ind, ',');
	ppp_e_access (f, mhoriz, ind + 2, old -> Refinement.acc);
	pppdelim (f, mhoriz, ind, ',');
	ppp_decl_list (f, mhoriz, ind + 2, old -> Refinement.locals);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Refinement.rcode);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Refinement.stat);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Refinement.trc);
	break;
      case TAGSynonym_value:
	pppstring (f, "Synonym_value");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Synonym_value.rhs);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Synonym_value.val);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Synonym_value.stat);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Synonym_value.rec);
	break;
      case TAGSynonym_type:
	pppstring (f, "Synonym_type");
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Synonym_type.stat);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Synonym_type.rec);
	break;
      case TAGType:
	pppstring (f, "Type");
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Type.stat);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Type.rec);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Type.concr);
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_decl");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a body */
void ppp_body (FILE *f, int horiz, int ind, body old)
{ int mhoriz;
  if (old == body_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_body (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  switch (old -> tag)
    { case TAGInternal:
	pppstring (f, "Internal");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Internal.tname);
	break;
      case TAGExternal:
	pppstring (f, "External");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> External.ename);
	break;
      case TAGRoutine:
	pppstring (f, "Routine");
	pppdelim (f, mhoriz, ind, ',');
	ppp_decl_list (f, mhoriz, ind + 2, old -> Routine.locals);
	pppdelim (f, mhoriz, ind, ',');
	ppp_node_list (f, mhoriz, ind + 2, old -> Routine.rcode);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Routine.rdecls);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Routine.pnr);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Routine.trc);
	break;
      case TAGAttach:
	pppstring (f, "Attach");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Attach.t);
	break;
      case TAGDetach:
	pppstring (f, "Detach");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Detach.t);
	break;
      case TAGGuard:
	pppstring (f, "Guard");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Guard.t);
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_body");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a value */
void ppp_value (FILE *f, int horiz, int ind, value old)
{ int mhoriz;
  if (old == value_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_value (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  switch (old -> tag)
    { case TAGIval:
	pppstring (f, "Ival");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Ival.i);
	break;
      case TAGBval:
	pppstring (f, "Bval");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Bval.b);
	break;
      case TAGRval:
	pppstring (f, "Rval");
	pppdelim (f, mhoriz, ind, ',');
	ppp_real (f, mhoriz, ind + 2, old -> Rval.r);
	break;
      case TAGTval:
	pppstring (f, "Tval");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Tval.t);
	break;
      case TAGErrval:
	pppstring (f, "Errval");
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_value");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a type */
void ppp_type (FILE *f, int horiz, int ind, type old)
{ int mhoriz;
  if (old == type_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_type (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> tnr);
  pppdelim (f, mhoriz, ind, ',');
  switch (old -> tag)
    { case TAGUnknown:
	pppstring (f, "Unknown");
	break;
      case TAGError:
	pppstring (f, "Error");
	break;
      case TAGAny_non_void:
	pppstring (f, "Any_non_void");
	break;
      case TAGAny_row:
	pppstring (f, "Any_row");
	break;
      case TAGAny_struct:
	pppstring (f, "Any_struct");
	break;
      case TAGAny_object:
	pppstring (f, "Any_object");
	break;
      case TAGAddr:
	pppstring (f, "Addr");
	break;
      case TAGInt:
	pppstring (f, "Int");
	break;
      case TAGBool:
	pppstring (f, "Bool");
	break;
      case TAGReal:
	pppstring (f, "Real");
	break;
      case TAGText:
	pppstring (f, "Text");
	break;
      case TAGVoid:
	pppstring (f, "Void");
	break;
      case TAGFile:
	pppstring (f, "File");
	break;
      case TAGNiltype:
	pppstring (f, "Niltype");
	break;
      case TAGProto_row:
	pppstring (f, "Proto_row");
	pppdelim (f, mhoriz, ind, ',');
	ppp_node (f, mhoriz, ind + 2, old -> Proto_row.sz);
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Proto_row.elt);
	break;
      case TAGRow:
	pppstring (f, "Row");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Row.sz);
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Row.elt);
	break;
      case TAGStruct:
	pppstring (f, "Struct");
	pppdelim (f, mhoriz, ind, ',');
	ppp_field_list (f, mhoriz, ind + 2, old -> Struct.flds);
	break;
      case TAGUnion:
	pppstring (f, "Union");
	pppdelim (f, mhoriz, ind, ',');
	ppp_field_list (f, mhoriz, ind + 2, old -> Union.uflds);
	break;
      case TAGProc_type:
	pppstring (f, "Proc_type");
	pppdelim (f, mhoriz, ind, ',');
	ppp_pdecl_list (f, mhoriz, ind + 2, old -> Proc_type.parms);
	pppdelim (f, mhoriz, ind, ',');
	ppp_type (f, mhoriz, ind + 2, old -> Proc_type.rtype);
	break;
      case TAGTname:
	pppstring (f, "Tname");
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Tname.tname);
	pppdelim (f, mhoriz, ind, ',');
	ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> Tname.id);
	break;
      case TAGProto_tname:
	pppstring (f, "Proto_tname");
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Proto_tname.line);
	pppdelim (f, mhoriz, ind, ',');
	ppp_int (f, mhoriz, ind + 2, old -> Proto_tname.column);
	pppdelim (f, mhoriz, ind, ',');
	ppp_string (f, mhoriz, ind + 2, old -> Proto_tname.tname);
	break;
      case TAGDisplay_type:
	pppstring (f, "Display_type");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type_list (f, mhoriz, ind + 2, old -> Display_type.flds);
	break;
      case TAGUnresolved:
	pppstring (f, "Unresolved");
	pppdelim (f, mhoriz, ind, ',');
	ppp_type_list (f, mhoriz, ind + 2, old -> Unresolved.typs);
	break;
      default:
	dcg_bad_tag (old -> tag, "ppp_type");
    };
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a pdecl */
void ppp_pdecl (FILE *f, int horiz, int ind, pdecl old)
{ int mhoriz;
  if (old == pdecl_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_pdecl (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_type (f, mhoriz, ind + 2, old -> ptype);
  pppdelim (f, mhoriz, ind, ',');
  ppp_e_access (f, mhoriz, ind + 2, old -> pacc);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a field */
void ppp_field (FILE *f, int horiz, int ind, field old)
{ int mhoriz;
  if (old == field_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_field (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_type (f, mhoriz, ind + 2, old -> ftype);
  pppdelim (f, mhoriz, ind, ',');
  ppp_string (f, mhoriz, ind + 2, old -> fname);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> offset);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a e_access */
void ppp_e_access (FILE *f, int horiz, int ind, e_access old)
{ switch (old)
     { case acc_any:
	pppstring (f, "acc_any");
	break;
       case acc_const:
	pppstring (f, "acc_const");
	break;
       case acc_var:
	pppstring (f, "acc_var");
	break;
       case acc_proc:
	pppstring (f, "acc_proc");
	break;
       case acc_error:
	pppstring (f, "acc_error");
	break;
      default:
	dcg_bad_tag (old, "ppp_e_access");
    };
}

/* Pretty print a status */
void ppp_status (FILE *f, int horiz, int ind, status old)
{ switch (old)
     { case not_checked:
	pppstring (f, "not_checked");
	break;
       case being_checked:
	pppstring (f, "being_checked");
	break;
       case recursive:
	pppstring (f, "recursive");
	break;
       case ref_checked:
	pppstring (f, "ref_checked");
	break;
       case type_checked:
	pppstring (f, "type_checked");
	break;
      default:
	dcg_bad_tag (old, "ppp_status");
    };
}

/* Pretty print a type_info */
void ppp_type_info (FILE *f, int horiz, int ind, type_info old)
{ int mhoriz;
  if (old == type_info_nil) { pppstring (f, "<>"); return; };
  mhoriz = horiz || (est_type_info (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '(');
  ppp_string (f, mhoriz, ind + 2, old -> text);
  pppdelim (f, mhoriz, ind, ',');
  ppp_type (f, mhoriz, ind + 2, old -> typ);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> attach_nr);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> detach_nr);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> guard_nr);
  pppdelim (f, mhoriz, ind, ',');
  ppp_vptr (f, mhoriz, ind + 2, (vptr) old -> stabs_nr);
  pppdelim (f, mhoriz, ind, ')');
}

/* Pretty print a pragmat */
void ppp_pragmat (FILE *f, int horiz, int ind, pragmat old)
{ switch (old)
     { case no_pragmat:
	pppstring (f, "no_pragmat");
	break;
       case nonreentrant:
	pppstring (f, "nonreentrant");
	break;
      default:
	dcg_bad_tag (old, "ppp_pragmat");
    };
}

/* Pretty print a packet_list */
void ppp_packet_list (FILE *f, int horiz, int ind, packet_list old)
{ int ix, mhoriz;
  if (old == packet_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_packet_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_packet (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a node_list */
void ppp_node_list (FILE *f, int horiz, int ind, node_list old)
{ int ix, mhoriz;
  if (old == node_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_node_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_node (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a case_part_list */
void ppp_case_part_list (FILE *f, int horiz, int ind, case_part_list old)
{ int ix, mhoriz;
  if (old == case_part_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_case_part_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_case_part (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a decl_list */
void ppp_decl_list (FILE *f, int horiz, int ind, decl_list old)
{ int ix, mhoriz;
  if (old == decl_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_decl_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_decl (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a type_list */
void ppp_type_list (FILE *f, int horiz, int ind, type_list old)
{ int ix, mhoriz;
  if (old == type_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_type_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_type (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a pdecl_list */
void ppp_pdecl_list (FILE *f, int horiz, int ind, pdecl_list old)
{ int ix, mhoriz;
  if (old == pdecl_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_pdecl_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_pdecl (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a field_list */
void ppp_field_list (FILE *f, int horiz, int ind, field_list old)
{ int ix, mhoriz;
  if (old == field_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_field_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_field (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a type_info_list */
void ppp_type_info_list (FILE *f, int horiz, int ind, type_info_list old)
{ int ix, mhoriz;
  if (old == type_info_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_type_info_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_type_info (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Pretty print a pragmat_list */
void ppp_pragmat_list (FILE *f, int horiz, int ind, pragmat_list old)
{ int ix, mhoriz;
  if (old == pragmat_list_nil) { pppstring (f, "<>"); return; };
  if (!old -> size) { pppstring (f, "[]"); return; };
  mhoriz = horiz || (est_pragmat_list (old) + ind < MAXWIDTH);
  pppdelim (f, mhoriz, ind, '[');
  for (ix = 0; ix < old -> size; ix++)
    { ppp_pragmat (f, mhoriz, ind + 2, old -> array[ix]);
      if (ix != old -> size - 1) pppdelim (f, mhoriz, ind, ',');
    };
  pppdelim (f, mhoriz, ind, ']');
}

/* Recursively save a packet */
void save_packet (BinFile bf, packet old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> fname);
  save_string (bf, old -> pname);
  save_string_list (bf, old -> pdefines);
  save_string_list (bf, old -> puses);
  save_string_list (bf, old -> puselibs);
  save_decl_list (bf, old -> pdecls);
  save_node_list (bf, old -> pcode);
}

/* Recursively save a node */
void save_node (BinFile bf, node old)
{ /* save fixed fields before variant ones */
  save_int (bf, old -> line);
  save_int (bf, old -> column);
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGInit:
	save_string (bf, old -> Init.name);
	save_node (bf, old -> Init.src);
	break;
      case TAGAssign:
	save_node (bf, old -> Assign.dest);
	save_node (bf, old -> Assign.src);
	break;
      case TAGCond:
	save_node (bf, old -> Cond.enq);
	save_node_list (bf, old -> Cond.thenp);
	save_node_list (bf, old -> Cond.elsep);
	break;
      case TAGNum:
	save_node (bf, old -> Num.enq);
	save_case_part_list (bf, old -> Num.cps);
	save_node_list (bf, old -> Num.oth);
	break;
      case TAGFor:
	save_string (bf, old -> For.lvar);
	save_int (bf, old -> For.dir);
	save_node (bf, old -> For.from);
	save_node (bf, old -> For.to);
	save_node (bf, old -> For.wenq);
	save_node_list (bf, old -> For.body);
	save_node (bf, old -> For.uenq);
	break;
      case TAGWhile:
	save_node (bf, old -> While.wenq);
	save_node_list (bf, old -> While.body);
	save_node (bf, old -> While.uenq);
	break;
      case TAGLeave:
	save_string (bf, old -> Leave.alg);
	save_node (bf, old -> Leave.with);
	break;
      case TAGDyn_call:
	save_node (bf, old -> Dyn_call.proc);
	save_node_list (bf, old -> Dyn_call.args);
	break;
      case TAGCall:
	save_string (bf, old -> Call.pname);
	save_node_list (bf, old -> Call.args);
	break;
      case TAGDisplay:
	save_node_list (bf, old -> Display.elts);
	break;
      case TAGSub:
	save_node (bf, old -> Sub.arr);
	save_node (bf, old -> Sub.index);
	break;
      case TAGSelect:
	save_node (bf, old -> Select.str);
	save_string (bf, old -> Select.sel);
	break;
      case TAGAbstr:
	save_string (bf, old -> Abstr.tname);
	save_node (bf, old -> Abstr.arg);
	break;
      case TAGConcr:
	save_node (bf, old -> Concr.arg);
	break;
      case TAGDyop:
	save_string (bf, old -> Dyop.dop);
	save_node (bf, old -> Dyop.arg1);
	save_node (bf, old -> Dyop.arg2);
	break;
      case TAGMonop:
	save_string (bf, old -> Monop.mop);
	save_node (bf, old -> Monop.arg);
	break;
      case TAGRef_appl:
	save_string (bf, old -> Ref_appl.name);
	break;
      case TAGAppl:
	save_string (bf, old -> Appl.name);
	break;
      case TAGNil:
	break;
      case TAGDenoter:
	save_value (bf, old -> Denoter.val);
	break;
      case TAGSkip:
	break;
      case TAGDefault:
	break;
      default:
	dcg_bad_tag (old -> tag, "save_node");
    };
}

/* Recursively save a case_part */
void save_case_part (BinFile bf, case_part old)
{ /* save fixed fields before variant ones */
  save_node_list (bf, old -> lbls);
  save_node_list (bf, old -> lcod);
}

/* Recursively save a decl */
void save_decl (BinFile bf, decl old)
{ /* save fixed fields before variant ones */
  save_int (bf, old -> line);
  save_int (bf, old -> column);
  save_type (bf, old -> typ);
  save_string (bf, old -> name);
  save_pragmat_list (bf, old -> prs);
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGFormal:
	save_e_access (bf, old -> Formal.acc);
	break;
      case TAGObject_decl:
	save_e_access (bf, old -> Object_decl.acc);
	break;
      case TAGProc_decl:
	save_decl_list (bf, old -> Proc_decl.fpars);
	save_body (bf, old -> Proc_decl.bdy);
	break;
      case TAGOp_decl:
	save_decl_list (bf, old -> Op_decl.fpars);
	save_body (bf, old -> Op_decl.bdy);
	break;
      case TAGRefinement:
	save_e_access (bf, old -> Refinement.acc);
	save_decl_list (bf, old -> Refinement.locals);
	save_node_list (bf, old -> Refinement.rcode);
	break;
      case TAGSynonym_value:
	save_node (bf, old -> Synonym_value.rhs);
	break;
      case TAGSynonym_type:
	break;
      case TAGType:
	break;
      default:
	dcg_bad_tag (old -> tag, "save_decl");
    };
}

/* Recursively save a body */
void save_body (BinFile bf, body old)
{ /* save fixed fields before variant ones */
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGInternal:
	save_string (bf, old -> Internal.tname);
	break;
      case TAGExternal:
	save_string (bf, old -> External.ename);
	break;
      case TAGRoutine:
	save_decl_list (bf, old -> Routine.locals);
	save_node_list (bf, old -> Routine.rcode);
	break;
      case TAGAttach:
	save_type (bf, old -> Attach.t);
	break;
      case TAGDetach:
	save_type (bf, old -> Detach.t);
	break;
      case TAGGuard:
	save_type (bf, old -> Guard.t);
	break;
      default:
	dcg_bad_tag (old -> tag, "save_body");
    };
}

/* Recursively save a value */
void save_value (BinFile bf, value old)
{ /* save fixed fields before variant ones */
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGIval:
	save_int (bf, old -> Ival.i);
	break;
      case TAGBval:
	save_int (bf, old -> Bval.b);
	break;
      case TAGRval:
	save_real (bf, old -> Rval.r);
	break;
      case TAGTval:
	save_string (bf, old -> Tval.t);
	break;
      case TAGErrval:
	break;
      default:
	dcg_bad_tag (old -> tag, "save_value");
    };
}

/* Recursively save a type */
void save_type (BinFile bf, type old)
{ /* save fixed fields before variant ones */
  save_char (bf, (char) old -> tag);
  switch (old -> tag)
    { case TAGUnknown:
	break;
      case TAGError:
	break;
      case TAGAny_non_void:
	break;
      case TAGAny_row:
	break;
      case TAGAny_struct:
	break;
      case TAGAny_object:
	break;
      case TAGAddr:
	break;
      case TAGInt:
	break;
      case TAGBool:
	break;
      case TAGReal:
	break;
      case TAGText:
	break;
      case TAGVoid:
	break;
      case TAGFile:
	break;
      case TAGNiltype:
	break;
      case TAGProto_row:
	save_node (bf, old -> Proto_row.sz);
	save_type (bf, old -> Proto_row.elt);
	break;
      case TAGRow:
	save_int (bf, old -> Row.sz);
	save_type (bf, old -> Row.elt);
	break;
      case TAGStruct:
	save_field_list (bf, old -> Struct.flds);
	break;
      case TAGUnion:
	save_field_list (bf, old -> Union.uflds);
	break;
      case TAGProc_type:
	save_pdecl_list (bf, old -> Proc_type.parms);
	save_type (bf, old -> Proc_type.rtype);
	break;
      case TAGTname:
	save_string (bf, old -> Tname.tname);
	break;
      case TAGProto_tname:
	save_int (bf, old -> Proto_tname.line);
	save_int (bf, old -> Proto_tname.column);
	save_string (bf, old -> Proto_tname.tname);
	break;
      case TAGDisplay_type:
	save_type_list (bf, old -> Display_type.flds);
	break;
      case TAGUnresolved:
	save_type_list (bf, old -> Unresolved.typs);
	break;
      default:
	dcg_bad_tag (old -> tag, "save_type");
    };
}

/* Recursively save a pdecl */
void save_pdecl (BinFile bf, pdecl old)
{ /* save fixed fields before variant ones */
  save_type (bf, old -> ptype);
  save_e_access (bf, old -> pacc);
}

/* Recursively save a field */
void save_field (BinFile bf, field old)
{ /* save fixed fields before variant ones */
  save_type (bf, old -> ftype);
  save_string (bf, old -> fname);
}

/* Recursively save a type_info */
void save_type_info (BinFile bf, type_info old)
{ /* save fixed fields before variant ones */
  save_string (bf, old -> text);
  save_type (bf, old -> typ);
}

/* Recursively save a packet_list */
void save_packet_list (BinFile bf, packet_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_packet (bf, l -> array[ix]);
}

/* Recursively save a node_list */
void save_node_list (BinFile bf, node_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_node (bf, l -> array[ix]);
}

/* Recursively save a case_part_list */
void save_case_part_list (BinFile bf, case_part_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_case_part (bf, l -> array[ix]);
}

/* Recursively save a decl_list */
void save_decl_list (BinFile bf, decl_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_decl (bf, l -> array[ix]);
}

/* Recursively save a type_list */
void save_type_list (BinFile bf, type_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_type (bf, l -> array[ix]);
}

/* Recursively save a pdecl_list */
void save_pdecl_list (BinFile bf, pdecl_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_pdecl (bf, l -> array[ix]);
}

/* Recursively save a field_list */
void save_field_list (BinFile bf, field_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_field (bf, l -> array[ix]);
}

/* Recursively save a type_info_list */
void save_type_info_list (BinFile bf, type_info_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_type_info (bf, l -> array[ix]);
}

/* Recursively save a pragmat_list */
void save_pragmat_list (BinFile bf, pragmat_list l)
{ int ix;
  savesize (bf, l -> size);
  for (ix = 0; ix < l -> size; ix++)
    save_pragmat (bf, l -> array[ix]);
}

/* Recursively load a packet */
void load_packet (BinFile bf, packet *x)
{ /* load fixed fields before variant ones */
  packet new = (packet) dcg_malloc (sizeof (struct str_packet));
  load_string (bf, &new -> fname);
  load_string (bf, &new -> pname);
  load_string_list (bf, &new -> pdefines);
  load_string_list (bf, &new -> puses);
  load_string_list (bf, &new -> puselibs);
  load_decl_list (bf, &new -> pdecls);
  load_node_list (bf, &new -> pcode);
  new -> pkdecls = decl_tree_nil;
  *x = new;
}

/* Recursively load a node */
void load_node (BinFile bf, node *x)
{ /* load fixed fields before variant ones */
  node new = (node) dcg_malloc (sizeof (struct str_node));
  char ch;
  load_int (bf, &new -> line);
  load_int (bf, &new -> column);
  load_char (bf, &ch);
  new -> tag = (tags_node) ch;
  switch (new -> tag)
    { case TAGInit:
	load_string (bf, &new -> Init.name);
	new -> Init.id = decl_nil;
	load_node (bf, &new -> Init.src);
	break;
      case TAGAssign:
	load_node (bf, &new -> Assign.dest);
	load_node (bf, &new -> Assign.src);
	break;
      case TAGCond:
	load_node (bf, &new -> Cond.enq);
	load_node_list (bf, &new -> Cond.thenp);
	load_node_list (bf, &new -> Cond.elsep);
	new -> Cond.typ = type_nil;
	break;
      case TAGNum:
	load_node (bf, &new -> Num.enq);
	load_case_part_list (bf, &new -> Num.cps);
	load_node_list (bf, &new -> Num.oth);
	new -> Num.typ = type_nil;
	break;
      case TAGFor:
	load_string (bf, &new -> For.lvar);
	new -> For.id = decl_nil;
	load_int (bf, &new -> For.dir);
	load_node (bf, &new -> For.from);
	load_node (bf, &new -> For.to);
	load_node (bf, &new -> For.wenq);
	load_node_list (bf, &new -> For.body);
	load_node (bf, &new -> For.uenq);
	break;
      case TAGWhile:
	load_node (bf, &new -> While.wenq);
	load_node_list (bf, &new -> While.body);
	load_node (bf, &new -> While.uenq);
	break;
      case TAGLeave:
	load_string (bf, &new -> Leave.alg);
	new -> Leave.id = decl_nil;
	load_node (bf, &new -> Leave.with);
	break;
      case TAGDyn_call:
	load_node (bf, &new -> Dyn_call.proc);
	load_node_list (bf, &new -> Dyn_call.args);
	break;
      case TAGCall:
	load_string (bf, &new -> Call.pname);
	new -> Call.id = decl_nil;
	load_node_list (bf, &new -> Call.args);
	break;
      case TAGDisplay:
	load_node_list (bf, &new -> Display.elts);
	new -> Display.typ = type_nil;
	break;
      case TAGSub:
	load_node (bf, &new -> Sub.arr);
	load_node (bf, &new -> Sub.index);
	new -> Sub.rtype = type_nil;
	break;
      case TAGSelect:
	load_node (bf, &new -> Select.str);
	load_string (bf, &new -> Select.sel);
	new -> Select.stype = type_nil;
	new -> Select.fld = field_nil;
	break;
      case TAGAbstr:
	load_string (bf, &new -> Abstr.tname);
	new -> Abstr.id = decl_nil;
	load_node (bf, &new -> Abstr.arg);
	break;
      case TAGConcr:
	load_node (bf, &new -> Concr.arg);
	new -> Concr.id = decl_nil;
	break;
      case TAGDyop:
	load_string (bf, &new -> Dyop.dop);
	new -> Dyop.id = decl_nil;
	load_node (bf, &new -> Dyop.arg1);
	load_node (bf, &new -> Dyop.arg2);
	break;
      case TAGMonop:
	load_string (bf, &new -> Monop.mop);
	new -> Monop.id = decl_nil;
	load_node (bf, &new -> Monop.arg);
	break;
      case TAGRef_appl:
	load_string (bf, &new -> Ref_appl.name);
	new -> Ref_appl.id = decl_nil;
	break;
      case TAGAppl:
	load_string (bf, &new -> Appl.name);
	new -> Appl.id = decl_nil;
	break;
      case TAGNil:
	new -> Nil.typ = type_nil;
	break;
      case TAGDenoter:
	load_value (bf, &new -> Denoter.val);
	break;
      case TAGSkip:
	break;
      case TAGDefault:
	break;
      default:
	dcg_bad_tag (new -> tag, "load_node");
    };
  *x = new;
}

/* Recursively load a case_part */
void load_case_part (BinFile bf, case_part *x)
{ /* load fixed fields before variant ones */
  case_part new = (case_part) dcg_malloc (sizeof (struct str_case_part));
  load_node_list (bf, &new -> lbls);
  load_node_list (bf, &new -> lcod);
  new -> els = int_list_nil;
  *x = new;
}

/* Recursively load a decl */
void load_decl (BinFile bf, decl *x)
{ /* load fixed fields before variant ones */
  decl new = (decl) dcg_malloc (sizeof (struct str_decl));
  char ch;
  load_int (bf, &new -> line);
  load_int (bf, &new -> column);
  load_type (bf, &new -> typ);
  load_string (bf, &new -> name);
  load_pragmat_list (bf, &new -> prs);
  load_char (bf, &ch);
  new -> tag = (tags_decl) ch;
  switch (new -> tag)
    { case TAGFormal:
	load_e_access (bf, &new -> Formal.acc);
	new -> Formal.vnr = int_nil;
	break;
      case TAGObject_decl:
	load_e_access (bf, &new -> Object_decl.acc);
	new -> Object_decl.vnr = int_nil;
	break;
      case TAGProc_decl:
	load_decl_list (bf, &new -> Proc_decl.fpars);
	load_body (bf, &new -> Proc_decl.bdy);
	break;
      case TAGOp_decl:
	load_decl_list (bf, &new -> Op_decl.fpars);
	load_body (bf, &new -> Op_decl.bdy);
	break;
      case TAGRefinement:
	load_e_access (bf, &new -> Refinement.acc);
	load_decl_list (bf, &new -> Refinement.locals);
	load_node_list (bf, &new -> Refinement.rcode);
	new -> Refinement.stat = status_nil;
	new -> Refinement.trc = int_nil;
	break;
      case TAGSynonym_value:
	load_node (bf, &new -> Synonym_value.rhs);
	new -> Synonym_value.val = value_nil;
	new -> Synonym_value.stat = status_nil;
	new -> Synonym_value.rec = int_nil;
	break;
      case TAGSynonym_type:
	new -> Synonym_type.stat = status_nil;
	new -> Synonym_type.rec = int_nil;
	break;
      case TAGType:
	new -> Type.stat = status_nil;
	new -> Type.rec = int_nil;
	new -> Type.concr = type_nil;
	break;
      default:
	dcg_bad_tag (new -> tag, "load_decl");
    };
  *x = new;
}

/* Recursively load a body */
void load_body (BinFile bf, body *x)
{ /* load fixed fields before variant ones */
  body new = (body) dcg_malloc (sizeof (struct str_body));
  char ch;
  load_char (bf, &ch);
  new -> tag = (tags_body) ch;
  switch (new -> tag)
    { case TAGInternal:
	load_string (bf, &new -> Internal.tname);
	break;
      case TAGExternal:
	load_string (bf, &new -> External.ename);
	break;
      case TAGRoutine:
	load_decl_list (bf, &new -> Routine.locals);
	load_node_list (bf, &new -> Routine.rcode);
	new -> Routine.rdecls = decl_tree_nil;
	new -> Routine.pnr = int_nil;
	new -> Routine.trc = int_nil;
	break;
      case TAGAttach:
	load_type (bf, &new -> Attach.t);
	break;
      case TAGDetach:
	load_type (bf, &new -> Detach.t);
	break;
      case TAGGuard:
	load_type (bf, &new -> Guard.t);
	break;
      default:
	dcg_bad_tag (new -> tag, "load_body");
    };
  *x = new;
}

/* Recursively load a value */
void load_value (BinFile bf, value *x)
{ /* load fixed fields before variant ones */
  value new = (value) dcg_malloc (sizeof (struct str_value));
  char ch;
  load_char (bf, &ch);
  new -> tag = (tags_value) ch;
  switch (new -> tag)
    { case TAGIval:
	load_int (bf, &new -> Ival.i);
	break;
      case TAGBval:
	load_int (bf, &new -> Bval.b);
	break;
      case TAGRval:
	load_real (bf, &new -> Rval.r);
	break;
      case TAGTval:
	load_string (bf, &new -> Tval.t);
	break;
      case TAGErrval:
	break;
      default:
	dcg_bad_tag (new -> tag, "load_value");
    };
  *x = new;
}

/* Recursively load a type */
void load_type (BinFile bf, type *x)
{ /* load fixed fields before variant ones */
  type new = (type) dcg_malloc (sizeof (struct str_type));
  char ch;
  new -> tnr = int_nil;
  load_char (bf, &ch);
  new -> tag = (tags_type) ch;
  switch (new -> tag)
    { case TAGUnknown:
	break;
      case TAGError:
	break;
      case TAGAny_non_void:
	break;
      case TAGAny_row:
	break;
      case TAGAny_struct:
	break;
      case TAGAny_object:
	break;
      case TAGAddr:
	break;
      case TAGInt:
	break;
      case TAGBool:
	break;
      case TAGReal:
	break;
      case TAGText:
	break;
      case TAGVoid:
	break;
      case TAGFile:
	break;
      case TAGNiltype:
	break;
      case TAGProto_row:
	load_node (bf, &new -> Proto_row.sz);
	load_type (bf, &new -> Proto_row.elt);
	break;
      case TAGRow:
	load_int (bf, &new -> Row.sz);
	load_type (bf, &new -> Row.elt);
	break;
      case TAGStruct:
	load_field_list (bf, &new -> Struct.flds);
	break;
      case TAGUnion:
	load_field_list (bf, &new -> Union.uflds);
	break;
      case TAGProc_type:
	load_pdecl_list (bf, &new -> Proc_type.parms);
	load_type (bf, &new -> Proc_type.rtype);
	break;
      case TAGTname:
	load_string (bf, &new -> Tname.tname);
	new -> Tname.id = decl_nil;
	break;
      case TAGProto_tname:
	load_int (bf, &new -> Proto_tname.line);
	load_int (bf, &new -> Proto_tname.column);
	load_string (bf, &new -> Proto_tname.tname);
	break;
      case TAGDisplay_type:
	load_type_list (bf, &new -> Display_type.flds);
	break;
      case TAGUnresolved:
	load_type_list (bf, &new -> Unresolved.typs);
	break;
      default:
	dcg_bad_tag (new -> tag, "load_type");
    };
  *x = new;
}

/* Recursively load a pdecl */
void load_pdecl (BinFile bf, pdecl *x)
{ /* load fixed fields before variant ones */
  pdecl new = (pdecl) dcg_malloc (sizeof (struct str_pdecl));
  load_type (bf, &new -> ptype);
  load_e_access (bf, &new -> pacc);
  *x = new;
}

/* Recursively load a field */
void load_field (BinFile bf, field *x)
{ /* load fixed fields before variant ones */
  field new = (field) dcg_malloc (sizeof (struct str_field));
  load_type (bf, &new -> ftype);
  load_string (bf, &new -> fname);
  new -> offset = int_nil;
  *x = new;
}

/* Load a e_access */
void load_e_access (BinFile bf, e_access *x)
{ char ch;
  load_char (bf, &ch);
  *x = (e_access) ch;
}

/* Load a status */
void load_status (BinFile bf, status *x)
{ char ch;
  load_char (bf, &ch);
  *x = (status) ch;
}

/* Recursively load a type_info */
void load_type_info (BinFile bf, type_info *x)
{ /* load fixed fields before variant ones */
  type_info new = (type_info) dcg_malloc (sizeof (struct str_type_info));
  load_string (bf, &new -> text);
  load_type (bf, &new -> typ);
  new -> attach_nr = int_nil;
  new -> detach_nr = int_nil;
  new -> guard_nr = int_nil;
  new -> stabs_nr = int_nil;
  *x = new;
}

/* Load a pragmat */
void load_pragmat (BinFile bf, pragmat *x)
{ char ch;
  load_char (bf, &ch);
  *x = (pragmat) ch;
}

/* Recursively load a packet_list */
void load_packet_list (BinFile bf, packet_list *l)
{ int ix, size;
  packet_list new;
  loadsize (bf, &size);
  new = init_packet_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_packet (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a node_list */
void load_node_list (BinFile bf, node_list *l)
{ int ix, size;
  node_list new;
  loadsize (bf, &size);
  new = init_node_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_node (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a case_part_list */
void load_case_part_list (BinFile bf, case_part_list *l)
{ int ix, size;
  case_part_list new;
  loadsize (bf, &size);
  new = init_case_part_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_case_part (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a decl_list */
void load_decl_list (BinFile bf, decl_list *l)
{ int ix, size;
  decl_list new;
  loadsize (bf, &size);
  new = init_decl_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_decl (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a type_list */
void load_type_list (BinFile bf, type_list *l)
{ int ix, size;
  type_list new;
  loadsize (bf, &size);
  new = init_type_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_type (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a pdecl_list */
void load_pdecl_list (BinFile bf, pdecl_list *l)
{ int ix, size;
  pdecl_list new;
  loadsize (bf, &size);
  new = init_pdecl_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_pdecl (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a field_list */
void load_field_list (BinFile bf, field_list *l)
{ int ix, size;
  field_list new;
  loadsize (bf, &size);
  new = init_field_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_field (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a type_info_list */
void load_type_info_list (BinFile bf, type_info_list *l)
{ int ix, size;
  type_info_list new;
  loadsize (bf, &size);
  new = init_type_info_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_type_info (bf, &new -> array[ix]);
  *l = new;
}

/* Recursively load a pragmat_list */
void load_pragmat_list (BinFile bf, pragmat_list *l)
{ int ix, size;
  pragmat_list new;
  loadsize (bf, &size);
  new = init_pragmat_list (size);
  new -> size = size;
  for (ix = 0; ix < size; ix++)
    load_pragmat (bf, &new -> array[ix]);
  *l = new;
}

