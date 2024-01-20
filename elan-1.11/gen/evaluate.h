/*
   File: evaluate.h
   Evaluates static expressions
   Checks synonym value definitions

   CVS ID: "$Id: evaluate.h,v 1.3 2005/02/25 20:08:35 marcs Exp $"
*/
#ifndef IncEvaluate
#define IncEvaluate

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
int evaluate_intval (node n);
void evaluate_synonym_values (decl_list dl);

#endif /* IncEvaluate */
