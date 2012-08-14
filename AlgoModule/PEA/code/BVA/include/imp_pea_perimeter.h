#ifndef _IMP_PEA_PERIMETER_H_
#define _IMP_PEA_PERIMETER_H_

#include "imp_pea_common.h"
#include "imp_pea_bva_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{
	IMP_S32 s32PerimeterUseMtrend;
	IMP_S32 s32ConverseUseMtrend;
	IMP_S32 s32AbNormalVUseMtrend;

	IMP_S32 s32PerimeterDelicacy;
	IMP_S32 s32PerimeterRatio;
	IMP_S32 s32PerimeterUseBottom;

	IMP_S32 s32PerimeterIntrusionRatio;
	IMP_S32 s32PerimeterEnterRatio;
	IMP_S32 s32PerimeterExitRatio;
	IMP_S32 s32PerimeterAppearRatio;
	IMP_S32 s32PerimeterDisppearRatio;
	IMP_S32 s32PerimeterDisppearAlarmTime;

	IMP_S32 s32PerimeterAnalysisLength;
	IMP_S32 s32PerimeterValidRatio;

	IMP_S32 s32ConverseAnalysisLength;
	IMP_S32 s32ConverseThDistance;

	IMP_S32 s32AngleTolerance;
	IMP_S32 s32ObjectAreaRatio;
	RULE_S *pstRule;
} IpPerimeterPara;



typedef enum
{
	IMP_PRIMETER_DIRECTION_OUT_SCOPE,
	IMP_PRIMETER_DIRECTION_IN_SCOPE
} IpPerimeterDirectionStatus;


typedef struct
{
	IpTargetPosition *apstFirstPos[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32 s32MaxDistance[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32 s32AppearFlag[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32 s32EnterFlag[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32 s32ExitFlag[IMP_MAX_NUM_RULE_ZONE];

	IMP_S32 s32CollisionNum;
	IpPerimeterDirectionStatus  stPerimeterDirectionStatus;

	IpCycleByteArray stPerimeterCycleArray;
} IpPerimeterTargetData;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;					/**< 资源 */
	PEA_RESULT_S *pstResult;						    /**< 处理结果 */
	IpPerimeterPara *pstParams;					        /**< 参数 */

	IpRecordInfo astRecordInfo[IMP_MAX_TGT_CNT];
	IMP_S32 s32Num;
	IMP_S32 s32SafeDisFlag;
	IpStaticList stStaticList;
} IpPerimeterAnalyst;

IMP_S32 IMP_GetMemSizePerimeter(IMP_S32 s32Width, IMP_S32 s32Height);
IMP_VOID ipCreatePerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleasePerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst );
IMP_VOID ipConfigPerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst, IpPerimeterPara *pstParams );
IMP_S32 ipProcessPerimeterAnalystInternal( IpPerimeterAnalyst *pstAnalyst );
IMP_S32 ipPostProcessPerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst );


#ifdef __cplusplus
}
#endif

#endif
