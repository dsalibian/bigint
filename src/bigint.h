#ifndef BIGINT_H
#define BIGINT_H

#include <stdint.h>
#include <stddef.h>


typedef struct biginteger {
    uint64_t* digs;
    size_t len, mlen;
    int sign;
} bigint;

bigint bigint_new(int64_t);
void bigint_free(bigint*);
void bigint_push_back(bigint*, uint64_t);
void bigint_resize(bigint*, size_t);

int64_t bigint_to_int64(bigint*);
const char* bigint_tostr(bigint*, uint64_t);

int bigint_cmp_abs(uint64_t*, size_t, uint64_t*, size_t);
int bigint_cmp(bigint*, bigint*);
size_t bigint_clz(uint64_t*, size_t);

bigint bigint_add(bigint*, bigint*);
bigint bigint_sub(bigint*, bigint*);
bigint bigint_tmul(bigint*, bigint*);
bigint bigint_tdiv(bigint*, bigint*);
bigint bigint_mod(bigint*, bigint*);

#endif
