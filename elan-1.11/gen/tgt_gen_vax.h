/*
   File: tgt_gen_vax.h
   Generates VAX11 machine code

   CVS ID: "$Id: tgt_gen_vax.h,v 1.4 2005/05/19 15:48:42 marcs Exp $"
*/
#ifndef IncTgtGenVax
#define IncTgtGenVax

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
int vax_size_from_type (type t);
int vax_regsize_from_arg (pdecl pd);
void vax_code_generation ();

#endif /* IncTgtGenVax */
