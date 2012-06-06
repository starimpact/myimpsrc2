#include "imp_pea_static.h"
#include "imp_pea_bva.h"
#include "imp_para.h"

extern IpStaticTargetData* ipGetStaticTargetData( IMP_VOID *pData );
static IMP_S32 ipProcessStaticAnalystChange( IpStaticAnalyst *pstAnalyst );




IMP_VOID ipCreateStaticAnalyst( IpStaticAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	memset( pstAnalyst, 0, sizeof(IpStaticAnalyst) );


}
IMP_VOID ipReleaseStaticAnalyst( IpStaticAnalyst *pstAnalyst )
{

}
IMP_VOID ipConfigStaticAnalyst( IpStaticAnalyst *pstAnalyst, IpStaticPara *pstPara )
{
	pstAnalyst->pstPara = pstPara;


}
IMP_VOID ipStartStaticAnalyst( IpStaticAnalyst *pstAnalyst )
{

}
IMP_VOID ipStopStaticAnalyst( IpStaticAnalyst *pstAnalyst )
{

}

static IMP_VOID ipPreProcessStaticAnalyst( IpStaticAnalyst *pstAnalyst )
{
	IMP_S32 i, s32Cnt;
	PEA_RESULT_S *pstResult = pstAnalyst->pstResult;

}

IMP_S32 ipProcessStaticAnalyst( IpStaticAnalyst *pstAnalyst )
{
	IMP_S32 s32Ret = IMP_TRUE;
	PEA_STATUS_S *penStatus;

	return s32Ret;
}
IMP_S32 ipPostProcessStaticAnalyst( IpStaticAnalyst *pstAnalyst )
{
	return IMP_TRUE;
}

static IMP_S32 ipProcessStaticAnalystChange( IpStaticAnalyst *pstAnalyst )
{
	IMP_S32  i, s32ZoneIndex;

	return 1;
}


IMP_VOID ipFillTargetEventDataObjectRemoved( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{

}
IMP_VOID ipFillTargetEventDataObjectLeft( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{

}
IMP_VOID ipFillTargetEventDataNoParking( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{

}
