## 
## Process this file with automake to produce Makefile.in
##
## CVSID: "$Id: Makefile.am,v 1.8 2011/08/13 15:58:24 marcs Exp $"
##
ACLOCAL_AMFLAGS = -I m4

if INSTALL_SOCKET
   SOCKET_SUBDIR = esocket
endif
if INSTALL_THREADS
   THREAD_SUBDIR = ethread
endif
if INSTALL_GRAPHICS
   GRAPHICS_SUBDIR = egraphics
endif

SUBDIRS = manuals erts gen prelude $(SOCKET_SUBDIR) $(THREAD_SUBDIR) $(GRAPHICS_SUBDIR) scripts exp

EXTRA_DIST = depcomp
