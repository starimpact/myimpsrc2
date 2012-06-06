
#ifndef _IMP_GA_FFT_H_
#define _IMP_GA_FFT_H_

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


typedef struct
{
	float re;
	float im;
} IpComplex;

typedef struct
{
	double re;
	double im;
} IpDComplex;



#ifndef IP_ERROR
#define IP_ERROR -1
#endif
#ifndef IP_NO_ERROR
#define IP_NO_ERROR 0
#endif



#define FFT_FORWARD	0

#define FFT_INVERSE 1

#define PI      3.1415926535897932
#define TWOPI   6.2831853071795865
#define HALFPI  1.5707963267948966
#define PI8 	0.392699081698724
#define RT2 	1.4142135623731
#define IRT2 	0.707106781186548


int ipForwardFFT2f(IpComplex *array, int rows, int cols);

int ipInverseFFT2f(IpComplex *array, int rows, int cols);

int ipForwardFFT2d(IpDComplex *array, int rows, int cols);


int ipInverseFFT2d(IpDComplex *array, int rows, int cols);

IMP_S32 ipFromGrayImage( IMP_U8 * src, IpComplex *array, IMP_U16 width, IMP_U16 height );

IMP_S32 ipToGrayImage( IpComplex * src, IMP_U8 *dst, IMP_U16 width, IMP_U16 height, float scale );

#ifdef __cplusplus
}
#endif

#endif
