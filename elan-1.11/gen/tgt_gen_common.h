/*
   File: tgt_gen_common.h
   Defines the common code generation routines

   CVS ID: "$Id: tgt_gen_common.h,v 1.4 2005/03/14 22:19:54 marcs Exp $"
*/
#ifndef IncTgtGenCommon
#define IncTgtGenCommon

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"
#include "elan_imc.h"

/* general coding utilities */
void open_assembler_file ();
void sync_assembler_file ();
void close_assembler_file ();
void code_string (char *format, ...);
void code_line (char *format, ...);
void code_newline ();

/* general coding */
void code_quoted_string (string str);
void code_preamble ();
void code_comment_tuple (tuple t);
void code_block_label (block b);
void code_uproc_label (int pnr);
void code_proc_label (string s);
void code_called_proc (oprd opd);
void code_jump_target (oprd opd);

int new_local_label ();
int new_const_label ();
void init_common_code_generation ();

#endif /* IncTgtGenCommon */
