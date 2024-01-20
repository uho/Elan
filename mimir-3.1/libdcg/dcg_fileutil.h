/*
   File: dcg_fileutil.h
   Defines routines to handle files, filenames, links and directories

   Copyright 2009-2011 M. Seutter

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

   CVS ID: "$Id: dcg_fileutil.h,v 1.8 2011/10/14 11:19:10 marcs Exp $"
*/
#ifndef IncDcgFileutil
#define IncDcgFileutil

/* Local includes */
#include <dcg.h>

#ifdef __cplusplus
extern "C" {
#endif

/* General routines for all kind of files */
FILE *dcg_fopen (char *path, char *mode);
FILE *dcg_try_fopen (char *path, char *mode);
FILE *dcg_fopen_path (char *dirpath, char *fname, char *mode);
FILE *dcg_try_fopen_path (char *dirpath, char *fname, char *mode);
int dcg_file_exists (char *path);
int dcg_file_ext_exists (char *path, char *ext);
int dcg_is_normal_file (char *path);
int dcg_file_mtime (char *path, time_t *mtime);

/* Directories and removing files and directories */
int dcg_is_directory (char *path);
void dcg_mkdir (char *dname);
void dcg_rmdir (char *dname);
void dcg_unlink (char *path);

/* Symlink handling */
int dcg_is_symlink (char *path);

#ifndef WIN32
void dcg_mksymlink (char *trgt, char *as);
char *dcg_rdsymlink (char *link);
#endif

/* Filename construction */
char *dcg_construct_path (char *directory, char *fname);
char *dcg_getcwd ();

#ifdef __cplusplus
}
#endif
#endif /* IncDcgFileutil */
