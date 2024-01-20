/*
   File: type_table.h
   Handles types

   CVS ID: "$Id: type_table.h,v 1.4 2005/05/19 15:48:42 marcs Exp $"
*/
#ifndef IncTypeTable
#define IncTypeTable

/* standard includes */
#include <stdio.h>

/* support libdcg and local includes */
#include <dcg.h>
#include <dcg_string.h>
#include "elan_ast.h"

/* The table of all types */
extern type_info_list type_table;

#define t_unknown	type_table -> array[0] -> typ
#define t_error		type_table -> array[1] -> typ
#define t_any_non_void  type_table -> array[2] -> typ
#define t_any_row	type_table -> array[3] -> typ
#define t_any_struct	type_table -> array[4] -> typ
#define t_any_object	type_table -> array[5] -> typ
#define t_addr		type_table -> array[6] -> typ
#define t_int		type_table -> array[7] -> typ
#define t_bool	 	type_table -> array[8] -> typ
#define t_real	 	type_table -> array[9] -> typ
#define t_text		type_table -> array[10] -> typ
#define t_void		type_table -> array[11] -> typ
#define t_file		type_table -> array[12] -> typ
#define t_niltype	type_table -> array[13] -> typ
#define nr_precoded_types 14

/* exported code */
type construct_procedure_type (type rtyp, decl_list fpars);
pdecl_list construct_pdecls (decl_list fpars);
string convert_type_text (type t);
int equivalent_type (type t1, type t2);
int actual_matches_formal_type (type at, type ft);
int actual_match_formal_parms (pdecl_list atl, pdecl_list ftl);
int is_a_procedure_type (type t, pdecl_list *parms, type *rtype);
int is_a_floating_type (type t);
int is_a_niltype (type t);
int feebly_coercable (type at, type ft);
void evaluate_type_declarations (decl_list dl);
void unify_decl_types (decl_list dl);
void init_type_table ();
void register_field_offsets ();

#endif /* IncTypeTable */
