
#ifndef _IMP_PEA_STATIC_H_
#define _IMP_PEA_STATIC_H_

#include "imp_ga_common.h"
#include "imp_pea_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define ITERATOR_TARGET_START \
	for( i=0,s32Cnt=0; i<IMP_MAX_TGT_CNT; i++ ) \
	{ \
	pstTarget = &pstTargetSet->astTargets[i]; \
	if( pstTarget->s32Used && pstTarget->stTargetImage.pstDRegions ) \
	{


#define ITERATOR_TARGET_END \
} \
	s32Cnt += (pstTarget->s32Used ? 1 : 0); \
	if( s32Cnt>=pstTargetSet->s32UsedTotal ) \
	break; \
	}


typedef struct
{
	IMP_S32 s32FrmDura;

	IMP_S32 s32Width;
	IMP_S32 s32Height;

	IMP_S32 s32StaticGradRadius;
	IMP_S32 s32StaticStrengthMin;
	IMP_S32 s32StaticStrengthDiff;
	IMP_S32 s32StaticStrengthRatio;

	RULE_S *pstRule;
} IpStaticPara;


typedef struct
{
	IMP_S32 s32EdgeStrengthCur;
	IMP_S32 s32EdgeStrengthBg;
} IpStaticTargetData;


typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	IpStaticPara *pstPara;
} IpStaticAnalyst;

IMP_VOID ipCreateStaticAnalyst( IpStaticAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseStaticAnalyst( IpStaticAnalyst *pstAnalyst );
IMP_VOID ipConfigStaticAnalyst( IpStaticAnalyst *pstAnalyst, IpStaticPara *pstPara );
IMP_VOID ipStartStaticAnalyst( IpStaticAnalyst *pstAnalyst );
IMP_VOID ipStopStaticAnalyst( IpStaticAnalyst *pstAnalyst );
IMP_S32 ipProcessStaticAnalyst( IpStaticAnalyst *pstAnalyst );
IMP_S32 ipPostProcessStaticAnalyst( IpStaticAnalyst *pstAnalyst );





#ifdef __cplusplus
}
#endif

#endif
