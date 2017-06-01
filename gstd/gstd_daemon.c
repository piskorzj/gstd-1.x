/*
 * GStreamer Daemon - Gst Launch under steroids
 * Copyright (c) 2015-2017 Ridgerun, LLC (http://www.ridgerun.com)
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
 */

/* Code based on testd.c found in 
 * http://0pointer.de/lennart/projects/libdaemon/reference/html/testd_8c-example.html
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstd_daemon.h"
#include "gstd_log.h"

#include <stdlib.h>
#include <errno.h>

#include <libdaemon/dlog.h>
#include <libdaemon/dfork.h>
#include <libdaemon/dsignal.h>
#include <libdaemon/dpid.h>
#include <libdaemon/dexec.h>

static gboolean gstd_daemon_start_parent ();
static gboolean gstd_daemon_start_child ();
static const gchar *gstd_daemon_pid ();

static gboolean _initialized = FALSE;

void
gstd_daemon_init (gint argc, gchar * argv[])
{
  const gchar *process_name;

  g_return_if_fail (argv);

  if (_initialized)
    return;

  /* Sanitize the process name to use it as PID identification */
  process_name = daemon_ident_from_argv0 (argv[0]);

  /* Use the process name as the identification prefix for the 
     pid file */
  daemon_pid_file_ident = process_name;

  /* Override the default pid file location to /tmp/ to avoid the need
     of root privileges */
  daemon_pid_file_proc = gstd_daemon_pid;

  _initialized = TRUE;
}

gboolean
gstd_daemon_start (gboolean * parent)
{
  pid_t pid;
  gboolean ret = FALSE;

  g_return_val_if_fail (_initialized, FALSE);
  g_return_val_if_fail (parent, FALSE);

  /* Check that the daemon is not rung twice a the same time */
  pid = daemon_pid_file_is_running ();
  if (pid >= 0) {
    GST_ERROR ("Daemon already running on PID file %u", pid);
    goto out;
  }

  /* Prepare for return value passing from the initialization
     procedure of the daemon process */
  if (daemon_retval_init () < 0) {
    GST_ERROR ("Failed to create pipe.");
    goto out;
  }

  pid = daemon_fork ();

  /* Negative PID means fork failure */
  if (pid < 0) {
    daemon_retval_done ();
    goto out;
  }
  /* Positive PID is for parent flow */
  if (pid > 0) {
    ret = gstd_daemon_start_parent ();
    *parent = TRUE;
  } else {
    ret = gstd_daemon_start_child ();
    *parent = FALSE;
  }

out:
  {
    return ret;
  }
}

static gboolean
gstd_daemon_start_parent ()
{
  gint iret;
  gboolean ret = FALSE;
  guint timeout = 20;

  g_return_val_if_fail (_initialized, FALSE);

  /* Wait for 20 seconds for the return value passed from the daemon
     process */
  iret = daemon_retval_wait (timeout);
  if (iret < 0) {
    GST_ERROR ("Could not recieve return value from daemon process: %s",
        g_strerror (errno));
  } else if (iret > 0) {
    GST_ERROR ("Child process ended unexpectedly with code: %d", ret);
  } else {
    ret = TRUE;
  }

  return ret;
}

static gboolean
gstd_daemon_start_child ()
{
  gint retval = 0;
  gboolean ret;

  GST_INFO ("Detached form parent process");

  /* Create the PID file */
  if (daemon_pid_file_create () < 0) {
    GST_ERROR ("Could not create PID file: %s (%s).", gstd_daemon_pid (),
        g_strerror (errno));
    goto error;
  }

  GST_INFO ("Daemon successfully started");
  retval = 0;
  ret = TRUE;

  goto out;

error:
  {
    ret = FALSE;
    retval = 1;
  }
out:
  {
    daemon_retval_send (retval);
    return ret;
  }
}

gboolean
gstd_daemon_stop ()
{
  gboolean ret = FALSE;
  guint timeout = 5;

  /* Check if the new function daemon_pid_file_kill_wait() is
     available, if it is, use it. */
  ret = daemon_pid_file_kill_wait (SIGTERM, timeout);
  if (ret < 0) {
    GST_ERROR ("No running Gstd found");
    ret = FALSE;
  } else {
    ret = TRUE;
  }

  /* Cleanup the PID file */
  daemon_pid_file_remove ();

  return ret;
}

static const gchar *
gstd_daemon_pid ()
{
  static gchar *fn = NULL;

  if (fn) {
    g_free (fn);
  }

  fn = g_strdup_printf (GSTD_RUN_STATE_DIR "%s.pid",
      daemon_pid_file_ident ? daemon_pid_file_ident : "gstd");

  return fn;

}