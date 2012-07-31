#ifndef _IMP_PEA_BVA_PARA_H_
#define _IMP_PEA_BVA_PARA_H_

#include "imp_ga_common.h"
#include "imp_pea_common.h"
#include "imp_pea_perimeter.h"
#include "imp_pea_tripwire.h"
#include "imp_pea_multripwire.h"
#include "imp_pea_tgt_evt_mgr.h"
#include "imp_pea_bva_common.h"
#include "imp_pea_static.h"


#ifdef __cplusplus
extern "C"
{
#endif


typedef struct
{

#if defined(SUPPORT_OBJECTREMOVED) | defined(SUPPORT_OBJECTLEFT) | defined(SUPPORT_NOPARKING)
	IpStaticPara stStaticPara;
#endif

	IpPerimeterPara stPerimeterPara;
	IpTripwirePara stTripwirePara;
	IpMTripwirePara stMTripwirePara;
	IMP_S32 s32InitEventLife;
	IMP_S32 s32OutputEvents;

	IMP_S32	s32FrmDura;
	IMP_U32 u32FuncCode;
	IMP_S32 s32ObjectAreaRatio;
	RULE_S *pstRule;
} IpAnalystPara;


IMP_VOID ipParseAnaystParaData( IpAnalystPara *pstPara );


#ifdef __cplusplus
}
#endif

#endif
