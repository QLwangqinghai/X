//
//  main.c
//  Test
//
//  Created by vector on 2020/6/10.
//  Copyright © 2020 haoqi. All rights reserved.
//

#include <stdio.h>
#include <math.h>

/*
 18446744073709551557
 18446744073709551533
 
 
 4294967291
 */


static int isPrime(uint64_t n) {
    if (n <= 3) {
        return n > 1;
    }
    __uint128_t limit = sqrtl((long double)n) + 1;
    while (limit * limit < n) {
        limit += 1;
    }

    for(uint64_t i = 2; i < limit; i++){
        if (n % i == 0) {
            return 0;
        }
    }
    return 1;
}


int main(int argc, const char * argv[]) {
    // insert code here...
    printf("Hello, World!\n");
    
    
    for (uint64_t i=UINT32_MAX; i>=0; i--) {
        
        if (isPrime(i) == 1) {
            printf("yes: %llu\n", i);
        } else {
            printf("no: %llu\n", i);
        }
    }
    
    
    return 0;
}
