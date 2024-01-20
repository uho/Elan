/*
   File: rts_random.h
   Provides simple random generator

   CVS ID: "$Id: rts_random.h,v 1.2 2004/12/18 13:24:51 marcs Exp $"
*/

#ifndef IncRtsMisc
#define IncRtsMisc

/* miscellaneous routines */
extern void rts_init_random ();
extern void rts_initialize_random (int a);
extern int rts_random (int a, int b);

#endif /* IncRtsMisc */
