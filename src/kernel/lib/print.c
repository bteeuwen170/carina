/*
 * TODO Write own version!
 * TODO Implement color
 */

/*
 *
 * vsprintf.c -- Lars Wirzenius & Linus Torvalds.
 *
 * Wirzenius wrote this portably, Torvalds fucked it up :-)
 *
 * (C) 1991  Linus Torvalds
 * Last modified by Bastiaan Teeuwen on 2016/09/19
 *
 */

#include <video/fb.h>
#include <kbd/kbd.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <system.h>
#include <video/vga.h>
#include <kernel/print.h>

/* we use this so that we can do without the ctype library */
#define is_digit(c)	((c) >= '0' && (c) <= '9')

static int skip_atoi(const char **s)
{
	int i = 0;

	while (is_digit(**s))
		i = i * 10 + *((*s)++) - '0';
	return i;
}

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */

static char *number(int allowed_length, char * str, long num, int base,
		int size, int precision, int type)
{
	char c, sign, tmp[72];
	const char *digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	int i = 0;

	if (type & LEFT)
		type &= ~ZEROPAD;

	if (base < 2 || base > 36)
		return 0;

	c = (type & ZEROPAD) ? '0' : ' ';

	if (type & SIGN && num < 0) {
		sign = '-';
		num = -num;
	} else {
		sign = (type & PLUS) ? '+' : ((type & SPACE) ? ' ' : 0);
	}

	if (sign)
		size--;
	if (type & SPECIAL) {
		if (base == 16)
			size -= 2;
		else if (base == 8)
			size--;
	}

	if (num == 0)
		tmp[i++] = '0';
	else
		while (num != 0) {
			tmp[i++] = digits[num % base];
			num = num / base;
		}

	if (i > precision)
		precision = i;
	size -= precision;

	if (!(type & (ZEROPAD + LEFT)))
		while (size-- > 0) {
			if (allowed_length == 0)
				return str;
			*str++ = ' ';
			allowed_length--;
		}

	if (sign) {
		if (allowed_length == 0)
			return str;

		*str++ = sign;
		allowed_length--;
	}

	if (type & SPECIAL) {
		if (base == 8) {
			if (allowed_length == 0)
				return str;

			*str++ = '0';
			allowed_length--;
		} else if (base == 16) {
			if (allowed_length == 0)
				return str;

			*str++ = '0';

			if(--allowed_length == 0)
				return str;

			*str++ = 'x';
			allowed_length--;
		}
	}
	if (!(type & LEFT))
		while (size-- > 0) {
			if (allowed_length == 0)
				return str;

			*str++ = c;
			allowed_length--;
		}
	
	while (i < precision--) {
		if (allowed_length == 0)
			return str;

		*str++ = '0';
		allowed_length--;
	}

	while (i-- > 0) {
		if (allowed_length == 0)
			return str;

		*str++ = tmp[i];
		allowed_length--;
	}
	
	while (size-- > 0) {
		if (allowed_length == 0)
			return str;

		*str++ = ' ';
		allowed_length--;
	}

	return str;
}

int vsnprintf(char *buf, int size, const char *fmt, va_list args)
{
	int len;
	int i;
	char *str, *newstr;
	char *s;
	int *ip;

	int flags = 0;

	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max
				   number of chars for from string */

	int used_size = 1; /* for null character */
	
	if (size == 0)
		return 0;
	*buf=0;
	if (size == 1)
		return 1;

	for (str=buf; *fmt; fmt++) {
		if (*fmt != '%') {
			if (used_size >= size)
				break;

			*str++ = *fmt;
			used_size++;
			continue;
		}
			
		/* Process flags */
rep:
		fmt++;
		switch (*fmt) {
			case '-':
				flags |= LEFT;
				goto rep;
			case '+':
				flags |= PLUS;
				goto rep;
			case ' ':
				flags |= SPACE;
				goto rep;
			case '#':
				flags |= SPECIAL;
				goto rep;
			case '0':
				flags |= ZEROPAD;
				goto rep;
		}
		
		/* get field width */
		field_width = -1;
		if (is_digit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == '*') {
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == '.') {
			fmt++;	
			if (is_digit(*fmt)) {
				precision = skip_atoi(&fmt);
			} else if (*fmt == '*') {
				precision = va_arg(args, int);
			}

			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L')
			++fmt;

		switch (*fmt) {
		case 'c':
			if (!(flags & LEFT))
				while (--field_width > 0) {
					if(used_size >= size)
						goto ret;
					*str++ = ' ';
					used_size++;
				}
			
			if(used_size >= size)
				goto ret;
			*str++ = (unsigned char) va_arg(args, int);
			used_size++;
			while (--field_width > 0) {
				if(used_size >= size)
					goto ret;
				*str++ = ' ';
				used_size++;
			}
			break;
		case 's':
			s = va_arg(args, char *);
			len = strlen(s);
			if (precision < 0)
				precision = len;
			else if (len > precision)
				len = precision;

			if (!(flags & LEFT))
				while (len < field_width--) {
					if(used_size >= size)
						goto ret;
					*str++ = ' ';
					used_size++;
				}
			for (i = 0; i < len; ++i) {
				if(used_size >= size)
					goto ret;
				*str++ = *s++;
				used_size++;
			}
			while (len < field_width--) {
				if(used_size >= size)
					goto ret;
				*str++ = ' ';
				used_size++;
			}
			break;

		case 'o':
			newstr = number(size-used_size, str, va_arg(args, unsigned long), 8,
				field_width, precision, flags);
			used_size += newstr - str; str = newstr;
			if(used_size >= size)
				goto ret;
			break;

		case 'p':
			if (field_width == -1) {
				field_width = 8;
				flags |= ZEROPAD;
			}
			newstr = number(size-used_size, str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			used_size += newstr - str; str = newstr;
			if(used_size >= size)
				goto ret;
			break;
		case 'x':
			newstr = number(size-used_size, str, va_arg(args, unsigned long), 16,
				field_width, precision, flags);

			used_size += newstr - str;
			str = newstr;

			if(used_size >= size)
				goto ret;

			break;

		case 'd':
		case 'i':
			flags |= SIGN;
		case 'u':
			newstr = number(size-used_size, str, va_arg(args, unsigned long), 10,
				field_width, precision, flags);
			used_size += newstr - str; str = newstr;
			if(used_size >= size)
				goto ret;
			break;

		case 'n':
			ip = va_arg(args, int *);
			*ip = (str - buf);
			break;

		default:
			if (*fmt != '%') {
				if(used_size >= size)
					goto ret;
				*str++ = '%';
				used_size++;
			}
			if (*fmt) {
				if(used_size >= size)
					goto ret;
				*str++ = *fmt;
				used_size++;
			}
			else
				--fmt;
			break;
		}
	}

ret:
	*str=0;
	return used_size;
}

i32 vsprintf(char *s, const char *fmt, va_list args)
{
	/* TODO Define a size_max */
	return vsnprintf(s, 512, fmt, args);
}

i32 sprintf(char *buf, const char *fmt, ...)
{
	int res;
	va_list args;

	va_start(args, fmt);
	res = vsprintf(buf, fmt, args);
	va_end(args);

	return res;
}

/* This function is a mess */
void kprintf(const loglevel_t kp, char *prefix, char *fmt, ...)
{
	(void) kp;
	char printbuf[512], fmtbuf[512], prefixbuf[256];
	va_list args;

	//if (kp > get_kp();

	va_start(args, fmt);
	vsnprintf(printbuf, 512, fmt, args);

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
	char printbuf[512];
	va_list args;

	va_start(args, fmt);
	vsnprintf(printbuf, 512, fmt, args);
	prints(printbuf);
	va_end(args);
}
