/*
   File: dcg_binfile.h
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

   CVS ID: "$Id: dcg_binfile.h,v 1.4 2012/05/30 14:23:57 marcs Exp $"
*/
#ifndef IncDcgBinFile
#define IncDcgBinFile

/* Local includes */
#include <dcg.h>
#include <dcg_alloc.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Type definition and initialisation for binary files */
typedef struct dcg_bin_file_rec *BinFile;
BinFile dcg_bin_fopen (char *path, char *mode, int forced);
BinFile dcg_bin_fopen_path (char *dirpath, char *fname, char *mode, int forced);
void dcg_bin_fclose (BinFile *bf);
size_t dcg_bin_read (BinFile bf, void *buf, size_t nbytes);
size_t dcg_bin_write (BinFile bf, void *buf, size_t nbytes);

/* Binary saving routines */
void dcg_bin_save_eof     (BinFile bf);
void dcg_bin_save_char    (BinFile bf, char x);
void dcg_bin_save_int     (BinFile bf, int x);
void dcg_bin_save_int64   (BinFile bf, int64 x);
void dcg_bin_save_u_int   (BinFile bf, u_int x);
void dcg_bin_save_u_int64 (BinFile bf, u_int64 x);
void dcg_bin_save_real	  (BinFile bf, real x);
void dcg_bin_save_string  (BinFile bf, char *x);
void dcg_bin_save_header  (BinFile bf, char *tool, char *kind, char *version);

/* Binary loading routines */
void dcg_bin_verify_eof    (BinFile bf);
void dcg_bin_load_char     (BinFile bf, char *x);
void dcg_bin_load_int      (BinFile bf, int *x);
void dcg_bin_load_int64    (BinFile bf, int64 *x);
void dcg_bin_load_u_int    (BinFile bf, u_int *x);
void dcg_bin_load_u_int64  (BinFile bf, u_int64 *x);
void dcg_bin_load_real     (BinFile bf, real *x);
void dcg_bin_load_string   (BinFile bf, char **x);
void dcg_bin_verify_header (BinFile bf, char *tool, char *kind, char *version);
int dcg_bin_verify_version (BinFile bf, char *tool, char *kind, char *version);

/* Special */
void dcg_bin_pool_load_string (BinFile bf, pool my_pool, char **x);
void dcg_bin_pool_load_unaligned_string (BinFile bf, pool my_pool, char **x);

/* Define save shorthands */
#define save_char(bf,x)		dcg_bin_save_char (bf,x)
#define save_int(bf,x)		dcg_bin_save_int (bf,x)
#define save_int32(bf,x)	dcg_bin_save_int (bf,x)
#define save_int64(bf,x)	dcg_bin_save_int64 (bf,x)
#define save_u_int(bf,x)	dcg_bin_save_u_int (bf,x)
#define save_u_int32(bf,x)	dcg_bin_save_u_int (bf,x)
#define save_u_int64(bf,x)	dcg_bin_save_u_int64 (bf,x)
#define save_real(bf,x)		dcg_bin_save_real (bf,x)
#define save_string(bf,x)	dcg_bin_save_string (bf,x)
#define save_vptr(bf,x)
#define savesize(bf,x)		dcg_bin_save_int (bf,x)

/* Define load shorthands */
#define load_char(bf,x)		dcg_bin_load_char (bf,x)
#define load_int(bf,x)		dcg_bin_load_int (bf,x)
#define load_int32(bf,x)	dcg_bin_load_int (bf,x)
#define load_int64(bf,x)	dcg_bin_load_int64 (bf,x)
#define load_u_int(bf,x)	dcg_bin_load_u_int (bf,x)
#define load_u_int32(bf,x)	dcg_bin_load_u_int (bf,x)
#define load_u_int64(bf,x)	dcg_bin_load_u_int64 (bf,x)
#define load_real(bf,x)		dcg_bin_load_real (bf,x)
#define load_string(bf,x)	dcg_bin_load_string (bf,x)
#define load_vptr(bf,x)		*x = vptr_nil
#define loadsize(bf,x)		dcg_bin_load_int (bf,x)

/* Define special shorthands */
#define dcg_bin_load_unaligned_string(bf,x) dcg_bin_pool_load_unaligned_string(bf,my_default_pool,x)

#ifdef __cplusplus
}
#endif
#endif /* IncDcgBinFile */
