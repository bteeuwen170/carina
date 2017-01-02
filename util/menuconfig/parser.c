#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "menuconfig.h"

static const char s_menu[] = "menu";
static const char s_menuconfig[] = "menuconfig";
static const char s_config[] = "config";
static const char s_set_config[] = "set_config";
static const char s_if[] = "if";
static const char s_copen[] = "/*";
static const char s_cclose[] = "*/";

static const char c_end = ';';
static const char c_open = '{';
static const char c_close = '}';
static const char c_string = '"';

struct entry *ep;

static char *strtrm(char *str)
{
	char *trim;

	while (isspace(*(str++)));
	if (*str == '\0')
		return str;

	trim = str + strlen(str) - 1;
	while (trim > str && isspace(*(trim--)));

	*(trim + 2) = '\0';

	return str;
}

static struct entry *child_alloc(struct entry *pp)
{
	struct entry *cp;
	int i;

	cp = malloc(sizeof(struct entry));
	if (!cp)
		return NULL;

	cp->type = 0;
	cp->ident = cp->name = NULL;
	cp->value = 0;
	cp->lock = 0;

	cp->pp = pp;

	if (!pp)
		return cp;

	for (i = 0; i < CHILDREN; i++) {
		if (!pp->cp[i]) {
			pp->cp[i] = cp;
			return cp;
		}
	}

	return NULL;
}

static int stat_read(FILE *fp, char *buf, int _size)
{
	int size = _size;
	int i = 0;
	unsigned char cmnt = 0;

	buf[0] = '\0';
	while (isspace(buf[0] = fgetc(fp)));

	do {
		if (i >= size) {
			size += BUF_SIZE;
			buf = realloc(buf, sizeof(char) * size);
			if (!buf)
				return 0;
		}

		/* FIXME Comments are a little buggy */
		if (cmnt == 0 && buf[i] == s_copen[0]) {
			cmnt++;
		} else if (cmnt == 1) {
			if (buf[i] == s_copen[1])
				cmnt++;
			else
				cmnt = 0;
		} else if (cmnt == 2 && buf[i] == s_cclose[0]) {
			cmnt++;
		} else if (cmnt == 3) {
			if (buf[i] == s_cclose[1]) {
				i = 0;
				break;
			} else {
				cmnt = 2;
			}
		}

		if (cmnt > 1)
			continue;

		if (buf[i] == c_close) {
			ep = ep->pp;
			break;
		}

		if (buf[i] == c_open || buf[i] == c_end) {
			break;
		}
	} while ((buf[++i] = fgetc(fp)) != EOF);

	if (buf[i] == EOF)
		buf[0] = EOF;

	buf[i] = '\0';

	buf = strtrm(buf);

	return size;
}

/* FIXME Very messy */
static char *ident_read(char *buf)
{
	char *m1, *m2, *ident;

	while (isspace(*(buf++)));
	m1 = strpbrk(buf, " \t\n\v\f\r");
	if (!m1)
		return NULL;

	while (isspace(*(m1++)));
	m2 = strpbrk(--m1, " \t\n\v\f\r;");
	if (!m2)
		return NULL;

	/* XXX Safe? */
	strncpy(buf, m1, m2 - m1 + 1);
	buf[m2 - m1] = '\0';

	ident = strdup(buf);

	buf[m2 - m1] = ' ';

	return ident;
}

static char *string_read(char *buf)
{
	buf = strchr(buf, c_string);
	if (!buf)
		return NULL;

	buf++;
	buf[strlen(buf) - 1] = '\0';

	return strdup(buf);
}

struct entry *config_parse(char *path)
{
	FILE *fp;
	char *buf;
	int buf_size = BUF_SIZE;

	fp = fopen(path, "r");
	if (!fp)
		goto err;

	ep = child_alloc(NULL);
	if (!ep)
		goto err;

	buf = malloc(sizeof(char) * BUF_SIZE);
	if (!buf)
		goto err;

	do {
		buf_size = stat_read(fp, buf, buf_size);
		if (!buf_size)
			goto err;

		if (buf[0] == '\0')
			continue;

		if (strncmp(buf, s_menuconfig, strlen(s_menuconfig)) == 0) {
			ep = child_alloc(ep);
			if (!ep)
				goto err;
			ep->type = TYPE_MENUCONFIG;

			ep->ident = ident_read(buf);
			if (!ep->ident)
				goto err;

			ep->name = string_read(buf);
			if (!ep->name)
				goto err;
		} else if (strncmp(buf, s_menu, strlen(s_menu)) == 0) {
			ep = child_alloc(ep);
			if (!ep)
				goto err;
			ep->type = TYPE_MENU;

			ep->name = string_read(buf);
			if (!ep->name)
				goto err;
		} else if (strncmp(buf, s_config, strlen(s_config)) == 0) {
			ep = child_alloc(ep);
			if (!ep)
				goto err;
			ep->type = TYPE_CONFIG;

			ep->ident = ident_read(buf);
			if (!ep->ident)
				goto err;

			ep->name = string_read(buf);
			if (!ep->name)
				goto err;

			ep = ep->pp;
		} else if (strncmp(buf,
				s_set_config, strlen(s_set_config)) == 0) {
			ep = child_alloc(ep);
			if (!ep)
				goto err;
			ep->type = TYPE_CONFIG;

			/* FIXME HACK */
			buf[strlen(buf)] = c_end;
			ep->ident = ident_read(buf);
			if (!ep->ident)
				goto err;

			ep->value = 1;
			ep->lock = 1;

			ep = ep->pp;
		} else if (strncmp(buf, s_if, strlen(s_if)) == 0) {
			ep = child_alloc(ep);
			if (!ep)
				goto err;
			ep->type = TYPE_IF;

			ep->ident = "1";
			/* TODO */
		}
	} while (buf[0] != EOF);

	fclose(fp);

	return ep;

err:
	free(ep); /* TODO And all children... */
	fclose(fp);

	fprintf(stderr, "menuconfig: syntax error or out of memory\n");

	return NULL;
}
