#!/bin/sh

NAME=libnsh
MAJOR=0
MINOR=1

VERSION=$MAJOR.$MINOR
PKG=$NAME-$VERSION
ARCHIVE=$PKG.tar.gz

echo "Building .gz tarball of $PKG in parent dir..."
git archive --format=tar --prefix=$PKG/ v$VERSION | gzip >../$ARCHIVE
(cd ..; md5sum $ARCHIVE | tee $ARCHIVE.md5)
