/*
   File: options.h
   Defines flags and parameters of the elan compiler
   It also specifies some compiler defaults

   CVS ID: "$Id: options.h,v 1.8 2011/08/13 15:58:54 marcs Exp $"
*/
#ifndef IncOptions
#define IncOptions

/* libdcg and local includes */
#include <dcg.h>
#include <dcg_string.h>
#include "elan_ast.h"

/* exported flags and pragmats */
extern int iris_compatibility;
extern int preparse_packet;
extern int generate_assembler;
extern int generate_object;
extern int generate_stabs;
extern int optimize_code;
extern int parse_only;

/* exported debug flags */
extern int debug;
extern int dump_lex;
extern int dump_parser;
extern int dump_checker;
extern int dump_imcgen;

/* exported target configuration */
extern string basename;
extern string assembler_fname;
extern string object_fname;
extern string target_fname;
extern string target_cpu;
extern string target_os;

/* exported source configuration */
extern string_list packet_search_path;
extern string source_name;

/* exported link configuration */
extern string_list library_search_path;
extern string_list link_library_names;

/* exported actual source name and dir */
extern string act_source_dir;
extern string act_source_name;

/* exported routines */
void init_options ();
void set_actual_source (char *src_name);
void parse_command_line (int argc, char **argv);

#endif /* IncOptions */
