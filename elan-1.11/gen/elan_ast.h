/*
   File: elan_ast.h
   Generated on Wed Oct  5 12:44:33 2011
*/
#ifndef IncElan_ast
#define IncElan_ast

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_binfile.h>

/* Introduce record type definitions */
typedef struct str_packet *packet;
typedef struct str_node *node;
typedef struct str_case_part *case_part;
typedef struct str_decl *decl;
typedef struct str_body *body;
typedef struct str_value *value;
typedef struct str_type *type;
typedef struct str_pdecl *pdecl;
typedef struct str_field *field;
typedef struct str_type_info *type_info;

/* Introduce type_list definitions */
typedef struct str_packet_list *packet_list;
typedef struct str_node_list *node_list;
typedef struct str_case_part_list *case_part_list;
typedef struct str_decl_list *decl_list;
typedef struct str_type_list *type_list;
typedef struct str_pdecl_list *pdecl_list;
typedef struct str_field_list *field_list;
typedef struct str_type_info_list *type_info_list;
typedef struct str_pragmat_list *pragmat_list;

/* Introduce enumeration types */
typedef enum
{ acc_any, acc_const, acc_var,
  acc_proc, acc_error
} e_access;

typedef enum
{ not_checked, being_checked, recursive,
  ref_checked, type_checked
} status;

typedef enum
{ no_pragmat, nonreentrant
} pragmat;

/* Include imported defs */
#include <dcg_plist.h>
#include <decl_tree.h>

/* Introduce record nils */
#define packet_nil ((packet) NULL)
#define node_nil ((node) NULL)
#define case_part_nil ((case_part) NULL)
#define decl_nil ((decl) NULL)
#define body_nil ((body) NULL)
#define value_nil ((value) NULL)
#define type_nil ((type) NULL)
#define pdecl_nil ((pdecl) NULL)
#define field_nil ((field) NULL)
#define type_info_nil ((type_info) NULL)

/* Introduce type_list nils */
#define packet_list_nil ((packet_list) NULL)
#define node_list_nil ((node_list) NULL)
#define case_part_list_nil ((case_part_list) NULL)
#define decl_list_nil ((decl_list) NULL)
#define type_list_nil ((type_list) NULL)
#define pdecl_list_nil ((pdecl_list) NULL)
#define field_list_nil ((field_list) NULL)
#define type_info_list_nil ((type_info_list) NULL)
#define pragmat_list_nil ((pragmat_list) NULL)

/* Introduce enumeration nils */
#define e_access_nil ((e_access) 0)
#define status_nil ((status) 0)
#define pragmat_nil ((pragmat) 0)

/* introduce constructors */
#define Init u.ue_Init
#define Assign u.ue_Assign
#define Cond u.ue_Cond
#define Num u.ue_Num
#define For u.ue_For
#define While u.ue_While
#define Leave u.ue_Leave
#define Dyn_call u.ue_Dyn_call
#define Call u.ue_Call
#define Display u.ue_Display
#define Sub u.ue_Sub
#define Select u.ue_Select
#define Abstr u.ue_Abstr
#define Concr u.ue_Concr
#define Dyop u.ue_Dyop
#define Monop u.ue_Monop
#define Ref_appl u.ue_Ref_appl
#define Appl u.ue_Appl
#define Nil u.ue_Nil
#define Denoter u.ue_Denoter
#define Skip u.ue_Skip
#define Default u.ue_Default
#define Formal u.ue_Formal
#define Object_decl u.ue_Object_decl
#define Proc_decl u.ue_Proc_decl
#define Op_decl u.ue_Op_decl
#define Refinement u.ue_Refinement
#define Synonym_value u.ue_Synonym_value
#define Synonym_type u.ue_Synonym_type
#define Type u.ue_Type
#define Internal u.ue_Internal
#define External u.ue_External
#define Routine u.ue_Routine
#define Attach u.ue_Attach
#define Detach u.ue_Detach
#define Guard u.ue_Guard
#define Ival u.ue_Ival
#define Bval u.ue_Bval
#define Rval u.ue_Rval
#define Tval u.ue_Tval
#define Errval u.ue_Errval
#define Unknown u.ue_Unknown
#define Error u.ue_Error
#define Any_non_void u.ue_Any_non_void
#define Any_row u.ue_Any_row
#define Any_struct u.ue_Any_struct
#define Any_object u.ue_Any_object
#define Addr u.ue_Addr
#define Int u.ue_Int
#define Bool u.ue_Bool
#define Real u.ue_Real
#define Text u.ue_Text
#define Void u.ue_Void
#define File u.ue_File
#define Niltype u.ue_Niltype
#define Proto_row u.ue_Proto_row
#define Row u.ue_Row
#define Struct u.ue_Struct
#define Union u.ue_Union
#define Proc_type u.ue_Proc_type
#define Tname u.ue_Tname
#define Proto_tname u.ue_Proto_tname
#define Display_type u.ue_Display_type
#define Unresolved u.ue_Unresolved

/* Introduce constructor tags */
typedef enum en_tags_node
{ TAGInit,
  TAGAssign,
  TAGCond,
  TAGNum,
  TAGFor,
  TAGWhile,
  TAGLeave,
  TAGDyn_call,
  TAGCall,
  TAGDisplay,
  TAGSub,
  TAGSelect,
  TAGAbstr,
  TAGConcr,
  TAGDyop,
  TAGMonop,
  TAGRef_appl,
  TAGAppl,
  TAGNil,
  TAGDenoter,
  TAGSkip,
  TAGDefault
} tags_node;

typedef enum en_tags_decl
{ TAGFormal,
  TAGObject_decl,
  TAGProc_decl,
  TAGOp_decl,
  TAGRefinement,
  TAGSynonym_value,
  TAGSynonym_type,
  TAGType
} tags_decl;

typedef enum en_tags_body
{ TAGInternal,
  TAGExternal,
  TAGRoutine,
  TAGAttach,
  TAGDetach,
  TAGGuard
} tags_body;

typedef enum en_tags_value
{ TAGIval,
  TAGBval,
  TAGRval,
  TAGTval,
  TAGErrval
} tags_value;

typedef enum en_tags_type
{ TAGUnknown,
  TAGError,
  TAGAny_non_void,
  TAGAny_row,
  TAGAny_struct,
  TAGAny_object,
  TAGAddr,
  TAGInt,
  TAGBool,
  TAGReal,
  TAGText,
  TAGVoid,
  TAGFile,
  TAGNiltype,
  TAGProto_row,
  TAGRow,
  TAGStruct,
  TAGUnion,
  TAGProc_type,
  TAGTname,
  TAGProto_tname,
  TAGDisplay_type,
  TAGUnresolved
} tags_type;

/* Introduce structure for constructor Init */
typedef struct str_Init
{ string name;
  decl id;
  node src;
} C_Init;

/* Introduce structure for constructor Assign */
typedef struct str_Assign
{ node dest;
  node src;
} C_Assign;

/* Introduce structure for constructor Cond */
typedef struct str_Cond
{ node enq;
  node_list thenp;
  node_list elsep;
  type typ;
} C_Cond;

/* Introduce structure for constructor Num */
typedef struct str_Num
{ node enq;
  case_part_list cps;
  node_list oth;
  type typ;
} C_Num;

/* Introduce structure for constructor For */
typedef struct str_For
{ string lvar;
  decl id;
  int dir;
  node from;
  node to;
  node wenq;
  node_list body;
  node uenq;
} C_For;

/* Introduce structure for constructor While */
typedef struct str_While
{ node wenq;
  node_list body;
  node uenq;
} C_While;

/* Introduce structure for constructor Leave */
typedef struct str_Leave
{ string alg;
  decl id;
  node with;
} C_Leave;

/* Introduce structure for constructor Dyn_call */
typedef struct str_Dyn_call
{ node proc;
  node_list args;
} C_Dyn_call;

/* Introduce structure for constructor Call */
typedef struct str_Call
{ string pname;
  decl id;
  node_list args;
} C_Call;

/* Introduce structure for constructor Display */
typedef struct str_Display
{ node_list elts;
  type typ;
} C_Display;

/* Introduce structure for constructor Sub */
typedef struct str_Sub
{ node arr;
  node index;
  type rtype;
} C_Sub;

/* Introduce structure for constructor Select */
typedef struct str_Select
{ node str;
  string sel;
  type stype;
  field fld;
} C_Select;

/* Introduce structure for constructor Abstr */
typedef struct str_Abstr
{ string tname;
  decl id;
  node arg;
} C_Abstr;

/* Introduce structure for constructor Concr */
typedef struct str_Concr
{ node arg;
  decl id;
} C_Concr;

/* Introduce structure for constructor Dyop */
typedef struct str_Dyop
{ string dop;
  decl id;
  node arg1;
  node arg2;
} C_Dyop;

/* Introduce structure for constructor Monop */
typedef struct str_Monop
{ string mop;
  decl id;
  node arg;
} C_Monop;

/* Introduce structure for constructor Ref_appl */
typedef struct str_Ref_appl
{ string name;
  decl id;
} C_Ref_appl;

/* Introduce structure for constructor Appl */
typedef struct str_Appl
{ string name;
  decl id;
} C_Appl;

/* Introduce structure for constructor Nil */
typedef struct str_Nil
{ type typ;
} C_Nil;

/* Introduce structure for constructor Denoter */
typedef struct str_Denoter
{ value val;
} C_Denoter;

/* Introduce structure for constructor Formal */
typedef struct str_Formal
{ e_access acc;
  int vnr;
} C_Formal;

/* Introduce structure for constructor Object_decl */
typedef struct str_Object_decl
{ e_access acc;
  int vnr;
} C_Object_decl;

/* Introduce structure for constructor Proc_decl */
typedef struct str_Proc_decl
{ decl_list fpars;
  body bdy;
} C_Proc_decl;

/* Introduce structure for constructor Op_decl */
typedef struct str_Op_decl
{ decl_list fpars;
  body bdy;
} C_Op_decl;

/* Introduce structure for constructor Refinement */
typedef struct str_Refinement
{ e_access acc;
  decl_list locals;
  node_list rcode;
  status stat;
  int trc;
} C_Refinement;

/* Introduce structure for constructor Synonym_value */
typedef struct str_Synonym_value
{ node rhs;
  value val;
  status stat;
  int rec;
} C_Synonym_value;

/* Introduce structure for constructor Synonym_type */
typedef struct str_Synonym_type
{ status stat;
  int rec;
} C_Synonym_type;

/* Introduce structure for constructor Type */
typedef struct str_Type
{ status stat;
  int rec;
  type concr;
} C_Type;

/* Introduce structure for constructor Internal */
typedef struct str_Internal
{ string tname;
} C_Internal;

/* Introduce structure for constructor External */
typedef struct str_External
{ string ename;
} C_External;

/* Introduce structure for constructor Routine */
typedef struct str_Routine
{ decl_list locals;
  node_list rcode;
  decl_tree rdecls;
  int pnr;
  int trc;
} C_Routine;

/* Introduce structure for constructor Attach */
typedef struct str_Attach
{ type t;
} C_Attach;

/* Introduce structure for constructor Detach */
typedef struct str_Detach
{ type t;
} C_Detach;

/* Introduce structure for constructor Guard */
typedef struct str_Guard
{ type t;
} C_Guard;

/* Introduce structure for constructor Ival */
typedef struct str_Ival
{ int i;
} C_Ival;

/* Introduce structure for constructor Bval */
typedef struct str_Bval
{ int b;
} C_Bval;

/* Introduce structure for constructor Rval */
typedef struct str_Rval
{ real r;
} C_Rval;

/* Introduce structure for constructor Tval */
typedef struct str_Tval
{ string t;
} C_Tval;

/* Introduce structure for constructor Proto_row */
typedef struct str_Proto_row
{ node sz;
  type elt;
} C_Proto_row;

/* Introduce structure for constructor Row */
typedef struct str_Row
{ int sz;
  type elt;
} C_Row;

/* Introduce structure for constructor Struct */
typedef struct str_Struct
{ field_list flds;
} C_Struct;

/* Introduce structure for constructor Union */
typedef struct str_Union
{ field_list uflds;
} C_Union;

/* Introduce structure for constructor Proc_type */
typedef struct str_Proc_type
{ pdecl_list parms;
  type rtype;
} C_Proc_type;

/* Introduce structure for constructor Tname */
typedef struct str_Tname
{ string tname;
  decl id;
} C_Tname;

/* Introduce structure for constructor Proto_tname */
typedef struct str_Proto_tname
{ int line;
  int column;
  string tname;
} C_Proto_tname;

/* Introduce structure for constructor Display_type */
typedef struct str_Display_type
{ type_list flds;
} C_Display_type;

/* Introduce structure for constructor Unresolved */
typedef struct str_Unresolved
{ type_list typs;
} C_Unresolved;

/* Introduce structure of type packet */
struct str_packet
{ string fname;
  string pname;
  string_list pdefines;
  string_list puses;
  string_list puselibs;
  decl_list pdecls;
  node_list pcode;
  decl_tree pkdecls;
};

/* Introduce structure of type node */
struct str_node
{ int line;
  int column;
  tags_node tag;
  union uni_node
    { C_Init ue_Init;
      C_Assign ue_Assign;
      C_Cond ue_Cond;
      C_Num ue_Num;
      C_For ue_For;
      C_While ue_While;
      C_Leave ue_Leave;
      C_Dyn_call ue_Dyn_call;
      C_Call ue_Call;
      C_Display ue_Display;
      C_Sub ue_Sub;
      C_Select ue_Select;
      C_Abstr ue_Abstr;
      C_Concr ue_Concr;
      C_Dyop ue_Dyop;
      C_Monop ue_Monop;
      C_Ref_appl ue_Ref_appl;
      C_Appl ue_Appl;
      C_Nil ue_Nil;
      C_Denoter ue_Denoter;
    } u;
};

/* Introduce structure of type case_part */
struct str_case_part
{ node_list lbls;
  node_list lcod;
  int_list els;
};

/* Introduce structure of type decl */
struct str_decl
{ int line;
  int column;
  type typ;
  string name;
  pragmat_list prs;
  tags_decl tag;
  union uni_decl
    { C_Formal ue_Formal;
      C_Object_decl ue_Object_decl;
      C_Proc_decl ue_Proc_decl;
      C_Op_decl ue_Op_decl;
      C_Refinement ue_Refinement;
      C_Synonym_value ue_Synonym_value;
      C_Synonym_type ue_Synonym_type;
      C_Type ue_Type;
    } u;
};

/* Introduce structure of type body */
struct str_body
{ tags_body tag;
  union uni_body
    { C_Internal ue_Internal;
      C_External ue_External;
      C_Routine ue_Routine;
      C_Attach ue_Attach;
      C_Detach ue_Detach;
      C_Guard ue_Guard;
    } u;
};

/* Introduce structure of type value */
struct str_value
{ tags_value tag;
  union uni_value
    { C_Ival ue_Ival;
      C_Bval ue_Bval;
      C_Rval ue_Rval;
      C_Tval ue_Tval;
    } u;
};

/* Introduce structure of type type */
struct str_type
{ int tnr;
  tags_type tag;
  union uni_type
    { C_Proto_row ue_Proto_row;
      C_Row ue_Row;
      C_Struct ue_Struct;
      C_Union ue_Union;
      C_Proc_type ue_Proc_type;
      C_Tname ue_Tname;
      C_Proto_tname ue_Proto_tname;
      C_Display_type ue_Display_type;
      C_Unresolved ue_Unresolved;
    } u;
};

/* Introduce structure of type pdecl */
struct str_pdecl
{ type ptype;
  e_access pacc;
};

/* Introduce structure of type field */
struct str_field
{ type ftype;
  string fname;
  int offset;
};

/* Introduce structure of type type_info */
struct str_type_info
{ string text;
  type typ;
  int attach_nr;
  int detach_nr;
  int guard_nr;
  int stabs_nr;
};

/* Introduce structure of type packet_list */
struct str_packet_list
{ int size;
  int room;
  packet *array;
};

/* Introduce structure of type node_list */
struct str_node_list
{ int size;
  int room;
  node *array;
};

/* Introduce structure of type case_part_list */
struct str_case_part_list
{ int size;
  int room;
  case_part *array;
};

/* Introduce structure of type decl_list */
struct str_decl_list
{ int size;
  int room;
  decl *array;
};

/* Introduce structure of type type_list */
struct str_type_list
{ int size;
  int room;
  type *array;
};

/* Introduce structure of type pdecl_list */
struct str_pdecl_list
{ int size;
  int room;
  pdecl *array;
};

/* Introduce structure of type field_list */
struct str_field_list
{ int size;
  int room;
  field *array;
};

/* Introduce structure of type type_info_list */
struct str_type_info_list
{ int size;
  int room;
  type_info *array;
};

/* Introduce structure of type pragmat_list */
struct str_pragmat_list
{ int size;
  int room;
  pragmat *array;
};

/* Introduce list allocation routines */
packet_list init_packet_list (int room);
node_list init_node_list (int room);
case_part_list init_case_part_list (int room);
decl_list init_decl_list (int room);
type_list init_type_list (int room);
pdecl_list init_pdecl_list (int room);
field_list init_field_list (int room);
type_info_list init_type_info_list (int room);
pragmat_list init_pragmat_list (int room);

/* Introduce room allocation routines */
void room_packet_list (packet_list l, int room);
void room_node_list (node_list l, int room);
void room_case_part_list (case_part_list l, int room);
void room_decl_list (decl_list l, int room);
void room_type_list (type_list l, int room);
void room_pdecl_list (pdecl_list l, int room);
void room_field_list (field_list l, int room);
void room_type_info_list (type_info_list l, int room);
void room_pragmat_list (pragmat_list l, int room);

/* Introduce list allocation shorthands */
#define new_packet_list() init_packet_list(2)
#define new_node_list() init_node_list(2)
#define new_case_part_list() init_case_part_list(2)
#define new_decl_list() init_decl_list(2)
#define new_type_list() init_type_list(2)
#define new_pdecl_list() init_pdecl_list(2)
#define new_field_list() init_field_list(2)
#define new_type_info_list() init_type_info_list(2)
#define new_pragmat_list() init_pragmat_list(2)

/* Introduce type allocation routines */
packet new_packet (string fname, string pname, string_list pdefines, string_list puses, string_list puselibs, decl_list pdecls, node_list pcode);
node new_Init (int line, int column,
		string name, node src);
node new_Assign (int line, int column,
		node dest, node src);
node new_Cond (int line, int column,
		node enq, node_list thenp, node_list elsep);
node new_Num (int line, int column,
		node enq, case_part_list cps, node_list oth);
node new_For (int line, int column,
		string lvar, int dir, node from, node to, node wenq, node_list body, node uenq);
node new_While (int line, int column,
		node wenq, node_list body, node uenq);
node new_Leave (int line, int column,
		string alg, node with);
node new_Dyn_call (int line, int column,
		node proc, node_list args);
node new_Call (int line, int column,
		string pname, node_list args);
node new_Display (int line, int column,
		node_list elts);
node new_Sub (int line, int column,
		node arr, node index);
node new_Select (int line, int column,
		node str, string sel);
node new_Abstr (int line, int column,
		string tname, node arg);
node new_Concr (int line, int column,
		node arg);
node new_Dyop (int line, int column,
		string dop, node arg1, node arg2);
node new_Monop (int line, int column,
		string mop, node arg);
node new_Ref_appl (int line, int column,
		string name);
node new_Appl (int line, int column,
		string name);
node new_Nil (int line, int column);
node new_Denoter (int line, int column,
		value val);
node new_Skip (int line, int column);
node new_Default (int line, int column);
case_part new_case_part (node_list lbls, node_list lcod);
decl new_Formal (int line, int column, type typ, string name, pragmat_list prs,
		e_access acc);
decl new_Object_decl (int line, int column, type typ, string name, pragmat_list prs,
		e_access acc);
decl new_Proc_decl (int line, int column, type typ, string name, pragmat_list prs,
		decl_list fpars, body bdy);
decl new_Op_decl (int line, int column, type typ, string name, pragmat_list prs,
		decl_list fpars, body bdy);
decl new_Refinement (int line, int column, type typ, string name, pragmat_list prs,
		e_access acc, decl_list locals, node_list rcode);
decl new_Synonym_value (int line, int column, type typ, string name, pragmat_list prs,
		node rhs);
decl new_Synonym_type (int line, int column, type typ, string name, pragmat_list prs);
decl new_Type (int line, int column, type typ, string name, pragmat_list prs);
body new_Internal (string tname);
body new_External (string ename);
body new_Routine (decl_list locals, node_list rcode);
body new_Attach (type t);
body new_Detach (type t);
body new_Guard (type t);
value new_Ival (int i);
value new_Bval (int b);
value new_Rval (real r);
value new_Tval (string t);
value new_Errval ();
type new_Unknown ();
type new_Error ();
type new_Any_non_void ();
type new_Any_row ();
type new_Any_struct ();
type new_Any_object ();
type new_Addr ();
type new_Int ();
type new_Bool ();
type new_Real ();
type new_Text ();
type new_Void ();
type new_File ();
type new_Niltype ();
type new_Proto_row (node sz, type elt);
type new_Row (int sz, type elt);
type new_Struct (field_list flds);
type new_Union (field_list uflds);
type new_Proc_type (pdecl_list parms, type rtype);
type new_Tname (string tname);
type new_Proto_tname (int line, int column, string tname);
type new_Display_type (type_list flds);
type new_Unresolved (type_list typs);
pdecl new_pdecl (type ptype, e_access pacc);
field new_field (type ftype, string fname);
type_info new_type_info (string text, type typ);

/* Introduce attaching of types */
#define attach_packet(orig)(packet) dcg_attach ((char *) orig)
#define att_packet(orig)(void) dcg_attach ((char *) orig)
#define attach_node(orig)(node) dcg_attach ((char *) orig)
#define att_node(orig)(void) dcg_attach ((char *) orig)
#define attach_case_part(orig)(case_part) dcg_attach ((char *) orig)
#define att_case_part(orig)(void) dcg_attach ((char *) orig)
#define attach_decl(orig)(decl) dcg_attach ((char *) orig)
#define att_decl(orig)(void) dcg_attach ((char *) orig)
#define attach_body(orig)(body) dcg_attach ((char *) orig)
#define att_body(orig)(void) dcg_attach ((char *) orig)
#define attach_value(orig)(value) dcg_attach ((char *) orig)
#define att_value(orig)(void) dcg_attach ((char *) orig)
#define attach_type(orig)(type) dcg_attach ((char *) orig)
#define att_type(orig)(void) dcg_attach ((char *) orig)
#define attach_pdecl(orig)(pdecl) dcg_attach ((char *) orig)
#define att_pdecl(orig)(void) dcg_attach ((char *) orig)
#define attach_field(orig)(field) dcg_attach ((char *) orig)
#define att_field(orig)(void) dcg_attach ((char *) orig)
#define attach_e_access(orig) (orig)
#define att_e_access(orig)
#define attach_status(orig) (orig)
#define att_status(orig)
#define attach_type_info(orig)(type_info) dcg_attach ((char *) orig)
#define att_type_info(orig)(void) dcg_attach ((char *) orig)
#define attach_pragmat(orig) (orig)
#define att_pragmat(orig)

/* Introduce attaching of lists */
#define attach_packet_list(orig) (packet_list) dcg_attach ((char *) orig)
#define att_packet_list(orig) (void) dcg_attach ((char *) orig)
#define attach_node_list(orig) (node_list) dcg_attach ((char *) orig)
#define att_node_list(orig) (void) dcg_attach ((char *) orig)
#define attach_case_part_list(orig) (case_part_list) dcg_attach ((char *) orig)
#define att_case_part_list(orig) (void) dcg_attach ((char *) orig)
#define attach_decl_list(orig) (decl_list) dcg_attach ((char *) orig)
#define att_decl_list(orig) (void) dcg_attach ((char *) orig)
#define attach_type_list(orig) (type_list) dcg_attach ((char *) orig)
#define att_type_list(orig) (void) dcg_attach ((char *) orig)
#define attach_pdecl_list(orig) (pdecl_list) dcg_attach ((char *) orig)
#define att_pdecl_list(orig) (void) dcg_attach ((char *) orig)
#define attach_field_list(orig) (field_list) dcg_attach ((char *) orig)
#define att_field_list(orig) (void) dcg_attach ((char *) orig)
#define attach_type_info_list(orig) (type_info_list) dcg_attach ((char *) orig)
#define att_type_info_list(orig) (void) dcg_attach ((char *) orig)
#define attach_pragmat_list(orig) (pragmat_list) dcg_attach ((char *) orig)
#define att_pragmat_list(orig) (void) dcg_attach ((char *) orig)

/* Introduce detaching of enumeration types */
#define detach_e_access(old)
#define det_e_access(old)
#define detach_status(old)
#define det_status(old)
#define detach_pragmat(old)
#define det_pragmat(old)

/* Introduce detaching of record types */
void detach_packet (packet *optr);
#define det_packet(optr) detach_packet(optr)
void detach_node (node *optr);
#define det_node(optr) detach_node(optr)
void detach_case_part (case_part *optr);
#define det_case_part(optr) detach_case_part(optr)
void detach_decl (decl *optr);
#define det_decl(optr) detach_decl(optr)
void detach_body (body *optr);
#define det_body(optr) detach_body(optr)
void detach_value (value *optr);
#define det_value(optr) detach_value(optr)
void detach_type (type *optr);
#define det_type(optr) detach_type(optr)
void detach_pdecl (pdecl *optr);
#define det_pdecl(optr) detach_pdecl(optr)
void detach_field (field *optr);
#define det_field(optr) detach_field(optr)
void detach_type_info (type_info *optr);
#define det_type_info(optr) detach_type_info(optr)

/* Introduce detaching of lists */
void detach_packet_list (packet_list *lp);
void detach_node_list (node_list *lp);
void detach_case_part_list (case_part_list *lp);
void detach_decl_list (decl_list *lp);
void detach_type_list (type_list *lp);
void detach_pdecl_list (pdecl_list *lp);
void detach_field_list (field_list *lp);
void detach_type_info_list (type_info_list *lp);
void detach_pragmat_list (pragmat_list *lp);

/* Introduce nonrecursive detaching of lists */
void nonrec_detach_packet_list (packet_list *lp);
void nonrec_detach_node_list (node_list *lp);
void nonrec_detach_case_part_list (case_part_list *lp);
void nonrec_detach_decl_list (decl_list *lp);
void nonrec_detach_type_list (type_list *lp);
void nonrec_detach_pdecl_list (pdecl_list *lp);
void nonrec_detach_field_list (field_list *lp);
void nonrec_detach_type_info_list (type_info_list *lp);
void nonrec_detach_pragmat_list (pragmat_list *lp);

/* Introduce list append routines */
packet_list append_packet_list (packet_list l, packet el);
node_list append_node_list (node_list l, node el);
case_part_list append_case_part_list (case_part_list l, case_part el);
decl_list append_decl_list (decl_list l, decl el);
type_list append_type_list (type_list l, type el);
pdecl_list append_pdecl_list (pdecl_list l, pdecl el);
field_list append_field_list (field_list l, field el);
type_info_list append_type_info_list (type_info_list l, type_info el);
pragmat_list append_pragmat_list (pragmat_list l, pragmat el);

/* Introduce void shorthands for list append routines */
#define app_packet_list(l,el) (void) append_packet_list (l,el)
#define app_node_list(l,el) (void) append_node_list (l,el)
#define app_case_part_list(l,el) (void) append_case_part_list (l,el)
#define app_decl_list(l,el) (void) append_decl_list (l,el)
#define app_type_list(l,el) (void) append_type_list (l,el)
#define app_pdecl_list(l,el) (void) append_pdecl_list (l,el)
#define app_field_list(l,el) (void) append_field_list (l,el)
#define app_type_info_list(l,el) (void) append_type_info_list (l,el)
#define app_pragmat_list(l,el) (void) append_pragmat_list (l,el)

/* Introduce list concat routines */
packet_list concat_packet_list (packet_list l1, packet_list l2);
node_list concat_node_list (node_list l1, node_list l2);
case_part_list concat_case_part_list (case_part_list l1, case_part_list l2);
decl_list concat_decl_list (decl_list l1, decl_list l2);
type_list concat_type_list (type_list l1, type_list l2);
pdecl_list concat_pdecl_list (pdecl_list l1, pdecl_list l2);
field_list concat_field_list (field_list l1, field_list l2);
type_info_list concat_type_info_list (type_info_list l1, type_info_list l2);
pragmat_list concat_pragmat_list (pragmat_list l1, pragmat_list l2);

/* Introduce void shorthands for list concat routines */
#define conc_packet_list(l1,l2) (void) concat_packet_list (l1,l2)
#define conc_node_list(l1,l2) (void) concat_node_list (l1,l2)
#define conc_case_part_list(l1,l2) (void) concat_case_part_list (l1,l2)
#define conc_decl_list(l1,l2) (void) concat_decl_list (l1,l2)
#define conc_type_list(l1,l2) (void) concat_type_list (l1,l2)
#define conc_pdecl_list(l1,l2) (void) concat_pdecl_list (l1,l2)
#define conc_field_list(l1,l2) (void) concat_field_list (l1,l2)
#define conc_type_info_list(l1,l2) (void) concat_type_info_list (l1,l2)
#define conc_pragmat_list(l1,l2) (void) concat_pragmat_list (l1,l2)

/* Introduce list insertion routines */
packet_list insert_packet_list (packet_list l, int pos, packet el);
node_list insert_node_list (node_list l, int pos, node el);
case_part_list insert_case_part_list (case_part_list l, int pos, case_part el);
decl_list insert_decl_list (decl_list l, int pos, decl el);
type_list insert_type_list (type_list l, int pos, type el);
pdecl_list insert_pdecl_list (pdecl_list l, int pos, pdecl el);
field_list insert_field_list (field_list l, int pos, field el);
type_info_list insert_type_info_list (type_info_list l, int pos, type_info el);
pragmat_list insert_pragmat_list (pragmat_list l, int pos, pragmat el);

/* Introduce void shorthands for list insertion routines */
#define ins_packet_list(l,pos,el) (void) insert_packet_list (l,pos,el)
#define ins_node_list(l,pos,el) (void) insert_node_list (l,pos,el)
#define ins_case_part_list(l,pos,el) (void) insert_case_part_list (l,pos,el)
#define ins_decl_list(l,pos,el) (void) insert_decl_list (l,pos,el)
#define ins_type_list(l,pos,el) (void) insert_type_list (l,pos,el)
#define ins_pdecl_list(l,pos,el) (void) insert_pdecl_list (l,pos,el)
#define ins_field_list(l,pos,el) (void) insert_field_list (l,pos,el)
#define ins_type_info_list(l,pos,el) (void) insert_type_info_list (l,pos,el)
#define ins_pragmat_list(l,pos,el) (void) insert_pragmat_list (l,pos,el)

/* Introduce list deletion routines */
packet_list delete_packet_list (packet_list l, int pos);
node_list delete_node_list (node_list l, int pos);
case_part_list delete_case_part_list (case_part_list l, int pos);
decl_list delete_decl_list (decl_list l, int pos);
type_list delete_type_list (type_list l, int pos);
pdecl_list delete_pdecl_list (pdecl_list l, int pos);
field_list delete_field_list (field_list l, int pos);
type_info_list delete_type_info_list (type_info_list l, int pos);
pragmat_list delete_pragmat_list (pragmat_list l, int pos);

/* Introduce void shorthands for list deletion routines */
#define del_packet_list(l,pos) (void) delete_packet_list (l,pos)
#define del_node_list(l,pos) (void) delete_node_list (l,pos)
#define del_case_part_list(l,pos) (void) delete_case_part_list (l,pos)
#define del_decl_list(l,pos) (void) delete_decl_list (l,pos)
#define del_type_list(l,pos) (void) delete_type_list (l,pos)
#define del_pdecl_list(l,pos) (void) delete_pdecl_list (l,pos)
#define del_field_list(l,pos) (void) delete_field_list (l,pos)
#define del_type_info_list(l,pos) (void) delete_type_info_list (l,pos)
#define del_pragmat_list(l,pos) (void) delete_pragmat_list (l,pos)

/* Equality test for types */
int cmp_packet (packet a, packet b);
int cmp_node (node a, node b);
int cmp_case_part (case_part a, case_part b);
int cmp_decl (decl a, decl b);
int cmp_body (body a, body b);
int cmp_value (value a, value b);
int cmp_type (type a, type b);
int cmp_pdecl (pdecl a, pdecl b);
int cmp_field (field a, field b);
#define cmp_e_access(a,b) (cmp_int ((int)(a),(int)(b)))
#define cmp_status(a,b) (cmp_int ((int)(a),(int)(b)))
int cmp_type_info (type_info a, type_info b);
#define cmp_pragmat(a,b) (cmp_int ((int)(a),(int)(b)))

/* Comparison tests for lists */
int cmp_packet_list (packet_list a, packet_list b);
int cmp_node_list (node_list a, node_list b);
int cmp_case_part_list (case_part_list a, case_part_list b);
int cmp_decl_list (decl_list a, decl_list b);
int cmp_type_list (type_list a, type_list b);
int cmp_pdecl_list (pdecl_list a, pdecl_list b);
int cmp_field_list (field_list a, field_list b);
int cmp_type_info_list (type_info_list a, type_info_list b);
int cmp_pragmat_list (pragmat_list a, pragmat_list b);

/* Equality shorthands for types */
#define equal_packet(a,b) !cmp_packet ((a),(b))
#define equal_node(a,b) !cmp_node ((a),(b))
#define equal_case_part(a,b) !cmp_case_part ((a),(b))
#define equal_decl(a,b) !cmp_decl ((a),(b))
#define equal_body(a,b) !cmp_body ((a),(b))
#define equal_value(a,b) !cmp_value ((a),(b))
#define equal_type(a,b) !cmp_type ((a),(b))
#define equal_pdecl(a,b) !cmp_pdecl ((a),(b))
#define equal_field(a,b) !cmp_field ((a),(b))
#define equal_e_access(a,b) !cmp_e_access ((a),(b))
#define equal_status(a,b) !cmp_status ((a),(b))
#define equal_type_info(a,b) !cmp_type_info ((a),(b))
#define equal_pragmat(a,b) !cmp_pragmat ((a),(b))

/* Equality shorthands for lists */
#define equal_packet_list(a,b) !cmp_packet_list((a),(b))
#define equal_node_list(a,b) !cmp_node_list((a),(b))
#define equal_case_part_list(a,b) !cmp_case_part_list((a),(b))
#define equal_decl_list(a,b) !cmp_decl_list((a),(b))
#define equal_type_list(a,b) !cmp_type_list((a),(b))
#define equal_pdecl_list(a,b) !cmp_pdecl_list((a),(b))
#define equal_field_list(a,b) !cmp_field_list((a),(b))
#define equal_type_info_list(a,b) !cmp_type_info_list((a),(b))
#define equal_pragmat_list(a,b) !cmp_pragmat_list((a),(b))

/* Estimating printing of types */
int est_packet (packet old);
int est_node (node old);
int est_case_part (case_part old);
int est_decl (decl old);
int est_body (body old);
int est_value (value old);
int est_type (type old);
int est_pdecl (pdecl old);
int est_field (field old);
int est_e_access (e_access old);
int est_status (status old);
int est_type_info (type_info old);
int est_pragmat (pragmat old);

/* Estimate printing of lists */
int est_packet_list (packet_list old);
int est_node_list (node_list old);
int est_case_part_list (case_part_list old);
int est_decl_list (decl_list old);
int est_type_list (type_list old);
int est_pdecl_list (pdecl_list old);
int est_field_list (field_list old);
int est_type_info_list (type_info_list old);
int est_pragmat_list (pragmat_list old);

void ppp_packet (FILE *f, int horiz, int ind, packet old);
void ppp_node (FILE *f, int horiz, int ind, node old);
void ppp_case_part (FILE *f, int horiz, int ind, case_part old);
void ppp_decl (FILE *f, int horiz, int ind, decl old);
void ppp_body (FILE *f, int horiz, int ind, body old);
void ppp_value (FILE *f, int horiz, int ind, value old);
void ppp_type (FILE *f, int horiz, int ind, type old);
void ppp_pdecl (FILE *f, int horiz, int ind, pdecl old);
void ppp_field (FILE *f, int horiz, int ind, field old);
void ppp_e_access (FILE *f, int horiz, int ind, e_access old);
void ppp_status (FILE *f, int horiz, int ind, status old);
void ppp_type_info (FILE *f, int horiz, int ind, type_info old);
void ppp_pragmat (FILE *f, int horiz, int ind, pragmat old);

/* Introduce pretty printing of lists */
void ppp_packet_list (FILE *f, int horiz, int ind, packet_list old);
void ppp_node_list (FILE *f, int horiz, int ind, node_list old);
void ppp_case_part_list (FILE *f, int horiz, int ind, case_part_list old);
void ppp_decl_list (FILE *f, int horiz, int ind, decl_list old);
void ppp_type_list (FILE *f, int horiz, int ind, type_list old);
void ppp_pdecl_list (FILE *f, int horiz, int ind, pdecl_list old);
void ppp_field_list (FILE *f, int horiz, int ind, field_list old);
void ppp_type_info_list (FILE *f, int horiz, int ind, type_info_list old);
void ppp_pragmat_list (FILE *f, int horiz, int ind, pragmat_list old);

/* Introduce pretty printing shorthands */
#define pp_packet(f,x) ppp_packet (f,0,0,x)
#define pp_node(f,x) ppp_node (f,0,0,x)
#define pp_case_part(f,x) ppp_case_part (f,0,0,x)
#define pp_decl(f,x) ppp_decl (f,0,0,x)
#define pp_body(f,x) ppp_body (f,0,0,x)
#define pp_value(f,x) ppp_value (f,0,0,x)
#define pp_type(f,x) ppp_type (f,0,0,x)
#define pp_pdecl(f,x) ppp_pdecl (f,0,0,x)
#define pp_field(f,x) ppp_field (f,0,0,x)
#define pp_e_access(f,x) ppp_e_access (f,0,0,x)
#define pp_status(f,x) ppp_status (f,0,0,x)
#define pp_type_info(f,x) ppp_type_info (f,0,0,x)
#define pp_pragmat(f,x) ppp_pragmat (f,0,0,x)

/* Introduce pretty printing shorthands for lists */
#define pp_packet_list(f,x) ppp_packet_list (f,0,0,x)
#define pp_node_list(f,x) ppp_node_list (f,0,0,x)
#define pp_case_part_list(f,x) ppp_case_part_list (f,0,0,x)
#define pp_decl_list(f,x) ppp_decl_list (f,0,0,x)
#define pp_type_list(f,x) ppp_type_list (f,0,0,x)
#define pp_pdecl_list(f,x) ppp_pdecl_list (f,0,0,x)
#define pp_field_list(f,x) ppp_field_list (f,0,0,x)
#define pp_type_info_list(f,x) ppp_type_info_list (f,0,0,x)
#define pp_pragmat_list(f,x) ppp_pragmat_list (f,0,0,x)

/* Introduce saving of types */
#define save_e_access(bf,x) save_char(bf, (char) x)
#define save_status(bf,x) save_char(bf, (char) x)
#define save_pragmat(bf,x) save_char(bf, (char) x)
void save_packet (BinFile bf, packet old);
void save_node (BinFile bf, node old);
void save_case_part (BinFile bf, case_part old);
void save_decl (BinFile bf, decl old);
void save_body (BinFile bf, body old);
void save_value (BinFile bf, value old);
void save_type (BinFile bf, type old);
void save_pdecl (BinFile bf, pdecl old);
void save_field (BinFile bf, field old);
void save_type_info (BinFile bf, type_info old);

/* Introduce saving of lists */
void save_packet_list (BinFile bf, packet_list l);
void save_node_list (BinFile bf, node_list l);
void save_case_part_list (BinFile bf, case_part_list l);
void save_decl_list (BinFile bf, decl_list l);
void save_type_list (BinFile bf, type_list l);
void save_pdecl_list (BinFile bf, pdecl_list l);
void save_field_list (BinFile bf, field_list l);
void save_type_info_list (BinFile bf, type_info_list l);
void save_pragmat_list (BinFile bf, pragmat_list l);

/* Introduce loading of types */
void load_packet (BinFile bf, packet *x);
void load_node (BinFile bf, node *x);
void load_case_part (BinFile bf, case_part *x);
void load_decl (BinFile bf, decl *x);
void load_body (BinFile bf, body *x);
void load_value (BinFile bf, value *x);
void load_type (BinFile bf, type *x);
void load_pdecl (BinFile bf, pdecl *x);
void load_field (BinFile bf, field *x);
void load_e_access (BinFile bf, e_access *x);
void load_status (BinFile bf, status *x);
void load_type_info (BinFile bf, type_info *x);
void load_pragmat (BinFile bf, pragmat *x);

/* Introduce loading of lists */
void load_packet_list (BinFile bf, packet_list *l);
void load_node_list (BinFile bf, node_list *l);
void load_case_part_list (BinFile bf, case_part_list *l);
void load_decl_list (BinFile bf, decl_list *l);
void load_type_list (BinFile bf, type_list *l);
void load_pdecl_list (BinFile bf, pdecl_list *l);
void load_field_list (BinFile bf, field_list *l);
void load_type_info_list (BinFile bf, type_info_list *l);
void load_pragmat_list (BinFile bf, pragmat_list *l);

#endif /* IncElan_ast */
