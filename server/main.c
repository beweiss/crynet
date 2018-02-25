#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <error.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include "crynet.h"

int server;
int logfile;

int main(int argc, char **argv)
{
	struct user_data *arg;
	int client;
	pid_t pid;

	init_server();

	while(1)
	{
		if ((client = accept_client(server)) == -1)
		{
			errlog(FAIL_SYS, "main: accept");
			continue;
		}
		if ((pid = fork()) == -1)
		{
			errlog(FAIL_SYS, "main: fork");
			continue;
		}
		if (pid == 0)
		{
			if (!(arg = fill_user_data(client)))
			{
				errlog(0, "main: read_user_data");
				goto close_client;
			}
			if (client_login(arg) == -1)
			{
				errlog(0, "main: client_login");
				goto close_client;
			}
			process_command(arg);
			logout_user(arg); //doesn't include close(2)....
close_client:		close(client);
			del_user_data(arg);
			exit(0);
		}
		close(client);
	}
}

void init_server()
{
	DIR *dir;
	struct sigaction act;

	if ((logfile = open("./log", O_WRONLY | O_CREAT | O_APPEND, 0600)) == -1) error_fail(FAIL_SYS, "init_server: open");
	server = start_server();
	if (!(dir = opendir("./profiles")))
	{
		errlog(FAIL_SYS, "init_server: opendir");
		if (mkdir("./profiles", 0600) == -1) error_fail(FAIL_SYS, "init_server: mkdir");
	}
	closedir(dir);
	act.sa_handler = sig_handler;
	if (sigemptyset(&(act.sa_mask)) == -1) error_fail(FAIL_SYS, "init_server: sigemptyset");
	act.sa_flags = 0;
	if (sigaction(SIGINT, &act, NULL) == -1
	    || sigaction(SIGTERM, &act, NULL) == -1
	    || sigaction(SIGABRT, &act, NULL) == -1
	    || sigaction(SIGQUIT, &act, NULL) == -1) error_fail(FAIL_SYS, "init_server: sigaction");
	errlog(0, "Server have been started!");
	return;
}

void sig_handler(int signum)
{
	errlog(0, "got signal : %d (Program was terminated)", signum);
	close(server);
	close(logfile);
	exit(1);
}

void errlog(int code, char *fmt, ...)
{
	time_t t;
	char *ts;
	int num = code, eno = errno;
	va_list ap;

	if ((t = time(NULL)) == -1) goto _WRITE;
	if (!(ts = ctime(&t))) goto _WRITE;
	ts[strlen(ts) - 1] = 0; /* clear the trailing '\n' */
	dprintf(logfile, "[%s] ", ts);
	_WRITE:
	va_start(ap, fmt);
	vdprintf(logfile, fmt, ap);
	va_end(ap);
	if (num == FAIL_SYS) dprintf(logfile, ": %s", strerror(eno));
	dprintf(logfile, "\n");
}
