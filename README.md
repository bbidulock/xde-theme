[xde-theme -- read me first file.  2019-09-05]: # 

xde-theme
===============

Package `xde-theme-1.3` was released under GPLv3 license 2019-09-05.

This is a set of styles for the XDE (_X Desktop Environment_) which
provides a small and consistent set of window manager styles across a
wide range of light-weight window managers.  The purpose is to provide a
consistent visual appearance for the _X Desktop Environment_ across all
supported window managers.


Release
-------

This is the `xde-theme-1.3` package, released 2019-09-05.  This
release, and the latest version, can be obtained from [GitHub][1], using
a command such as:

    $> git clone https://github.com/bbidulock/xde-theme.git

Please see the [NEWS][3] file for release notes and history of user
visible changes for the current version, and the [ChangeLog][4] file for
a more detailed history of implementation changes.  The [TODO][5] file
lists features not yet implemented and other outstanding items.

Please see the [INSTALL][7] file for installation instructions.

When working from `git(1)`, please use this file.  An abbreviated
installation procedure that works for most applications appears below.

This release is published under GPLv3.  Please see the license in the
file [COPYING][9].


Quick Start
-----------

The quickest and easiest way to get `xde-theme` up and running is to run
the following commands:

    $> git clone https://github.com/bbidulock/xde-theme.git
    $> cd xde-theme
    $> ./autogen.sh
    $> ./configure
    $> make
    $> make DESTDIR="$pkgdir" install

This will configure, compile and install `xde-theme` the quickest.  For
those who like to spend the extra 15 seconds reading `./configure
--help`, some compile time options can be turned on and off before the
build.

For general information on GNU's `./configure`, see the file
[INSTALL][7].


Running
-------

Read the manual page after installation:


Issues
------

Report issues on GitHub [here][2].



[1]: https://github.com/bbidulock/xde-theme
[2]: https://github.com/bbidulock/xde-theme/issues
[3]: https://github.com/bbidulock/xde-theme/blob/1.3/NEWS
[4]: https://github.com/bbidulock/xde-theme/blob/1.3/ChangeLog
[5]: https://github.com/bbidulock/xde-theme/blob/1.3/TODO
[6]: https://github.com/bbidulock/xde-theme/blob/1.3/COMPLIANCE
[7]: https://github.com/bbidulock/xde-theme/blob/1.3/INSTALL
[8]: https://github.com/bbidulock/xde-theme/blob/1.3/LICENSE
[9]: https://github.com/bbidulock/xde-theme/blob/1.3/COPYING

[ vim: set ft=markdown sw=4 tw=72 nocin nosi fo+=tcqlorn spell: ]: #
