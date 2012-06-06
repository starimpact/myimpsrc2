/*****************************************************************************
 ** 
 * 文件说明：相关的数值计算数値演算
 * @brief
 * @date           2004/12/15
 * history:
 * @date           2005/12/12 脸部识别和检出模块的合并及标准化
 *　　　　　　　　　　　　　　Update Face recognition and detect module merged and standardize
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
 *****************************************************************************/
#ifndef __ETC_H__
#define __ETC_H__
//#include "FaceDetectMain.h"
//#include "DetectFaces.h"
#include "FaceDetectDef.h"

/*-----------------------------------------------------------------*
 * 宏定义
 *-----------------------------------------------------------------*/
#ifndef ABS
    #define ABS( a ) ( ( a ) >= 0 ? ( a ) : ( -( a ) ) )
#endif

/*20050309
#ifndef SQR
    #define SQR( a ) ( ( a ) == 0 ? 0 : ( ( a ) * ( a ) ) )
#endif*/
#ifndef SQR
    #define SQR( a ) ( ( a ) * ( a ) )
#endif

#ifndef MIN
    #define MIN( a, b ) ( ( a ) < ( b ) ? ( a ) : ( b ) )
#endif

#ifndef MAX
    #define MAX( a, b ) ( ( a ) > ( b ) ? ( a ) : ( b ) )
#endif

#ifndef CLIP
    #define CLIP( x ) ( MAX( 0, MIN( 255, ( x ) ) ) )
#endif

/*
#ifndef CLIP_512
    #define CLIP_512( x ) ( MIN( 511, MAX( 0, ( x ) ) ) )
#endif
*/

#ifdef __cplusplus
extern "C" {
#endif

/* sin函数
 * degree( multiples of 15 ) -> -FXP_UNIT_TRIANGULAR ～ +FXP_UNIT_TRIANGULAR */
IFACE_ERROR_CODE MySinDegree15( FaceRecognitionHandle* in_pInstance, const short in_Degree, short* out_pResult);

/* cos函数
 * degree( multiples of 15 ) -> -FXP_UNIT_TRIANGULAR ～ +FXP_UNIT_TRIANGULAR */
IFACE_ERROR_CODE MyCosDegree15( FaceRecognitionHandle* in_pInstance, const short in_Degree, short* out_pResult );

/* 对数函数 */
IFACE_ERROR_CODE MyLog2( FaceRecognitionHandle* in_pInstance, const unsigned long in_Val, long* out_pResult );

#ifdef __cplusplus
}
#endif


#endif /* __ETC_H__ */


/* End of file */
