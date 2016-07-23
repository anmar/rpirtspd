dnl required version of autoconf
AC_PREREQ([2.53])

AC_CONFIG_MACRO_DIR([m4])

AC_INIT([rpirtspd],[1.0.0])

dnl required versions
GST_REQUIRED=1.0.0
GSTPB_REQUIRED=1.0.0
GSTRTSP_REQUIRED=1.0.0
ALSA_REQUIRED=1.0.0

AC_CONFIG_SRCDIR([src/rpirtspd.c])
AC_CONFIG_HEADERS([config.h])

dnl required version of automake
AM_INIT_AUTOMAKE([-Wno-portability 1.11 no-dist-gzip dist-xz tar-ustar subdir-objects]) 

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl Pretty Printing
m4_ifdef([AM_SILENT_RULES],[AM_SILENT_RULES([yes])],
  [AM_DEFAULT_VERBOSITY=1
   AC_SUBST(AM_DEFAULT_VERBOSITY)])

dnl check for tools (compiler etc.)
AC_PROG_CC

dnl required version of libtool
LT_PREREQ([2.2.6])
LT_INIT

dnl give error and exit if we don't have pkgconfig
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed!])
])

# Checks for libraries.
PKG_PROG_PKG_CONFIG([0.24])
PKG_CHECK_MODULES([glib], [glib-2.0 >= 2.28])

AC_ARG_ENABLE(
  [control],
  [AC_HELP_STRING(
    [--enable-control],
    [enable control socket]
  )],
  [enable_gcontrol=${enableval}],
  [enable_gcontrol=no]
)
if test x"$enable_gcontrol" = x"yes" ; then
  PKG_CHECK_MODULES([gio], [gio-2.0 >= 2.28])
  PKG_CHECK_MODULES([gio_unix], [gio-unix-2.0 >= 2.28], [test_have_giounix=yes], [test_have_giounix=no])
  if test x"$test_have_giounix" = x"yes" ; then
    AC_DEFINE(HAVE_GIO_UNIX, 1, "Define for gio-unix")
  else
    AC_DEFINE(HAVE_GIO_UNIX, 0, "Define for gio-unix")
  fi
else
  AC_DEFINE(HAVE_GIO_UNIX, 0, "Define for gio-unix")
fi

PKG_CHECK_MODULES(GST, [
  gstreamer-1.0 >= $GST_REQUIRED
  gstreamer-base-1.0 >= $GST_REQUIRED
  gstreamer-video-1.0 >= $GSTPB_REQUIRED
], [
  AC_SUBST(GST_CFLAGS)
  AC_SUBST(GST_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the GStreamer development
      packages on your system. On debian-based systems these are
      libgstreamer1.0-dev and libgstreamer-plugins-base1.0-dev.
      on RPM-based systems gstreamer1-devel, libgstreamer1.0-devel
      or similar. The minimum version required is $GST_REQUIRED.
  ])
])

AC_ARG_WITH([static-gst-rtsp-server-src], AC_HELP_STRING([--with-static-gst-rtsp-server-src=DIR],
	[path to gst-rtsp-server source directory for static linking]), [path_gstrtspserversrc=${withval}])
if (test -n "${path_gstrtspserversrc}"); then
  GSTRTSP_CFLAGS="-I${path_gstrtspserversrc}"
  GSTRTSP_LIBS="-static ${path_gstrtspserversrc}/gst/rtsp-server/libgstrtspserver-1.0.la -Wl,-Bdynamic"
  AC_SUBST(GSTRTSP_CFLAGS)
  AC_SUBST(GSTRTSP_LIBS)
else
  PKG_CHECK_MODULES(GSTRTSP, [
    gstreamer-rtsp-server-1.0 >= $GSTRTSP_REQUIRED
  ], [
    AC_SUBST(GSTRTSP_CFLAGS)
    AC_SUBST(GSTRTSP_LIBS)
  ], [
    AC_MSG_ERROR([
        You need to install or upgrade the gstreamer-rtsp-server development
        packages on your system. On debian-based systems these are
        libgstrtspserver-1.0-dev on RPM-based systems gstreamer1-rtsp-devel
        or similar. The minimum version required is $GSTRTSP_REQUIRED.
    ])
  ])
fi

PKG_CHECK_MODULES(ALSA, [
  alsa >= $ALSA_REQUIRED
], [
  AC_SUBST(ALSA_CFLAGS)
  AC_SUBST(ALSA_LIBS)
], [
  AC_MSG_ERROR([
      You need to install or upgrade the alsa development
      packages on your system. On debian-based systems these are
      libasound2-dev on RPM-based systems alsa-lib-devel
      or similar. The minimum version required is $ALSA_REQUIRED.
  ])
])

dnl check if compiler understands -Wall (if yes, add -Wall to GST_CFLAGS)
AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_CFLAGS="$GST_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])

# systemd support
AC_ARG_WITH([systemdunitdir], AC_HELP_STRING([--with-systemdunitdir=DIR],
	[path to systemd service directory]), [path_systemdunit=${withval}],
	[path_systemdunit="`$PKG_CONFIG --variable=systemdsystemunitdir systemd`"])
if (test -n "${path_systemdunit}"); then
SYSTEMD_UNITDIR="${path_systemdunit}"
AC_SUBST(SYSTEMD_UNITDIR)
AM_CONDITIONAL(SYSTEMD, test -n "${path_systemdunit}")
fi

AC_CONFIG_FILES([
Makefile
src/Makefile
data/Makefile
data/rpirtspd.service
])
AC_OUTPUT
dnl ==========================================================================
echo "
                    fwupd $VERSION
                  =================

        prefix:                    ${prefix}
        compiler:                  ${CC}
        cflags:                    ${CFLAGS}
        cppflags:                  ${CPPFLAGS}
"
