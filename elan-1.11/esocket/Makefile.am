##
## Process this file with automake to produce Makefile.in
##
## CVS ID: "$Id: Makefile.am,v 1.6 2007/01/17 23:52:28 marcs Exp $"
##
elandir = $(datadir)/elan
preldir = $(elandir)/prelude
incldir = $(elandir)/incl
sourcedir = $(elandir)/src
elancc = ../gen/elancc

prel_DATA = socket.ppe

source_DATA = socket.e

incl_HEADERS = elan_socket.h

lib_LTLIBRARIES = libesocket.la

libesocket_la_SOURCES = elan_socket.h elan_socket.c

VERSION_FLAGS = $(subst .,:,$(VERSION))

libesocket_la_LDFLAGS = -version-info $(VERSION_FLAGS)

libesocket_la_CFLAGS = @DBG_OPT@ -Wall $(AM_CFLAGS)

INCLUDES = -I$(top_srcdir)/erts

socket.ppe: socket.e
	$(elancc) -pp socket.e

EXTRA_DIST = socket.e

CLEANFILES = socket.ppe
