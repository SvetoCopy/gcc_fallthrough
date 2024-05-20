// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0
#include "stdio.h"

double test3_main(int b) {
    if (b == 3) {
        return 2;
    }
} 

double test3_help() {
    return 1;
}

int main() {
    double res = test3_main(3);
    if (res == 1) printf("TEST_3 FAILED");
    else          printf("TEST_3 PASSED");
} 
