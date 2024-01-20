/*
   File: parser.h
   Defines the ELAN parser

   CVS ID: "$Id: parser.h,v 1.4 2005/02/25 20:08:36 marcs Exp $"
*/
#ifndef IncParser
#define IncParser

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
void should_be_packet (string pname, packet *pk);
void should_be_main_packet (packet *pk);

#endif /* IncParser */
