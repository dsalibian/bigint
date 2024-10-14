#include "tests.h"
#include "bigint.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

extern uint BASE;

void test(int u, bigint(*op)(bigint*, bigint*), int64(*ans)(int64, int64)) {
    if( BASE > 16 ) printf("WARNING: LARGE BASE\n\n");
    for(int i = -u; i <= u; ++i) {
        bigint a = new_bigint(i);
        for(int j = -u; j <= u; ++j) {
            bigint b = new_bigint(j);
            bigint c = op(&a, &b);

            int64 n = bigint_to_int64(&c);
            int64 m = ans(i, j);
            if( n - m ) {
                printf("a: %d \nb: %d\n\nbigint: %lld \nans: %lld\n\n",i, j, n, m);
                abort();
            }

            free(b.digits);
            free(c.digits);
        }
        free(a.digits);
    }
}

void add_test(int u)   { test(u, add, _add); }
void sub_test(int u)   { test(u, sub, _sub); }
void tmult_test(int u) { test(u, mult_trad, _mul); }

int64 _add(int64 a, int64 b) { return a + b; }
int64 _sub(int64 a, int64 b) { return a - b; }
int64 _mul(int64 a, int64 b) { return a * b; }
