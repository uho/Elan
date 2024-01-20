dnl------------------------------------------------------------------
dnl File: configure.ac
dnl Top configuration file for autoconf/automake of mimir
dnl
dnl Process this file with autoconf to produce a configure script
dnl
dnl CVSID: "$Id: configure.ac,v 1.10 2012/12/22 16:18:58 marcs Exp $"
dnl------------------------------------------------------------------
AC_INIT(dcg/main.c)
AM_INIT_AUTOMAKE(mimir,3.1)
AM_CONFIG_HEADER(config.h)
mkdir -p m4
AC_CONFIG_MACRO_DIR([m4])
AM_SANITY_CHECK

dnl------------------------------------------------------------------
dnl Check to use libtool (needs canonical host)
dnl------------------------------------------------------------------
AC_CANONICAL_HOST
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)
echo 'Building new dcg generator for a' $host 'system'

dnl------------------------------------------------------------------
dnl Checks for programs.
dnl------------------------------------------------------------------
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O
AM_PROG_LEX
AC_PATH_PROG(BISON, bison)
AC_SUBST(YACC)

dnl------------------------------------------------------------------
dnl Checks for header files.
dnl------------------------------------------------------------------
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h stdint.h unistd.h)
AC_CHECK_HEADERS(string.h malloc.h ctype.h)
AC_CHECK_HEADERS(sys/types.h sys/stat.h sys/time.h)
AC_CHECK_HEADERS(sys/param.h sys/wait.h)

dnl------------------------------------------------------------------
dnl Checks for typedefs, structures, and compiler characteristics.
dnl------------------------------------------------------------------
AC_C_BIGENDIAN
AC_HEADER_TIME

dnl------------------------------------------------------------------
dnl Checks for functions
dnl------------------------------------------------------------------
AC_FUNC_VPRINTF

dnl------------------------------------------------------------------
dnl Checks for libraries.
dnl------------------------------------------------------------------
AC_CHECK_LIB(m, pow)

dnl------------------------------------------------------------------
dnl Check whether we must generate the debug version
dnl------------------------------------------------------------------
AC_ARG_ENABLE(debug, 
   AC_HELP_STRING([--enable-debug],[enable debug code]),
   [ case "${enableval}" in
        yes) debug=true ;;
        no)  debug=false ;;
        *)   AC_MSG_ERROR(bad value ${enableval} for --enable-debug) ;;
     esac ], [debug=false] )
if test x$debug = xtrue; then
   echo 'Compiling with debugging enabled, optimization off'
   dbg_opt='-DDEBUG -g'
else
   dbg_opt='-O2'
fi
AC_SUBST(DBG_OPT,$dbg_opt)

dnl------------------------------------------------------------------
dnl Check whether we must generate a version with memory checking
dnl This is performance costing, but may be handy.
dnl------------------------------------------------------------------
AC_ARG_ENABLE(memcheck,
   AC_HELP_STRING([--enable-memlog],[enable memory checking code]),
   [ case "${enableval}" in
        yes) memcheck=true ;;
        no)  memcheck=false ;;
        *)   AC_MSG_ERROR(bad value ${enableval} for --enable-memcheck) ;;
     esac ], [memcheck=false] )
if test x$memcheck = xtrue; then
   echo 'Compiling with memory checking enabled'
   memcheck_opt='-DMEMCHECK'
else
   memcheck_opt=''
fi
AC_SUBST(MEMCHECK_OPT,$memcheck_opt)

dnl------------------------------------------------------------------
dnl When configuring we (re)generate libdcg/dcg_version.h
dnl In this way we need not set any special dependencies and let
dnl depcomp figure things out. Moreover this makes the Version check
dnl also portible in a MS Visual C environment
dnl Generate it in the build directory so that the source can be
dnl read-only (one of the checks by "make distcheck").
dnl------------------------------------------------------------------
version_file=./libdcg/dcg_version.h
$mkdir_p libdcg
echo 'Generating DCG version file' $version_file
echo '/*' > $version_file
echo '   File: dcg_version.h' >> $version_file
echo -n '   Generated on: ' >> $version_file
date >> $version_file
echo '*/' >> $version_file
echo '#ifndef IncDcgVersion' >> $version_file
echo '#define IncDcgVersion' >> $version_file
echo '#define DCG_VERSION' '"'${VERSION}'"' >> $version_file
echo '#endif /* IncDcgVersion */' >> $version_file

dnl------------------------------------------------------------------
dnl Check if make macro gets set
dnl------------------------------------------------------------------
AC_PROG_MAKE_SET

dnl------------------------------------------------------------------
dnl Define variable where the header files
dnl will be stored after installation
dnl------------------------------------------------------------------
AC_SUBST(includedir,['$(datadir)/dcg/include'])

dnl------------------------------------------------------------------
dnl Specify which Makefiles should be generated
dnl------------------------------------------------------------------
AC_OUTPUT([
Makefile
libdcg/Makefile
dcg/Makefile
libdcg2/Makefile
manuals/Makefile
manuals/man1/Makefile
])
