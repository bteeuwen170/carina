#include <ncurses.h>

#include <string.h>

#include "menuconfig.h"

#define COLOR_W		1
#define COLOR_MW	2

static WINDOW *mwp;

void entry_print(char *name)
{
	
}

void window_clear(void)
{
	wrefresh(mwp);

	getch();
}

void window_init(void)
{
	int height, width;

	initscr();
	noecho();
	/* curs_set(0); */

	getmaxyx(stdscr, height, width);

	mwp = newwin(height - 4, width - 5, 2, 2);

	if (has_colors()) {
		start_color();
		init_pair(COLOR_W, COLOR_BLACK, COLOR_BLUE);
		init_pair(COLOR_MW, COLOR_BLACK, COLOR_WHITE);

		bkgd(COLOR_PAIR(COLOR_W));

		/* hline(0x2588, width); */

		move(3, width - 3);
		vline(0x2588, height - 5);
		move(3, width - 2);
		vline(0x2588, height - 5);

		move(height - 2, 4);
		hline(0x2588, width - 5);

		attron(A_BOLD);
		wbkgd(mwp, COLOR_PAIR(COLOR_MW)); /* TODO OR */
		attroff(A_BOLD);
	}

	refresh();
}

void window_term(void)
{
	delwin(mwp);
	endwin();
}
