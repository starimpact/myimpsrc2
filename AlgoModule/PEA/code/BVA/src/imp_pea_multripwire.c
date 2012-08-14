#include "imp_pea_multripwire.h"


#if defined(SUPPORT_MTRIPWIRE)
extern IpTripwireTargetData* ipGetTripwireTargetData( IMP_VOID *pData );
extern IpMTripwireTargetData* ipGetMTripwireTargetData( IMP_VOID *pData );

IMP_S32 IMP_GetMemSizeMTripwire(IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Size = 0;
	
	return s32Size;
}

IMP_VOID ipCreateMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{

	memset( pstAnalyst, 0, sizeof(IpMTripwireAnalyst) );
	pstAnalyst->pstResult = pstResult;
	pstAnalyst->pstHwResource = pstHwResource;
}

IMP_VOID ipReleaseMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst )
{
}

IMP_VOID ipConfigMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst, IpMTripwirePara *pstPara )
{
	pstAnalyst->pstPara = pstPara;
}

static IMP_VOID ipAnalysisBehaviorDoubleTripwire( IpMTripwirePara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex  );

IMP_S32 ipProcessMTripwireAnalystInternal( IpMTripwireAnalyst *pstAnalyst )
{
	IMP_S32  i, s32ZoneIndex;
	IpTrackedTarget *pstTarget;
	IpTrackedTargetSet *pstTargetSet = &pstAnalyst->pstResult->stTrackedTargetSet;
	RULE_ZONE_S *pstZone = pstAnalyst->pstPara->pstRule->stZones.astZone;
	IpMTripwirePara *pstPara = pstAnalyst->pstPara;
	IpTargetPosition *pstPos;

   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTargetSet->astTargets[i];
		if( !ipTrackedTargetIsActive( pstTarget ) )
			continue;

		if( ipTrackedTargetIsMeasured( pstTarget ) )
		{
			for( s32ZoneIndex=0; s32ZoneIndex<IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
			{
				if( 0
					||	pstZone[s32ZoneIndex].u32Valid == 0
					||	pstZone[s32ZoneIndex].u32Enable == 0
//					||	(zone_object_available(pstTarget, image, s32ZoneIndex)) == 0
				   )
					continue;


#if defined(SUPPORT_MTRIPWIRE)
				if( pstZone[s32ZoneIndex].u32Mode & IMP_FUNC_MTRIPWIRE )
				{
					ipAnalysisBehaviorDoubleTripwire(pstPara, pstTarget, s32ZoneIndex);
				}

#endif
			}
		}
		else
		{
			IMP_U32 u32TgtEvent = (0|IMP_TGT_EVENT_MTRIPWIRE);
			pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -1);
			u32TgtEvent &= pstPos->u32TgtEvent;
			pstTarget->stTargetInfo.u32TgtEvent |= u32TgtEvent;
		}
	}

	return 1;
}

IMP_S32 ipPostProcessMTripwireAnalysti( IpMTripwireAnalyst *pstAnalyst )
{
	return 1;
}


IMP_VOID ipFillTargetEventDataMTripwire( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{
	IMP_S32 s32ZoneIndex = pstEvent->u32Zone;
	RULE_ZONE_S *pstZone = &pstRule->stZones.astZone[s32ZoneIndex];
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
 	EVT_DATA_MTRIPWIRE_S *pData = (EVT_DATA_MTRIPWIRE_S *)pstEvent->au8Data;
    IpMTripwireTargetData *pstTargetData = ipGetMTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IMP_S32 s32LineIndex = 0;
	if (pstEvent->u32Status == IMP_EVT_STATUS_START || pstEvent->u32Status == IMP_EVT_STATUS_PROCEDURE)
	{
		memcpy(&pData->stRect,&pstPos->stRg,sizeof(IMP_RECT_S));
	}

	if( pstTargetData->as32LineIndex[s32ZoneIndex] != -1 )
	{
		s32LineIndex = pstTargetData->as32LineIndex[s32ZoneIndex];
		pData->stRule.as32AbnmlAngle[0] = pstZone->stPara.stMTripwire.astLines[s32LineIndex].as32AbnmlAngle[0];
		pData->stRule.as32AbnmlAngle[1] = pstZone->stPara.stMTripwire.astLines[s32LineIndex].as32AbnmlAngle[1];
        memcpy(pData->stRule.astLine,pstZone->stPara.stMTripwire.astLines[s32LineIndex].astLine, sizeof(LINE_S));
        pData->u8LineIndex = s32LineIndex;
	}

}

static IMP_S32  ipMTripwireIsAccordantType ( IpMTripwirePara *pstPara, RULE_PARA_MTRIPWIRE_S  *pstMTripwirePara, IpTrackedTarget *pstTarget,IMP_S32 s32ZoneIndex );
static IMP_S32 ipMTripwireIsAccordantConstrain( IpMTripwirePara *pstPara, RULE_PARA_MTRIPWIRE_S *pstMTripwirePara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );

static IMP_S32 ipMTripwireIsBehave( IpMTripwireTargetData *pstTargetData, IpMTripwirePara  *pstPara, IMP_S32 s32ZoneIndex,
								  IpTrackedTarget *pstTarget, IMP_S32 s32LineChoice, IMP_S32 s32TimeCur);



static IMP_VOID ipAnalysisBehaviorDoubleTripwire( IpMTripwirePara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex  )
{
	IMP_S32 s32LineChoice1, s32LineChoice2;
	IMP_S32 s32Line1Status, s32Line2Status;
	IMP_S32 s32TimeCur, s32Time1, s32Time2;
	RULE_PARA_MTRIPWIRE_S  *pstMTripwirePara;
	RULE_ZONE_S *pstZone = &pstPara->pstRule->stZones.astZone[s32ZoneIndex];
	IpMTripwireTargetData *pstTargetData = ipGetMTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );
	pstMTripwirePara = &pstPara->pstRule->stZones.astZone[s32ZoneIndex].stPara.stMTripwire;

	s32Line1Status = 0;
	s32Line2Status = 0;
	s32TimeCur = 0;
	s32Time1 = 0;
	s32Time2 = 0;


	s32TimeCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, 0)->u32Time;



	s32LineChoice1 = 0;


	ipMTripwireIsBehave(pstTargetData,pstPara,s32ZoneIndex, pstTarget,s32LineChoice1,s32TimeCur );


	s32LineChoice2 = 1;
	ipMTripwireIsBehave(pstTargetData,pstPara,s32ZoneIndex, pstTarget,s32LineChoice2,s32TimeCur );


	if (
//	ipMTripwireIsAccordantType(pstPara,pstMTripwirePara,pstTarget,s32ZoneIndex)
//		&&
		ipMTripwireIsAccordantConstrain(pstPara,pstMTripwirePara,pstTarget,s32ZoneIndex)
		)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_MTRIPWIRE;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
	}


}

static IMP_S32 ipMTripwireIsAccordantConstrain( IpMTripwirePara *pstPara, RULE_PARA_MTRIPWIRE_S *pstMTripwirePara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	IMP_S32 i = 0;
	IMP_S32 s32Ret = 0;
	IMP_S32 s32Time1 = 0;
	IMP_S32 s32Time2 = 0;
    IMP_S32	s32Life = 0;
	IpMTripwireTargetData *pstTargetData;
	IMP_U32 u32Type = pstTarget->stTargetInfo.u32Type;
	MTRIPWIRE_LMT_PARA_S *pstMTripwireLmtPara = &pstMTripwirePara->stLmtPara;
	pstTargetData = ipGetMTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );


	for (i = 0 ; i < IMP_MAX_MTRIPWIRE_CNT; i++)
	{
		if (pstMTripwirePara->astLines[i].s32Valid )
		{
			s32Time1 = pstTargetData->astTargetData[0].s32TypeTime[s32ZoneIndex][i];
			s32Time2 = pstTargetData->astTargetData[1].s32TypeTime[s32ZoneIndex][i];
			s32Life = abs(s32Time2 - s32Time1);
			if (pstTargetData->astTargetData[0].astCrossStatus[s32ZoneIndex][i] > MTRIPWIRE_CROSS_NONE
				&&pstTargetData->astTargetData[1].astCrossStatus[s32ZoneIndex][i] > MTRIPWIRE_CROSS_NONE
				&& s32Life >= pstMTripwireLmtPara->s32TimeMin
				)
			{
				pstTargetData->as32LineIndex[s32ZoneIndex] = i;
				s32Ret = 1;
			}
		}
	}

	return s32Ret;
};

static IMP_S32 ipMTripwireIsAccordantType( IpMTripwirePara *pstPara, RULE_PARA_MTRIPWIRE_S *pstMTripwirePara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	IMP_S32 s32Ret = 0;

	IpMTripwireTargetData *pstTargetData;

	IMP_U32 u32Type = pstTarget->stTargetInfo.u32Type;
	pstTargetData = ipGetMTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );

	if( pstPara->astSTripwire[0].s32TripwireUseMtrend && !IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) )
		u32Type = IMP_TGT_TYPE_UNKNOWN;

	if (!pstTargetData->s32TypeRecorded)
	{
		if( (   u32Type == IMP_TGT_TYPE_HUMAN && pstMTripwirePara->s32TypeHuman )
			||( u32Type == IMP_TGT_TYPE_VEHICLE && pstMTripwirePara->s32TypeVehicle)
			)
		{
			pstTargetData->s32TypeLikehood = 1;
			pstTargetData->s32TypeRecorded = 1;
		}
	}
	else
	{
		if(   ( u32Type == IMP_TGT_TYPE_HUMAN && pstMTripwirePara->s32TypeHuman )
			||( u32Type == IMP_TGT_TYPE_VEHICLE && pstMTripwirePara->s32TypeVehicle )
		  )
			pstTargetData->s32TypeLikehood++;
		else
			pstTargetData->s32TypeLikehood--;
	}

	if(pstTargetData->s32TypeLikehood > 100)
		pstTargetData->s32TypeLikehood = 1;
	else if(pstTargetData->s32TypeLikehood < -100)
		pstTargetData->s32TypeLikehood = 0;


	s32Ret =  (pstTargetData->s32TypeLikehood > 0);
	return s32Ret;
}

static IMP_S32 ipMTripwireIsSlopOver(IpMTripwirePara *pstPara, LINE_S stTripwireLine, IpTrackedTarget *pstTarget, IMP_S32 s32LineChoice);
static IMP_S32 ipMTripwireIsThrough(IpMTripwirePara *pstPara, IMP_S32 s32TripwireForbiddenAngle, LINE_S stTripwireLine, IpTrackedTarget *pstTarget, IMP_S32 s32LineChoice);


static IMP_S32 ipMTripwireIsBehave( IpMTripwireTargetData *pstTargetData, IpMTripwirePara  *pstPara, IMP_S32 s32ZoneIndex,
								  IpTrackedTarget *pstTarget, IMP_S32 s32LineChoice, IMP_S32 s32TimeCur)
{
	IMP_S32 s32Ret = 0;
	IMP_S32 s32ForbiddenAngle;
	LINE_S stTripwireLine;
	RULE_ZONE_S *pstZone = NULL;
	RULE_PARA_MTRIPWIRE_S *pstMTripwirePara = NULL;

	pstZone = &pstPara->pstRule->stZones.astZone[s32ZoneIndex];
	pstMTripwirePara = &pstZone->stPara.stMTripwire;
	{
		IMP_S32 i = 0;
		for (i = 0 ; i < IMP_MAX_MTRIPWIRE_CNT; i++)
		{
			s32ForbiddenAngle = pstMTripwirePara->astLines[i].as32AbnmlAngle[s32LineChoice];
			stTripwireLine = pstMTripwirePara->astLines[i].astLine[s32LineChoice];
			if (pstMTripwirePara->astLines[i].s32Valid )
			{
				if(!pstTargetData->astTargetData[s32LineChoice].astCrossStatus[s32ZoneIndex][i])
				{
					s32Ret = ipMTripwireIsThrough(pstPara,s32ForbiddenAngle,stTripwireLine,pstTarget,s32LineChoice);
					if (s32Ret == 1)
					{
						pstTargetData->astTargetData[s32LineChoice].astCrossStatus[s32ZoneIndex][i] += 1;
						pstTargetData->astTargetData[s32LineChoice].s32TypeTime[s32ZoneIndex][i] = s32TimeCur;
					}
				}

			}
		}
	}
	return  s32Ret;
}


static IMP_S32 ipMTripwireIsThrough(IpMTripwirePara *pstPara,IMP_S32 s32TripwireForbiddenAngle,LINE_S stTripwireLine,IpTrackedTarget *pstTarget,IMP_S32 s32LineChoice)
{
	IMP_FLOAT fKLine,fBLine,fLineAngle = 0;
	IMP_S32 s32TrajectLen;			£»
	IMP_S32 s32FstPosNum = 0,s32FstNegNum = 0;
	IMP_S32 s32LastPosNum = 0, s32LastNegNum = 0;
	IMP_S32 s32FstSign = 0, s32LastSign = 0;
	IMP_S32 s32TmpRst;
	IpTargetPosition *pstPosCur = NULL;
	IMP_S32 i,s,e;
//	IMP_S32 s32AnalysisLen = 50, s32SegLen = 10,s32MesureNum=0,temp_num=0,s32AwayDistance = 5,s32ExtendDist = 10,s32ValidNumTh = 4;
	IMP_S32 s32AnalysisLen = 15, s32SegLen = 5, s32MesureNum = 0, s32AwayDistance = 3;//,s32ExtendDist = 10,s32ValidNumTh = 3;

	IMP_FLOAT fRatio = 0.8f;
	IMP_U8 u8ThroughFlag = 0;

	IMP_POINT_S stCurPt;

	s32AnalysisLen = pstPara->astSTripwire[s32LineChoice].s32AnalysisLength;
	s32SegLen = pstPara->astSTripwire[s32LineChoice].s32SegLength;
	s32AwayDistance = pstPara->astSTripwire[s32LineChoice].s32AwayDistance;
	fRatio = (IMP_FLOAT)pstPara->astSTripwire[s32LineChoice].s32ValidRatio / 100;

	s32TrajectLen = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	if (s32TrajectLen < s32AnalysisLen)
		return 0;


	if ((stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X) == 0)
	{
		memset(&fKLine,1,sizeof(IMP_FLOAT));
		fBLine = stTripwireLine.stPe.s16X;
	}
	else
	{
		fKLine = (IMP_FLOAT)(stTripwireLine.stPe.s16Y - stTripwireLine.stPs.s16Y) / (stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X);
		fBLine = (IMP_FLOAT)(stTripwireLine.stPe.s16Y - fKLine * stTripwireLine.stPe.s16X);
	}


	if((stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X) != 0)
	{
		fLineAngle = (IMP_FLOAT)atan(fKLine);
		if (fLineAngle < 0)
		{
			fLineAngle = (IMP_FLOAT)(IMP_PI + fLineAngle);
		}
	}
	else
	{
		fLineAngle = (IMP_FLOAT)(IMP_PI/2);
	}

	fLineAngle = (IMP_FLOAT)( (fLineAngle/IMP_PI) * 180 );


	s = -s32TrajectLen+1; e = -s32TrajectLen+1 + s32SegLen;
	for (i = s; i < e; i++)
	{
		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,i);

		if (pstPara->astSTripwire[s32LineChoice].s32UseBottom)
		{
			stCurPt.s16X = (pstPosCur->stRg.s16X1 + pstPosCur->stRg.s16X2)/2;
			stCurPt.s16Y = pstPosCur->stRg.s16Y2;
		}
		else
		{
			memcpy(&stCurPt,&pstPosCur->stPt,sizeof(IMP_POINT_S));
		}

		if(pstPosCur->s32Used == IMP_TPOS_STATUS_MEASURED)
		{
			if((stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X) != 0)
			{
				s32TmpRst = (IMP_S32)( stCurPt.s16Y - fKLine*stCurPt.s16X - fBLine );
				s32TmpRst = (IMP_S32)( s32TmpRst/(1+fKLine*fKLine) );
			}
			else
			{
				s32TmpRst = (IMP_S32)( stCurPt.s16X - fBLine );

			}
			s32MesureNum++;
			if (s32TmpRst > s32AwayDistance)
				s32FstPosNum++;
			else if(s32TmpRst <-s32AwayDistance)
				s32FstNegNum++;
		}
	}


	if (s32FstPosNum >= s32MesureNum*fRatio)
	{
		s32FstSign = 1;
	}

	else if(s32FstNegNum >= s32MesureNum*fRatio)
	{
		s32FstSign  = -1;
	}
// 	else
// 		return 0;


	s32MesureNum = 0;
	s32FstPosNum = 0;
	s32FstNegNum = 0;
	s = -s32SegLen + 1; e = 1;
	for (i = s; i < e; i++)
	{
		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,i);
// 		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);

		if (pstPara->astSTripwire[s32LineChoice].s32UseBottom)
		{
			stCurPt.s16X = (pstPosCur->stRg.s16X1 + pstPosCur->stRg.s16X2)/2;
			stCurPt.s16Y = pstPosCur->stRg.s16Y2;
		}
		else
		{
			memcpy(&stCurPt,&pstPosCur->stPt,sizeof(IMP_POINT_S));
		}


		if (pstPosCur->s32Used == IMP_TPOS_STATUS_MEASURED)
		{

			if((stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X) != 0)
			{
				s32TmpRst = (IMP_S32)( stCurPt.s16Y - fKLine * stCurPt.s16X - fBLine );
				s32TmpRst = (IMP_S32)( s32TmpRst / (1 + fKLine * fKLine) );
			}
			else
			{
				s32TmpRst = (IMP_S32)( stCurPt.s16X - fBLine );
			}
			s32MesureNum++;

			if ( s32TmpRst > s32AwayDistance )
				s32LastPosNum++;
			else if(s32TmpRst < -s32AwayDistance)
				s32LastNegNum++;
		}
	}

	if (s32LastPosNum >= s32MesureNum * fRatio )
	{
		s32LastSign = 1;
	}
	else if( s32LastNegNum >= s32MesureNum * fRatio )
	{
		s32LastSign  = -1;
	}

	if (s32FstSign == s32LastSign  )
	{
		return 0;
	}


	u8ThroughFlag = ipMTripwireIsSlopOver(pstPara,stTripwireLine,pstTarget,s32LineChoice);
	if (!u8ThroughFlag)
	{
		return 0;
	}


	if (fLineAngle >= 0 && fLineAngle < 90)
	{
//		if ((s32TripwireForbiddenAngle >= 0 && s32TripwireForbiddenAngle<=180-fLineAngle) || (s32TripwireForbiddenAngle>=360-fLineAngle && s32TripwireForbiddenAngle<360))
		if(s32TripwireForbiddenAngle >= fLineAngle && s32TripwireForbiddenAngle < fLineAngle + 180)
		{
			if (s32FstSign == -1 && s32LastSign == 1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (s32FstSign == 1 && s32LastSign == -1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}
	else
	{
//		if (s32TripwireForbiddenAngle>=180-fLineAngle && s32TripwireForbiddenAngle<360-fLineAngle)
		if (s32TripwireForbiddenAngle >= fLineAngle && s32TripwireForbiddenAngle < 180 + fLineAngle)
		{
			if (s32FstSign == 1 && s32LastSign == -1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			if (s32FstSign == -1 && s32LastSign == 1)
			{
				return 1;
			}
			else
			{
				return 0;
			}
		}
	}

	return 0;
}

static IMP_S32 ipMTripwireIsSlopOver( IpMTripwirePara *pstPara, LINE_S stTripwireLine, IpTrackedTarget *pstTarget ,IMP_S32 s32LineChoice)
{
	IMP_FLOAT fKRef, fBRef,fKTmp,fBTmp;
	IMP_S32 j, s32Cnt;

	IMP_POINT_S stLPt, stRPt, stPrePt, stCurPt;
	IpTargetPosition *pstPosCur = NULL, *pstPosPre = NULL;
	IMP_FLOAT fTmpRst1,fTmpRst2,fTmp1,fTmp2,fTmp;

	stLPt.s16X = stTripwireLine.stPs.s16X;
	stLPt.s16Y = stTripwireLine.stPs.s16Y;
	stRPt.s16X = stTripwireLine.stPe.s16X;
	stRPt.s16Y = stTripwireLine.stPe.s16Y;


	s32Cnt = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory) - 1;
    if( stRPt.s16X != stLPt.s16X )
	{
		fKRef = (IMP_FLOAT)(stRPt.s16Y-stLPt.s16Y)/(stRPt.s16X-stLPt.s16X);
		fBRef = (IMP_FLOAT)(stRPt.s16Y - fKRef * stRPt.s16X);
	}
	else
	{
		fBRef = stRPt.s16X;
	}
	// search for the crossed point
	for(j = 0; j >= (-s32Cnt); j--)
	{
		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,j);
		if(pstPosCur->s32Used == IMP_TPOS_STATUS_MEASURED)
		{
			if (pstPosPre == NULL)
			{
				pstPosPre = pstPosCur;
				continue;
			}

			if (pstPara->astSTripwire[s32LineChoice].s32UseBottom)
			{
				stCurPt.s16X = (pstPosCur->stRg.s16X1 + pstPosCur->stRg.s16X2)/2;
				stCurPt.s16Y = pstPosCur->stRg.s16Y2;

				stPrePt.s16X = (pstPosPre->stRg.s16X1 + pstPosPre->stRg.s16X2)/2;
				stPrePt.s16Y = pstPosPre->stRg.s16Y2;
			}
			else
			{
				stCurPt.s16X = pstPosCur->stPt.s16X;
				stCurPt.s16Y = pstPosCur->stPt.s16Y;
				stPrePt.s16X = pstPosPre->stPt.s16X;
				stPrePt.s16Y = pstPosPre->stPt.s16Y;
			}

			if (stRPt.s16X != stLPt.s16X)
			{
				fTmpRst1 = stCurPt.s16Y - fKRef*stCurPt.s16X - fBRef;
				fTmpRst2 = stPrePt.s16Y - fKRef*stPrePt.s16X - fBRef;
			}
			else
			{
				fTmpRst1 = stCurPt.s16X - fBRef;
				fTmpRst2 = stPrePt.s16X - fBRef;
			}
			fTmp = fTmpRst1 * fTmpRst2;
			if(fTmp <= 0)
			{
				if (stCurPt.s16X != stPrePt.s16X)
				{
					fKTmp = (IMP_FLOAT)(stCurPt.s16Y-stPrePt.s16Y) / (stCurPt.s16X-stPrePt.s16X);
					fBTmp = (IMP_FLOAT)(stCurPt.s16Y - fKTmp * stCurPt.s16X);
					fTmp1 = stLPt.s16Y - fKTmp * stLPt.s16X - fBTmp;
					fTmp2 = stRPt.s16Y - fKTmp * stRPt.s16X - fBTmp;
				}
				else
				{
					fBTmp = stCurPt.s16X;
					fTmp1 = stLPt.s16X - fBTmp;
					fTmp2 = stRPt.s16X - fBTmp;
				}
				if (fTmp1 * fTmp2 <= 0)
				{
					return 1;
				}
			}


			pstPosPre = pstPosCur;
		}
	}
	return 0;
}

#endif

