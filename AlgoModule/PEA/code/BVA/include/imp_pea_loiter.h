#ifndef _IMP_PEA_LOITER_H_
#define _IMP_PEA_LOITER_H_

#include "imp_pea_common.h"
#include "imp_pea_bva_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct impLoiterPara_S
{
	RULE_S *pstRule;
} IMP_LoiterPara_S;


typedef struct impLoiterTgtInfo_S
{
	IMP_U32	u32TargetId;
	IMP_U32 u32Used;
	IMP_S32 as32DrtHist[8];
} IMP_LoiterTgtInfo_S;


typedef struct impLoiter_S
{
	PEA_RESULT_S *pstResult;
	GA_HARDWARE_RS_S *pstHwResource;
	
	IMP_LoiterPara_S *pstParams;
	
	IMP_LoiterTgtInfo_S astLTI[IMP_MAX_TGT_CNT];
} IMP_Loiter_S;


IMP_VOID imp_CreateLoiter(IMP_Loiter_S *pstLoiter, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);
IMP_VOID imp_ReleaseLoiter(IMP_Loiter_S *pstLoiter);
IMP_VOID imp_ConfigLoiter(IMP_Loiter_S *pstLoiter, IMP_LoiterPara_S *pstLP);
IMP_S32 imp_ProcessLoiter(IMP_Loiter_S *pstLoiter);


#ifdef __cplusplus
}
#endif

#endif


