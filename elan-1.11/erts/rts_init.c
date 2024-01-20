/*
   File: rts_init.c
   Provides initialization and finalization

   CVS ID: "$Id: rts_init.c,v 1.4 2005/02/10 12:53:07 marcs Exp $"
*/

/* global includes */
#include <stdio.h>

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_term.h"
#include "rts_random.h"
#include "rts_system.h"
#include "rts_init.h"

void rts_init (int argc, char **argv)
	{ rts_init_gc ();
	  rts_init_termio ();
	  rts_init_random ();
	  rts_init_system (argc, argv);
	}; 

void rts_finish ()
	{ rts_finish_termio ();
	};
