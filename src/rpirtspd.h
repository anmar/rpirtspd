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

#include <glib.h>

#ifndef RS__RTSPD_H__
#define RS__RTSPD_H__

G_BEGIN_DECLS

extern gchar *rs_args__control_socket;
extern gboolean rs_args__control_send;
extern gchar *rs_args__bind_address;
extern gchar *rs_args__bind_port;
extern gchar *rs_args__video_source;
extern gchar *rs_args__video_args;
extern gchar *rs_args__video_profile;
extern gint rs_args__video_width;
extern gint rs_args__video_height;
extern gint rs_args__video_frm;
extern gchar *rs_args__audio_args;
extern gint rs_args__audio_bitrate;
extern gint rs_args__audio_channels;
extern gint rs_args__audio_clockrate;
extern gint rs_args__audio_delay;
extern gboolean rs_args__audio_compress;
extern gboolean rs_args__out_quiet;
extern gboolean rs_args__out_verbose;
extern gboolean rs_args__mode_test;
extern gboolean rs_args__listen_rtsp;
#if HAVE_GIO_UNIX
extern gboolean rs_args__listen_control;
#endif

G_END_DECLS

#endif /* RS__RTSPD_H__ */
