#ifndef _IMP_PEA_MULTRIPWIRE_H_
#define _IMP_PEA_MULTRIPWIRE_H_

//#include "imp_ga_common.h"
#include "imp_pea_common.h"
#include "imp_pea_tripwire.h"

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
} IpSTripwirePara;

typedef struct
{
	IpSTripwirePara astSTripwire[2];
	RULE_S *pstRule;
} IpMTripwirePara;


typedef enum
{
	MTRIPWIRE_CROSS_NONE = 0,
	MTRIPWIRE_CROSS_ONCE = 1,
	MTRIPWIRE_CROSS_TWICE = 2
} IpMCrossStatus;


typedef struct
{
	IMP_S32  s32TypeTime[IMP_MAX_NUM_RULE_ZONE][IMP_MAX_MTRIPWIRE_CNT];
	IpMCrossStatus  astCrossStatus[IMP_MAX_NUM_RULE_ZONE][IMP_MAX_MTRIPWIRE_CNT];
} IpSTripwireTargetData;


typedef struct
{
	IMP_S32  s32TypeRecorded;
	IMP_S32  s32TypeLikehood;
	IMP_S32  as32LineIndex[IMP_MAX_NUM_RULE_ZONE];
	IpSTripwireTargetData astTargetData[2];
} IpMTripwireTargetData;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	IpMTripwirePara *pstPara;
} IpMTripwireAnalyst;

IMP_S32 IMP_GetMemSizeMTripwire(IMP_S32 s32Width, IMP_S32 s32Height);
IMP_VOID ipCreateMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst );
IMP_VOID ipConfigMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst, IpMTripwirePara *pstPara );
IMP_S32 ipProcessMTripwireAnalystInternal( IpMTripwireAnalyst *pstAnalyst );
IMP_S32 ipPostProcessMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst );


#ifdef __cplusplus
}
#endif

#endif
