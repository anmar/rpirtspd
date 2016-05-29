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

#include "rpirtspd.h"

#include "gstsrc.h"

#include "alsasrc.h"

/* this timeout is periodically run to clean up the expired sessions from the
 * pool. This needs to be run explicitly currently but might be done
 * automatically as part of the mainloop. */
static gboolean session_pool_cleanup (GstRTSPServer * server, gboolean ignored) {
  GstRTSPSessionPool *pool;

  pool = gst_rtsp_server_get_session_pool (server);
  gst_rtsp_session_pool_cleanup (pool);
  g_object_unref (pool);

  return TRUE;
}

static gboolean stream_test( const gchar *pipeline_description ) {
  GError *error = NULL;
  gst_parse_launch (pipeline_description, &error);
  if ( error ) {
    if ( !rs_args__out_quiet ) {
      g_print ("Pipeline failure [%s]\n", pipeline_description);
    }
    g_error_free(error);
    return FALSE;
  }
  return TRUE;
}

static gchar * stream_pipeline( gchar *pipeline_video, gchar *pipeline_audio ) {
  gchar *pipeline = g_strdup_printf("( %s %s )", pipeline_video ?pipeline_video : "", pipeline_audio ? pipeline_audio : "");
  if ( pipeline_video ) {
    g_free(pipeline_video);
  }
  if ( pipeline_audio ) {
    g_free(pipeline_audio);
  }
  return pipeline;
}

static gchar * stream_pipeline_video( void ) {
  gchar *pipeline = g_strdup_printf("rpicamsrc %s !"
    " video/x-h264,width=%d,height=%d,framerate=%d/1 ! "
    "h264parse ! rtph264pay config-interval=5 pt=96 name=pay0",
      rs_args__video_args ? rs_args__video_args : "bitrate=1000000",
      rs_args__video_width, rs_args__video_height, rs_args__video_frm );
  return pipeline;
}
static gchar * stream_pipeline_audio_device_first( void ) {
  gchar *device = audio_alsasrc_device_first();
  if ( !device ) {
    return NULL;
  }
  gchar *pipeline = g_strdup_printf("device=plughw:%s", device);
  g_free(device);
  return pipeline;
  
}
static gchar * stream_pipeline_audio( void ) {
  gchar *device = stream_pipeline_audio_device_first();
  if ( !device && !rs_args__audio_args ) {
    return NULL;
  }
  gchar *pipeline =  g_strdup_printf("alsasrc %s !"
    " queue ! audio/x-raw,rate=%d ! alawenc ! rtppcmapay name=pay1 pt=97",
      rs_args__audio_args ? rs_args__audio_args : device, rs_args__audio_bitrate );
  if ( device ) {
    g_free(device);
  }
  return pipeline;
}

static void set_stream_main( GstRTSPMountPoints *mounts ) {
  gchar *pipeline;
  GstRTSPMediaFactory *factory;
  factory = gst_rtsp_media_factory_new ();
  gst_rtsp_media_factory_set_shared(factory, TRUE);
  pipeline = stream_pipeline(stream_pipeline_video(), stream_pipeline_audio());
  if ( rs_args__out_verbose ) {
    g_print("set_stream_main: Pipeline [%s]\n", pipeline);
  }
  gst_rtsp_media_factory_set_launch (factory, pipeline);
  gst_rtsp_mount_points_add_factory (mounts, "/main", factory);
  if ( !rs_args__out_quiet ) {
    g_print ("[rtsp://127.0.0.1:8554/main] Main stream (audio+video)\n");
  }
}

static void set_stream_test( GstRTSPMountPoints *mounts ) {
  GstRTSPMediaFactory *factory;
  /* make a media factory for a test stream. The default media factory can use
   * gst-launch syntax to create pipelines.
   * any launch line works as long as it contains elements named pay%d. Each
   * element with pay%d names will be a stream */
  factory = gst_rtsp_media_factory_new ();
  /* Media from a factory can be shared by setting the shared flag with
   * gst_rtsp_media_factory_set_shared(). When a factory is shared,
   * gst_rtsp_media_factory_construct() will return the same GstRTSPMedia when
   * the url matches. */
  gst_rtsp_media_factory_set_shared(factory, TRUE);
  /* any launch line works as long as it contains elements named pay%d. Each
   * element with pay%d names will be a stream */
  gst_rtsp_media_factory_set_launch (factory, "( "
      "videotestsrc ! video/x-raw,width=352,height=288,framerate=15/1 ! "
      "x264enc ! rtph264pay name=pay0 pt=96 "
      "audiotestsrc ! audio/x-raw,rate=8000 ! "
      "alawenc ! rtppcmapay name=pay1 pt=97 " ")");
  /* attach the factory to the url */
  gst_rtsp_mount_points_add_factory (mounts, "/test", factory);
  if ( !rs_args__out_quiet ) {
    g_print ("[rtsp://127.0.0.1:8554/test] Test stream\n");
  }
  return;
}

static void set_stream_video( GstRTSPMountPoints *mounts ) {
  gchar *pipeline;
  GstRTSPMediaFactory *factory;
  pipeline = stream_pipeline(stream_pipeline_video(), NULL);
  if ( pipeline==NULL ) {
    return;
  }
  if ( rs_args__out_verbose ) {
    g_print("set_stream_video: Pipeline [%s]\n", pipeline);
  }
  factory = gst_rtsp_media_factory_new ();
  gst_rtsp_media_factory_set_shared(factory, TRUE);
  gst_rtsp_media_factory_set_launch (factory, pipeline);
  gst_rtsp_mount_points_add_factory (mounts, "/video", factory);
  if ( !rs_args__out_quiet ) {
    g_print ("[rtsp://127.0.0.1:8554/video] Video only stream\n");
  }
  return;
}

gint server_gstsrc_startgst_init (int *argc, char **argv[]) {
  GstRTSPServer *server;
  GstRTSPMountPoints *mounts;

  gst_init (argc, argv);

  if ( !stream_test("(rpicamsrc)") || !stream_test("(alsasrc)") ) {
    if ( !rs_args__mode_test ) {
      goto failed;
    }
  }

  /* create a server instance */
  server = gst_rtsp_server_new ();

  /* get the mount points for this server, every server has a default object
   * that be used to map uri mount points to media factories */
  mounts = gst_rtsp_server_get_mount_points (server);

  /* Test stream (video+audio) */
  set_stream_test(mounts);

  /* Main stream (video+audio) */
  set_stream_main(mounts);

  /* Video stream */
  set_stream_video(mounts);

  /* don't need the ref to the mapper anymore */
  g_object_unref (mounts);

  if ( rs_args__listen_rtsp ) {
    if ( rs_args__bind_address ) {
      gst_rtsp_server_set_address(server, rs_args__bind_address);
    }
    if ( rs_args__bind_port ) {
      gst_rtsp_server_set_service(server, rs_args__bind_port);
    }
    /* attach the server to the default maincontext */
    if (gst_rtsp_server_attach (server, NULL) == 0)
      goto failed;

    /* add a timeout for the session cleanup */
    g_timeout_add_seconds (2, (GSourceFunc) session_pool_cleanup, server);

    if ( !rs_args__out_quiet ) {
      g_print ("Server streams ready for clients "
        "[%s][%s]\n", gst_rtsp_server_get_address(server), gst_rtsp_server_get_service(server));
    }
  }
  return 0;
  /* ERRORS */
failed:
  {
    if ( !rs_args__out_quiet ) {
      g_print ("failed to start rtsp server\n");
    }
    return 1;
  }
}
