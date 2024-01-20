## Process this file with automake to produce Makefile.in
docdir = $(prefix)/doc
doc_DATA = $(PACKAGE)-manual-$(VERSION).ps

$(PACKAGE)-manual-$(VERSION).ps: $(PACKAGE).ps
	cp $(PACKAGE).ps $(PACKAGE)-manual-$(VERSION).ps

EXTRA_DIST = elan.ps \
	     elan.tex

CLEANFILES = elan.dvi elan.aux elan.log \
	     $(PACKAGE)-manual-$(VERSION).ps
