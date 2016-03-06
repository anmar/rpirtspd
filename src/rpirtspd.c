/*
 * Copyright (c) 2016, Angel Marin
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *  3. Neither the name of the copyright holder nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>

#include "gstsrc.h"
#include "rpirtspd.h"

gchar *rs_args__bind_address = NULL;
gchar *rs_args__bind_port = NULL;
gchar *rs_args__video_args = NULL;
gint rs_args__video_width = 720;
gint rs_args__video_height = 480;
gint rs_args__video_frm = 25;
gchar *rs_args__audio_args = NULL;
gint rs_args__audio_bitrate = 8000;
gboolean rs_args__out_quiet = FALSE;
gboolean rs_args__out_verbose = FALSE;
gboolean rs_args__mode_test = FALSE;
gboolean rs_args__listen_rtsp = FALSE;

int main (int argc, char *argv[]) {
  gint retc = 0;
  GMainLoop *loop;
  GOptionContext *context;
  GError *error = NULL;
  GOptionEntry entries[] =
  {
    { "bind-address", 0, 0, G_OPTION_ARG_STRING, &rs_args__bind_address, "Local IP address to bind", "0.0.0.0" },
    { "bind-port", 0, 0, G_OPTION_ARG_STRING, &rs_args__bind_port, "Listen port", "8554" },
    { "video-args", 0, 0, G_OPTION_ARG_STRING, &rs_args__video_args, "rpicamsrc video pipeline arguments", "bitrate=..." },
    { "video-width", 0, 0, G_OPTION_ARG_INT, &rs_args__video_width, "Video width", "720" },
    { "video-height", 0, 0, G_OPTION_ARG_INT, &rs_args__video_height, "Video height", "480" },
    { "video-framerate", 0, 0, G_OPTION_ARG_INT, &rs_args__video_frm, "Video frame rate", "25" },
    { "audio-args", 0, 0, G_OPTION_ARG_STRING, &rs_args__audio_args, "alsasrc audio pipeline arguments", "device=..." },
    { "audio-bitrate", 0, 0, G_OPTION_ARG_INT, &rs_args__audio_bitrate, "Audio bitrate", "8000" },
    { "quiet", 'q', 0, G_OPTION_ARG_NONE, &rs_args__out_quiet, "Quiet", NULL },
    { "verbose", 'v', 0, G_OPTION_ARG_NONE, &rs_args__out_verbose, "Verbose", NULL },
    { "rtsp", 0, 0, G_OPTION_ARG_NONE, &rs_args__listen_rtsp, "Start rtsp server", NULL },
    { "test", 0, 0, G_OPTION_ARG_NONE, &rs_args__mode_test, "Test mode (allows pipeline parse errors)", NULL },
    { NULL }
  };

  context = g_option_context_new ("- rPi rtsp stream server ");
  g_option_context_add_main_entries (context, entries, NULL);
  if (!g_option_context_parse (context, &argc, &argv, &error)) {
    g_print ("option parsing failed: %s\n", error->message);
    goto failed;
  }
  retc = server_gstsrc_startgst_init(&argc, &argv);
  if ( retc!=0 ) {
    goto failed;
  }

  if ( rs_args__listen_rtsp ) {
    loop = g_main_loop_new (NULL, FALSE);
    g_main_loop_run (loop);
  }

  return 0;

  /* ERRORS */
failed:
  {
    if ( !rs_args__out_quiet ) {
      g_print ("failed to attach the server\n");
    }
    return retc;
  }
}
