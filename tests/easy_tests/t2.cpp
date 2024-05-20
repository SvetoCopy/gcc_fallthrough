// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include "stdio.h"

float test2_main(int b) {
    if (b == 3) {
        return 2;
    }
} 

float test2_help() {
    return 1;
}

int main() {
    float res = test2_main(3);
    if (res == 1) printf("TEST_2 FAILED");
    else          printf("TEST_2 PASSED");
} 
