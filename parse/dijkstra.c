#include <stdlib.h>
#include <stdio.h>
#define ABORT exit(1)
#define true 1
#define false 0

int schwa();
float _minus(float a, float b);
int _gcd(int a, int b);
int _factorial(int a);

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
    answer = _gcd(70,120);
    printf("answer 	 %d\n", answer);
    answer = _factorial(7);
    printf("answer 	 %d\n", answer);
    printf("pi 	 %f\n", pi);
}

float _minus(float a, float b)
{
    return a-b;
}

int _gcd(int a, int b)
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

int _factorial(int a)
{
    if ((1<=a)) {
        return _factorial(a-1)*a;
    } else if ((0==a)) {
        return 1;
    } else {
        ABORT;
    }
}
