##
## Process this file with automake to produce Makefile.in
##
## CVSID: "$Id: Makefile.am,v 1.11 2011/10/21 13:00:22 marcs Exp $"
##
bin_PROGRAMS = dcg

dcg_SOURCES =	dcg_parser.y \
		dcg_parser2.h dcg_parser2.c \
		dcg_lex.h dcg_lex.l \
		dcg_code.h dcg_code.c \
		main.c \
		options.h options.c \
		coder.h coder.c \
		analyze.h analyze.c \
		xform_coder.h xform_coder.c

dcg_CFLAGS = @DBG_OPT@ -Wall $(AM_CFLAGS)

dcg_DEPENDENCIES = $(top_builddir)/libdcg/libdcg.la

dcg_LDADD = $(top_builddir)/libdcg/libdcg.la

INCLUDES = -I$(top_srcdir)/libdcg \
	   -DLIBPATH=\"${libdir}\" -DINCPATH=\"${includedir}\"

# Special rules for parser and lexer
dcg_parser.h dcg_parser.c: dcg_parser.y
	$(BISON) -v -d -o dcg_parser.c -p pdcg_ $<

dcg_lex.c: dcg_lex.l
	$(LEX) -v -t --nounistd $< > $@

EXTRA_DIST = dcg_parser.h dcg_parser.c ndcg.dcg dcg_lex.c

CLEANFILES = dcg_parser.h dcg_parser.c dcg_parser.output dcg_lex.c
