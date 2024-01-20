/*
   File: pre_checker.h
   Collects declarations with a common tag
   Checks conflicting double declarations
   Checks refinement applications

   CVS ID: "$Id: pre_checker.h,v 1.3 2005/02/25 20:08:36 marcs Exp $"
*/
#ifndef IncPreChecker
#define IncPreChecker

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
void pre_check_packet (packet pk);

#endif /* IncPreChecker */
