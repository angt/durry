AC_PREREQ([2.60])
AC_INIT([durry], [0.2])
AC_CONFIG_SRCDIR([Makefile.am])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_AUX_DIR(.)

AM_INIT_AUTOMAKE([foreign dist-bzip2])

m4_ifndef([XORG_MACROS_VERSION], [m4_fatal([must install xorg-macros 1.3 or later before running autoconf/autogen])])
XORG_MACROS_VERSION(1.3)
XORG_DEFAULT_OPTIONS

AC_DISABLE_STATIC
AC_PROG_LIBTOOL

AH_TOP([#include "xorg-server.h"])

AC_ARG_WITH(xorg-module-dir, [  --with-xorg-module-dir=DIR ],
                             [ moduledir="$withval" ],
                             [ moduledir="$libdir/xorg/modules" ])
AC_SUBST(moduledir)

PKG_CHECK_MODULES(XORG, xorg-server xproto fontsproto $REQUIRED_MODULES)

DRIVER_NAME=durry
AC_SUBST([DRIVER_NAME])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
