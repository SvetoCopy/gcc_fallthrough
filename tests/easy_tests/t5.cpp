// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include "stdio.h"

int* test5_main(int b) {
    if (b == 3) {
        int a = 5;
        return &a;
    }
} 

int* test5_help() {
    int a = 5;
    return &a;
}

int main() {
    int* res = test5_main(3);
    if (res == nullptr) printf("TEST_5 PASSED");
    else                printf("TEST_5 FAILED");
} 
