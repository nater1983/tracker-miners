/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/*
 * Copyright (C) 2008, Nokia
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 */

#include "config.h"

#include <glib.h>
#include <glib/gprintf.h>

#include <gio/gio.h>

#ifdef HAVE_GDKPIXBUF
#include <gdk-pixbuf/gdk-pixbuf.h>
#endif

#include <dbus/dbus-glib-bindings.h>

#define ALBUMARTER_SERVICE      "com.nokia.albumart"
#define ALBUMARTER_PATH         "/com/nokia/albumart/Requester"
#define ALBUMARTER_INTERFACE    "com.nokia.albumart.Requester"


#include "tracker-albumart.h"

static gboolean 
tracker_heuristic_albumart (const gchar *artist,  const gchar *album, const gchar *filename)
{
	// TODO: implement and return TRUE if something was found and copied 
	return FALSE;
}



static DBusGProxy*
tracker_dbus_get_albumart_requester (void)
{
	static DBusGProxy *albart_proxy = NULL;

	if (!albart_proxy) {
		GError          *error = NULL;
		DBusGConnection *connection = dbus_g_bus_get (DBUS_BUS_SESSION, &error);

		if (!error) {
			albart_proxy = dbus_g_proxy_new_for_name (connection,
								  ALBUMARTER_SERVICE,
								  ALBUMARTER_PATH,
								  ALBUMARTER_INTERFACE);
		} else
			g_error_free (error);
	}

	return albart_proxy;
}

static void
get_file_albumart_queue_cb (DBusGProxy     *proxy,
			    DBusGProxyCall *call,
			    gpointer	     user_data)
{
	GError *error = NULL;
	guint	handle;

	/* FIXME: What is the point of this? */
	dbus_g_proxy_end_call (proxy, call, &error,
			       G_TYPE_UINT, &handle,
			       G_TYPE_INVALID);

	if (error) {
		g_warning (error->message);
		g_error_free (error);
	}
}

static void
tracker_get_albumart_path (const gchar *a, const gchar *b, const gchar *prefix, gchar **path)
{
	gchar *art_filename, *str;
	gchar *dir = NULL;
	gchar *_tmp14, *_tmp13, *down;

	if (!prefix)
		prefix = "album";

	if (!a && !b) {
		*path = NULL;
		return;
	}

	if (!a)
		a = "";

	if (!b)
		b = "";

	down = g_utf8_strdown (_tmp14 = (g_strconcat ((_tmp13 = g_strconcat (a, " ", NULL)), b, NULL)),-1);

	g_free (_tmp14);
	g_free (_tmp13);

	dir = g_build_filename (g_get_user_cache_dir (), "media-art", NULL);

	*path = NULL;

	if(!g_file_test (dir, G_FILE_TEST_EXISTS))
		g_mkdir_with_parents (dir, 0770);

	str = g_compute_checksum_for_string (G_CHECKSUM_MD5, down, -1);

	art_filename = g_strdup_printf ("%s-%s.jpeg", prefix, str);

	*path = g_build_filename (dir, art_filename, NULL);

	g_free (str);
	g_free (art_filename);
	g_free (down);
	g_free (dir);
}

#ifdef HAVE_GDKPIXBUF

static gboolean
tracker_save_albumart (const unsigned char *buffer,
		       size_t               len,
		       const gchar         *artist, 
		       const gchar         *album,
		       const gchar         *uri)
{
	GdkPixbufLoader *loader;
	GdkPixbuf       *pixbuf = NULL;
	gchar           *filename;
	GError          *error = NULL;

	g_type_init ();

	if (!artist && !album) {
		g_warning ("No identification data for embedded image");
		return FALSE;
	}

	tracker_get_albumart_path (artist, album, "album", &filename);

	loader = gdk_pixbuf_loader_new ();

	if (!gdk_pixbuf_loader_write (loader, buffer, len, &error)) {
		g_warning ("%s\n", error->message);
		g_error_free (error);

		gdk_pixbuf_loader_close (loader, NULL);
		g_free (filename);
		return FALSE;
	}

	pixbuf = gdk_pixbuf_loader_get_pixbuf (loader);

	if (!gdk_pixbuf_save (pixbuf, filename, "jpeg", &error, NULL)) {
		g_warning ("%s\n", error->message);
		g_error_free (error);

		g_free (filename);
		g_object_unref (pixbuf);

		gdk_pixbuf_loader_close (loader, NULL);
		return FALSE;
	}

	g_free (filename);
	g_object_unref (pixbuf);

	if (!gdk_pixbuf_loader_close (loader, &error)) {
		g_warning ("%s\n", error->message);
		g_error_free (error);
		return FALSE;
	}

	return TRUE;
}

#endif

gboolean
tracker_process_albumart (const unsigned char *buffer,
                          size_t               len,
                          const gchar         *artist,
                          const gchar         *album,
                          const gchar         *filename)
{
	gchar *art_path;
	gboolean retval = TRUE;

	tracker_get_albumart_path (artist, album, "album", &art_path);

	if (!g_file_test (art_path, G_FILE_TEST_EXISTS)) {

#ifdef HAVE_GDKPIXBUF

		if (buffer && len) {
			retval = tracker_save_albumart (buffer, len,
						       artist,
						       album,
						       filename);

		} else {
#endif
			if (!tracker_heuristic_albumart (artist, album, filename)) {

				dbus_g_proxy_begin_call (tracker_dbus_get_albumart_requester (),
					 "Queue",
					 get_file_albumart_queue_cb,
					 NULL, NULL,
					 G_TYPE_STRING, artist,
					 G_TYPE_STRING, album,
					 G_TYPE_STRING, "album",
					 G_TYPE_UINT, 0,
					 G_TYPE_INVALID);
			}
#ifdef HAVE_GDKPIXBUF

		}

#endif
	}

	g_free (art_path);

	return retval;
}
