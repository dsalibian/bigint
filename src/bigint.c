#include "bigint.h"
#include <assert.h>
#include <limits.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

const uint NEGATIVE = 1;
const uint BASE = 10;

bigint new_bigint(int64 n) {
    bigint bint;
    bint.sign = n < 0;
    n = llabs(n);

    uint digit_count = 0;
    
    int64 n_copy = n;
    do {
        ++digit_count;
        n_copy /= BASE;
    } while ( n_copy );

    bint.size   = digit_count;
    bint.msize  = digit_count;
    bint.digits = malloc(digit_count * sizeof(uint));

    for(uint i = 0; i < digit_count; ++i) { 
        bint.digits[i] = (uint)(n % BASE);
        n /= BASE;
    }

    return bint;
}

void free_bigint(bigint* bint) {
    free(bint->digits);
    free(bint);
}

int64 bigint_to_int64(bigint* bint) {
    int64 sum = 0;
    int64 k = 1;

    uint u = bint->size;
    for(uint i = 0; i < u; ++i) {
        sum += bint->digits[i] * k;
        k *= BASE;
    }

    return bint->sign ? -sum : sum;
}

void print_bigint(bigint* bint) {
    if( bint->sign ) printf("-");    
    print_digits(bint->digits, bint->size);
}

void print_digits(uint* arr, uint arr_size) {
    for(int i = (int)arr_size - 1; i >= 0; --i) {
        uint d = arr[i];
        if( BASE <= 10 ) printf("%u", d);
        else if( BASE <= 36) printf("%c", d < 10 ? d+48 : d-10+65);
        else printf("%u ", d);
    }
    printf("\n");
}

int cmp(bigint* a, bigint* b) {
    if( a->sign != b->sign ) return a->sign ? -1 : 1;

    if( a->size > b->size ) return a->sign ? -1 :  1;
    if( a->size < b->size ) return a->sign ?  1 : -1;

    assert(a->size < INT_MAX);
    for(int i = (int)a->size - 1; i >= 0; --i) {
        if( a->digits[i] > b->digits[i] ) return a->sign ? -1 :  1;
        if( a->digits[i] < b->digits[i] ) return a->sign ?  1 : -1;
    }

    return 0;
}

int cmp_abs(uint* a, uint a_size, uint* b, uint b_size) {
    if( a_size != b_size ) return a_size > b_size ? 1 : -1;

    assert(a_size < INT_MAX);
    for(int i = (int)a_size - 1; i >= 0; --i) {
        if( a[i] > b[i] ) return  1;
        if( a[i] < b[i] ) return -1;
    }

    return 0;
}

uint lzc(uint* arr, uint arr_size) {
    uint i = arr_size;
    while( --i && !arr[i] );
    return arr_size - 1 - i;
}

bigint add(bigint* a, bigint* b) {
    bigint result;
    uint u = MAX(a->size, b->size);

    if( a->sign != b->sign ) {
        uint at = a->sign;
        uint bt = b->sign;

        a->sign = b->sign = !NEGATIVE;
        bigint t = at ? sub(b, a) : sub(a, b);

        a->sign = at;
        b->sign = bt;
        return t;
    }
    else {
        result.msize = u + 1;
        result.digits = calloc(u+1, sizeof(uint));
        result.sign = a->sign;

        add_digits(a->digits, a->size, b->digits, b->size, result.digits, &result.size);
    }

    return result;
}

void add_digits(uint* a, uint a_size, uint* b, uint b_size, uint* result, uint* result_size) {
    uint u = MAX(a_size, b_size);
    if( result_size ) 
        *result_size = u;

    uint64 carry = 0;
    uint i = 0;
    for( ; i < u; ++i) {
        carry += (i < a_size ? (uint64)a[i] : 0) + (i < b_size ? (uint64)b[i] : 0);
        result[i] = (uint)(carry % BASE);
        carry /= BASE;
    }

    assert(carry < UINT_MAX);
    if( carry ) result[result_size ? (*result_size)++ : i] = (uint)carry;
}

void inc(uint** n, uint* n_size, uint* n_msize) {
    uint64 carry = 1;
    for(uint i = 0; i < *n_size && carry; ++i) {
        carry += (*n)[i];
        (*n)[i] = (uint)(carry % BASE);
        carry /= BASE;
    }
    if( carry ) {
        if( *n_size == *n_msize ) *n = realloc(*n, ++*n_msize * sizeof(uint));
        assert(carry < UINT_MAX);
        (*n)[n_size == n_msize ? *n_msize - 1 : (*n_size)++] = (uint)carry;
    }
}

bigint sub(bigint* a, bigint* b) {
    // a - (-b) = a+b
    // (-a) - b = -(a + b)
    if( a->sign - b->sign ) {
        if( !a->sign ) {
            uint at = a->sign;
            uint bt = b->sign;

            a->sign = b->sign = !NEGATIVE;
            bigint t = add(a, b);

            a->sign = at;
            b->sign = bt;
            t.sign = !NEGATIVE;

            return t;
        }
        else {
            uint at = a->sign;
            uint bt = b->sign;

            a->sign = b->sign = !NEGATIVE;
            bigint t = add(a, b);

            a->sign = at;
            b->sign = bt;
            t.sign = NEGATIVE;

            return t;
        }
    }  
    // a - b 
    // (-a) - (-b) = b - a;

    bigint result;
    uint u = MAX(a->size, b->size);    
    result.msize = u;
    result.size = u;

    int k = cmp_abs(a->digits, a->size, b->digits, b->size);

    result.sign = k != -1 ? NEGATIVE : !NEGATIVE;
    if( !a->sign ) result.sign = !result.sign;

    result.digits = calloc(u, sizeof(uint));

    if(k != -1)
        sub_digits(a->digits, a->size, b->digits, b->size, result.digits, &result.size);
    else
        sub_digits(b->digits, b->size, a->digits, a->size, result.digits, &result.size);

    return result;     
}

void sub_digits(uint* a, uint a_size, uint* b, uint b_size, uint* result, uint* result_size) {
    uint borrow_flag = 0;
    uint modified_flag = 0;
    for(uint i = 0; i < a_size; ++i) {
        if( borrow_flag ) {
            if( a[i] ) {
                borrow_flag = 0;
                modified_flag = 1;
                --a[i--];
            } else 
                result[i] = BASE - 1 - (i < b_size ? b[i] : 0);
            continue;
        }

        if( a[i] < (i < b_size ? b[i] : 0) ) {
            borrow_flag = 1;
            result[i] = (uint)((uint64)a[i] + BASE - b[i]);
        } else 
            result[i] = a[i] - (i < b_size ? b[i] : 0);

        if( modified_flag ) {
            modified_flag = 0;
            ++a[i];
        }
    }
    
    if( result_size ) 
        *result_size -= lzc(result, *result_size);
    
}

bigint mult_trad(bigint* a, bigint* b) {
    bigint result;

    uint u = MAX(a->size, b->size);
    result.size = result.msize = u * 2 + 1;
    result.sign = a->sign ^ b->sign;
    result.digits = calloc(result.msize, sizeof(uint));

    mult_digits_trad(a->digits, a->size, b->digits, b->size, result.digits, &result.size);

    return result;
}

void mult_digits_trad(uint* a, uint a_size, uint* b, uint b_size, uint* result, uint* result_size) {
    assert(a_size < INT_MAX && b_size < INT_MAX);
    for(uint i = 0; i < a_size; ++i) {
        uint64 carry = 0;
        uint64 adder_carry = 0;
        for(uint j = 0; j < b_size; ++j) {
            uint64 p = (uint64)a[i] * (uint64)b[j];
            uint64 t = ((p % BASE) + (carry % BASE)) % BASE; 
            carry = (uint64)(((double)carry + (double)p) / BASE);

            adder_carry += t + result[i+j];
            result[j+i] = (uint)(adder_carry % BASE);
            adder_carry /= BASE;
        }
        assert(carry + adder_carry < UINT_MAX);
        result[b_size + i] += (uint)(carry + adder_carry);
    } 
    
    if( result_size ) 
        *result_size -= lzc(result, *result_size);
}

bigint div_trad(bigint* a, bigint* b) {
    (void)(a);
    (void)(b);
    return new_bigint(0);
}
void div_digits_trad(uint* a, uint a_size, uint* b, uint b_size, uint* result, uint* result_size) {
    (void)(a);
    (void)(a_size);
    (void)(b);
    (void)(b_size);
    (void)(result);
    (void)(result_size);
}
