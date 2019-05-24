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




/*****************************************************************************/
/*  4. USER FUNCTIONS (DECLARATION)                                          */
/*****************************************************************************/

int _main();
float _uniform();
float _laplace();




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
    
}

/*****************************************************************************/
/*  8. USER FUNCTIONS (IMPLEMENTATION)                                       */
/*****************************************************************************/

int _main()
{
}

float _uniform()
{
    return uniform();
}

float _laplace()
{
    return laplace();
}

