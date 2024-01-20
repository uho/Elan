##
## Process this file with automake to produce Makefile.in
##
## CVS ID: "$Id: Makefile.am,v 1.2 2007/01/17 23:52:33 marcs Exp $"
##
elandir = $(datadir)/elan
preldir = $(elandir)/prelude
incldir = $(elandir)/include
sourcedir = $(elandir)/src
elancc = ../gen/elancc

prel_DATA = threads.ppe

source_DATA = threads.e

incl_HEADERS = elan_threads.h

lib_LTLIBRARIES = libethread.la

libethread_la_SOURCES = elan_threads.h elan_threads.c

VERSION_FLAGS = $(subst .,:,$(VERSION))

libethread_la_LDFLAGS = -version-info $(VERSION_FLAGS)

libethread_la_CFLAGS = @DBG_OPT@ -Wall $(AM_CFLAGS)

INCLUDES = -I$(top_srcdir)/erts

threads.ppe: threads.e
	$(elancc) -pp threads.e

EXTRA_DIST = threads.e

CLEANFILES = threads.ppe
