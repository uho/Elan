/*
   File: imc_opt.c
   Optimizes intermediate code

   CVS ID: "$Id: imc_opt.c,v 1.5 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "options.h"
#include "imc_opt.h"

void intermediate_code_optimization ()
{ if (!optimize_code) return;
  dcg_warning (0, "   optimizing intermediate code...");
}
