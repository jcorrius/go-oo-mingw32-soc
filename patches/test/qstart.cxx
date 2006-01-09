#include <unistd.h>
#include <gtk/gtk.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <fcntl.h>

#define PIPEDEFAULTPATH		"/tmp"
#define PIPEALTERNATEPATH	"/var/tmp"

static char *
read_path_and_chomp (const char *app)
{
		int len;
		char buffer[PATH_MAX];

		// Readlink on argv[0] to point to the install path
		if ((len = readlink (app, buffer, sizeof(buffer))) < 0)
				return NULL;
		buffer[MIN(len, 4095)] = '\0';

		return g_path_get_dirname (buffer);
}

static char *
get_app_path (const char *app_exec)
{
		if (app_exec[0] != '/') {
				fprintf (stderr, "error: %s must be called with an absolute path\n",
						 app_exec);
				return NULL;
		}
		
		return g_path_get_dirname (app_exec);
}

// OO.o likes to find real paths, unwinding symlinks etc.
static char *
get_real_app_path (const char *app_path)
{
		char *path, *bin_path;

		path = g_strdup (app_path);

		// Hack for use with linkoo: follow soffice.bin
		// if it is a symlink
		bin_path = g_strconcat (app_path, "/soffice.bin", NULL);
		if (g_file_test (bin_path, G_FILE_TEST_IS_SYMLINK)) 
		{
				g_free (path);
				path = read_path_and_chomp (bin_path);
		}
		g_free (bin_path);

		// Nasty - but OO.o likes to call realpath too ...
		char *real_path = (char *)g_malloc (PATH_MAX + 1);
		realpath (path, real_path);
		real_path[PATH_MAX] = '\0';
		g_free (path);

		return real_path;
}

static char *
get_pipe_path (const char *app_path)
{
		char *pipe_path;
		char *real_app_path;
		char *terminated_path;
		const char *base_path;

		real_app_path = get_real_app_path (app_path);
		terminated_path = g_strconcat (real_app_path, "/", NULL);

		if (access(PIPEDEFAULTPATH, R_OK|W_OK) == 0)
				base_path = PIPEDEFAULTPATH;
		else
				base_path = PIPEALTERNATEPATH;

		pipe_path = g_strdup_printf (
				"%s/OSL_PIPE_%u_SingleOfficeIPC_%d-%x", base_path, (int)getuid(),
				SUPD, g_str_hash (terminated_path));

//		fprintf (stderr, "pipe path is '%s' from '%s'\n", pipe_path, real_app_path);

		g_free (terminated_path);
		g_free (real_app_path);

		return pipe_path;
}

static int
connect_pipe (const char *pipe_path)
{
		int fd;
		size_t len;
		struct sockaddr_un addr;

		memset(&addr, 0, sizeof(addr));

		if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
				return fd;
		
		fcntl (fd, F_SETFD, FD_CLOEXEC);

		addr.sun_family = AF_UNIX;
		strncpy(addr.sun_path, pipe_path, sizeof(addr.sun_path));

/* cut / paste from osl's pipe.c */
#if defined(FREEBSD)
		len = SUN_LEN(&addr);
#else
		len = sizeof(addr);
#endif

		if (connect (fd, (struct sockaddr *)&addr, len) < 0)
				return -1;

		return fd;
}

static gboolean
send_args (int fd, int argc, char **argv)
{
		GString *args = g_string_new ("");
		/* FIXME: elide -nologo etc. */
		for (int i = 1; i < argc; i++)
		{
				if (argv[i][0] != '-') {
						/* FIXME: generate absolute paths (file URIs)
						   instead of relative ones */
				}
				g_string_append (args, argv[i]);
				g_string_append_c (args, '|');
		}
		
		fprintf (stderr, "Whack '%s'\n", args->str);
		int len = args->len + 1;

		// FIXME: do a blocking write (?)
		return write (fd, args->str, len) == len;
}

static void
init_gtk (int argc, char **argv)
{
    char *args[3];
	char **argptr = args;
	
	args[0] = argv[0];
	argc = 1;
	for (int i = 0; i < argc - 1; i++)
	{
			if (!strcmp (argv[i], "-display") ||
				!strcmp (argv[i], "--display"))
			{
					args[argc++] = g_strdup ("--display");
					args[argc++] = argv[i+1];
			}
	}
    gtk_init_check( &argc, &argptr );
}

static void
center_window (GtkWindow *window, void *dummy)
{
	GdkRectangle rect;

	gdk_screen_get_monitor_geometry (window->screen, 0, &rect);
	int sx = rect.width;
	int sy = rect.height;
	
	GdkGeometry geom;
	geom.base_width = GTK_WIDGET (window)->allocation.width;
	geom.base_height = GTK_WIDGET (window)->allocation.height;
	geom.min_width = geom.max_width = geom.base_width;
	geom.min_height = geom.max_height = geom.base_height;

	gtk_window_move (window, sx / 2 - geom.min_width / 2,
					 sy / 2 - geom.min_height / 2);

	int hints = GDK_HINT_POS | GDK_HINT_USER_POS |
			GDK_HINT_BASE_SIZE | GDK_HINT_MIN_SIZE | GDK_HINT_MAX_SIZE |
			GDK_HINT_USER_SIZE;

	gtk_window_set_geometry_hints (window, NULL, &geom, (GdkWindowHints) hints);
}

typedef struct {
		gboolean error;
		GtkProgressBar *progress_bar;
}  StatusClosure;

static gboolean
status_pipe_update (GIOChannel * source,
					GIOCondition condition, 
					StatusClosure *cl)
{
	gboolean retval = TRUE;

	fprintf (stderr, "Status pipe update (0x%x)\n", (int) condition);

	if ((int)condition & (G_IO_IN | G_IO_PRI))
	{
			int percent;
			GString *str = g_string_new ("");
			switch (g_io_channel_read_line_string (source, str, NULL, NULL)) {
			case G_IO_STATUS_ERROR:
					cl->error = TRUE;
					// drop through
			case G_IO_STATUS_EOF:
					retval = FALSE;
					break;
			default:
					if (sscanf (str->str, "%d%%", &percent))
							gtk_progress_bar_set_fraction (cl->progress_bar,
														   (float) percent / 100.0);
					break;
			}
			g_string_free (str, TRUE);
	}

	if ((int)condition & (G_IO_HUP | G_IO_NVAL | G_IO_ERR))
	{
			cl->error = TRUE;
			retval = FALSE;
	}

	if (!retval)
			gtk_main_quit ();

	return retval;
}

static void
show_splash (GIOChannel *channel)
{
		GdkPixbuf *pixbuf;
		GtkWidget *contents;
		GtkWidget *vbox;
		StatusClosure cl;

		if (!(pixbuf = gdk_pixbuf_new_from_file ("intro-nld.bmp", NULL)))
				pixbuf = gdk_pixbuf_new_from_file ("intro.bmp", NULL);

		if (pixbuf)
				contents = gtk_image_new_from_pixbuf (pixbuf);
		else
				contents = gtk_label_new ("No splash pixmap");

		// setup closure
		cl.error = FALSE;
		cl.progress_bar = GTK_PROGRESS_BAR (gtk_progress_bar_new ());

		// pack contents & progres bar
		vbox = gtk_vbox_new (FALSE, 0);
		gtk_box_pack_start (GTK_BOX (vbox), contents,
							TRUE, FALSE, 0);
		gtk_progress_bar_set_orientation (
			cl.progress_bar, GTK_PROGRESS_LEFT_TO_RIGHT);
		gtk_progress_bar_set_fraction (cl.progress_bar, 2);
		gtk_box_pack_start (GTK_BOX (vbox), GTK_WIDGET (cl.progress_bar),
							FALSE, FALSE, 0);

		// setup window
		GtkWindow *window;
		window = GTK_WINDOW (gtk_window_new (GTK_WINDOW_TOPLEVEL));
		gtk_window_set_has_frame (window, FALSE);
		gtk_window_set_resizable (window, FALSE);
		gtk_window_set_decorated (window, FALSE);
//		gtk_window_set_icon_name (window, "foo"); FIXME splash WM icon ?

		gtk_container_add (GTK_CONTAINER (window), vbox);
		g_signal_connect (G_OBJECT (window), "realize", 
						  G_CALLBACK (center_window), NULL);
		gtk_widget_show_all (GTK_WIDGET (window));

		// setup status listener ...
		g_io_add_watch (channel, 
						(GIOCondition)(G_IO_IN | G_IO_PRI | G_IO_HUP | G_IO_NVAL | G_IO_ERR),
						(GIOFunc) status_pipe_update, (gpointer) &cl);

		g_warning ("Start main loop...\n");
		gtk_main ();
        
        g_io_channel_shutdown (channel, FALSE, NULL);
        g_io_channel_unref (channel);
}

const char *
detect_no_args (int argc, char **argv)
{
		int i;
		const char *arg_name = NULL;

		for (i = 1; i < argc; i++)
				if (argv[i][0] != '-')
						break;
		// It's necessary to set this eg. -writer for writer.
		if (i == argc)
				return g_getenv ("OOO_EXTRA_ARG");

		return arg_name;
}

typedef struct {
		int pipe_fd;
} ChildClosure;

static void
child_setup (gpointer user_data)
{
		ChildClosure *cl = (ChildClosure *)user_data;
		close (cl->pipe_fd);
}

static GIOChannel *
fork_app (const char *app_path, int argc, char **incoming_argv, int pipe_fd)
{
		int i;
		int status_pipe[2];
		gboolean sync_success;
		GError *error = NULL;
		char *splash_pipe_arg;
		char **argv = g_new (char *, argc + 4);
		ChildClosure *cl = g_new (ChildClosure, 1);
		GIOChannel *output;

		for (i = 0; i < argc; i++)
				argv[i] = incoming_argv[i];

		argv[0] = g_strconcat (app_path, "/soffice", NULL);

		if ((argv[i] = (char *) detect_no_args (argc, argv)))
				argc++;

		/* create pipe */
     	if (pipe (status_pipe) < 0) {
				fprintf (stderr, "no file handles\n");
				return NULL;
		}

        output = g_io_channel_unix_new (status_pipe[0]);
        g_io_channel_set_encoding (output, NULL, NULL);

		splash_pipe_arg = g_strdup_printf ("-splash-pipe=%d", status_pipe[1]);
		fprintf ( stderr, "Pass splash pipe arg: '%s'\n", splash_pipe_arg);
		argv[argc++] = splash_pipe_arg;
		argv[argc++] = NULL; 

		fprintf (stderr, "Fork app '%s' in '%s'\n",
				 argv[0], app_path);
		// Hack for now:
		cl->pipe_fd = pipe_fd;
		sync_success = g_spawn_async (app_path, argv, NULL,
									  G_SPAWN_LEAVE_DESCRIPTORS_OPEN,
									  (GSpawnChildSetupFunc) child_setup,
									  (gpointer) cl,
									  NULL, // &exit_status,
									  &error);

		close (status_pipe[1]);

		if (!sync_success)
				fprintf (stderr, "Error forking '%s/%s': '%s'\n",
						 app_path, "/soffice",
						 error ? error->message : "<no message>");

		g_free (splash_pipe_arg);
		g_free (argv[0]);
		g_free (argv);

		return output;
}

int main (int argc, char **argv)
{
		int fd;
		GIOChannel *channel = NULL;
		gboolean sent_args = FALSE;
		char *app_path, *pipe_path;

		app_path = get_app_path (argv[0]);
		if (!app_path)
		{
				fprintf (stderr, "Pathalogical failure: can't read app link\n");
				return 1;
		}
		chdir (app_path);

		pipe_path = get_pipe_path (app_path);

		if ((fd = connect_pipe (pipe_path)) >= 0)
				sent_args = send_args (fd, argc, argv);
		else
				fprintf (stderr, "Failed to connect to pipe '%s'\n", pipe_path);

		if (!sent_args)
		{
				if (! (channel = fork_app (app_path, argc, argv, fd)))
						return 1;
		}

		init_gtk (argc, argv);
		if (sent_args)
		{
				gdk_notify_startup_complete ();
				return 0;
		}

		show_splash (channel);

		g_free (app_path);

		return 0;
}
