/*
   File: tgt_gen_machdep.h
   Decides for which target to code

   CVS ID: "$Id: tgt_gen_machdep.h,v 1.5 2006/12/24 13:27:28 marcs Exp $"
*/
#ifndef IncTgtGenMachdep
#define IncTgtGenMachdep

/* libdcg includes */
#include <dcg.h>
#include <dcg_string.h>

typedef enum 
	{ TGT_UNDEF, TGT_SPARC, TGT_INTELx86,
	  TGT_ALPHA, TGT_VAX, TGT_AMD64,
	} cpu_type;

/* exported variables */
extern string local_label_prefix;
extern string asm_underscore;
extern cpu_type tcpu;
extern int old_os;

/* exported code */
void determine_target_dependencies ();

#endif /* IncTgtGenMachdep */
