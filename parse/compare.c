// author: samtenka
// change: /*DATE*/ 
// create: 2019-05-20 
// descrp: /*DESCRIPTION*/

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#define LRELU_LEAK 0.2
#define WEIGHT_INIT_SCALE 0.1
#define LEARNING_RATE 0.01
#define HISTORY_CAPACITY 100 
#define BASELINE_AVG_TIMESCALE 10 

/*****************************************************************************/
/*  0. HELPERS (DECLARATION)                                                 */
/*****************************************************************************/
#define ABORT exit(1)
float uniform();
float laplace();



/*****************************************************************************/
/*  1. GLOBALS                                                               */
/*****************************************************************************/
static int i, j, k;
static float randval, cumulative;
static float reward, baseline, signal;



/*****************************************************************************/
/*  2. NN HELPERS (DECLARATION)                                              */
/*****************************************************************************/
float lrelu(float h);
float dlrelu(float h);
void initialize_weights();



/*****************************************************************************/
/*  3. NN GLOBALS                                                            */
/*****************************************************************************/
static float input0[3];
static float weight_u0[4][3];    
static float active_h0[4];       static float dlossd_h0[4];
static float active_z0[4];       static float dlossd_z0[4];
static float weight_v0[2][4];    
static float active_hh0[2];      static float dlossd_hh0[2];
static float active_exphh0[2];   
static float partition0;
static int   sample0;

static float input_history0[HISTORY_CAPACITY][3-1];
static int   sample_history0[HISTORY_CAPACITY];
static int   history_len0;




/*****************************************************************************/
/*  4. USER FUNCTIONS (DECLARATION)                                          */
/*****************************************************************************/

int _main();
int _compare(float a, float b);




/*****************************************************************************/
/*  5. MAIN                                                                  */
/*****************************************************************************/
int main()
{
    initialize_weights();
    printf("\033[33m");
    baseline = 0.0;
    _main();
    printf("\033[37m");
}



/*****************************************************************************/
/*  6. HELPERS (IMPLEMENTATION)                                              */
/*****************************************************************************/
float uniform()
{
    return ((float)rand())/RAND_MAX;
}
float laplace()
{
    return log(uniform()) * (rand()%2 ? -1 : +1);
}
float lrelu(float h)
{
    return (h<0 ? LRELU_LEAK * h : h);
}
float dlrelu(float h)
{
    return (h<0 ? LRELU_LEAK : 1.0);
} 



/*****************************************************************************/
/*  7. NN HELPERS (IMPLEMENTATION)                                           */
/*****************************************************************************/
void initialize_weights()
{
    for (int i=0; i!=4; ++i) {
    for (int j=0; j!=3; ++j) {
        weight_u0[i][j] = WEIGHT_INIT_SCALE * laplace();
    }
}
for (int i=0; i!=2; ++i) {
    for (int j=0; j!=4; ++j) {
        weight_v0[i][j] = WEIGHT_INIT_SCALE * laplace();
    }
}

}

/*****************************************************************************/
/*  8. USER FUNCTIONS (IMPLEMENTATION)                                       */
/*****************************************************************************/

int _main()
{
    float _aa;
    float _bb;
    int _c;
    int _i;
    float _r;
    _i = 0;
    while (true) {
        if ((_i!=100)) {
            _aa = 10.0;
            _bb = -10.0;
            _c = _compare(_aa,_bb);
            if ((((_c==0)))&&(((_aa<=_bb)))) {
                _r = (1.0);
            } else if ((((_c==1)))&&(((_aa>=_bb)))) {
                _r = (1.0);
            } else if ((((_c==0)))&&(((_aa>=_bb)))) {
                _r = (-1.0);
            } else if ((((_c==1)))&&(((_aa<=_bb)))) {
                _r = (-1.0);
            } else {
                ABORT;
            }
            printf("r \t %f\n", _r);
            reward = (_r);
            if (history_len0 != 0) {
                for (k=0; k!= history_len0; ++k) {
                    // forward pass from history:
                    for (i=0; i!=3-1; ++i) {
                        input0[i] = input_history0[k][i];
                    }
                    input0[3-1] = 1.0;
                    // forward pass:
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
                        partition0+= active_exphh0[i];
                    }
                    // sample based on distribution:
                    randval = uniform();
                    cumulative = 0.0;
                    for (i=0; i!=2; ++i) {
                        cumulative += active_exphh0[i] / partition0;
                        if (cumulative >= randval) { break; }
                    }
                    sample0 = i;
                    sample0 = sample_history0[k];
                    // backward pass:
                    signal = (reward - baseline) / history_len0;
                    for (i=0; i!=2; ++i) {
                        dlossd_hh0[i] = - signal * active_exphh0[i] / partition0;
                    }
                    dlossd_hh0[sample0] += signal;
                    for (j=0; j!=4; ++j) {
                        dlossd_z0[j] = 0.0;
                        for (i=0; i!=2; ++i) {
                            dlossd_z0[j] += weight_v0[i][j] * dlossd_hh0[i];
                            weight_v0[i][j] += LEARNING_RATE * dlossd_hh0[i] * active_z0[j];
                        }
                        dlossd_h0[j] = dlossd_z0[j] * dlrelu(active_h0[j]);
                    }
                    for (j=0; j!=3; ++j) {
                        for (i=0; i!=4; ++i) {
                            weight_u0[i][j] += LEARNING_RATE * dlossd_h0[i] * input0[j];
                        }
                    }
                }
                // clear history:
                history_len0 = 0;
            }
            // update baseline:
            baseline += (reward - baseline) / BASELINE_AVG_TIMESCALE;
            _i = _i+1;
        } else {
            break;
        }
    }
}

int _compare(float a, float b)
{
    input0[0] = a;
    input0[1] = b;
    input0[2] = 1.0;
    // forward pass:
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
        partition0+= active_exphh0[i];
    }
    // sample based on distribution:
    randval = uniform();
    cumulative = 0.0;
    for (i=0; i!=2; ++i) {
        cumulative += active_exphh0[i] / partition0;
        if (cumulative >= randval) { break; }
    }
    sample0 = i;
    // update history:
    if (history_len0 == HISTORY_CAPACITY) {
        i = rand() % HISTORY_CAPACITY;
    } else {
        i = history_len0;
        history_len0 += 1;
    }
    for (j=0; j!=3-1; ++j) {
        input_history0[i][j] = input0[j];
    }
    sample_history0[i] = sample0;
    switch (sample0) {
        case 0: {
            return 0;
        } break;
        case 1: {
            return 1;
        } break;
    }
}

