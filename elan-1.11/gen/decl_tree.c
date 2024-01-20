/*
   File: decl_tree.c
   Handles trees of declarations

   CVS ID: "$Id: decl_tree.c,v 1.6 2011/10/05 10:50:38 marcs Exp $"
*/

/* standard includes */
#include <stdio.h>
#include <string.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>
#include <dcg_dump.h>
#include <dcg_binfile.h>

/* local includes */
#include "elan_ast.h"
#include "decl_tree.h"

/* introduce necessary routines and shorthands */
void detach_decl_tree (decl_tree *optr)
{ decl_tree old = (decl_tree) dcg_predetach ((void **) optr);
  if (old == decl_tree_nil) return;
  detach_string (&old -> tag);
  detach_decl_list (&old -> decls);
  detach_decl_tree (&old -> left);
  detach_decl_tree (&old -> right);
  dcg_detach ((void **) &old);
}

/* supplementary routines and shorthands */
static void my_ppp_decl_tree (FILE *f, int ind, decl_tree old)
{ if (old == decl_tree_nil) return;
  my_ppp_decl_tree (f, ind, old -> left);
  pppdelim (f, 0, ind, '<');
  ppp_string (f, 1, ind + 2, old -> tag);
  pppdelim (f, 0, ind, ',');
  ppp_decl_list (f, 0, ind + 2, old -> decls);
  pppdelim (f, 0, ind, '>');
  my_ppp_decl_tree (f, ind, old -> right);
}

void ppp_decl_tree (FILE *f, int horiz, int ind, decl_tree old)
{ pppdelim (f, 0, ind, '{');
  my_ppp_decl_tree (f, ind + 2, old);
  pppdelim (f, 0, ind, '}');
}

void save_decl_tree (BinFile bf, decl_tree old)
{
}

void load_decl_tree (BinFile bf, decl_tree *x)
{ 
}

/* specific routines and shorthands */
void enter_decl_tree (decl_tree *root, decl d)
{ decl_tree current = *root;
  int cond;
  if (current == decl_tree_nil)
    { decl_tree new = (decl_tree) dcg_malloc (sizeof (struct decl_tree_rec));
      new -> tag = attach_string (d -> name);
      new -> decls = new_decl_list ();
      app_decl_list (new -> decls, attach_decl (d));
      new -> left = decl_tree_nil;
      new -> right = decl_tree_nil;
      *root = new;
      return;
    };
  cond = strcmp (d -> name, current -> tag);
  if (cond < 0) enter_decl_tree (&current -> left, d);
  else if (cond > 0) enter_decl_tree (&current -> right, d);
  else app_decl_list (current -> decls, attach_decl (d));
}

void enter_decls_into_tree (decl_tree *root, decl_list dl)
{ int ix;
  for (ix = 0; ix < dl -> size; ix++)
     enter_decl_tree (root, dl -> array[ix]);
}

decl_list lookup_decl_tree (decl_tree root, string tag)
{ int cond;
  if (root == decl_tree_nil) return (decl_list_nil);
  cond = strcmp (tag, root -> tag);
  if (cond < 0) return (lookup_decl_tree (root -> left, tag));
  else if (cond > 0) return (lookup_decl_tree (root -> right, tag));
  else return (root -> decls);
}
