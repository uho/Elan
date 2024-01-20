dnl------------------------------------------------------------------
dnl File: configure.in
dnl Top configuration file for autoconf/automake of elan
dnl
dnl Process this file with autoconf to produce a configure script
dnl
dnl CVS ID: "$Id: configure.ac,v 1.4 2011/09/05 19:57:06 marcs Exp $"
dnl------------------------------------------------------------------
AC_INIT(gen/main.c)
AM_INIT_AUTOMAKE(elan,1.11)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_SANITY_CHECK

dnl------------------------------------------------------------------
dnl Check the local build system
dnl------------------------------------------------------------------
AC_CANONICAL_HOST
echo 'Building elan compiler for a' $host 'system'
AC_SUBST(TCPU,$host_cpu)
AC_SUBST(TOS,$host_os)

dnl------------------------------------------------------------------
dnl Check for libtool
dnl------------------------------------------------------------------
AC_PROG_LIBTOOL
AC_SUBST(LIBTOOL_DEPS)

dnl------------------------------------------------------------------
dnl Checks for programs.
dnl------------------------------------------------------------------
AC_PROG_CC
AC_PROG_INSTALL

dnl------------------------------------------------------------------
dnl Check if we need --rpath or some such to link programs,
dnl using libtool. backend.c needs it.
dnl------------------------------------------------------------------
RPATH_OPT=""
AC_MSG_CHECKING([what libtool found for hardcoding paths of runtime libraries])
find_rpath_option() {
    (
    ./libtool --config >configure.tmp$$
    . configure.tmp$$
    rm configure.tmp$$
    libdir="%s"     # use sprintf to fill in $libdir
    eval echo $hardcode_libdir_flag_spec
    )
}

rpath_opt=`find_rpath_option`
if test -n "$rpath_opt"
then
  AC_DEFINE_UNQUOTED(RPATH_OPT, ["$rpath_opt"], [Set to a rpath option, if needed])
  AC_MSG_RESULT([$rpath_opt])
else
  AC_MSG_RESULT([nothing])
fi

dnl------------------------------------------------------------------
dnl Check to locate dcg
dnl------------------------------------------------------------------
AC_PATH_PROG(DCG, dcg,,[$PATH:/usr/local/kun/bin:/usr/local/bin])
if test x$DCG = x; then
   AC_MSG_ERROR([could not locate dcg])
fi
AC_SUBST(DCG,$DCG)
dcg_ipath=`$DCG -il`
echo 'Using' $dcg_ipath 'to locate dcg header files'
AC_SUBST(DCG_IPATH,$dcg_ipath)
dcg_lpath=`$DCG -ll`
echo 'Using' $dcg_lpath 'to locate dcg libraries'
AC_SUBST(DCG_LPATH,$dcg_lpath)

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
   dbg_opt='-O'
fi
AC_SUBST(DBG_OPT,$dbg_opt)

dnl------------------------------------------------------------------
dnl Check whether we install the socket library
dnl------------------------------------------------------------------
AC_ARG_ENABLE(socket,
   AC_HELP_STRING([--enable-socket],[install the socket library]),
   [ case "${enableval}" in
        yes) socket=true ;;
        no)  socket=false ;;
        *)   AC_MSG_ERROR(bad value ${enableval} for --enable-socket) ;;
     esac ], [socket=false] )
if test x$socket = xtrue; then
   echo 'Compiling elan with socket library'
fi
AM_CONDITIONAL(INSTALL_SOCKET, test x$socket = xtrue)

dnl------------------------------------------------------------------
dnl Check whether we want a runtime library with threads
dnl------------------------------------------------------------------
AC_ARG_ENABLE(threads,
   AC_HELP_STRING([--enable-threads],[allow multiple execution threads (experimental) ]),
   [ case "${enableval}" in
        yes) threads=true ;;
        no)  threads=false ;;
        *)   AC_MSG_ERROR(bad value ${enableval} for --enable-threads) ;;
     esac ], [threads=false] )
if test x$threads = xtrue; then
   echo 'Compiling elan with multithreaded support'
   echo -n 'Checking whether to use -pthread or -lpthread... '
   save_LIBS=$LIBS
   LIBS="$save_LIBS -pthread"
   AC_TRY_LINK([#include <pthread.h>],
               [pthread_create(NULL,NULL,NULL,NULL);],
	       [have_pthread=yes], [have_pthread=no])
   LIBS="$save_LIBS -lpthread"
   AC_TRY_LINK([#include <pthread.h>],
               [pthread_create(NULL,NULL,NULL,NULL);],
	       [have_lpthread=yes], [have_lpthread=no])
   if test "$have_pthread" = yes; then
      threads_opt='-DUSE_THREADS -DTHREAD_LIB=\"-pthread\"'
      echo '-pthread'
   else
      if test "$have_lpthread" = yes; then
	 threads_opt='-DUSE_THREADS -DTHREAD_LIB=\"-lpthread\"'
	 echo '-lpthread'
      else
	 echo 'Could not locate usable posix thread library'
         threads_opt=''
      fi
   fi
   LIBS=$save_LIBS
else
   threads_opt=''
fi
AC_SUBST(THREADS_OPT,$threads_opt)
AM_CONDITIONAL(INSTALL_THREADS, test x$threads = xtrue)

dnl------------------------------------------------------------------
dnl Check whether we install the graphics library
dnl To add: check on location of X11
dnl------------------------------------------------------------------
AC_ARG_ENABLE(graphics,
   AC_HELP_STRING([--enable-graphics],[install the graphics library]),
   [ case "${enableval}" in
        yes) graphics=true ;;
        no)  graphics=false ;;
        *)   AC_MSG_ERROR(bad value ${enableval} for --enable-graphics) ;;
     esac ], [graphics=false] )
if test x$graphics = xtrue; then
   echo 'Compiling elan with graphics library'
fi
AM_CONDITIONAL(INSTALL_GRAPHICS, test x$graphics = xtrue)

dnl------------------------------------------------------------------
dnl Checks for paths to X11 libraries
dnl------------------------------------------------------------------
echo 'Checking for location of X11 tree'
AC_PATH_X
X11LDIR=${x_libraries}
X11IDIR=${x_includes}
AC_SUBST(X11IDIR)
AC_SUBST(X11LDIR)

dnl------------------------------------------------------------------
dnl Checks for header files.
dnl------------------------------------------------------------------
AC_HEADER_STDC
AC_CHECK_HEADERS(stdlib.h malloc.h ctype.h sys/types.h sys/time.h sys/param.h)
AC_CHECK_HEADERS(unistd.h limits.h float.h netdb.h)
AC_CHECK_HEADERS(termios.h sys/ioctl.h)
AC_CHECK_HEADERS(sys/resource.h sys/wait.h)

dnl------------------------------------------------------------------
dnl Checks for typedefs, structures, and compiler characteristics.
dnl------------------------------------------------------------------
AC_C_BIGENDIAN
AC_CHECK_SIZEOF(char *)
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
dnl Check whether to compile the runtime system with some curses
dnl library for cursor control or use the plain VT52 cursor control
dnl Some hosts like mingw32 platforms must use curses
dnl------------------------------------------------------------------
case "$host_os" in
   mingw32*) curses=true ;;
   *) AC_ARG_WITH(curses,
	 AC_HELP_STRING([--with-curses],[compile runtime system with curses cursor control]),
	    [ case "${withval}" in
		 yes) curses=true ;;
		 no)  curses=false ;;
	         *)   AC_MSG_ERROR(bad value ${withval} for --with-curses) ;;
	      esac
	    ], [curses=false]) ;;
esac

dnl------------------------------------------------------------------
dnl Check which curses package is available
dnl Prefer ncurses over pd curses over curses
dnl------------------------------------------------------------------
if test "$curses" = true; then
   save_LIBS=$LIBS
   LIBS="$save_LIBS -lncurses"
   echo 'checking for ncurses, pdcurses and curses usability...'
   AC_TRY_LINK([#include <ncurses.h>],
	       [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
	       [have_ncurses=yes], [have_ncurses=no])
   LIBS="$save_LIBS -lpdcurses"
   AC_TRY_LINK([#include <curses.h>],
	       [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
	       [have_pdcurses=yes], [have_pdcurses=no])
   LIBS="$save_LIBS -lcurses"
   AC_TRY_LINK([#include <curses.h>],
	       [chtype a; int b=A_STANDOUT, c=KEY_LEFT; initscr(); ],
	       [have_curses=yes], [have_curses=no])
   if test "$have_ncurses" = yes; then
      curses_opt='-DCURSES_LIB=\"-lncurses\"'
      curses_lib='-lncurses'
      AC_DEFINE([HAVE_NCURSES_H], [], [Use ncurses.h and libncurses])
      echo 'Compiling runtime system with ncurses'
   else
      if test "$have_pdcurses" = yes; then
         curses_opt='-DCURSES_LIB="\"-lpdcurses\""'
	 curses_lib='-lpdcurses'
         AC_DEFINE([HAVE_CURSES_H], [], [Use curses.h and libpdcurses])
         echo 'Compiling runtime system with pdcurses'
      else
	 if test "$have_curses" = yes; then
            curses_opt='-DCURSES_LIB=\"-lcurses\"'
	    curses_lib='-lcurses'
	    AC_DEFINE([HAVE_CURSES_H], [], [Use curses.h and libcurses])
	    echo 'Compiling runtime system with curses'
	 else
 	    AC_MSG_ERROR([can not compile with curses: curses library not available])
         fi
      fi
   fi
   LIBS=$save_LIBS
else
   curses_opt=''
   curses_lib=''
   echo 'Compiling runtime system with plain VT52 cursor control'
fi
AC_SUBST(CURSES_OPT, $curses_opt)
AC_SUBST(CURSES_LIB, $curses_lib)

dnl------------------------------------------------------------------
dnl Check if make macro gets set
dnl------------------------------------------------------------------
AC_PROG_MAKE_SET

dnl------------------------------------------------------------------
dnl Specify which Makefiles should be generated
dnl------------------------------------------------------------------
AC_OUTPUT([
Makefile
manuals/Makefile
manuals/common/Makefile
manuals/language/Makefile
manuals/compiler/Makefile
manuals/man1/Makefile
erts/Makefile
gen/Makefile
prelude/Makefile
esocket/Makefile
ethread/Makefile
egraphics/Makefile
scripts/Makefile
exp/Makefile
])
