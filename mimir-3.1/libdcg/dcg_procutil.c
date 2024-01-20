/*
   File: dcg_procutil.c
   Defines routines to spawn subprocesses. For the moment we only
   need code that spawns a subprocess and then waits for completion.
   As soon as we need asynchronous process communication we will add it.

   Copyright 2011 M. Seutter

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

   CVS ID: "$Id: dcg_procutil.c,v 1.3 2012/01/20 08:41:41 marcs Exp $"
*/

/* include <config.h> if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* global includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#endif

#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif /* HAVE_SYS_WAIT_H */

#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

/* local includes */
#include "dcg.h"
#include "dcg_error.h"
#include "dcg_string.h"


#ifdef WIN32
static char* flatten (char *argv [])
{ char **ptr, *result;
  int length = 0;
  for (ptr = argv; (*ptr) != NULL; ptr++)
    { if (ptr == argv)
        result = (char *) dcg_malloc (length + (strlen (*ptr) + 2) * sizeof (char));
      else dcg_realloc ((void **) &result, length + (strlen (*ptr) + 2) * sizeof (char));
      strcpy (result + length, *ptr);
      length += (strlen (*ptr) + 1);
      result [length - 1] = ' ';
      result [length] = '\0';
    };
  dcg_realloc ((void **) &result, length * sizeof (char));
  result [length - 1] = '\0';
  return (result);
}
#endif /* WIN32 */

int dcg_spawn_and_wait (char *argv[])
#ifdef WIN32
{ STARTUPINFO si;
  PROCESS_INFORMATION pi;
  char *cmdline;
#else
{ pid_t pid;
  int status;
#endif /* WIN32 */

  /* when full verbose, dump command to spawn */
  if (full_verbose)
    { char **ptr;
      dcg_eprint ("     ");
      for (ptr = argv; (*ptr) != NULL; ptr++)
	{ dcg_eprint (" ");
	  dcg_eprint (*ptr);
	};
      dcg_wlog ("");
    };

#ifdef WIN32
  memset ((void *) &si, 0, sizeof (si));
  si.cb = sizeof (si);
  memset ((void *) &pi, 0, sizeof (pi));
  cmdline = flatten (argv); 
  if (!CreateProcess (NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    dcg_panic ("could not create process: error %d", GetLastError ());
  else
    { /* Parent code */
      DWORD result = WaitForSingleObject (pi.hProcess, INFINITE);
      CloseHandle (pi.hProcess);
      CloseHandle (pi.hThread);
      detach_string (&cmdline);
      if (result == WAIT_OBJECT_0) return (0);
      return (1);
    }
#else
  /* Fork off process */
  pid = fork ();
  if (pid < 0)
    { /* could not fork, bad if so */
      dcg_panic ("could not fork: %s", strerror (errno));
    }
  else if (pid == 0)
    { /* child code */
      execvp (argv[0], argv);

      /* if we reach this point, we could not exec */
      dcg_panic ("could not exec %s: %s", argv[0], strerror (errno));
    };

  /* parent code: wait for child to finish */
  waitpid (pid, &status, 0);
  return (status);
#endif /* WIN32 */
}
