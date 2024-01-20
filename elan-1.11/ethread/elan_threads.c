/*
   File: elan_threads.c
   Defines a thread library for elan

   CVS ID: "$Id: elan_threads.c,v 1.2 2006/02/04 15:23:51 marcs Exp $"
*/

/* global includes */
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/* pthread stuff includes */
#include <errno.h>
#include <pthread.h>

/* liberts includes */
#include <rts_error.h>
#include <rts_alloc.h>
#include <rts_texts.h>

/* local includes */
#include <elan_threads.h>

/* BOOL OP = (PID pid1, pid2) */
int eth_equal_pid_pid (pthread_t pid1, pthread_t pid2)
	{ return ((pthread_equal (pid1, pid2))?1:0);
	};

/* INT PROC int (ARG arg) */
int eth_int_arg (void *arg)
	{ return ((int)(long) arg);
	};

/* ARG PROC thread arg (INT i) */
void *eth_thread_arg_int (int i)
	{ return ((void *) (long) i);
	};

/* PID PROC create thread (PROC code, ARG arg) */
pthread_t eth_create_thread_proc_arg (void *(*code)(), void *arg)
	{ pthread_t pid;
	  int status = pthread_create (&pid, NULL, code, arg);
	  if (status)
	     rts_error ("Could not create thread, reason: %s", strerror (errno));
	  return (pid);
	};

/* PID PROC create thread (PROC code) */
pthread_t eth_create_thread_proc (void *(*code)())
	{ pthread_t pid;
	  int status = pthread_create (&pid, NULL, code, NULL);
	  if (status)
	     rts_error ("Could not create thread, reason: %s", strerror (errno));
	  return (pid);
	};

/* PROC finish thread */
void finish_thread ()
	{ pthread_exit (NULL);
	};
