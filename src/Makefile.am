ACLOCAL_AMFLAGS = -I m4

pkgincludedir = $(includedir)/libnsh
pkginclude_HEADERS = nsh.h scalar.h table.h private.h deprecated.h

lib_LTLIBRARIES = libnsh.la
libnsh_la_SOURCES = scalar.c table.c
libnsh_la_LDFLAGS = -version-info 3:0:3
