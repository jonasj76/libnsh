AM_CFLAGS       = -I$(top_srcdir)/src
AM_LDFLAGS      = -L$(top_srcdir)/src
LIBS            = -lnetsnmp -lnetsnmpagent -lnetsnmpmibs
LDADD           = $(top_srcdir)/src/libnsh.la

scalar_const_SOURCES    = scalar_const.c agent.c
scalar_ro_SOURCES       = scalar_ro.c agent.c
scalar_group_ro_SOURCES = scalar_group_ro.c agent.c
scalar_str_ro_SOURCES   = scalar_str_ro.c agent.c
table_SOURCES           = table.c agent.c

noinst_PROGRAMS = scalar_const scalar_group_ro scalar_ro scalar_str_ro table
