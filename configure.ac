AC_PREREQ([2.61])
AC_INIT([libnsh], [0.2], [https://github.com/jonasj76/libnsh/issues])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_CONFIG_MACRO_DIR([m4])

AC_PROG_CC
AM_PROG_AR
LT_INIT

# Optional building of examples
AC_ARG_ENABLE([examples],
	[AC_HELP_STRING([--enable-examples], [build examples in examples])],
	[enable_examples=yes], [])
AM_CONDITIONAL([ENABLE_EXAMPLES], [test "$enable_examples" = yes])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile examples/Makefile])

AC_OUTPUT
