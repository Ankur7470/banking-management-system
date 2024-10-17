#include <stdio.h>
#include "temp.h"
#include "temp1.h"

int main(){
    int a = 10, b = 20;

    printf("%d\n", add(a,b));
    printf("%d\n", subtract(a,b));
    printf("%d", mult(a,b));

    return 0;
}