// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include "stdio.h"

char test4_main(int b) {
    if (b == 3) {
        return 2;
    }
} 

char test4_help() {
    return 1;
}

int main() {
    char res = test4_main(3);
    if (res == 1) printf("TEST_4 FAILED");
    else          printf("TEST_4 PASSED");
} 
