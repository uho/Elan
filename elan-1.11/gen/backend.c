/*
   File: backend.c
   Finishes the compilation by assembling and linking the code

   CVS ID: "$Id: backend.c,v 1.8 2011/10/14 11:38:31 marcs Exp $"
*/

/* include <config.h> if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>

/* libdcg includes */
#include <dcg.h>
#include <dcg_error.h>
#include <dcg_string.h>
#include <dcg_procutil.h>

/* local includes */
#include "options.h"
#include "backend.h"

/* TMPDIR is the usual environment variable to consult */ 
#ifndef TMPDIR
#define TMPDIR "TMPDIR"
#endif

/* Define the usual default temporary directory */
#ifndef DEFAULT_TMP_DIR
#define DEFAULT_TMP_DIR "/tmp"
#endif

static void do_assemble ()
{ char buf[MAXPATHLEN + 1];
  char *ass_argv[12];
  int status;
  int ix = 0;

  /* Define the name of the object file */
  if (!generate_object)
     { /* We do not want to leave the object code in place */
       string temp_dir = getenv (TMPDIR);
       if (temp_dir == NULL) temp_dir = DEFAULT_TMP_DIR;
       sprintf (buf, "%s/elan_%d.o", temp_dir, (int) getpid ());
     }
  else if (target_fname != NULL)	/* -o name given */
     strcpy (buf, target_fname);
  else sprintf (buf, "%s.o", basename);
  object_fname = new_string (buf);

  /* Fill in the argument vector to assemble */
  dcg_warning (0, "   assembling machine code...");
  ass_argv[ix++] = "gcc";
  ass_argv[ix++] = "-c";
  ass_argv[ix++] = assembler_fname;
  ass_argv[ix++] = "-o";
  ass_argv[ix++] = object_fname;
  ass_argv[ix] = NULL;
  status = dcg_spawn_and_wait (ass_argv);

  /* Delete the generated assembler file */
  if (unlink (assembler_fname))
     dcg_panic ("could not delete '%s'", assembler_fname);
  if (status) dcg_exit (4);
};

static void do_link ()
{ char buf[MAXPATHLEN + 1];
  char **ld_argv;
  char *exe_name;
  int nr_args = 3 * library_search_path -> size + link_library_names -> size + 12;
  int status;
  int ix = 0;
  int iy;
  ld_argv = (char **) dcg_calloc (nr_args, sizeof (char *));

  /* Define the name of the executable */
  if (target_fname != NULL)		/* -o name given */
     exe_name = target_fname;
  else exe_name = basename;

  /* Fill in the argument vector to link */
  dcg_warning (0, "   linking object code...");
  ld_argv[ix++] = "gcc";
  ld_argv[ix++] = "-o";
  ld_argv[ix++] = exe_name;
  ld_argv[ix++] = object_fname;
  for (iy = 0; iy < library_search_path -> size; iy++)
     { sprintf (buf, "-L%s", library_search_path -> array[iy]);
       ld_argv[ix++] = new_string (buf);
       ld_argv[ix++] = "-Wl,-rpath";
       sprintf (buf, "-Wl,%s", library_search_path -> array[iy]);
       ld_argv[ix++] = new_string (buf);
     };
  for (iy = 0; iy < link_library_names -> size; iy++)
     { sprintf (buf, "-l%s", link_library_names -> array[iy]);
       ld_argv[ix++] = new_string (buf);
     };
  ld_argv[ix++] = "-lerts";
#ifdef CURSES_LIB 
  ld_argv[ix++] = CURSES_LIB;
#endif
#ifdef THREAD_LIB
  ld_argv[ix++] = THREAD_LIB;
#endif
  ld_argv[ix++] = "-lm";
  ld_argv[ix] = NULL;
  status = dcg_spawn_and_wait (ld_argv);

  /* Delete the generated object file */
  if (unlink (object_fname))
     dcg_panic ("could not delete '%s'", object_fname);
  if (status) dcg_exit (4);
};

void assemble_and_link ()
{ /* If we only have to generate assembler, we are done */
  if (generate_assembler) return;
  do_assemble ();
  if (generate_object) return;
  do_link ();
};
