// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include "stdio.h"

struct A {
    int a;
};

A test6_main(int b) {
    if (b == 3) {

        A t = {};
        t.a = 3;

        return t;
    }
} 

A test6_help() {

    A t = {};
    t.a = 3;

    return t;
}

int main() {
    A res = test6_main(2);

    if (res.a == 3) printf("TEST_6 FAILED");
    else            printf("TEST_6 PASSED");
} 
