/*
   File: symbol_table.c
   Handles the symbol table
   Maintains the extended scope

   CVS ID: "$Id: symbol_table.c,v 1.7 2011/09/05 19:57:09 marcs Exp $"
*/

/* standard includes */
#include <stdio.h>
#include <string.h>

/* support lib includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_alloc.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "symbol_table.h"

/* introduce type and nil definitions */
/* may be balance the symbol table later */
typedef struct symbol_table_rec *symbol_table;
struct symbol_table_rec
{ string tag;
  symbol_stack stack;
  symbol_table left;
  symbol_table right;
};

#define symbol_table_nil ((symbol_table) NULL)
static symbol_table root;

/*
   Lookup the symbol table entry corresponding to tag
*/
static symbol_table lookup_in_symbol_table (string tag)
{ symbol_table old = root;
  while (old != symbol_table_nil)
    { int cond = strcmp (tag, old -> tag);
      if (cond < 0) old = old -> left;
      else if (cond > 0) old = old -> right;
      else return (old);
    };
  return (symbol_table_nil);
}

/*
   Locate the symbol table entry corresponding to tag
   and push the declaration stack onto this symbol's stack
   This code is used upon entering a new scope.
*/
void push_decls_into_symbol_table (string tag, decl_list decls)
{ symbol_table *ptr = &root;
  while (1)
    { symbol_table new = *ptr;
      int cond;
      if (new == symbol_table_nil)
	{ /* This was a new entry in the symbol table */
	  /* Create a stack for this tag */
	  symbol_stack new_stack = (symbol_stack) dcg_malloc (sizeof (struct symbol_stack_rec));
	  new_stack -> decls = attach_decl_list (decls);
	  new_stack -> tail = symbol_stack_nil;

	  /* Create a symbol table entry for this tag */
	  new = (symbol_table) dcg_malloc (sizeof (struct symbol_table_rec));
	  new -> tag = attach_string (tag);
	  new -> stack = new_stack;
	  new -> left = symbol_table_nil;
	  new -> right = symbol_table_nil;
	  *ptr = new;
	  return;
	};

      cond = strcmp (tag, new -> tag);
      if (cond < 0) ptr = &new -> left;
      else if (cond > 0) ptr = &new -> right;
      else
	{ /* There was already an entry, push on its stack */
	  symbol_stack new_stack = (symbol_stack) dcg_malloc (sizeof (struct symbol_stack_rec));
	  new_stack -> decls = attach_decl_list (decls);
	  new_stack -> tail = new -> stack;
	  new -> stack = new_stack;
	  return;
	};
    };
}

/*
   Pop one scope from the symbol stack belonging to this tag
   This code is called upon leaving a scope
*/
void pop_decls_from_symbol_table (string tag)
{ symbol_table old = lookup_in_symbol_table (tag);
  symbol_stack new;
  if (old == symbol_table_nil)
    dcg_internal_error ("pop_decls_from_symbol_table");
  if (old -> stack == symbol_stack_nil)
    dcg_internal_error ("pop_decls_from_symbol_table");
  detach_decl_list (&old -> stack -> decls);
  new = old -> stack -> tail;
  dcg_detach ((void **) &old -> stack);
  old -> stack = new;
}

/*
   Try and make an extended scope entry for tag
*/
decl_list try_make_extended_scope_entry (string tag)
{ /* Lookup the symbol table entry, which must exist */
  symbol_table st = lookup_in_symbol_table (tag);
  if (st == symbol_table_nil)
    dcg_internal_error ("try_make_extended_scope_entry");
  if (st -> stack == symbol_stack_nil)
    { /* A fresh entry for the extended scope is needed */
      symbol_stack new_stack = (symbol_stack) dcg_malloc (sizeof (struct symbol_stack_rec));
      new_stack -> decls = new_decl_list ();
      new_stack -> tail = symbol_stack_nil;
      st -> stack = new_stack;
      return (new_stack -> decls);
    }
  else
    { /*
	 Some declarations with this tag already existed in the extended scope
      */
      symbol_stack old_stack = st -> stack;
      if (old_stack -> tail != symbol_stack_nil)
	dcg_internal_error ("try_make_extended_scope_entry");
      return (old_stack -> decls);
    };
}

/*
   This routine merges a declaration into the current (extended scope)
*/
void merge_decls_into_symbol_table (string tag, decl_list defs)
{ symbol_table st = lookup_in_symbol_table (tag);
  if (st == symbol_table_nil)
    dcg_internal_error ("merge_decls_into_symbol_table");
  if (st -> stack == symbol_stack_nil)
    { symbol_stack new_stack = (symbol_stack) dcg_malloc (sizeof (struct symbol_stack_rec));
      new_stack -> decls = attach_decl_list (defs);
      new_stack -> tail = symbol_stack_nil;
      st -> stack = new_stack;
    }
  else
    { decl_list dl = st -> stack -> decls;
      int ix;
      for (ix = 0; ix < defs -> size; ix++)
	app_decl_list (dl, attach_decl (defs -> array[ix]));
    };
}

/*
   Convenience routines for identification and initialization
*/
symbol_stack lookup_symbol_stack (string tag)
{ symbol_table old = lookup_in_symbol_table (tag);
  if (old == symbol_table_nil) return (symbol_stack_nil);
  return (old -> stack);
}

decl_list lookup_symbol (string tag)
{ symbol_stack stack = lookup_symbol_stack (tag);
  if (stack == symbol_stack_nil) return (decl_list_nil);
  return (stack -> decls);
}

void init_symbol_table ()
{ root = symbol_table_nil;
}
