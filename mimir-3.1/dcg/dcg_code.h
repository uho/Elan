/*
   File: dcg_code.h
   Generated on Tue Aug 21 21:55:29 2012

   Copyright (C) 2011 Marc Seutter

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_code.h,v 1.12 2012/08/21 19:58:57 marcs Exp $"
*/
#ifndef IncDcgCode
#define IncDcgCode

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_binfile.h>

/* Introduce record type definitions */
typedef struct str_def *def;
typedef struct str_stat *stat;
typedef struct str_field *field;
typedef struct str_vfield *vfield;
typedef struct str_type *type;

/* Introduce type_list definitions */
typedef struct str_def_list *def_list;
typedef struct str_stat_list *stat_list;
typedef struct str_field_list *field_list;
typedef struct str_vfield_list *vfield_list;
typedef struct str_type_list *type_list;
typedef struct str_string_list *string_list;

/* Introduce enumeration types */
/* Include imported defs */

/* Introduce record nils */
#define def_nil ((def) NULL)
#define stat_nil ((stat) NULL)
#define field_nil ((field) NULL)
#define vfield_nil ((vfield) NULL)
#define type_nil ((type) NULL)

/* Introduce type_list nils */
#define def_list_nil ((def_list) NULL)
#define stat_list_nil ((stat_list) NULL)
#define field_list_nil ((field_list) NULL)
#define vfield_list_nil ((vfield_list) NULL)
#define type_list_nil ((type_list) NULL)
#define string_list_nil ((string_list) NULL)

/* Introduce enumeration nils */

/* introduce constructors */
#define Primitive u.ue_Primitive
#define Enum u.ue_Enum
#define Record u.ue_Record
#define Use u.ue_Use
#define Import u.ue_Import
#define Tname u.ue_Tname
#define Tlist u.ue_Tlist

/* Introduce constructor tags */
typedef enum en_tags_def
{ TAGPrimitive,
  TAGEnum,
  TAGRecord
} tags_def;

typedef enum en_tags_stat
{ TAGUse,
  TAGImport
} tags_stat;

typedef enum en_tags_type
{ TAGTname,
  TAGTlist
} tags_type;

/* Introduce structure for constructor Enum */
typedef struct str_Enum
{ string_list elems;
} C_Enum;

/* Introduce structure for constructor Record */
typedef struct str_Record
{ field_list fixed;
  vfield_list variant;
} C_Record;

/* Introduce structure for constructor Use */
typedef struct str_Use
{ type_list utype;
} C_Use;

/* Introduce structure for constructor Import */
typedef struct str_Import
{ string imp;
} C_Import;

/* Introduce structure for constructor Tname */
typedef struct str_Tname
{ string tname;
} C_Tname;

/* Introduce structure for constructor Tlist */
typedef struct str_Tlist
{ type etyp;
} C_Tlist;

/* Introduce structure of type def */
struct str_def
{ string lhs;
  int nrlsts;
  int implsts;
  tags_def tag;
  union uni_def
    { C_Enum ue_Enum;
      C_Record ue_Record;
    } u;
};

/* Introduce structure of type stat */
struct str_stat
{ tags_stat tag;
  union uni_stat
    { C_Use ue_Use;
      C_Import ue_Import;
    } u;
};

/* Introduce structure of type field */
struct str_field
{ string fname;
  type ftype;
  int ftrav;
};

/* Introduce structure of type vfield */
struct str_vfield
{ string cons;
  field_list parts;
};

/* Introduce structure of type type */
struct str_type
{ tags_type tag;
  union uni_type
    { C_Tname ue_Tname;
      C_Tlist ue_Tlist;
    } u;
};

/* Introduce structure of type def_list */
struct str_def_list
{ int size;
  int room;
  def *array;
};

/* Introduce structure of type stat_list */
struct str_stat_list
{ int size;
  int room;
  stat *array;
};

/* Introduce structure of type field_list */
struct str_field_list
{ int size;
  int room;
  field *array;
};

/* Introduce structure of type vfield_list */
struct str_vfield_list
{ int size;
  int room;
  vfield *array;
};

/* Introduce structure of type type_list */
struct str_type_list
{ int size;
  int room;
  type *array;
};

/* Introduce structure of type string_list */
struct str_string_list
{ int size;
  int room;
  string *array;
};

/* Introduce list allocation routines */
def_list init_def_list (int room);
stat_list init_stat_list (int room);
field_list init_field_list (int room);
vfield_list init_vfield_list (int room);
type_list init_type_list (int room);
string_list init_string_list (int room);

/* Introduce room allocation routines */
void room_def_list (def_list l, int room);
void room_stat_list (stat_list l, int room);
void room_field_list (field_list l, int room);
void room_vfield_list (vfield_list l, int room);
void room_type_list (type_list l, int room);
void room_string_list (string_list l, int room);

/* Introduce list allocation shorthands */
#define new_def_list() init_def_list(2)
#define new_stat_list() init_stat_list(2)
#define new_field_list() init_field_list(2)
#define new_vfield_list() init_vfield_list(2)
#define new_type_list() init_type_list(2)
#define new_string_list() init_string_list(2)

/* Introduce type allocation routines */
def new_Primitive (string lhs);
def new_Enum (string lhs,
		string_list elems);
def new_Record (string lhs,
		field_list fixed, vfield_list variant);
stat new_Use (type_list utype);
stat new_Import (string imp);
field new_field (string fname, type ftype, int ftrav);
vfield new_vfield (string cons, field_list parts);
type new_Tname (string tname);
type new_Tlist (type etyp);

/* Introduce attaching of types */
#define attach_def(orig)(def) dcg_attach ((char *) orig)
#define att_def(orig)(void) dcg_attach ((char *) orig)
#define attach_stat(orig)(stat) dcg_attach ((char *) orig)
#define att_stat(orig)(void) dcg_attach ((char *) orig)
#define attach_field(orig)(field) dcg_attach ((char *) orig)
#define att_field(orig)(void) dcg_attach ((char *) orig)
#define attach_vfield(orig)(vfield) dcg_attach ((char *) orig)
#define att_vfield(orig)(void) dcg_attach ((char *) orig)
#define attach_type(orig)(type) dcg_attach ((char *) orig)
#define att_type(orig)(void) dcg_attach ((char *) orig)

/* Introduce attaching of lists */
#define attach_def_list(orig) (def_list) dcg_attach ((char *) orig)
#define att_def_list(orig) (void) dcg_attach ((char *) orig)
#define attach_stat_list(orig) (stat_list) dcg_attach ((char *) orig)
#define att_stat_list(orig) (void) dcg_attach ((char *) orig)
#define attach_field_list(orig) (field_list) dcg_attach ((char *) orig)
#define att_field_list(orig) (void) dcg_attach ((char *) orig)
#define attach_vfield_list(orig) (vfield_list) dcg_attach ((char *) orig)
#define att_vfield_list(orig) (void) dcg_attach ((char *) orig)
#define attach_type_list(orig) (type_list) dcg_attach ((char *) orig)
#define att_type_list(orig) (void) dcg_attach ((char *) orig)
#define attach_string_list(orig) (string_list) dcg_attach ((char *) orig)
#define att_string_list(orig) (void) dcg_attach ((char *) orig)

/* Introduce detaching of enumeration types */

/* Introduce detaching of record types */
void detach_def (def *optr);
#define det_def(optr) detach_def(optr)
void detach_stat (stat *optr);
#define det_stat(optr) detach_stat(optr)
void detach_field (field *optr);
#define det_field(optr) detach_field(optr)
void detach_vfield (vfield *optr);
#define det_vfield(optr) detach_vfield(optr)
void detach_type (type *optr);
#define det_type(optr) detach_type(optr)

/* Introduce detaching of lists */
void detach_def_list (def_list *lp);
void detach_stat_list (stat_list *lp);
void detach_field_list (field_list *lp);
void detach_vfield_list (vfield_list *lp);
void detach_type_list (type_list *lp);
void detach_string_list (string_list *lp);

/* Introduce nonrecursive detaching of lists */
void nonrec_detach_def_list (def_list *lp);
void nonrec_detach_stat_list (stat_list *lp);
void nonrec_detach_field_list (field_list *lp);
void nonrec_detach_vfield_list (vfield_list *lp);
void nonrec_detach_type_list (type_list *lp);
void nonrec_detach_string_list (string_list *lp);

/* Introduce list append routines */
def_list append_def_list (def_list l, def el);
stat_list append_stat_list (stat_list l, stat el);
field_list append_field_list (field_list l, field el);
vfield_list append_vfield_list (vfield_list l, vfield el);
type_list append_type_list (type_list l, type el);
string_list append_string_list (string_list l, string el);

/* Introduce void shorthands for list append routines */
#define app_def_list(l,el) (void) append_def_list (l,el)
#define app_stat_list(l,el) (void) append_stat_list (l,el)
#define app_field_list(l,el) (void) append_field_list (l,el)
#define app_vfield_list(l,el) (void) append_vfield_list (l,el)
#define app_type_list(l,el) (void) append_type_list (l,el)
#define app_string_list(l,el) (void) append_string_list (l,el)

/* Introduce list concat routines */
def_list concat_def_list (def_list l1, def_list l2);
stat_list concat_stat_list (stat_list l1, stat_list l2);
field_list concat_field_list (field_list l1, field_list l2);
vfield_list concat_vfield_list (vfield_list l1, vfield_list l2);
type_list concat_type_list (type_list l1, type_list l2);
string_list concat_string_list (string_list l1, string_list l2);

/* Introduce void shorthands for list concat routines */
#define conc_def_list(l1,l2) (void) concat_def_list (l1,l2)
#define conc_stat_list(l1,l2) (void) concat_stat_list (l1,l2)
#define conc_field_list(l1,l2) (void) concat_field_list (l1,l2)
#define conc_vfield_list(l1,l2) (void) concat_vfield_list (l1,l2)
#define conc_type_list(l1,l2) (void) concat_type_list (l1,l2)
#define conc_string_list(l1,l2) (void) concat_string_list (l1,l2)

/* Introduce list insertion routines */
def_list insert_def_list (def_list l, int pos, def el);
stat_list insert_stat_list (stat_list l, int pos, stat el);
field_list insert_field_list (field_list l, int pos, field el);
vfield_list insert_vfield_list (vfield_list l, int pos, vfield el);
type_list insert_type_list (type_list l, int pos, type el);
string_list insert_string_list (string_list l, int pos, string el);

/* Introduce void shorthands for list insertion routines */
#define ins_def_list(l,pos,el) (void) insert_def_list (l,pos,el)
#define ins_stat_list(l,pos,el) (void) insert_stat_list (l,pos,el)
#define ins_field_list(l,pos,el) (void) insert_field_list (l,pos,el)
#define ins_vfield_list(l,pos,el) (void) insert_vfield_list (l,pos,el)
#define ins_type_list(l,pos,el) (void) insert_type_list (l,pos,el)
#define ins_string_list(l,pos,el) (void) insert_string_list (l,pos,el)

/* Introduce list deletion routines */
def_list delete_def_list (def_list l, int pos);
stat_list delete_stat_list (stat_list l, int pos);
field_list delete_field_list (field_list l, int pos);
vfield_list delete_vfield_list (vfield_list l, int pos);
type_list delete_type_list (type_list l, int pos);
string_list delete_string_list (string_list l, int pos);

/* Introduce void shorthands for list deletion routines */
#define del_def_list(l,pos) (void) delete_def_list (l,pos)
#define del_stat_list(l,pos) (void) delete_stat_list (l,pos)
#define del_field_list(l,pos) (void) delete_field_list (l,pos)
#define del_vfield_list(l,pos) (void) delete_vfield_list (l,pos)
#define del_type_list(l,pos) (void) delete_type_list (l,pos)
#define del_string_list(l,pos) (void) delete_string_list (l,pos)

/* Equality test for types */
int cmp_def (def a, def b);
int cmp_stat (stat a, stat b);
int cmp_field (field a, field b);
int cmp_vfield (vfield a, vfield b);
int cmp_type (type a, type b);

/* Comparison tests for lists */
int cmp_def_list (def_list a, def_list b);
int cmp_stat_list (stat_list a, stat_list b);
int cmp_field_list (field_list a, field_list b);
int cmp_vfield_list (vfield_list a, vfield_list b);
int cmp_type_list (type_list a, type_list b);
int cmp_string_list (string_list a, string_list b);

/* Equality shorthands for types */
#define equal_def(a,b) !cmp_def ((a),(b))
#define equal_stat(a,b) !cmp_stat ((a),(b))
#define equal_field(a,b) !cmp_field ((a),(b))
#define equal_vfield(a,b) !cmp_vfield ((a),(b))
#define equal_type(a,b) !cmp_type ((a),(b))

/* Equality shorthands for lists */
#define equal_def_list(a,b) !cmp_def_list((a),(b))
#define equal_stat_list(a,b) !cmp_stat_list((a),(b))
#define equal_field_list(a,b) !cmp_field_list((a),(b))
#define equal_vfield_list(a,b) !cmp_vfield_list((a),(b))
#define equal_type_list(a,b) !cmp_type_list((a),(b))
#define equal_string_list(a,b) !cmp_string_list((a),(b))

/* Estimating printing of types */
int est_def (def old);
int est_stat (stat old);
int est_field (field old);
int est_vfield (vfield old);
int est_type (type old);

/* Estimate printing of lists */
int est_def_list (def_list old);
int est_stat_list (stat_list old);
int est_field_list (field_list old);
int est_vfield_list (vfield_list old);
int est_type_list (type_list old);
int est_string_list (string_list old);

void ppp_def (FILE *f, int horiz, int ind, def old);
void ppp_stat (FILE *f, int horiz, int ind, stat old);
void ppp_field (FILE *f, int horiz, int ind, field old);
void ppp_vfield (FILE *f, int horiz, int ind, vfield old);
void ppp_type (FILE *f, int horiz, int ind, type old);

/* Introduce pretty printing of lists */
void ppp_def_list (FILE *f, int horiz, int ind, def_list old);
void ppp_stat_list (FILE *f, int horiz, int ind, stat_list old);
void ppp_field_list (FILE *f, int horiz, int ind, field_list old);
void ppp_vfield_list (FILE *f, int horiz, int ind, vfield_list old);
void ppp_type_list (FILE *f, int horiz, int ind, type_list old);
void ppp_string_list (FILE *f, int horiz, int ind, string_list old);

/* Introduce pretty printing shorthands */
#define pp_def(f,x) ppp_def (f,0,0,x)
#define pp_stat(f,x) ppp_stat (f,0,0,x)
#define pp_field(f,x) ppp_field (f,0,0,x)
#define pp_vfield(f,x) ppp_vfield (f,0,0,x)
#define pp_type(f,x) ppp_type (f,0,0,x)

/* Introduce pretty printing shorthands for lists */
#define pp_def_list(f,x) ppp_def_list (f,0,0,x)
#define pp_stat_list(f,x) ppp_stat_list (f,0,0,x)
#define pp_field_list(f,x) ppp_field_list (f,0,0,x)
#define pp_vfield_list(f,x) ppp_vfield_list (f,0,0,x)
#define pp_type_list(f,x) ppp_type_list (f,0,0,x)
#define pp_string_list(f,x) ppp_string_list (f,0,0,x)

/* Introduce saving of types */
void save_def (BinFile bf, def old);
void save_stat (BinFile bf, stat old);
void save_field (BinFile bf, field old);
void save_vfield (BinFile bf, vfield old);
void save_type (BinFile bf, type old);

/* Introduce saving of lists */
void save_def_list (BinFile bf, def_list l);
void save_stat_list (BinFile bf, stat_list l);
void save_field_list (BinFile bf, field_list l);
void save_vfield_list (BinFile bf, vfield_list l);
void save_type_list (BinFile bf, type_list l);
void save_string_list (BinFile bf, string_list l);

/* Introduce loading of types */
void load_def (BinFile bf, def *x);
void load_stat (BinFile bf, stat *x);
void load_field (BinFile bf, field *x);
void load_vfield (BinFile bf, vfield *x);
void load_type (BinFile bf, type *x);

/* Introduce loading of lists */
void load_def_list (BinFile bf, def_list *l);
void load_stat_list (BinFile bf, stat_list *l);
void load_field_list (BinFile bf, field_list *l);
void load_vfield_list (BinFile bf, vfield_list *l);
void load_type_list (BinFile bf, type_list *l);
void load_string_list (BinFile bf, string_list *l);

/* Introduce recursive duplication of types */
def rdup_def (def old);
stat rdup_stat (stat old);
field rdup_field (field old);
vfield rdup_vfield (vfield old);
type rdup_type (type old);

/* Introduce recursive copying of lists */
def_list rdup_def_list (def_list old);
stat_list rdup_stat_list (stat_list old);
field_list rdup_field_list (field_list old);
vfield_list rdup_vfield_list (vfield_list old);
type_list rdup_type_list (type_list old);
string_list rdup_string_list (string_list old);

/* Introduce recursive refcount checking of types */
void check_not_freed_def (def curr);
void check_not_freed_stat (stat curr);
void check_not_freed_field (field curr);
void check_not_freed_vfield (vfield curr);
void check_not_freed_type (type curr);

/* Introduce recursive refcount checking of lists */
void check_not_freed_def_list (def_list curr);
void check_not_freed_stat_list (stat_list curr);
void check_not_freed_field_list (field_list curr);
void check_not_freed_vfield_list (vfield_list curr);
void check_not_freed_type_list (type_list curr);
void check_not_freed_string_list (string_list curr);

#endif /* IncDcgCode */
