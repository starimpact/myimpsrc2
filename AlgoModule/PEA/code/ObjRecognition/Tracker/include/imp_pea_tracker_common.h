#ifndef _IMP_PEA_TRACKER_COMMON_H_
#define _IMP_PEA_TRACKER_COMMON_H_

#include "imp_pea_common.h"
#include "imp_pea_tracker_para.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define IMP_TGT_ID_NICE_MIN_MOTION		0x90000000

#define IMP_TGT_ID_NICE_MAX_MOTION		0x9FFFFFFE

#define IMP_TGT_ID_NICE_PTZ			    0x9FFFFFFF

#define IMP_TGT_ID_NICE_MIN_STATIC		0xA0000000

#define IMP_TGT_ID_NICE_MAX_STATIC		0xAFFFFFFF


typedef enum impPROCESS_TYPE_E
{
	IMP_PROCESS_CHG = 1,
	IMP_PROCESS_MOTION = 2,
	IMP_PROCESS_OSC = 3  //OSC type
}PROCESS_TYPE_E;



#ifdef  DEBUG_TRACKER
#define DEBUG_OBJRECG_TRACKER_DISP_NUM
#define DEBUG_IVP_OBJRECG_TRACKER_INFO1
#define DEBUG_IVP_OBJRECG_TRACKER_INFO2
#define DEBUG_OBJRECG_TRACKER_INFO2
#define DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD


  #define DEBUG_TRACKER_MMS



#ifdef DEBUG_TRACKER_TIMER
#include <WTYPES.H>
extern LARGE_INTEGER   TimerFrequency;
extern LARGE_INTEGER   TimerStart;
extern LARGE_INTEGER   TimerEnd;
extern double TimerDelta;
#endif


#include "windows.h"
extern IMP_S8 imp_info1[10240];
extern IMP_S8 *info1;
extern IMP_S8 imp_info2[10240];
extern IMP_S8 *info2;
extern IMP_S8 imp_info3[10240];
extern IMP_S8 *info3;
extern IMP_S8 imp_info4[10240];
extern IMP_S8 *info4;
#endif

#ifndef IMP_GRAY_LEVEL
#define IMP_GRAY_LEVEL				32
#define IMP_GRAY_LEVEL_B			3
#endif

/** (2^4) ^ 3 */
#define IMP_RGB_LEVEL				4096
/** 8-used bits */
#define IMP_RGB_LEVEL_B				4

#define NO_CANDIDATE 64



#ifdef IMP_ARM_PEA

#define IMP_MAX_TARGET_CNT_IN_GROUP 4

#define IMP_MAX_GROUP_CNT 4
#else

#define IMP_MAX_TARGET_CNT_IN_GROUP 8

#define IMP_MAX_GROUP_CNT 8
#endif

#define IMP_MATCH_GROUP -1


typedef enum impPOSITION_CHECK_E
{
	DO_CHECK = 0,
	DO_CORRECT = 1
}POSITION_CHECK_E;


#define IMP_IS_TGT_BLOB_MEASURED(x)  (!IMP_IS_TGT_DELETE(x) && IMP_IS_TGT_MEASURED(x) && !IMP_IS_TGT_MEANSHIFT(x) /*&& !IMP_IS_TGT_MRF(x)*/)
#define IMP_IS_TPOS_BLOB_MEASURED(x) (!IMP_IS_TGT_DELETE(x) && IMP_IS_TPOS_MEASURED(x) && !IMP_IS_TPOS_MEANSHIFT(x)/* && !IMP_IS_TPOS_MRF(x)*/)


IMP_S32 ipGetMinimumRegionDistance( const IMP_RECT_S *pstRect1, const IMP_RECT_S *pstRect2, IMP_S32 *s32IntersectArea);
IMP_VOID ipRectPointImgPositionCorrect(IMP_RECT_S *rect, IMP_POINT_S *point, IMP_S32 imgW, IMP_S32 imgH, IMP_S32 mode);
#ifdef __cplusplus
}
#endif

#endif
