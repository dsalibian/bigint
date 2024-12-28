#include "bigint.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

const uint64_t BASE = 10;

#define MAX(a, b) ((a) > (b) ? (a) : (b));
#define MIN(a, b) ((a) < (b) ? (a) : (b));

bigint bigint_new(int64_t n) {
    bigint bint;
    bint.sign = n < 0;
    bint.len = 0;
    bint.mlen = 1;
    bint.digs = malloc(sizeof(uint64_t));

    uint64_t n_abs = (uint64_t)llabs(n);
    do {
        bigint_push_back(&bint, n_abs % BASE);
        n_abs /= BASE;
    } while( n_abs );

    return bint;
}

void bigint_free(bigint* bint) {
    free(bint->digs);
}

void bigint_resize(bigint* bint, size_t size) {
    bint->mlen = size < bint->mlen ? (bint->len = size) : size;
    bint->digs = realloc(bint->digs, bint->mlen * sizeof(uint64_t));
}

void bigint_push_back(bigint* bint, uint64_t d) {
    if( bint->len == bint->mlen )
        bigint_resize(bint, 2 * bint->mlen);
    
    bint->digs[(bint->len)++] = d;
}


const char* bigint_tostr(bigint* arr, uint64_t base) {
    (void)(arr); (void)(base);

    return NULL;    
}

int64_t bigint_to_int64(bigint* bint) {
    uint64_t sum = 0;
    uint64_t k = 1;

    for(size_t i = 0; i < bint->len; ++i) {
        sum += bint->digs[i] * k;
        k *= BASE;
    }

    return bint->sign ? -(int64_t)sum : (int64_t)sum;
}

int bigint_cmp_abs(uint64_t* a, size_t a_len, uint64_t* b, size_t b_len) {
    if( a_len != b_len ) 
        return a_len > b_len ? 1 : -1;

    for(size_t i = a_len - 1; i < a_len; --i) {
        if(a[i] > b[i]) return  1;
        if(a[i] < b[i]) return -1;
    }

    return 0;
}

int bigint_cmp(bigint* a, bigint* b) {
    if( a->sign - b->sign )  
        return a->sign ? -1 : 1;

    int t = bigint_cmp_abs(a->digs, a->len, b->digs, b->len);
    return
        t ==  1 ? (a->sign ? -1 :  1) :
        t == -1 ? (a->sign ?  1 : -1) : 
        0;
}

size_t bigint_clz(uint64_t* arr, size_t len) {
    size_t i = len;
    while( --i && !arr[i] );
    return len - 1 - i;
}

void bigint_adder(uint64_t* a, size_t a_len, uint64_t* b, size_t b_len, uint64_t* result, size_t* result_len) {
    size_t u = MAX(a_len, b_len);
    if(result_len) 
        *result_len = u;

    __uint128_t carry = 0;
    size_t i = 0;
    for( ; i < u; ++i) {
        carry += (i < a_len ? (__uint128_t)a[i] : 0) + (i < b_len ? (__uint128_t)b[i] : 0);
        result[i] = (uint64_t)(carry % BASE);
        carry /= BASE;
    }

    if(carry) 
        result[result_len ? (*result_len)++ : i] = (uint64_t)carry;

    if(result_len) 
        *result_len -= bigint_clz(result, *result_len);
}

void bigint_incer(uint64_t* a, size_t* a_len, size_t* a_mlen) {
    uint64_t one[1] = {1};
    if(*a_len == *a_mlen) {
        *a_mlen *= 2;
        a = realloc(a, *a_mlen * sizeof(uint64_t));
    }

    bigint_adder(a, *a_len, one, 1, a, a_len);
}

void bigint_suber(uint64_t* a, size_t a_len, uint64_t* b, size_t b_len, uint64_t* result, size_t* result_len) {
    int borrow_flag = 0;
    int modified_flag = 0;
    for(size_t i = 0; i < a_len; ++i) {
        if( borrow_flag ) {
            if( a[i] ) {
                borrow_flag = 0;
                modified_flag = 1;
                --a[i--];
            } else 
                result[i] = BASE - 1 - (i < b_len ? b[i] : 0);
            continue;
        }

        if( a[i] < (i < b_len ? b[i] : 0) ) {
            borrow_flag = 1;
            result[i] = (uint64_t)((__uint128_t)a[i] + BASE - b[i]);
        } else 
            result[i] = a[i] - (i < b_len ? b[i] : 0);

        if( modified_flag ) {
            modified_flag = 0;
            ++a[i];
        }
    }

    if(result_len) 
        *result_len -= bigint_clz(result, *result_len);
}

void bigint_tmuler(uint64_t* a, size_t a_len, uint64_t* b, size_t b_len, uint64_t* result, size_t* result_len) {
    for(size_t i = 0; i < a_len; ++i) {
        __uint128_t carry = 0;
        __uint128_t adder_carry = 0;
        for(size_t j = 0; j < b_len; ++j) {
            __uint128_t p = (__uint128_t)a[i] * b[j];
            __uint128_t t = ((p % BASE) + (carry % BASE)) % BASE; 
            carry = (carry / BASE) + (p / BASE) + ( ((carry % BASE) + (p % BASE)) / BASE );
            
            adder_carry += t + result[i+j];
            result[j+i] = (uint64_t)(adder_carry % BASE);
            adder_carry /= BASE;
        }

        result[b_len + i] += (uint64_t)(carry + adder_carry);
    } 
    
    if( result_len ) 
        *result_len -= bigint_clz(result, *result_len);
}

uint64_t bigint_tdiver_div_rs(uint64_t* a, size_t a_len, uint64_t* b, size_t b_len) {
    uint64_t* a_copy = malloc(a_len * sizeof(uint64_t));
    memcpy(a_copy, a, a_len * sizeof(uint64_t));
    a = a_copy;
    a_copy = calloc(a_len, sizeof(uint64_t));

    size_t a_olen = a_len;
    uint64_t count = 0;
    for(; bigint_cmp_abs(a, a_len, b, b_len) > -1; ++count) {
        bigint_suber(a, a_len, b, b_len, a_copy, &a_len);

        memcpy(a, a_copy, a_olen * sizeof(uint64_t));
        memset(a_copy, 0, a_olen * sizeof(uint64_t));
    }

    free(a);
    free(a_copy);
    return count;
}

void bigint_tdiver(uint64_t* a, size_t a_len, uint64_t* b, size_t b_len, uint64_t* result) {
    if(b_len > a_len)
        return;

    size_t k = b_len + (bigint_cmp_abs(b, b_len, &a[a_len - b_len], b_len) == 1);

    if(a_len - k > a_len)
        return;

    const size_t mlen = a_len;
    size_t ln_len, prod_len, diff_len;
    ln_len = prod_len = diff_len = a_len;

    uint64_t* ptr = calloc(3 * mlen,  sizeof(uint64_t));
    uint64_t* ln = ptr;
    uint64_t* prod = &ptr[mlen];
    uint64_t* diff = &ptr[mlen * 2];

    result[a_len - k] = bigint_tdiver_div_rs(&a[a_len - k], k, b, b_len);
    bigint_tmuler(&result[a_len - k], 1, b, b_len, prod, &prod_len);
    bigint_suber(&a[a_len - k], k, prod, prod_len, diff, &diff_len);

    memcpy(ln, diff, mlen * sizeof(uint64_t));
    ln_len = diff_len;

    memset(prod, 0, mlen * 2 * sizeof(uint64_t));
    prod_len = diff_len = mlen;

    for(; ++k <= a_len;) {
        memmove(&ln[1], ln, (mlen - 1) * sizeof(uint64_t));
        ln[0] = a[a_len - k];
        ++ln_len;

        ln_len = mlen - bigint_clz(ln, mlen);

        result[a_len - k] = bigint_tdiver_div_rs(ln, ln_len, b, b_len);
        bigint_tmuler(&result[a_len - k], 1, b, b_len, prod, &prod_len);
        bigint_suber(ln, ln_len, prod, prod_len, diff, &diff_len);

        memcpy(ln, diff, mlen * sizeof(uint64_t)); 
        memset(prod, 0, 2 * mlen * sizeof(uint64_t));
        ln_len = diff_len;
        diff_len = prod_len = mlen;
    }

    free(ptr);
}

bigint bigint_add(bigint* a, bigint* b) {
    bigint result;

    if(a->sign - b->sign) {
        int a_sign = a->sign;
        int b_sign = b->sign;

        a->sign = b->sign = 0;
        result = a_sign ? bigint_sub(b, a) : bigint_sub(a, b);

        a->sign = a_sign;
        b->sign = b_sign;
    } else {
        result.mlen = result.len = 1 + MAX(a->len, b->len);
        result.digs = calloc(result.mlen, sizeof(uint64_t));
        result.sign = a->sign;

        bigint_adder(a->digs, a->len, b->digs, b->len, result.digs, &result.len);
    }

    return result;
}

bigint bigint_sub(bigint* a, bigint* b) {
    bigint result;

    if(a->sign - b->sign) {
        int a_sign = a->sign;
        int b_sign = b->sign;

        a->sign = b->sign = 0;
        result = bigint_add(a, b);
        result.sign = a_sign ? 1 : 0;

        a->sign = a_sign;
        b->sign = b_sign;

    } else {
        result.mlen = result.len = 1 + MAX(a->len, b->len);
        result.digs = calloc(result.mlen, sizeof(uint64_t));

        int k = bigint_cmp_abs(a->digs, a->len, b->digs, b->len);
        if(k > -1) {
            result.sign = a->sign ? 1 : 0;
            bigint_suber(a->digs, a->len, b->digs, b->len, result.digs, &result.len);
        } else {
            result.sign = a->sign ? 0 : 1;
            bigint_suber(b->digs, b->len, a->digs, a->len, result.digs, &result.len);
        }
    }

    return result;
}

bigint bigint_tmul(bigint* a, bigint* b) {
    bigint result;
    result.sign = a->sign ^ b->sign;
    result.len = result.mlen = a->len + b->len;
    result.digs = calloc(result.mlen, sizeof(uint64_t));

    bigint_tmuler(a->digs, a->len, b->digs, b->len, result.digs, &result.len);

    return result;
}

bigint bigint_tdiv(bigint* a, bigint* b) {
    bigint result;
    result.sign = a->sign ^ b->sign;
    result.len = result.mlen = 1 + MAX(a->len, b->len);
    result.digs = calloc(result.mlen, sizeof(uint64_t));

    bigint_tdiver(a->digs, a->len, b->digs, b->len, result.digs);
    result.len -= bigint_clz(result.digs, result.mlen);

    return result;
}

bigint bigint_mod(bigint* a, bigint* b) {
    bigint div = bigint_tdiv(a, b);
    bigint prod = bigint_tmul(b, &div);

    bigint result = bigint_sub(a, &prod);

    bigint_free(&div);
    bigint_free(&prod);
    return result;
}
