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
#define WEIGHT_CLIP_SCALE 5.0
#define LEARNING_RATE 0.05
#define HISTORY_CAPACITY 64 
#define BASELINE_AVG_TIMESCALE 100 


#define _2__main() _main()
#define _2__uniform() _uniform()
#define _2__laplace() _laplace()
#define _2__compare(a, b) _compare((a), (b))
#define _2__compare_and_swap(a, b) _compare_and_swap((&a), (&b))
#define _2__trytosort(a, b, c, d) _trytosort((a), (b), (c), (d))


/*****************************************************************************/
/*  0. HELPERS (DECLARATION)                                                 */
/*****************************************************************************/
#define ABORT exit(1)
float uniform();
float laplace();
float lrelu(float h);
float dlrelu(float h);
float clip(float w);



/*****************************************************************************/
/*  1. GLOBALS                                                               */
/*****************************************************************************/
static int i, j, k;
static float randval, cumulative;
static float reward, reward_exp, reward_var, signal;



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
static float input1[5];
static float weight_u1[12][5];    
static float active_h1[12];       static float dlossd_h1[12];
static float active_z1[12];       static float dlossd_z1[12];
static float weight_v1[3][12];    
static float active_hh1[3];      static float dlossd_hh1[3];
static float active_exphh1[3];   
static float partition1;
static int   sample1;

static float input_history1[HISTORY_CAPACITY][5-1];
static int   sample_history1[HISTORY_CAPACITY];
static int   history_len1;




/*****************************************************************************/
/*  4. USER FUNCTIONS (DECLARATION)                                          */
/*****************************************************************************/

int _main();
float _uniform();
float _laplace();
int _compare(float _a, float _b);
int _compare_and_swap( float* _a,  float* _b);
float _trytosort(float _a, float _b, float _c, float _d);




/*****************************************************************************/
/*  5. MAIN                                                                  */
/*****************************************************************************/
int main()
{
    srand(17029);
    initialize_weights();
    reward_exp = 0.0;
    reward_var = 1.0;
    printf("\033[33m");
    _2__main();
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
float clip(float w)
{
    return (w!=w)   ? WEIGHT_INIT_SCALE * laplace() :
           (w<-WEIGHT_CLIP_SCALE) ? -WEIGHT_CLIP_SCALE :
           (w>WEIGHT_CLIP_SCALE) ? WEIGHT_CLIP_SCALE : w;
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
for (int i=0; i!=12; ++i) {
    for (int j=0; j!=5; ++j) {
        weight_u1[i][j] = WEIGHT_INIT_SCALE * laplace();
    }
}
for (int i=0; i!=3; ++i) {
    for (int j=0; j!=12; ++j) {
        weight_v1[i][j] = WEIGHT_INIT_SCALE * laplace();
    }
}

}

/*****************************************************************************/
/*  8. USER FUNCTIONS (IMPLEMENTATION)                                       */
/*****************************************************************************/

int _main()
{
    float _sortsuccess;
    float _acc;
    _acc = 0.0;
    int _counter;
    int _tt;
    _tt = 50000;
    int _t;
    _t = 5000;
    _counter = 0;
    while (true) {
        if ((((_counter!=_tt)))) {
            _counter = _counter+1;
            float _a;
            _a = _2__uniform();
            float _b;
            _b = _2__uniform();
            float _c;
            _c = _2__uniform();
            float _d;
            _d = _2__uniform();
            _sortsuccess = _2__trytosort(_a,_b,_c,_d);
            reward = (_sortsuccess);
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
                    sample0 = 0;
                    for (i=0; i!=2; ++i) {
                        cumulative += active_exphh0[i] / partition0;
                        if (cumulative >= randval) {
                            sample0 = i;
                            break;
                        }
                    }
                    sample0 = sample_history0[k];
                    // backward pass:
                    signal = (reward - reward_exp) / history_len0;
                    for (i=0; i!=2; ++i) {
                        dlossd_hh0[i] = - signal * active_exphh0[i] / partition0;
                    }
                    dlossd_hh0[sample0] += signal;
                    for (j=0; j!=4; ++j) {
                        dlossd_z0[j] = 0.0;
                        for (i=0; i!=2; ++i) {
                            dlossd_z0[j] += weight_v0[i][j] * dlossd_hh0[i];
                            weight_v0[i][j] += LEARNING_RATE * dlossd_hh0[i] * active_z0[j];
                            weight_v0[i][j] = clip(weight_v0[i][j]);
                        }
                        dlossd_h0[j] = dlossd_z0[j] * dlrelu(active_h0[j]);
                    }
                    for (j=0; j!=3; ++j) {
                        for (i=0; i!=4; ++i) {
                            weight_u0[i][j] += LEARNING_RATE * dlossd_h0[i] * input0[j];
                            weight_u0[i][j] = clip(weight_u0[i][j]);
                        }
                    }
                }
                // clear history:
                history_len0 = 0;
            }
            if (history_len1 != 0) {
                for (k=0; k!= history_len1; ++k) {
                    // forward pass from history:
                    for (i=0; i!=5-1; ++i) {
                        input1[i] = input_history1[k][i];
                    }
                    input1[5-1] = 1.0;
                    // forward pass:
                    for (i=0; i!=12; ++i) {
                        active_h1[i] = 0.0;
                        for (j=0; j!=5; ++j) {
                            active_h1[i] += weight_u1[i][j] * input1[j];
                        }
                        active_z1[i] = lrelu(active_h1[i]);
                    }
                    partition1 = 0.0;
                    for (i=0; i!=3; ++i) {
                        active_hh1[i] = 0.0;
                        for (j=0; j!=12; ++j) {
                            active_hh1[i] += weight_v1[i][j] * active_z1[j];
                        }
                        active_exphh1[i] = exp(active_hh1[i]);
                        partition1+= active_exphh1[i];
                    }
                    // sample based on distribution:
                    randval = uniform();
                    cumulative = 0.0;
                    sample1 = 0;
                    for (i=0; i!=3; ++i) {
                        cumulative += active_exphh1[i] / partition1;
                        if (cumulative >= randval) {
                            sample1 = i;
                            break;
                        }
                    }
                    sample1 = sample_history1[k];
                    // backward pass:
                    signal = (reward - reward_exp) / history_len1;
                    for (i=0; i!=3; ++i) {
                        dlossd_hh1[i] = - signal * active_exphh1[i] / partition1;
                    }
                    dlossd_hh1[sample1] += signal;
                    for (j=0; j!=12; ++j) {
                        dlossd_z1[j] = 0.0;
                        for (i=0; i!=3; ++i) {
                            dlossd_z1[j] += weight_v1[i][j] * dlossd_hh1[i];
                            weight_v1[i][j] += LEARNING_RATE * dlossd_hh1[i] * active_z1[j];
                            weight_v1[i][j] = clip(weight_v1[i][j]);
                        }
                        dlossd_h1[j] = dlossd_z1[j] * dlrelu(active_h1[j]);
                    }
                    for (j=0; j!=5; ++j) {
                        for (i=0; i!=12; ++i) {
                            weight_u1[i][j] += LEARNING_RATE * dlossd_h1[i] * input1[j];
                            weight_u1[i][j] = clip(weight_u1[i][j]);
                        }
                    }
                }
                // clear history:
                history_len1 = 0;
            }
            // update baseline:
            reward_var += ((reward-reward_exp)*(reward-reward_exp) - reward_var) / BASELINE_AVG_TIMESCALE;
            reward_exp += (reward - reward_exp) / BASELINE_AVG_TIMESCALE;
            _acc = _acc+(_sortsuccess-_acc)/_t;
            if ((((_t*(_counter/_t)==_counter)))) {
                printf("acc \t %f\n", _acc);
            } else if ((!(((_t*(_counter/_t)==_counter))))) {
            } else {
                printf("FAILED ALTERNATIVE CONSTRUCT ((t*(counter/t)==counter)  etc)\n");
                ABORT;
            }
        } else {
            break;
        }
    }
}

float _uniform()
{
    return uniform();
}

float _laplace()
{
    return laplace();
}

int _compare(float _a, float _b)
{
    input0[0] = _a;
    input0[1] = _b;
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
    sample0 = 0;
    for (i=0; i!=2; ++i) {
        cumulative += active_exphh0[i] / partition0;
        if (cumulative >= randval) {
            sample0 = i;
            break;
        }
    }
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
            if ((_a<_b)) {
                return 0;
            } else if ((_a>=_b)) {
                return 1;
            } else {
                printf("FAILED ALTERNATIVE CONSTRUCT (a<b  etc)\n");
                ABORT;
            }
        } break;
        case 1: {
            if ((_a>_b)) {
                return 0;
            } else if ((_a<=_b)) {
                return 1;
            } else {
                printf("FAILED ALTERNATIVE CONSTRUCT (a>b  etc)\n");
                ABORT;
            }
        } break;
    }
}

int _compare_and_swap( float* _a,  float* _b)
{
    int _issorted;
    int _temp;
    _issorted = _2__compare((*_a),(*_b));
    if ((_issorted!=0)) {
    } else if ((_issorted==0)) {
        _temp = (*_a);
        *_a = (*_b);
        *_b = _temp;
    } else {
        printf("FAILED ALTERNATIVE CONSTRUCT (issorted!=0  etc)\n");
        ABORT;
    }
    return _issorted;
}

float _trytosort(float _a, float _b, float _c, float _d)
{
    bool _issorted;
    float _temp;
    int _counter;
    _counter = 0;
    while (true) {
        if ((_counter!=6)) {
            _counter = _counter+1;
            input1[0] = _a;
            input1[1] = _b;
            input1[2] = _c;
            input1[3] = _d;
            input1[4] = 1.0;
            // forward pass:
            for (i=0; i!=12; ++i) {
                active_h1[i] = 0.0;
                for (j=0; j!=5; ++j) {
                    active_h1[i] += weight_u1[i][j] * input1[j];
                }
                active_z1[i] = lrelu(active_h1[i]);
            }
            partition1 = 0.0;
            for (i=0; i!=3; ++i) {
                active_hh1[i] = 0.0;
                for (j=0; j!=12; ++j) {
                    active_hh1[i] += weight_v1[i][j] * active_z1[j];
                }
                active_exphh1[i] = exp(active_hh1[i]);
                partition1+= active_exphh1[i];
            }
            // sample based on distribution:
            randval = uniform();
            cumulative = 0.0;
            sample1 = 0;
            for (i=0; i!=3; ++i) {
                cumulative += active_exphh1[i] / partition1;
                if (cumulative >= randval) {
                    sample1 = i;
                    break;
                }
            }
            // update history:
            if (history_len1 == HISTORY_CAPACITY) {
                i = rand() % HISTORY_CAPACITY;
            } else {
                i = history_len1;
                history_len1 += 1;
            }
            for (j=0; j!=5-1; ++j) {
                input_history1[i][j] = input1[j];
            }
            sample_history1[i] = sample1;
            switch (sample1) {
                case 0: {
                    _issorted = _2__compare_and_swap(_a,_b);
                } break;
                case 1: {
                    _issorted = _2__compare_and_swap(_b,_c);
                } break;
                case 2: {
                    _issorted = _2__compare_and_swap(_c,_d);
                } break;
            }
        } else {
            break;
        }
    }
    if ((((_a<=_b)&&(_b<=_c)&&(_c<=_d)))) {
        return 1.0;
    } else if ((!(((_a<=_b)&&(_b<=_c)&&(_c<=_d))))) {
        return 0.0;
    } else {
        printf("FAILED ALTERNATIVE CONSTRUCT ((a<=b and b<=c and c<=d)  etc)\n");
        ABORT;
    }
}

