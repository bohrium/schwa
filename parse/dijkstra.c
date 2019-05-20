#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define ABORT exit(1)
#define bool char
#define true 1
#define false 0
static int i, j;
static float r, cumulative;

int schwa();
float _minus(float a, float b);
int _gcd(int a, int b);
int _factorial(int a);

static float reward;

float uniform()
{
    return ((float)rand())/RAND_MAX;
}
float laplace()
{
    return log(uniform()) * (rand()%2 ? -1 : +1);
}
void initialize_weights()
{
}
float lrelu(float h)
{
        return (h<0 ? 0.2 * h : h);
}
float dlrelu(float h)
{
        return (h<0 ? 0.2 : 1.0);
}

int main()
{
    initialize_weights();
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
    printf("answer \t %d\n", answer);
    answer = _factorial(7);
    printf("answer \t %d\n", answer);
    printf("pi \t %f\n", pi);
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
