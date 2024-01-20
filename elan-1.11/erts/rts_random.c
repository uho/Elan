/*
   File: rts_random.c
   Provides simple random generator

   CVS ID: "$Id: rts_random.c,v 1.5 2005/04/06 18:55:27 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* system includes */
#include <stdio.h>
#include <stdlib.h>

/* include sys/time.h with or without time.h */
#include <sys/types.h>
#if TIME_WITH_SYS_TIME
#   include <sys/time.h>
#   include <time.h>
#else
#   if HAVE_SYS_TIME_H
#      include <sys/time.h>
#   else
#      include <time.h>
#   endif
#endif

/* Check if configured to use threads */
#ifdef USE_THREADS
#include <pthread.h>
#endif

/* local includes */
#include "rts_error.h"
#include "rts_random.h"

/* When configured to use threads, reserve a lock */
#ifdef USE_THREADS
static pthread_mutex_t random_mutex;
#endif

/*
   The actual random generators
*/
#define maxbuffer 1024
#define p 98
#define q 27

static unsigned int random_buffer [maxbuffer];
static unsigned int random_idx;

/*
   fill random buffer
*/
static void refill_random_buffer ()
	{ int i;
	  for (i=0; i<=q; i++) random_buffer [i] =
	     random_buffer [maxbuffer-q+i] ^ random_buffer [maxbuffer-p+i];
	  for (i=q+1;i<=p; i++) random_buffer [i] =
	     random_buffer [i-q] ^ random_buffer [maxbuffer-p+i];
	  for (i=p+1;i<maxbuffer; i++) random_buffer [i] =
	     random_buffer [i-q] ^ random_buffer [i-p];
	  random_idx = 0;
	};

static unsigned int new_random ()
	{ unsigned int buf;
	  buf = random_buffer [random_idx];
	  random_idx++;
	  if (random_idx == maxbuffer) refill_random_buffer ();
	  return (buf);
	};

#define time_nil (time_t *)(NULL)
#define tp 101
#define tq 137
static void init_random_buffer (unsigned int a)
	{ int i;
	  random_buffer [maxbuffer-p-1] = a;
	  for (i = maxbuffer-p; i <= maxbuffer; i++)
	     random_buffer [i] = tp * random_buffer[i-1] + tq;
	  refill_random_buffer ();
	  refill_random_buffer ();
	};

static int random_in (int sides)
	{ return ((int) (new_random () % sides));
	};

/* INT PROC random (INT CONST a,b) */
int rts_random (int a, int b)
	{ int offset;
	  if (a > b) return (rts_random (b, a));
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&random_mutex))
	     rts_panic ("Could not lock mutex in random generator: random");
#endif
	  offset = random_in (b - a + 1);
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&random_mutex))
	     rts_panic ("Could not unlock mutex in random generator: random");
#endif
	  return (a + offset);
	};

/* PROC initialize random (INT CONST a) */
void rts_initialize_random (int a)
	{
#ifdef USE_THREADS
	  if (pthread_mutex_lock (&random_mutex))
	     rts_panic ("Could not lock mutex in random generator: initialize random");
#endif
	  init_random_buffer ((unsigned int) a);
#ifdef USE_THREADS
	  if (pthread_mutex_unlock (&random_mutex))
	     rts_panic ("Could not unlock mutex in random generator: initialize random");
#endif
	};

/* Initialization call */
void rts_init_random ()
	{ time_t curr_time = time (NULL);
	  init_random_buffer ((unsigned int) curr_time);

	  /* If configured to use threads, init mutex */
#ifdef USE_THREADS
	  if (pthread_mutex_init (&random_mutex, NULL))
	     rts_panic ("Could not initialize mutex in random generator");
#endif
	};

