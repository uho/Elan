##
## Process this file with automake to produce Makefile.in
##
elandir = $(datadir)/elan
preldir = $(elandir)/prelude

bin_PROGRAMS = elancc

elancc_SOURCES = elan_ast.dcg elan_ast.c elan_ast.h \
		 elan_imc.dcg elan_imc.c elan_imc.h \
		 decl_tree.h decl_tree.c decl_tree.def \
		 main.c options.h options.c \
		 symbol_table.h symbol_table.c \
		 ast_utils.h ast_utils.c \
		 lexer.h lexer.c \
		 parser.h parser.c \
		 contfree.h contfree.c \
		 contsens.h contsens.c \
		 pre_checker.h pre_checker.c \
		 evaluate.h evaluate.c \
		 ident.h ident.c \
		 type_table.h type_table.c \
		 checker.h checker.c \
		 imc_utils.h imc_utils.c \
		 imc_gen.h imc_gen.c \
		 imc_opt.h imc_opt.c \
		 tgt_gen.h tgt_gen.c \
		 tgt_gen_machdep.h tgt_gen_machdep.c \
		 tgt_gen_common.h tgt_gen_common.c \
		 tgt_gen_intelx86.h tgt_gen_intelx86.c \
		 tgt_gen_amd64.h tgt_gen_amd64.c \
		 tgt_gen_sparc.h tgt_gen_sparc.c \
		 tgt_gen_alpha.h tgt_gen_alpha.c \
		 tgt_gen_vax.h tgt_gen_vax.c \
		 backend.h backend.c \
		 warshall.h warshall.c

INCLUDES = -DVERSION=\"${VERSION}\" \
	   -DTCPU=\"${TCPU}\" \
	   -DTOS=\"${TOS}\" \
	   -DPRELDIR=\"${preldir}\" \
	   -DLIBDIR=\"${libdir}\" \
	   -DX11LIBDIR=\"@X11LDIR@\"

elancc_LDADD = -L@DCG_LPATH@ -ldcg2 -ldcg

elancc_CFLAGS = @DBG_OPT@ @CURSES_OPT@ @THREADS_OPT@ -Wall -I@DCG_IPATH@ $(AM_CFLAGS)

elan_ast.h elan_ast.c elan_ast.def: elan_ast.dcg decl_tree.def
	@DCG@ -nrd elan_ast.dcg

elan_imc.h elan_imc.c elan_imc.def: elan_imc.dcg elan_ast.dcg decl_tree.def
	@DCG@ -nrd elan_imc.dcg

CLEANFILES = elan_ast.h elan_ast.c elan_ast.def \
	     elan_imc.h elan_imc.c elan_imc.def
