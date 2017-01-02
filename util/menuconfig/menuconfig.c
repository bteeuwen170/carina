#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "menuconfig.h"

#if 1
struct entry *lp[CHILDREN];

static int getch(void)
{
	struct termios term, oterm;
	int c = 0;

	tcgetattr(0, &oterm);
	memcpy(&term, &oterm, sizeof(term));

	term.c_lflag &= ~(ICANON | ECHO);
	term.c_cc[VMIN] = 1;
	term.c_cc[VTIME] = 0;

	tcsetattr(0, TCSANOW, &term);

	c = getchar();
	tcsetattr(0, TCSANOW, &oterm);

	return c;
}

/* TODO Move this to the parser and reparse everytime a value is modified */
static int eval_if(const char *stat)
{
	(void) stat;
	return 1;
}

static void print_item(struct entry *mep, int index)
{
	if (mep->lock)
		return;

	switch (mep->type) {
	case TYPE_MENU:
		printf(" %d |     %s --->\n", index + 1, mep->name);
		break;
	case TYPE_MENUCONFIG:
		printf(" %d | [%c] %s (%s) %s\n", index + 1,
				mep->value ? 'x' : ' ',
				mep->name, mep->ident,
				mep->value ? "--->" : "");
		break;
	case TYPE_CONFIG:
		printf(" %d | [%c] %s (%s)\n", index + 1,
				mep->value ? 'x' : ' ',
				mep->name, mep->ident);
		break;
	}
}

static int eval_menu(struct entry *mp)
{
	int i, j;

	if (!mp)
		return 0;

	for (i = 0; i < CHILDREN; i++) {
		if (!mp->cp[i])
			break;

		if (mp->cp[i]->type == TYPE_IF) {
			if (eval_if(mp->ident))
				eval_menu(mp->cp[i]);
		} else {
			for (j = 0; j < CHILDREN; j++) {
				if (!lp[j]) {
					lp[j] = mp->cp[i];
					print_item(lp[j], j);
					break;
				}
			}

			if (j >= CHILDREN)
				return 0;
		}
	}

	return 1;
}

static int menu_display(struct entry *mp)
{
	int c, c2;

	memset(lp, 0, sizeof(*lp) * CHILDREN);

	printf("\n     %s\n", (mp->name) ? mp->name : "Main Menu");
	printf(
"     Controls: [space] and [0-9] to move down a menuconfig; \
[return] and [0-9] to toggle an option.\n\n");

	if (!eval_menu(mp))
		return 1;

	/* if (!i) {
		if (mp->pp)
			return menu_display(mp->pp);
		else
			return 1;
	} */

	printf("\n$ ");
retry:
	c = getch();

	if (isdigit(c) && lp[c - '0' - 1]) {
		printf("%d", c - '0');
		switch (lp[c - '0' - 1]->type) {
		case TYPE_MENUCONFIG:
			if (!lp[c - '0' - 1]->value) {
				lp[c - '0' - 1]->value = 1;
				break;
			} else {
				c2 = getch();
				if (c2 != ' ') {
					lp[c - '0' - 1]->value = 0;
					break;
				}
			}
		case TYPE_MENU:
			return menu_display(lp[c - '0' - 1]);
		case TYPE_CONFIG:
			lp[c - '0' - 1]->value ^= 1;
		case TYPE_IF:
			break;
		}

		return menu_display(mp);
	} else if ((c == 127 || c == 'q')) {
		if (mp->pp)
			return menu_display(mp->pp);
		else
			return 0;
	}

	goto retry;
}
#else
static void menu_display(void)
{
	
}
#endif

int main(int argc, char **argv)
{
	struct entry *mp;
	int res;

	if (argc != 2)
		return 1;

	mp = config_parse(*(++argv));
	if (!mp)
		return 1;

#if 1
	res = menu_display(mp);
	putc('\n', stdout);
#else
	window_init();
	window_clear();

	menu_display();

	window_term();
#endif

	return res;
}
