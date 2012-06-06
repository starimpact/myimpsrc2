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
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "defines.h"
//#include "FaceDetectMain.h"
//#include "DetectFaces.h"
#include "FaceDetectDef.h"

/*-----------------------------------------------------------------*
 * 常数
 *-----------------------------------------------------------------*/
/* ==== RGB ==== */
#define CHID_R     0
#define CHID_G     1
#define CHID_B     2
/* ==== YCbCr ==== */
#define CHID_Y     0
#define CHID_CB    1
#define CHID_CR    2


/*-----------------------------------------------------------------*
 * 宏定义
 *-----------------------------------------------------------------*/
/* ==== 像数值存取 ==== */
/* ---- single channel image ---- */
#define val_single_image( img, x, y, x_num, y_num )  ( img[(y) * (x_num) + (x)] )

/* ---- Normal access ( pixel order ) ---- */
/* normal */
#define val_pixel_image( img, x, y, ch, x_num, y_num, ch_num )  \
                                                     ( img[( (y) * (x_num) + (x) ) * (ch_num) + ch] )

/* rgb from rgb */
#define val_r_rgb( img, x, y, x_num, y_num )         ( val_pixel_image( img, x, y, CHID_R, x_num, y_num, 3 ) )
#define val_g_rgb( img, x, y, x_num, y_num )         ( val_pixel_image( img, x, y, CHID_G, x_num, y_num, 3 ) )
#define val_b_rgb( img, x, y, x_num, y_num )         ( val_pixel_image( img, x, y, CHID_B, x_num, y_num, 3 ) )

/* ycbcr from rgb */
#define val_y_rgb( img, x, y, x_num, y_num )        \
    ( (  77 * val_r_rgb( img, x, y, x_num, y_num )  \
      + 151 * val_g_rgb( img, x, y, x_num, y_num )  \
      +  28 * val_b_rgb( img, x, y, x_num, y_num ) ) >> 8 )

/* ---- Clip ---- */
#define limit( x, a, b )  ( ( ( x ) < ( a ) ) ? ( a ) : ( ( ( x ) > ( b ) ) ? ( b ) : ( x ) ) )
#define clip( x )         limit( x, 0, 255 )


typedef struct {
    unsigned char* img;  /**< 图像数据 */
    long h;              /**< 高 */
    long w;              /**< 宽 */
    long scale;
} UcharImage;

typedef struct {
	UcharImage images[MAX_MULTIRESOLUTION_LEVELS];
	long NumberOfLevels;
} MultiUcharImage;

typedef struct {
	MultiUcharImage series[MAX_MULTIRESOLUTION_SERIES];
	long NumberOfSeries;
} MultiUcharImages;


#ifdef __cplusplus
extern "C" {
#endif


IFACE_ERROR_CODE ConvertRGBtoY( FaceRecognitionHandle* in_pInstance, 
				    const unsigned char* in_pImage, 
				    const long in_Width, const long in_Height, 
					unsigned char* io_pYImage );

IFACE_ERROR_CODE AffineSingleNoRot( FaceRecognitionHandle* in_pInstance, 
					    const unsigned char* in_pImage, 
                        const long in_Width, const long in_Height,
                        const short in_Scale, /* FXP_UNIT_SCALE倍値 */
                        const long in_DstWidth, const long in_DstHeight,
                        unsigned char* io_DstImage, unsigned char* io_TmpImage );

IFACE_ERROR_CODE MakeMultiResolution( FaceRecognitionHandle* in_pInstance, MultiUcharImage* io_pMultiImage );

IFACE_ERROR_CODE NormalizeLocalDeviation2( FaceRecognitionHandle* in_pInstance, unsigned char* io_pImage, 
							   const long in_Width, const long in_Height, const long in_RectSize,
							   long* io_pBufImage1, long* io_pBufImage2, long* io_pBufImage3 );



#ifdef __cplusplus
}
#endif


#endif /* __IMAGE_H__ */


/* End of file */
