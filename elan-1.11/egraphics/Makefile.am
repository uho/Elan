##
## Process this file with automake to produce Makefile.in
##
## CVS ID: "$Id: Makefile.am,v 1.2 2007/03/03 12:10:02 marcs Exp $"
##
elandir = $(datadir)/elan
preldir = $(elandir)/prelude
incldir = $(elandir)/include
sourcedir = $(elandir)/src
elancc = ../gen/elancc

prel_DATA = graphics.ppe

source_DATA = graphics.e

incl_HEADERS = elan_graphics.h

lib_LTLIBRARIES = libegraphics.la

libegraphics_la_SOURCES = elan_graphics.h elan_graphics.c \
			  egr_objects.h egr_objects.c \
			  Primitive.h PrimitiveP.h Primitive.c

VERSION_FLAGS = $(subst .,:,$(VERSION))

libegraphics_la_LDFLAGS = -version-info $(VERSION_FLAGS)

libegraphics_la_CFLAGS = @DBG_OPT@ -Wall -I@X11IDIR@ $(AM_CFLAGS)

INCLUDES = -I$(top_srcdir)/erts

graphics.ppe: graphics.e
	$(elancc) -pp graphics.e

EXTRA_DIST = graphics.e

CLEANFILES = graphics.ppe
