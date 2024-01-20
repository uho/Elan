/*
   File: rts_term.h
   Provides terminal interfacing

   CVS ID: "$Id: rts_term.h,v 1.3 2004/12/18 13:24:51 marcs Exp $"
*/

#ifndef IncRtsTerm
#define IncRtsTerm

/* ELAN terminal ctl codes */
#define ELAN_CLEARSCREEN	 1
#define ELAN_CLEAREOL		 2
#define ELAN_BREAK		 3
#define ELAN_DELETE		 4
#define ELAN_RIGHT		 5
#define ELAN_LEFT		 6
#define ELAN_BELL		 7
#define ELAN_UP			 8
#define ELAN_DOWN		 9
#define ELAN_LINEFEED		10
#define ELAN_RUB		11
#define ELAN_FLUSH		12
#define ELAN_RETURN		13

/* initialization and finalization */
extern void rts_init_termio ();
extern void rts_finish_termio ();

/* ELAN externals */
/* INT PROC screen width */
extern int rts_screen_width ();

/* INT PROC screen length */
extern int rts_screen_length ();

/* PROC get cursor (INT VAR column, line) */
extern void rts_get_cursor (int *column, int *line);

/* PROC cursor (INT column, line) */
extern void rts_cursor (int column, int line);

/* PROC put (TEXT s) */
extern void rts_put_text (char *s);

#endif /* IncRtsTerm */
