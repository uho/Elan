/*
   File: rts_system.h
   Provides interface to OS
   Rather Unix tailored

   CVS ID: "$Id: rts_system.h,v 1.3 2011/08/13 15:59:21 marcs Exp $"
*/

#ifndef IncRtsSystem
#define IncRtsSystem

/* miscellaneous routines */
extern int rts_exectime (void);
extern int rts_system (char *cmd);
extern void rts_rsleep (double s);
extern void rts_sleep (int n);
extern void rts_stop ();
extern void rts_error_stop ();
extern int rts_argument_count ();
extern char *rts_argument (int i);

/* initialize the interface */
extern void rts_init_system (int argc, char **argv);

#endif /* IncRtsSystem */

