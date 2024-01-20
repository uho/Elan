/*
   File: elan_threads.h
   Provides basic thread working

   CVS ID: "$Id: elan_threads.h,v 1.2 2006/02/04 15:23:51 marcs Exp $"
*/
#ifndef IncElanThreads
#define IncElanThreads

#include <pthread.h>

/* Exported routines */
int eth_equal_pid_pid (pthread_t pid1, pthread_t pid2);
int eth_int_arg (void *arg);
void *eth_thread_arg_int (int i);
pthread_t eth_create_thread_proc (void *(*code)());
pthread_t eth_create_thread_proc_arg (void *(*code)(), void *arg);
void eth_finish_thread ();
 
#endif /* IncElanThreads */
