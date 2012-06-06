#ifndef _IMP_PEA_TRACKER_H_
#define _IMP_PEA_TRACKER_H_

#include "imp_pea_tracker_common.h"
#include "imp_pea_tracker_ms.h"

#ifdef __cplusplus
extern "C"
{
#endif

IMP_VOID ipParseTrackerParaData( IpTrackerPara *pstPara );
IMP_VOID ipForceInnerParaToTrackerPara( INNER_PARA_S *pstInnerPara, IpTrackerPara *pstPara );

typedef struct
{
	IMP_S32 s32UsedTotal;
	PEA_DETECTED_REGION_S* pstRegion;
	IMP_U32 au32TargetIds[IMP_MAX_TARGET_CNT_IN_GROUP];
	IpTrackedTarget* pastTargets[IMP_MAX_TARGET_CNT_IN_GROUP];
	IMP_S32 s32ToBeDeleted[IMP_MAX_TARGET_CNT_IN_GROUP];

	IMP_S32 s32GroupArea;
	IMP_S32 s32MemberMaxArea;
	IMP_S32 s32MemberMinArea;
	IMP_S32 s32SumAreaTargets;
	IMP_S32 s32SumAreaRegions;

	IMP_S32 s32StablishCollision;
	IMP_S32 s32StableTime;

	IMP_S32 s32DrgIdforSingleTgtInMultiSplit;
	IMP_S32 as32DrgIdInMultiSplit[IMP_MAX_TARGET_CNT_IN_GROUP];

	IMP_RECT_S groupRect;
}IpGroup;


typedef struct
{
	IMP_S32 s32UsedTotal;
	IpGroup astGroups[IMP_MAX_GROUP_CNT];
}IpGroupSet;


typedef struct tagSAMPLE_TARDAT
{
	IMP_S32 s32ObjArea;
	IMP_S32 s32ActualArea;
	IMP_POINT_S stCentroid;
	IMP_RECT_S  stRect;
	IMP_S32 s32Direction;
} SAMPLE_TARDAT;

typedef struct tagCOLLISION_SPLIT_TARDAT
{
	IMP_S32 s32CurCollision;
	IMP_S32 s32LastCollision;
	IMP_S32 s32Split;
	IMP_S32 s32CollisionLength;
	IpGroup*  pstGroup;
	SAMPLE_TARDAT sampleTardat;
}COLLISION_SPLIT_TARDAT;



typedef struct
{

#ifdef TRACKER_USE_RGB_INFO
	IMP_U32 au32ObjectHistogram[IMP_RGB_LEVEL];

	IMP_FLOAT E[3];
	IMP_FLOAT sigma[3];
	IMP_FLOAT skewness[3];
#else
	IMP_U32 au32ObjectHistogram[IMP_GRAY_LEVEL];
#endif

	IMP_POINT_S stCurCentroid;
	IMP_POINT_S stPredictedCentroid;
	IMP_FLOAT fPredictedIncrementX;
	IMP_FLOAT fPredictedIncrementY;
	IMP_FLOAT fPredictedCentroidX;
	IMP_FLOAT fPredictedCentroidY;
	IMP_FLOAT fMatchCoefficient;
	IMP_S32 s32MatchedWith;
	IMP_RECT_S stMinRect;
	IMP_S32 s32ObjArea;
	IMP_S32 s32ActualArea;
	IMP_S32 s32FirstTime;
	IMP_S32 s32LastTime;
	IMP_S32 s32TrackedFrameNum;
	IpTargetPosition stFirstPos;

	IpTargetPosition stFirstPredictPosAfterTracked;
	IMP_S32 s32FirstPredictPosAfterTrackedFlag;
	IMP_S32 s32PredictTargetTrackedNum;

	IMP_FLOAT fVelocityX;
	IMP_FLOAT fVelocityY;
	IMP_S32 s32HaveFilledVelocity;

	IMP_S32 w_sum, h_sum, w_max, w_min, h_max, h_min;
	IMP_S32 trd_neg_x, trd_pot_x, trd_zero_x, trd_neg_y, trd_pot_y, trd_zero_y;
	IMP_S32 trd_zero_xy, trd_zero_motion, trd_total_xy, trd_non0_motion_x, trd_non0_motion_y;
	IMP_S32 pot_dis_x, neg_dis_x, pot_dis_y, neg_dis_y;


	COLLISION_SPLIT_TARDAT cs;


} IpTargetDataTrackerM;


typedef struct
{
	IpGroupSet stGroupSet;
} IpTargetSetDataTrackerM;


typedef struct
{
#ifdef TRACKER_USE_RGB_INFO
	IMP_U32 au32ObjectHistogram[IMP_RGB_LEVEL];

	IMP_FLOAT E[3];
	IMP_FLOAT sigma[3];
	IMP_FLOAT skewness[3];
#else
	IMP_U32 au32ObjectHistogram[IMP_GRAY_LEVEL];
#endif

	IMP_POINT_S stCurCentroid;
	IMP_RECT_S stMinRect;
	IMP_S32 s32MaxMatchDist;
 	IMP_S32 s32CurrentTime;
	IMP_S32 s32MatchedWith;
	IMP_FLOAT fMatchCoefficient;
	IMP_U16 u16Status;

	IMP_S32 s32Collision;
	IMP_S32 s32Split;
	IMP_S32 s32Merged;
	IpGroup* pstGroup;

} IpDrgDataTrackerM;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;            /**< 资源 */
	PEA_RESULT_S *pstResult;                    /**< 处理结果 */
	IpTrackerPara *pstPara;                     /**< 参数 */

	IMP_FLOAT afMatchCoeff[IMP_MAX_TGT_CNT][IMP_MAX_TGT_CNT];
	IMP_U32 u32TargetCurId;
	IpTargetDataTrackerM *pstTargetDatas;
	IpTargetSetDataTrackerM *pstTargetSetData;
	IpDrgDataTrackerM *pstDRegionDatas;

#ifdef	TRACKER_USE_RGB_INFO
#ifdef _TIDSP
	GRAY_IMAGE16_S stImageRGB565;
#endif
	RGB_IMAGE_S stImageRGB;
#endif

	GRAY_IMAGE_S pstImgInGrayLast;
	GRAY_IMAGE_S pstImgFgRgnLast;


} IpTargetTrackerMotion;

IMP_VOID ipCreateMotionTrackerInternal( IpTargetTrackerMotion *pstTracker, GA_HARDWARE_RS_S *pstHwResource, PEA_RESULT_S *pstResult );
IMP_VOID ipReleaseMotionTrackerInternal( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipConfigMotionTrackerInternal( IpTargetTrackerMotion *pstTracker, IpTrackerPara *pstPara );
IMP_S32 ipProcessMotionTrackerInternal( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipStartMotionTrackerInternal( IpTargetTrackerMotion *pstTracker );

typedef struct
{
	IMP_FLOAT fMatchCoefficient;
	IMP_S32 s32MatchedWidth;
	IMP_POINT_S stCurCentroid;
	IMP_POINT_S stPredictedCentroid;
	IMP_RECT_S stMinRect;
	IMP_S32 s32ObjArea;
	IMP_S32 s32ActualArea;
	IMP_S32 s32FirstTime;
	IMP_S32 s32LastTime;
	IMP_S32 s32TrackedFrameNum;
	IpTargetPosition stFirstPos;
} IpTargetDataTrackerS;


typedef struct
{
	IMP_FLOAT fMatchCoefficient;
	IMP_S32 s32MatchedWidth;
	IMP_POINT_S stCurCentroid;
	IMP_RECT_S stMinRect;
	IMP_S32 s32MaxMatchDist;
	IMP_S32 s32CurrentTime;
} IpDrgDataTrackerS;

typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	IpTrackerPara *pstPara;

	IpTargetDataTrackerS *pstTargetDatas;
	IpDrgDataTrackerS *pstDRegionDatas;

	IMP_FLOAT afMatchCoeff[IMP_MAX_TGT_CNT][IMP_MAX_TGT_CNT];
	IMP_U32 u32TargetCurId;

} IpTargetTrackerStatic;

IMP_VOID ipCreateStaticTrackerInternal( IpTargetTrackerStatic *pstTracker, GA_HARDWARE_RS_S *pstHwResource, PEA_RESULT_S *pstResult, PROCESS_TYPE_E enFlag);
IMP_VOID ipReleaseStaticTrackerInternal( IpTargetTrackerStatic *pstTracker );
IMP_VOID ipConfigStaticTrackerInternal( IpTargetTrackerStatic *pstTracker, IpTrackerPara *pstPara );
IMP_S32 ipProcessStaticTrackerInternal( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag);
IMP_S32 ipPostProcessStaticTrackerInternal( IpTargetTrackerStatic *pstTracker);
IMP_VOID ipStartStaticTrakerInternal( IpTargetTrackerStatic *pstTracker );


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;           /**< 资源 */
	PEA_RESULT_S *pstResult;                   /**< 处理结果 */
	IpTrackerPara stPara;                      /**< 参数*/


	IpTargetTrackerMotion stMotionModule;
#ifdef SUPPORT_CHANGE
	IpTargetTrackerStatic stStaticModuleChange;
#endif

	IpTargetTrackerStatic stStaticModuleOsc; //Static Tracker for OSC

} IpTargetTracker;


IMP_VOID ipCreateTargetTrackerInternal( IpTargetTracker *pstTracker, GA_HARDWARE_RS_S *pstHwResource, PEA_RESULT_S *pstResult );
IMP_VOID ipReleaseTargetTrackerInternal( IpTargetTracker *pstTracker );
IMP_VOID ipConfigTargetTrackerInternal( IpTargetTracker *pstTracker, IpTrackerPara *pstPara );
IMP_S32 ipProcessTargetTrackerInternal( IpTargetTracker *pstTracker );
IMP_S32 ipPostProcessTargetTrackerInternal( IpTargetTracker *pstTracker );

IMP_S32 ipOpenLogFile();
IMP_S32 ipOutputTrackDataFrame(IpTargetTracker *pstTracker);
IMP_S32 ipCloseLogFile();

typedef struct
{
	IpTargetTracker *pstTracker;
} IpModuleTracker;



IMP_MODULE_HANDLE ipCreateTracker( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResouce );
IMP_VOID ipReleaseTracker( IMP_MODULE_HANDLE hModule );
IMP_VOID ipConfigTracker( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstInnerPara );
IMP_S32 IMP_PEA_ProcessTracker( IMP_MODULE_HANDLE hModule );
IMP_S32 ipPostProcessTracker( IMP_MODULE_HANDLE hModule );



#ifdef __cplusplus
}
#endif

#endif
