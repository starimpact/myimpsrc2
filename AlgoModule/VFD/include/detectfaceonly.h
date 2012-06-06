/*****************************************************************************
 ** 
 * 文件说明：脸检出
 * @brief        : Face detection
 * @date           2004/12/15
 * history:
 * @date           2005/12/12 脸部识别和检出模块的合并及标准化
 *　　　　　　　　　　　　　　Update Face recognition and detect module merged and standardize
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
 *****************************************************************************/

#ifndef __DETECTFACEONLY_H__
#define __DETECTFACEONLY_H__

/*-----------------------------------------------------------------*
 * File reference definition
 *-----------------------------------------------------------------*/
//#include "DetectFaceCommon.h"
//#include "Candidates.h"
//#include "FaceDetectMain.h"
//#include "DetectFaces.h"
#include "FaceDetectDef.h"

/*-----------------------------------------------------------------*
 * Function definition
 *-----------------------------------------------------------------*/
#ifdef __cplusplus
extern "C" {
#endif


/* 脸检出 */
/* Face detection */
IFACE_ERROR_CODE DetectFacesOnlyAllDirection( FaceRecognitionHandle* in_pInstance,
								  const unsigned char *in_pImage,
								  const short/*long*/ in_Width,
                                  const short/*long*/ in_Height,
                                  const short in_FaceSize30,
								  CandidatesList* io_pFaceList );




#ifdef __cplusplus
}
#endif

#endif /* __DETECTFACEONLY_H__ */

/* End of file */
