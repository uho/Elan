/*
   File: symbol_table.h
   Handles the symbol table and maintains the extended scope

   CVS ID: "$Id: symbol_table.h,v 1.5 2011/09/05 19:57:09 marcs Exp $"
*/
#ifndef IncSymbolTable
#define IncSymbolTable

/* standard includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_alloc.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"

/* introduce symbol stack type */
typedef struct symbol_stack_rec *symbol_stack;
struct symbol_stack_rec
{ decl_list decls;
  symbol_stack tail;
};
#define symbol_stack_nil ((symbol_stack) NULL)

/* specific routines */
void init_symbol_table ();
void push_decls_into_symbol_table (string tag, decl_list decls);
void pop_decls_from_symbol_table (string tag);
decl_list try_make_extended_scope_entry (string tag);
void merge_decls_into_symbol_table (string tag, decl_list defs);
symbol_stack lookup_symbol_stack (string tag);
decl_list lookup_symbol (string tag);

#endif /* IncSymbolTable */
