#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

#include "list.h"
#include "log.h"
#include "xmalloc.h"
#include "xoutputd.h"
#include "y.tab.h"

static const char *connection_strings[] = {
	"connected",
	"disconnected",
	"unknown connection"
};

static output_list_t outputs = LIST_INITIALIZER(&outputs);
rule_list_t rules = LIST_INITIALIZER(&rules);

static int rr_event_base;
static int rr_error_base;

static void get_all_outputs(Display *dpy)
{
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);
	XRRScreenResources *screen_resources = XRRGetScreenResources(dpy, root);

	if (screen_resources == NULL)
		fatal("unable to get screen resources");

	for (int i = 0; i < screen_resources->noutput; i++) {
		XRROutputInfo *output_info
			= XRRGetOutputInfo(dpy, screen_resources,
					   screen_resources->outputs[i]);
		if (output_info == NULL)
			fatal("unable to get output info");

		output_t *output = new_output(xstrdup(output_info->name),
					      screen_resources->outputs[i],
					      output_info->connection);
		ADD_TO_LIST(&outputs, output);
		XRRFreeOutputInfo(output_info);
	}
	XRRFreeScreenResources(screen_resources);
}

output_t *lookup_output_by_id(RROutput id)
{
	output_t *output;

	FOREACH(&outputs, output) {
		if (output->id == id)
			return output;
	}
	return NULL;
}

output_t *lookup_output_by_name(const char *name)
{
	output_t *output;

	FOREACH(&outputs, output) {
		if (!strcmp(output->name, name))
			return output;
	}
	return NULL;
}

output_t *new_output(char *name, RROutput id, Connection conn)
{
	output_t *output = xcalloc(1, sizeof(*output));

	output->name = name;
	output->id = id;
	output->conn = conn;
	return output;
}

rule_t *new_rule(output_list_t *_outputs, Connection conn, char *cmd)
{
	rule_t *rule = xcalloc(1, sizeof(*rule));
	output_t *output;

	FOREACH(_outputs, output)
		output->conn = conn;
	rule->outputs = _outputs;
	rule->cmd = cmd;
	return rule;
}

static bool match_rule(const rule_t *rule)
{
	const output_t *routput;

	FOREACH(rule->outputs, routput) {
		const output_t *output = lookup_output_by_id(routput->id);

		if (output == NULL)
			return false;
		if (output->conn != routput->conn)
			return false;
	}
	return true;
}

static const rule_t *find_matching_rule(void)
{
	const rule_t *rule;

	FOREACH (&rules, rule) {
		if (match_rule(rule))
			return rule;
	}
	return NULL;
}

static void execute_matching_rule(void)
{
	const rule_t *rule = find_matching_rule();

	if (rule) {
		log_info("executing `%s`", rule->cmd);
		system(rule->cmd);
	}
}

static void handle_output_change_event(XRROutputChangeNotifyEvent *event)
{
	output_t *output = lookup_output_by_id(event->output);

	if (output == NULL) {
		log_warn("received event on unknown output (%lu)\n",
			 event->output);
		return;
	}

	Connection oldconn = output->conn;
	Connection newconn = event->connection;

	if (oldconn == newconn)
		return;
	log_info("output %s connection %s->%s", output->name,
		 connection_strings[oldconn],
		 connection_strings[newconn]);
	output->conn = newconn;
	execute_matching_rule();
}

__attribute__((noreturn))
static void monitor_output_changes(Display *dpy)
{
	int screen = DefaultScreen(dpy);
	Window root = RootWindow(dpy, screen);

	XRRSelectInput(dpy, root, RROutputChangeNotifyMask);
	for (;;) {
		XEvent event;

		XNextEvent(dpy, &event);
		if (event.type != (rr_event_base + RRNotify_OutputChange)) {
			log_warn("received unknown event");
			continue;
		}
		XRRUpdateConfiguration(&event);
		handle_output_change_event((XRROutputChangeNotifyEvent *)
					   &event);
	}
}

static char *user_configfile_path(void)
{
	const char *home_path = getenv("HOME");
	const char filename[] = ".xoutputdrc";

	if (home_path == NULL)
		fatal("could not find config file: $HOME is not defined");

	char *configfile = xcalloc(PATH_MAX, sizeof(*configfile));

	if (snprintf(configfile, PATH_MAX, "%s/%s", home_path, filename)
	    >= PATH_MAX)
		fatal("could not find config file: path too long");
	return configfile;
}

static void usage(void)
{
	extern const char *__progname;

	fprintf(stderr, "Usage: %s [-hv] [-f config_file]\n", __progname);
}

int main(int argc, char **argv)
{
	int opt;
	char *configfile = NULL;

	while ((opt = getopt(argc, argv, "hvf:")) != -1) {
		switch (opt) {
		case 'f':
			if (configfile != NULL)
				fatal("-f cannot be specified multiple times");
			configfile = xstrdup(optarg);
			break;
		case 'h':
			usage();
			return EXIT_SUCCESS;
		case 'v':
			fputs("xoutputd v" VERSION "\n", stderr);
			return EXIT_SUCCESS;
		default:
			usage();
			return EXIT_FAILURE;
		}
	}

	const char * const display_name = NULL;
	Display *dpy = XOpenDisplay(display_name);

	if (dpy == NULL)
		fatal("unable to open display %s",
		      XDisplayName(display_name));

	if (!XRRQueryExtension(dpy, &rr_event_base, &rr_error_base))
		fatal("randr extension is not supported");

	get_all_outputs(dpy);

	if (configfile == NULL)
		configfile = user_configfile_path();

	extern FILE *yyin;

	yyin = fopen(configfile, "r");
	if (yyin == NULL)
		fatal("%s: %s", configfile, strerror(errno));
	while (!feof(yyin))
		yyparse();
	fclose(yyin);
	free(configfile);

	execute_matching_rule();
	monitor_output_changes(dpy);
	/* never reached */
}
