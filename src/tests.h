#ifndef TESTS_H
#define TESTS_H

#include "bigint.h"

void test(int, bigint*(*)(bigint*, bigint*), int64(*)(int64, int64));

void add_test(int);
void sub_test(int);
void tmult_test(int);

int64 _add(int64, int64);
int64 _sub(int64, int64);
int64 _mul(int64, int64);

#endif
