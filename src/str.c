#include <string.h>
#include <limits.h>
#include <stddef.h>
#include <stdio.h>

#include "assert.h"
#include "str.h"
#include "mem.h"
#include "utf8.h"
#include "except.h"

char *Str_asub(const char *s, size_t i, size_t j) {
    char *str, *p;
    assert(s);
    assert(i <= j);

    if(*s == '\0') {
        p = ALLOC(1);
        *p = '\0';
        return p;
    }

    p = str = ALLOC(j - i + 1);

    while (i < j)
        *p++ = s[i++];

    *p = '\0';
    return str;
}

char *Str_adup(const char *s) {
    size_t len;
    void* s1;

    assert(s);

    len = strlen (s) + 1;
    s1 = ALLOC (len);
    return (char *) memcpy (s1, s, len);
}

char *Str_areverse(const char *s) {
    char* p, *str;
    size_t len;

    assert(s);

    len = strlen(s);
    str = p = ALLOC(len + 1);

    *(str+len) = '\0';
    while(len--)
        *p++ = *(s + len);

    return str;
}

char *Str_acat(const char *s1, const char *s2) {
    size_t l1, l2;
    char* t;

    assert(s1);
    assert(s2);

    l1 = strlen(s1);
    l2 = strlen(s2);

    t = ALLOC(l1 + l2 + 1);
    memcpy(t, s1, l1);
    memcpy(t + l1, s2, l2);
    *(t + l1 + l2) = '\0';
    return t;
}

char *Str_acatvx(const char *s, ...) {
    char *str, *p;
    const char *save = s;
    size_t len = 0;
    va_list ap;

    assert(s);

    va_start(ap, s);
    while (s) {
        assert(s);
        len += strlen(s);
        s = va_arg(ap, const char *);
    }
    va_end(ap);

    p = str = ALLOC(len + 1);
    s = save;

    va_start(ap, s);
    while (s) {
        assert(s);
        while (*s)
            *p++ = *s++;

        s = va_arg(ap, const char *);
    }

    va_end(ap);
    *p = '\0';
    return str;
}

char *Str_amap(const char *s, const char *from, const char *to) {
    char map[256] = { 0 };

    if (from && to) {
        unsigned c;
        for (c = 0; c < sizeof map; c++)
            map[c] = c;

        while (*from && *to)
            map[(unsigned char)*from++] = *to++;

        assert(*from == 0 && *to == 0);
    } else {
        assert(from == NULL && to == NULL && s);
        assert(map['a']);
    }

    if (s) {
        char *str, *p;

        p = str = ALLOC(strlen(s) + 1);

        while (*s)
            *p++ = map[(unsigned char)*s++];

        *p = '\0';
        return str;
    } else
        return NULL;
}

char* Str_avsprintf(const char *fmt, va_list ap)
{
    signed cnt = 0;
    size_t sz = 0;
    char *buf;

    sz = 512;
    buf = (char*)ALLOC(sz);
 try_print:
    cnt = vsnprintf(buf, sz, fmt, ap);
    if (cnt == -1) {
        /* Clear indication that output was truncated, but no
            * clear indication of how big buffer needs to be, so
            * simply double existing buffer size for next time.
            */
        cnt = sz * 2;

    } else if (cnt == (signed) sz) {
        /* Output was truncated (since at least the \0 could
            * not fit), but no indication of how big the buffer
            * needs to be, so just double existing buffer size
            * for next time.
            */
        cnt = sz * 2;

    } else if (cnt > (signed) sz) {
        /* Output was truncated, but we were told exactly how
            * big the buffer needs to be next time. Add two chars
            * to the returned size. One for the \0, and one to
            * prevent ambiguity in the next case below.
            */
        cnt = cnt + 2;

    } else if (cnt == (signed)(sz - 1)) {
        /* This is ambiguous. May mean that the output string
            * exactly fits, but on some systems the output string
            * may have been trucated. We can't tell.
            * Just double the buffer size for next time.
            */
        cnt = sz * 2;

    }
    if( cnt < 0) /* capture the case where sz overflowed cnt*/
        RAISE(Mem_Failed);

    if (cnt >= (signed) sz) {
        buf = (char*)ALLOC(cnt + 1);
        sz = cnt + 1;
        goto try_print;
    }

    return buf;
}

char* Str_asprintf(const char *fmt, ...)
{
    char* buf;
    va_list args;

    va_start(args, fmt);

    buf = Str_avsprintf(fmt, args);

    va_end(args);
    return buf;
}

char** Str_split(char* s, const char* delimiters, unsigned empties) {
    tokenizer_t tok = tokenizer( s, delimiters, empties );
    char** buf = ALLOC(512 * sizeof(char*));
    unsigned n = 0;
    char* token;
    unsigned allocs = 1;

    assert(s);
    assert(delimiters);
    assert(empties == TOKENIZER_EMPTIES_OK || empties == TOKENIZER_NO_EMPTIES);
    assert(*delimiters != '\0');

    while ( (token = tokenize( &tok )) != NULL) {
        buf[n++] = token;
        if(n >= 512 * allocs) {
            REALLOC(buf, 512 * allocs * sizeof(char*));
            allocs++;
        }
    }
    buf[n] = NULL;
    return REALLOC(buf, (n+1) * allocs * sizeof(char*));
}

uint32_t* Str_atoucs   (const char *src) {
    size_t len = strlen(src);
    size_t wlen = (len + 1) * sizeof(uint32_t);

    uint32_t* buf = ALLOC(wlen);
    int bytes = u8_toucs(buf, len + 1, src, -1);

    REALLOC(buf, bytes + 1);
    return buf;
}

char* Str_atoutf8   (const uint32_t *src) {
    size_t len = strlen((char*)src);

    char* buf = ALLOC(len);
    int chars = u8_toutf8(buf, len, src, -1);
    REALLOC(buf, (chars + 1) * sizeof(uint32_t));
    return buf;
}

