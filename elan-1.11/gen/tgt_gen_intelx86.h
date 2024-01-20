/*
   File: tgt_gen_intelx86.h
   Generates Intelx86 machine code

   CVS ID: "$Id: tgt_gen_intelx86.h,v 1.4 2005/05/19 15:48:41 marcs Exp $"
*/
#ifndef IncTgtGenIntelX86
#define IncTgtGenIntelX86

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
int intelx86_size_from_type (type t);
int intelx86_regsize_from_arg (pdecl pd);
void intelx86_code_generation ();

#endif /* IncTgtGenIntelX86 */
