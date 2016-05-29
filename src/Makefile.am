
bin_PROGRAMS = rpirtspd

AM_CPPFLAGS = -I$(top_srcdir) -I$(srcdir)

rpirtspd_SOURCES = \
	gstsrc.c \
	alsasrc.c \
	gcontrol.c \
	rpirtspd.c

rpirtspd_CFLAGS = \
	$(glib_CFLAGS) \
	$(gio_CFLAGS) \
	$(gio_unix_CFLAGS) \
	$(GST_CFLAGS) \
	$(GSTRTSP_CFLAGS) \
	$(ALSA_CFLAGS)

rpirtspd_LDFLAGS = \
	$(glib_LIBS) \
	$(gio_LIBS) \
	$(gio_unix_LIBS) \
	$(GSTRTSP_LIBS) \
	$(GST_LIBS) \
	$(ALSA_LIBS)

CLEANFILES = $(BUILT_SOURCES)
