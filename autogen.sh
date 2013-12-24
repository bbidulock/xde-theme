#!/bin/bash

VERSION=

if [ -z "$VERSION" ]; then
	VERSION='1.1'
	if [ -x "`which git 2>/dev/null`" -a -d .git ]; then
		VERSION=$(git describe|sed 's,[-_],.,g;s,\.g.*$,,')
		(
		   echo -e "# created with git log --stat=76 | fmt -sct -w80\n"
		   git log --stat=76 | fmt -sct -w80
		)>ChangeLog
	fi
fi

sed -e "s:[[]xde-styles[]],[[][^]]*[]]:[xde-styles],[$VERSION]:
	s:AC_REVISION([[][^]]*[]]):AC_REVISION([$VERSION]):" configure.template >configure.ac

autoreconf -fiv
