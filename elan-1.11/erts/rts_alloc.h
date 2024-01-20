/*
   File: rts_alloc.h
   Provides basic allocation routines and garbage collection

   CVS ID: "$Id: rts_alloc.h,v 1.2 2004/12/18 13:24:50 marcs Exp $"
*/

#ifndef IncRtsAlloc
#define IncRtsAlloc

extern char *rts_malloc (int size);
extern char *rts_attach (char **ptr);
extern void rts_detach (char **ptr);
extern char *rts_guard (char **ptr, int size);
extern char *rts_predetach (char **ptr);
extern void rts_init_gc ();

#endif /* IncRtsAlloc */
