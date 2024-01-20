/*
   File: dcg_fileutil.c
   Defines routines to handle files, filenames, links and directories

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

   CVS ID: "$Id: dcg_fileutil.c,v 1.9 2012/01/11 15:43:01 marcs Exp $"

   MS: Check all of the WIN32 file util code for a better implementation
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

/* File opening */
FILE *dcg_fopen (char *path, char *mode)
{ FILE *fd = fopen (path, mode);
  if (fd == NULL)
    dcg_panic ("could not open file '%s' for mode '%s'", path, mode);
  return (fd);
}

FILE *dcg_try_fopen (char *path, char *mode)
{ FILE *fd = fopen (path, mode);
  return (fd);
}

FILE *dcg_fopen_path (char *dirpath, char *fname, char *mode)
{ FILE *fd;
  string cpath = dcg_new_fmtd_string ("%s%c%s", dirpath, DIR_SEP, fname);
  fd = fopen (cpath, mode);
  if (fd == NULL)
    dcg_panic ("could not open file '%s' for mode '%s'", cpath, mode);
  detach_string (&cpath);
  return (fd);
}

FILE *dcg_try_fopen_path (char *dirpath, char *fname, char *mode)
{ FILE *fd;
  string cpath = dcg_new_fmtd_string ("%s%c%s", dirpath, DIR_SEP, fname);
  fd = fopen (cpath, mode);
  detach_string (&cpath);
  return (fd);
}

/* primitive routines to check the existence of a path */
int dcg_file_exists (char *path)
{
#ifdef WIN32
  struct _stat sbuf;
  if (_stat (path, &sbuf)) return (0);		/* Check CreateFile */
#else
  struct stat sbuf;
  if (stat (path, &sbuf)) return (0);
#endif
  return (1);
}

int dcg_file_ext_exists (char *path, char *ext)
{ string npath = dcg_new_fmtd_string ("%s.%s", path, ext);
  int st = dcg_file_exists (npath);
  detach_string (&npath);
  return (st);
}

int dcg_is_normal_file (char *path)
{
#ifdef WIN32
  struct _stat sbuf;
  if (_stat (path, &sbuf)) return (0);
  return (sbuf.st_mode & _S_IFREG);
#else
  struct stat sbuf;
  if (stat (path, &sbuf)) return (0);
  return (S_ISREG (sbuf.st_mode));
#endif
}

int dcg_is_directory (char *path)
{
#ifdef WIN32
  struct _stat sbuf;
  if (_stat (path, &sbuf)) return (0);
  return (sbuf.st_mode & _S_IFDIR);
#else
  struct stat sbuf;
  if (stat (path, &sbuf)) return (0);
  return (S_ISDIR (sbuf.st_mode));
#endif
}

int dcg_is_symlink (char *path)
{
#ifdef WIN32
  return (0);		/* No real symlinks in Win32 */
#else
  struct stat sbuf;
  if (lstat (path, &sbuf)) return (0);
  return (S_ISLNK (sbuf.st_mode));
#endif
}

int dcg_file_mtime (char *path, time_t *mtime)
{
#ifdef WIN32
  struct _stat sbuf;
  if (_stat (path, &sbuf)) return (0);
#else
  struct stat sbuf;
  if (stat (path, &sbuf)) return (0);
#endif
  *mtime = sbuf.st_mtime;
  return (1);
}

/*
   Directories and removing files and directories
*/

/*
   Check whether the argument exists as a directory
   If not do a controlled mkdir
*/
void dcg_mkdir (char *dname)
#ifdef WIN32
{ struct _stat buf;
  if (_stat (dname, &buf))
    { if (!CreateDirectory (dname, NULL))
        dcg_panic ("could not create directory '%s', error code = %d",
		   dname, GetLastError ());
    }
  else if (!(buf.st_mode & _S_IFDIR))
    dcg_panic ("file '%s' is not a directory", dname);
}
#else
{ struct stat buf;
  if (stat (dname, &buf))
    { if (mkdir (dname, 0775))
        dcg_panic ("could not create directory '%s', errno = %d", dname, errno);
    }
  else if (!S_ISDIR (buf.st_mode))
    dcg_panic ("file '%s' is not a directory", dname);
}
#endif

/*
   Do a controlled remove directory
*/
void dcg_rmdir (char *dname)
#ifdef WIN32
{ struct _stat buf;
  if (_stat (dname, &buf)) return;
  else if (!(buf.st_mode & _S_IFDIR))
    dcg_panic ("'%s' is not a directory", dname);
  else if (!RemoveDirectory (dname))
    dcg_panic ("could not remove directory '%s', error = %d",
	       dname, GetLastError ());
}
#else /* POSIX style */
{ struct stat buf;
  if (stat (dname, &buf)) return;
  else if (!S_ISDIR (buf.st_mode))
    dcg_panic ("'%s' is not a directory", dname);
  else if (rmdir (dname))
    dcg_panic ("could not remove directory '%s', errno = %d", dname, errno);
}
#endif

/*
   Do a controlled delete
*/
void dcg_unlink (char *fname)
#ifdef WIN32
{ struct _stat buf;
  if (_stat (fname, &buf)) return;
  else if (unlink (fname))
    dcg_panic ("could not remove '%s'", fname);
}
#else /* POSIX style */
{ struct stat buf;
  if (stat (fname, &buf) && lstat (fname, &buf)) return;
  else if (unlink (fname))
    dcg_panic ("could not remove '%s'", fname);
}
#endif

#ifndef WIN32
/*
   Create a symlink. Delete the original one if it existed
*/
void dcg_mksymlink (string trgt, string as)
{ if (unlink (as))
    { if (errno != ENOENT)
        dcg_panic ("could not unlink '%s'");
    };
  if (symlink (trgt, as))
    dcg_panic ("could not create symlink '%s' -> '%s'", as, trgt);
}

char *dcg_rdsymlink (char *link)
{ char buf[MAXPATHLEN + 1];
  int stat = readlink (link, buf, MAXPATHLEN);
  if (stat != -1)
    { buf[stat] = '\0';
      return (new_string (buf));
    }
  else dcg_panic ("symlink '%s' is invalid", link);
  return (string_nil);
}

#endif

/* path construction */
char *dcg_construct_path (char *directory, char *fname)
{ return (dcg_new_fmtd_string ("%s%c%s", directory, DIR_SEP, fname));
}

char *dcg_getcwd ()
{ char buf[MAXPATHLEN+1];
#ifdef WIN32
  if (!GetCurrentDirectory (MAXPATHLEN+1, buf))
    dcg_panic ("could not get current working directory, error = %d", GetLastError ());
#else
  if (!getcwd (buf, MAXPATHLEN))
    dcg_panic ("could not get current working directory, errno = %d", errno);
#endif
  return (new_string (buf));
}

