#ifndef TESTS_H
#define TESTS_H

#include "bigint.h"

void test_low(int, bigint(*)(bigint*, bigint*), int64_t(*)(int64_t, int64_t));

int64_t bigint_add_tester(int64_t, int64_t);
int64_t bigint_sub_tester(int64_t, int64_t);
int64_t bigint_mul_tester(int64_t, int64_t);
int64_t bigint_div_tester(int64_t, int64_t);
int64_t bigint_mod_tester(int64_t, int64_t);

void bigint_add_test_low(int);
void bigint_sub_test_low(int);
void bigint_tmul_test_low(int);
void bigint_tdiv_test_low(int);
void bigint_mod_test_low(int);

#endif
