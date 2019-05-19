#include <stdlib.h>
#include <stdio.h>
#define ABORT exit(1)
#define true 1
#define false 0

int schwa();
float minus(float a, float b);
int gcd(int a, int b);

int main()
{
    printf("\033[33m");
    schwa();
    printf("\033[37m");
}

int schwa()
{
    float pi;
    pi = 3.1415926;
    int answer;
    answer = gcd(70,120);
    printf("answer 	 %d\n", answer);
    printf("pi 	 %f\n", pi);
}

float minus(float a, float b)
{
    return a-b;
}

int gcd(int a, int b)
{
    while (true) {
        if ((!(a<=b))) {
            a = a-b;
        } else if ((a<b)) {
            b = b-a;
        } else {
            break;
        }
    }
    return a;
}