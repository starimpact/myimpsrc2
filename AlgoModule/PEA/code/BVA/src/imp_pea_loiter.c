#include "imp_pea_loiter.h"
#include "imp_pea_target.h"


IMP_VOID imp_CreateLoiter(IMP_Loiter_S *pstLoiter, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	pstLoiter->pstResult = pstResult;
	pstLoiter->pstHwResource = pstHwResource;
	
	memset(pstLoiter->astLTI, 0, IMP_MAX_TGT_CNT * sizeof(IMP_LoiterTgtInfo_S));
	
	return;
}


IMP_VOID imp_ReleaseLoiter(IMP_Loiter_S *pstLoiter)
{
	
	
	return;
}


IMP_VOID imp_ConfigLoiter(IMP_Loiter_S *pstLoiter, IMP_LoiterPara_S *pstLP)
{
	pstLoiter->pstParams = pstLP;
	
	return;
}


IMP_S32 imp_ProcessLoiter(IMP_Loiter_S *pstLoiter)
{
	IMP_S32  i, s32ZoneIndex;
	IpTrackedTarget *pstTarget = 0;
	IpTrackedTargetSet *pstTargetSet = &pstLoiter->pstResult->stTrackedTargetSet;
	RULE_ZONE_S *pstZone = pstLoiter->pstParams->pstRule->stZones.astZone;
	IMP_LoiterTgtInfo_S *pstParams = pstLoiter->pstParams;
	IpTargetPosition *pstPos = 0;
	
   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTargetSet->astTargets[i];
		if(!ipTrackedTargetIsActive(pstTarget))
			continue;
		
		if(ipTrackedTargetIsMeasured(pstTarget))
		{
			pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -1);
			pstTarget->stTargetInfo.u32TgtEvent |= IMP_TGT_EVENT_LOITER;
		}
	}
	
	
	return 0;
}



