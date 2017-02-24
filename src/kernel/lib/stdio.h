/* XXX TEMP XXX */
#ifndef _TEMP_TEMP_TEMP_STDIO_H
#define _TEMP_TEMP_TEMP_STDIO_H

/* XXX TEMP XXX */
#ifdef CONFIG_KEYBOARD
char getch(void);
#else
static inline char getch(void) { return 0; }
#endif
/* XXX TEMP XXX */

#endif
/* XXX TEMP XXX */
