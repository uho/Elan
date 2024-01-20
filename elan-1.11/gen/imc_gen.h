/*
   File: imc_gen.h
   Generates intermediate code

   CVS ID: "$Id: imc_gen.h,v 1.3 2005/02/25 20:08:35 marcs Exp $"
*/
#ifndef IncImcGen
#define IncImcGen

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
void intermediate_code_generation (packet_list all_pks);

#endif /* IncImcGen */
