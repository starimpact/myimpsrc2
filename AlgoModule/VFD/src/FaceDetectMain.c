/** 
 * @brief	         人脸检测SDK的标准模块	
 * @date	2008/07/22
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
 *
 *----------------------------------------------------------------------
 */

#include <stdlib.h>
#include <math.h>
#include <memory.h>

#include "FaceDetectMain.h"
#include "DetectFaceOnly.h"
#include "IO.h"
#include "FixedPoint.h"
#include "detectimage.h"

/**
 *
 * 初始化
 * Initialize
 *
 * @param	io_pInstance        
 * [in/out] 实例句柄
 *          Handle of instance
 * @param	in_pBinPath     	
 * [in]		学习数据的存放路径
 *			Path of study data
 * @param	out_pModelSize      
 * [out]    模型数据的大小
 *			Size of model data
 * @param	out_pHeight         
 * [out]    规一化图像的高
 *			Height of regularized face image
 * @param	out_pWidth          
 * [out]    规一化图像的宽
 *			Width of regularized face image
 *
 * @return	IFACE_ERR_SUCCESS_ID    :正常结束
 *									 Normal end
 *          IFACE_ERR_INVALID_PARAM :参数不正确
 *									 Invalid parameter
 *          IFACE_ERR_FILE_OPEN		:文件打开失败
 *									 File Open Failed
 *          IFACE_ERR_FILE_READ		:文件读入失败
 *									 File Read Failed
 *
 */
IFACE_ERROR_CODE Initialize( 
			FaceRecognitionHandle* io_pInstance
			)

{
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
	/* Parameter check */
	FACEHANDLE * ptr;


	if( io_pInstance == NULL) {
		return IFACE_ERR_INVALID_PARAM;
	}
	if( *io_pInstance == NULL){
		*io_pInstance = calloc(sizeof(FACEHANDLE), 1);
		ptr = (FACEHANDLE *)*io_pInstance;
	}else{
		return IFACE_ERR_INVALID_PARAM;
	}


	if(ptr->initFlag != 1){

		/* 设定参数的缺省值 */
		/* Setting the default value to the parameter.  */
		resultCode = SetFaceDetectParameters(io_pInstance, 5, 5,
			SEARCH_INPLANE_000,
			FRONT,LOCK_ROT90_ON,
			LOCK_LARGE_ON,
			MODE_NORMAL, 100, 50);
		if( resultCode != IFACE_ERR_SUCCESS_ID) {
			return resultCode;
		}
		ptr->initFlag = 1;
	}

	return resultCode;
}


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
IFACE_ERROR_CODE SetFaceDetectParameters(
		FaceRecognitionHandle*	in_pInstance,
		const short in_FaceSize,
 		const short in_FaceSize30,
 		const SearchInplaneMode in_InPlane,
 		const SearchOutplaneMode in_OutPlane,
 		const LockRotation90Mode in_LockRotate90,
 		const LockSearchLarge in_LockLarge,
 		const SearchMode in_DetectMode,
 		const short in_FaceDetectThreshold,
		const short in_FaceCropThreshold ) {

	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;		
	FACEHANDLE* ptr = NULL;
	DetectParam* pDetectParam = NULL;

	/* Parameter check */
	if( in_pInstance == NULL){
		resultCode = IFACE_ERR_INVALID_PARAM;
		return resultCode;
	}
	if( *in_pInstance == NULL){
		resultCode = IFACE_ERR_WRONG_SEQUENCE;
		return resultCode;
	}
		
	if( in_FaceSize<=1 || in_FaceSize>100
		|| in_FaceSize30<=1 || in_FaceSize30>100 ||
		( in_InPlane != SEARCH_INPLANE_000 && in_InPlane != SEARCH_INPLANE_ALL ) || 
		( in_OutPlane != FRONT && in_OutPlane != PROFILE && in_OutPlane != SEARCH_OUTPLANE_ALL ) ||
		( in_LockRotate90 != LOCK_ROT90_ON && in_LockRotate90 != LOCK_ROT90_OFF ) ||
		( in_LockLarge != LOCK_LARGE_ON && in_LockLarge != LOCK_LARGE_OFF ) ||
		( in_DetectMode != MODE_NORMAL && in_DetectMode != MODE_TP && in_DetectMode != MODE_SPEED ) ||
		//( in_FaceDetectThreshold < 0 || in_FaceDetectThreshold > 100 ) || //2008-07-14 修改
		( in_FaceDetectThreshold < 0 || in_FaceDetectThreshold > 1000 ) ||
		( in_FaceCropThreshold < 0 || in_FaceCropThreshold > 100) ) {

		resultCode = IFACE_ERR_INVALID_PARAM;
		return resultCode;
	}

	/* 从句柄中取得结构参数	*/
	/* Get the structure for parameter from handle */
	ptr = ((FACEHANDLE*)*in_pInstance);
	pDetectParam = &ptr->detectParam;
	if( pDetectParam == NULL) {
		resultCode = IFACE_ERR_WRONG_SEQUENCE;
		
		return resultCode;
	}

	/* Setting the least size of detected face for up-down-left-right four direction */
	pDetectParam->faceSize = in_FaceSize;
	/* Setting the least size of detected face for vertical 30 grade direction */
	pDetectParam->faceSize30 = in_FaceSize30;	
	/* Setting Search for in plane rotation */
	pDetectParam->inPlane = in_InPlane;
	/* Setting search for out plane rotation */	
	pDetectParam->outPlane = in_OutPlane;
	/* Setting direction fixed flag when a face is found  */
	pDetectParam->lockRotate90 = in_LockRotate90;
	/* Setting smaller face detect flag when a face is found */
	pDetectParam->lockLarge = in_LockLarge;
	/* Setting priority mode for face detect */
	pDetectParam->detectMode = in_DetectMode;	
	/* Setting threshold for face crop score */
	pDetectParam->faceDetectThreshold = in_FaceDetectThreshold;
	pDetectParam->faceCropThreshold = in_FaceCropThreshold;

	return IFACE_ERR_SUCCESS_ID;
}

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
IFACE_ERROR_CODE  DetectFaces(
				FaceRecognitionHandle* in_pInstance, 
				const unsigned char* in_pImage,
                const long in_Width,
                const long in_Height,
				short* io_pFaceNo,
				FACEINFO* io_pFace ) {

	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
	unsigned char* pYImage;

	FACEHANDLE * ptr;

	/* Parameter check */
	if( in_pInstance == NULL){
		resultCode = IFACE_ERR_INVALID_PARAM;
		return resultCode;
	}
	if( *in_pInstance == NULL){
		resultCode = IFACE_ERR_WRONG_SEQUENCE;
		return resultCode;
	}
	if( in_pImage == NULL || in_Width <= 0 ||
		in_Height <= 0 || io_pFaceNo == NULL || io_pFace == NULL ) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}

    /******************************
     * RGB->Y                     *
     ******************************/
	pYImage = ( unsigned char * ) malloc( in_Width * in_Height );
	if( pYImage==NULL )	{
		resultCode = IFACE_ERR_NO_ENOUGH_MEMORY;
		return resultCode;
	}

    resultCode = ConvertRGBtoY( in_pInstance, in_pImage, in_Width, in_Height, pYImage );
	if( resultCode != IFACE_ERR_SUCCESS_ID ) {
		if( pYImage!=NULL ) {
			if( pYImage != NULL ) {
				free( pYImage );
				pYImage = NULL;
			}
		}
		return resultCode;
	}

    /********************************
	 * Face detection
     ********************************/
    resultCode = DetectFaces2( in_pInstance, pYImage, in_Width, in_Height, io_pFaceNo, io_pFace );


	if( resultCode != IFACE_ERR_SUCCESS_ID )
	{
		
		if( pYImage!=NULL ) {
			if( pYImage != NULL ) {
				free( pYImage );
				pYImage = NULL;
			}
		}
		return resultCode;
	}

	if( pYImage != NULL ) {
		free( pYImage );
		pYImage = NULL;
	}

    return resultCode;
}


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
IFACE_ERROR_CODE Uninitialize(
				FaceRecognitionHandle*	in_pInstance)
{
	IFACE_ERROR_CODE	ret = IFACE_ERR_SUCCESS_ID;
	FACEHANDLE *ptr;

	if( in_pInstance == NULL){
		ret = IFACE_ERR_INVALID_PARAM;
		return ret;
	}
	if( *in_pInstance == NULL){
		ret = IFACE_ERR_WRONG_SEQUENCE;
		return ret;
	} else{
		ptr = (FACEHANDLE *)*in_pInstance;
		
		ptr->initFlag = 0;
		free(ptr);
		ptr = NULL;

		return IFACE_ERR_SUCCESS_ID;
	}
}

