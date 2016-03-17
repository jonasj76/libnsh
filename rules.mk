.PHONY: all clean distclean

# Figure out root of library, unless used as submodule
ROOTDIR    ?= $(shell pwd)

CC         ?= $(CROSS)gcc
AR         ?= $(CROSS)ar
STRIP      ?= $(CROSS)strip
INSTALL    := install --backup=off
STRIPINST  := $(INSTALL) -s --strip-program=$(STRIP) -m 0755

CPPFLAGS   += -D_GNU_SOURCE
ARFLAGS     = crus

# Default to silent build, use V=1 to get verbose mode
ifeq ($V,1)
Q           =
PRINT       = @true
REDIRECT    =
MAKEFLAGS   =
VERBOSE     = -v
else
Q           = @
PRINT       = @printf
REDIRECT    = >/dev/null
MAKEFLAGS   = --no-print-directory --silent
VERBOSE     =
endif

# Default install paths
prefix     ?= /usr/local
libdir     ?= $(prefix)/lib
incdir     ?= $(prefix)/include

# Pretty printing and GCC -M for auto dep files
%.o: %.c
	$(PRINT) "  CC      $(subst $(ROOTDIR)/,,$(ROOTDIR)/)$@\n"
	$(Q)$(CC) $(CFLAGS) $(CPPFLAGS) -c -MMD -MP -o $@ $<

# Default build rules
all: $(TARGET)

clean:
	-$(Q)$(RM) $(OBJS) $(DEPS) $(TARGET)

distclean: clean
	-$(Q)$(RM) *.o *.a *.so* *.~*
