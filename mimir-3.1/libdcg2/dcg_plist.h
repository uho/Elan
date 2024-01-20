/*
   File: dcg_plist.h
   Generated on Thu Aug 23 22:26:45 2012
*/
#ifndef IncDcg_plist
#define IncDcg_plist

/* standard includes */
#include <stdio.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_binfile.h>

/* Introduce record type definitions */

/* Introduce type_list definitions */
typedef struct str_int_list *int_list;
typedef struct str_string_list *string_list;

/* Introduce enumeration types */
/* Include imported defs */

/* Introduce record nils */

/* Introduce type_list nils */
#define int_list_nil ((int_list) NULL)
#define string_list_nil ((string_list) NULL)

/* Introduce enumeration nils */

/* introduce constructors */

/* Introduce constructor tags */
/* Introduce structure of type int_list */
struct str_int_list
{ int size;
  int room;
  int *array;
};

/* Introduce structure of type string_list */
struct str_string_list
{ int size;
  int room;
  string *array;
};

/* Introduce list allocation routines */
int_list init_int_list (int room);
string_list init_string_list (int room);

/* Introduce room allocation routines */
void room_int_list (int_list l, int room);
void room_string_list (string_list l, int room);

/* Introduce list allocation shorthands */
#define new_int_list() init_int_list(2)
#define new_string_list() init_string_list(2)

/* Introduce type allocation routines */

/* Introduce attaching of types */

/* Introduce attaching of lists */
#define attach_int_list(orig) (int_list) dcg_attach ((char *) orig)
#define att_int_list(orig) (void) dcg_attach ((char *) orig)
#define attach_string_list(orig) (string_list) dcg_attach ((char *) orig)
#define att_string_list(orig) (void) dcg_attach ((char *) orig)

/* Introduce detaching of enumeration types */

/* Introduce detaching of record types */

/* Introduce detaching of lists */
void detach_int_list (int_list *lp);
void detach_string_list (string_list *lp);

/* Introduce nonrecursive detaching of lists */
void nonrec_detach_int_list (int_list *lp);
void nonrec_detach_string_list (string_list *lp);

/* Introduce list append routines */
int_list append_int_list (int_list l, int el);
string_list append_string_list (string_list l, string el);

/* Introduce void shorthands for list append routines */
#define app_int_list(l,el) (void) append_int_list (l,el)
#define app_string_list(l,el) (void) append_string_list (l,el)

/* Introduce list concat routines */
int_list concat_int_list (int_list l1, int_list l2);
string_list concat_string_list (string_list l1, string_list l2);

/* Introduce void shorthands for list concat routines */
#define conc_int_list(l1,l2) (void) concat_int_list (l1,l2)
#define conc_string_list(l1,l2) (void) concat_string_list (l1,l2)

/* Introduce list insertion routines */
int_list insert_int_list (int_list l, int pos, int el);
string_list insert_string_list (string_list l, int pos, string el);

/* Introduce void shorthands for list insertion routines */
#define ins_int_list(l,pos,el) (void) insert_int_list (l,pos,el)
#define ins_string_list(l,pos,el) (void) insert_string_list (l,pos,el)

/* Introduce list deletion routines */
int_list delete_int_list (int_list l, int pos);
string_list delete_string_list (string_list l, int pos);

/* Introduce void shorthands for list deletion routines */
#define del_int_list(l,pos) (void) delete_int_list (l,pos)
#define del_string_list(l,pos) (void) delete_string_list (l,pos)

/* Equality test for types */

/* Comparison tests for lists */
int cmp_int_list (int_list a, int_list b);
int cmp_string_list (string_list a, string_list b);

/* Equality shorthands for types */

/* Equality shorthands for lists */
#define equal_int_list(a,b) !cmp_int_list((a),(b))
#define equal_string_list(a,b) !cmp_string_list((a),(b))

/* Estimating printing of types */

/* Estimate printing of lists */
int est_int_list (int_list old);
int est_string_list (string_list old);


/* Introduce pretty printing of lists */
void ppp_int_list (FILE *f, int horiz, int ind, int_list old);
void ppp_string_list (FILE *f, int horiz, int ind, string_list old);

/* Introduce pretty printing shorthands */

/* Introduce pretty printing shorthands for lists */
#define pp_int_list(f,x) ppp_int_list (f,0,0,x)
#define pp_string_list(f,x) ppp_string_list (f,0,0,x)

/* Introduce saving of types */

/* Introduce saving of lists */
void save_int_list (BinFile bf, int_list l);
void save_string_list (BinFile bf, string_list l);

/* Introduce loading of types */

/* Introduce loading of lists */
void load_int_list (BinFile bf, int_list *l);
void load_string_list (BinFile bf, string_list *l);

/* Introduce recursive duplication of types */

/* Introduce recursive copying of lists */
int_list rdup_int_list (int_list old);
string_list rdup_string_list (string_list old);

/* Introduce recursive refcount checking of types */

/* Introduce recursive refcount checking of lists */
void check_not_freed_int_list (int_list curr);
void check_not_freed_string_list (string_list curr);

#endif /* IncDcg_plist */
