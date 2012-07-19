#include "imp_pea_loiter.h"
#include "imp_pea_target.h"


IMP_LoiterTgtInfo_S *imp_GetLoiterTarget(IMP_LoiterTgtInfo_S *pstLTIs, IMP_S32 s32LTNum, IMP_U32 u32Id);
IMP_S32 imp_DeleteLoiterTargets(IMP_LoiterTgtInfo_S *pstLTIs, IMP_S32 s32LTNum, IpTrackedTargetSet *pstTargetSet);
IMP_S32 imp_JudgeIfLoiter(IMP_Loiter_S *pstLoiter, IpTrackedTarget *pstTarget, IMP_S32 s32TimeMin);


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
	IMP_S32 s32I, s32ZI, s32Loiter;
	IpTrackedTarget *pstTarget = 0;
	IpTrackedTargetSet *pstTargetSet = &pstLoiter->pstResult->stTrackedTargetSet;
	RULE_ZONE_S *pstZones = pstLoiter->pstParams->pstRule->stZones.astZone;
	IMP_LoiterTgtInfo_S *pstParams = pstLoiter->pstParams;
	
//	printf("IMP_MAX_TGT_CNT:%d\n", IMP_MAX_TGT_CNT);
	
   	for(s32I = 0; s32I < IMP_MAX_TGT_CNT; s32I++)
	{
		pstTarget = &pstTargetSet->astTargets[s32I];
		if(!ipTrackedTargetIsActive(pstTarget))
			continue;
		
		for(s32ZI = 0; s32ZI < IMP_MAX_NUM_RULE_ZONE; s32ZI++)
		{
			RULE_ZONE_S *pstZone = &pstZones[s32ZI];
			if(pstZone->u32Valid && pstZone->u32Enable && (pstZone->u32Mode & IMP_FUNC_LOITER))
			{
				s32Loiter = imp_JudgeIfLoiter(pstLoiter, pstTarget, pstZone->stPara.stLoiter.s32TimeMin);
				if (s32Loiter)
				{
				//	printf("\033[33m=======================================\033[0m\n");
				//	pstTarget->stTargetInfo.u32TgtEvent |= IMP_TGT_EVENT_LOITER;
					pstTarget->stTargetInfo.au32Events[s32ZI] |= IMP_TGT_EVENT_LOITER;
				}
			}
		}
	}
	
	
	imp_DeleteLoiterTargets(pstLoiter->astLTI, IMP_MAX_TGT_CNT, pstTargetSet);
	
	return 0;
}


IMP_S32 imp_JudgeIfLoiter(IMP_Loiter_S *pstLoiter, IpTrackedTarget *pstTarget, IMP_S32 s32TimeMin)
{
	IMP_S32 s32PI;
	IMP_LoiterTgtInfo_S *pstLTIs = pstLoiter->astLTI, *pstLTI = 0;
	IpTargetPosition *pstPos1 = 0, *pstPos2 = 0;
	IMP_S32 *ps32DrtHist = 0;
	IMP_S32 s32TraLen;
	
//	s32TraLen = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	
//	if (s32TraLen < 40) return 0;
	
	pstLTI = imp_GetLoiterTarget(pstLTIs, IMP_MAX_TGT_CNT, pstTarget->u32TargetId);
	
	if (pstLTI == 0) return 0;
	
	pstPos1 = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -1);
//	pstPos2 = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -30);
	ps32DrtHist = pstLTI->as32DrtHist;
	IMP_S32 s32Drt = pstPos1->s32Direction / 45;
	if (s32Drt >= 8) s32Drt = 7;
	
	pstLTI->s32Time++;
	
	if (pstLTI->s32Time == 1)
	{
		pstLTI->stPosMean.s32X = pstPos1->stPt.s16X;
		pstLTI->stPosMean.s32Y = pstPos1->stPt.s16Y;
		
		pstLTI->stPosVar.s32X = 1;
		pstLTI->stPosVar.s32Y = 1;
	}
	
	pstLTI->stPosMean.s32X = pstLTI->stPosMean.s32X * (1 - 0.05) + pstPos1->stPt.s16X * 0.05 + 0.5f;
	pstLTI->stPosMean.s32Y = pstLTI->stPosMean.s32Y * (1 - 0.05) + pstPos1->stPt.s16Y * 0.05 + 0.5f;
	
	pstLTI->stPosVar.s32X = pstLTI->stPosVar.s32X * (1 - 0.2) + abs(pstPos1->stPt.s16X - pstLTI->stPosMean.s32X) * 0.2 + 0.5f;
	pstLTI->stPosVar.s32Y = pstLTI->stPosVar.s32Y * (1 - 0.2) + abs(pstPos1->stPt.s16Y - pstLTI->stPosMean.s32Y) * 0.2 + 0.5f;
	
	ps32DrtHist[s32Drt]++;

#if 0
//	printf("id:%d_s32Direction:%d, Drt8:%d\n", pstTarget->u32TargetId, pstPos1->s32Direction, pstPos1->s32Direction / 45);
	printf("id_%d: ", pstTarget->u32TargetId);
	for (s32PI = 0; s32PI < 8; s32PI++)
	{
		printf("%d,", ps32DrtHist[s32PI]);
	}
	printf("   ");
	printf("(%d, %d), (%d, %d)", pstLTI->stPosMean.s32X, pstLTI->stPosMean.s32Y, pstLTI->stPosVar.s32X, pstLTI->stPosVar.s32Y);
	printf("\n");
#endif
	
	IMP_S32 s32DrtNum = 0, s32DrtMax = 0, s32DrtMaxID = 0;
	for (s32PI = 0; s32PI < 8; s32PI++)
	{
		s32DrtNum += ps32DrtHist[s32PI];
		if (s32DrtMax < ps32DrtHist[s32PI])
		{
			s32DrtMax = ps32DrtHist[s32PI];
			s32DrtMaxID = s32PI;
		}
	//	printf("%d,", ps32DrtHist[s32PI]);
	}
	
	if (s32DrtMaxID > 0) s32DrtMax += ps32DrtHist[s32DrtMaxID - 1];
	else s32DrtMax += ps32DrtHist[7];
	
	if (s32DrtMaxID < 7) s32DrtMax += ps32DrtHist[s32DrtMaxID + 1];
	else s32DrtMax += ps32DrtHist[0];
	
//	printf("DrtNum:%d, %d%%\n", s32DrtNum, s32DrtMax * 100 / s32DrtNum);
	
	if (s32DrtMax * 100 < s32DrtNum * 70 && pstLTI->s32Time >= s32TimeMin)
	{
		return 1;
	}

	return 0;
}


IMP_LoiterTgtInfo_S *imp_GetLoiterTarget(IMP_LoiterTgtInfo_S *pstLTIs, IMP_S32 s32LTNum, IMP_U32 u32Id)
{
	IMP_S32 s32PI;
	IMP_LoiterTgtInfo_S *pstLTI;
	IMP_S32 s32Found = 0;
	
	for (s32PI = 0; s32PI < s32LTNum; s32PI++)
	{
		pstLTI = &pstLTIs[s32PI];
		if (pstLTI->s32Used)
		{
			if (pstLTI->u32TargetId == u32Id)
			{
				s32Found = 1;
				break;
			//	return pstLTI;
			}
		}
	}
	
	if (!s32Found)
	{
		for (s32PI = 0; s32PI < s32LTNum; s32PI++)
		{
			pstLTI = &pstLTIs[s32PI];
			if (!pstLTI->s32Used)
			{
				memset(pstLTI, 0, sizeof(IMP_LoiterTgtInfo_S));
				pstLTI->s32Used = 1;
				pstLTI->u32TargetId = u32Id;
				s32Found = 1;
				break;
			//	return pstLTI;
			}
		}
	}



	if (s32Found)
	{
		return pstLTI;
	}
	
	return 0;
}


IMP_S32 imp_DeleteLoiterTargets(IMP_LoiterTgtInfo_S *pstLTIs, IMP_S32 s32LTNum, IpTrackedTargetSet *pstTargetSet)
{
	IMP_S32 s32PI, s32TI;
	IMP_LoiterTgtInfo_S *pstLTI;
	IpTrackedTarget *pstTarget = 0;
	

	
	for (s32PI = 0; s32PI < s32LTNum; s32PI++)
	{
		pstLTI = &pstLTIs[s32PI];
		if (pstLTI->s32Used)
		{
			for(s32TI = 0; s32TI < IMP_MAX_TGT_CNT; s32TI++)
			{
				pstTarget = &pstTargetSet->astTargets[s32TI];
				if (ipTrackedTargetIsActive(pstTarget))// && ipTrackedTargetIsMeasured(pstTarget))
			//	if (pstTarget->s32Used)
				{
					if (pstLTI->u32TargetId == pstTarget->u32TargetId)
					{
						break;
					}
				}
			}
			
			if (s32TI == IMP_MAX_TGT_CNT)
			{
				memset(pstLTI, 0, sizeof(IMP_LoiterTgtInfo_S));
			}
		}
	}
	
#if 0
	IMP_S32 s32Num = 0;
	for (s32PI = 0; s32PI < s32LTNum; s32PI++)
	{
		pstLTI = &pstLTIs[s32PI];
		if (pstLTI->s32Used)
		{
			s32Num++;
		}
	}
	printf("---------------------LTIs32Num:%d\n", s32Num);
#endif
	
	return 0;
}


IMP_VOID imp_FillTargetEventDataLoiter(EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule)
{
	IMP_S32 s32ZoneIndex = pstEvent->u32Zone;
	RULE_ZONE_S *pstZone = &pstRule->stZones.astZone[s32ZoneIndex];
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
	EVT_DATA_TARGET_S *pstData = (EVT_DATA_TARGET_S*)pstEvent->au8Data;

	memcpy(&pstData->stRect,&pstPos->stRg,sizeof(IMP_RECT_S));
}




