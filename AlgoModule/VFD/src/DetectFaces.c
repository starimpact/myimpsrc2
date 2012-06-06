/*****************************************************************************
 ** 
 * 文件说明：公用的人脸检出模块
 * @brief		　: Face detection wrapper module for openning
 * @date           2004/12/15
 * history:
 * @date           2005/12/05 人脸检测与识别模块的合并与标准化
 * Copyright (C) 2004 WISEBIRD CO., LTD. All rights reserved.
 *****************************************************************************/



#include <stdio.h>
#include <malloc.h>
#include "FaceDetectDef.h"
#include "detectimage.h"

#include "DetectFaces.h"
#include "Defines.h"
#include "FixedPoint.h"
#include "Etc.h"
#include "DetectFaceOnly.h"
#include "BaseError.h"
#include "IO.h"


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
IFACE_ERROR_CODE DetectFaces2(
				FaceRecognitionHandle* in_pInstance, 
				const unsigned char* in_pImage,
                const short/*long*/ in_Width,
                const short/*long*/ in_Height,
				short/*long*/* io_pFaceNo,
				FACEINFO* io_pFace ){
	IFACE_ERROR_CODE resultCode = IFACE_ERR_SUCCESS_ID;
	FACEHANDLE* ptr = NULL;
	DetectParam* pDetectParam = NULL;
 	
	//<<<--- 2009-12-30 将long类型修改为short
	/* 缩放处理 */
	/* Resize processing movement */
    unsigned char* pResized;			/* 缩放后图像（RGB) */
	                                    /* Image（RGB) of having been resized */
    short/*long*/ resizedWidth, resizedHeight;   /* 缩放后图像大小 */
	                                    /* Size of having been resized */
    short scale;						/* 输入图像的量级（原图像相对于FXP_UNIT_SCALE的像素量级) */
	                                    /* Scale of input image(number of relative pixels when number of pixels of 
	                                       original picture images is assumed to be FXP_UNIT_SCALE) */
    short size;
    short size30;						/* 被搜索图像的人脸短边最大值，从纵横轴30度倾斜开始搜索人脸 */
	                                    /* The maximum values of pixels in short side of the searched image in which
										   the face incline for vertical and horizontal 30 grade direction */
										
										 



    unsigned char* pYTemp;


	CandidatesList faceList;
	FaceCandidatesNew face[MAX_NUM_CANDIDATES];
	short/*long*/ n;
    short/*long*/ score;     
	short/*long*/ cnt;       


	/* Parameter check */
	if( in_pInstance == NULL || in_pImage == NULL || in_Width <= 0 ||
		in_Height <= 0 || io_pFaceNo == NULL || io_pFace == NULL ) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}

	ptr = ((FACEHANDLE*)*in_pInstance);
	pDetectParam = &ptr->detectParam;
	if( pDetectParam == NULL) {
		resultCode = IFACE_ERR_WRONG_SEQUENCE;
		
		return resultCode;
	}
	
    size = FACE_DETECT_UNIT * pDetectParam->faceSize;
    if ( ( pDetectParam->faceSize30 <= 0 ) || ( pDetectParam->faceSize30 > pDetectParam->faceSize ) ) {
        size30 = size;
    }
    else {
        size30 = FACE_DETECT_UNIT * pDetectParam->faceSize30;
    }

    /******************************
	 * 扩缩       
     ******************************/
    if ( in_Width > in_Height ) {
        resizedHeight = size;
        scale = ( short ) ( ( 2 * FXP_UNIT_SCALE * resizedHeight / in_Height + 1 ) / 2 );
        resizedWidth = ( long ) ( ( 2 * in_Width * resizedHeight / in_Height + 1 ) / 2 );
    }
    else {
        resizedWidth = size;
        scale = ( short ) ( ( 2 * FXP_UNIT_SCALE * resizedWidth / in_Width + 1 ) / 2 );
        resizedHeight = ( long ) ( ( 2 * in_Height * resizedWidth / in_Width + 1 ) / 2 );
    }

    pResized = ( unsigned char * ) malloc( resizedWidth * resizedHeight );
	if( pResized == NULL ) {
		resultCode = IFACE_ERR_NO_ENOUGH_MEMORY;
		
		return resultCode;
	}
    pYTemp = ( unsigned char * ) malloc( ( in_Width / 2 ) * ( in_Height / 2 ) );
	if( pYTemp == NULL ) {
		resultCode = IFACE_ERR_NO_ENOUGH_MEMORY;
		
		if( pResized != NULL ) {
			free( pResized );
			pResized = NULL;
		}

		return resultCode;
	}


    resultCode = AffineSingleNoRot( in_pInstance, in_pImage, in_Width, in_Height, scale, resizedWidth, resizedHeight, pResized, pYTemp );

	if( resultCode != IFACE_ERR_SUCCESS_ID ) {
		
		if( pYTemp != NULL ) {
			free( pYTemp );
			pYTemp = NULL;
		}
		if( pResized != NULL ) {
			free( pResized );
			pResized = NULL;
		}
		return resultCode;
	}
    if( pYTemp != NULL ) {
		free( pYTemp );
		pYTemp = NULL;
	}


    /********************************
     * 人脸检出        
	 * Face detection
     ********************************/
	faceList.maxNumber  = MAX_NUM_CANDIDATES;
	faceList.number     = 0;
	faceList.unitLength = sizeof( FaceCandidatesNew );
	faceList.pTop        = face;
    resultCode = DetectFacesOnlyAllDirection(in_pInstance, pResized, resizedWidth, resizedHeight, size30, &faceList );
	if( resultCode != IFACE_ERR_SUCCESS_ID &&
		resultCode != IFACE_ERR_TOO_MANY_CANDIDATES){
		
		if( pResized != NULL ) {
			free( pResized );
			pResized = NULL;
		}
		return resultCode;
	}
	/* 缩放处理 */
	/* Resize processing movement */
	if( pResized != NULL ) {
		free( pResized );
		pResized = NULL;
	}

    /********************************
     * 结论                         *
	 * Result
     ********************************/
    cnt = 0;
	for ( n = 0 ; n < faceList.number && cnt < *io_pFaceNo ; n++ ) {
        
        /* 根据score, score2算出综合得分 */
		/* Calculate the mixture score according to score and score2. */
        if ( face[n].angleOutPlane == 0 ) {
            score = ( face[n].score * 5 + face[n].score2 * 2 ) / 30;/*正面脸 */
			                                                        /* Front face */
        }
		/* 45度人脸判别器导入 */
		/* 45 grade face distinction machine introduction */
		/* 45度与侧脸相同处理 */
		/* 45 grade is the same as the side face */
        else {
            score = ( face[n].score * 5 + face[n].score2 * 2 ) / 27;/* 侧脸      */
			                                                        /* side face */
        }
        if ( score < pDetectParam->faceDetectThreshold ) {
            continue;
        }

		io_pFace[cnt].rightEyeX = face[n].aCanonicalEyeWidth[EYE_RIGHT] - ( resizedWidth - 1 ) * FXP_UNIT_POS / 2;
        io_pFace[cnt].rightEyeX = ( io_pFace[cnt].rightEyeX + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / scale - FXP_UNIT_POS_HALF;
        io_pFace[cnt].rightEyeX = ( io_pFace[cnt].rightEyeX + ( resizedWidth - 1 ) * FXP_UNIT_POS * FXP_UNIT_SCALE / scale / 2 ) / FXP_UNIT_POS;

        io_pFace[cnt].rightEyeY = face[n].aCanonicalEyeHeight[EYE_RIGHT] - ( resizedHeight - 1 ) * FXP_UNIT_POS / 2;
        io_pFace[cnt].rightEyeY = ( io_pFace[cnt].rightEyeY + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / scale - FXP_UNIT_POS_HALF;
        io_pFace[cnt].rightEyeY = ( io_pFace[cnt].rightEyeY + ( resizedHeight - 1 ) * FXP_UNIT_POS * FXP_UNIT_SCALE / scale / 2 ) / FXP_UNIT_POS;

        io_pFace[cnt].leftEyeX  = face[n].aCanonicalEyeWidth[EYE_LEFT] - ( resizedWidth - 1 ) * FXP_UNIT_POS / 2;
        io_pFace[cnt].leftEyeX  = ( io_pFace[cnt].leftEyeX + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / scale - FXP_UNIT_POS_HALF;
        io_pFace[cnt].leftEyeX  = ( io_pFace[cnt].leftEyeX + ( resizedWidth - 1 ) * FXP_UNIT_POS * FXP_UNIT_SCALE / scale / 2 ) / FXP_UNIT_POS;

        io_pFace[cnt].leftEyeY  = face[n].aCanonicalEyeHeight[EYE_LEFT] - ( resizedHeight - 1 ) * FXP_UNIT_POS / 2;
        io_pFace[cnt].leftEyeY  = ( io_pFace[cnt].leftEyeY + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / scale - FXP_UNIT_POS_HALF;
        io_pFace[cnt].leftEyeY  = ( io_pFace[cnt].leftEyeY + ( resizedHeight - 1 ) * FXP_UNIT_POS * FXP_UNIT_SCALE / scale / 2 ) / FXP_UNIT_POS;

        io_pFace[cnt].centerX   = face[n].centerWidth - ( resizedWidth - 1 ) * FXP_UNIT_POS / 2;
        io_pFace[cnt].centerX   = ( io_pFace[cnt].centerX + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / scale - FXP_UNIT_POS_HALF;
        io_pFace[cnt].centerX   = ( io_pFace[cnt].centerX + ( resizedWidth - 1 ) * FXP_UNIT_POS * FXP_UNIT_SCALE / scale / 2 ) / FXP_UNIT_POS;

        io_pFace[cnt].centerY   = face[n].centerHeight - ( resizedHeight - 1 ) * FXP_UNIT_POS / 2;
        io_pFace[cnt].centerY   = ( io_pFace[cnt].centerY + FXP_UNIT_POS_HALF ) * FXP_UNIT_SCALE / scale- FXP_UNIT_POS_HALF;
        io_pFace[cnt].centerY   = ( io_pFace[cnt].centerY + ( resizedHeight - 1 ) * FXP_UNIT_POS * FXP_UNIT_SCALE / scale / 2 ) / FXP_UNIT_POS;

        io_pFace[cnt].faceSize = ( FACE_DETECT_UNIT / 2 ) * SQR( FXP_UNIT_SCALE ) / face[n].scale / scale;

		io_pFace[cnt].angleInPlane  = face[n].angle;
		io_pFace[cnt].angleOutPlane = face[n].angleOutPlane;

        io_pFace[cnt].faceScore = score;

		io_pFace[cnt].serialNumber = cnt + 1;		

        cnt++;
	}
    
	if( n < faceList.number ) {
		resultCode = IFACE_ERR_INVALID_PARAM;
		
		return resultCode;
	}

	*io_pFaceNo = cnt;

	return resultCode;
}



/* End of file */