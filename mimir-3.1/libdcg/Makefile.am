##
## Process this file with automake to produce Makefile.in
##
## CVSID: "$Id: Makefile.am,v 1.12 2011/10/14 11:19:09 marcs Exp $"
##
include_HEADERS = dcg_version.h \
		  dcg_error.h \
		  dcg_arith.h \
		  dcg_alloc.h \
		  dcg_string.h \
		  dcg_dstring.h \
		  dcg.h dcg.def \
		  dcg_dump.h \
		  dcg_random.h \
		  dcg_lexutil.h \
		  dcg_fileutil.h \
		  dcg_binfile.h \
		  dcg_procutil.h \
		  dcg_warshall.h

lib_LTLIBRARIES = libdcg.la

libdcg_la_SOURCES = dcg.h dcg_version.h \
		    dcg_error.h dcg_error.c \
		    dcg_arith.h dcg_arith.c \
		    dcg_alloc.h dcg_alloc.c \
		    dcg_string.h dcg_string.c \
		    dcg_dstring.h dcg_dstring.c \
		    dcg_dump.h dcg_dump.c \
		    dcg_random.h dcg_random.c \
		    dcg_lexutil.h dcg_lexutil.c \
		    dcg_fileutil.h dcg_fileutil.c \
		    dcg_binfile.h dcg_binfile.c \
		    dcg_procutil.h dcg_procutil.c \
		    dcg_warshall.h dcg_warshall.c

VERSION_FLAGS = $(subst .,:,$(VERSION))

libdcg_la_LDFLAGS = -version-info $(VERSION_FLAGS)

libdcg_la_CFLAGS = @DBG_OPT@ @MEMCHECK_OPT@ -Wall $(AM_CFLAGS)

