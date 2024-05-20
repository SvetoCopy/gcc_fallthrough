// -ftrivial-auto-var-init=zero -O1
// -ftrivial-auto-var-init=zero -O2
// -ftrivial-auto-var-init=zero -O3
// -ftrivial-auto-var-init=zero -O0

#include <stdio.h>
#include <stdlib.h>

template <class T>
T f(T a) {

    if (a == NULL)
        return a;

    fputs("Oops\n", stderr);
}

int main(int argc, char **argv)
{
    int res = f<int>(argc);
    
    return res;
}
