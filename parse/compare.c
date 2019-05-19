#include <stdlib.h>
#include <stdio.h>
#define ABORT exit(1)
#define bool char
#define true 1
#define false 0

int schwa();
int _compare(float a, float b);

int main()
{
    printf("\033[33m");
    schwa();
    printf("\033[37m");
}

int schwa()
{
}

int _compare(float a, float b)
{
    return 0;
}
