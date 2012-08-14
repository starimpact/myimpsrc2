#ifndef _IMP_PEA_TRIPWIRE_H_
#define _IMP_PEA_TRIPWIRE_H_

//#include "imp_ga_common.h"
#include "imp_pea_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{

	IMP_S32 s32ToleranceAngle;
	IMP_S32 s32ObjectAreaRatio;
	IMP_S32 s32TripwireUseMtrend;
	IMP_S32 s32UseObjectTrend;
	IMP_S32 s32UseBottom;
	IMP_S32 s32AnalysisLength;
	IMP_S32 s32SegLength;
	IMP_S32 s32AwayDistance;
	IMP_S32 s32ValidRatio;

	RULE_S *pstRule;
} IpTripwirePara;


typedef enum
{
	TRIPWIRE_CROSS_NONE,
	TRIPWIRE_CROSS_ONCE,
	TRIPWIRE_CROSS_TWICE
} IpCrossStatus;



typedef struct
{
	IpTargetPosition   *apstFirstPos[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32  s32MaxDist[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32  s32TypeRecorded;
	IMP_S32  s32TypeLikehood;
	IpCrossStatus  astCrossStatus[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32  as32LineIndex[IMP_MAX_NUM_RULE_ZONE];
} IpTripwireTargetData;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	IpTripwirePara *pstParams;
} IpTripwireAnalyst;

IMP_S32 IMP_GetMemSizeTripwire(IMP_S32 s32Width, IMP_S32 s32Height);
IMP_VOID ipCreateTripwireAnalysti( IpTripwireAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseTripwireAnalysti( IpTripwireAnalyst *pstAnalyst );
IMP_VOID ipConfigTripwireAnalysti( IpTripwireAnalyst *pstAnalyst, IpTripwirePara *pstParams );
IMP_S32 ipProcessTripwireAnalystInternal( IpTripwireAnalyst *pstAnalyst );
IMP_S32 ipPostProcessTripwireAnalysti( IpTripwireAnalyst *pstAnalyst );



#ifdef __cplusplus
}
#endif

#endif
