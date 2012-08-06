
#ifndef _IMP_PEA_BVA_H_
#define _IMP_PEA_BVA_H_

#include "imp_ga_common.h"
#include "imp_pea_common.h"
#include "imp_pea_regionset_label.h"

#include "imp_pea_perimeter.h"
#include "imp_pea_tripwire.h"
#include "imp_pea_multripwire.h"
#include "imp_pea_tgt_evt_mgr.h"
#include "imp_pea_bva_para.h"
#include "imp_pea_bva_common.h"
#include "imp_pea_static.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct impPEA_BVA_TGT_EVT_DATA_S
{
	IMP_U8 aau8EvtLife[IMP_MAX_NUM_RULE_ZONE][16];
	IMP_U32 u32TgtTypePre;
} PEA_BVA_TGT_EVT_DATA_S;


typedef struct impTARDAT_ANALYST_S
{
	PEA_BVA_TGT_EVT_DATA_S stEventData;


#if defined(SUPPORT_PERIMETER)
	IpPerimeterTargetData stDataPerimeter;
#endif

#if defined(SUPPORT_TRIPWIRE)
	IpTripwireTargetData stDataTripwire;
#endif

#if defined(SUPPORT_MTRIPWIRE)
	IpMTripwireTargetData stDataMTripwire;
#endif

} TARDAT_ANALYST_S;


typedef struct impTARGET_DATA_ANALYST_OSC_S
{
	PEA_BVA_TGT_EVT_DATA_S stEventData;
} TARGET_DATA_ANALYST_OSC_S;


typedef struct impTARGET_DATA_ANALYST_STATIC_S
{
	PEA_BVA_TGT_EVT_DATA_S stEventData;
} TARGET_DATA_ANALYST_STATIC_S;


typedef struct impBEHAVIOR_ANALYSIS_S
{
	GA_HARDWARE_RS_S *pHwResouce;
	PEA_RESULT_S *pstResult;
	TARDAT_ANALYST_S *pstTargetData;

	TARGET_DATA_ANALYST_OSC_S *pstAnalystOSC;

	IpAnalystPara stPara;

#if defined(SUPPORT_PERIMETER) | defined(SUPPORT_ABNORMALVEL) | defined(SUPPORT_CONVERSE)
	IpPerimeterAnalyst stModulePerimeter;
#endif

#if defined(SUPPORT_TRIPWIRE)
	IpTripwireAnalyst stModuleTripwire;
#endif

#if defined(SUPPORT_MTRIPWIRE)
	IpMTripwireAnalyst stModuleMTripwire;
#endif

	IpTargetEvtMgr stTargetEvtMgr;

} BEHAVIOR_ANALYSIS_S;

IMP_VOID ipCreateAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResouce );
IMP_VOID ipReleaseAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva );
IMP_VOID ipConfigAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva, IpAnalystPara *pstPara );
IMP_VOID ipStartAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva );
IMP_VOID ipStopAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva );
IMP_S32 IMP_PEA_BVA_ProcessBvaInternal( BEHAVIOR_ANALYSIS_S *pstBva );
IMP_S32 ipPostProcessAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva );

typedef struct impModuleAnalyst
{
	BEHAVIOR_ANALYSIS_S *pstAnalyst;
} IpModuleAnalyst;

IMP_MODULE_HANDLE ipCreateAnalyst( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce );
IMP_VOID ipReleaseAnalyst( IMP_MODULE_HANDLE hModule );
IMP_VOID ipConfigAnalyst( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pVapara );
IMP_VOID ipStartAnalyst( IMP_MODULE_HANDLE hModule );
IMP_VOID ipStopAnalyst( IMP_MODULE_HANDLE hModule );
IMP_S32 ipProcessAnalyst( IMP_MODULE_HANDLE hModule );
IMP_S32 ipPostProcessAnalyst( IMP_MODULE_HANDLE hModule );


typedef struct impModuleBehaviorAnalysis
{
	IMP_MODULE_HANDLE hAnalyst;
	PEA_RESULT_S *pstResult;
	GA_HARDWARE_RS_S *pstHwResouce;
} IpModuleBehaviorAnalysis;


IMP_S32 IMP_GetMemSizeBehaviorAnalysis(IMP_S32 s32Width, IMP_S32 s32Height);

IMP_MODULE_HANDLE IMP_PEA_BVA_CreateBehaviorAnalysis( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce );
IMP_VOID IMP_PEA_BVA_ReleaseBehaviorAnalysis( IMP_MODULE_HANDLE hModule );
IMP_VOID IMP_PEA_BVA_ConfigBehaviorAnalysis( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pVapara );
IMP_VOID IMP_PEA_BVA_StartBehaviorAnalysis( IMP_MODULE_HANDLE hModule );
IMP_VOID IMP_PEA_BVA_StopBehaviorAnalysis( IMP_MODULE_HANDLE hModule );
IMP_S32 IMP_PEA_BVA_ProcessBehaviorAnalysis( IMP_MODULE_HANDLE hModule );
IMP_S32 IMP_PEA_BVA_PostProcessBehaviorAnalysis( IMP_MODULE_HANDLE hModule );



#ifdef __cplusplus
}
#endif

#endif
