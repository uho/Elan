/*
   File: rts_term.c
   Provides terminal interfacing

   CVS ID: "$Id: rts_term.c,v 1.14 2005/02/23 14:55:39 marcs Exp $"
*/

/* include <config.h> if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* global includes */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

/* system includes */
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>

/* Check for sys/ioctl.h and termios.h */
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif /* HAVE_SYS_IOCTL_H */
#ifdef HAVE_TERMIOS_H
#include <termios.h>
#endif /* HAVE_TERMIOS_H */

/* Check for curses stuff */
#ifdef HAVE_CURSES_H
#include <curses.h>
#endif /* HAVE_CURSES_H */
#ifdef HAVE_NCURSES_H
#include <ncurses.h>
#endif /* HAVE_NCURSES_H */

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_texts.h"
#include "rts_ints.h"
#include "rts_term.h"

/* saved terminal status */
#ifdef HAVE_TERMIOS_H
static struct termios saved_attrs;
#endif /* HAVE_TERMIOS_H */
static int real_tty_in;
static int real_tty_out;
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
static WINDOW *elanwin;
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */

/* current position and tty size */
static int cur_line;
static int cur_column;
static int nr_lines;
static int nr_columns;

/* access routines for the above variables */
/* INT PROC screen width */
int rts_screen_width ()
	{ return (nr_columns);
	};

/* INT PROC screen length */
int rts_screen_length ()
	{ return (nr_lines);
	};

/* PROC get cursor (INT VAR column, line) */
void rts_get_cursor (int *column, int *line)
	{ *column = cur_column + 1;
	  *line = cur_line + 1;
	};

/* PROC cursor (INT column, line) */
void rts_cursor (int column, int line)
	{ cur_column = column - 1;
	  cur_line = line - 1;
	  if ((cur_column < 0) || (cur_column >= nr_columns) ||
	      (cur_line < 0) || (cur_line >= nr_lines))
	     rts_error ("PROC cursor (INT, INT) called with inappropriate coordinates (%d,%d)",
			column, line);
	  if (real_tty_out)
#if defined (HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wmove (elanwin, cur_line, cur_column);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\e[%d;%dH", line, column);
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  fflush (stdout);
	};

/* simple character output */
static void clear_screen ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wclear (elanwin);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\e[H\e[2J");
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  cur_line = 0;
	  cur_column = 0;
	  fflush (stdout);
	};

static void clear_to_end_of_line ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wclrtoeol (elanwin);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\e[K");
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  fflush (stdout);
	};

static void carriage_return ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wmove (elanwin, cur_line, 0);
	       wrefresh (elanwin);
	     }
#else
	     fputc ('\r', stdout);
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  /* MS: not orig else fputc ('\r', stdout); */
	  cur_column = 0;
	  fflush (stdout);
	};

static void cursor_up ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wmove (elanwin, cur_line ? cur_line - 1 : 0, cur_column);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\e[A");
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  if (cur_line) cur_line--;
	  fflush (stdout);
	};

static void cursor_down ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wmove (elanwin, cur_line < nr_lines ? cur_line + 1 : nr_lines,
	         cur_column);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\n");
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  if (cur_line < nr_lines) cur_line++;
	  fflush (stdout);
	};

static void crlf ()
	{ carriage_return ();
	  if (real_tty_out) cursor_down ();
	  else fputc ('\n', stdout);
	};

static void cursor_right ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wmove (elanwin, cur_line, cur_column < nr_columns ?
	         cur_column + 1 : nr_columns);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\e[C");
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  cur_column++;
	  if (cur_column == nr_columns)
	     crlf ();
	  fflush (stdout);
	};

static void cursor_left ()
	{ if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { wmove (elanwin, cur_line, cur_column ? cur_column - 1 : 0);
	       wrefresh (elanwin);
	     }
#else
	     fprintf (stdout, "\e[D");
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  if (cur_column) cur_column--;
	  fflush (stdout);
	};

static void bell ()
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	{ if (real_tty_out) beep ();
	  else fputc ('\007', stdout);
#else 
	{ fputc ('\007', stdout);
	  fflush (stdout);
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	};

static void do_printable (int ch)
	{ if (isprint (ch))
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { if (real_tty_out) wprintw (elanwin, "%c", ch);
	       else fputc (ch, stdout);
#elif defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
	     { fputc (ch, stdout);
#else
	     { fputc (ch, stdout);
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	       cur_column++;
	       if (cur_column == nr_columns)
		  crlf ();
	     };
	};

/*
   The character dispatcher.
   Note that not every ELAN control code gets printed
*/
static void rts_putchar (int ch)
	{ switch (ch)
	     { case ELAN_CLEARSCREEN:	clear_screen (); break;
	       case ELAN_CLEAREOL:	clear_to_end_of_line (); break;
	       case ELAN_RIGHT:		cursor_right (); break;
	       case ELAN_LEFT:		cursor_left (); break;
	       case ELAN_BELL:		bell (); break;
	       case ELAN_UP:		cursor_up (); break;
	       case ELAN_DOWN:		cursor_down (); break;
	       case ELAN_LINEFEED:	crlf (); break;
	       case ELAN_FLUSH:		fflush (stdout); break;
	       case ELAN_RETURN:	carriage_return (); break;
	       default: do_printable (ch);
	     };
	};

/* PROC put (TEXT s) */
void rts_put_text (char *s)
	{ char *ptr;
	  if (s == NULL)
	     rts_error ("PROC put (TEXT) called with uninitialized text");
	  for (ptr = s; *ptr; ptr++)
	     rts_putchar ((int) (*ptr));
	};

/*
   Read a single character from stdin
*/
#if defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
static int has_input_within_time (int ms)
	{ struct timeval timeout;
	  fd_set read_fds;
	  int stat;

	  /* initialize timeout and fd_set */
	  timeout.tv_sec = 0;
	  timeout.tv_usec = ms * 1000;
	  FD_ZERO (&read_fds);
	  FD_SET (0, &read_fds);

	  /* do select */
	  stat = select (1, &read_fds, NULL, NULL, &timeout);
	  return (stat > 0);
	};
#endif /* HAVE_TERMIOS_H && HAVE_SYS_IOCTL_H */

static char read_single_char ()
	{ char ch;
/* Deze test is te rigide */
#if defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
	  int stat;
	  stat = read (0, &ch, 1);
	  if (stat <= 0)
	     rts_error ("End of file or IO error detected on standard input");
#elif defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	  ch = wgetch (elanwin);
#else
	     #error "GLORP"
#endif /* HAVE_TERMIOS_H && HAVE_SYS_IOCTL_H */
	  return (ch);
	};

static char interpret_char (char ch)
	{ /* if not reading from a terminal, change \n into RETURN */
	  if (!real_tty_in)
	     { if (ch == '\n') return (ELAN_RETURN);
	       return (ch);
	     };

	  /* check for special codes */
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	  // fprintf (stderr, "Interpreting: %c (= %d)\n", ch, ch);
	  // fflush (stderr);
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  switch (ch)
	     { case '\033':
		  { /* Escape, check for cursor movement */
		    /* Check for more input; 5 ms should be enough */
#if defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
		    if (!has_input_within_time (5)) return (ch);
		    ch = read_single_char ();
#endif /* HAVE_TERMIOS_H && HAVE_SYS_IOCTL_H */
		    if (ch != '[') return (ELAN_BELL);
		    switch (read_single_char ())
		       { case 'A': return (ELAN_UP);
			 case 'B': return (ELAN_DOWN);
			 case 'C': return (ELAN_RIGHT);
			 case 'D': return (ELAN_LEFT);
			 default: return (ELAN_BELL);
		       };
	          };
	       case '\010': return (ELAN_DELETE);	/* ctrl H */
	       case '\177': return (ELAN_RUB);		/* delete */
	       default: return (ch);
	     };	
	};

/* TEXT PROC inchar */
char *rts_inchar ()
	{ char *result = rts_malloc (2);
	  char inch;
	  fflush (stdout);
	  inch = read_single_char ();
	  result[0] = interpret_char (inch);
	  result[1] = '\0';
	  return (result);
	};

/* TEXT PROC incharety */
char *rts_incharety ()
	{ char *result;
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	  int ch;
	  nodelay (elanwin, TRUE);
	  ch = wgetch (elanwin);
	  nodelay (elanwin, FALSE);
	  if (ch == ERR)
	     { result = rts_malloc (1);
	       result[0] = '\0';
	     }
	  else
	     { result = rts_malloc (2);
	       result[0] = ch;
	       result[1] = '\0';
	     }
#elif defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
	  if (has_input_within_time (0))
	     return (rts_inchar ());
	  result = rts_malloc (1);
	  result[0] = '\0';
#else
#error "GLORP"
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  return (result);
	};
 
/*
   Initialize the terminal input
   Note that rts_init_termio should only be called once
   and that stdin, stdout and stderr are open as usual
*/
void rts_init_termio ()
	{ /* flush remaining output before switching */
	  fflush (stdout);

	  /* check if stdin is connected to some terminal interface */
	  real_tty_in = isatty (fileno (stdin));
#if defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
	  if (real_tty_in)
	     { /* pick up current tty attributes, save them for later
		  and modify them to handle the tty according to ELAN */
	       struct termios our_attrs;
	       tcgetattr (0, &saved_attrs);
	       our_attrs = saved_attrs;

	       /* Input modes: No NL -> CR, CR -> NL mapping */ 
	       our_attrs.c_iflag &= ~INLCR;
	       our_attrs.c_iflag &= ~ICRNL;
		
	       /* Output modes: No CR -> NL, NL -> CRNL mapping */
	       our_attrs.c_oflag &= ~ONLCR;
	       our_attrs.c_oflag &= ~OCRNL;

	       /* Control modes: if they were bad, we can't help it */
	       /* Local modes: No Canonical mode (special handling of EOF,EOL, etc)
		               No echo */
	       our_attrs.c_lflag &= ~ECHO;
	       our_attrs.c_lflag &= ~ICANON;

	       /* Input handling: minimum nr of chars = 2 (we do our own interpretation)
				  no timeout on input (wait when we ask for a character) */
	       our_attrs.c_cc[VMIN] = 1;
	       our_attrs.c_cc[VTIME] = 0;

	       /* set new tty attributes */
	       tcsetattr (0, TCSANOW, &our_attrs);
	     };
#endif /* HAVE_TERMIOS_H && HAVE_SYS_IOCTL_H */

	  /* check if stdout is connected to some terminal interface */
	  real_tty_out = isatty (fileno (stdout));
	  if (real_tty_out)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { elanwin = initscr ();
	       noecho ();
		 cbreak ();
		 nodelay (elanwin, FALSE);
		 keypad (elanwin, TRUE);
		 clearok (elanwin, FALSE);
		 getmaxyx (elanwin, nr_lines, nr_columns);

	     }
#elif defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
	     { struct winsize tty_win;
	       ioctl (1, TIOCGWINSZ, &tty_win);
	       nr_lines = tty_win.ws_row;
	       nr_columns = tty_win.ws_col;
	     }
#else
#error "GLORP"
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	  else
	     { /* probably redirected output */
	       nr_lines = rts_maxint ();
	       nr_columns = MAX_TEXT_LEN;
	     };

	  /* clear screen */
	  /* rts_putchar (ELAN_CLEARSCREEN); */
	};

/*
   Finish term io is called by program aborts either through panic
   or through the signal handler installed by rts_misc. The only
   thing it does is to reinstall our saved terminal attributes.
*/
void rts_finish_termio ()
	{ /* bring cursor to bottom of screen while flushing output */
	  rts_cursor (1, nr_lines);
	  if (real_tty_in)
#if defined(HAVE_CURSES_H) || defined(HAVE_NCURSES_H)
	     { clearok (elanwin, FALSE);
	       endwin ();
	     }
#elif defined(HAVE_TERMIOS_H) && defined(HAVE_SYS_IOCTL_H)
	     tcsetattr (0, TCSANOW, &saved_attrs);
#else
	     #error "GLORP"
#endif /* HAVE_CURSES_H || HAVE_NCURSES_H */
	};

