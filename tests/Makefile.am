TESTS      = check_register
AM_CFLAGS  = -I$(top_srcdir)/src @CHECK_CFLAGS@
AM_LDFLAGS = -L$(top_srcdir)/src
LIBS       = -lnetsnmp -lnetsnmpagent -lnetsnmpmibs
LDADD      = $(top_srcdir)/src/libnsh.la @CHECK_LIBS@

if HAVE_CHECK

check_register_SOURCES = check_register.c

check_PROGRAMS = $(TESTS)

endif
