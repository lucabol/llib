#include <stdint.h>
#include <string.h>

#include "utf8.h"
#include "test.h"
#include "log.h"
#include "locale.h"

int test(char* str) {
    uint32_t   ucsbuf[256];
    char       u8buf[256];
    char       escbuf[256];

    u8_toucs(ucsbuf, 256, str, -1);
    u8_toutf8(u8buf, 256, ucsbuf, -1);

    /* even chcp 65001 (utf-8) and Lucide Console don't print arabic and chinese
       you need to redirect to a text file to see them.
    */
    log("%s : redirect stderr to  a file to see utf-8 chars", u8buf);

    test_assert_str(u8buf, str);

    u8_escape(escbuf, 256, u8buf, 1);
    log(escbuf);
    return TEST_SUCCESS;
}

/* Chinese characters for "zhongwen" ("Chinese language"). 2 chars */
static char kChineseSampleText[]     = {-28, -72, -83, -26, -106, -121, 0};

/* Arabic "al-arabiyya" ("Arabic"). */
static char kArabicSampleText[]      = {-40, -89, -39, -124, -40, -71, -40, -79, -40, -88, -39, -118, -40, -87, 0};

/* Spanish word "canon" with an "n" with "~" on top and an "o" with an acute accent. 5 chars*/
static char kSpanishSampleText[]     = {99, 97, -61, -79, -61, -77, 110, 0};

#define mytest(str) if(test(str) == TEST_FAILURE) return TEST_FAILURE;

int test_utf8_roundtrip() {

#ifdef _WIN32 /* windows doesn't support utf-8 locale */
    char* locale = setlocale(LC_ALL, "");
    test_assert(!u8_is_locale_utf8(locale));
    setlocale(LC_ALL, locale);
#endif

    mytest(kChineseSampleText);
    mytest(kArabicSampleText);
    mytest(kSpanishSampleText);

    return TEST_SUCCESS;
}

int test_utf8_len() {
    test_assert(u8_strlen(kChineseSampleText) == 2);
    test_assert(u8_strlen(kArabicSampleText) == 7);
    test_assert(u8_strlen(kSpanishSampleText) == 5);
    return TEST_SUCCESS;
}