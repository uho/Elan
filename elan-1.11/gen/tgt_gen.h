/*
   File: tgt_gen.h
   Interfaces with the actual target code generators

   CVS ID: "$Id: tgt_gen.h,v 1.4 2005/05/19 15:48:41 marcs Exp $"
*/
#ifndef IncTgtGen
#define IncTgtGen

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
void target_code_generation ();
int size_from_type (type t);
int indirect_size_from_type (type t);
int regsize_from_arg (pdecl pd);

#endif /* IncTgtGen */
