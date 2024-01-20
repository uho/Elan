/*
   File: decl_tree.h
   Handles trees of declarations

   CVS ID: "$Id: decl_tree.h,v 1.4 2011/09/05 19:57:09 marcs Exp $"
*/
#ifndef IncDeclTree
#define IncDeclTree

/* standard includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_dump.h>

/* local includes */
#include "elan_ast.h"

/* introduce type and nil definitions */
typedef struct decl_tree_rec *decl_tree;
struct decl_tree_rec
	{ string tag;
	  decl_list decls;
	  decl_tree left;
	  decl_tree right;
	};
#define decl_tree_nil ((decl_tree) NULL)

/* introduce necessary routines and shorthands */
#define new_decl_tree() decl_tree_nil
#define attach_decl_tree(orig) (decl_tree) dcg_attach ((char *) orig)
#define att_decl_tree(orig) (void) dcg_attach ((char *) orig)
void detach_decl_tree (decl_tree *optr);
#define det_decl_tree(optr) detach_decl_tree(optr)
#define equal_decl_tree(a,b) ((a) == (b))

/* supplementary routines and shorthands */
#define est_decl_tree(x) MAXWIDTH
void ppp_decl_tree (FILE *f, int horiz, int ind, decl_tree old);
#define pp_decl_tree(f,x) ppp_decl_tree (f,0,0,x)
void save_decl_tree (BinFile bf, decl_tree old);
void load_decl_tree (BinFile bf, decl_tree *x);

/* specific routines and shorthands */
void enter_decl_tree (decl_tree *root, decl d);
void enter_decls_into_tree (decl_tree *root, decl_list dl);
decl_list lookup_decl_tree (decl_tree root, string tag);

#endif /* IncDeclTree */
