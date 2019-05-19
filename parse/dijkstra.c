#include <stdlib.h>
#include <stdio.h>
#define ABORT exit(1)
#define true 1
#define false 0

int schwa();
float schwa_0_minus(float a, float b);
int schwa_0_gcd(int a, int b);

int main()
{
    schwa();
}

int schwa()
{
    float pi;
    pi = 3.141;
    int hi;
    hi = schwa_0_gcd(70,120);
    printf("%d\n", hi);
}

float schwa_0_minus(float a, float b)
{
    return a-b;
}

int schwa_0_gcd(int a, int b)
{
    while (true) {
        if (a > b) {
            a = a-b;
        } else if (a < b) {
            b = b-a;
        } else {
            break;
        }
    }
    return a;
}
