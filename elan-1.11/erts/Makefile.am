##
## Process this file with automake to produce Makefile.in
##
## CVS ID: "$Id: Makefile.am,v 1.8 2007/01/17 23:52:27 marcs Exp $"
##
elandir = $(datadir)/elan
incldir = $(elandir)/include
incl_HEADERS =	rts_alloc.h \
		rts_error.h \
		rts_term.h \
		rts_init.h \
		rts_ints.h \
		rts_files.h \
		rts_reals.h \
		rts_texts.h \
		rts_random.h \
		rts_system.h

lib_LTLIBRARIES = liberts.la

liberts_la_SOURCES = rts_alloc.h rts_alloc.c \
		     rts_error.h rts_error.c \
		     rts_term.h rts_term.c \
		     rts_init.h rts_init.c \
		     rts_ints.h rts_ints.c \
		     rts_files.h rts_files.c \
		     rts_reals.h rts_reals.c \
		     rts_texts.h rts_texts.c \
		     rts_random.h rts_random.c \
		     rts_system.h rts_system.c

VERSION_FLAGS = $(subst .,:,$(VERSION))

liberts_la_LDFLAGS = -version-info $(VERSION_FLAGS)

liberts_la_CFLAGS = @DBG_OPT@ @CURSES_OPT@ @THREADS_OPT@ -Wall $(AM_CFLAGS)

## Conditionally generate local test program
EXTRA_PROGRAMS = erts_test

erts_test_SOURCES = rts_test.c

erts_test_DEPENDENCIES = liberts.la

erts_test_LDADD = liberts.la @CURSES_LIB@

test: $(EXTRA_PROGRAMS)

CLEANFILES = $(EXTRA_PROGRAMS)
