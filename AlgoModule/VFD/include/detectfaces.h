/*****************************************************************************
 ** 
 * 文件说明：公用的人脸检出模块
 * @brief		　: Face detection wrapper module for openning
 * @date           2004/12/15
 * history:
 * @date           2005/12/05 人脸检测与识别模块的合并与标准化
 * Copyright (C) 2004 WISEBIRD CO., LTD. All rights reserved.
 *****************************************************************************/

#ifndef __DETECTFACES_H__
#define __DETECTFACES_H__

/*---------------------------------------------------------
 * Private parameter
 *---------------------------------------------------------*/

//<<<--- 2008-01-05
//#define	HALFPROFILE					4
//--->>>

/*---------------------------------------------------------
 * Type definition
 *---------------------------------------------------------*/

#include "FaceDetectDef.h"

/*---------------------------------------------------------
 * Function definition
 *---------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------*/
/** DetectFaces2 : 人脸检出
 *                 Face detection wrapper
 * @param in_pInstance			
 * [in] 实例句柄
 *          handle of instance
 * @param in_pImage
 * [in]图像数据
 *	   image data
 * @param in_Width
 * [in]图像宽度
 *	   image width
 * @param in_Height
 * [in]图像高度
 *	   image height
 * @param io_pFaceNo
 * [io]最多可检出的人脸候选数（输入）
 *	   the most detected face candidate number (input)
 *     实际检出的人脸数（输出）
 *	   detected face candidate number (output)
 * @param io_pFace
 * [io]检出人脸的坐标、得分列表
 *	   detected face coordinates, score struct list
 * @return	IFACE_ERR_SUCCESS_ID    :		正常结束
 *											Normal End
 *          IFACE_ERR_WRONG_SEQUENCE:		顺序错误
 *											Wrong Sequence
 *          IFACE_ERR_INVALID_PARAM :		非法参数
 *											Invalid Parameter
 *			IFACE_ERR_NO_ENOUGH_MEMORY	:	内存分配失败
 *											no enough memory
 *			IFACE_ERR_NO_FACE_FOUND		:	没有检出人脸
 *											no face detected
/*----------------------------------------------------------------------------*/
//<<<--- 2009-12-30 将long类型修改为short
IFACE_ERROR_CODE DetectFaces2(
				FaceRecognitionHandle* in_pInstance, 
				const unsigned char* in_pImage,
                const short/*long*/ in_Width,
                const short/*long*/ in_Height,
				short/*long*/* io_pFaceNo,
				FACEINFO* io_pFace );




#ifdef __cplusplus
}
#endif


#endif /* __DETECTFACES_H__ */



/* End of file */
