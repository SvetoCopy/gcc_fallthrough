// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include <stdio.h>
#include <stdlib.h>

class Test {
public:
    virtual int f(int a) {
        if (a == 42)
            return a;

        fputs("Oops1\n", stderr);
    }
};

class Test_child : public Test {
public:
    int f(int a) override {
        if (a == 42)
            return a;

        fputs("Oops2\n", stderr);
    }
};

int main(){
    Test_child a = {};
    a.f(3);
}