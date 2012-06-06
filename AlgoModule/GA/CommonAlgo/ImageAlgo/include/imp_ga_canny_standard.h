
#ifndef _IMP_GA_CANNY_STANDARD_H_
#define _IMP_GA_CANNY_STANDARD_H_


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "imp_algo_type.h"
#include "imp_ga_math.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define VERBOSE 1
#define BOOSTBLURFACTOR 90.0

void ipCannyOriginal(unsigned char *image, int rows, int cols, float sigma,
         float tlow, float thigh, unsigned char *edge, float * gradDir);
void gaussian_smooth(unsigned char *image, int rows, int cols, float sigma,
        short int **smoothedim);
void make_gaussian_kernel(float sigma, float **kernel, int *windowsize);
void derrivative_x_y(short int *smoothedim, int rows, int cols,
        short int **delta_x, short int **delta_y);
void magnitude_x_y(short int *delta_x, short int *delta_y, int rows, int cols,
        short int **magnitude);
void apply_hysteresis(short int *mag, unsigned char *nms, int rows, int cols,
        float tlow, float thigh, unsigned char *edge);
void radian_direction(short int *delta_x, short int *delta_y, int rows,
		int cols, float **dir_radians, int xdirtag, int ydirtag);
double angle_radians(double x, double y);
void non_max_supp(short *mag, short *gradx, short *grady, int nrows, int ncols,
    unsigned char *result);
#ifdef __cplusplus
}
#endif

#endif
