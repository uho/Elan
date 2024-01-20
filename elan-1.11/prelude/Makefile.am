##
## Process this file with automake to produce Makefile.in
##
## CVS ID: "$Id: Makefile.am,v 1.6 2005/02/09 11:37:08 marcs Exp $"
##
elandir = $(datadir)/elan
preldir = $(elandir)/prelude
sourcedir = $(elandir)/src
elancc = ../gen/elancc

prel_DATA = standard.ppe elan0.ppe kareltje.ppe

source_DATA = standard.e elan0.e kareltje.e

standard.ppe: standard.e
	$(elancc) -pp standard.e

elan0.ppe: elan0.e
	$(elancc) -pp elan0.e

kareltje.ppe: kareltje.e
	$(elancc) -pp kareltje.e

EXTRA_DIST = standard.e elan0.e kareltje.e

CLEANFILES = standard.ppe elan0.ppe kareltje.ppe
