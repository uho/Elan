/*
   File: rts_files.c
   Provides files

   CVS ID: "$Id: rts_files.c,v 1.3 2005/02/10 12:53:07 marcs Exp $"
*/

/* global includes */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

/* local includes */
#include "rts_error.h"
#include "rts_alloc.h"
#include "rts_texts.h"
#include "rts_files.h"

/*
   Rts Support
   Files can be attached directly since their internal structure is
   not visible. To detach we must check if the stream is still open
   If so, we close it, detach the name and then self destruct
*/
void rts_detach_file (elan_file *fptr)
	{ elan_file old = (elan_file) rts_predetach ((char **) fptr);
	  if (old == NULL) return;
	  rts_close_file (old);
	  rts_detach ((char **) &old -> fname);
	  rts_detach ((char **) &old);
	};

/*
   Opening, closing and erasing
*/

/* FILE PROC sequential file (TRANSPUTDIRECTION dir, TEXT name) */
elan_file rts_sequential_file (int dir, char *name)
	{ FILE *stream = NULL;
	  elan_file new;

	  /* Check validity of file name */
	  if (name == NULL)
	     rts_error ("FILE PROC sequential file (TRANSPUTDIRECTION dir, TEXT name)\n\
			called with uninitialized argument");

	  /* Allocate structure and fill partially */
	  new = (elan_file) rts_malloc (sizeof (struct elan_file_rec));
	  new -> opened = 0;
	  new -> fname = rts_attach (&name);
	  new -> dir = dir;

	  /* open the stream according to its dir */
	  switch (dir)
	     { case 1: stream = fopen (new -> fname, "r"); break;
	       case 2: stream = fopen (new -> fname, "w"); break;
	       case 3: stream = popen (new -> fname, "r"); break;
	       case 4: stream = popen (new -> fname, "w"); break;
	       default:
	          rts_error ("FILE PROC sequential file (TRANSPUTDIRECTION dir, TEXT name)\n\
			     called with illegal direction");
	     };
	  if (stream != NULL) new -> opened = 1;
	  new -> stream = stream;
	  return (new);
	};

/* PROC close (FILE f) */
void rts_close_file (elan_file old)
	{ if (old == NULL)
	     rts_error ("PROC close (FILE f) called with uninitialized argument");
	  if (old -> opened)
	     { switch (old -> dir)
		  { case 1:
		    case 2: fclose (old -> stream); break;
		    case 3:
		    case 4: pclose (old -> stream); break;
		    default:
		       rts_panic ("Closing file with illegal direction");
		  };
	     };
	  old -> opened = 0;
	};

/* PROC erase (FILE f) */
void rts_erase_file (elan_file old)
	{ if (old == NULL)
	     rts_error ("PROC erase (FILE f) called with uninitialized argument");
	  rts_close_file (old);
	  if ((old -> dir == 1) || (old -> dir == 2))
	     unlink (old -> fname);
	};

/* BOOL PROC opened (FILE f) */
int rts_opened_file (elan_file old)
	{ if (old == NULL)
	     rts_error ("BOOL PROC opened (FILE f) called with uninitialized argument");
	  return (old -> opened);
	};

/* BOOL PROC eof (FILE f) */
int rts_eof_file (elan_file old)
	{ int ch;
	  if (old == NULL)
	     rts_error ("BOOL PROC eof (FILE f) called with uninitialized argument");
	  if (!old -> opened)
	     rts_error ("BOOL PROC eof (FILE f) called with unopened file");
	  if (feof (old -> stream)) return (1);
	  if ((old -> dir != 1) && (old -> dir != 3)) return (0);
	  ch = fgetc (old -> stream);
	  if (ch == EOF) return (1);
	  ungetc (ch, old -> stream);
	  return (0);
	};

/*
   Transput operations
*/

/* PROC put (FILE f, TEXT t) */
void rts_put_file_text (elan_file f, char *txt)
	{ if ((f == NULL) || (txt == NULL))
	     rts_error ("PROC put (FILE f, TEXT t) called with uninitialized argument");
	  if (!f -> opened)
	     rts_error ("PROC put (FILE f, TEXT t) called with unopened file");
	  if ((f -> dir != 2) && (f -> dir != 4))
	     rts_error ("PROC put (FILE f, TEXT t) called with file of wrong direction");
	  fputs (txt, f -> stream);
	};

/* PROC get line (FILE f, TEXT VAR t) */
void rts_get_line_file_text (elan_file f, char **txt)
	{ char buf[MAX_TEXT_LEN + 1];
	  char *new;
	  int len;
	  if (f == NULL)
	     rts_error ("PROC get line (FILE f, TEXT VAR t) called with uninitialized argument");
	  if (!f -> opened)
	     rts_error ("PROC get line (FILE f, TEXT VAR t) called with unopened file");
	  if ((f -> dir != 1) && (f -> dir != 3))
	     rts_error ("PROC get line (FILE f, TEXT t) called with file of wrong direction");
	  if (fgets (buf, MAX_TEXT_LEN, f -> stream) != NULL)
	     { len = strlen (buf);
	       if (buf[len - 1] == '\n') buf[--len] = '\0';
	       new = rts_malloc (len + 1);
	       strcpy (new, buf);
	     }
	  else
	     { new = rts_malloc (1);
	       *new = '\0';
	     };
	  rts_detach (txt);
	  *txt = new;
	};

/* PROC get (FILE f, TEXT VAR t) */
void rts_get_file_text (elan_file f, char **txt)
	{ char buf[MAX_TEXT_LEN + 1];
	  int len = 0;
	  char *new;
	  if (f == NULL)
	     rts_error ("PROC get (FILE f, TEXT VAR t) called with uninitialized argument");
	  if (!f -> opened)
	     rts_error ("PROC get (FILE f, TEXT VAR t) called with unopened file");
	  if ((f -> dir != 1) && (f -> dir != 3))
	     rts_error ("PROC get (FILE f, TEXT t) called with file of wrong direction");
	  while (len <= MAX_TEXT_LEN)
	     { int ch = fgetc (f -> stream);
	       if (ch == EOF) break;
	       if (isspace (ch))
		  { if (len) break;
		    continue;
		  };
	       buf[len] = ch;
	       len++;
	     };
	  buf[len] = '\0';
	  new = rts_malloc (len + 1);
	  strcpy (new, buf);
	  rts_detach (txt);
	  *txt = new;
	};

