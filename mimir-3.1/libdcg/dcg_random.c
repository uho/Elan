/*
   File: dcg_random.c
   Defines a random generator

   Copyright (C) 2008 Marc Seutter

   This library is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published
   by the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_random.c,v 1.9 2011/08/17 19:14:44 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

/* standard includes */
#include <stdio.h>
#include <math.h>
#include <sys/types.h>

/* include sys/time.h with or without time.h */
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

/* local includes */
#include "dcg.h"
#include "dcg_random.h"

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
}

static unsigned int new_random ()
{ unsigned int buf;
  buf = random_buffer [random_idx];
  random_idx++;
  if (random_idx == maxbuffer) refill_random_buffer ();
  return (buf);
}

#define time_nil (time_t *)(NULL)
#define tp 101
#define tq 137
static void init_random_buffer ()
{ int i;
  random_buffer [maxbuffer-p-1] = (unsigned int) time (time_nil);
  for (i = maxbuffer-p; i <= maxbuffer; i++)
    random_buffer [i] = tp * random_buffer[i-1] + tq;
  refill_random_buffer ();
  refill_random_buffer ();
}

/*
   provide support for arrays of random numbers
*/
static void fill_new_random_array (unsigned int *array, unsigned int nr)
{ unsigned int i = 0;
  while (nr >= maxbuffer - random_idx)
    { nr -= maxbuffer - random_idx;
      for (; random_idx < maxbuffer; random_idx++)
	array[i++] = random_buffer[random_idx];
      refill_random_buffer ();
    };

  for (; i < nr; i++)
    array[i] = random_buffer[random_idx++];
}
  
int dcg_dice (int sides)
{ return ((int) (new_random () % sides) + 1);
}

int dcg_random_in (int range)
{ return ((int) (new_random () % range));
}

#define NatNRand 5
int dcg_nat_gauss (int range)
{ int sum = 0;
  int i;
  for (i = 0; i < NatNRand; i++) sum += dcg_random_in (range);
  sum /= NatNRand;
  return (sum);
}

#define UmaxInt 4294967295.0
double dcg_my_random ()
{ return (((double) new_random ()) / UmaxInt);
}

#define NRand 4
static double gauss_fac;
static double gauss_add;
double dcg_my_gauss ()
{ double sum = 0.0;
  int i;
  unsigned int array[NRand];
  fill_new_random_array (array, NRand);
  for (i=0; i < NRand; i++) sum += (double) array[i];
  return (gauss_fac * sum - gauss_add);
}

void dcg_init_random ()
{ gauss_add = sqrt (3.0 * NRand);
  gauss_fac = 2.0 * gauss_add / (NRand * UmaxInt);
  init_random_buffer ();
}

