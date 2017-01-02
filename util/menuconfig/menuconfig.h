
#define TYPE_MENU	1
#define TYPE_MENUCONFIG	2
#define TYPE_CONFIG	3
#define TYPE_IF		4

#define CHILDREN	50
#define BUF_SIZE	1024

struct entry {
	int		type;		/* Entry type */
	char		*ident;		/* Identifier or statement */
	char		*name;		/* Name */
	unsigned char	value;		/* Value */
	unsigned char	lock;		/* Disallow writing */

	struct entry	*pp;		/* Parent pointer */
	struct entry	*cp[CHILDREN];	/* Children pointer */
};

struct entry *config_parse(char *path);

void window_clear(void);
void window_init(void);
void window_term(void);
