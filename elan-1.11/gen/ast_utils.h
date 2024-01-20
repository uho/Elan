/*
   File: ast_utils.h
   Defines some access functions on ast structures

   CVS ID: "$Id: ast_utils.h,v 1.5 2005/05/19 15:48:40 marcs Exp $"
*/
#ifndef IncAstUtils
#define IncAstUtils

/* libdcg and local includes */
#include <dcg.h>
#include "elan_ast.h"

/* exported code */
type type_from_decl (decl d);
type type_from_value (value v);
e_access access_from_decl (decl d);

#endif /* IncAstUtils */
