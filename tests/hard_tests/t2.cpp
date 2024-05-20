// -O2

#include <stdio.h>
#include <stdlib.h>

class IncompleteType;

IncompleteType* f(int a) {
    if (a == 42) {
        return nullptr;
    }

    fputs("Oops\n", stderr);
}

int main(int argc, char **argv)
{
    f(argc);
}