#!/bin/bash

VERSION=

if [ -z "$VERSION" ]; then
	VERSION='1.1'
	if [ -x "`which git 2>/dev/null`" -a -d .git ]; then
		VERSION=$(git describe|sed 's,[-_],.,g;s,\.g.*$,,')
	fi
fi

sed -i -e "s:[[]xde-styles[]],[[][^]]*[]]:[xde-styles],[$VERSION]:
	   s:AC_REVISION([[][^]]*[]]):AC_REVISION([$VERSION]):" configure.ac

autoreconf -fiv
