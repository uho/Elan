/*
   File: tgt_gen_amd64.h
   Generates AMD64 machine code

   CVS ID: "$Id: tgt_gen_amd64.h,v 1.1 2006/12/24 13:27:28 marcs Exp $"
*/
#ifndef IncTgtGenAmd64
#define IncTgtGenAmd64

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
int amd64_size_from_type (type t);
int amd64_regsize_from_arg (pdecl pd);
void amd64_code_generation ();

#endif /* IncTgtGenAmd64 */
