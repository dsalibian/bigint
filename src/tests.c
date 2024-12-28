#include "tests.h"
#include "bigint.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

extern uint64_t BASE;

void test_low(int u, bigint(*op)(bigint*, bigint*), int64_t(*op_test)(int64_t, int64_t)) {
    if( BASE > 16 ) printf("WARNING: LARGE BASE\n\n");
    for(int i = -u; i <= u; ++i) {
        bigint a = bigint_new(i);
        for(int j = -u; j <= u; ++j) {
            if((op_test == bigint_div_tester || op_test == bigint_mod_tester) && !j)
                continue;

            bigint b = bigint_new(j);
            bigint c = op(&a, &b);

            int64_t n = bigint_to_int64(&c);
            int64_t m = op_test(i, j);

            if( n - m ) {
                typedef unsigned long long ull;
                printf("a: %d \nb: %d\n\nbigint: %lld \nans: %lld\n\n",
                        i, j, (ull)n, (ull)m);
                abort();
            }

            bigint_free(&b);
            bigint_free(&c);
        }
        bigint_free(&a);
    }
}

int64_t bigint_add_tester(int64_t a, int64_t b) { return a + b; }
int64_t bigint_sub_tester(int64_t a, int64_t b) { return a - b; }
int64_t bigint_mul_tester(int64_t a, int64_t b) { return a * b; }
int64_t bigint_div_tester(int64_t a, int64_t b) { return a / b; }
int64_t bigint_mod_tester(int64_t a, int64_t b) { return a % b; }

void bigint_add_test_low(int u)  { test_low(u, bigint_add, bigint_add_tester); }
void bigint_sub_test_low(int u)  { test_low(u, bigint_sub, bigint_sub_tester); }
void bigint_tmul_test_low(int u) { test_low(u, bigint_tmul, bigint_mul_tester); } 
void bigint_tdiv_test_low(int u) { test_low(u, bigint_tdiv, bigint_div_tester); }  
void bigint_mod_test_low(int u) { test_low(u, bigint_mod, bigint_mod_tester); }  
