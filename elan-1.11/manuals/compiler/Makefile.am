## Process this file with automake to produce Makefile.in
docdir = $(prefix)/doc
doc_DATA = $(PACKAGE)-comp-$(VERSION).ps

$(PACKAGE)-comp-$(VERSION).ps: comp.ps
	cp comp.ps $(PACKAGE)-comp-$(VERSION).ps

EXTRA_DIST = comp.ps \
	     comp.tex \
	     flow.fig \
	     flow.eps \
	     flow2.fig \
	     flow2.eps

CLEANFILES = comp.dvi comp.aux comp.log \
	     $(PACKAGE)-comp-$(VERSION).ps
