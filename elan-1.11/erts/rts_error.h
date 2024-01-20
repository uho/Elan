/*
   File: rts_error.h
   Defines abnormal exits from running Elan programs

   CVS ID: "$Id: rts_error.h,v 1.2 2004/12/18 13:24:51 marcs Exp $"
*/
#ifndef RtsError
#define RtsError

extern int debug;
extern void rts_panic (char *format, ...);
extern void rts_error (char *format, ...);
extern void rts_warning (char *format, ...);
extern void rts_log (char *format, ...);

/* Special exits to call from generated code */
extern void rts_index_out_of_range ();
extern void rts_offset_from_0 ();

#endif /* RtsError */
