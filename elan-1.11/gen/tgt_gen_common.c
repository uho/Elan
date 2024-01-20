/*
   File: tgt_gen_common.c
   Defines the common code generation routines

   CVS ID: "$Id: tgt_gen_common.c,v 1.7 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <unistd.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "options.h"
#include "elan_ast.h"
#include "elan_imc.h"
#include "imc_utils.h"
#include "tgt_gen_machdep.h"
#include "tgt_gen_common.h"

/* TMPDIR is the usual environment variable to consult */ 
#ifndef TMPDIR
#define TMPDIR "TMPDIR"
#endif

/* Define the usual default temporary directory */
#ifndef DEFAULT_TMP_DIR
#define DEFAULT_TMP_DIR "/tmp"
#endif

static FILE *cod;
void open_assembler_file ()
{ char buf [MAXPATHLEN+1];
  if (!generate_assembler)		/* ! -S */
    { /* We do not want to leave the generated Asssembler code in place */
      string temp_dir = getenv (TMPDIR);
      if (temp_dir == NULL) temp_dir = DEFAULT_TMP_DIR;
      sprintf (buf, "%s/elan_%d.s", temp_dir, (int) getpid ());
    }
  else if (target_fname != NULL)	/* -o name given */
    strcpy (buf, target_fname);	
  else sprintf (buf, "%s.s", basename);
  assembler_fname = new_string (buf);
  cod = fopen (buf, "w");
  if (cod == NULL) dcg_panic ("could not open output file '%s'", buf);
}

void sync_assembler_file ()
{ fflush (cod);
}

void close_assembler_file ()
{ fclose (cod);
}

void code_string (char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  fprintf (cod, "%s", buf);
}

void code_line (char *format, ...)
{ char buf[MAXSTRLEN];
  va_list arg_ptr;
  va_start (arg_ptr, format);
  vsprintf (buf, format, arg_ptr);
  va_end (arg_ptr);
  fprintf (cod, "%s\n", buf);
}

void code_newline ()
{ fputc ('\n', cod);
}

void code_preamble ()
{ code_line ("\t/* Code derived from %s */", basename);
  if (generate_stabs)
    { code_line ("\t.stabs\t\"%s/\",100,0,0,Ltext0", act_source_dir);
      code_line ("\t.stabs\t\"%s\",100,0,0,Ltext0", act_source_name);
      code_line ("\t.text");
      code_line ("Ltext0:");
    };
}

/*
   Routines common to all code generators
*/
void code_quoted_string (string str)
{ char *ptr;
  fputc ('"', cod);
  for (ptr = str; *ptr; ptr++)
    { char ch = *ptr;
      if (ch < ' ') fprintf (cod, "\\%03o", ch & 0377);
      else fputc (ch, cod);
    };
  fputc ('"', cod);
}

void code_block_label (block b)
{ tuple init = b -> anchor;
  switch (init -> opc)
    { case start_program:
      case enter_procedure: return;
      default: code_line ("%sBB%d:", local_label_prefix, b -> bnr);
    }
}

void code_comment_tuple (tuple t)
{ string opc = convert_opcode_text (t -> opc);
  string op1 = convert_operand_text (t -> op1);
  string op2 = convert_operand_text (t -> op2);
  string dst = convert_operand_text (t -> dst);
  code_line ("\t/* [ %s, %s, %s, %s ] */", opc, op1, op2, dst);
  detach_string (&op1);
  detach_string (&op2);
  detach_string (&dst);
}

void code_proc_label (string s)
{ code_string ("%s%s", asm_underscore, s);
}

void code_uproc_label (int pnr)
{ code_string ("P%d", pnr);
}

void code_called_proc (oprd opd)
{ switch (opd -> tag)
    { case TAGRts: code_proc_label (opd -> Rts.proc); break;
      case TAGProc: code_uproc_label (opd -> Proc.pnr); break; 
      default: dcg_bad_tag (opd -> tag, "code_called_proc");
    };
}

void code_jump_target (oprd opd)
{ if (opd -> tag != TAGBlock)
    dcg_internal_error ("code_jump_target");
  code_string ("%sBB%d", local_label_prefix, opd -> Block.bnr);
}

/*
   Local label administration
*/
static int local_labelnr;
int new_local_label ()
{ int new_label = local_labelnr;
  local_labelnr++;
  return (new_label);
}

static int const_labelnr;
int new_const_label ()
{ int new_label = const_labelnr;
  const_labelnr++;
  return (new_label);
}

void init_common_code_generation ()
{ local_labelnr = 1;
  const_labelnr = 1;
}
