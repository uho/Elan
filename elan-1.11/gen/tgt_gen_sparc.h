/*
   File: tgt_gen_sparc.h
   Generates Sparc machine code

   CVS ID: "$Id: tgt_gen_sparc.h,v 1.4 2005/05/19 15:48:41 marcs Exp $"
*/
#ifndef IncTgtGenSparc
#define IncTgtGenSparc

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
int sparc_size_from_type (type t);
int sparc_regsize_from_arg (pdecl pd);
void sparc_code_generation ();

#endif /* IncTgtGenSparc */
