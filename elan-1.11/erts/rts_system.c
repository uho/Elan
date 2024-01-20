/*
   File: rts_system.c
   Provides initialization and finalization

   CVS ID: "$Id: rts_system.c,v 1.9 2011/08/13 15:59:21 marcs Exp $"
*/

/* include <config.h> if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* global includes */
#include <stdio.h>
#include <string.h>

/* system includes */
#include <sys/types.h>
#include <sys/time.h>
#ifdef HAVE_SYS_RESOURCE_H
#include <sys/resource.h>
#endif /* HAVE_SYS_RESOURCE_H */
#ifdef HAVE_SYS_WAIT_H
#include <sys/wait.h>
#endif /* HAVE_SYS_WAIT_H */
#include <stdlib.h>
#include <unistd.h>
#ifdef WIN32
#include <windows.h>
#endif /* WIN32 */

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_term.h"
#include "rts_system.h"

/* create local copies of argument list */
static int my_argc;
static char **my_argv;
#define program_name my_argv[0]

/*
   INT PROC exectime
   Returns execution time in seconds
*/
int rts_exectime (void)
#ifdef WIN32
	{ FILETIME kernel, user, dump1, dump2;
	  ULONGLONG k_uli, u_uli, result;
	  HANDLE me = OpenProcess (PROCESS_QUERY_INFORMATION, FALSE, GetCurrentProcessId ());
	  GetProcessTimes (me, &dump1, &dump2, &kernel, &user); 
	  k_uli = (((ULONGLONG) kernel.dwHighDateTime) << 32) | 
		   ((ULONGLONG) kernel.dwLowDateTime);
	  u_uli = (((ULONGLONG) user.dwHighDateTime) << 32) | 
		   ((ULONGLONG) user.dwLowDateTime);
	  result = (k_uli / 10000000) + (u_uli / 10000000);
	  return ((int) result);
#else
	{ struct rusage ru;
	  int carry;
	  if (getrusage (RUSAGE_SELF, &ru))
	     rts_panic ("Could not get rusage");
	  carry = (ru.ru_utime.tv_usec + ru.ru_stime.tv_usec > 500000)?1:0;
	  return (ru.ru_utime.tv_sec + ru.ru_stime.tv_sec + carry);
#endif /* WIN32 */
	};

/*
   INT PROC system (TEXT cmd)
   Note that we have to implement system with fork and exec ourselves
   since the libc system () call can not be interrupted
*/
/* declare environ */
extern char **environ;

int rts_system (char *cmd)
#ifdef WIN32
	{ STARTUPINFO si;
	  PROCESS_INFORMATION pi;
#else
	{ int pid, status;
#endif /* WIN32 */ 
	  if (cmd == NULL)
	     rts_error ("INT PROC system (TEXT cmd) called with uninitialized argument");
	  
#ifdef WIN32
	  memset ((void *) &si, 0, sizeof (si));
	  si.cb = sizeof (si);
	  memset ((void *) &pi, 0, sizeof (pi));
	  if (!CreateProcess (NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
	     rts_panic ("could not create process");
	  else
	     { /* Parent code */
	       DWORD result = WaitForSingleObject (pi.hProcess, INFINITE);
	       CloseHandle (pi.hProcess);
	       CloseHandle (pi.hThread);
	       if (result == WAIT_OBJECT_0) return (1);
	     }
	  return (0);
#else
	  pid = fork ();
	  if (pid < 0)
	     rts_panic ("could not fork");
	  else if (pid == 0)
	     { /* Child code */
	       char *argv[4];
	       argv[0] = "sh";
	       argv[1] = "-c";
	       argv[2] = cmd;		/* Note no attach! */
	       argv[3] = NULL;
	       execve ("/bin/sh", argv, environ);
	       exit (127);		/* We should not be here */
	     }
	  else
	     { /* Parent code */
	       waitpid (pid, &status, 0);
	       if (WIFEXITED (status)) return (1);
	     };
#endif /* WIN32 */
	  return (0);
	};

/* PROC sleep (DOUBLE s) */
void rts_rsleep (double s)
	{ int usec = (int) (s * 1000000.0 + 1.5);
#ifdef WIN32
	  Sleep (usec);
#else
	  usleep (usec);
#endif /* WIN32 */
	};

/* PROC sleep (INT n) */
void rts_sleep (int n)
#ifdef WIN32
	{ Sleep (n * 1000);
#else
        { sleep (n);
#endif /* WIN32 */
        };

/* PROC stop */
void rts_stop ()
	{ rts_log ("\nStopping program %s", my_argv[0]);
	  rts_finish_termio ();
	  exit (0);
	};

/* PROC error stop */
void rts_error_stop ()
	{ rts_error ("Aborting program %s", my_argv[0]);
	};

/* INT PROC argument count */
int rts_argument_count ()
	{ return (my_argc);
	};

char *rts_argument (int nr)
	{ if ((nr < 0) || (nr >= my_argc))
	     rts_error ("TEXT PROC argument (INT nr) called with arg %d", nr);
	  return (rts_attach (&my_argv[nr]));
	};

void rts_init_system (int argc, char **argv)
	{ int ix;
	  my_argc = argc;
	  my_argv = (char **) rts_malloc ((argc + 1) * sizeof (char *));
	  for (ix = 0; ix < argc; ix++)
	     { int len = strlen (argv[ix]);
	       my_argv[ix] = rts_malloc (len + 1);
	       strcpy (my_argv[ix], argv[ix]);
	     };
	};
