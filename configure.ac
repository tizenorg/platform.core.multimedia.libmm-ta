#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT([libmm-ta], [0.1])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])
AC_CONFIG_SRCDIR([src/mm_ta.h])
AC_CONFIG_HEADER([config.h])

AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
m4_ifdef([AM_PROG_AR], [AM_PROG_AR])
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL

# Checks for libraries.

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([malloc.h string.h sys/time.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_HEADER_TIME

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([gethostname gettimeofday memset uname])

#AM_CONDITIONAL([ISPROTECTOR_VODA_SDK], [test "x$DISTRO" = "xvodafone-sdk" && test "x$MACHINE" = "xprotector"])

AC_CONFIG_FILES([Makefile
                 src/Makefile
		mm-ta.pc
])
AC_OUTPUT
