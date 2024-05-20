// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include "stdio.h"

int test1_main(int b) {
    if (b == 3) {
        return 2;
    }
} 

int test1_help() {
    return 1;
}

int main() {
    int res = test1_main(3);
    if (res == 1) printf("TEST_1 FAILED");
    else          printf("TEST_1 PASSED");
} 
