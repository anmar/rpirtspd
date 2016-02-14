
bin_PROGRAMS = rpirtspd

AM_CPPFLAGS = -I$(top_srcdir) -I$(srcdir)

rpirtspd_SOURCES = \
	gstsrc.c \
	alsasrc.c \
	rpirtspd.c

rpirtspd_CFLAGS = \
	$(GST_CFLAGS) \
	$(GSTRTSP_CFLAGS) \
	$(ALSA_CFLAGS)

rpirtspd_LDFLAGS = \
	$(GSTRTSP_LIBS) \
	$(GST_LIBS) \
	$(ALSA_LIBS)

CLEANFILES = $(BUILT_SOURCES)
