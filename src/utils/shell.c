#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *pname = "shell";

#define ARGSIZE		128
#define BUFSIZE		1024
#define HISTSIZE	8192
/* TODO Realloc */
#define PATHSIZE	256

/*int builtin_cd(char **argv);
int builtin_exit(char **argv);
int builtin_clear(char **argv);
int builtin_pwd(char **argv);

char clearbuf = 0;

static const char *builtin[] = {
	"cd",
	"clear",
	"exit",
	"pwd"
};

int (*builtin_func[]) (char **) = {
	&builtin_cd,
	&builtin_clear,
	&builtin_exit,
	&builtin_pwd
};*/

static const char *pss = " $ ";
//static const char *path = "/usr/bin/";
//static const char *home = "/home";

static char ps[ARGSIZE];

/*int builtin_cd(char **argv)
{
	if (argv[1]) {
		if (argv[1][0] == '~') {
			char buf[strlen(argv[1]) + strlen(home)];

			strcpy(buf, home);
			strcat(buf, argv[1] + 1);

			if (!chdir(buf))
				return 0;
		} else {
			if (!chdir(argv[1]))
				return 0;
		}
	} else if (!chdir(home)) {
		return 0;
	}

	fprintf(stderr, "%s: unable to find %s\n", argv[0], argv[1]);

	return 1;
}

int builtin_clear(char **argv)
{
	(void) argv;

	printf("\033[H\033[2J");
	fflush(stdout);

	return 0;
}

int builtin_exit(char **argv)
{
	(void) argv;

	exit(0);
	return 0;
}

int builtin_pwd(char **argv)
{
	(void) argv;
	char wd[PATHSIZE];

	if (getcwd(wd, PATHSIZE)) {
		printf("%s\n", wd);
		return 1;
	}

	return 0;
}

int execute(char **argv)
{
	unsigned int i;
	char buf[BUFSIZE];

	if (argv[0] == '\0')
		return 0;

	if (strchr(argv[0], '/')) {
		strcpy(buf, argv[0]);
	} else {
		for (i = 0; i < sizeof(builtin) / sizeof(char *); i++)
			if (strcmp(argv[0], builtin[i]) == 0) {
				(*builtin_func[i]) (argv);
				return 0;
			}

		strcpy(buf, path);
		strcat(buf, argv[0]);
	}

	if (access(buf, X_OK) < 0)
		return 1;

	if (fork() == 0) {
		if (execv(buf, argv) < 0)
			perror(pname);

		exit(1);
	} else {
		wait(0);

		return 1;
	}

	return 0;
}*/

/*char **argv_get(char *cmd)
{
	int i = 0, bs = ARGSIZE;
	char **argv = malloc(sizeof(char *) * bs);
	char *arg;

	if (!argv) {
		fprintf(stderr, "%s: allocation error\n", pname);
		exit(1);
	}

	arg = strtok(cmd, " \t");
	while (arg != NULL) {
		argv[i++] = arg;

		if (i >= bs) {
			bs += ARGSIZE;
			argv = realloc(argv, sizeof(char) * bs);

			if (!argv) {
				fprintf(stderr, "%s: allocation error\n", pname);
				exit(1);
			}
		}

		arg = strtok(NULL, " \t");
	}

	argv[i] = arg;

	return argv;
}

void go_home(int *p)
{
	if (*p == 0)
		return;

	printf("\033[%dD", *p);
	*p = 0;
}

void go_end(int *p, int *i)
{
	if (getch() != '~' || *p == *i)
		return;

	printf("\033[%dC", *i - *p);
	*p = *i;
}

void signal_handle(int s)
{
	switch (s) {
	case SIGINT:
		printf("\n%s", ps);
		fflush(stdout);
		clearbuf = 1;
		break;
	}
}*/

void ps_update(void)
{
	//char wd[PATHSIZE];

	//if (getcwd(wd, PATHSIZE)) {
	//	strcpy(ps, wd);
	//	strcat(ps, pss);
	//} else {
		strcpy(ps, pss);
	//}
}

/* TODO Stuff has to freed */
int main_shell(void)
{
	int h = 0, i = 0, j = 0, p = 0, u = 0;
	int bs = BUFSIZE, hs = HISTSIZE;
	char c = '\0';
	char *cmd = malloc(sizeof(char) * bs);
	char *hcmd;
	char **hist = malloc(sizeof(char *) * hs);

	ps_update();
	prints(ps);

	for (;;) {
		c = getch();

		switch (c) {
		/* New line */
		case '\n':
		case EOF:
			printc(c);

			if (c != '\0') {
				hist[h] = malloc(strlen(cmd));
				memcpy(hist[h], cmd, strlen(cmd));
			}

			prints("Command not found");
			//execute(argv_get(cmd));
			/* if (execute(argv_get(cmd)) != 0) */
			/*	fprintf(stderr, "%s: unable to find %s\n", pname, cmd); */ /* XXX TEMP TEMP XXX */

			memset(cmd, 0, sizeof(char) * bs);

			ps_update();
			prints(ps);

			i = p = u = 0;
			h++;
			break;
		/* Backspace */
		case '\177':
			u = 0;

			if (p > 0) {
				printc(c);
				printc(' ');

				for (j = --p; j < i; j++)
					cmd[j] = cmd[j + 1];

				//printf("\033[2K\r");
				prints(ps);
				prints(cmd);
				//if (p != --i)
				//	printf("\033[%dD", i - p);
			}
			break;
		default:
			u = 0;

			if (p == i) {
				printc(c);
				//strncat(cmd, &c, 1);

				/*if (p++ == i && ++i >= bs) {
					bs += BUFSIZE;
					cmd = realloc(cmd, sizeof(char) * bs);

					if (!cmd) {
						fprintf(stderr, "%s: allocation error\n", pname);
						exit(1);
					}
				}*/
			/* TODO Realloc */
			} else {
				for (j = ++i; j > p; j--)
					cmd[j] = cmd[j - 1];

				cmd[p++] = c;
		}
		break;
		}
	}
}

