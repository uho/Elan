/*
   File: main.c
   Driver for elan compiler

   CVS ID: "$Id: main.c,v 1.9 2011/09/02 12:50:29 marcs Exp $"
*/

/* global includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_alloc.h>

/* local includes */
#include "elan_ast.h"
#include "options.h"
#include "contfree.h"
#include "contsens.h"
#include "type_table.h"
#include "tgt_gen_machdep.h"
#include "imc_gen.h"
#include "imc_opt.h"
#include "tgt_gen.h"
#include "backend.h"

static void compile_source ()
{ packet_list all_pks;
  context_free_analysis (&all_pks);
  context_sensitive_analysis (all_pks);
  determine_target_dependencies ();
  intermediate_code_generation (all_pks);
  intermediate_code_optimization ();
  target_code_generation ();  
  assemble_and_link ();
}

int main (int argc, char **argv)
{ dcg_init_error (stderr, "elancc", NULL);
  dcg_init_alloc ("elancc", 0);
  init_options ();
  parse_command_line (argc, argv);
  init_type_table ();
  if (preparse_packet)
    parse_and_save_source ();
  else if (parse_only)
    { packet_list all_pks;
      context_free_analysis (&all_pks);
    }
  else compile_source ();
  return (0);
}
