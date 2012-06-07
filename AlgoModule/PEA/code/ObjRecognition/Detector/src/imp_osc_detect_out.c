#include "time.h"
#include "stdlib.h"

#if 0
#include "cv.h"
#include "highgui.h"
#endif


#include "imp_osc_detect.h"
#include "imp_osc_detect_inner.h"
#include "imp_pea_regionset_label.h"

IMP_S32 impIfAccordRegion(IMP_STATIC_OBJ_S *pstSOBJ, OSC_OBJ_LMT_SPECL_REGIONS_S *pstOscRgn);
IMP_S32 impIfAccordTimeAndSize(IMP_STATIC_OBJ_S *pstSOBJ, OSC_LMT_PARA_S stOscPara);
IMP_S32 impAddObj(IMP_STATIC_OBJ_S *pstSOBJ, TGT_SET_S *pstTargetSet);
IMP_S32 impAddEvent(IMP_STATIC_OBJ_S *pstSOBJ, IMP_S32 s32ZoneIndex, IMP_S32 s32OIndex, EVT_TYPE_E event, EVT_SET_S *pstEventSet);

IMP_S32 impOutPutResult(IMP_OSCD_S *pstModule)
{
	IMP_S32 s32I, s32ZI, s32OI;
	IMP_STATIC_OBJ_S *pstSOBJS = 0, *pstSObj = 0;
	PEA_RESULT_S *pstResult = pstModule->pstResult; //系统公共数据
	RULE_S *pstRule = pstModule->pstRule;
	IMP_OUTPUT *pstOutput = &pstResult->stOutput;
	
	pstSOBJS = pstModule->stSObjSet.aSObjs;
	
	memset(pstOutput, 0, sizeof(IMP_OUTPUT));
	
	//Stop Alart
	for (s32I = 0; s32I < OSCD_MAX_TGT_CNT; s32I++)
	{
	//	pstSObj = impGetSObjBySign(pstSOBJS, s32I);
		pstSObj = &pstSOBJS[s32I];
		if (OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used))
		{
			pstSObj->s32InRgn = 0;
			impAddObj(pstSObj, &pstOutput->stTargetSet);
			
			RULE_ZONE_S *pstZones = pstRule->stZones.astZone;
			for (s32ZI = 0; s32ZI < IMP_MAX_NUM_RULE_ZONE; s32ZI++)
			{
				IMP_S32 s32AccordRegion = 0, s32AccordTimeAndSize = 0;
				RULE_ZONE_S *pstZone = &pstZones[s32ZI];
				
				if (!pstZone->u32Valid || !pstZone->u32Enable || !(pstZone->u32Mode & IMP_URP_FUNC_OSC)) continue;
				
				RULE_PARA_OSC_S *pstOsc = &pstZone->stPara.stOsc;
				
				s32AccordTimeAndSize = impIfAccordTimeAndSize(pstSObj, pstOsc->stOscPara);
				
				for (s32OI = 0; s32OI < IMP_MAX_OSC_NUM; s32OI++)
				{
					s32AccordRegion = 0;
					if (pstOsc->astSpclRgs[s32OI].s32Valid)
					{
						s32AccordRegion = impIfAccordRegion(pstSObj, &pstOsc->astSpclRgs[s32OI]);
					}
					if (s32AccordRegion)
					{
						pstSObj->s32InRgn = 1;
						if (s32AccordTimeAndSize)
						{
							impAddEvent(pstSObj, s32ZI, s32OI, IMP_EVT_TYPE_AlarmOsc, &pstOutput->stEventSet);
						}
					}
				}
				/////////////////
			}
			
			if (!pstSObj->s32InRgn)
			{
				impDeleteObject(pstModule, pstSObj);
			}
			////////////
		}
		
		///////////////
	}
	
	
	//RMV Alart
	for (s32I = 0; s32I < OSCD_MAX_TGT_CNT; s32I++)
	{
		
	//	pstSObj = impGetRObjBySign(pstSOBJS, s32I);
		pstSObj = &pstSOBJS[s32I];
		if (OSCD_ROBJ_IS_REALOBJ(pstSObj->u8Used))
		{
		//	printf("ROBJ\n");
			pstSObj->s32InRgn = 0;
			impAddObj(pstSObj, &pstOutput->stTargetSet);
			
			RULE_ZONE_S *pstZones = pstRule->stZones.astZone;
			for (s32ZI = 0; s32ZI < IMP_MAX_NUM_RULE_ZONE; s32ZI++)
			{
				IMP_S32 s32AccordRegion = 0, s32AccordTimeAndSize = 0;
				RULE_ZONE_S *pstZone = &pstZones[s32ZI];
				
				if (!pstZone->u32Valid || !pstZone->u32Enable || !(pstZone->u32Mode & IMP_FUNC_OSC_R)) continue;
				
				RULE_PARA_OSC_S *pstOsc = &pstZone->stPara.stOsc;
				
				s32AccordTimeAndSize = impIfAccordTimeAndSize(pstSObj, pstOsc->stOscPara);
				
				for (s32OI = 0; s32OI < IMP_MAX_OSC_NUM; s32OI++)
				{
					s32AccordRegion = 0;
					if (pstOsc->astSpclRgs[s32OI].s32Valid)
					{
						s32AccordRegion = impIfAccordRegion(pstSObj, &pstOsc->astSpclRgs[s32OI]);
					}
					if (s32AccordRegion)
					{
						pstSObj->s32InRgn = 1;
						if (s32AccordTimeAndSize)
						{
							impAddEvent(pstSObj, s32ZI, s32OI, IMP_EVT_TYPE_AlarmOsc_R, &pstOutput->stEventSet);
						}
					}
				}
				/////////////////
			}
			
			if (!pstSObj->s32InRgn)
			{
				impDeleteObject(pstModule, pstSObj);
			}
			////////////
		}
		///////////////
	}
	
	return 0;
}


IMP_S32 impAddEvent(IMP_STATIC_OBJ_S *pstSOBJ, IMP_S32 s32ZoneIndex, IMP_S32 s32OIndex, EVT_TYPE_E event, EVT_SET_S *pstEventSet)
{	
	EVT_ITEM_S *pstEvt = 0;
	EVT_ITEM_S *pstEvents = pstEventSet->astEvents;
	IMP_S32 s32EventNum = pstEventSet->s32EventNum;
	EVT_DATA_TARGET_S *pstTgt = 0;
	
	if (s32EventNum < IMP_MAX_EVT_CNT)
	{
		pstEventSet->s32EventNum++;
	}
	else
	{
		return -1;
	}
	
	pstEvt = &pstEvents[s32EventNum];
	
	pstEvt->u32Type = event;
	pstEvt->u32Id = 0;
	pstEvt->u32Level = 0;
	pstEvt->u32Status = IMP_EVT_STATUS_START;
	pstEvt->u32Zone = s32ZoneIndex;
	
	pstTgt = (EVT_DATA_TARGET_S*)pstEvt->au8Data;
	pstTgt->u32TId = pstSOBJ->u32TargetId;
	pstTgt->stRect = pstSOBJ->stRect;
	
	if (pstSOBJ->u32EvtLife == 0)
		pstSOBJ->u32EvtLife = 1;
	
	printf("Event Number:%d\n", pstEventSet->s32EventNum);
	
	return 0;
}


IMP_S32 impAddObj(IMP_STATIC_OBJ_S *pstSOBJ, TGT_SET_S *pstTargetSet)
{
	IMP_S32 s32TI = 0;
	TGT_ITEM_S *pstTgt = 0;
	TGT_ITEM_S *pstTargets = pstTargetSet->astTargets;
	IMP_S32 s32TargetNum = pstTargetSet->s32TargetNum;
	
	if (s32TargetNum < IMP_MAX_TGT_CNT)
	{
		pstTargetSet->s32TargetNum++;
	}
	else
	{
		return -1;
	}
	
	pstTgt = &pstTargets[s32TargetNum];
	pstTgt->u32Id = pstSOBJ->u32TargetId;
	pstTgt->u32Status = 0;
	pstTgt->u32Type = 0;
	pstTgt->u32Event = 0;
	pstTgt->stPoint = pstSOBJ->stPos;
	pstTgt->stRect = pstSOBJ->stRect;
	
	TGT_MOTION_ITEM_S *item_data = (TGT_MOTION_ITEM_S*)(pstTgt->au8Data);
	TGT_TRAJECT_S *traject = &item_data->stTraject;
	
	if (traject->s32Length < IMP_MAX_TRAJECT_LEN)
	{
		traject->stPoints[traject->s32Length] = pstSOBJ->stPos;
		traject->s32Length++;
	}
	else
	{
		for (s32TI = 0; s32TI < IMP_MAX_TRAJECT_LEN - 1; s32TI++)
		{
			traject->stPoints[s32TI] = traject->stPoints[s32TI + 1];
		}
		traject->stPoints[IMP_MAX_TRAJECT_LEN - 1] = pstSOBJ->stPos;
	}
	
	return 0;
}


IMP_S32 impIfAccordRegion(IMP_STATIC_OBJ_S *pstSOBJ, OSC_OBJ_LMT_SPECL_REGIONS_S *pstOscRgn)
{
	IMP_POINT_S stPos = pstSOBJ->stPos;
	POLYGON_REGION_S *pstOscRg, *pstSpclRgA, *pstSpclRgB, *pstSpclRgC;
	IMP_S32 as32Status[4];
	IMP_S32 s32AccordRegion = 0;
	
	pstOscRg = &pstOscRgn->stOscRg;
	pstSpclRgA = &pstOscRgn->stSpclRgA;
	pstSpclRgB = &pstOscRgn->stSpclRgB;
	pstSpclRgC = &pstOscRgn->stSpclRgC;
	
	as32Status[0] = pstOscRg->s32Valid && (impInsidePolygon(pstOscRg->astPoint, pstOscRg->s32PointNum, stPos) == INSIDE);
	as32Status[1] = pstSpclRgA->s32Valid && (impInsidePolygon(pstSpclRgA->astPoint, pstSpclRgA->s32PointNum, stPos) == INSIDE);
	as32Status[2] = pstSpclRgB->s32Valid && (impInsidePolygon(pstSpclRgB->astPoint, pstSpclRgB->s32PointNum, stPos) == INSIDE);
	as32Status[3] = pstSpclRgC->s32Valid && (impInsidePolygon(pstSpclRgC->astPoint, pstSpclRgC->s32PointNum, stPos) == INSIDE);
	
	if (as32Status[0] && !as32Status[1] && !as32Status[2] && !as32Status[3])
	{
		s32AccordRegion = 1;
	}
	
	return s32AccordRegion;
}


IMP_S32 impIfAccordTimeAndSize(IMP_STATIC_OBJ_S *pstSOBJ, OSC_LMT_PARA_S stOscPara)
{
	IMP_S32 s32Size = pstSOBJ->u32AreaSize;
	IMP_S32 s32Life = pstSOBJ->u32Life;
	
//	printf("Life:%d, TimeMin:%d\n", s32Life, stOscPara.s32TimeMin);
	if (s32Size > stOscPara.s32SizeMin && s32Size < stOscPara.s32SizeMax && s32Life > stOscPara.s32TimeMin)
	{
		return 1;
	}
	
	return 0;
}


/*
//update outer target info from inner object
IMP_S32 impUpdateTargetInfo(IpTrackedTarget *pstTTarget, IMP_STATIC_OBJ_S *pstSOBJS, IMP_S32 s32CurrentTime);

IMP_S32 impOutPutResult(IMP_OSCD_S *pstModule)
{
	IMP_S32 s32I;
	IMP_S32 s32TIndex;
	IpTrackedTargetSet *pstTTS = 0;
	IpTrackedTarget *pstTargets = 0, *pstTarget = 0;
	IMP_STATIC_OBJ_S *pstSOBJS = 0;
	PEA_RESULT_S *pstResult = pstModule->pstResult; //系统公共数据
	IpTargetDataTrackerS *pstTgtData = 0;
	IpTargetPosition *pstCurPos = 0;
	IMP_S32 s32CurrentTime = 0;
	PEA_DETECTED_REGIONSET_S *pstDrgSet = 0;
	
	pstTTS = &pstResult->stTrackedTargetSetOsc;
	pstTargets = pstTTS->astTargets;
	pstDrgSet = &pstResult->stDRegionSetOsc;
	pstSOBJS = pstModule->stSObjSet.aSObjs;
	
	s32CurrentTime = pstResult->u32FrmTimeCur;
	
//	IMP_MAX_TGT_CNT 
	for (s32I = 0; s32I < OSCD_MAX_TGT_CNT; s32I++)
	{
		pstSObj = impGetSObjBySign(pstSOBJS, s32I);
		if (pstSObj && OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used) && pstSObj->s32OutTargetIndex == 0)
		{
			s32TIndex = ipTrackedTargetSetAlloc(pstTTS);
			if (s32TIndex)
			{
				pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;
				
				pstSObj->s32OutTargetIndex = s32TIndex;
				pstTarget = &pstTargets[s32TIndex];
				
				IMP_SET_TGT_INIT( pstTarget->stTargetInfo.u32Status );
				pstTarget->s32Used = IMP_TRUE;
				pstTarget->u32TargetId = pstSObj->u32TargetId;
				pstTgtData->s32FirstTime = s32CurrentTime;
				pstTgtData->s32LastTime = s32CurrentTime;
				pstTgtData->s32MatchedWidth = s32DrgIndex;
				pstTgtData->fMatchCoefficient = 1;
				pstTgtData->s32TrackedFrameNum = 1;
				
				impUpdateTargetInfo();
				
				ipTargetTrajectoryNewPosition(&pstTarget->stTrajectory);
				pstCurPos = ipTargetTrajectoryGetPosition( &pstTTarget->stTrajectory, 0 );
				memcpy( &pstTgtData->stFirstPos, pstCurPos, sizeof(IpTargetPosition) );
			}
		}
	}
	
	
	return 0;
}


//update outer target info from inner object
IMP_S32 impUpdateTargetInfo(IpTrackedTarget *pstTTarget, IMP_STATIC_OBJ_S *pstSOBJS, IMP_S32 s32CurrentTime)
{
	IpTargetPosition *pstCurPos = 0;
	IpTargetDataTrackerS *pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;
	IMP_POINT_S stPos;
	IMP_S32 s32AreaSize = 0;
	
	stPos = pstSOBJS->stPos;
	s32AreaSize = pstSOBJS->u32AreaSize;
	
	pstTgtData->s32LastTime = s32CurrentTime;
	pstTgtData->stCurCentroid.s16X = stPos.s16X;
	pstTgtData->stCurCentroid.s16Y = stPos.s16Y;
	pstTgtData->s32ObjArea = s32AreaSize;
	pstTgtData->s32ActualArea = s32AreaSize;
	pstTgtData->stMinRect = pstSOBJS->stRect;
	
	pstCurPos = ipTargetTrajectoryGetPosition(&pstTTarget->stTrajectory, 0);
	IMP_SET_TPOS_MEASURED(pstCurPos->s32Used);
	pstCurPos->u32Time = s32CurrentTime;
	pstCurPos->u32AreaPixel = s32AreaSize;
	pstCurPos->u32AreaActual = s32AreaSize;
	pstCurPos->u32DrgType = 0;
	pstCurPos->stPt = pstTgtData->stCurCentroid;
	pstCurPos->stRg = pstTgtData->stMinRect;
	
	pstTTarget->stTargetImage.pstDRegions = 0;
	pstTTarget->stTargetImage.s32DrgIndex = 0;
	
	IMP_SET_TGT_MEASURED(pstTTarget->stTargetInfo.u32Status);
	
	return 0;
}
*/




