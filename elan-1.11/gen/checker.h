/*
   File: checker.h
   Does identification, type and access checking 

   CVS ID: "$Id: checker.h,v 1.4 2005/02/25 20:08:35 marcs Exp $"
*/
#ifndef IncChecker
#define IncChecker

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
void check_paragraph (node_list par, type t, e_access acc, type *rtype, e_access *racc);

#endif /* IncChecker */
