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




/*****************************************************************************/
/*  4. USER FUNCTIONS (DECLARATION)                                          */
/*****************************************************************************/

int _main();
float _uniform();
float _laplace();
int _compare(float _a, float _b);




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

}

/*****************************************************************************/
/*  8. USER FUNCTIONS (IMPLEMENTATION)                                       */
/*****************************************************************************/

int _main()
{
    int _counter;
    int _tt;
    _tt = 1000;
    int _t;
    _t = 100;
    float _acc;
    _acc = 0.0;
    _counter = 0;
    while (true) {
        if ((((_counter!=_tt)))) {
            _counter = _counter+1;
            float _x;
            _x = _2__uniform();
            float _y;
            _y = _2__uniform();
            int _c;
            _c = _2__compare(_x,_y);
            if ((((_x<=_y)&&(_c==0)))) {
                reward = (1.0);
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
                // update baseline:
                reward_var += ((reward-reward_exp)*(reward-reward_exp) - reward_var) / BASELINE_AVG_TIMESCALE;
                reward_exp += (reward - reward_exp) / BASELINE_AVG_TIMESCALE;
                _acc = _acc+(1-_acc)/_t;
            } else if ((((_x<=_y)&&(_c==1)))) {
                reward = (-1.0);
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
                // update baseline:
                reward_var += ((reward-reward_exp)*(reward-reward_exp) - reward_var) / BASELINE_AVG_TIMESCALE;
                reward_exp += (reward - reward_exp) / BASELINE_AVG_TIMESCALE;
                _acc = _acc+(0-_acc)/_t;
            } else if ((((_x>=_y)&&(_c==0)))) {
                reward = (-1.0);
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
                // update baseline:
                reward_var += ((reward-reward_exp)*(reward-reward_exp) - reward_var) / BASELINE_AVG_TIMESCALE;
                reward_exp += (reward - reward_exp) / BASELINE_AVG_TIMESCALE;
                _acc = _acc+(0-_acc)/_t;
            } else if ((((_x>=_y)&&(_c==1)))) {
                reward = (1.0);
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
                // update baseline:
                reward_var += ((reward-reward_exp)*(reward-reward_exp) - reward_var) / BASELINE_AVG_TIMESCALE;
                reward_exp += (reward - reward_exp) / BASELINE_AVG_TIMESCALE;
                _acc = _acc+(1-_acc)/_t;
            } else {
                printf("FAILED ALTERNATIVE CONSTRUCT ((x <= y and c==0)  etc)\n");
                ABORT;
            }
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
            if ((_a<=_b)) {
                return 0;
            } else if ((_a>=_b)) {
                return 1;
            } else {
                printf("FAILED ALTERNATIVE CONSTRUCT (a<=b  etc)\n");
                ABORT;
            }
        } break;
        case 1: {
            if ((_a>=_b)) {
                return 0;
            } else if ((_a<=_b)) {
                return 1;
            } else {
                printf("FAILED ALTERNATIVE CONSTRUCT (a>=b  etc)\n");
                ABORT;
            }
        } break;
    }
}

