#ifndef _IMP_PEA_OSC_H_
#define _IMP_PEA_OSC_H_

#include "imp_ga_common.h"
#include "imp_pea_common.h"
#include "imp_pea_regionset_label.h"
#include "imp_pea_bva_common.h"

#ifdef __cplusplus
extern "C"
{
#endif



typedef struct
{
	IMP_S32 s32InitTime;
	IMP_S32 s32InitBgEdgeRatioTh;
	IMP_S32 s32AddWeight;
	IMP_S32 s32SubWeight;
	IMP_S32 s32LeftAccumLevel;
	IMP_S32 s32RemovedAccumLevel;
	PEA_RGE_LABLE_PARA_S stLabelPara;
	IMP_S32 s32MinArea;
	IMP_S32 s32MorphLevel;
	IMP_S32 s32MinConnectivity;
	IMP_S32 s32BgUpdateTime;

	IMP_S32 s32LeaveLimit;
	IMP_S32 s32UseBorderConstrain;
	IMP_S32 s32BorderWidth;
	IMP_S32 s32DoSpecialRegionProcess;
	IMP_S32 s32SpclMinTime;
	IMP_S32 s32TrgnTrajectLen;
	IMP_S32 s32TrgnTrajectRatio;
	IMP_S32	s32FrmDura;
	RULE_S *pstRule;
} IpOscPara;


typedef struct
{
	IMP_S32 as32Flag[IMP_MAX_NUM_RULE_ZONE];
	IpTargetPosition *pastFirstPos[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32 s32IsFirstPos;
	IMP_S32 s32FirstPosTime;
} IpOscTargetData;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;		/**< 资源 */
	PEA_RESULT_S *pstResult;				/**< 处理结果 */

	GRAY_IMAGE_S stImgBgEdge;
	GRAY_IMAGE_S stImgBgInDiffMag;

	GRAY_IMAGE_S stImgBgInDiff;
	GRAY_IMAGE_S stImgLeftLife;
	GRAY_IMAGE_S stImgRemovedLife;
	GRAY_IMAGE_S stImgLeft;
	GRAY_IMAGE_S stImgRemoved;
	GRAY_IMAGE_S stImgLeftMorph;
	GRAY_IMAGE_S stImgRemovedMorph;
	GRAY_IMAGE_S stImgStatistic;
	GRAY_IMAGE_S stImgMediate;
	PEA_DETECTED_REGIONSET_S stDRegionSet;

	IMP_S32 s32InitStartFrm;
	IMP_S32 s32InitTotalCnt;
	IMP_S32 s32BgEdgeInited;
	IMP_S32 s32BgUpdateStartTime;
	IpOscPara *pstPara;
} IpOscAnalyst;


IMP_VOID ipCreateOscAnalysti( IpOscAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseOscAnalysti( IpOscAnalyst *pstAnalyst );
IMP_VOID ipConfigOscAnalysti( IpOscAnalyst *pstAnalyst, IpOscPara *pstPara );
IMP_S32 ipProcessOscAnalysti( IpOscAnalyst *pstAnalyst );
IMP_S32 ipPostProcessOscAnalysti( IpOscAnalyst *pstAnalyst );


/*@}*/


#ifdef __cplusplus
}
#endif

#endif /*_IMP_PEA_OSC_H_*/
