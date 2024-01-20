/*
   File: options.c
   Defines flags and parameters of the elan compiler
   It also specifies some compiler defaults

   CVS ID: "$Id: options.c,v 1.14 2011/08/13 15:58:54 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

/* Check for windows dependent stuff */
#ifndef WIN32
#include <unistd.h>
#include <sys/param.h>
#endif /* WIN32 */
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif /* MAXPATHLEN */

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>

/* local includes */
#include "elan_ast.h"
#include "options.h"

/* exported flags and pragmats */
int iris_compatibility;
int preparse_packet;
int generate_assembler;
int generate_object;
int generate_stabs;
int optimize_code;
int parse_only;

/* exported debug flags */
int debug;
int dump_lex;
int dump_parser;
int dump_checker;
int dump_imcgen;
static int show_version;

/* target configuration */
string basename;			/* basename from source file */
string assembler_fname;			/* implicit from opts */
string object_fname;			/* implicit from opts */
string target_fname;			/* -o target */
string target_cpu;			/* -b archname */
string target_os;			/* -os osname */

/* source configuration */
string_list packet_search_path;
string source_name;

/* exported link configuration */
string_list library_search_path;
string_list link_library_names;

/* actual source name and dir */
string act_source_dir;
string act_source_name;

/* set default target flags */
#ifndef TCPU
#define TCPU "i586"
#endif
#ifndef TOS
#define TOS "Linux"
#endif

/* set default location of standard prelude and library */
#ifndef PRELDIR
#define PRELDIR "/home/marcs/elan/etc"
#endif
#ifndef LIBDIR
#define LIBDIR "/home/marcs/elan/lib"
#endif

/*
   Read a column separated path from an environment variable
   and store it into the paths list
*/
static void read_paths_from_environment (char *env_var, string_list paths)
{ char *env_paths = getenv (env_var);
  char buf[MAXPATHLEN+1];
  char *ptr, *dptr;

  /* Environment var has no value, ignore */
  if (env_paths == NULL) return;
  ptr = env_paths;
  if (!(*ptr)) return;
  dptr = buf;
  
  /* As long as we have something to copy do so */
  while (1)
    if ((*ptr == ':') || !(*ptr))
      { *dptr = 0;
	if (strlen (buf))
	  { /* We have some path in the buffer */
#ifdef WIN32
	    struct _stat sbuf;
	    if ((_stat (buf, &sbuf) == 0) && (sbuf.st_mode & _S_IFDIR))
#else
	    struct stat sbuf;
	    if ((stat (buf, &sbuf) == 0) && S_ISDIR (sbuf.st_mode))
#endif
	      app_string_list (paths, new_string (buf));
	  };
	ptr++;
	if (!(*ptr)) return;
	dptr = buf;	
      }
    else *dptr++ = *ptr++;
}

void init_options ()
{ iris_compatibility = 1;	/* for the time being */
  preparse_packet = 0;
  generate_assembler = 0;
  generate_object = 0;
  generate_stabs = 0;
  optimize_code = 0;
  parse_only = 0;
  debug = 0;
  dump_lex = 0;
  dump_parser = 0;
  dump_checker = 0;
  dump_imcgen = 0;
  show_version = 0;
  full_verbose = 0;
  verbose = 0;
  assembler_fname = string_nil;
  object_fname = string_nil;
  target_fname = string_nil;
  target_cpu = new_string (TCPU);
  target_os = new_string (TOS);
  source_name = string_nil;

  /* Add path to prelude and cwd to search for packets */
  packet_search_path = new_string_list ();
  read_paths_from_environment ("ELAN_INCLUDE_PATH", packet_search_path);
  app_string_list (packet_search_path, new_string (PRELDIR));
  app_string_list (packet_search_path, new_string ("."));
  library_search_path = new_string_list ();
  read_paths_from_environment ("ELAN_LIBRARY_PATH", library_search_path);
  app_string_list (library_search_path, new_string (LIBDIR));

  /* Add elan runtime system and libmath as default libs to link against */
  link_library_names = new_string_list ();
};

static void print_usage ()
{ dcg_wlog ("usage: elancc [flags] filename [more_flags]");
  dcg_wlog ("where flags and more_flags may be any of the following:");
  dcg_wlog ("-h:  provide this help");
  dcg_wlog ("-V:  show version");
  dcg_wlog ("-d:  set debugging");
  dcg_wlog ("-v:  verbose");
  dcg_wlog ("-fv: full verbose");
  dcg_wlog ("-ic: allow iris constructs");
  dcg_wlog ("-pp: preparse packet");
  dcg_wlog ("-po: only do context-free analysis of main packet");
  dcg_wlog ("-dl: dump lexical analyzer output");
  dcg_wlog ("-dp: dump parser output");
  dcg_wlog ("-dc: dump checker output");
  dcg_wlog ("-di: dump intermediate code");
  dcg_wlog ("-g:  generate debugging information");
  dcg_wlog ("-O: optimize generated code");
  dcg_wlog ("-S: leave assembler code in place");
  dcg_wlog ("-c: only compile code");
  dcg_wlog ("-o oname: place output in file 'ofile'");
  dcg_wlog ("-b cpu: generate code for this target cpu");
  dcg_wlog ("-os os: generate code for this target os");
  dcg_wlog ("-Ipath: add path to packet search path");
  dcg_wlog ("-Lpath: add path to library search path");
  dcg_wlog ("-llib: link executable against this library");
  dcg_exit (4);
};

static void syntax_error (char *syn_error)
{ dcg_error (0, "error on command line: %s", syn_error);
  print_usage ();
};

static void scan_option (char *ptr, int *i, int argc, char **argv)
{ if (streq (ptr, "dl")) { dump_lex = 1; verbose = 1; }
  else if (streq (ptr, "dp")) { dump_parser = 1; verbose = 1; }
  else if (streq (ptr, "dc")) { dump_checker = 1; verbose = 1; }
  else if (streq (ptr, "di")) { dump_imcgen = 1; verbose = 1; }
  else if (streq (ptr, "d"))  debug = 1;
  else if (streq (ptr, "V"))  show_version = 1;
  else if (streq (ptr, "v"))  { show_version = 1; verbose = 1; }
  else if (streq (ptr, "fv")) { show_version = 1; verbose = 1; full_verbose = 1; }
  else if (streq (ptr, "ic")) iris_compatibility = 1;
  else if (streq (ptr, "pp")) preparse_packet = 1;
  else if (streq (ptr, "po")) parse_only = 1;
  else if (streq (ptr, "h"))  print_usage ();
  else if (streq (ptr, "g"))  generate_stabs = 1;
  else if (streq (ptr, "S"))  generate_assembler = 1;
  else if (streq (ptr, "c"))  generate_object = 1;
  else if (streq (ptr, "O"))  optimize_code = 1;
  else if (streq (ptr, "o"))
    { *i = *i + 1;
      if ((*i) < argc)
	{ detach_string (&target_fname); target_fname = new_string (argv[*i]); }
      else syntax_error ("missing output name");
    }
  else if (streq (ptr, "b"))
    { *i = *i + 1;
      if ((*i) < argc)
	{ detach_string (&target_cpu); target_cpu = new_string (argv[*i]); }
      else syntax_error ("missing target cpu");
    }
  else if (streq (ptr, "os"))
    { *i = *i + 1;
      if ((*i) < argc)
	{ detach_string (&target_os); target_os = new_string (argv[*i]); }
      else syntax_error ("missing target os");
    }
  else if (*ptr == 'I')
    { if (*(ptr + 1))
	ins_string_list (packet_search_path, 0, new_string (ptr + 1));
      else
	{ *i = *i + 1;
	  if ((*i) < argc)
	    ins_string_list (packet_search_path, 0, new_string (argv[*i]));
	  else syntax_error ("missing packet search path component");
	};
    }
  else if (*ptr == 'L')
    { if (*(ptr + 1))
	ins_string_list (library_search_path, 0, new_string (ptr + 1));
      else
	{ *i = *i + 1;
	  if ((*i) < argc)
	    ins_string_list (library_search_path, 0, new_string (argv[*i]));
	  else syntax_error ("missing library search path component");
	};
    }
  else if (*ptr == 'l')
    { if (*(ptr + 1))
	ins_string_list (link_library_names, 0, new_string (ptr + 1));
      else syntax_error ("missing link library name");
    }
  else syntax_error ("illegal option specified");
}

#ifndef VERSION
#define VERSION "1.0"
#endif
static void report_version ()
{ if (!show_version) return;
  dcg_wlog ("This is ELAN3, C Version %s, (C) M. Seutter", VERSION);
}

static void set_basename ()
{ char buf[MAXSTRLEN];
  char *sptr, *dptr;
  for (sptr = source_name, dptr = buf; (*sptr) && (*sptr != '.'); sptr++, dptr++)
    *dptr = *sptr;
  *dptr = '\0';
  basename = new_string (buf);
}

void parse_command_line (int argc, char **argv)
{ int ix;
  for (ix = 1; ix < argc; ix++)
    { char *arg = argv[ix];
      if (arg[0] == '-') scan_option (arg+1, &ix, argc, argv);
      else if (source_name == string_nil) source_name = new_string (arg);
      else syntax_error ("too many arguments");
    };
  report_version ();

  /* Exit if no source given */
  if (source_name == string_nil)
    { if (!show_version)
	dcg_wlog ("no source name given, bailing out...");
      dcg_exit (0);
    };

  /* Set the basename for later use */
  set_basename ();
}

void set_actual_source (char *src_name)
{ char buf[MAXPATHLEN + 1];
  if (getcwd (buf, MAXPATHLEN) == NULL)
    dcg_panic ("could not get current working directory");
  act_source_dir = new_string (buf);
  act_source_name = new_string (src_name);
}
