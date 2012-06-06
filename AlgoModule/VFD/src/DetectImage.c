/*****************************************************************************
 ** 
 * 文件说明：图像处理
 * @brief        : Image handling
 * @date           2004/12/15
 * history:
 * @date           2005/12/12 人脸检出与识别模块的合并及标准化
 *　　　　　　　　　　　　　　Update Face recognition and detect module merged and standardize
 * Copyright (C) 2004 WISEBIRD CO., LTD. All rights reserved.
 *****************************************************************************/


#include <stdio.h>

#include "DetectImage.h"
#include "etc.h"
#include "fixedpoint.h"

#include "IO.h"



/*-----------------------------------------------------------------*
 * 局部函数定义
 * Local function declaration
 *-----------------------------------------------------------------*/
IFACE_ERROR_CODE SumRect( FaceRecognitionHandle* in_pInstance, 
			  const long* in_pSrcImage, 
              const long in_SrcWidth, const long in_SrcHeight,
              const long in_RectWidth, const long in_RectHeight, 
			  long* io_pSumImage, long* io_pSqSumImage );
IFACE_ERROR_CODE AffineSingleNoRotBilinear( FaceRecognitionHandle* in_pInstance, 
								 const unsigned char* in_pImage, 
                                 const long in_Width, const long in_Height,
                                 const short in_Scale, /* FXP_UNIT_SCALE的2倍 */
								                       /* FXP_UNIT_SCALE Twice value */
                                 const long in_DstWidth, const long in_DstHeight,
                                 unsigned char* io_DstImage );
IFACE_ERROR_CODE AffineSingleNoRotHalf( FaceRecognitionHandle* in_pInstance, 
							 const unsigned char* in_pImage, 
                             const long in_Width, const long in_Height,
                             unsigned char* io_DstImage );

/*-----------------------------------------------------------------*
 * Function
 *-----------------------------------------------------------------*/
/*****************************************************************************/
/** ConvertRGBtoY : RGB到Y的变换
/** ConvertRGBtoY : Convert RGB to Y
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param in_pImage
 * [in]输入图像（RGB）
 * [in]Input image (RGB)
 * @param in_Width
 * [in]输入图像宽
  * [in] Number of horizontal direction pixels of input images 
 * @param in_Height
 * [in]输入图像高
 * [in] Number of vertical direction pixels of input images
 * @param io_y
 * [io]输出图像（Y）
 * [io]Output image (Y)
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
IFACE_ERROR_CODE ConvertRGBtoY( FaceRecognitionHandle* in_pInstance, 
				    const unsigned char* in_pImage, 
				    const long in_Width, const long in_Height, 
					unsigned char* io_pYImage ){
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
    long x, y;
	/* Parameter check */
	if( in_pInstance == NULL || in_pImage == NULL ||
		io_pYImage == NULL ) {
		char tmp[256];
		sprintf( tmp, "" );
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}

    for ( y = 0 ; y < in_Height ; y++ ) {
        for ( x = 0 ; x < in_Width ; x++ ) {
            io_pYImage[in_Width * y + x]  = ( unsigned char ) clip( val_y_rgb( in_pImage, x, y, in_Width, in_Height ) );
		}
    }
	return resultCode;
}

/*****************************************************************************/
//仿射的：与一个固定原点及固定坐标系有关的等价于平移、收缩或展开的坐标变换式的，
/** AffineSingleNoRot : 仿射变换（没有旋转；线性插值）
/** AffineSingleNoRot : affine transformation （non-rotation；linear interpolation ）
 *      量值在1/2及以下时，重复按1/2缩小操作（2X2均衡，1/2间隔），
 *      When the scale isi 1/2 or less, repeating 1/2 reductions (2x2 average ＆1/2 thinning out)
 *      以线性插值缩小
 *      Reduce by a linear interpolation
 *      固定图像一角，根据量值移动起始点位置
 *      Fix image corner, Shift the starting point position according to the scale
 *      坐标系：输入图像的原点坐标(0, 0), 输入图像像数单位为1
 *      Coordinate system: Starting point coordinates (0,0) of input image, 1 size of pixels of input image
 *      → 量值变换后的原点坐标（1/2/in_Scale-1/2, 1/2/in_Scale-1/2）
 *      → Starting point coordinates of image after scale is converted（1/2/in_Scale-1/2, 1/2/in_Scale-1/2）
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param in_pImage
 * [in]输入图像（１波段）
 * [in]Input image ( 1 channel )
 * @param in_Width
 * [in]输入图像宽
 * [in] Number of horizontal direction pixels of input images 
 * @param in_Height
 * [in]输入图像高
 * [in] Number of vertical direction pixels of input images
 * @param in_Scale
 * [in]扩缩率（输入图像的in_Scale倍值）为FXP_UNIT_SCALE的2倍
 * [in]Scaling rate (The input image is done by in_Scale twice) FXP_UNIT_SCALE Twice value
 * @param io_DstImage
 * [io]变换后图像
 * [io]Image after haveing been converted
 * @param io_TmpImage
 * [io]中间内存；保证能分配(in_Width/2)*(in_Height/2)以上
 * [io]The middle buffer; Reserve more than (in_Width/2)*(in_Height/2) and call it. 
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
IFACE_ERROR_CODE AffineSingleNoRot( FaceRecognitionHandle* in_pInstance, 
					    const unsigned char* in_pImage, 
                        const long in_Width, const long in_Height,
                        const short in_Scale, /* FXP_UNIT_SCALE的2倍 */
						                      /* FXP_UNIT_SCALE Twice value */
                        const long in_DstWidth, const long in_DstHeight,
                        unsigned char* io_DstImage, unsigned char* io_TmpImage ) {
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
    long tempWidth, tempHeight;
    short scale;
	/* Parameter check */
	if( in_pInstance == NULL || in_pImage == NULL ) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}

    if ( in_Scale > ( FXP_UNIT_SCALE >> 1 ) ) {
        resultCode = AffineSingleNoRotBilinear( in_pInstance,in_pImage, in_Width,
			in_Height, in_Scale, in_DstWidth, in_DstHeight, io_DstImage );
		if( resultCode != IFACE_ERR_SUCCESS_ID ) {
			
			return resultCode;
		}
    }
    else {
        if ( NULL == io_TmpImage ) {
			resultCode = IFACE_ERR_INVALID_PARAM;
			
			return resultCode;
        }

        resultCode = AffineSingleNoRotHalf( in_pInstance, in_pImage, in_Width, in_Height, io_TmpImage );
		if( resultCode != IFACE_ERR_SUCCESS_ID ) {
			
			return resultCode;
		}

        tempWidth = in_Width >> 1;
        tempHeight = in_Height >> 1;
        scale = in_Scale << 1;
        while ( scale <= ( FXP_UNIT_SCALE >> 1 ) ) { /* 按照2*2的平均值缩小直到score > 1/2  */
			                                         /* Until scale > 1/2, reduce by the average of 2x2  */
            resultCode = AffineSingleNoRotHalf( in_pInstance, io_TmpImage, tempWidth, tempHeight, io_TmpImage );
			if( resultCode != IFACE_ERR_SUCCESS_ID ) {
				
				return resultCode;
			}
            tempWidth >>= 1;
            tempHeight >>= 1;
            scale <<= 1;
        }

        resultCode = AffineSingleNoRotBilinear( in_pInstance, io_TmpImage,
			tempWidth, tempHeight, scale,
			in_DstWidth, in_DstHeight, io_DstImage );
		if( resultCode != IFACE_ERR_SUCCESS_ID ) {
			
			return resultCode;
		}
    }
	return resultCode;
}

/*****************************************************************************/
/** AffineSingleNoRot_Bilinear : 仿射变换（没有旋转；线性插值）
/** AffineSingleNoRot_Bilinear : affine transformation （non-rotation；linear interpolation ）
 *      固定图像一角，根据scale的值移动起始点位置
 *      Fix image corner, Shift the starting point position according to the scale
 *      坐标系：输入图像的原点坐标（0，0），输入图像像数大小为1
 *      Coordinate system: Starting point coordinates (0,0) of input image, 1 size of pixels of input image
 *      → 比例变换后图像的原点坐标为（1/2/in_Scale-1/2, 1/2/in_Scale-1/2）
 *      → Starting point coordinates of image after scale is converted（1/2/in_Scale-1/2, 1/2/in_Scale-1/2）
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param in_pImage
 * [in]输入图像（1通道）
 * [in]Input image ( 1 channel )
 * @param in_Width
 * [in]输入图像的X方向像数数
 * [in] Number of horizontal direction pixels of input images 
 * @param in_Height
 * [in]输入图像的Y方向像数数
 * [in] Number of vertical direction pixels of input images
 * @param in_Scale
 * [in]扩缩率（输入图像in_Scale的两倍）FXP_UNIT_SCALE的倍值
 * [in]Scaling rate (The input image is done by in_Scale twice) FXP_UNIT_SCALE Twice value
 * @param io_DstImage
 * [io]变换后图像
 * [io]Image after converted
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
IFACE_ERROR_CODE AffineSingleNoRotBilinear( FaceRecognitionHandle* in_pInstance, 
								 const unsigned char* in_pImage, 
                                 const long in_Width, const long in_Height,
                                 const short in_Scale, /* FXP_UNIT_SCALE的倍值 */
								                       /* FXP_UNIT_SCALE Twice value */
                                 const long in_DstWidth, const long in_DstHeight,
                                 unsigned char* io_DstImage ) {
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
    unsigned char* pOutImage;
    long x, y;
    long srcX, srcY;
    long srcXIdx1, srcXIdx2, srcYIdx1, srcYIdx2;
    long rX1, rX2, rY1, rY2;

    long sX[4096], sY[4096];
    long rX[4096], rY[4096];

    if ( ( in_DstWidth > 4096 ) || ( in_DstHeight > 4096 ) ) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
    }

    pOutImage = io_DstImage;

#if 1/* 预处理20050210_4的系数 */
	 /* In advance, calculate 20050210_4 factor evaluation  */
    for ( x = 0 ; x < in_DstWidth ; x++ ) {
		/*-----------------------------
         * 原图像上的位置 
		 * Position in former image
         *   (x+0.5)/scale-0.5 
         *   -> (x*FXP_UNIT_POS + FXP_UNIT_POS_HALF)/(scale/FXP_UNIT_SCALE)-FXP_UNIT_POS_HALF
         *   ※整数化：(x+0.5)/scale-0.5)+0.5 四舍五入
		 *   ※Making to integer：round  down (x+0.5)/scale-0.5)+0.5
		 *-----------------------------*/
        srcX = ( x * FXP_UNIT_POS + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / in_Scale - FXP_UNIT_POS_HALF; /* FXP_UNIT_POS的倍值 */
		                                                                                                 /* FXP_UNIT_POS Twice value */
        sX[x] = srcX / FXP_UNIT_POS;
        if ( sX[x] < 0 ) {
            sX[x] = 0;
            rX[x] = 0;
        }
        else if ( sX[x] >= in_Width - 1 ) {
            sX[x] = in_Width - 2;
            rX[x] = FXP_UNIT_LINEAR;
        }
        else {
            rX[x] = FXP_UNIT_LINEAR * srcX / FXP_UNIT_POS - FXP_UNIT_LINEAR * sX[x];
        }
    }
    for ( y = 0 ; y < in_DstHeight ; y++ ) {
		 /*-----------------------------
          * 原图像上的位置 
		  * Position in former image
          * (y+0.5)/scale-0.5 
          * -> (y*FXP_UNIT_POS + FXP_UNIT_POS_HALF)/(scale/FXP_UNIT_SCALE)-FXP_UNIT_POS_HALF
          * ※整数化：(y+0.5)/scale-0.5)+0.5 四舍五入
		  * ※Making to integer：round  down (y+0.5)/scale-0.5)+0.5
		  *-----------------------------*/
        srcY = ( y * FXP_UNIT_POS + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / in_Scale - FXP_UNIT_POS_HALF; /* FXP_UNIT_POS的倍值 */
		                                                                                                 /* FXP_UNIT_POS Twice value */
        sY[y] = srcY / FXP_UNIT_POS;
        if ( sY[y] < 0 ) {
            sY[y] = 0;
            rY[y] = 0;
        }
        else if ( sY[y] >= in_Height - 1 ) {
            sY[y] = in_Height - 2;
            rY[y] = FXP_UNIT_LINEAR;
        }
        else {
            rY[y] = FXP_UNIT_LINEAR * srcY / FXP_UNIT_POS - FXP_UNIT_LINEAR * sY[y];
        }
    }
    for ( y = 0 ; y < in_DstHeight ; y++ ) {
        for ( x = 0 ; x < in_DstWidth ; x++ ) {
            srcXIdx1 = sX[x];
            rX2        = rX[x];
            srcXIdx2 = srcXIdx1+1;
            rX1        = FXP_UNIT_LINEAR - rX2;
            srcYIdx1 = sY[y];
            rY2        = rY[y];
            srcYIdx2 = srcYIdx1+1;
            rY1        = FXP_UNIT_LINEAR - rY2;

            *pOutImage ++
                = ( unsigned char ) (
                    (   rY1 * (
                                rX1 * val_single_image( in_pImage, srcXIdx1, srcYIdx1, in_Width, in_Height )
                                + rX2 * val_single_image( in_pImage, srcXIdx2, srcYIdx1, in_Width, in_Height ) )
                    + rY2 * (
                                rX1 * val_single_image( in_pImage, srcXIdx1, srcYIdx2, in_Width, in_Height)
                                + rX2 * val_single_image( in_pImage, srcXIdx2, srcYIdx2, in_Width, in_Height ) )
                    ) / FXP_UNIT_LINEAR / FXP_UNIT_LINEAR
                );
        }
    }
#endif
	return resultCode;
	
}

/*****************************************************************************/
/** AffineSingleNoRotHalf : 1/2缩小（没有旋转；2X2平均）
/** AffineSingleNoRotHalf : 1/2 reduction (non-rotation; 2x2 average ）
 *      图像一角固定，按照scale值移动坐标原点位置
 *      Fix image corner, Shift the starting point position according to the scale
 *      坐标系：输入图像的原点坐标（0，0）输入图像的像数大小为1
 *      Coordinate system: Starting point coordinates (0,0) of input image, 1 size of pixels of input image
 *      → 比例变换后的原点坐标（1/2/in_Scale-1/2, 1/2/in_Scale-1/2）
 *      → Starting point coordinates of image after scale is converted（1/2/in_Scale-1/2, 1/2/in_Scale-1/2）
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param in_pImage
 * [in]输入图像（1 通道）
 * [in]Input image ( 1 channel )
 * @param in_Width
 * [in]输入图像的X方向像数数
 * [in] Number of horizontal direction pixels of input images 
 * @param in_Height
 * [in]输入图像的Y方向像数数
 * [in] Number of vertical direction pixels of input images
 * @param io_DstImage
 * [io]变换后图像
 * [io]Image after converted
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
IFACE_ERROR_CODE AffineSingleNoRotHalf( FaceRecognitionHandle* in_pInstance, 
							 const unsigned char* in_pImage, 
                             const long in_Width, const long in_Height,
                             unsigned char* io_DstImage ) {
    IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
	const unsigned char* pSrcImage1;
	const unsigned char* pSrcImage2;
    unsigned char* pDstImage;
    long srcWidth, srcHeight, dstWidth, dstHeight;
    long x, y;

    srcWidth  = in_Width;
    srcHeight = in_Height;
    dstWidth  = srcWidth  >> 1;
    dstHeight = srcHeight >> 1;

    for ( y = 0 ; y < dstHeight ; y++ ) {
        pSrcImage1 = in_pImage + ( 2 * y     ) * srcWidth;
        pSrcImage2 = in_pImage + ( 2 * y + 1 ) * srcWidth;
        pDstImage  = io_DstImage + y * dstWidth;
        for ( x = 0 ; x < dstWidth ; x++ ) {
            *pDstImage ++ = ( unsigned char ) ( ( *( pSrcImage1 ) + *( pSrcImage1 + 1 ) + *( pSrcImage2 ) + *( pSrcImage2 + 1 ) ) >> 2 );
            pSrcImage1 += 2;
            pSrcImage2 += 2;
        }
    }
	return resultCode;
}

/*****************************************************************************/
/** MakeMultiResolution : 制作1, 1/2, 1/4, 1/8...的缩小图像（单纯平均）
/** MakeMultiResolution : 1, 1/2, 1/4, 1/8...The reduction image is made. (simple average)
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param io_multi_image
 * [io]多重分辨率图像（1 通道）
 * [io]Multiple resolution image ( 1 channel )
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
IFACE_ERROR_CODE MakeMultiResolution( FaceRecognitionHandle* in_pInstance, 
									 MultiUcharImage* io_pMultiImage ) {
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
    long dstWidth, dstHeight;
    long srcWidth, srcHeight;
    long x, y;
    long level;
    unsigned char* pSrcImage1;
	unsigned char* pSrcImage2;
	unsigned char* pDstImage;

	/* 参数检查	*/
	/* Parameter check */
	if( in_pInstance == NULL || io_pMultiImage == NULL ) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}


    for ( level = 1 ; level < io_pMultiImage->NumberOfLevels ; level ++ ) {
        srcWidth = io_pMultiImage->images[level - 1].w;
        srcHeight = io_pMultiImage->images[level - 1].h;
        dstWidth = io_pMultiImage->images[level].w;
        dstHeight = io_pMultiImage->images[level].h;

        for ( y = 0 ; y < dstHeight ; y++ ) {
            pSrcImage1 = io_pMultiImage->images[level - 1].img + ( 2 * y     ) * srcWidth;
            pSrcImage2 = io_pMultiImage->images[level - 1].img + ( 2 * y + 1 ) * srcWidth;
            pDstImage  = io_pMultiImage->images[level].img + y * dstWidth;
            for ( x = 0 ; x < dstWidth ; x++ ) {
                *pDstImage ++ = ( unsigned char ) ( ( *( pSrcImage1 ) + *( pSrcImage1 + 1 ) + *( pSrcImage2 ) + *( pSrcImage2 + 1 ) ) >> 2 );
                pSrcImage1 += 2;
                pSrcImage2 += 2;
            }
        }
    }

	return resultCode;
}


/*****************************************************************************/
/** NormalizeLocalDeviation2 : 对比度(亮度差)正则化处理
/** NormalizeLocalDeviation2 : Normalize the contrast 
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param io_img
 * [io]输入、输出图像（1通道）
 * [io]Input output image ( 1 channel )
 * @param in_Width
 * [in]输入图像的X方向像数数
 * [in] Number of horizontal direction pixels of input images 
 * @param in_Height
 * [in]输入图像的Y方向像数数
 * [in] Number of vertical direction pixels of input images
 * @param in_RectSize
 * [in]用于求和及和的平方的矩形的纵横方向的像数数
 * [in]Number of pixels in vertical and horizontal direction of rectangle that is shown the range of sum and sum of squares
 * @param io_pBufImage1
 * [io]处理时使用的Buffer 1（像数数同输入输出图像）
 * [io]Buffer 1 used for processing( Number of pixels are the same as I/O image)
 * @param io_pBufImage2
 * [io]处理时使用的Buffer 2（像数数同输入输出图像）
 * [io]Buffer 2 used for processing( Number of pixels are the same as I/O image)
 * @param io_pBufImage3
 * [io]处理时使用的Buffer 3（像数数同输入输出图像）
 * [io]Buffer 3 used for processing( Number of pixels are the same as I/O image)
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
/* #include "normalize.txt" */ /*20050210_7*/
/* #define GAMMA8BIT */ /*20050318*/
#ifdef GAMMA8BIT/* 20050318 */
#include "normalize2_8bit.txt"/* 20050318 */
#else/*20050318*/
#include "normalize2.txt"/* 20050210_7 *//* 20050318 */
#endif/* 20050318 */
#define FXIS_T_050222_NO8/* 消减表达式 20050309 */
                         /* Reduction of assignment expression 20050309 */
IFACE_ERROR_CODE NormalizeLocalDeviation2( FaceRecognitionHandle* in_pInstance, unsigned char* io_pImage, 
							   const long in_Width, const long in_Height, const long in_RectSize,
							   long* io_pBufImage1, long* io_pBufImage2, long* io_pBufImage3 ) {
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
	long x, y;
	long* img1;
	long* img2;
	long* img0;
	unsigned char* src;
	long* p1;
	long* p2;
	long* p0;
	long num, num2;
	long val;
#ifdef FXIS_T_050222_NO8 /*20050309*/
	long variance;
    long cnt = in_Width * in_Height;
#else /*20050309*/
	long variance, mean, sqmean;
  	long coef;
#endif /*20050309*/



	/* 参数检查	*/
    /* Parameter check */
	if( in_pInstance == NULL || io_pImage == NULL ||
		io_pBufImage1 == NULL || io_pBufImage2 == NULL || 
		io_pBufImage3 == NULL) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}

	img1 = io_pBufImage1;
	img2 = io_pBufImage2;
	img0 = io_pBufImage3;

	src = io_pImage;
	p0 = img0;
	for ( y = 0 ; y < in_Height ; y++ ) {
		for ( x = 0 ; x < in_Width ; x++ ) {
			*( p0 ) = normalize_gamma2[ *( src ) ];
			src++;
			p0++;
		}
	}

	resultCode = SumRect( in_pInstance, img0, in_Width, in_Height, in_RectSize, in_RectSize, img1, img2 );
	if( resultCode != IFACE_ERR_SUCCESS_ID ) {
		
		return resultCode;
	}

	num  = SQR( in_RectSize );
	num2 = SQR( num );
	src = io_pImage;
	p0 = img0;
	p1 = img1;
	p2 = img2;
#ifdef FXIS_T_050222_NO8 /*20050309*/
    for ( x = 0 ; x < cnt ; x++ ) {			
        val = *( p0 );
		val -=( *( p1 )/num);
		variance = (num * (*p2) - SQR( *p1 ))/num2;

		if ( variance < NORMALIZE_TBL_LENGTH1 ) {
			val *= normalize_tbl1[variance];
		}
		else if ( variance >> NORMALIZE_TBL_SHIFT2 < NORMALIZE_TBL_LENGTH2 ) {
			val *= normalize_tbl2[variance>>NORMALIZE_TBL_SHIFT2];
		}
		else if ( variance >> NORMALIZE_TBL_SHIFT3 < NORMALIZE_TBL_LENGTH3 ) {
			val *= normalize_tbl3[variance>>NORMALIZE_TBL_SHIFT3];
		}
		else if ( variance >> NORMALIZE_TBL_SHIFT4 < NORMALIZE_TBL_LENGTH4 ) {
			val *= normalize_tbl4[variance>>NORMALIZE_TBL_SHIFT4];
		}
		else {
			val *= normalize_tbl4[NORMALIZE_TBL_LENGTH4-1];
		}

        val += 32768;
        val >>= 8;
        *( src ) = ( unsigned char )clip( val );

		src++;
		p0++;
		p1++;
		p2++;
    }

#endif /*20050309*/

	return resultCode;
}


/*****************************************************************************/
/** SumRect : 做成和及平方和的图像
/** SumRect : The image of sum and sum of squares is made 
 * @param in_pInstance			
 * [in] 实例句柄
 *        handle of instance
 * @param in_src
 * [in]输入图像（1 通道）
 * [in]Input image ( 1 channel )
 * @param in_SrcWidth
 * [in]输入图像的X方向像数数
 * [in] Number of horizontal direction pixels of input images
 * @param in_SrcHeight
 * [in]输入图像的Y方向像数数
 * [in] Number of vertical direction pixels of input images
 * @param in_RectWidth
 * [in]求和及平方和的矩形框的X方向像数数
 * [in]Number of pixels in horizontal direction of rectangle that show the range of sum and sum of squares
 * @param in_RectHeight
 * [in]求和及平方和的矩形框的Y方向像数数
 * [in]Number of pixels in vertical direction of rectangle that is shown the range of sum and sum of squares
 * @param io_pSumImage
 * [io]和的图像（1 通道）
 * [io]Sum image (1 channel )
 * @param io_pSqSumImage
 * [io]平方和的图像（1通道）
 * [io] Sum of squares image(1 channel )
 * @return IFACE_ERROR_CODE
 *****************************************************************************/
#define FXIS_T_050202_NO1/* 如果使用了/03+/QaxNB，处理处理为20050309 */
                         /* Action of apprication error 20050309, when /03+/QaxNB is used */
IFACE_ERROR_CODE SumRect( FaceRecognitionHandle* in_pInstance, 
			  const long* in_pSrcImage, 
              const long in_SrcWidth, const long in_SrcHeight,
              const long in_RectWidth, const long in_RectHeight, 
			  long* io_pSumImage, long* io_pSqSumImage ) {
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
    long x, y;
    long halfWidth, halfHeight;
	const long* pi_c;
	const long* pi_c1;
	const long* pi_c2;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
	long* po_sum;
	long* po_sqsum;
    long* pi_sumHead;
	long* pi_sqsumHead;
    long index = 0;
#else /* 20050309 */
	long* po_sum;
	long* po_sqsum;
#endif /* 20050309 */
	long val, val_p1, val_m1, val_p2, val_m2;

    halfWidth = in_RectWidth / 2;
    halfHeight = in_RectHeight / 2;

	pi_c     = in_pSrcImage;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
	pi_sumHead = po_sum   = io_pSumImage;
	pi_sqsumHead = po_sqsum = io_pSqSumImage;
#else /* 20050309 */
	po_sum   = io_pSumImage;
	po_sqsum = io_pSqSumImage;
#endif /* 20050309 */

    if ( ( in_RectWidth >= in_SrcWidth ) || ( in_RectHeight >= in_SrcHeight ) ) {
		/* ===最上端=== */
		/* ===Uppermost=== */
		/* 最左端 */
		/* High-order end */
		*( po_sum   ) = halfHeight * halfWidth *    ( *( pi_c ) );
		*( po_sqsum ) = halfHeight * halfWidth * SQR( *( pi_c ) );
		for ( x = 0 ; x <= halfWidth ; x++ ) {
			*( po_sum   ) += ( halfHeight *    ( *( pi_c + MIN( x, in_SrcWidth-1 ) ) ) );
			*( po_sqsum ) += ( halfHeight * SQR( *( pi_c + MIN( x, in_SrcWidth-1 ) ) ) );
		}
		for ( y = 0 ; y <= halfHeight ; y++ ) {
			*( po_sum   ) += ( halfWidth *    ( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth ) ) );
			*( po_sqsum ) += ( halfWidth * SQR( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth ) ) );
		}
		for ( y = 0 ; y <= halfHeight ; y++ ) {
			for ( x = 0 ; x <= halfWidth ; x++ ) {
				*( po_sum   ) +=    ( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth + MIN( x, in_SrcWidth-1 ) ) );
				*( po_sqsum ) += SQR( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth + MIN( x, in_SrcWidth-1 ) ) );
			}
		}
		pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
        index++;
		po_sum = pi_sumHead + index;
		po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		po_sum ++;
		po_sqsum ++;
#endif /* 20050309 */

		/* 所有 */
		/* Total */
		for ( x = 1 ; x < in_SrcWidth ; x++ ) {
			val_m1 = ( *( pi_c + MIN( halfWidth, in_SrcWidth-1-x ) ) ) - ( *( pi_c + MAX( -halfWidth-1, -x ) ) );
			val_p1 = ( *( pi_c + MIN( halfWidth, in_SrcWidth-1-x ) ) ) + ( *( pi_c + MAX( -halfWidth-1, -x ) ) );
			val = halfHeight * val_m1; 
			*( po_sum   ) = ( *( po_sum   - 1 ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + val * val_p1;
			for ( y = 0 ; y <= halfHeight ; y++ ) {
				val_m1 = ( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth + MIN( halfWidth, in_SrcWidth-1-x ) ) ) 
					   - ( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth + MAX( -halfWidth-1, -x ) ) );
				val_p1 = ( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth + MIN( halfWidth, in_SrcWidth-1-x ) ) ) 
					   + ( *( pi_c + MIN( y, in_SrcHeight-1 ) * in_SrcWidth + MAX( -halfWidth-1, -x ) ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
            index++;
		    po_sum = pi_sumHead + index;
		    po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		    po_sum ++;
		    po_sqsum ++;
#endif /* 20050309 */
		}

		/* ===所有=== */
		/* ===Total=== */
        for ( y = 1 ; y < in_SrcHeight ; y++ ) {
			/* 最左端 */
			/* High-order end  */
			pi_c1 = pi_c + MAX( -halfHeight-1, -y )        * in_SrcWidth;
			pi_c2 = pi_c + MIN( halfHeight, in_SrcHeight-1-y ) * in_SrcWidth;
			val_m1 = ( *( pi_c2 ) ) - ( *( pi_c1 ) );
			val_p1 = ( *( pi_c2 ) ) + ( *( pi_c1 ) );
			val = halfWidth * val_m1;
			*( po_sum   ) = ( *( po_sum   - in_SrcWidth ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - in_SrcWidth ) ) + val * val_p1;
			for ( x = 0 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + MIN( x, in_SrcWidth-1 ) ) ) - ( *( pi_c1 + MIN( x, in_SrcWidth-1 ) ) );
				val_p1 = ( *( pi_c2 + MIN( x, in_SrcWidth-1 ) ) ) + ( *( pi_c1 + MIN( x, in_SrcWidth-1 ) ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
            index++;
		    po_sum = pi_sumHead + index;
		    po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		    po_sum ++;
		    po_sqsum ++;
#endif /* 20050309 */

			/* 所有 */
			/* Total */
			for ( x = 1 ; x < in_SrcWidth ; x++ ) {
				pi_c1 = pi_c + MAX( -halfHeight-1, -y )        * in_SrcWidth;
				pi_c2 = pi_c + MIN( halfHeight, in_SrcHeight-1-y ) * in_SrcWidth;
				val_m1 = ( *( pi_c2 + MIN( halfWidth, in_SrcWidth-1-x ) ) ) - ( *( pi_c2 + MAX( -halfWidth-1, -x ) ) );
				val_p1 = ( *( pi_c2 + MIN( halfWidth, in_SrcWidth-1-x ) ) ) + ( *( pi_c2 + MAX( -halfWidth-1, -x ) ) );
				val_m2 = ( *( pi_c1 + MAX( -halfWidth-1, -x ) ) ) - ( *( pi_c1 + MIN( halfWidth, in_SrcWidth-1-x ) ) );
				val_p2 = ( *( pi_c1 + MAX( -halfWidth-1, -x ) ) ) + ( *( pi_c1 + MIN( halfWidth, in_SrcWidth-1-x ) ) );

				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum - in_SrcWidth ) )   - ( *( po_sum - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;

				pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		        po_sum ++;
		        po_sqsum ++;
#endif /* 20050309 */
			}
		}
	}
	else {
		/* ===最上端=== */
		/* ===Uppermost=== */
		/* 最左端 */
		/* High-order end */
		*( po_sum   ) = halfHeight * halfWidth *    ( *( pi_c ) );
		*( po_sqsum ) = halfHeight * halfWidth * SQR( *( pi_c ) );
		for ( x = 0 ; x <= halfWidth ; x++ ) {
			*( po_sum   ) += ( halfHeight *    ( *( pi_c + x ) ) );
			*( po_sqsum ) += ( halfHeight * SQR( *( pi_c + x ) ) );
		}
		for ( y = 0 ; y <= halfHeight ; y++ ) {
			*( po_sum   ) += ( halfWidth *    ( *( pi_c + y * in_SrcWidth ) ) );
			*( po_sqsum ) += ( halfWidth * SQR( *( pi_c + y * in_SrcWidth ) ) );
		}
		for ( y = 0 ; y <= halfHeight ; y++ ) {
			for ( x = 0 ; x <= halfWidth ; x++ ) {
				*( po_sum   ) +=    ( *( pi_c + y * in_SrcWidth + x ) );
				*( po_sqsum ) += SQR( *( pi_c + y * in_SrcWidth + x ) );
			}
		}
		pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
        index++;
		po_sum = pi_sumHead + index;
		po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		po_sum ++;
		po_sqsum ++;
#endif /* 20050309 */

		/* Left of center */
		for ( x = 1 ; x <= halfWidth ; x++ ) {
			val_m1 = ( *( pi_c + halfWidth ) ) - ( *( pi_c - x ) );
			val_p1 = ( *( pi_c + halfWidth ) ) + ( *( pi_c - x ) );
			val = halfHeight * val_m1;
			*( po_sum   ) = ( *( po_sum   - 1 ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + val * val_p1;
			for ( y = 0 ; y <= halfHeight ; y++ ) {
				val_m1 = ( *( pi_c + y * in_SrcWidth + halfWidth ) ) - ( *( pi_c + y * in_SrcWidth - x ) );
				val_p1 = ( *( pi_c + y * in_SrcWidth + halfWidth ) ) + ( *( pi_c + y * in_SrcWidth - x ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
            index++;
		    po_sum = pi_sumHead + index;
		    po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		    po_sum ++;
		    po_sqsum ++;
#endif /* 20050309 */
		}
		/* 中间 */
		/* Center */
		for ( x = halfWidth + 1 ; x <= in_SrcWidth - halfWidth - 2 ; x++ ) {
			val_m1 = ( *( pi_c + halfWidth ) ) - ( *( pi_c - halfWidth - 1 ) );
			val_p1 = ( *( pi_c + halfWidth ) ) + ( *( pi_c - halfWidth - 1 ) );
			val = halfHeight * val_m1;
			*( po_sum   ) = ( *( po_sum   - 1 ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + val * val_p1;
			for ( y = 0 ; y <= halfHeight ; y++ ) {
				val_m1 = ( *( pi_c + y * in_SrcWidth + halfWidth ) ) - ( *( pi_c + y * in_SrcWidth - halfWidth - 1 ) );
				val_p1 = ( *( pi_c + y * in_SrcWidth + halfWidth ) ) + ( *( pi_c + y * in_SrcWidth - halfWidth - 1 ) );
				*( po_sum )   += val_m1;
				*( po_sqsum ) += val_m1 * val_p1;
			}
			pi_c ++;
#ifdef FXIS_T_050202_NO1 /* 20050309 */
            index++;
		    po_sum = pi_sumHead + index;
		    po_sqsum = pi_sqsumHead + index;
#else /* 20050309 */
		    po_sum ++;
		    po_sqsum ++;
#endif /* 20050309 */
		}
		/* 偏右 */
		/* Right of center */
		for ( x = in_SrcWidth - halfWidth - 1 ; x < in_SrcWidth ; x++ ) {
			val_m1 = ( *( pi_c + in_SrcWidth - 1 - x ) ) - ( *( pi_c - halfWidth - 1 ) );
			val_p1 = ( *( pi_c + in_SrcWidth - 1 - x ) ) + ( *( pi_c - halfWidth - 1 ) );
			val = halfHeight * val_m1;
			*( po_sum   ) = ( *( po_sum   - 1 ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + val * val_p1;
			for ( y = 0 ; y <= halfHeight ; y++ ) {
				val_m1 = ( *( pi_c + y * in_SrcWidth + in_SrcWidth - 1- x ) ) - ( *( pi_c + y * in_SrcWidth - halfWidth - 1 ) );
				val_p1 = ( *( pi_c + y * in_SrcWidth + in_SrcWidth - 1- x ) ) + ( *( pi_c + y * in_SrcWidth - halfWidth - 1 ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
            index++;
		    po_sum = pi_sumHead + index;
		    po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		    po_sum ++;
		    po_sqsum ++;
#endif /*20050309*/
		}

		/* ===偏上=== */
		/* === Up of center === */
		pi_c1 = pi_c - in_SrcWidth;
		pi_c2 = pi_c + halfHeight * in_SrcWidth;
		for ( y = 1 ; y <= halfHeight ; y++ ) {
			/* 最左端 */
			/* High-order end */
			val_m1 = ( *( pi_c2 ) ) - ( *( pi_c1 ) );
			val_p1 = ( *( pi_c2 ) ) + ( *( pi_c1 ) );
			val = halfWidth * val_m1;
			*( po_sum   ) = ( *( po_sum   - in_SrcWidth ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - in_SrcWidth ) ) + val * val_p1;
			for ( x = 0 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + x ) ) - ( *( pi_c1 + x ) );
				val_p1 = ( *( pi_c2 + x ) ) + ( *( pi_c1 + x ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c1 ++;
			pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
            index++;
		    po_sum = pi_sumHead + index;
		    po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		    po_sum ++;
		    po_sqsum ++;
#endif /*20050309*/

			/* 偏左 */
			/* Left-of-center */
			for ( x = 1 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + halfWidth ) ) - ( *( pi_c2 - x ) );
				val_p1 = ( *( pi_c2 + halfWidth ) ) + ( *( pi_c2 - x ) );
				val_m2 = ( *( pi_c1 - x ) ) - ( *( pi_c1 + halfWidth ) );
				val_p2 = ( *( pi_c1 - x ) ) + ( *( pi_c1 + halfWidth ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			/* 中间 */
			/* Center */
			for ( x = halfWidth + 1 ; x <= in_SrcWidth - halfWidth - 2 ; x++ ) {
				val_m1 = ( *( pi_c2 + halfWidth ) ) - ( *( pi_c2 - halfWidth - 1 ) );
				val_p1 = ( *( pi_c2 + halfWidth ) ) + ( *( pi_c2 - halfWidth - 1 ) );
				val_m2 = ( *( pi_c1 - halfWidth - 1 ) ) - ( *( pi_c1 + halfWidth ) );
				val_p2 = ( *( pi_c1 - halfWidth - 1 ) ) + ( *( pi_c1 + halfWidth ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			/* 偏右 */
			/* Right-of-center */
			for ( x = in_SrcWidth - halfWidth - 1 ; x < in_SrcWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + in_SrcWidth - 1 - x ) ) - ( *( pi_c2 - halfWidth - 1 ) );
				val_p1 = ( *( pi_c2 + in_SrcWidth - 1 - x ) ) + ( *( pi_c2 - halfWidth - 1 ) );
				val_m2 = ( *( pi_c1 - halfWidth - 1 ) ) - ( *( pi_c1 + in_SrcWidth - 1 - x ) );
				val_p2 = ( *( pi_c1 - halfWidth - 1 ) ) + ( *( pi_c1 + in_SrcWidth - 1 - x ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			pi_c1 -= in_SrcWidth;
		}

		/* ===中间=== */
		/* === Center === */
		for ( y = halfHeight + 1 ; y <= in_SrcHeight - halfHeight - 2 ; y++ ) {
			/* 最左端 */
			/* High-order end */
			val_m1 = ( *( pi_c2 ) ) - ( *( pi_c1 ) );
			val_p1 = ( *( pi_c2 ) ) + ( *( pi_c1 ) );
			val = halfWidth * val_m1;
			*( po_sum   ) = ( *( po_sum   - in_SrcWidth ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - in_SrcWidth ) ) + val * val_p1;
			for ( x = 0 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + x ) ) - ( *( pi_c1 + x ) );
				val_p1 = ( *( pi_c2 + x ) ) + ( *( pi_c1 + x ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c1 ++;
			pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
            index++;
            po_sum = pi_sumHead + index;
	        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
	        po_sum ++;
	        po_sqsum ++;
#endif /*20050309*/

			/* 偏左 */
			/* Left-of-center */
			for ( x = 1 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + halfWidth ) ) - ( *( pi_c2 - x ) );
				val_p1 = ( *( pi_c2 + halfWidth ) ) + ( *( pi_c2 - x ) );
				val_m2 = ( *( pi_c1 - x ) ) - ( *( pi_c1 + halfWidth ) );
				val_p2 = ( *( pi_c1 - x ) ) + ( *( pi_c1 + halfWidth ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			/* 中间 */
			/* Center */
			for ( x = halfWidth + 1 ; x <= in_SrcWidth - halfWidth - 2 ; x++ ) {
				val_m1 = ( *( pi_c2 + halfWidth ) ) - ( *( pi_c2 - halfWidth - 1 ) );
				val_p1 = ( *( pi_c2 + halfWidth ) ) + ( *( pi_c2 - halfWidth - 1 ) );
				val_m2 = ( *( pi_c1 - halfWidth - 1 ) ) - ( *( pi_c1 + halfWidth ) );
				val_p2 = ( *( pi_c1 - halfWidth - 1 ) ) + ( *( pi_c1 + halfWidth ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			/* 偏右 */
			/* Right-of-center */
			for ( x = in_SrcWidth - halfWidth - 1 ; x < in_SrcWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + in_SrcWidth - 1 - x ) ) - ( *( pi_c2 - halfWidth - 1 ) );
				val_p1 = ( *( pi_c2 + in_SrcWidth - 1 - x ) ) + ( *( pi_c2 - halfWidth - 1 ) );
				val_m2 = ( *( pi_c1 - halfWidth - 1 ) ) - ( *( pi_c1 + in_SrcWidth - 1 - x ) );
				val_p2 = ( *( pi_c1 - halfWidth - 1 ) ) + ( *( pi_c1 + in_SrcWidth - 1 - x ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
		}

		/* ===偏下=== */
		/* ===Lowered === */
		for ( y = in_SrcHeight - halfHeight - 1 ; y < in_SrcHeight ; y++ ) {
			/* 最左端 */
			/* High-order end */
			val_m1 = ( *( pi_c2 ) ) - ( *( pi_c1 ) );
			val_p1 = ( *( pi_c2 ) ) + ( *( pi_c1 ) );
			val = halfWidth * val_m1;
			*( po_sum   ) = ( *( po_sum   - in_SrcWidth ) ) + val;
			*( po_sqsum ) = ( *( po_sqsum - in_SrcWidth ) ) + val * val_p1;
			for ( x = 0 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + x ) ) - ( *( pi_c1 + x ) );
				val_p1 = ( *( pi_c2 + x ) ) + ( *( pi_c1 + x ) );
				*( po_sum   ) += val_m1;
				*( po_sqsum ) += ( val_m1 * val_p1 );
			}
			pi_c1 ++;
			pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/

			/* 偏左 */
			/* Left-of-center  */
			for ( x = 1 ; x <= halfWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + halfWidth ) ) - ( *( pi_c2 - x ) );
				val_p1 = ( *( pi_c2 + halfWidth ) ) + ( *( pi_c2 - x ) );
				val_m2 = ( *( pi_c1 - x ) ) - ( *( pi_c1 + halfWidth ) );
				val_p2 = ( *( pi_c1 - x ) ) + ( *( pi_c1 + halfWidth ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			/* 中间 */
			/* Center */
			for ( x = halfWidth + 1 ; x <= in_SrcWidth - halfWidth - 2 ; x++ ) {
				val_m1 = ( *( pi_c2 + halfWidth ) ) - ( *( pi_c2 - halfWidth - 1 ) );
				val_p1 = ( *( pi_c2 + halfWidth ) ) + ( *( pi_c2 - halfWidth - 1 ) );
				val_m2 = ( *( pi_c1 - halfWidth - 1 ) ) - ( *( pi_c1 + halfWidth ) );
				val_p2 = ( *( pi_c1 - halfWidth - 1 ) ) + ( *( pi_c1 + halfWidth ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			/* 偏右 */
			/* Right-of-center  */
			for ( x = in_SrcWidth - halfWidth - 1 ; x < in_SrcWidth ; x++ ) {
				val_m1 = ( *( pi_c2 + in_SrcWidth - 1 - x ) ) - ( *( pi_c2 - halfWidth - 1 ) );
				val_p1 = ( *( pi_c2 + in_SrcWidth - 1 - x ) ) + ( *( pi_c2 - halfWidth - 1 ) );
				val_m2 = ( *( pi_c1 - halfWidth - 1 ) ) - ( *( pi_c1 + in_SrcWidth - 1 - x ) );
				val_p2 = ( *( pi_c1 - halfWidth - 1 ) ) + ( *( pi_c1 + in_SrcWidth - 1 - x ) );
				*( po_sum   ) = ( *( po_sum   - 1 ) ) + ( *( po_sum   - in_SrcWidth ) ) - ( *( po_sum   - in_SrcWidth - 1 ) )
					+ val_m1 + val_m2;
				*( po_sqsum ) = ( *( po_sqsum - 1 ) ) + ( *( po_sqsum - in_SrcWidth ) ) - ( *( po_sqsum - in_SrcWidth - 1 ) )
					+ val_m1 * val_p1 + val_m2 * val_p2;
				pi_c1 ++;
				pi_c2 ++;
#ifdef FXIS_T_050202_NO1 /*20050309*/
                index++;
		        po_sum = pi_sumHead + index;
		        po_sqsum = pi_sqsumHead + index;
#else /*20050309*/
		        po_sum ++;
		        po_sqsum ++;
#endif /*20050309*/
			}
			pi_c2 -= in_SrcWidth;
		}
	}
	return resultCode;
}


/* End of file */
