/*
   File: options.c
   Defines flags and parameters of the datastructure generator

   Copyright (C) 2008 Marc Seutter

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: options.c,v 1.10 2011/09/04 11:18:58 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Conditional inclusion to define MAXPATHLEN */
#ifndef WIN32
#include <sys/param.h>
#endif
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>
#include <dcg_version.h>

/* local includes */
#include "dcg_code.h"
#include "options.h"

/* Define INCPATH and LIBPATH if configure did not do this */
#ifndef INCPATH
#define INCPATH "/home/marcs/include"
#endif
#ifndef LIBPATH
#define LIBPATH "/home/marcs/lib"
#endif

/* Exported flags */
int coding_flag;
int saving_flag;
int loading_flag;
int xformer_flag;
int code_transput;
int code_rdup;
int code_extra_tags;
int code_pretty_print;

/* Exported parameters and search paths */
string_list ex_names;
string_list include_path;
string xformer_ptype;
string xformer_pname;
string basic_types;

/* Local flags */
static int report_version;

/*
   Initialize the options and default search paths
*/
void init_options ()
{ /* Flags */
  full_verbose = 0;
  verbose = 0;
  debug = 0;
  coding_flag = 1;
  saving_flag = 0;
  loading_flag = 0;
  xformer_flag = 0;
  report_version = 0;
  code_transput = 1;
  code_rdup = 1;
  code_pretty_print = 1;
  code_extra_tags = 0;

  /* Default search paths */
  ex_names = new_string_list ();
  include_path = new_string_list ();
  app_string_list (include_path, new_string (INCPATH));
  app_string_list (include_path, new_string ("."));
  xformer_ptype = NULL;
  xformer_pname = new_string ("xformp");
  basic_types = new_string ("dcg");
}

void try_report_version ()
{ if (verbose || report_version)
    dcg_wlog ("This is DCG, C version %s, (C) M.Seutter", DCG_VERSION);
}

/*
   Comment the usage
*/
static void usage ()
{ dcg_wlog ("Usage: dcg [option...] file[.dcg]");
  dcg_wlog ("where option may be any of the following:");
  dcg_wlog ("   -n: do not code");
  dcg_wlog ("   -v: verbose execution");
  dcg_wlog ("   -s: save internal datastructures");
  dcg_wlog ("   -l: load internal datastructures");
  dcg_wlog ("   -x: generate transformer");
  dcg_wlog ("   -V: give version of dcg");
  dcg_wlog ("   -xt: generate an extra file coding enums and tags only");
  dcg_wlog ("   -nxp: do not code transput routines");
  dcg_wlog ("   -npp: do not code pretty print routines");
  dcg_wlog ("   -nrd: do not code rdup routines");
  dcg_wlog ("   -e name: exclude routine (transformer only)");
  dcg_wlog ("   -I path: add path to include search paths");
  dcg_wlog ("   -xp string: specify transformer parameter type");
  dcg_wlog ("   -xn string: specify transformer routine name");
  dcg_wlog ("   -fv: full verbose execution");
  dcg_wlog ("   -bt name: use name to locate basic system types");
  dcg_wlog ("   -il: output location of mimirs include files");
  dcg_wlog ("   -ll: output location of mimirs library files");
  dcg_wlog ("   -h: show this help");
#ifdef DEBUG
  dcg_wlog ("   -d: turn on debugging (when configured)");
#endif
  dcg_exit (0);
}

static void syntax_error (char *msg)
{ dcg_error (0, "Syntax error: %s", msg);
  usage ();
}

static void skip_args (int nr, int pos, int *argc, char **argv)
{ int i;
  for (i=pos; i < *argc - nr; i++)
     argv[i] = argv[i+nr];
  *argc -= nr;
}

void scan_options (int *argc, char **argv)
{ int i = 0;
  skip_args (1, i, argc, argv);		/* skip command_name */
  while (i < *argc)
     { char *arg = argv[i];
       if (streq (arg, "-n"))
	  { coding_flag = 0;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-v"))
	  { verbose = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-s"))
	  { saving_flag = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-l"))
	  { loading_flag = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-x"))
	  { xformer_flag = 1;
	    coding_flag = 0;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-xt"))
          { code_extra_tags = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-nxp"))
	  { code_transput = 0;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-npp"))
	  { code_pretty_print = 0;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-nrd"))
	  { code_rdup = 0;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-e"))
	  { skip_args (1, i, argc, argv);
	    if (i < *argc)
	       { app_string_list (ex_names, new_string (argv[i]));
		 skip_args (1, i, argc, argv);
	       }
	    else syntax_error ("missing routine name");
	  }
       else if (streq (arg, "-xp"))
	  { skip_args (1, i, argc, argv);
	    if (i < *argc)
	       { xformer_ptype = new_string (argv[i]);
		 skip_args (1, i, argc, argv);
	       }
	    else syntax_error ("missing transformer parameter type");
	  }
       else if (streq (arg, "-xn"))
	  { skip_args (1, i, argc, argv);
	    if (i < *argc)
	       { xformer_pname = new_string (argv[i]);
		 skip_args (1, i, argc, argv);
	       }
	    else
	       syntax_error ("missing transformer routine name prefix");
          }
       else if (streq (arg, "-bt"))
	  { skip_args (1, i, argc, argv);
	    if (i < *argc)
	       { basic_types = new_string (argv[i]);
		 skip_args (1, i, argc, argv);
	       }
	    else syntax_error ("missing basic types name");
	  }
       else if ((arg[0] == '-') && (arg[1] == 'I'))
	  { if (arg[2]) ins_string_list (include_path, 0, new_string (arg + 2));
	    else 
	       { skip_args (1, i, argc, argv);
	         if (i < *argc)
	            ins_string_list (include_path, 0, new_string (argv[i]));
	         else syntax_error ("missing include path");
	       };
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-V"))
	  { report_version = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-d"))
	  { debug = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-fv"))
	  { full_verbose = 1;
	    verbose = 1;
	    skip_args (1, i, argc, argv);
	  }
       else if (streq (arg, "-il"))
	  { fprintf (stdout, "%s\n", INCPATH);
	    exit (0);
	  }
       else if (streq (arg, "-ll"))
	  { fprintf (stdout, "%s\n", LIBPATH);
	    exit (0);
	  }
       else if (streq (arg, "-h")) usage ();
       else if (arg[0] == '-') syntax_error ("unknown option");
       else i++;
     }
}

void scan_basename (int argc, char **argv, char *basename)
{ int len;
  if (argc == 0)
    { if (report_version) try_report_version ();
      else syntax_error ("missing argument");
      exit (0);
    }
  else if (argc > 2) syntax_error ("too many arguments");
  len = strlen (argv[0]);
  if (len > MAXPATHLEN-4) syntax_error ("too long argument");
  strcpy (basename, argv[0]);
  if (len < 3) return;
  if (strcmp (basename + len - 4, ".dcg") == 0)
     basename [len - 4] = '\0';
}
