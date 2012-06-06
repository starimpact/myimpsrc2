#include "imp_pea_tripwire.h"

#if defined(SUPPORT_TRIPWIRE)

extern IpTripwireTargetData* ipGetTripwireTargetData( IMP_VOID *pData );
IMP_VOID ipFillTargetEventDataTripwire( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{
	IMP_S32 s32ZoneIndex = pstEvent->u32Zone;
	RULE_ZONE_S *pstZone = &pstRule->stZones.astZone[s32ZoneIndex];
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
	EVT_DATA_TRIPWIRE_S *pData = (EVT_DATA_TRIPWIRE_S *)pstEvent->au8Data;
    IpTripwireTargetData *pstTargetData = ipGetTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IMP_S32 s32LineIndex = 0;
	if (pstEvent->u32Status == IMP_EVT_STATUS_START || pstEvent->u32Status == IMP_EVT_STATUS_PROCEDURE)
	{
		memcpy(&pData->stRect,&pstPos->stRg,sizeof(IMP_RECT_S));
	}

	if( pstTargetData->as32LineIndex[s32ZoneIndex] != -1 )
	{
		s32LineIndex = pstTargetData->as32LineIndex[s32ZoneIndex];
		pData->stRule.s32Bidirection = pstZone->stPara.stTripwire.astLines[s32LineIndex].s32Bidirection;
		pData->stRule.s32AbnmlAngle = pstZone->stPara.stTripwire.astLines[s32LineIndex].s32AbnmlAngle;
        pData->stRule.stLine = pstZone->stPara.stTripwire.astLines[s32LineIndex].stLine;
        pData->u8LineIndex = s32LineIndex;
	}
}

IMP_VOID ipCreateTripwireAnalysti( IpTripwireAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	memset( pstAnalyst, 0, sizeof(IpTripwireAnalyst) );
	pstAnalyst->pstResult = pstResult;
	pstAnalyst->pstHwResource = pstHwResource;
}

IMP_VOID ipReleaseTripwireAnalysti( IpTripwireAnalyst *pstAnalyst )
{
}

IMP_VOID ipConfigTripwireAnalysti( IpTripwireAnalyst *pstAnalyst, IpTripwirePara *pstParams )
{
	pstAnalyst->pstParams = pstParams;
}


static IMP_VOID ipAnalysisBehaviorTripwire( IpTripwirePara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );

IMP_S32 ipProcessTripwireAnalystInternal( IpTripwireAnalyst *pstAnalyst )
{
	IMP_S32  i, s32ZoneIndex;
	IpTrackedTarget *pstTarget;
	IpTrackedTargetSet *pstTargetSet = &pstAnalyst->pstResult->stTrackedTargetSet;
	RULE_ZONE_S *pstZone = pstAnalyst->pstParams->pstRule->stZones.astZone;
	IpTripwirePara *pstParams = pstAnalyst->pstParams;
	IpTargetPosition *pstPos;

   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTargetSet->astTargets[i];
		if( !ipTrackedTargetIsActive( pstTarget ) )
			continue;

	//	printf("ipProcessTripwireAnalystInternal:1\n");
		if( ipTrackedTargetIsMeasured( pstTarget ) )
		{	
	//	printf("ipProcessTripwireAnalystInternal:2\n");
			for( s32ZoneIndex = 0; s32ZoneIndex < IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
			{
				if( 0
					||	pstZone[s32ZoneIndex].u32Valid == 0
					||	pstZone[s32ZoneIndex].u32Enable == 0
//					||	(zone_object_available(pstTarget, image, s32ZoneIndex)) == 0
				   )
					continue;

#if defined(SUPPORT_TRIPWIRE)
				if( pstZone[s32ZoneIndex].u32Mode & IMP_FUNC_TRIPWIRE )
				{//printf("ipProcessTripwireAnalystInternal:3\n");
					ipAnalysisBehaviorTripwire( pstParams, pstTarget, s32ZoneIndex );
				}

#endif
			}
		}
		else// if( TRACKED_TARGET_is_predicted( pstTarget ) )
		{
			IMP_U32 u32TgtEvent = (0 | IMP_TGT_EVENT_TRIPWIRE);
			pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -1);
			u32TgtEvent &= pstPos->u32TgtEvent;
			pstTarget->stTargetInfo.u32TgtEvent |= u32TgtEvent;
		}
	}

	return 1;
}

IMP_S32 ipPostProcessTripwireAnalysti( IpTripwireAnalyst *pstAnalyst )
{
	return 1;
}


extern IpTripwireTargetData* ipGetTripwireTargetData( IMP_VOID *pData );
static IMP_S32  ipTripwireIsAccordantType ( IpTripwirePara *pstParams, RULE_PARA_TRIPWIRE_S  *pstTripwirePara, IpTrackedTarget *pstTarget );
static IMP_S32  ipTripwireIsBehave( IpTripwirePara *pstParams, IMP_S32 s32ZoneIndex,
                                   IpTrackedTarget *pstTarget, IMP_S32 s32LineChoice,  IMP_S32 *ps32LineIndex );

#if defined(DEBUG_ADVANCE_PARA)
static IMP_S32 ipTripwireUserAdvanceParaConstraint(IpTripwirePara *pstParams, IpTrackedTarget *pstTarget)
{
    IMP_S32 s32Ret = 1;
	IMP_S32 s32BiDir;
	IMP_S32 s32Width, s32Height;
	IpTripwirePara *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IpTargetPosition *pstPos;
	URP_ADVANCE_PARA_S  *pstURPPara;
	RULE_S *pstRule;
	USR_ADV_PARA_S *pstUsrAdvPara;
	pstRule = pstParams->pstRule;
	pstUsrAdvPara = (USR_ADV_PARA_S*)pstRule->u8AdvParaData;
    pstURPPara = &pstUsrAdvPara->stParaUrp;

#if defined(IMP_DEBUG_PRINT)
    printf("###################### tripwire advance para judgement start ####################!\n");
#endif
    if(pstURPPara->s32AdvanceParaEnable)// enable advance para
    {
#if defined(IMP_DEBUG_PRINT)
        printf("advance para is enable!\n");
#endif
        pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
        s32Width = pstPos->stRg.s16X2 - pstPos->stRg.s16X1;
        s32Height = pstPos->stRg.s16Y2 - pstPos->stRg.s16Y1;


#if defined(IMP_DEBUG_PRINT)
         printf("pixel size = %d, advance para: s32TargetMinSize = %d s32TargetMaxSize = %d  \n",pstPos->u32AreaPixel, pstURPPara->s32TargetMinSize,pstURPPara->s32TargetMaxSize);
#endif
        // pixel size
        if(pstURPPara->s32TargetMinSize >= 0)//-1 define uncheck
        {
            if(pstPos->u32AreaPixel < pstURPPara->s32TargetMinSize)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxSize >= 0)//-1 define uncheck
        {
            if(pstPos->u32AreaPixel > pstURPPara->s32TargetMaxSize)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
#if defined(IMP_DEBUG_PRINT)
        printf("s32Width constrain= %d, advance para: s32TargetMinWidth = %d s32TargetMaxWidth = %d  \n",s32Width, pstURPPara->s32TargetMinWidth,pstURPPara->s32TargetMaxWidth);
#endif
        // s32Width constrain
        if(pstURPPara->s32TargetMinWidth >= 0)//-1 define uncheck
        {
            if(s32Width < pstURPPara->s32TargetMinWidth)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxWidth >= 0)//-1 define uncheck
        {
            if(s32Width > pstURPPara->s32TargetMaxWidth)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
#if defined(IMP_DEBUG_PRINT)
        printf("s32Height constrain= %d, advance para: s32TargetMinHeight = %d s32TargetMaxHeight = %d  \n",s32Height, pstURPPara->s32TargetMinHeight,pstURPPara->s32TargetMaxHeight);
#endif
        // s32Height constrain
        if(pstURPPara->s32TargetMinHeight >= 0)//-1 define uncheck
        {
            if(s32Height < pstURPPara->s32TargetMinHeight)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxHeight >= 0)//-1 define uncheck
        {
            if(s32Height > pstURPPara->s32TargetMaxHeight)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
#if defined(IMP_DEBUG_PRINT)
        printf("speed= %d, advance para: s32TargetMinSpeed = %d s32TargetMaxSpeed = %d  \n",pstPos->s32Velocity, pstURPPara->s32TargetMinSpeed,pstURPPara->s32TargetMaxSpeed);
#endif
        //speed
        if(pstURPPara->s32TargetMinSpeed >= 0)//-1 define uncheck
        {
            if(pstPos->s32Velocity < pstURPPara->s32TargetMinSpeed)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxSpeed >= 0)//-1 define uncheck
        {
            if(pstPos->s32Velocity > pstURPPara->s32TargetMaxSpeed)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
    }
    else
    {
#if defined(IMP_DEBUG_PRINT)
        printf("advance para is disable!\n");
#endif
    }
#if defined(IMP_DEBUG_PRINT)
    printf("##############tripwire advance para judgement end ########################!\n");
#endif

	return s32Ret;
}
#else
static IMP_S32 ipTripwireUserAdvanceParaConstraint(IpTripwirePara *pstParams, IpTrackedTarget *pstTarget)
{
    IMP_S32 s32Ret = 1;
	IMP_S32 s32Width, s32Height;
	IpTripwireTargetData *pstTargetData = ipGetTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IpTargetPosition *pstPos;
	URP_ADVANCE_PARA_S  *pstURPPara;
	RULE_S *pstRule;
	USR_ADV_PARA_S *pstUsrAdvPara;
	pstRule = pstParams->pstRule;
	pstUsrAdvPara = (USR_ADV_PARA_S*)pstRule->u8AdvParaData;
    pstURPPara = &pstUsrAdvPara->stParaUrp;

    if(pstURPPara->s32AdvanceParaEnable)// enable advance para
    {
        pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
        s32Width = pstPos->stRg.s16X2 - pstPos->stRg.s16X1;
        s32Height = pstPos->stRg.s16Y2 - pstPos->stRg.s16Y1;

        // pixel size
        if(pstURPPara->s32TargetMinSize >= 0)//-1 define uncheck
        {
            if(pstPos->u32AreaPixel < pstURPPara->s32TargetMinSize)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxSize >= 0)//-1 define uncheck
        {
            if(pstPos->u32AreaPixel > pstURPPara->s32TargetMaxSize)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        // s32Width constrain
        if(pstURPPara->s32TargetMinWidth >= 0)//-1 define uncheck
        {
            if(s32Width < pstURPPara->s32TargetMinWidth)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxWidth >= 0)//-1 define uncheck
        {
            if(s32Width > pstURPPara->s32TargetMaxWidth)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        // s32Height constrain
        if(pstURPPara->s32TargetMinHeight >= 0)//-1 define uncheck
        {
            if(s32Height < pstURPPara->s32TargetMinHeight)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxHeight >= 0)//-1 define uncheck
        {
            if(s32Height > pstURPPara->s32TargetMaxHeight)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        //speed
        if(pstURPPara->s32TargetMinSpeed >= 0)//-1 define uncheck
        {
            if(pstPos->s32Velocity < pstURPPara->s32TargetMinSpeed)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
        if(pstURPPara->s32TargetMaxSpeed >= 0)//-1 define uncheck
        {
            if(pstPos->s32Velocity > pstURPPara->s32TargetMaxSpeed)
            {
                s32Ret = 0;
                return s32Ret;
            }
        }
    }
//printf("ipTripwireUserAdvanceParaConstraint:%d\n", s32Ret);
	return s32Ret;
}
#endif


IMP_S32 ipTargetNewTripwire( IpTrackedTarget *pstTTarget,IMP_S32 s32ZoneIndex )
{
    IMP_S32 i;
    IpTripwireTargetData *pstTargetData = ipGetTripwireTargetData( pstTTarget->stPrivateData.pDataAnalyst );

    if(pstTargetData->astCrossStatus[s32ZoneIndex] == TRIPWIRE_CROSS_ONCE)
    {
        for( i = 0; i < s32ZoneIndex; i++)
        {
            if(pstTargetData->astCrossStatus[i] == TRIPWIRE_CROSS_ONCE)
            {
                //pstTargetData->astCrossStatus[i] == TRIPWIRE_CROSS_NONE;
                return i + 1;
            }
        }
    }
    return 0;
}
static IMP_S32 ipTripwireAccumulateDistance(IpTripwirePara *pstParams,IpTrackedTarget *pstTarget,IMP_S32 s32AnalystLen)
{
	IpTargetPosition *pstCurPos, *pstPrePos = NULL;
	IMP_S32 i, X, Y;
	IMP_S32 s32AccumDist = 0;
	IMP_S32 s32MaxDist = -1;


	pstPrePos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-s32AnalystLen);
	for (i = 0; i < s32AnalystLen; i++)
	{
		pstCurPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		X=(pstCurPos->stPt.s16X-pstPrePos->stPt.s16X);
		Y=(pstCurPos->stPt.s16Y-pstPrePos->stPt.s16Y);

		s32AccumDist = (IMP_S32)sqrt(X * X + Y * Y);

		if (s32AccumDist > s32MaxDist)
		{
			s32MaxDist = s32AccumDist;
		}
	}

	return s32MaxDist;
}


static IMP_S32 ipTripwireIsAccordantConstraint( IpTripwirePara *pstParams, RULE_PARA_TRIPWIRE_S *pstTripwirePara, IpTrackedTarget *pstTarget,IMP_S32 s32ZoneIndex )
{
	IMP_S32 s32Ret = 0;
	TRIPWIRE_LMT_PARA_S *pstTripwireLmtPara = &pstTripwirePara->stLmtPara;
	IpTripwireTargetData *pstTargetData = ipGetTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 s32Len, s32Dist, s32Life;
	IMP_S32 s32Tolerance = pstParams->s32AwayDistance;
	s32Len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
	if( NULL == pstTargetData->apstFirstPos[s32ZoneIndex] )
	{
		pstTargetData->apstFirstPos[s32ZoneIndex] = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -(s32Len - 1) );
		pstTargetData->s32MaxDist[s32ZoneIndex] = 0;
	}
	pstPos1 = pstTargetData->apstFirstPos[s32ZoneIndex];
	pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	
	s32Life = pstPos2->u32Time - pstPos1->u32Time;
	if( s32Life >= pstTripwireLmtPara->s32TimeMin )
	{
		if( pstTargetData->s32MaxDist[s32ZoneIndex] < pstTripwireLmtPara->s32DistMin )
		{
			s32Dist = ipTripwireAccumulateDistance(pstParams,pstTarget,s32Len - 1);
			{
				if( pstTargetData->s32MaxDist[s32ZoneIndex] < s32Dist )
					pstTargetData->s32MaxDist[s32ZoneIndex] = s32Dist;
			}
		}
		s32Ret = ( pstTargetData->s32MaxDist[s32ZoneIndex] >= pstTripwireLmtPara->s32DistMin );
	}
//printf("ipTripwireIsAccordantConstraint:%d\n", s32Ret);
	return s32Ret;
}


IMP_S32 ags32LineIndex[8] = {-1,-1,-1,-1,-1,-1,-1,-1};

static IMP_VOID ipAnalysisBehaviorTripwire( IpTripwirePara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	RULE_ZONE_S *pstZone = &pstParams->pstRule->stZones.astZone[s32ZoneIndex];
	RULE_PARA_TRIPWIRE_S *pstTripwirePara = &pstZone->stPara.stTripwire;
	IpTripwireTargetData *pstTargetData = ipGetTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );

	IMP_S32 s32LineIndex = -1;

	if(!pstTargetData->astCrossStatus[s32ZoneIndex])
	{
		pstTargetData->astCrossStatus[s32ZoneIndex] += ipTripwireIsBehave(pstParams,s32ZoneIndex,pstTarget, 1,&s32LineIndex);
		if(s32LineIndex != -1)
        {
            ags32LineIndex[s32ZoneIndex] = s32LineIndex;
            pstTargetData->as32LineIndex[s32ZoneIndex] = s32LineIndex;
        }
	}
    if (1
		&& ipTripwireIsAccordantType(pstParams, pstTripwirePara, pstTarget)
		&& pstTargetData->astCrossStatus[s32ZoneIndex] == TRIPWIRE_CROSS_ONCE
		&& ipTripwireIsAccordantConstraint(pstParams, pstTripwirePara, pstTarget, s32ZoneIndex)
		&& ipTripwireUserAdvanceParaConstraint(pstParams,pstTarget)
		)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_TRIPWIRE;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
	}
}

static IMP_S32 ipTripwireIsAccordantType( IpTripwirePara *pstParams, RULE_PARA_TRIPWIRE_S *pstTripwirePara, IpTrackedTarget *pstTarget )
{
	IMP_U32 u32Type = pstTarget->stTargetInfo.u32Type;
	IpTripwireTargetData *pstTargetData = ipGetTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );

	if( pstParams->s32TripwireUseMtrend && !IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) )
		u32Type = IMP_TGT_TYPE_UNKNOWN;

	if (!pstTripwirePara->s32TypeConstrain) return 1;
//printf("pstTripwirePara->s32TypeHuman:%d, pstTripwirePara->s32TypeVehicle:%d\n", pstTripwirePara->s32TypeHuman, pstTripwirePara->s32TypeVehicle);
	if (!pstTargetData->s32TypeRecorded)
	{
		if( (   u32Type==IMP_TGT_TYPE_HUMAN && pstTripwirePara->s32TypeHuman )
			||( u32Type==IMP_TGT_TYPE_VEHICLE && pstTripwirePara->s32TypeVehicle )
			)
		{
			pstTargetData->s32TypeLikehood = 1;
			pstTargetData->s32TypeRecorded = 1;
		}
	}
	else
	{
		if(   ( u32Type==IMP_TGT_TYPE_HUMAN && pstTripwirePara->s32TypeHuman )
			||( u32Type==IMP_TGT_TYPE_VEHICLE && pstTripwirePara->s32TypeVehicle )
		  )
			pstTargetData->s32TypeLikehood++;
		else
			pstTargetData->s32TypeLikehood--;
	}

//printf("ipTripwireIsAccordantType:%d  pstTargetData->s32TypeRecorded:%d\n", pstTargetData->s32TypeLikehood, pstTargetData->s32TypeRecorded);

	if(pstTargetData->s32TypeLikehood > 100)
		pstTargetData->s32TypeLikehood = 1;
	else if(pstTargetData->s32TypeLikehood < -100)
		pstTargetData->s32TypeLikehood = 0;

	return pstTargetData->s32TypeLikehood > 0;

}

static IMP_S32 ipTripwireIsSlopOver(IpTripwirePara *pstParams, LINE_S stTripwireLine,IpTrackedTarget *pstTarget );
static IMP_S32 ipTripwireIsThrough(IpTripwirePara *pstParams,IMP_S32 s32ForbidenAngle,LINE_S stTripwireLine,IMP_S32 s32BiDirection,IpTrackedTarget *pstTarget);
static IMP_S32 ipTripwire1IsThrough(IpTripwirePara *pstParams,IMP_S32 s32ForbidenAngle,LINE_S stTripwireLine,IMP_S32 s32BiDirection,IpTrackedTarget *pstTarget);
static IMP_S32 ipTripwireIsThrough2(IpTripwirePara *pstParams,IMP_S32 s32ForbidenAngle,LINE_S stTripwireLine,IMP_S32 s32BiDirection,IpTrackedTarget *pstTarget);

static IMP_S32 ipTripwireIsBehave( IpTripwirePara  *pstParams, IMP_S32 s32ZoneIndex,
                                  IpTrackedTarget *pstTarget, IMP_S32 s32LineChoice,  IMP_S32 *ps32LineIndex )
{
	IMP_S32   s32Ret = 0;
	IMP_S32   s32Ret1  = 0;
	IMP_S32   s32ForbiddenAngle;
	IMP_S32	  s32BiDirection = 0;
	LINE_S	stTripwireLine;
	RULE_ZONE_S *pstZone = NULL;
	RULE_PARA_TRIPWIRE_S *pstTripwirePara = NULL;

	pstZone = &pstParams->pstRule->stZones.astZone[s32ZoneIndex];
	pstTripwirePara = &pstZone->stPara.stTripwire;

	{
	IMP_S32 i = 0;

	for (i = 0 ; i < IMP_MAX_TRIPWIRE_CNT; i++)
	{
		s32ForbiddenAngle = pstTripwirePara->astLines[i].s32AbnmlAngle;
		stTripwireLine = pstTripwirePara->astLines[i].stLine;
		s32BiDirection = pstTripwirePara->astLines[i].s32Bidirection;
		if (pstTripwirePara->astLines[i].s32Valid )
		{
		//	s32Ret = ipTripwireIsThrough(pstParams,s32ForbiddenAngle,stTripwireLine,s32BiDirection,pstTarget);
		//	s32Ret1 = ipTripwire1IsThrough(pstParams,s32ForbiddenAngle,stTripwireLine,s32BiDirection,pstTarget);
		//	printf("s32Ret:%d, s32Ret1:%d\n", s32Ret, s32Ret1);
            s32Ret = ipTripwireIsThrough2(pstParams,s32ForbiddenAngle,stTripwireLine,s32BiDirection,pstTarget);
        //    printf("ipTripwireIsThrough2:%d\n", s32Ret);
		//	if (s32Ret == 1 || s32Ret1 == 1)
            if (s32Ret)
			{
				*ps32LineIndex = i;
				return 1;
			}
		}
	}
	}
	return  s32Ret;
}
#if 0
static IMP_S32 ipJudgeTargetIntersectTripwire( IpTargetPosition *pstPos,LINE_S stTripwireLine,IMP_S32 s32Width,IpTrackedTarget *pstTarget )
{
	IMP_S32 s32Ret = 0;
	IMP_FLOAT k = 0;
	IMP_FLOAT b = 0;
	IMP_S32 x = 0, y = 0;
	IMP_S32 p_s_x = min(stTripwireLine.stPs.s16X,stTripwireLine.stPe.s16X);
	IMP_S32 p_s_y = min(stTripwireLine.stPs.s16Y,stTripwireLine.stPe.s16Y);
	IMP_S32 p_e_x = max(stTripwireLine.stPs.s16X,stTripwireLine.stPe.s16X);
	IMP_S32 p_e_y = max(stTripwireLine.stPs.s16Y,stTripwireLine.stPe.s16Y);
	IMP_S16 lb_x, lb_y, ru_x, ru_y;

	IMP_S32 s32RgIndex=0;
	IMP_S32 s32ObjSn = 0;
	IMP_S32 s32Ratio = 0;
	IMP_S32 s32Sn = 0;
	IMP_U8 *pu8DatCtrSn;
	IMP_S32 s32Flag = 0;
	IMP_S32 w = s32Width;
	//求警戒直线方程
	if ((stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X) == 0)
	{
		memset(&k,1,sizeof(IMP_FLOAT));

		b = stTripwireLine.stPe.s16X;
	}
	else
	{
		k = (IMP_FLOAT)(stTripwireLine.stPe.s16Y - stTripwireLine.stPs.s16Y) / (stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X);
		b = (IMP_FLOAT)(stTripwireLine.stPe.s16Y - k * stTripwireLine.stPe.s16X);
	}

	lb_x = pstPos->stRg.s16X1;
	lb_y = pstPos->stRg.s16Y1;
	ru_x = pstPos->stRg.s16X2;
	ru_y = pstPos->stRg.s16Y2;

	if ( ((k*lb_x+b) > lb_y) && ((k*lb_x+b) < ru_y) || ((k*ru_x+b) > lb_y) && ((k*ru_x+b) < ru_y) )
	{
// 		for (y=pstPos->stRg.s16Y1; y<pstPos->stRg.s16Y2; y++)
// 		{
// 			pu8DatCtrSn = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->pu8Data + w * y + pstPos->stRg.s16X1;
// 			for (x=pstPos->stRg.s16X1; x<pstPos->stRg.s16X2; x++)
// 			{
//
// 				s32Sn = pu8DatCtrSn[0];
// 				if ( s32Sn == s32ObjSn && x > p_s_x && x < p_e_x && y > p_s_y && y < p_e_y)
// 				{
// 						s32Flag = 1;
// 				}
// 				pu8DatCtrSn++;
// 			}
// 		}

		IMP_S32 s32MinX = min(pstPos->stRg.s16X1,pstPos->stRg.s16X2);
		IMP_S32 s32MinY = min(pstPos->stRg.s16Y1,pstPos->stRg.s16Y2);
		IMP_S32 s32MaxX = max(pstPos->stRg.s16X1,pstPos->stRg.s16X2);
		IMP_S32 s32MaxY = max(pstPos->stRg.s16Y1,pstPos->stRg.s16Y2);
		for (y = s32MinY; y < s32MaxY; y++)
		{
			pu8DatCtrSn = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->pu8Data + w * y + pstPos->stRg.s16X1;
			for (x = s32MinX; x < s32MaxX; x++)
			{
				//pixel_val = dat_ctr[0];
				s32Sn = pu8DatCtrSn[0];
				if ( s32Sn == s32ObjSn && x > p_s_x && x < p_e_x && y > p_s_y && y < p_e_y)
					//				if (  x > p_s_x && x < p_e_x && y > p_s_y && y < p_e_y)
				{
					s32Flag = 1;
				}
				pu8DatCtrSn++;
			}
		}
		if (s32Flag)
		{
			s32Ret = 1;
		}
		else
		{
			s32Ret = 0;
		}
	}
	else
	{
		s32Ret = 0;
	}
	return s32Ret;
}
#else
static IMP_S32 ipJudgeTargetIntersectTripwire( IpTargetPosition *pstPos,LINE_S stTripwireLine,IMP_S32 s32Width,IpTrackedTarget *pstTarget )
{
	IMP_S32 s32Ret = 0;
	IMP_FLOAT k = 0;
	IMP_FLOAT b = 0;
	IMP_S32 x = 0, y = 0;
	IMP_S32 p_s_x = min(stTripwireLine.stPs.s16X,stTripwireLine.stPe.s16X);
	IMP_S32 p_s_y = min(stTripwireLine.stPs.s16Y,stTripwireLine.stPe.s16Y);
	IMP_S32 p_e_x = max(stTripwireLine.stPs.s16X,stTripwireLine.stPe.s16X);
	IMP_S32 p_e_y = max(stTripwireLine.stPs.s16Y,stTripwireLine.stPe.s16Y);
	IMP_S16 lb_x, lb_y, ru_x, ru_y;

	IMP_S32 s32RgIndex=0;
	IMP_S32 s32ObjSn = 0;
	IMP_S32 s32Ratio = 0;
	IMP_S32 s32Sn = 0;
	IMP_U8 *pu8DatCtrSn;
	IMP_S32 s32Flag = 0;
	IMP_S32 w = s32Width;

	lb_x = pstPos->stRg.s16X1;
	lb_y = pstPos->stRg.s16Y1;
	ru_x = pstPos->stRg.s16X2;
	ru_y = pstPos->stRg.s16Y2;


	if (abs(stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X) <= 3)
	{
		if ( ru_x > p_s_x && lb_x < p_s_x && ru_y > p_s_y && lb_y < p_e_y)
		{
			s32Ret = 1;
		}
		else
		{
			s32Ret = 0;
		}
	}
	else if (abs(stTripwireLine.stPe.s16Y - stTripwireLine.stPs.s16Y) <= 3)
	{
		if ( ru_y > p_s_y && lb_y < p_s_y && ru_x > p_s_x && lb_x < p_e_x)
		{
			s32Ret = 1;
		}
		else
		{
			s32Ret = 0;
		}
	}
	else
	{
		k = (IMP_FLOAT)(stTripwireLine.stPe.s16Y - stTripwireLine.stPs.s16Y) / (stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X);
		b = (IMP_FLOAT)(stTripwireLine.stPe.s16Y - k * stTripwireLine.stPe.s16X);
		if ( ((k*lb_x+b) > lb_y) && ((k*lb_x+b) < ru_y) || ((k*ru_x+b) > lb_y) && ((k*ru_x+b) < ru_y) )
		{
			IMP_S32 s32MinX = min(pstPos->stRg.s16X1,pstPos->stRg.s16X2);
			IMP_S32 s32MinY = min(pstPos->stRg.s16Y1,pstPos->stRg.s16Y2);
			IMP_S32 s32MaxX = max(pstPos->stRg.s16X1,pstPos->stRg.s16X2);
			IMP_S32 s32MaxY = max(pstPos->stRg.s16Y1,pstPos->stRg.s16Y2);
			for (y = s32MinY; y < s32MaxY; y++)
			{
			//	pu8DatCtrSn = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->pu8Data + w * y + pstPos->stRg.s16X1;
				for (x = s32MinX; x < s32MaxX; x++)
				{
					//pixel_val = dat_ctr[0];
					//s32Sn = pu8DatCtrSn[0];s32Sn == s32ObjSn
					if (  x > p_s_x && x < p_e_x && y > p_s_y && y < p_e_y)
					{
						s32Flag = 1;
					}
				//	pu8DatCtrSn++;
				}
			}
			if (s32Flag)
			{
				s32Ret = 1;
			}
			else
			{
				s32Ret = 0;
			}
		}
	}

	return s32Ret;
}
#endif

IMP_S32 ifHasIntersection(IMP_S32 al1[4], IMP_S32 al2[4]);

static IMP_S32 ipTripwireIsThroughLine(IpTripwirePara *pstParams,LINE_S stTripwireLine, IpTrackedTarget *pstTarget)
{
	IMP_S32 s32TrajectLen;
	IMP_S32 s32TmpRst;
	IMP_FLOAT f32TmpRst;
	IpTargetPosition *pstPosCur = NULL;

	IMP_FLOAT f32Ratio = 0.6f;
	IMP_U8 u8ThroughFlag = 0;
	
	IMP_POINT_S stCurPt;
	IMP_S32 s32XY1[2], s32XY2[2];
	IMP_S32 al1[4], al2[4], minLen;
	
	minLen = 25;
	s32TrajectLen = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
//	printf("id:%d len:%d  ", pstTarget->u32TargetId, s32TrajectLen);
	if (s32TrajectLen <= minLen)
		return 0;

	pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
	if (pstParams->s32UseBottom) //using center bottom of BoundBox
	{
		stCurPt.s16X = (pstPosCur->stRg.s16X1 + pstPosCur->stRg.s16X2)/2;
		stCurPt.s16Y = pstPosCur->stRg.s16Y2;
	}
	else
	{
		memcpy(&stCurPt,&pstPosCur->stPt,sizeof(IMP_POINT_S));
	}
	s32XY1[0] = stCurPt.s16X;
	s32XY1[1] = stCurPt.s16Y;
	
	pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -minLen+1);

	if (pstParams->s32UseBottom)
	{
		stCurPt.s16X = (pstPosCur->stRg.s16X1 + pstPosCur->stRg.s16X2)/2;
		stCurPt.s16Y = pstPosCur->stRg.s16Y2;
	}
	else
	{
		memcpy(&stCurPt,&pstPosCur->stPt,sizeof(IMP_POINT_S));
	}

	s32XY2[0] = stCurPt.s16X;
	s32XY2[1] = stCurPt.s16Y;	

	{
		IMP_S32 dwRet;
		al1[0] = stTripwireLine.stPs.s16X;
		al1[1] = stTripwireLine.stPs.s16Y;
		al1[2] = stTripwireLine.stPe.s16X;
		al1[3] = stTripwireLine.stPe.s16Y;
		
		al2[0] = s32XY1[0];
		al2[1] = s32XY1[1];
		al2[2] = s32XY2[0];
		al2[3] = s32XY2[1];
		
	//	printf("id:%d \n", pstTarget->u32TargetId);
		dwRet = ifHasIntersection(al1, al2);
	//	printf("-------[%d,%d,%d,%d] [%d,%d,%d,%d]\n", al1[0], al1[1], al1[2], al1[3], al2[0], al2[1], al2[2], al2[3]);
	//	
		if (dwRet) return 1;
	}

	return 0;
}


IMP_S32 ifHasIntersection(IMP_S32 al1[4], IMP_S32 al2[4])
{
	IMP_FLOAT tg1[2], tg2[2];
	IMP_FLOAT a, b, c, d, e, f, g, h, x, y;
	IMP_FLOAT fa, fb;
	IMP_S32 d1[3], d2[3];
	IMP_S32 ok1, ok2;
	
	a = tg1[0] = al1[3] - al1[1];
	b = tg1[1] = al1[2] - al1[0];
	
	c = tg2[0] = al2[3] - al2[1];
	d = tg2[1] = al2[2] - al2[0];
	
	e = al1[1];
	f = al1[0];
	g = al2[1];
	h = al2[0];
	
	if (c*b == a*d) return 0; //parallel
	fa = a*c*(h-f);
	fa += c*b*e;
	fa -= a*d*g;
	fb = c*b-a*d;
	y = fa / fb;
	if (a == 0)
	{
		x = d*(y-g)/c+h;
	}
	else
	{
		x = b*(y-e)/a+f;
	}
//	printf("-------[%d,%d,%d,%d] [%d,%d,%d,%d] %.f,%.f\n", al1[0], al1[1], al1[2], al1[3], al2[0], al2[1], al2[2], al2[3], x, y);
	
	ok1 = 0;
	ok2 = 0;
	d1[0] = sqrtf((al1[2] - al1[0]) * (al1[2] - al1[0]) + (al1[3] - al1[1]) * (al1[3] - al1[1]));
	d1[1] = sqrtf((x - al1[0]) * (x - al1[0]) + (y - al1[1]) * (y - al1[1]));
	d1[2] = sqrtf((x - al1[2]) * (x - al1[2]) + (y - al1[3]) * (y - al1[3]));
	
	d2[0] = sqrtf((al2[2] - al2[0]) * (al2[2] - al2[0]) + (al2[3] - al2[1]) * (al2[3] - al2[1]));
	d2[1] = sqrtf((x - al2[0]) * (x - al2[0]) + (y - al2[1]) * (y - al2[1]));
	d2[2] = sqrtf((x - al2[2]) * (x - al2[2]) + (y - al2[3]) * (y - al2[3]));
	
	if (d1[1] > 0 && d1[2] > 0 && d1[0] + 1 >= d1[1] + d1[2]) ok1 = 1;
	if (d2[1] > 0 && d2[2] > 0 && d2[0] + 1 >= d2[1] + d2[2]) ok2 = 1;
//	printf("-------[%d,%d,%d]%d [%d,%d,%d]%d\n", d1[0], d1[1], d1[2], ok1, d2[0], d2[1], d2[2], ok2);
	
	if (ok1 * ok2) return 1;
	
	return 0;
}


//added by mzhang
static IMP_S32 ipTripwireIsThrough2(IpTripwirePara *pstParams,IMP_S32 s32ForbidenAngle,LINE_S stTripwireLine,IMP_S32 s32BiDirection,IpTrackedTarget *pstTarget)
{
	IpTargetPosition  *pstPos, *pstPos1;

	IMP_S32 s32Ret = 0, s32Ret1;

	pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );


//	s32Ret1 = ipJudgeTargetIntersectTripwire( pstPos,stTripwireLine,352,pstTarget );
    s32Ret1 = ipTripwireIsThroughLine(pstParams, stTripwireLine, pstTarget);
//	s32Ret1 = ipTripwireIsSlopOver( pstParams, stTripwireLine, pstTarget );
	if (!s32Ret1) return 0;

	if (s32BiDirection)
	{
		s32Ret = 1;
	}
	else
	{
        IMP_S32 sx, sy, sang, sdiff, direct;
        
        pstPos1 = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, -24);
        
        direct = atan2(pstPos->stPt.s16Y - pstPos1->stPt.s16Y, pstPos->stPt.s16X - pstPos1->stPt.s16X) * 180 / 3.1415926f;
        direct = direct < 0 ? direct + 360 : direct;
        sy = stTripwireLine.stPe.s16Y - stTripwireLine.stPs.s16Y;
        sx = stTripwireLine.stPe.s16X - stTripwireLine.stPs.s16X;
        sang = atan2(sy, sx) * 180 / 3.1415926f; //tangent angle
        sang = sang < 0 ? sang + 360 : sang;
        sang = sang + 90; //normal line
        sang = sang >= 360 ? sang - 360 : sang;
        sdiff = abs(direct - sang);
        sdiff = sdiff >= 180 ? 360 - sdiff : sdiff;
 //       printf("s32ForbidenAngle:%d, s32Direction:%d, [%d, %d]\n", s32ForbidenAngle, direct, sang, sdiff);
        if (sdiff * 2 <= s32ForbidenAngle) s32Ret = 1;
	}
	return s32Ret;
}

static IMP_S32 ipTripwire1IsThrough(IpTripwirePara *pstParams,IMP_S32 s32ForbidenAngle,LINE_S stTripwireLine,IMP_S32 s32BiDirection,IpTrackedTarget *pstTarget)
{
	IpTargetPosition  *pstPos;

	IMP_S32 s32Ret = 0;

	pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );


	s32Ret = ipJudgeTargetIntersectTripwire( pstPos,stTripwireLine,352,pstTarget );
	if (s32BiDirection)
	{
		if (s32Ret)
		{
			s32Ret = 1;
		}
	}
	else
	{
		if ( s32Ret )
		{
	//	    printf("s32ForbidenAngle:%d, pstPos->s32Direction:%d\n", s32ForbidenAngle, pstPos->s32Direction);
			if (abs(s32ForbidenAngle - pstPos->s32Direction) < 70 )
			{
				s32Ret = 1;
			}
			else if(s32ForbidenAngle > 0 && s32ForbidenAngle < 90 && pstPos->s32Direction > 270 && pstPos->s32Direction < 360)
			{
				if ( abs(360 + s32ForbidenAngle - pstPos->s32Direction) < 70 )
				{
					s32Ret = 1;
				}
				else
				{
					s32Ret = 0;
				}
			}
			else if (s32ForbidenAngle > 270 && s32ForbidenAngle < 360 && pstPos->s32Direction >= 0 && pstPos->s32Direction < 90)
			{
				if ( abs(360 - s32ForbidenAngle + pstPos->s32Direction) < 70 )
				{
					s32Ret = 1;
				}
				else
				{
					s32Ret = 0;
				}
			}
			else
			{
				s32Ret = 0;
			}

		}
		else
			s32Ret = 0;
	}
	return s32Ret;
}


static IMP_S32 ipTripwireIsThrough(IpTripwirePara *pstParams,IMP_S32 s32ForbidenAngle,LINE_S stTripwireLine,IMP_S32 s32BiDirection,IpTrackedTarget *pstTarget)
{
	IMP_FLOAT fKLine,fBLine,fLineAngle = 0;
	IMP_S32 s32TrajectLen;
	IMP_S32 s32FstPosNum = 0,s32FstNegNum = 0;
	IMP_S32 s32LastPosNum = 0, s32LastNegNum = 0;
	IMP_S32 s32FstSign = 0, s32LastSign = 0;
	IMP_S32 s32TmpRst;
	IpTargetPosition *pstPosCur = NULL;
	IMP_S32 i,s,e;
//	IMP_S32 s32AnalyLen = 50, s32SegLength = 10,s32MesureNum=0,s32TmpNum=0,s32AwayDistance = 5,s32ExtentDist = 10,s32ValidNumTh = 4;
	IMP_S32 s32AnalyLen = 15, s32SegLength = 5,s32MesureNum=0,s32AwayDistance = 3;//,s32ExtentDist = 10,s32ValidNumTh = 3;

	IMP_FLOAT s32Ratio = 0.8f;
	IMP_U8 u8ThroughFlag = 0;

	IMP_POINT_S stCurPt;

	s32AnalyLen = pstParams->s32AnalysisLength;
	s32SegLength = pstParams->s32SegLength;
	s32AwayDistance = pstParams->s32AwayDistance;
	s32Ratio = (IMP_FLOAT)pstParams->s32ValidRatio / 100;

	s32TrajectLen = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	if (s32TrajectLen < s32AnalyLen)
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
	    //added by mzhang
	    if (stTripwireLine.stPe.s16Y - stTripwireLine.stPs.s16Y >= 0)
            fLineAngle = (IMP_FLOAT)(IMP_PI/2);
        else
            fLineAngle = (IMP_FLOAT)(IMP_PI/2) + IMP_PI;
	}

	fLineAngle = (IMP_FLOAT)( (fLineAngle/IMP_PI) * 180 );


	s = -s32TrajectLen+1; e = -s32TrajectLen+1 + s32SegLength;
	for (i = s; i < e; i++)
	{
		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,i);

		if (pstParams->s32UseBottom)
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


	if (s32FstPosNum >= s32MesureNum*s32Ratio)
	{
		s32FstSign = 1;
	}

	else if(s32FstNegNum >= s32MesureNum*s32Ratio)
	{
		s32FstSign  = -1;
	}

	s32MesureNum = 0;
	s32FstPosNum=0;
	s32FstNegNum=0;
	s = -s32SegLength+1; e = 1;
	for (i = s; i < e; i++)
	{
		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,i);
// 		pstPosCur = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);

		if (pstParams->s32UseBottom)
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

	if (s32LastPosNum >= s32MesureNum * s32Ratio )
	{
		s32LastSign = 1;
	}
	else if( s32LastNegNum >= s32MesureNum * s32Ratio )
	{
		s32LastSign  = -1;
	}
// 	else
// 		return 0;



	if (s32FstSign == s32LastSign  )
	{
		return 0;
	}


	u8ThroughFlag = ipTripwireIsSlopOver(pstParams,stTripwireLine,pstTarget);
//	printf("s32ForbidenAngle:%d, fLineAngle:%f, s32FstSign:%d, s32LastSign:%d\n", s32ForbidenAngle, fLineAngle, s32FstSign, s32LastSign);
	if (!u8ThroughFlag)
	{
		return 0;
	}
	if (s32BiDirection)
	{
		return 1;
	}


	if (fLineAngle >= 0 && fLineAngle < 90)
	{
//		if ((s32ForbidenAngle >= 0 && s32ForbidenAngle<=180-fLineAngle) || (s32ForbidenAngle>=360-fLineAngle && s32ForbidenAngle<360))
		if(s32ForbidenAngle >= fLineAngle && s32ForbidenAngle < fLineAngle + 180)
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
//		if (s32ForbidenAngle>=180-fLineAngle && s32ForbidenAngle<360-fLineAngle)
		if (s32ForbidenAngle >= fLineAngle && s32ForbidenAngle < 180 + fLineAngle)
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


static IMP_S32 ipTripwireIsSlopOver( IpTripwirePara *pstParams, LINE_S stTripwireLine, IpTrackedTarget *pstTarget )
{
	IMP_FLOAT fKRef, fBRef,fKtmp,fBTmp;
	IMP_S32 j, s32Cnt;

	IMP_POINT_S stLPt, stRPt, stPrePt, stCurPt;
	IpTargetPosition *pstPosCur = NULL, *pstPosPre = NULL;
	IMP_FLOAT fTmpRst1, fTmpRst2, fTmp1, fTmp2, fTmp;

	stLPt.s16X = stTripwireLine.stPs.s16X;
	stLPt.s16Y = stTripwireLine.stPs.s16Y;
	stRPt.s16X = stTripwireLine.stPe.s16X;
	stRPt.s16Y = stTripwireLine.stPe.s16Y;


	s32Cnt = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory) - 1;
    if( stRPt.s16X != stLPt.s16X )
	{
		fKRef = (IMP_FLOAT)(stRPt.s16Y-stLPt.s16Y) / (stRPt.s16X - stLPt.s16X);
		fBRef = (IMP_FLOAT)(stRPt.s16Y - fKRef * stRPt.s16X);
	}
	else
	{
		fBRef = stRPt.s16X;
	}

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

			if (pstParams->s32UseBottom)
			{
				stCurPt.s16X = (pstPosCur->stRg.s16X1 + pstPosCur->stRg.s16X2) / 2;
				stCurPt.s16Y = pstPosCur->stRg.s16Y2;

				stPrePt.s16X = (pstPosPre->stRg.s16X1 + pstPosPre->stRg.s16X2) / 2;
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
					fKtmp = (IMP_FLOAT)(stCurPt.s16Y-stPrePt.s16Y) / (stCurPt.s16X - stPrePt.s16X);
					fBTmp = (IMP_FLOAT)(stCurPt.s16Y - fKtmp * stCurPt.s16X);
					fTmp1 = stLPt.s16Y - fKtmp * stLPt.s16X - fBTmp;
					fTmp2 = stRPt.s16Y - fKtmp * stRPt.s16X - fBTmp;
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

