/*
   File: contfree.h
   Does context free analysis and packet searches

   CVS ID: "$Id: contfree.h,v 1.4 2005/02/25 20:08:35 marcs Exp $"
*/
#ifndef IncContFree
#define IncContFree

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
void parse_and_save_source ();
void context_free_analysis (packet_list *all_pks);

#endif /* IncContFree */
