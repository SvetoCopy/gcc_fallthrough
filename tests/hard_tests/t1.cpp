// -O2

#include <stdio.h>
#include <stdlib.h>

struct A {
  int i;
  ~A() { }
};

A f (int a)
{
    if (a == 42) {
        A t = {};
        return t;
    }
    fputs("Oops\n", stderr);
}

int main(int argc, char **argv)
{
    f(argc);
}