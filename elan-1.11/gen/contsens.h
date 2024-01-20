/*
   File: checker.h
   Does context dependent analysis

   CVS ID: "$Id: contsens.h,v 1.4 2005/02/25 20:08:35 marcs Exp $"
*/
#ifndef IncContSens
#define IncContSens

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported packet fname */
extern string contsens_fname;

/* exported code */
void context_sensitive_analysis (packet_list all_pks);
void contsens_error (int line, int column, char *format, ...);
void contsens_warning (int line, int column, char *format, ...);

#endif /* IncContSens */
