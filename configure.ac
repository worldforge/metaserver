dnl Process this file with autoconf to produce a configure script.
AC_INIT([metaserver], [1.9])
AC_CONFIG_SRCDIR([src/metaserver_main.cpp])
AC_CONFIG_HEADERS([config.h])

AM_INIT_AUTOMAKE([nostdinc dist-bzip2])

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX

dnl Checks for libraries.
AC_CHECK_HEADERS(getopt.h)
AC_CHECK_LIB(gnugetopt, getopt_long)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(syslog.h unistd.h features.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(socket strerror inet_ntop inet_aton)
AC_CHECK_LIB(socket, socket)
AC_CHECK_LIB(nsl, inet_ntop)
AC_CHECK_LIB(resolv, inet_aton)

AC_ARG_ENABLE(debug,[  --enable-debug          creates debugging code [default=no]],
[
  if test x$enableval = xno; then
    ac_use_debug_code="no"
  else
    ac_use_debug_code="yes"
  fi
], [ac_user_debug_code="no"])
if test "$ac_use_debug_code" = "yes"; then
  CPPFLAGS="$CPPFLAGS -g -DDEBUG"
else
  CPPFLAGS="$CPPFLAGS -DNDEBUG"
fi

AC_OUTPUT(Makefile src/Makefile TestServer/Makefile TestClient/Makefile )
