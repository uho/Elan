/*
   File: rts_reals.h
   Provides real engine

   CVS ID: "$Id: rts_reals.h,v 1.2 2004/12/18 13:24:51 marcs Exp $"
*/

#ifndef IncRtsReals
#define IncRtsReals

typedef double real;
extern real rts_maxreal ();
extern real rts_smallreal ();
extern real rts_real_pow_int (real a, int b);
extern int rts_trunc_real_to_int (real a);
extern int rts_round_real_to_int (real a);
extern char *rts_real_to_text (double r);
extern real rts_text_to_real (char *text, int *ok);

#endif /* IncRtsReals */

