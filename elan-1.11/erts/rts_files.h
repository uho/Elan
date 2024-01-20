/*
   File: rts_files.h
   Provides files

   CVS ID: "$Id: rts_files.h,v 1.2 2004/12/18 13:24:51 marcs Exp $"
*/

#ifndef IncRtsFiles
#define IncRtsFiles

typedef struct elan_file_rec
	{ FILE *stream;
	  char *fname;
	  int opened;
	  int dir;
	} *elan_file;

/* Runtime support */
extern void rts_detach_file (elan_file *fptr);

/* Opening, Closing and Cleaning */
extern elan_file rts_sequential_file (int dir, char *name);
extern void rts_close_file (elan_file old);
extern void rts_erase_file (elan_file old);
extern int rts_opened_file (elan_file old);
extern int rts_eof_file (elan_file old);

/* Transput calls */
extern void rts_put_file_text (elan_file f, char *txt);

#endif /* IncRtsFiles */
