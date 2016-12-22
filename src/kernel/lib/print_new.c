/*
 *
 * Elarix
 * src/kernel/lib/print.c
 *
 * Copyright (C) 2016 Bastiaan Teeuwen <bastiaan.teeuwen170@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301,
 * USA.
 *
 */

/*
 * TODO
 * Binary output
 */

/*TMP*/
#include <char/serial/serial.h>
#include <kbd/kbd.h>
#include <video/fb.h>
#include <video/vga.h>
/*TMP*/

#include <kernel.h>

#include <stdarg.h>
#include <string.h>

/* Format flags */
#define FLAG_LEFT	1	/* Left justified output */
#define FLAG_POS	2
#define FLAG_ALT	4
#define FLAG_SPACE	8
#define FLAG_ZERO	16
#define FLAG_DONE	32
#define FLAG_UNSIGNED	64

/* Length modifiers */
#define LEN_CHAR	1
#define LEN_SHORT	2
#define LEN_LONG	4
#define LEN_LONGLONG	8
#define LEN_SIZET	16

/* Base modifiers */
#define BASE_OCT	8
#define BASE_DEC	10
#define BASE_HEX	16

static char *num_get(char *str, size_t num, int base, int size, int prec,
		int type)
{
	return " ";
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
	char *str;

	for (str = buf; *fmt; fmt++) {
		const char *stop = fmt;

		int flags = 0;
		int width = -1;
		int prec = 0;
		int len = 0;
		int base = 0;

		while (*fmt && *fmt != '%') {
			*str++ = *fmt;
			continue;
		}

		/* Format flags */
		while (!(flags & FLAG_DONE)) {
			/* XXX Can't contain multiple % right */
			switch (*(++fmt)) {
			case '-':
				flags |= FLAG_LEFT;
				break;
			case '+':
				flags |= FLAG_POS;
				break;
			case '#':
				flags |= FLAG_ALT;
				break;
			case ' ':
				flags |= FLAG_SPACE;
				break;
			case '0':
				flags |= FLAG_ZERO;
				break;
			default:
				flags |= FLAG_DONE;
			}
		}

		/* Min. width */
		if (*(fmt++) == '*') {
			width = va_arg(args, int);

			if (width < 0) {
				flags |= FLAG_LEFT;
				width = -width;
			}
		} else {
			//TODO We will have strtoi for this
			width = (int) strtol(fmt, (char **) &fmt, 10);
		}

		/* Precision */
		if (*(fmt++) == '.') {
			if (*fmt == '*')
				prec = va_arg(args, int);
			else
				prec = (int) strtol(fmt, (char **) &fmt, 10);
				//TODO We will have strtoi for this 

			flags &= ~FLAG_ZERO;
		}

		/* Length */
		switch (*(fmt++)) {
		case 'h':
			if (*fmt == 'h') {
				len = LEN_CHAR;
				fmt++;
			} else {
				len = LEN_SHORT;
			}
			break;
		case 'l':
			if (*fmt == 'l') {
				len = LEN_LONGLONG;
				fmt++;
			} else {
				len = LEN_LONG;
			}
			break;
		case 'z':
			len = LEN_SIZET;
			break;
		default:
			fmt--;
			break;
		}

		/* Conversion */
		switch (*fmt) {
		case '%':
			*str++ = '%';
			break;

		/*
		 * unsigned int, decimal
		 */
		case 'u':
			flags |= FLAG_UNSIGNED;
		case 'd':
			base = BASE_DEC;
			break;
		/*
		 * unsigned int, octal
		 */
		case 'o':
			flags |= FLAG_UNSIGNED;
			base = BASE_OCT;
			break;
		/*
		 * unsigned int, hex
		 */
		case 'x':
		case 'X':
			flags |= FLAG_UNSIGNED;
			base = BASE_HEX;
			break;
		/*
		 * char
		 */
		case 'c':
			if (!(flags & FLAG_LEFT))
				while (--width > 0)
					*str++ = ' ';

			*str++ = (unsigned char) va_arg(args, int);

			while (--width > 0)
				*str++ = ' ';
			continue;
		/*
		 * char *
		 */
		case 's': {
				char *s = va_arg(args, char*);
				int l = strnlen(s, prec);
				int i;

				if (!(flags & FLAG_LEFT))
					while (--width > 0)
						*str++ = ' ';

				for (i = 0; i < l; i++)
					*str++ = *s++;

				while (--width > 0)
					*str++ = ' ';
				continue;
			}
		/*
		 * void *
		 */
		case 'p':
			base = BASE_HEX;
			//TODO
			continue;
		/*
		 * int *
		 */
		case 'n':
			//TODO
			continue;

		default:
			*str++ = '%';

			if (*fmt)
				*str++ = *fmt;
			else
				fmt--;
			continue;
		}

		if (!base)
			continue;

		if (flags & FLAG_UNSIGNED) {
			size_t n = 0;

			switch (len) {
			case LEN_CHAR:
				n = (unsigned char) va_arg(args, int);
				break;
			case LEN_SHORT:
				n = (unsigned short) va_arg(args, int);
				break;
			case LEN_LONG:
				n = va_arg(args, unsigned long);
				break;
			case LEN_LONGLONG:
				n = va_arg(args, unsigned long long);
				break;
			case LEN_SIZET:
				n = va_arg(args, size_t);
				break;
			default:
				n = va_arg(args, unsigned int);
				break;
			}

			str = num_get(str, n, base, width, prec, flags);
		} else {
			ssize_t n = 0;

			switch (len) {
			case LEN_CHAR:
				n = (char) va_arg(args, int);
				break;
			case LEN_SHORT:
				n = (short) va_arg(args, int);
				break;
			case LEN_LONG:
				n = va_arg(args, long);
				break;
			case LEN_LONGLONG:
				n = va_arg(args, long long);
				break;
			case LEN_SIZET:
				n = va_arg(args, ssize_t);
				break;
			default:
				n = va_arg(args, unsigned int);
				break;
			}

			str = num_get(str, n, base, width, prec, flags);
		}
	}

	*str = '\0';

	return str - buf;
}

//TODO vfprintf, vprintf, fprintf, printf, sprintf
//TODO scanf, fscanf, sscanf
//TODO fputs

i32 sprintf(char *buf, const char *fmt, ...)
{
	int res;
	va_list args;

	va_start(args, fmt);
	res = vsprintf(buf, fmt, args);
	va_end(args);

	return res;
}

/* TODO Check if vsprintf succeeds */
/* This function is a mess */
void kprintf(const loglevel_t kp, char *prefix, char *fmt, ...)
{
	(void) kp;
	char printbuf[1024], fmtbuf[1024], prefixbuf[1024];
	va_list args;

	//if (kp > get_kp();

	va_start(args, fmt);
	vsprintf(printbuf, fmt, args);

	sprintf(prefixbuf, "%7s", prefix);
	fmtbuf[0] = '\0';
	if (prefix) {
		strcat(fmtbuf, prefixbuf);
		strcat(fmtbuf, ": ");
	}
	strcat(fmtbuf, printbuf);

	prints(fmtbuf);
	va_end(args);

	//TEMP
	u32 i;
	for (i = 0; i < strlen(fmtbuf); i++)
		serial_out(COM0, fmtbuf[i]);
	serial_out(COM0, 0x0D);
}

/* TEMP XXX */
void kprint(char *fmt, ...)
{
	char printbuf[1024];
	va_list args;

	va_start(args, fmt);
	vsprintf(printbuf, fmt, args);
	prints(printbuf);
	va_end(args);
}
