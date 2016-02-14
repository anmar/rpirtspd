#!/bin/sh
# you can either set the environment variables AUTOCONF, AUTOHEADER, AUTOMAKE,
# ACLOCAL, AUTOPOINT and/or LIBTOOLIZE to the right versions, or leave them
# unset and get the defaults

autoreconf --verbose --force --install -I m4 || {
 echo 'autogen.sh failed';
 exit 1;
}
if test -z "$NOCONFIGURE"; then
 ./configure "$@" || {
  echo 'configure failed';
  exit 1;
 }
 echo
 echo "Now type 'make' to compile."
 echo
fi

exit 0
