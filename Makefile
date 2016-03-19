# libnsh -- Net-SNMP helper C-library
#
# Copyright (c) 2016  Jonas Johansson <jonasj76@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

NAME        = libnsh
MAJOR       = 0
MINOR       = 1
VERSION     = $(MAJOR).$(MINOR)

PKG         = $(NAME)-$(VERSION)
ARCHIVE     = $(PKG).tar.gz

CFLAGS     += -fPIC
CPPFLAGS   += -W -Wall
LDFLAGS    +=
ARFLAGS    +=

HEADERS     = scalar.h table.h priv.h
OBJS        = scalar.o table.o
DEPS       := $(OBJS:.o=.d)

LIBNAME     = $(NAME)
SOLIB       = $(LIBNAME).so.$(VERSION)
STATICLIB   = $(LIBNAME).a
TARGET      = $(STATICLIB) $(SOLIB)

include rules.mk

$(OBJS): Makefile

$(SOLIB): Makefile $(OBJS)
	$(PRINT) "  LINK    %s\n" $@
	$(Q)$(CC) $(LDFLAGS) -shared -Wl,-soname,$(LIBNAME).so.$(MAJOR) -o $@ $(OBJS)

$(STATICLIB): Makefile $(OBJS)
	$(Q)$(PRINT) "  ARCHIVE $@\n"
	$(Q)$(AR) $(ARFLAGS) $@ $(OBJS) 2>/dev/null

install-exec: all
	$(Q)$(PRINT) "  INSTALL $(DESTDIR)$(libdir)/$(SOLIB)\n"
	$(Q)$(INSTALL) -d $(DESTDIR)$(libdir)
	$(Q)$(STRIPINST) $(SOLIB) $(DESTDIR)$(libdir)/$(SOLIB)
	$(Q)ln -sf $(SOLIB) $(DESTDIR)$(libdir)/$(LIBNAME).so
	$(Q)ln -sf $(SOLIB) $(DESTDIR)$(libdir)/$(LIBNAME).so.$(MAJOR)

install-dev:
	$(Q)$(INSTALL) -d $(DESTDIR)$(incdir)/$(LIBNAME)
	$(Q)for file in $(HEADERS); do	                                		\
		printf "  INSTALL $(DESTDIR)$(incdir)/$(LIBNAME)/$$file\n";		\
		$(INSTALL) -m 0644 $$file $(DESTDIR)$(incdir)/$(LIBNAME)/$$file;	\
	done
	$(PRINT) "  INSTALL $(DESTDIR)$(libdir)/$(STATICLIB)\n"
	$(Q)$(INSTALL) -d $(DESTDIR)$(libdir)
	$(Q)$(INSTALL) $(STATICLIB) $(DESTDIR)$(libdir)/$(STATICLIB)

install: install-exec install-dev

dist:
	$(Q)echo "Building .gz tarball of $(PKG) in parent dir..."
	git archive --format=tar --prefix=$(PKG)/ v$(VERSION) | gzip >../$(ARCHIVE)
	$(Q)(cd ..; md5sum $(ARCHIVE) | tee $(ARCHIVE).md5)

# Include automatically generated rules
-include $(DEPS)
