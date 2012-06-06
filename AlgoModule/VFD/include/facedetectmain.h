/** 
 * @brief：人脸检测SDK的标准模块
 * @date	2008/07/22
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
 *
 *----------------------------------------------------------------------
 */
#ifndef __FACEDETECT_H__
#define __FACEDETECT_H__

#include <stdio.h>
#include "FaceDetectDef.h"

#define LIB_API

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------
  public function
----------------------------------------------------------------------*/
/**
 *
 * 初始化
 * Initialize
 *
 * @param	io_pInstance        
 * [in/out] 实例句柄
 *          Handle of instance
 *
 * @return	IFACE_ERR_SUCCESS_ID    :正常结束
 *									 Normal end
 *          IFACE_ERR_INVALID_PARAM :参数不正确
 *									 Invalid parameter
 *
 */
LIB_API IFACE_ERROR_CODE Initialize( 
			FaceRecognitionHandle* io_pInstance
			);

/**
 *
 * 脸检出参数设置
 * Setting face detect parameters
 *
 * @param	in_pInstance			
 * [in/out] 实例句柄
 *          handle of instance
 * @param	in_FaceSize     		
 * [in]	 上下左右方向的最小检出脸的大小
 *		 the least size of detected face for up-down-left-right four direction
 *		 缺省值：　			0.1
 *		 default：　		0.1
 * @param	in_FaceSize30     		
 * [in]	 纵轴30度方向的最小检出脸大小
 *		 the least size of detected face for vertical 30 grade direction
 *       缺省值：　			0.2
 *		 default：　		0.2
 * @param	in_InPlane     			
 * [in]	 对象在平面内旋转
 *		 search for in plane rotation
 *		 SEARCH_INPLANE_000：	仅仅正立方向
 *								stand direction only
 *		 SEARCH_INPLANE_ALL：	12个方向，每个方向间隔30度
 *								inclreasing 30 grade for 12 direction
 *		 缺省值：				SEARCH_INPLANE_000　
 *		 default：　			SEARCH_INPLANE_000
 * @param	in_OutPlane     		
 * [in]	 对象在平面的垂直方向旋转
 *		 search for out plane rotation
 *		 FRONT：　			　	仅仅正面脸部
 *								front face only
 *		 PROFILE：			　	侧脸
 *								profile
 *		 SEARCH_OUTPLANE_ALL：	所有方向
 *								all direction
 *		 缺省值：　			FRONT
 *		 default：　			FRONT
 * @param	in_LockRotate90    		
 * [in]	 找到脸时上下方向的固定标记
 *		 direction fixed flag when a face is found 
 *		 LOCK_ROT90_ON：　		固定
 *								lock
 *		 LOCK_ROT90_OFF：		不固定
 *								don't lock
 *		 缺省值：			LOCK_ROT90_ON
 *		 default：　			LOCK_ROT90_ON
 * @param	in_LockLarge     		
 * [in]	 找到脸时小尺寸脸是否搜索的标志
 *		 smaller face detect flag when a face is found 
 *		 LOCK_LARGE_ON：　		不搜索
 *								don't detect
 *		 LOCK_LARGE_OFF：		搜索
 *								detect
 *		 缺省值：			LOCK_LARGE_ON
 *		 default：　			LOCK_LARGE_ON
 * @param	in_DetectMode  			
 * [in]	 脸检出优先方式
 *		 priority mode for face detect
 *		 MODE_NORMAL：			通常
 *								normal
 *		 MODE_TP：　　			TP优先
 *								TP has a high priority
 *		 MODE_SPEED：			速度优先
 *								detect speed has a high priority
 *		 缺省值：			MODE_NORMAL
 *		 default：　			MODE_NORMAL
 * @param	in_FaceDetectThreshold  
 * [in]	 脸检出适用的分值门限
 *		 threshold for face detect score
 *		 范围：					0-1000
 *		 range:					0-1000
 *		 缺省值：			100
 *		 default：　			100
 * @param	in_FaceCropThreshold    
 * [in]	 切出脸的分值门限
 *		 threshold for face crop score
 *		 范围：					0-100
 *		 range:					0-100
 *		 缺省值：			100
 *		 default：　			100
 *
 * @return	IFACE_ERR_SUCCESS_ID    :正常结束
 *									 Normal End
 *          IFACE_ERR_WRONG_SEQUENCE:顺序错误
 *									 Wrong Sequence
 *          IFACE_ERR_INVALID_PARAM :非法参数
 *									 Invalid Parameter
 */
LIB_API IFACE_ERROR_CODE SetFaceDetectParameters(
		FaceRecognitionHandle*	in_pInstance,
		const short in_FaceSize,
 		const short in_FaceSize30,
 		const SearchInplaneMode in_InPlane,
 		const SearchOutplaneMode in_OutPlane,
 		const LockRotation90Mode in_LockRotate90,
 		const LockSearchLarge in_LockLarge,
 		const SearchMode in_DetectMode,
 		const short in_FaceDetectThreshold,
		const short in_FaceCropThreshold);

/* 
 * 脸检出
 * face detect
 *
 * @param	io_pInstance			
 * [in/out] 实例句柄
 *          handle of instance
 * @param in_pImage
 * [in]图像数据
 *	   image data
 * @param in_Width
 * [in]图像宽
 *	   image width
 * @param in_Height
 * [in]图像高
 *	   image height
 * @param io_FaceNo
 * [io]检出人脸的最大候选数（输入）
 *	   the most detected face candidate number (input)
 *     检出的人脸数（输出）
 *	   detected face candidate number (output)
 * @param io_pFace
 * [io]检出人脸的坐标、得分等的结构队列
 *	   detected face coordinates, score struct list
 * @return	IFACE_ERR_SUCCESS_ID    :		正常结束
 *											Normal End
 *          IFACE_ERR_WRONG_SEQUENCE:		顺序错误
 *											Wrong Sequence
 *          IFACE_ERR_INVALID_PARAM :		非法参数
 *											Invalid Parameter
 *			IFACE_ERR_NO_ENOUGH_MEMORY	:	内存分配失败
 *											no enough memory
 *			IFACE_ERR_NO_FACE_FOUND		:	没有检到脸
 *											no face detected
 */
LIB_API IFACE_ERROR_CODE  DetectFaces(
				FaceRecognitionHandle* in_pInstance, 
				const unsigned char *in_pImage,
                const long in_Width,
                const long in_Height,
				short *io_FaceNo,
				FACEINFO *io_pFace );


/**
 *
 * 结束处理
 * Uninitialize
 *
 * @param	in_pInstance			[in]	Handle of instance
 *
 * @return	IFACE_ERR_SUCCESS_ID    :正常结束
 *									 Normal End
 *          IFACE_ERR_WRONG_SEQUENCE:顺序错误
 *									 Wrong Sequence
 *          IFACE_ERR_INVALID_PARAM :非法参数
 *									 Invalid Parameter
 */
LIB_API IFACE_ERROR_CODE Uninitialize(
				FaceRecognitionHandle*	in_pInstance
				);

#ifdef __cplusplus
}
#endif

#endif	/* __FACEDETECT_H__ */