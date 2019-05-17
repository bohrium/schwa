/* author: samtenka
 * change: 2019-05-16
 * create: 2019-05-16
 * descrp: test concept of reinforcement programming by learning to compare numbers
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define SEED 0
#define NB_TRIALS 100000

static float input[3];
static float weight_u[4][3];    
static float active_h[4];       static float dlossd_h[4];
static float active_z[4];       static float dlossd_z[4];
static float weight_v[2][4];    
static float active_hh[2];      static float dlossd_hh[2];
static float active_exphh[2];   
static float partition;
static int sample;
static float reward;

void salutation();
void valediction();
float uniform();
float laplace();
void initialize_weights();
void forward();
void backward(float learning_rate, float baseline);

void main()
{
        salutation();

        float reward_avg = 0.5;
        for (int t=0; t!=NB_TRIALS; ++t) {
                input[0] = uniform();
                input[1] = uniform();
                input[2] = 1.0;
                forward();
                reward = ((input[0] < input[1] && sample==1) || (input[1] <= input[0] && sample==0)) ? 1.0 : 0.0;
                backward(0.01, 0.5);

                reward_avg = reward_avg + (reward - reward_avg)/10000.0; 
                if (t%10000 == 0) {
                    printf("reward avg %0.2f", reward_avg);
                    printf("\n");
                }
        }

        valediction();
}


void salutation()
{
        srand(SEED);
        initialize_weights();
        printf("hi\n");
}
void valediction()
{
        printf("bye\n");
}
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
                        weight_u[i][j] = 0.1 * laplace();
                }
        }
        for (int i=0; i!=2; ++i) {
                for (int j=0; j!=4; ++j) {
                        weight_v[i][j] = 0.1 * laplace();
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
float clip5(float w)
{
        return (w<-5.0) ? -5.0 : (w>5.0) ? 5.0 : w;
}
void forward()
{
        int i, j;
        for (i=0; i!=4; ++i) {
                active_h[i] = 0.0;
                for (j=0; j!=3; ++j) {
                        active_h[i] += weight_u[i][j] * input[j]; 
                }
                active_z[i] = lrelu(active_h[i]); 
        }
        partition = 0.0;
        for (i=0; i!=2; ++i) {
                active_hh[i] = 0.0;
                for (j=0; j!=4; ++j) {
                        active_hh[i] += weight_v[i][j] * active_z[j]; 
                }
                active_exphh[i] = exp(active_hh[i]);
                partition += active_exphh[i];
        }

        float r = uniform();
        float cumulative = 0.0;
        for (i=0; i!=2; ++i) {
                cumulative += active_exphh[i] / partition;
                if (cumulative >= r) { break; }
        }

        sample = i;
}

void backward(float learning_rate, float baseline)
{
        float g = reward - baseline;

        int i,j;
        for (i=0; i!=2; ++i) {
                dlossd_hh[i] = - g * active_exphh[i] / partition;
        }
        dlossd_hh[sample] += g;

        for (j=0; j!=4; ++j) {
                dlossd_z[j] = 0.0;
                for (i=0; i!=2; ++i) {
                        dlossd_z[j] += weight_v[i][j] * dlossd_hh[i];
                        weight_v[i][j] += learning_rate * dlossd_hh[i] * active_z[j]; 
                        //weight_v[i][j] = clip5(weight_v[i][j]);
                }
                dlossd_h[j] = dlossd_z[j] * dlrelu(active_h[j]); 
        }

        for (j=0; j!=3; ++j) {
                for (i=0; i!=4; ++i) {
                        weight_u[i][j] += learning_rate * dlossd_h[i] * input[j]; 
                        //weight_u[i][j] = clip5(weight_u[i][j]);
                }
        }
}
