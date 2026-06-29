#define INCLUDE_CONF_IMPLEMENTATION
#include "conf.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int nfailed = 0;
static int npassed = 0;

#define TEST(name, expr) do {                                           \
        if (!(expr)) {                                                  \
                fprintf(stderr, "FAIL: %s (%s)\n", name, #expr);        \
                nfailed++;                                              \
        } else {                                                        \
                npassed++;                                              \
        }                                                               \
} while(0)

static void
test_open(void)
{
        Conf conf;

        /* success */
        TEST("Conf_open valid file",
             Conf_open(&conf, "test.lua") == CONF_OK);
        Conf_close(conf);

        /* file not found */
        TEST("Conf_open missing file",
             Conf_open(&conf, "nonexistent.lua") == CONF_NOTFOUND);

        /* syntax error in file */
        {
                FILE *f = fopen("/tmp/bad.lua", "w");
                assert(f);
                fprintf(f, "syntax error {{{{{");
                fclose(f);
                TEST("Conf_open syntax error",
                     Conf_open(&conf, "/tmp/bad.lua") == CONF_NOTFOUND);
                remove("/tmp/bad.lua");
        }
}

static void
test_close(void)
{
        Conf conf;

        /* success */
        Conf_open(&conf, "test.lua");
        TEST("Conf_close valid", Conf_close(conf) == CONF_OK);

        /* NULL handle */
        TEST("Conf_close NULL", Conf_close(NULL) == CONF_INVALID);
}

static void
test_get_num(void)
{
        Conf conf;
        double val;

        Conf_open(&conf, "test.lua");

        /* success */
        TEST("Conf_get_num valid", Conf_get_num(conf, "NumVal", &val) == CONF_OK);
        TEST("Conf_get_num value", val == 42.5);

        /* nested success */
        TEST("Conf_get_num nested",
             Conf_get_num(conf, "Deep.a.b.c", &val) == CONF_OK);
        TEST("Conf_get_num nested value", val == 42);

        /* missing key */
        TEST("Conf_get_num missing",
             Conf_get_num(conf, "Nonexistent", &val) == CONF_UNDEF);

        /* deep missing intermediate */
        TEST("Conf_get_num missing deep",
             Conf_get_num(conf, "Deep.a.z", &val) == CONF_UNDEF);

        /* type mismatch: string */
        TEST("Conf_get_num string",
             Conf_get_num(conf, "StrVal", &val) == CONF_INVALID);

        /* type mismatch: bool */
        TEST("Conf_get_num bool",
             Conf_get_num(conf, "TrueVal", &val) == CONF_INVALID);

        /* type mismatch: table (final value) */
        TEST("Conf_get_num table",
             Conf_get_num(conf, "TableVal", &val) == CONF_INVALID);

        /* intermediate not a table */
        TEST("Conf_get_num intermediate not table",
             Conf_get_num(conf, "Deep.a.b.c.foo", &val) == CONF_INVALID);

        /* deep missing last key (nil) */
        TEST("Conf_get_num deep nil",
             Conf_get_num(conf, "Deep.a.b.z", &val) == CONF_UNDEF);

        Conf_close(conf);
}

static void
test_get_int(void)
{
        Conf conf;
        long long val;

        Conf_open(&conf, "test.lua");

        /* success */
        TEST("Conf_get_int valid", Conf_get_int(conf, "IntVal", &val) == CONF_OK);
        TEST("Conf_get_int value", val == 100);

        /* negative */
        TEST("Conf_get_int negative",
             Conf_get_int(conf, "NegInt", &val) == CONF_OK);
        TEST("Conf_get_int negative value", val == -7);

        /* zero */
        TEST("Conf_get_int zero",
             Conf_get_int(conf, "ZeroInt", &val) == CONF_OK);
        TEST("Conf_get_int zero value", val == 0);

        /* float rejected */
        TEST("Conf_get_int float rejected",
             Conf_get_int(conf, "FloatVal", &val) == CONF_INVALID);

        /* missing key */
        TEST("Conf_get_int missing",
             Conf_get_int(conf, "Nonexistent", &val) == CONF_UNDEF);

        /* deep missing */
        TEST("Conf_get_int deep nil",
             Conf_get_int(conf, "Deep.a.z", &val) == CONF_UNDEF);

        /* type mismatch: string */
        TEST("Conf_get_int string",
             Conf_get_int(conf, "StrVal", &val) == CONF_INVALID);

        /* type mismatch: bool */
        TEST("Conf_get_int bool",
             Conf_get_int(conf, "TrueVal", &val) == CONF_INVALID);

        /* intermediate not table */
        TEST("Conf_get_int intermediate not table",
             Conf_get_int(conf, "Deep.a.b.c.foo", &val) == CONF_INVALID);

        Conf_close(conf);
}

static void
test_get_str(void)
{
        Conf conf;
        const char *str;

        Conf_open(&conf, "test.lua");

        /* success */
        TEST("Conf_get_str valid",
             Conf_get_str(conf, "StrVal", &str) == CONF_OK);
        TEST("Conf_get_str value", strcmp(str, "hello") == 0);

        /* empty string */
        TEST("Conf_get_str empty",
             Conf_get_str(conf, "EmptyStr", &str) == CONF_OK);
        TEST("Conf_get_str empty value", strcmp(str, "") == 0);

        /* nested string */
        TEST("Conf_get_str nested",
             Conf_get_str(conf, "Deep.a.b.d", &str) == CONF_OK);
        TEST("Conf_get_str nested value", strcmp(str, "nested_str") == 0);

        /* missing key */
        TEST("Conf_get_str missing",
             Conf_get_str(conf, "Nonexistent", &str) == CONF_UNDEF);

        /* type mismatch: number */
        TEST("Conf_get_str number",
             Conf_get_str(conf, "NumVal", &str) == CONF_INVALID);

        /* type mismatch: bool */
        TEST("Conf_get_str bool",
             Conf_get_str(conf, "TrueVal", &str) == CONF_INVALID);

        /* type mismatch: table */
        TEST("Conf_get_str table",
             Conf_get_str(conf, "TableVal", &str) == CONF_INVALID);

        /* intermediate not table */
        TEST("Conf_get_str intermediate not table",
             Conf_get_str(conf, "Deep.a.b.c.foo", &str) == CONF_INVALID);

        Conf_close(conf);
}

static void
test_get_bool(void)
{
        Conf conf;
        int val;

        Conf_open(&conf, "test.lua");

        /* true */
        TEST("Conf_get_bool true",
             Conf_get_bool(conf, "TrueVal", &val) == CONF_OK);
        TEST("Conf_get_bool true value", val != 0);

        /* false */
        TEST("Conf_get_bool false",
             Conf_get_bool(conf, "FalseVal", &val) == CONF_OK);
        TEST("Conf_get_bool false value", val == 0);

        /* missing key */
        TEST("Conf_get_bool missing",
             Conf_get_bool(conf, "Nonexistent", &val) == CONF_UNDEF);

        /* type mismatch: number */
        TEST("Conf_get_bool number",
             Conf_get_bool(conf, "NumVal", &val) == CONF_INVALID);

        /* type mismatch: string */
        TEST("Conf_get_bool string",
             Conf_get_bool(conf, "StrVal", &val) == CONF_INVALID);

        /* type mismatch: table */
        TEST("Conf_get_bool table",
             Conf_get_bool(conf, "TableVal", &val) == CONF_INVALID);

        /* intermediate not table */
        TEST("Conf_get_bool intermediate not table",
             Conf_get_bool(conf, "Deep.a.b.c.foo", &val) == CONF_INVALID);

        Conf_close(conf);
}

int
main(void)
{
        printf("=== Conf Library Tests ===\n\n");

        test_open();
        printf("  Conf_open tests: %d passed, %d failed\n", npassed, nfailed);
        npassed = nfailed = 0;

        test_close();
        printf("  Conf_close tests: %d passed, %d failed\n", npassed, nfailed);
        npassed = nfailed = 0;

        test_get_num();
        printf("  Conf_get_num tests: %d passed, %d failed\n", npassed, nfailed);
        npassed = nfailed = 0;

        test_get_int();
        printf("  Conf_get_int tests: %d passed, %d failed\n", npassed, nfailed);
        npassed = nfailed = 0;

        test_get_str();
        printf("  Conf_get_str tests: %d passed, %d failed\n", npassed, nfailed);
        npassed = nfailed = 0;

        test_get_bool();
        printf("  Conf_get_bool tests: %d passed, %d failed\n", npassed, nfailed);
        npassed = nfailed = 0;

        printf("\n");
        if (nfailed == 0)
                printf("ALL TESTS PASSED\n");
        else
                printf("SOME TESTS FAILED\n");

        return nfailed > 0 ? 1 : 0;
}
