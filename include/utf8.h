#ifndef UTF8_INCLUDED
#define UTF8_INCLUDED

#include <stdarg.h>
#include <stdint.h>

#include "utils.h"

BEGIN_DECLS

int u8_offset   (const char *str, int charnum);
int u8_charnum  (const char *s, int offset);

uint32_t u8_nextchar    (const char *s, int *i);
void u8_inc             (const char *s, int *i);
void u8_dec             (const char *s, int *i);


char *u8_strchr(char *s, uint32_t ch, int *charn);
char *u8_memchr(char *s, uint32_t ch, size_t sz, int *charn); /* doesn't stop at end of string char, but at sz*/

int u8_is_locale_utf8(char *locale);

int u8_strlen(const char *s);
char *u8_sub (const char *s, int i, int j);
char *u8_reverse(const char*s);

int u8_vprintf(char *fmt, va_list ap); /* fmt can be utf as well*/
int u8_printf(char *fmt, ...);

END_DECLS

#endif