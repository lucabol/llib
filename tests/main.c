#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <assert.h>
#include <test.h>
#include <except.h>
#include <log.h>

#include <mem.h>
#include <ring.h>

#include "getopt.h"

static int verbosity = 0;

static struct option long_options[] = {
    {"verbosity",      no_argument,       NULL,  'v' , "print out debug messages","", getopt_none, NULL ,&verbosity},
};

unsigned test_log();
unsigned test_native_exceptions();
unsigned test_ring_apply();
unsigned test_mem_perf();
unsigned test_mem_free();
unsigned test_arena_resize();
unsigned test_list();
unsigned test_list_perf();
unsigned test_getopt_parse();
unsigned test_utf8_roundtrip();
unsigned test_utf8_len();
unsigned test_utf8_sub();
unsigned test_str();
unsigned test_utf8_rev();
unsigned test_token();
unsigned test_file();
unsigned test_int_sign();
unsigned test_safe_int();
unsigned test_except_all();
unsigned test_rand();
unsigned test_gauss();
unsigned test_randstream();
unsigned test_streamgauss();
unsigned test_parallelrand();
unsigned test_parallelrand1();
unsigned test_atom();
unsigned test_table();

int main(int argc, char *argv[])
{
    int res;

    if(getopt_parse(argc, argv, long_options, "-- tests for llib", "", "") < 0)
        exit(-3);

    if(verbosity)
        log_set(stderr, LOG_INFO);

    test_add("mem", "free",             test_mem_free);
    test_add("mem", "perf",             test_mem_perf);
    test_add("arena", "resize",         test_arena_resize);
    test_add("exception", "all",        test_except_all);
    test_add("exception", "native",     test_native_exceptions);
    test_add("log", "printing",         test_log);
    test_add("list", "basic",           test_list);
    test_add("list", "perf",            test_list_perf);
    test_add("ring", "apply",           test_ring_apply);
    test_add("getopt", "parse",         test_getopt_parse);
    test_add("utf8", "roundtrip",       test_utf8_roundtrip);
    test_add("utf8", "strlen",          test_utf8_len);
    test_add("utf8", "sub",             test_utf8_sub);
    test_add("utf8", "rev",             test_utf8_rev);
    test_add("str", "all",              test_str);
    test_add("token", "all",            test_token);
    test_add("file", "all",             test_file);
    test_add("int", "signed",           test_int_sign);
    test_add("safeint", "cast",         test_safe_int);
    test_add("rand", "avg&std",         test_rand);
    test_add("rand", "gauss",           test_gauss);
    test_add("randstream", "rstream",   test_randstream);
    test_add("randstream", "gauss",     test_streamgauss);
    test_add("randstream", "parallel",  test_parallelrand);
    test_add("randstream", "parallel",  test_parallelrand1);
    test_add("atom", "general",         test_atom);
    test_add("table", "general",        test_table);
    res = test_run_all();

    Mem_print_stats();

    return res;
}
