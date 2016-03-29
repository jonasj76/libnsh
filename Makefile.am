ACLOCAL_AMFLAGS = -I m4

EXAMPLES        =
if ENABLE_EXAMPLES
  EXAMPLES     += examples
endif

SUBDIRS         = src $(EXAMPLES)

EXTRA_DIST      = LICENSE README.md autogen.sh

# Generate MD5 checksum file
MD5 = md5sum
md5-dist:
	@for file in $(DIST_ARCHIVES); do	\
		$(MD5) $$file > $$file.md5;	\
	done

# Target to run when building a release
release: dist md5-dist
	@for file in $(DIST_ARCHIVES); do				\
		printf "$$file    \tDistribution tarball\n";		\
		printf "$$file.md5\t"; cat $$file.md5 | cut -f1 -d' ';	\
	done
