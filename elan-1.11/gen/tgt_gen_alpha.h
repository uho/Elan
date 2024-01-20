/*
   File: tgt_gen_alpha.h
   Generates Alpha machine code

   CVS ID: "$Id: tgt_gen_alpha.h,v 1.4 2005/05/19 15:48:41 marcs Exp $"
*/
#ifndef IncTgtGenAlpha
#define IncTgtGenAlpha

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
int alpha_size_from_type (type t);
int alpha_regsize_from_arg (pdecl pd);
void alpha_code_generation ();

#endif /* IncTgtGenAlpha */
