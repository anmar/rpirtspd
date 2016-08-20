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

#include "gcontrol.h"

#include "rpirtspd.h"

#include "gstsrc.h"

#if HAVE_GIO_UNIX
gboolean gcontrol_incoming_callback( GSocketService *service, GSocketConnection *connection, GObject *source_object, gpointer user_data ) {
  gchar buffer[2048];
  gssize bytes;
  GError *error = NULL;
  GInputStream *istream;
  GOutputStream *ostream;

  istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
  bytes = g_input_stream_read(istream, buffer, sizeof buffer, NULL, &error);
  if ( error!=NULL ) {
    g_warning("Error reading from socket %s: %s", rs_args__control_socket, error->message);
    g_error_free(error);
    return 1;
  }
  if ( bytes==0 ) {
    return FALSE;
  }
  buffer[bytes] = '\0';
  g_debug("gcontrol_incoming_callback[%s]: [%s]\n", rs_args__control_socket, buffer);
  ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
  if ( server_gstsrc_configure(buffer) ) {
    g_output_stream_write(ostream, "ok", 2, NULL, &error);
  } else {
    g_output_stream_write(ostream, "err", 3, NULL, &error);
  }
  if ( error!=NULL ) {
    g_warning("Error writing to socket %s: %s", rs_args__control_socket, error->message);
    g_error_free(error);
    return 1;
  }
  return FALSE;
}
#endif /* HAVE_GIO_UNIX */

gint gcontrol_server_init( int *argc, char **argv[] ) {
#if HAVE_GIO_UNIX
  GError *error = NULL;
  GSocketAddress *address;
  GSocketService *service;

  /* drop existing socket */
  unlink(rs_args__control_socket);

  /* Bind socket */
  service = g_socket_service_new();
  address = g_unix_socket_address_new(rs_args__control_socket);
  g_socket_listener_add_address(G_SOCKET_LISTENER(service), address, G_SOCKET_TYPE_STREAM, G_SOCKET_PROTOCOL_DEFAULT, NULL, NULL, &error);

  if ( error!=NULL ) {
    g_warning("Failed to bind to Unix socket %s: %s", rs_args__control_socket, error->message);
    g_error_free(error);
    return 1;
  }

  /* listen to the 'incoming' signal */
  g_signal_connect(service, "incoming", G_CALLBACK(gcontrol_incoming_callback), NULL);

  /* start the socket service */
  g_socket_service_start(service);
#else
  g_warning("No support for Unix sockets");
#endif /* HAVE_GIO_UNIX */
  return 0;
}

gint gcontrol_client_send() {
#if HAVE_GIO_UNIX
  char buffer[2048];
  gssize bytes;
  GError *error = NULL;
  GSocketAddress *address;
  GSocketConnection *connection;
  GSocket *socket = g_socket_new(G_SOCKET_FAMILY_UNIX, G_SOCKET_TYPE_STREAM, 0, &error);

  if ( error!=NULL ) {
    g_warning("Failed to open Unix socket %s: %s", rs_args__control_socket, error->message);
    g_error_free(error);
    return 1;
  }
  g_socket_set_timeout(socket, 300);

  address = g_unix_socket_address_new(rs_args__control_socket);
  connection = g_socket_connection_factory_create_connection(socket);
  g_socket_connection_connect(connection, address, NULL, &error);
  if ( error!=NULL ) {
    g_warning("Failed to connect to Unix socket %s: %s", rs_args__control_socket, error->message);
    g_error_free(error);
    g_object_unref(connection);
    g_object_unref(address);
    g_object_unref(socket);
    return 1;
  }
  /* use the connection */
  GInputStream * istream = g_io_stream_get_input_stream(G_IO_STREAM(connection));
  GOutputStream * ostream = g_io_stream_get_output_stream(G_IO_STREAM(connection));
  bytes = g_snprintf(buffer, sizeof buffer, "%s", rs_args__video_args);
  /* Send message */
  g_output_stream_write(ostream, buffer, bytes, NULL, &error);

  bytes = g_input_stream_read(istream, buffer, sizeof buffer, NULL, NULL);
  buffer[bytes] = '\0';
  g_print("Response: \"%s\"\n", buffer);
  g_object_unref(connection);
  g_object_unref(address);
  g_object_unref(socket);
#else
  g_warning("No support for Unix sockets");
#endif /* HAVE_GIO_UNIX */
  return 0;
}
