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
/*NN_GLOBALS*/



/*****************************************************************************/
/*  4. USER FUNCTIONS (DECLARATION)                                          */
/*****************************************************************************/
/*USER_FUNCTION_DECLARATIONS*/



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
    /*WEIGHT_INIT*/
}

/*****************************************************************************/
/*  8. USER FUNCTIONS (IMPLEMENTATION)                                       */
/*****************************************************************************/
/*USER_FUNCTION_IMPLEMENTATIONS*/
