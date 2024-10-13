#ifndef BIGINT_H
#define BIGINT_H

typedef unsigned int uint;
typedef long long int64;
typedef unsigned long long uint64;


typedef struct bigint {
    uint* digits;
    uint sign;
    uint size;
    uint msize;
} bigint;

bigint* new_bigint(int64);
void free_bigint(bigint*);

int64 bigint_to_int64(bigint*);
void print_bigint(bigint*);
void print_digits(uint*, uint);

int cmp(bigint*, bigint*);
int cmp_abs(uint*, uint, uint*, uint);
uint lzc(uint*, uint);

bigint* add(bigint*, bigint*);
void add_digits(uint*, uint, uint*, uint, uint*, uint*);
void inc(uint**, uint*, uint*);

bigint* sub(bigint*, bigint*);
void sub_digits(uint*, uint, uint*, uint, uint*, uint*);

bigint* mult_trad(bigint*, bigint*);
void mult_digits_trad(uint*, uint, uint*, uint, uint*, uint*);

bigint* div_trad(bigint*, bigint*);
void div_digits_trad(uint*, uint, uint*, uint, uint*, uint*);

#endif
