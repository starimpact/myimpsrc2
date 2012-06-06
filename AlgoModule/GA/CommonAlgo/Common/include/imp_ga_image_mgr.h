#ifndef _IMP_GA_IMAGE_MGR_H_
#define _IMP_GA_IMAGE_MGR_H_

#include "imp_algo_type.h"
#include "imp_ga_image_algo.h"
#include "imp_ga_math.h"

#ifdef __cplusplus
extern "C"
{
#endif


IMP_EXPORTS IMP_VOID IMP_RGBImageCreate( RGB_IMAGE_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_RGBImageDestroy( RGB_IMAGE_S *pstImg, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_RGBImageClear( RGB_IMAGE_S *pstImg );

IMP_EXPORTS IMP_VOID IMP_RGBImageClone( RGB_IMAGE_S *pstSrc, RGB_IMAGE_S *pstDst );

IMP_EXPORTS IMP_VOID IMP_RGBImageGetPixelVal( RGB_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y, PIXEL_S *pstPixel );

IMP_EXPORTS IMP_VOID IMP_RGBImageSetPixelVal( RGB_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y, PIXEL_S *pstPixel );


IMP_EXPORTS IMP_VOID IMP_GrayImageCreate( GRAY_IMAGE_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_GrayImageDestroy( GRAY_IMAGE_S *pstImg, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_GrayImageClear( GRAY_IMAGE_S *pstImg );

IMP_EXPORTS IMP_VOID IMP_GrayImageSet( GRAY_IMAGE_S *pstImg, IMP_U8 u8Value );

IMP_EXPORTS IMP_VOID IMP_GrayImageClone( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst );

IMP_EXPORTS IMP_VOID IMP_GrayImageClone2( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst );

IMP_EXPORTS IMP_U8 IMP_GrayImageGetPixelVal( GRAY_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y );

IMP_EXPORTS IMP_VOID IMP_GrayImageSetPixelVal( GRAY_IMAGE_S *pstImg, IMP_S32 s32X, IMP_S32 s32Y, IMP_U8 u8Value );

IMP_EXPORTS IMP_VOID IMP_GrayImage16Create( GRAY_IMAGE16_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_GrayImage16Destroy( GRAY_IMAGE16_S *pstImg, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_GrayImage16Clear( GRAY_IMAGE16_S *pstImg );

IMP_EXPORTS IMP_VOID IMP_TmplImageCreate( TMPL_IMAGE_S *pstImg, IMP_S32 s32BufLen, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_TmplImageDestroy( TMPL_IMAGE_S *pstImg, IMP_VOID *pMmgr );

IMP_EXPORTS IMP_VOID IMP_TmplImageClear( TMPL_IMAGE_S *pstImg );

IMP_EXPORTS IMP_VOID IMP_TmplImageClone( TMPL_IMAGE_S *pstSrc, TMPL_IMAGE_S *pstDst );

IMP_EXPORTS IMP_VOID IMP_ClearImagePerimeter( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_U8 *pstImgDat );

IMP_EXPORTS IMP_VOID IMP_GrayImageCloneb( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst );

IMP_EXPORTS IMP_VOID IMP_GrayImageBinary( GRAY_IMAGE_S *pstImg, IMP_S32 s32Th, IMP_S32 s32Value );

IMP_EXPORTS IMP_VOID IMP_GrayImageBinarys( GRAY_IMAGE_S *pstImg );

IMP_EXPORTS IMP_VOID IMP_CheckBinary( GRAY_IMAGE_S *pstImg );

IMP_EXPORTS IMP_S32 IMP_GrayImageCompare( GRAY_IMAGE_S *pstSrc, GRAY_IMAGE_S *pstDst );

IMP_EXPORTS IMP_VOID IMP_CalcMotionDiffImage( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_S32 s32ThetaThld,
				IMP_U8 *pstImgCurData, IMP_U8 *pstImgPreData, IMP_U8 *pstImgDiffData );



enum
{
	IMP_SHORT_TERM_BG,
	IMP_LONG_TERM_BG,
};


#define IMP_IMGHIS_LEN	1


typedef struct impHISTORY_IMAGE_S
{
	IMP_U32 u32LastIndex;
	GRAY_IMAGE_S stImages[IMP_IMGHIS_LEN];
	IMP_U32 u32Times[IMP_IMGHIS_LEN];
} HISTORY_IMAGE_S;


IMP_VOID IMP_HistoryImageCreate( HISTORY_IMAGE_S *pstImgHis, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );

IMP_VOID IMP_HistoryImageDestroy( HISTORY_IMAGE_S *pstImgHis, IMP_VOID *pMmgr );

IMP_VOID IMP_HistoryImageInit( HISTORY_IMAGE_S *pstImgHis, GRAY_IMAGE_S *pstImgCur, IMP_U32 u32Time );

GRAY_IMAGE_S* IMP_GetHistoryImage( HISTORY_IMAGE_S *pstImgHis, IMP_U32 u32Time );

IMP_VOID IMP_SetHistoryImage( HISTORY_IMAGE_S *pstImgHis, GRAY_IMAGE_S *pstImgCur, IMP_U32 u32Time );




#ifdef __cplusplus
}
#endif

#endif
