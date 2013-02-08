#include <stdarg.h>
#include <stdint.h>

int u8_toucs    (uint32_t *dest, int sz, const char *src, int srcsz);
int u8_toutf8   (char *dest, int sz, const uint32_t *src, int srcsz);

int u8_wc_toutf8(char *dest, uint32_t ch); /* single character to UTF-8 */

int u8_offset   (const char *str, int charnum);
int u8_charnum  (const char *s, int offset);

uint32_t u8_nextchar    (const char *s, int *i);
void u8_inc             (const char *s, int *i);
void u8_dec             (const char *s, int *i);


char *u8_strchr(char *s, uint32_t ch, int *charn);
char *u8_memchr(char *s, uint32_t ch, size_t sz, int *charn); /* doesn't stop at end of string char, but at sz*/

int u8_strlen(const char *s);

int u8_is_locale_utf8(char *locale);

char *u8_sub (const char *s, int i, int j);

int u8_vprintf(char *fmt, va_list ap); /* fmt can be utf as well*/
int u8_printf(char *fmt, ...);
