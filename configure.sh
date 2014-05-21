#!/bin/bash

# need --enable-maintainer-mode to be able to run in place
#      must be disabled to build an installable package

# *FLAGS are what Arch Linux makepkg uses with the exception
#      that -Wall -Werror is added

./configure \
	--enable-maintainer-mode \
	--prefix=/usr \
	--sysconfdir=/etc \
	CPPFLAGS="-D_FORTIFY_SOURCE" \
	CFLAGS="-Wall -Werror -march=i686 -mtune=generic -O2 -pipe -fstack-protector-strong --param=ssp-buffer-size=4" \
	LDFLAGS="-Wl,-O1,--sort-common,--as-needed,-z,relro"
