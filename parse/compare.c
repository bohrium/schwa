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
int _compare(float a, float b);

static float input0[3];
static float weight_u0[4][3];
static float active_h0[4];       static float dlossd_h0[4];
static float active_z0[4];       static float dlossd_z0[4];
static float weight_v0[2][4];
static float active_hh0[2];      static float dlossd_hh0[2];
static float active_exphh0[2];
static float partition0;
static int sample0;
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
    for (int i=0; i!=4; ++i) {
        for (int j=0; j!=3; ++j) {
            weight_u0[i][j] = 0.1 * laplace();
        }
    }
    for (int i=0; i!=2; ++i) {
        for (int j=0; j!=4; ++j) {
            weight_v0[i][j] = 0.1 * laplace();
        }
    }
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
    float moo;
    float too;
    moo = -5.0;
    too = -6.0;
    int c;
    c = _compare(moo,too);
    reward = (5.0 + c);
    printf("c \t %d\n", c);
}

int _compare(float a, float b)
{
    input0[0] = a;
    input0[1] = b;
    input0[2] = 1.0;
    for (i=0; i!=4; ++i) {
        active_h0[i] = 0.0;
        for (j=0; j!=3; ++j) {
            active_h0[i] += weight_u0[i][j] * input0[j];
        }
        active_z0[i] = lrelu(active_h0[i]);
    }
    partition0 = 0.0;
    for (i=0; i!=2; ++i) {
        active_hh0[i] = 0.0;
        for (j=0; j!=4; ++j) {
            active_hh0[i] += weight_v0[i][j] * active_z0[j];
        }
        active_exphh0[i] = exp(active_hh0[i]);
        partition0 += active_exphh0[i];
    }
    r = uniform();
    cumulative = 0.0;
    for (i=0; i!=2; ++i) {
        cumulative += active_exphh0[i] / partition0;
        if (cumulative >= r) { break; }
    }
    sample0 = i;
    switch (sample0) {
        case 0: {
            return 0;
        } break;
        case 1: {
            return 1;
        } break;
    }
}
