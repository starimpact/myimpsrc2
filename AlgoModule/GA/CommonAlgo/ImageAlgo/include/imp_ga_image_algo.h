
#ifndef _IMP_GA_IMAGE_ALGO_H_
#define _IMP_GA_IMAGE_ALGO_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "imp_algo_type.h"
#include "imp_ga_math.h"
#include "imp_ga_canny_standard.h"
#include "imp_ga_fft.h"
#ifdef __cplusplus
extern "C"
{
#endif


IMP_VOID ipImageSub(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageDiff(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_S32 threshold, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageSubNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageAddNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst,  IMP_U16 stride2, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageAdd(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height);


IMP_VOID ipImageAnd(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageAndNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst,  IMP_U16 stride2, IMP_U16 width,IMP_U16 height);


IMP_VOID ipImageOr(IMP_U8 *src1,IMP_U16 stride1, IMP_U8 *src2, IMP_U16 stride2, IMP_U8 *dst, IMP_U16 stride3, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageOrNum(IMP_U8 *src, IMP_U16 stride1, IMP_U8 n,IMP_U8 *dst,  IMP_U16 stride2, IMP_U16 width,IMP_U16 height);

IMP_VOID ipImageBinary(IMP_U8 *src,IMP_U8 *dst,IMP_U16 width,IMP_U16 height,IMP_U8 threshold);


IMP_VOID ipImageThreshold (IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2,
					   IMP_U16 width, IMP_U16 height,
					   IMP_U8 threshold, IMP_U8 maxValue, IMP_U8 minValue, IMP_U8 mode);

IMP_S32 ipImageAdptiveBinary(IMP_U8 *src,IMP_U8 *dst,IMP_U16 width,IMP_U16 height);


IMP_VOID IMP_GA_InitDitherTab ();
IMP_VOID IMP_GA_FreeDitherTab ();

IMP_VOID ipConvertImageYUV2RGB(YUV_IMAGE422_S *src, RGB_IMAGE_S *dst );


IMP_VOID ipConvertYUVtoRGB (IMP_U8 *src0, IMP_U8 *src1, IMP_U8 *src2,
                      IMP_U8 *dst_ori,IMP_S32 width,IMP_S32 height);


IMP_VOID ipConvertImageRGB2YUV( RGB_IMAGE_S *img_rgb, YUV_IMAGE422_S *img_yuv );

IMP_VOID ipConvertImageRgb2HSV(  IMP_U8 *pRGB, IMP_FLOAT *pHSV, IMP_U16 width, IMP_U16 heigth );

static const IMP_U8 icvHue255To180[] =
{
	0,   1,   1,   2,   3,   4,   4,   5,   6,   6,   7,   8,   8,   9,  10,  11,
	11,  12,  13,  13,  14,  15,  16,  16,  17,  18,  18,  19,  20,  20,  21,  22,
	23,  23,  24,  25,  25,  26,  27,  28,  28,  29,  30,  30,  31,  32,  32,  33,
	34,  35,  35,  36,  37,  37,  38,  39,  40,  40,  41,  42,  42,  43,  44,  44,
	45,  46,  47,  47,  48,  49,  49,  50,  51,  52,  52,  53,  54,  54,  55,  56,
	56,  57,  58,  59,  59,  60,  61,  61,  62,  63,  64,  64,  65,  66,  66,  67,
	68,  68,  69,  70,  71,  71,  72,  73,  73,  74,  75,  76,  76,  77,  78,  78,
	79,  80,  80,  81,  82,  83,  83,  84,  85,  85,  86,  87,  88,  88,  89,  90,
	90,  91,  92,  92,  93,  94,  95,  95,  96,  97,  97,  98,  99, 100, 100, 101,
	102, 102, 103, 104, 104, 105, 106, 107, 107, 108, 109, 109, 110, 111, 112, 112,
	113, 114, 114, 115, 116, 116, 117, 118, 119, 119, 120, 121, 121, 122, 123, 124,
	124, 125, 126, 126, 127, 128, 128, 129, 130, 131, 131, 132, 133, 133, 134, 135,
	136, 136, 137, 138, 138, 139, 140, 140, 141, 142, 143, 143, 144, 145, 145, 146,
	147, 148, 148, 149, 150, 150, 151, 152, 152, 153, 154, 155, 155, 156, 157, 157,
	158, 159, 160, 160, 161, 162, 162, 163, 164, 164, 165, 166, 167, 167, 168, 169,
	169, 170, 171, 172, 172, 173, 174, 174, 175, 176, 176, 177, 178, 179, 179, 180
};


static const IMP_U8 icvHue180To255[] =
{
	0,   1,   3,   4,   6,   7,   9,  10,  11,  13,  14,  16,  17,  18,  20,  21,
	23,  24,  26,  27,  28,  30,  31,  33,  34,  35,  37,  38,  40,  41,  43,  44,
	45,  47,  48,  50,  51,  52,  54,  55,  57,  58,  60,  61,  62,  64,  65,  67,
	68,  69,  71,  72,  74,  75,  77,  78,  79,  81,  82,  84,  85,  86,  88,  89,
	91,  92,  94,  95,  96,  98,  99, 101, 102, 103, 105, 106, 108, 109, 111, 112,
	113, 115, 116, 118, 119, 120, 122, 123, 125, 126, 128, 129, 130, 132, 133, 135,
	136, 137, 139, 140, 142, 143, 145, 146, 147, 149, 150, 152, 153, 154, 156, 157,
	159, 160, 162, 163, 164, 166, 167, 169, 170, 171, 173, 174, 176, 177, 179, 180,
	181, 183, 184, 186, 187, 188, 190, 191, 193, 194, 196, 197, 198, 200, 201, 203,
	204, 205, 207, 208, 210, 211, 213, 214, 215, 217, 218, 220, 221, 222, 224, 225,
	227, 228, 230, 231, 232, 234, 235, 237, 238, 239, 241, 242, 244, 245, 247, 248,
	249, 251, 252, 254, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
	255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
};
IMP_VOID rgb2hsi(IMP_DOUBLE *r1, IMP_DOUBLE *g1,IMP_DOUBLE *b1, IMP_DOUBLE *h, IMP_DOUBLE *s, IMP_DOUBLE *v );
IMP_VOID rgb2hsi_uint8(IMP_U8* pRGB, IMP_U8* pHSV );
IMP_VOID ipImageConvertHSV2RGB(IMP_FLOAT* pHSV, IMP_U8* pRGB, IMP_U16 width, IMP_U16 height);


IMP_VOID ipImageConvertRGB2HSL(IMP_U8* pRGB, IMP_FLOAT* pHSL, IMP_U16 width, IMP_U16 height);


IMP_VOID ipImageConvertHSL2RGB(IMP_FLOAT* pHSL, IMP_U8* pRGB, IMP_U16 width, IMP_U16 height);




IMP_VOID ipImageConvolution( IMP_U8 *src, IMP_U8 *dst,IMP_U16 width,IMP_U16 height,IMP_S32 *kernel,IMP_S32 size );


IMP_VOID ipGaussFilter3_3(IMP_U8 * src, IMP_U16 stride1, IMP_U8 * dst, IMP_U16 stride2, IMP_S8 * mask, IMP_U16 width, IMP_U16 height);

IMP_VOID ipGaussFilter5_5(IMP_U8 * src, IMP_U16 stride1, IMP_U8 * dst, IMP_U16 stride2, IMP_S8 * mask, IMP_U16 width, IMP_U16 height);

IMP_VOID ipMeanFilter(IMP_U8 * src,IMP_U8 * dst,IMP_U16 width, IMP_U16 height);

IMP_VOID ipMedianFilter3_3(IMP_U8 * src,IMP_U8 * dst,IMP_U16 width,IMP_U16 height);

IMP_VOID ipMedianFilter5_5(IMP_U8 * src,IMP_U8 * dst,IMP_U16 width,IMP_U16 height);

IMP_VOID ipMedianImage( IMP_U8 *src, IMP_U8 *dst, IMP_U16 width, IMP_U16 height);



#define IP_NO_EDGE			0
#define IP_EDGE			255
#define IP_SUSPICION_EDGE	128
#define IP_GRADIEN_TH		10
#define IP_EDGE_MIN_ROWS	4
#define IP_EDGE_MIN_COLS	4

IMP_VOID ipCanny( IMP_U8 *src, IMP_S8 *grad, IMP_U8 *mag, IMP_U8 *edge,
			  IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
			  IMP_S32 width, IMP_S32 height, IMP_S32 tgrad );

IMP_VOID ipCannyCalcGradmagT( IMP_U8 *src, IMP_S8 *grad, IMP_U8 *mag,
						  IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
						  IMP_S32 width, IMP_S32 height );

IMP_VOID ipCannyCalcGradienti( IMP_U8 *src, IMP_S8 *grad,
							IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
							IMP_S32 width, IMP_S32 height );

IMP_VOID ipCannyCalcMagnitudei( IMP_S8 *grad, IMP_U8 *mag,
							IMP_S32 startCol, IMP_S32 startRow, IMP_S32 endCol, IMP_S32 endRow,
							IMP_S32 width, IMP_S32 height );

IMP_VOID ipCannyNonMaxSupp( IMP_S8 *grad, IMP_U8 *mag, IMP_U8 *edge,
						IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
						IMP_S32 width, IMP_S32 height, IMP_S32 tgrad );


IMP_VOID ipGradientX(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height);

IMP_VOID ipGradientY(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height);

IMP_VOID ipGradientXY(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height);


IMP_VOID ipSobel3_3(IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height);

IMP_VOID ipSobel5_5(IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height);

IMP_VOID ipSobelGrad3_3(IMP_U8 *src, IMP_U16 stride1, IMP_U8 *edge, IMP_U16 stride2, IMP_U16 * mag, IMP_U16 stride3, IMP_FLOAT * gradDir, IMP_U16 stride4, IMP_U16 width, IMP_U16 height);


IMP_VOID ipLaplace3_3(IMP_U8 *src, IMP_U8 *dst,IMP_U16 width, IMP_U16 height);


#ifndef IP_GRAY_LEVEL
#define IP_GRAY_LEVEL			32
#define IP_GRAY_LEVEL_B			3
#endif

#define IP_RGB_LEVEL				4096

#define IP_RGB_LEVEL_B				4

typedef struct IpHistgramRGB
{
	IMP_U32 pHistogram[IP_RGB_LEVEL];
}IpHistgramRGB;

typedef struct IpHistgramRGBStatistics
{
	IMP_U32 pHistogram[IP_RGB_LEVEL];
	IMP_FLOAT E[3];
	IMP_FLOAT sigma[3];
	IMP_FLOAT skewness[3];
	IMP_POINT_S centroid;
}IpHistgramRGBStatistics;


IMP_VOID ipHistogramGray32(IMP_U8 *src,IMP_U16 width,IMP_U16 height,IMP_RECT_S *pRect,IMP_U32 *pHistogram32);

IMP_VOID ipHistogramGray256(IMP_U8 *src, IMP_U16 stride, IMP_U16 width,IMP_U16 height,IMP_RECT_S *pRect,IMP_U32 *pHistogram256);

IMP_VOID ipHistogramRGB(IMP_U8 *src, IMP_U16 width, IMP_U16 height,
						IMP_RECT_S *pRect,IpHistgramRGB *pHistgramRGB);

IMP_VOID ipHistogramRGBStatistics(const RGB_IMAGE_S *pRGBImage,GRAY_IMAGE_S *src,
						IMP_RECT_S *pRect,IpHistgramRGBStatistics *pHistgramRGB,
						IMP_S32 flagUseBottom);

IMP_VOID ipMorphDilateImage ( IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height, const IMP_U8 *mask);

IMP_VOID ipMorphErodeImage( IMP_U8 *src, IMP_U16 stride1, IMP_U8 *dst, IMP_U16 stride2, IMP_U16 width, IMP_U16 height, const IMP_U8 *mask);

IMP_VOID ipMorphCloseImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, GRAY_IMAGE_S *tmp, IMP_S32 cnt );

IMP_VOID ipMorphOpenImage( GRAY_IMAGE_S *src, GRAY_IMAGE_S *dst, GRAY_IMAGE_S *tmp, IMP_S32 cnt );

IMP_VOID ipRemoveStablizationForeground( GRAY_IMAGE_S *img, IMP_POINT_S *mv );

IMP_VOID ipRemoveShakingForeground( GRAY_IMAGE_S *img_bg, GRAY_IMAGE_S *img_in, GRAY_IMAGE_S *img_fg, IMP_S32 gray_thld );


IMP_S32 ipAvgSdv( const IMP_U8 *src, IMP_U16 width, IMP_U16 height, IMP_U8* _mean, IMP_FLOAT* _std_dev, const IMP_RECT_S* rect );


IMP_S32 ipIntegralOrigial(const IMP_U8 *src,
			   IMP_S32*  sum,
               IMP_S32* sqsum,
               IMP_S32* tilted,
			   IMP_S32 height,
               IMP_S32 width,
			   IMP_S32 srcstep,
               IMP_S32 sumstep,
			   IMP_S32 sqsumstep,
               IMP_S32 tiltedstep
			);


typedef struct IpIntegralPixel
{
    IMP_S64 sqsum:32;
	IMP_S64 sum:4;
}IpIntegralPixel;


IMP_VOID ipIntegral(IMP_U8 *src, IMP_U16 stride1, IMP_U64 * dst, IMP_S32 stride2, IMP_U16 height, IMP_U16 width);

IMP_VOID ipNearestNeighborResize(IMP_U8* src, IMP_U8* dst, IMP_U16 srcH, IMP_U16 srcW, IMP_U16 dstH, IMP_U16 dstW, IMP_U16 pixelSize);


IMP_DOUBLE BiCubicKernel( IMP_DOUBLE x);

IMP_VOID ipBiCubicResize(IMP_U8* src, IMP_U8* dst, IMP_U16 srcH, IMP_U16 srcW, IMP_U16 dstH, IMP_U16 dstW, IMP_U16 pixelSize);


IMP_VOID ipBiLinearResize(IMP_U8* src, IMP_U8* dst, IMP_U16 srcH, IMP_U16 srcW, IMP_U16 dstH, IMP_U16 dstW, IMP_U16 pixelSize);


#ifdef __cplusplus
}
#endif

#endif
