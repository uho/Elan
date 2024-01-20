/*
   File: dcg_binfile.c
   Defines routines to save and load binary data in a machine and OS independent way.

   The format of the binary IO is taken from the ELF standard for the
   primitive types char, int and string. Furthermore we assume that
   structured data is saved recursively, field by field. Binary files
   contain a specific header, version and checksum.

   Copyright 2009-2011 Radboud University of Nijmegen

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   CVS ID: "$Id: dcg_binfile.c,v 1.7 2012/06/08 13:46:22 marcs Exp $"
*/

/* include config.h if autoconfigured */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif /* HAVE_CONFIG_H */

/* global includes */
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>

/* local includes */
#include "dcg.h"
#include "dcg_error.h"
#include "dcg_alloc.h"
#include "dcg_string.h"
#include "dcg_fileutil.h"
#include "dcg_binfile.h"


/* Binary file type definition and initialisation */
struct dcg_bin_file_rec
{ FILE *file;
  char *path;
  int writing;
  u_int32 checksum;
};
	  
BinFile dcg_bin_fopen (char *path, char *mode, int forced)
{ char *bmode = NULL;
  int writing = 0;
  FILE *file;
  BinFile bf;

  /* Verify file mode */
  if (strcmp (mode, "rb") == 0)		{ writing = 0; bmode = "rb"; }
  else if (strcmp (mode, "r") == 0)	{ writing = 0; bmode = "rb"; }
  else if (strcmp (mode, "wb") == 0)	{ writing = 1; bmode = "wb"; }
  else if (strcmp (mode, "w") == 0)	{ writing = 1; bmode = "wb"; }
  else
    dcg_abort ("dcg_bin_fopen", "Illegal mode '%s' to open binary file '%s'", mode, path);

  /* Open the file: this must succeed when forced */
  file = fopen (path, bmode);
  if (file == NULL)
    { if (!forced) return (NULL);
      dcg_panic ("Could not open binary file '%s' with mode '%s'", path, mode);
    };

  /* Allocate Binfile record to fill: we assume the default pool */
  bf = (BinFile) dcg_pool_malloc (my_default_pool, sizeof (struct dcg_bin_file_rec));
  bf -> file = file;
  bf -> path = new_string (path);
  bf -> writing = writing;
  bf -> checksum = 0;
  return (bf);
}

BinFile dcg_bin_fopen_path (char *dirpath, char *fname, char *mode, int forced)
{ string cpath = dcg_new_fmtd_string ("%s%c%s", dirpath, DIR_SEP, fname);
  BinFile bf = dcg_bin_fopen (cpath, mode, forced);
  detach_string (&cpath);
  return (bf);
}

void dcg_bin_fclose (BinFile *bf)
{ if ((bf == NULL) || (*bf == NULL))
    dcg_abort ("dcg_bin_fclose", "Called with null pointer");
  fclose ((*bf) -> file);
  dcg_detach ((void **) &(*bf) -> path);
  dcg_detach ((void **) bf);
}

size_t dcg_bin_read (BinFile bf, void *buf, size_t nbytes)
{ if (bf == NULL)	dcg_abort ("dcg_bin_read", "Called with null pointer");
  return (fread (buf, 1, nbytes, bf -> file));
}

size_t dcg_bin_write (BinFile bf, void *buf, size_t nbytes)
{ if (bf == NULL)	dcg_abort ("dcg_bin_write", "Called with null pointer");
  return (fwrite (buf, 1, nbytes, bf -> file));
}

/* Saving routines */
void dcg_bin_save_eof (BinFile bf)
{ if (bf == NULL)	dcg_abort ("dcg_bin_save_eof", "Called with null pointer");
  if (!bf -> writing)	dcg_abort ("dcg_bin_save_eof", "Binary file not open for writing");
  fputc ((0xff - bf -> checksum) & 0xff, bf -> file); 
}

void dcg_bin_save_char (BinFile bf, char x)
{ if (bf == NULL)	dcg_abort ("dcg_bin_save_char", "Called with null pointer");
  if (!bf -> writing)	dcg_abort ("dcg_bin_save_char", "Binary file not open for writing");
  bf -> checksum = (bf -> checksum + (u_int32) ((unsigned char) x)) & 0xff;
  fputc (x, bf -> file);
}

/*
   (Unsigned) ints are saved run length encoded according to the Dwarf 2 standard
   Chunks of 7 bit, beginning with the least significant bits are output until
   there are no more significant bits to output. The sign bit in each chunk is
   used to indicate if more chunks are following.
*/
void dcg_bin_save_int (BinFile bf, int x)
{ int value = x;
  int more = 1;
  do
    { int byte = value & 0x7f;				/* Cut off 7 lsbs */
      value >>= 7;					/* Discard them but keep sign */
      if ((value == 0) && !(byte & 0x40)) more = 0;
      if ((value == -1) && (byte & 0x40)) more = 0;
      if (more) byte |= 0x80;
      dcg_bin_save_char (bf, (char) byte);
    }
  while (more);
}

void dcg_bin_save_int64 (BinFile bf, int64 x)
{ int64 value = x;
  int more = 1;
  do
    { int byte = (int) (value & int64_const (0x7f));	/* Cut off 7 lsbs */
      value >>= 7;					/* Discard them but keep sign */
      if ((value == 0) && !(byte & 0x40)) more = 0;
      if ((value == -1) && (byte & 0x40)) more = 0;
      if (more) byte |= 0x80;
      dcg_bin_save_char (bf, (char) byte);
    }
  while (more);
}

void dcg_bin_save_u_int (BinFile bf, u_int x)
{ u_int value = x;
  do
    { int byte = value & 0x7f;				/* Cut off 7 lsbs */
      value >>= 7;
      if (value) byte |= 0x80;
      dcg_bin_save_char (bf, (char) byte);
    }
  while (value);
}

void dcg_bin_save_u_int64 (BinFile bf, u_int64 x)
{ u_int64 value = x;
  do
    { int byte = (int) (value & u_int64_const (0x7f));		/* Cut off 7 lsbs */
      value >>= 7;
      if (value) byte |= 0x80;
      dcg_bin_save_char (bf, (char) byte);
    }
  while (value);
}

/*
   Since all integer numbers are more or less saved in a little endian way,
   the real is saved as little endian
*/
void dcg_bin_save_real (BinFile bf, real x)
{ int i;
  char *dptr = (char *) &x;
#ifdef WORDS_BIGENDIAN
  for (i = sizeof (real) - 1; 0 <= i; i--)
    dcg_bin_save_char (bf, dptr[i]);
#else
  for (i = 0; i < sizeof (real); i++)
    dcg_bin_save_char (bf, dptr[i]);
#endif
}

/*
   Strings are saved as a length followed by the bytes of the string
*/
void dcg_bin_save_string (BinFile bf, char *x)
{ int len = (int) strlen (x);
  int ix;
  dcg_bin_save_int (bf, len);
  for (ix = 0; ix < len; ix++) dcg_bin_save_char (bf, x[ix]);
}

void dcg_bin_save_header (BinFile bf, char *tool, char *kind, char *version)
{ dcg_bin_save_string (bf, tool);
  dcg_bin_save_string (bf, kind);
  dcg_bin_save_string (bf, version);
}

/* Loading routines */
static void dcg_bin_check_bf (BinFile bf)
{ if (bf == NULL)	dcg_abort ("dcg_bin_check_bf", "Called with null pointer");
  if (bf -> writing)	dcg_abort ("dcg_bin_check_bf", "Binary file not open for reading");
}

static void dcg_bin_load_char_unchecked (BinFile bf, char *x)
{ int ch;
  ch = fgetc (bf -> file);
  if (ch == EOF)
    dcg_abort ("dcg_bin_load_char_unchecked", "Binary file '%s' ends prematurely", bf -> path);
  bf -> checksum = (bf -> checksum + (u_int32) ch) & 0xff;
  *x = (char) ch;
}

void dcg_bin_load_char (BinFile bf, char *x)
{ dcg_bin_check_bf (bf);
  dcg_bin_load_char_unchecked (bf, x);
}

void dcg_bin_verify_eof (BinFile bf)
{ char ch;
  dcg_bin_load_char (bf, &ch);			/* Load checksum */
  if ((bf -> checksum & 0xff) != 0xff)
    dcg_panic ("Binary file '%s' has an incorrect checksum", bf -> path);

  if (fgetc (bf -> file) != EOF)
    dcg_panic ("Binary file '%s' has trailing garbage", bf -> path);
}

void dcg_bin_load_int (BinFile bf, int *x)
{ int value = 0;
  int shift = 0;
  char bb;
  dcg_bin_check_bf (bf);
  do
    { dcg_bin_load_char_unchecked (bf, &bb);
      value |= (((u_int) (bb & 0x7f)) << shift);
      shift += 7;
    }
  while (bb & 0x80);
  if (shift > 31) shift = 31;
  if (bb & 0x40) value |= -(1 << shift);
  *x = value;
}

void dcg_bin_load_int64 (BinFile bf, int64 *x)
{ int64 value = 0;
  int shift = 0;
  char bb;
  dcg_bin_check_bf (bf);
  do
    { dcg_bin_load_char_unchecked (bf, &bb);
      value |= (((u_int64) (bb & 0x7f)) << shift);
      shift += 7;
    }
  while (bb & 0x80);
  if (shift > 63) shift = 63;
  if (bb & 0x40) value |= -(1 << shift);
  *x = value;
}

void dcg_bin_load_u_int (BinFile bf, u_int *x)
{ int value = 0;
  int shift = 0;
  char bb;
  dcg_bin_check_bf (bf);
  do
    { dcg_bin_load_char_unchecked (bf, &bb);
      value |= (((u_int) (bb & 0x7f)) << shift);
      shift += 7;
    }
  while (bb & 0x80);
  *x = value;
}

void dcg_bin_load_u_int64 (BinFile bf, u_int64 *x)
{ u_int64 value = 0;
  int shift = 0;
  char bb;
  dcg_bin_check_bf (bf);
  do
     { dcg_bin_load_char_unchecked (bf, &bb);
       value |= (((u_int64) (bb & 0x7f)) << shift);
       shift += 7;
     }
  while (bb & 0x80);
  *x = value;
}

void dcg_bin_load_real (BinFile bf, real *x)
{ int i;
  char ch;
  char *dptr = (char *) x;
#ifdef WORDS_BIGENDIAN
  for (i = sizeof (real) - 1; 0 <= i; i--)
    { dcg_bin_load_char (bf, &ch);
      dptr[i] = ch;
    };
#else
  for (i = 0; i < sizeof (real); i++)
    { dcg_bin_load_char (bf, &ch);
      dptr[i] = ch;
    };
#endif
}

void dcg_bin_load_string (BinFile bf, char **x)
{ char *buffer;
  int ix, size;
  dcg_bin_load_int (bf, &size);
  if (size < 0)
    dcg_abort ("dcg_bin_load_string", "Binary file '%s' contains a string of length %d",
	       bf -> path, size);
  else if (size >= MAXSTRLEN)
    dcg_abort ("dcg_bin_load_string", "Binary file '%s' contains a string of length %d",
	       bf -> path, size);
  *x = buffer = dcg_malloc (size + 1);
  for (ix = 0; ix < size; ix++)
    dcg_bin_load_char_unchecked (bf, &buffer[ix]);
  buffer[size] = '\0';
}

void dcg_bin_pool_load_string (BinFile bf, pool my_pool, char **x)
{ char *buffer;
  int ix, size;
  dcg_bin_load_int (bf, &size);
  if (size >= MAXSTRLEN)
    dcg_abort ("dcg_bin_pool_load_string", "Binary file '%s' contains a string of length %d",
	       bf -> path, size);
  *x = buffer = dcg_pool_malloc (my_pool, size + 1);
  for (ix = 0; ix < size; ix++)
    dcg_bin_load_char_unchecked (bf, &buffer[ix]);
  buffer[size] = '\0';
}

void dcg_bin_pool_load_unaligned_string (BinFile bf, pool my_pool, char **x)
{ char *buffer;
  int ix, size;
  dcg_bin_load_int (bf, &size);
  if (size >= MAXSTRLEN)
    dcg_abort ("dcg_bin_pool_load_unaligned_string",
	       "Binary file '%s' contains a string of length %d",
	       bf -> path, size);
  *x = buffer = dcg_pool_unaligned_malloc (my_pool, size + 1);
  for (ix = 0; ix < size; ix++)
    dcg_bin_load_char_unchecked (bf, &buffer[ix]);
  buffer[size] = '\0';
}

void dcg_bin_verify_header (BinFile bf, char *tool, char *kind, char *version)
{ char *hdr_tool;
  char *hdr_kind;
  char *hdr_version;

  /* Verify tool and kind */
  dcg_bin_load_string (bf, &hdr_tool);
  dcg_bin_load_string (bf, &hdr_kind);
  if (!streq (hdr_tool, tool) || !streq (hdr_kind, kind)) 
    dcg_panic ("File '%s' does not appear to be a binary %s %s file", bf -> path, tool, kind);
  detach_string (&hdr_tool);
  detach_string (&hdr_kind);

  /* Verify version */
  dcg_bin_load_string (bf, &hdr_version);
  if (!streq (hdr_version, version))
    { dcg_wlog ("File '%s' is generated by %s version %s", bf -> path, tool, hdr_version);
      dcg_wlog ("You should regenerate it with the current %s version %s", tool, version);
      dcg_exit (4);
    };
  detach_string (&hdr_version);
}

int dcg_bin_verify_version (BinFile bf, char *tool, char *kind, char *version)
{ char *hdr_tool;
  char *hdr_kind;
  char *hdr_version;
  int stat;

  /* Verify tool and kind */
  dcg_bin_load_string (bf, &hdr_tool);
  dcg_bin_load_string (bf, &hdr_kind);
  if (!streq (hdr_tool, tool) || !streq (hdr_kind, kind)) 
    dcg_panic ("File '%s' does not appear to be a binary %s %s file", bf -> path, tool, kind);
  detach_string (&hdr_tool);
  detach_string (&hdr_kind);

  /* Verify version */
  dcg_bin_load_string (bf, &hdr_version);
  stat = streq (hdr_version, version);
  detach_string (&hdr_version);
  return (stat);
}
