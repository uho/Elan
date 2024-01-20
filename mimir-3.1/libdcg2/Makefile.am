##
## Process this file with automake to produce Makefile.in
##
## CVSID: "$Id: Makefile.am,v 1.4 2005/02/09 14:50:39 marcs Exp $"
##
lib_LTLIBRARIES = libdcg2.la

include_HEADERS = dcg_plist.h dcg_plist.def dcg_plist_ops.h

libdcg2_la_SOURCES = dcg_plist.h dcg_plist.c \
		     dcg_plist_ops.h dcg_plist_ops.c

VERSION_FLAGS = $(subst .,:,$(VERSION))

libdcg2_la_LDFLAGS = -version-info $(VERSION_FLAGS)

libdcg2_la_CFLAGS = @DBG_OPT@ -Wall -I$(top_srcdir)/libdcg $(AM_CFLAGS)

EXTRA_DIST = dcg_plist.dcg

##
## Special rule for bootstrapping
##
dcg_plist.h dcg_plist.c dcg_plist.def: dcg_plist.dcg
	dcg dcg_plist.dcg
