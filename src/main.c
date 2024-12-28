#include "bigint.h"
#include "tests.h"

#include <string.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>

extern uint64_t BASE;

int main(int argc, char **argv) {
    (void)(argc); (void)(argv);

    int u = 1000;
    bigint_add_test_low(u);
    bigint_sub_test_low(u);
    bigint_tmul_test_low(u); 
    bigint_tdiv_test_low(u);  
    bigint_mod_test_low(u);  

    return 0;
}
