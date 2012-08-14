#include "imp_pea_perimeter.h"
#include "imp_pea_target.h"

#if defined(SUPPORT_PERIMETER) | defined(SUPPORT_ABNORMALVEL) | defined(SUPPORT_CONVERSE)

IpPerimeterTargetData* ipGetPerimeterTargetData( IMP_VOID *pData );
static IMP_S32 ipInitialStaticList(IpStaticList *pstList,IMP_S32 s32NodeNum);
static IMP_VOID ipRemoveFreeNode(IpStaticList *pstList);
static IMP_S32 ipGetNextFreeNode(IpStaticList *pstList);
static IMP_S32 ipAddUsedNode(IpStaticList *pstList,IMP_S32 s32NodeId);
static IMP_S32 ipGetNextUsedNode(IpStaticList *pstList,IMP_S32 s32NodeId);
static IMP_S32 ipRemoveUsedNode(IpStaticList *pstList,IMP_S32 s32NodeId);
static IMP_S32 ipAddFreeNode(IpStaticList *pstList,IMP_S32 s32NodeId);
static IMP_S32 ipGetFreeNum(IpStaticList *pstList);
static IMP_S32 ipGetUsedNum(IpStaticList *pstList);


IMP_S32 IMP_GetMemSizePerimeter(IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Size = 0;
	
	s32Size += 0;
	
	return s32Size;
}


IMP_VOID ipCreatePerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	memset( pstAnalyst, 0, sizeof(IpPerimeterAnalyst) );
	pstAnalyst->pstResult = pstResult;
	pstAnalyst->pstHwResource = pstHwResource;

	ipInitialStaticList(&pstAnalyst->stStaticList,IMP_MAX_TGT_CNT);
	memset(pstAnalyst->astRecordInfo,0,sizeof(pstAnalyst->astRecordInfo));
}

IMP_VOID ipReleasePerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst )
{
}

IMP_VOID ipConfigPerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst, IpPerimeterPara *pstParams )
{
	pstAnalyst->pstParams = pstParams;
}

IMP_S32 ipSafeDistanceDetectionZonesSet(IpPerimeterAnalyst *pstAnalyst)
{

	RULE_ZONE_S *pstZone = pstAnalyst->pstParams->pstRule->stZones.astZone;
	GRAY_IMAGE_S *pstImage = &pstAnalyst->pstParams->pstRule->stZones.stImage;
	IpPerimeterPara *pstParams = pstAnalyst->pstParams;
	RULE_PARA_PERIMETER_S *pstPeriPara = NULL;

	IMP_S32 s32ZoneIndex;
	IMP_S32 s32PixelVal;
	IMP_S32 s32Width = pstImage->s32W;
	IMP_S32 s32Height = pstImage->s32H;
	IMP_S32 s32PixelNum = s32Width * s32Height;
	IMP_S32 s32Index;
	IMP_S32 s32Ret = 0;

	for( s32ZoneIndex = 0; s32ZoneIndex < IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
	{
		if( !IMP_IS_ACTIVE_ZONE( &pstZone[s32ZoneIndex], IMP_FUNC_PERIMETER ) )
			continue;

		pstPeriPara = &pstParams->pstRule->stZones.astZone[s32ZoneIndex].stPara.stPerimeter;

		if( (pstPeriPara->s32Mode != IMP_PMODE_EXIT ) )
			continue;
#ifdef _TIDSP
		#pragma MUST_ITERATE(32,,32);
#endif
		for ( s32Index = 0; s32Index < s32PixelNum; s32Index++ )
		{
			s32PixelVal = pstImage->pu8Data[s32Index];
			s32Ret = s32PixelVal >> s32ZoneIndex & 0x01;
			if (!s32Ret)
				break;
		}
		if (!s32Ret)
		{
			continue;
		}
		else
		{
			for ( s32Index =  s32PixelNum / 2; s32Index < s32PixelNum; s32Index++ )
			{
				s32PixelVal = pstImage->pu8Data[s32Index];
				pstImage->pu8Data[s32Index] = s32PixelVal &(~(1 << s32ZoneIndex));
			}
		}
	}
	if (0)
	{

		FILE *pFile ;
		for ( s32Index = 0; s32Index < s32PixelNum; s32Index++ )
		{
			{
				if (pstImage->pu8Data[s32Index] == 254)
				{
					pstImage->pu8Data[s32Index] = 0;
				}
				else if (pstImage->pu8Data[s32Index] == 255)
				{
					pstImage->pu8Data[s32Index] = 255;
				}
			}
		}
		pFile = fopen( "c:/safedist.raw", "wb" );
		fwrite( pstImage->pu8Data, s32PixelNum, 1, pFile );
		fclose(pFile);
	}
	return s32Ret;
}
///////////////////////////////////////////////////////////////////////////////////////////
static IMP_S32 ipPreProcessPerimeterAnalystInternal( IpPerimeterAnalyst *pstAnalyst )
{
	IMP_S32  i, s32ZoneIndex;
	IpTrackedTarget *pstTarget;
	IpTrackedTargetSet *pstTargetSet = &pstAnalyst->pstResult->stTrackedTargetSet;
	RULE_ZONE_S *pstZone = pstAnalyst->pstParams->pstRule->stZones.astZone;
	GRAY_IMAGE_S *pstImage = &pstAnalyst->pstParams->pstRule->stZones.stImage;
	IpPerimeterPara *pstParams = pstAnalyst->pstParams;
	RULE_PARA_PERIMETER_S *pstPeriPara = NULL;
	IpPerimeterTargetData *pstTargetData = NULL;
	IMP_U8 u8ZoneValue = 0;

   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTargetSet->astTargets[i];
		pstTargetData = ipGetPerimeterTargetData(pstTarget->stPrivateData.pDataAnalyst);

		if( !ipTrackedTargetIsValid( pstTarget ) )
			continue;
//		if ( IMP_IS_TGT_INAIR(pstTarget->s32Used) )
//			continue;

		if( IMP_IS_TGT_COLLISION(pstTarget->s32Used) && pstTargetData->s32CollisionNum<8 )
		{
			continue;
		}
		u8ZoneValue = 0;
		
	//	
#ifdef SUPPORT_PERIMETER

	/*	if (!pstAnalyst->safe_dis_det_set_flag)
		{
			ipSafeDistanceDetectionZonesSet(pstAnalyst);
			pstAnalyst->safe_dis_det_set_flag = 1;
		}*/
		
		////////////////////////////////////////////
		for( s32ZoneIndex = 0; s32ZoneIndex < IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
		{
			if( !IMP_IS_ACTIVE_ZONE( &pstZone[s32ZoneIndex], IMP_FUNC_PERIMETER ) )
				continue;

				pstPeriPara = &pstParams->pstRule->stZones.astZone[s32ZoneIndex].stPara.stPerimeter;

				if(	( pstPeriPara->s32Mode == IMP_PMODE_INTRUSION )
					&& (ipZoneObjectEffective(pstTarget, pstImage, s32ZoneIndex,pstParams->s32PerimeterUseBottom,pstParams->s32PerimeterDelicacy,pstParams->s32PerimeterIntrusionRatio)!=0)
					)
				{
					ipSetBitValue(s32ZoneIndex,&u8ZoneValue);
				}
//
//    			if( (pstPeriPara->s32Mode == PMODE_APPEAR )
//					&&	(ipZoneObjectEffective(pstTarget, pstImage, s32ZoneIndex,pstParams->s32PerimeterUseBottom,0,pstParams->s32PerimeterAppearRatio)!=0)
//					)
//				{
//					ipSetBitValue(s32ZoneIndex,&u8ZoneValue);
//				}

				if( (pstPeriPara->s32Mode == IMP_PMODE_ENTER )
					&&	(ipZoneObjectEffective(pstTarget, pstImage, s32ZoneIndex,pstParams->s32PerimeterUseBottom,0,pstParams->s32PerimeterEnterRatio)!=0)
					)
				{
					ipSetBitValue(s32ZoneIndex,&u8ZoneValue);
				}

				if( (pstPeriPara->s32Mode == IMP_PMODE_EXIT )
					&& (ipZoneObjectEffective(pstTarget, pstImage, s32ZoneIndex,pstParams->s32PerimeterUseBottom,0,pstParams->s32PerimeterExitRatio)!=0)
					)
				{
					ipSetBitValue(s32ZoneIndex,&u8ZoneValue);
				}
//
//				if( (pstPeriPara->s32Mode == PMODE_DISAPPEAR )
//					&& (ipZoneObjectEffective(pstTarget, pstImage, s32ZoneIndex,pstParams->s32PerimeterUseBottom,0,pstParams->s32PerimeterExitRatio)!=0)
//					)
//				{
//					ipSetBitValue(s32ZoneIndex,&u8ZoneValue);
//				}
		}
		ipByteCArrayPushElem(&pstTargetData->stPerimeterCycleArray,u8ZoneValue);
#endif

	}
	return 1;
}
static IMP_VOID ipOutputDisppearEvent( IpPerimeterAnalyst *pstAnalyst );
static IMP_VOID ipOutputDisppearTarget( IpPerimeterAnalyst *pstAnalyst );
static IMP_VOID ipDeterminDisappearStatus(IpPerimeterAnalyst *pstAnalyst,IMP_S32 s32ZoneIndex);
static IMP_VOID ipRemoveUnusedRecord( IpPerimeterAnalyst *pstAnalyst );
static IMP_S32 ipAnalysisBehaviorPerimeter( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_S32 ipAnalysisBehaviorPerimeterAppear( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_S32 ipAnalysisBehaviorPerimeterEnter( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_S32 ipAnalysisBehaviorPerimeterExit( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_VOID ipAnalysisBehaviorPerimeterDisappear( IpPerimeterAnalyst *pstAnalyst, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_S32 ipAnalysisBehaviorAbnormalVel( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_S32 ipAnalysisBehaviorConverse( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
IMP_S32 ipProcessPerimeterAnalystInternal( IpPerimeterAnalyst *pstAnalyst )
{
	IMP_S32  i, s32ZoneIndex;
	IpTrackedTarget *pstTarget;
	IpTrackedTargetSet *pstTargetSet = &pstAnalyst->pstResult->stTrackedTargetSet;
	RULE_ZONE_S *pstZone = pstAnalyst->pstParams->pstRule->stZones.astZone;
	GRAY_IMAGE_S *pstImage = &pstAnalyst->pstParams->pstRule->stZones.stImage;
	IpPerimeterPara *pstParams = pstAnalyst->pstParams;
	RULE_PARA_PERIMETER_S *pstPeriPara = NULL;
	IpPerimeterTargetData *pstTargetData = NULL;
	IMP_S32 s32Disappear = 0;

	ipPreProcessPerimeterAnalystInternal(pstAnalyst);

   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTargetSet->astTargets[i];
		pstTargetData = ipGetPerimeterTargetData(pstTarget->stPrivateData.pDataAnalyst);

		if( !ipTrackedTargetIsActive( pstTarget ) )
			continue;
		if( IMP_IS_TGT_INAIR(pstTarget->s32Used) )
			continue;


		if( IMP_IS_TGT_COLLISION(pstTarget->s32Used) )
		{
			pstTargetData->s32CollisionNum++;
			if (pstTargetData->s32CollisionNum>=8)
				pstTargetData->s32CollisionNum = 0;
			else
				continue;
		}

		
		if( ipTrackedTargetIsMeasured( pstTarget ) )
		{
			for( s32ZoneIndex = 0; s32ZoneIndex < IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
			{
				if( !IMP_IS_ACTIVE_ZONE( &pstZone[s32ZoneIndex], (IMP_FUNC_PERIMETER) ) )
					continue;

				
#ifdef SUPPORT_PERIMETER
				pstPeriPara = &pstParams->pstRule->stZones.astZone[s32ZoneIndex].stPara.stPerimeter;

				if  (
					(pstZone[s32ZoneIndex].u32Mode&IMP_FUNC_PERIMETER)
					&& (pstPeriPara->s32Mode == IMP_PMODE_INTRUSION )
					&& (ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray,s32ZoneIndex,0) == 1)
					)
				{
					if ( pstParams->s32PerimeterUseMtrend && IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) )
					{
					
						ipAnalysisBehaviorPerimeter( pstParams, pstTarget, s32ZoneIndex );
					}
					if ( !pstParams->s32PerimeterUseMtrend )
					{
						ipAnalysisBehaviorPerimeter( pstParams, pstTarget, s32ZoneIndex );
					}
				}
				
				if  (
					(pstZone[s32ZoneIndex].u32Mode&IMP_FUNC_PERIMETER)
					&& (pstPeriPara->s32Mode == IMP_PMODE_ENTER )
					&&  (ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray,s32ZoneIndex,0) == 1)
					)
				{
					ipAnalysisBehaviorPerimeterEnter(pstParams, pstTarget, s32ZoneIndex);
				}

				if  (
					(pstZone[s32ZoneIndex].u32Mode&IMP_FUNC_PERIMETER)
					&& (pstPeriPara->s32Mode == IMP_PMODE_EXIT )
				//	&&  (ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray,s32ZoneIndex,0) == 0)///此处有修改
					)
				{

					ipAnalysisBehaviorPerimeterExit(pstParams, pstTarget, s32ZoneIndex);
				}


#endif

				if(	(ipZoneObjectEffective(pstTarget, pstImage, s32ZoneIndex,0,0,0)) == 0 )
					continue;

			}
		}
	}

#ifdef SUPPORT_PERIMETER
	for( s32ZoneIndex = 0; s32ZoneIndex < IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
	{
		pstPeriPara = &pstParams->pstRule->stZones.astZone[s32ZoneIndex].stPara.stPerimeter;
		if  (
			(pstZone[s32ZoneIndex].u32Mode&IMP_FUNC_PERIMETER)
//			&& (pstPeriPara->s32Mode == PMODE_DISAPPEAR )
			&&  pstZone[s32ZoneIndex].u32Valid != 0
			&&	pstZone[s32ZoneIndex].u32Enable != 0
			)
		{
			s32Disappear = 1;
			ipDeterminDisappearStatus(pstAnalyst,s32ZoneIndex);
		}
	}
	if(s32Disappear)
	{
		ipRemoveUnusedRecord( pstAnalyst );
		ipOutputDisppearEvent( pstAnalyst );
		ipOutputDisppearTarget( pstAnalyst );
	}
#endif

	return 1;
}

IMP_S32 ipPostProcessPerimeterAnalysti( IpPerimeterAnalyst *pstAnalyst )
{
	return 1;
}


static IMP_S32  ipPerimeterIsAccordantType( IpPerimeterPara *pstParams, RULE_PARA_PERIMETER_S *pstPeriPara,
                                            IpTrackedTarget *pstTarget );
static IMP_S32 ipPerimeterIsAccordantConstraint( IpPerimeterPara *pstParams, PERILMT_LMT_PARA_S *pstPeriLmtPara, IpTrackedTarget *pstTarget,IMP_S32 s32ZoneIndex );
static IMP_S32 ipPerimeterIsForbiddenDirection( IMP_S32 s32ForbiddenAngle, IMP_S32 s32Dir, IMP_S32 s32AngleTolerance );
static IMP_S32 ipAccumulateDistance(IpPerimeterPara *pstParams,IpTrackedTarget *pstTarget,IMP_S32 s32AnalystLen);
#if defined(DEBUG_ADVANCE_PARA)
static IMP_S32 ipPerimeterUserAdvanceParaConstraint(IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget)
{
    IMP_S32 s32Ret = 1;
	IMP_S32 s32BiDir;
	IMP_S32 s32Width, s32Height;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IpTargetPosition *pstPos;
	URP_ADVANCE_PARA_S  *pstURPPara;
	RULE_S *pstRule;
	USR_ADV_PARA_S *pstUsrAdvPara;
	pstRule = pstParams->pstRule;
	pstUsrAdvPara = (USR_ADV_PARA_S*)pstRule->u8AdvParaData;
    pstURPPara = &pstUsrAdvPara->stParaUrp;
#if defined(IMP_DEBUG_PRINT)
    printf("--------------------perimeter advance para judgement start -------------------!\n");
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
    printf("--------------------perimeter advance para judgement end -------------------!\n");
#endif
	return s32Ret;
}
#else
static IMP_S32 ipPerimeterUserAdvanceParaConstraint(IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget)
{
    IMP_S32 s32Ret = 1;
	IMP_S32 s32Width, s32Height;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
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
    return s32Ret;
}
#endif
static IMP_S32 ipAnalysisBehaviorPerimeter( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	RULE_ZONE_S *pstZone = &pstParams->pstRule->stZones.astZone[s32ZoneIndex];
	RULE_PARA_PERIMETER_S *pstPeriPara = &pstZone->stPara.stPerimeter;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );


// 	IMP_S32 s32Flag = 0;
 	IMP_S32 s32Ret = 0;




		if(	1
			&& ipPerimeterIsAccordantType( pstParams, pstPeriPara, pstTarget )
			&& ipPerimeterIsAccordantConstraint(pstParams,&pstPeriPara->stPara,pstTarget,s32ZoneIndex)//距离方向限制
			&& ipPerimeterUserAdvanceParaConstraint(pstParams,pstTarget)
			)
		{

			pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
			pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
			s32Ret = 1;
			pstTargetData->stPerimeterDirectionStatus=IMP_PRIMETER_DIRECTION_IN_SCOPE;

		}


	return s32Ret;
}

static IMP_S32 ipPerimeterIsAccordantConstraint( IpPerimeterPara *pstParams, PERILMT_LMT_PARA_S *pstPeriLmtPara, IpTrackedTarget *pstTarget,IMP_S32 s32ZoneIndex )
{
	IMP_S32 s32Ret = 0;
	IMP_S32 s32BiDir;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 s32Len, s32Dist, s32Life;
	IMP_S32 s32AngleTolerance = pstParams->s32AngleTolerance;
	IMP_S32 s32Dir;

	s32Len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
	if( NULL==pstTargetData->apstFirstPos[s32ZoneIndex] )
	{
		pstTargetData->apstFirstPos[s32ZoneIndex] = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -s32Len );
		pstTargetData->s32MaxDistance[s32ZoneIndex] = 0;
	}
	pstPos1 = pstTargetData->apstFirstPos[s32ZoneIndex];
	pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );

	s32Life = pstPos2->u32Time - pstPos1->u32Time;
	if( s32Life >= pstPeriLmtPara->s32TimeMin )
	{
		if( pstTargetData->s32MaxDistance[s32ZoneIndex] < pstPeriLmtPara->s32DistMin )
		{
			s32Dist=ipAccumulateDistance(pstParams,pstTarget,s32Len-1);
			{

				if( pstTargetData->s32MaxDistance[s32ZoneIndex] < s32Dist )
					pstTargetData->s32MaxDistance[s32ZoneIndex] = s32Dist;
			}
		}

		ipGetObjectDirectionByTrajectory(pstTarget,&s32Dir);
		if (pstPeriLmtPara->s32DirectionLmt)
		{
			if(pstTargetData->stPerimeterDirectionStatus!=IMP_PRIMETER_DIRECTION_IN_SCOPE)
			{
				s32BiDir=ipPerimeterIsForbiddenDirection(pstPeriLmtPara->s32ForbiddenDirection,s32Dir,s32AngleTolerance);
			}
			else
			{
				s32BiDir=1;
			}
		}
		else
		{
			s32BiDir=1;
		}
		s32Ret = ( pstTargetData->s32MaxDistance[s32ZoneIndex] >= pstPeriLmtPara->s32DistMin ) &&
																				s32BiDir;
	}

	return s32Ret;
}

static IMP_S32 ipPerimeterIsAccordantType( IpPerimeterPara *pstParams, RULE_PARA_PERIMETER_S *pstPeriPara,
                                           IpTrackedTarget *pstTarget )
{
	IMP_S32 s32Ret = 0;
	IMP_U32 u32Type = pstTarget->stTargetInfo.u32Type;

	if( pstParams->s32PerimeterUseMtrend && !IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) )
		u32Type = IMP_TGT_TYPE_UNKNOWN;
	if( pstPeriPara->s32TypeConstrain )
	{

		if( u32Type == IMP_TGT_TYPE_HUMAN && pstPeriPara->s32TypeHuman )
			s32Ret = IMP_TGT_TYPE_HUMAN;
		else if( u32Type == IMP_TGT_TYPE_VEHICLE && pstPeriPara->s32TypeVehicle )
			s32Ret = IMP_TGT_TYPE_VEHICLE;
	}
	else
	{
		s32Ret = IMP_TGT_TYPE_UNKNOWN;
	}
	return s32Ret;
}

static IMP_S32 ipPerimeterIsForbiddenDirection( IMP_S32 s32ForbiddenAngle, IMP_S32 s32Dir, IMP_S32 s32AngleTolerance )
{
	return ipIsAcoordantAngleRange( s32ForbiddenAngle, s32Dir, s32AngleTolerance );
}

static IMP_S32 ipAccumulateDistance(IpPerimeterPara *pstParams,IpTrackedTarget *pstTarget,IMP_S32 s32AnalystLen)
{
	IpTargetPosition *pstCurPos, *pstPrePos = NULL;
	IMP_S32 i, X, Y;
	IMP_S32 s32AccumDist = 0;
	IMP_S32 s32MaxDistance = -1;


	pstPrePos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-s32AnalystLen);
	for (i = 0; i < s32AnalystLen; i++)
	{
		pstCurPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		X = (pstCurPos->stPt.s16X-pstPrePos->stPt.s16X);
		Y = (pstCurPos->stPt.s16Y-pstPrePos->stPt.s16Y);

		s32AccumDist = (IMP_S32)sqrt(X * X + Y * Y);

		if (s32AccumDist > s32MaxDistance)
		{
			s32MaxDistance = s32AccumDist;
		}

	}

	return s32MaxDistance;
}
static IMP_S32 ipIsEnter(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams);
static IMP_S32 ipAnalysisBehaviorPerimeterEnter( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	RULE_ZONE_S *pstZone = &pstParams->pstRule->stZones.astZone[s32ZoneIndex];
	RULE_PARA_PERIMETER_S *pstPeriPara = &pstZone->stPara.stPerimeter;
	GRAY_IMAGE_S *pstImage = &pstParams->pstRule->stZones.stImage;
	IMP_S32 s32Flag = 0;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IMP_S32 s32Ret;

	if (pstTargetData->s32EnterFlag[s32ZoneIndex] == IMP_TRUE)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
		return 1;
	}

	s32Ret = ipIsEnter(pstTarget,pstImage,s32ZoneIndex,pstParams);
	if(	1
		&& ipPerimeterIsAccordantType( pstParams, pstPeriPara, pstTarget )
		&& s32Ret
		)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
		pstTargetData->s32EnterFlag[s32ZoneIndex] = IMP_TRUE;
		return 1;
	}
	return 0;
}
static IMP_S32 ipIsAppear(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams);
static IMP_S32 ipAnalysisBehaviorPerimeterAppear( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	RULE_ZONE_S *pstZone = &pstParams->pstRule->stZones.astZone[s32ZoneIndex];
	RULE_PARA_PERIMETER_S *pstPeriPara = &pstZone->stPara.stPerimeter;
	GRAY_IMAGE_S *pstImage = &pstParams->pstRule->stZones.stImage;
	IMP_S32 s32Flag = 0;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IMP_S32 s32Ret = 0;
	
	if (pstTargetData->s32AppearFlag[s32ZoneIndex] == IMP_TRUE)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
		return 1;
	}
	
	if (pstTargetData->s32AppearFlag[s32ZoneIndex] == IMP_NOT_APPEAR)
	{
		return 0;
	}
	
	s32Ret = ipIsAppear(pstTarget,pstImage,s32ZoneIndex,pstParams);
	
	if ( !s32Ret )
	{
		pstTargetData->s32AppearFlag[s32ZoneIndex] = IMP_NOT_APPEAR;
		return 0;
	}
	
	if(	1
		&& s32Flag
		&& s32Ret
		)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
		pstTargetData->s32AppearFlag[s32ZoneIndex] = IMP_TRUE;
		return 1;
	}
	
	return 0;
}
static IMP_S32 ipIsAppear(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams)
{
	IpTargetPosition  *pstPos;
	IMP_S32 s32PathLen;
	IMP_POINT_S stPoint;
	IMP_U8 u8PixValue;
	IMP_S32 s32Width = pstImage->s32W;
	IMP_S32 s32Temp,s32Ret;
	IMP_S32 s32PointNum = 0;
	s32PathLen = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-s32PathLen+1);
	if (pstParams->s32PerimeterUseBottom)
	{
		if (pstTarget->stTargetInfo.u32Type == IMP_TGT_TYPE_VEHICLE)
		{
			s32PointNum = ipComputePointsInZone(pstTarget,pstPos,pstImage,s32ZoneIndex);
			s32Ret = (s32PointNum==8) ? 1:0;
		}
		else
		{
			stPoint.s16X = pstPos->stRg.s16X1;
			stPoint.s16Y = pstPos->stRg.s16Y2;
			u8PixValue = pstImage->pu8Data[stPoint.s16Y * s32Width + stPoint.s16X];
			s32Temp = u8PixValue>>s32ZoneIndex & 0x01;
			if (!s32Temp)
				return 0;
			stPoint.s16X = pstPos->stRg.s16X2;
			stPoint.s16Y = pstPos->stRg.s16Y2;
			u8PixValue = pstImage->pu8Data[stPoint.s16Y * s32Width + stPoint.s16X];
			s32Ret = u8PixValue>>s32ZoneIndex & 0x01;
		}
	}
	else
	{
		stPoint.s16X = pstPos->stPt.s16X;	stPoint.s16Y = pstPos->stPt.s16Y;
		u8PixValue = pstImage->pu8Data[stPoint.s16Y * s32Width + stPoint.s16X];
		s32Ret = u8PixValue>>s32ZoneIndex & 0x01;
	}
	return s32Ret;
}

static IMP_S32 ipIsEnter(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams)
{
	IMP_S32 s32PathLen;
	IMP_S32 s32AnalyLen = 0;
	IMP_S32 i = 0;
	IMP_S32 s32Temp = -1;
	IMP_S32 s32Num = 0;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData(pstTarget->stPrivateData.pDataAnalyst);

	s32PathLen = ipByteCArrayGetLength(&pstTargetData->stPerimeterCycleArray);
	if (s32PathLen == 0)
	{
		return 0;
	}

	if (s32PathLen < pstParams->s32PerimeterAnalysisLength)
		s32AnalyLen = s32PathLen;
	else
		s32AnalyLen = pstParams->s32PerimeterAnalysisLength;

	for (i = s32AnalyLen; i > 0; i--)
	{
		s32Temp = ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray, s32ZoneIndex, -i + 1);
		s32Num += (s32Temp == 1) ? 0:1;
	}
	return (s32Num >= (s32AnalyLen * pstParams->s32PerimeterValidRatio / 100) && s32Num != 0);
}
static IMP_S32 ipIsExit(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams);
static IMP_S32 ipAnalysisBehaviorPerimeterExit( IpPerimeterPara *pstParams, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	RULE_ZONE_S *pstZone = &pstParams->pstRule->stZones.astZone[s32ZoneIndex];
	RULE_PARA_PERIMETER_S *pstPeriPara = &pstZone->stPara.stPerimeter;
	GRAY_IMAGE_S *pstImage = &pstParams->pstRule->stZones.stImage;
	IMP_S32 s32Flag = 0;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IMP_S32 s32Ret = 0;

	if (pstTargetData->s32ExitFlag[s32ZoneIndex] == IMP_TRUE)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
		return 1;
	}


	s32Ret = ipIsExit(pstTarget,pstImage,s32ZoneIndex,pstParams);
	if(	1
		&& (ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray,s32ZoneIndex,0) == 0)
		&& ipPerimeterIsAccordantType( pstParams, pstPeriPara, pstTarget )
		&& s32Ret
		)
	{
		pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_PERIMETER;
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
		pstTargetData->s32ExitFlag[s32ZoneIndex] = IMP_TRUE;
		return 1;
	}
	return 0;
}

static IMP_S32 ipIsExit(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams)
{
	IMP_S32 s32PathLen;
	IMP_S32 s32AnalyLen = 0;
	IMP_S32 i = 0;
	IMP_S32 s32Temp = -1;
	IMP_S32 s32Num = 0;

	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData(pstTarget->stPrivateData.pDataAnalyst);
	s32PathLen = ipByteCArrayGetLength(&pstTargetData->stPerimeterCycleArray);
	if (s32PathLen == 0)
	{
		return 0;
	}
	if (s32PathLen<pstParams->s32PerimeterAnalysisLength)
		s32AnalyLen = s32PathLen;
	else
		s32AnalyLen = pstParams->s32PerimeterAnalysisLength;


	for (i=s32AnalyLen; i>0; i--)
	{
		s32Temp = ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray,s32ZoneIndex,-i + 1);
		s32Num += (s32Temp == 1) ? 1:0;
	}
	return (s32Num >= (s32AnalyLen * pstParams->s32PerimeterValidRatio / 100) && s32Num != 0);
}
static IMP_VOID ipPerimeterUpdataInfo(IpPerimeterAnalyst *pstAnalyst,IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams);
static IMP_VOID ipAnalysisBehaviorPerimeterDisappear( IpPerimeterAnalyst *pstAnalyst, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	IpPerimeterPara *pstParams = pstAnalyst->pstParams;
	GRAY_IMAGE_S *pstImage = &pstParams->pstRule->stZones.stImage;

	ipPerimeterUpdataInfo(pstAnalyst,pstTarget,pstImage,s32ZoneIndex,pstParams);
}
static IMP_S32 ipIsRecorded(IpTrackedTarget *pstTarget,IpPerimeterAnalyst *pstAnalyst);
static IMP_VOID ipUpdateRecordInfo(IpTrackedTarget *pstTarget,IpRecordInfo *pstRecord,IMP_S32 s32ZoneFlag,IMP_S32 s32ZoneIndex);
static IMP_S32 ipIsInTargetSet(IpPerimeterAnalyst *pstAnalyst,IpRecordInfo *pstRecord);
static IMP_S32 ipIsInZone(IpRecordInfo *pstRecord,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams);
static IMP_VOID ipPerimeterUpdataInfo(IpPerimeterAnalyst *pstAnalyst,IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams)
{
	IMP_S32 s32Flag, s32Tmp;
	IpStaticList *pstList = &pstAnalyst->stStaticList;
	IpRecordInfo *pstRecord = pstAnalyst->astRecordInfo;
	IMP_S32 s32Index;
	IpPerimeterTargetData *pstTargetData = ipGetPerimeterTargetData(pstTarget->stPrivateData.pDataAnalyst);

	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);


// 	s32Flag = ipZoneObjectEffective( pstTarget, pstImage, s32ZoneIndex,pstAnalyst->pstParams->s32PerimeterUseBottom,0,pstAnalyst->pstParams->s32PerimeterDisppearRatio );
	s32Flag = ipIsObjectInZone(&pstTargetData->stPerimeterCycleArray,s32ZoneIndex,0);
	s32Tmp = ipIsRecorded(pstTarget,pstAnalyst);
	if (s32Tmp != -1)
	{
		ipUpdateRecordInfo(pstTarget,pstRecord+s32Tmp,s32Flag,s32ZoneIndex);
	}
	if (s32Flag && -1 == s32Tmp)
	{
		s32Index = ipGetNextFreeNode(pstList);
		if (s32Index == -1)
			return;
		ipUpdateRecordInfo(pstTarget,pstRecord + s32Index,s32Flag,s32ZoneIndex);
		ipAddUsedNode(pstList,s32Index);
	}

}

static IMP_S32 ipIsRecorded(IpTrackedTarget *pstTarget,IpPerimeterAnalyst *pstAnalyst)
{
	IpStaticList *pstList = &pstAnalyst->stStaticList;
	IpRecordInfo *pstRecord = pstAnalyst->astRecordInfo;
	IMP_S32 s32UsedNum, i, s32Index;
	s32UsedNum = ipGetUsedNum(pstList);
	s32Index = -1;
	for (i = 0; i < s32UsedNum; i++)
	{
		s32Index = ipGetNextUsedNode(pstList,s32Index);
		if (s32Index == -1)
		{
			return -1;
		}
		if (pstRecord[s32Index].u32TargetId == pstTarget->u32TargetId)
		{
			return s32Index;
		}
	}
	return -1;
}
static IMP_VOID ipUpdateRecordInfo( IpTrackedTarget *pstTarget,IpRecordInfo *pstRecord,IMP_S32 s32ZoneFlag,IMP_S32 s32ZoneIndex )
{
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);

	pstRecord->stCentroid.s16X = pstPos->stPt.s16X;
	pstRecord->stCentroid.s16Y = pstPos->stPt.s16Y;

	pstRecord->u32TargetId = pstTarget->u32TargetId;

	pstRecord->stRect.s16X1 = pstPos->stRg.s16X1;
	pstRecord->stRect.s16Y1 = pstPos->stRg.s16Y1;
	pstRecord->stRect.s16X2 = pstPos->stRg.s16X2;
	pstRecord->stRect.s16Y2 = pstPos->stRg.s16Y2;
	pstRecord->s32TargetType = pstTarget->stTargetInfo.u32Type;
	pstRecord->aZoneFlag[s32ZoneIndex] = s32ZoneFlag;
}

static IMP_S32 ipIsInTargetSet(IpPerimeterAnalyst *pstAnalyst,IpRecordInfo *pstRecord)
{
	IMP_S32 i;
	IpTrackedTargetSet *pstTargetSet = &pstAnalyst->pstResult->stTrackedTargetSet;
	IpTrackedTarget *astTargets = pstTargetSet->astTargets;
	for (i = 0; i < IMP_MAX_TGT_CNT; i++)
	{
		if (pstRecord->u32TargetId == astTargets[i].u32TargetId && ipTrackedTargetIsValid( astTargets + i ))
		{
			return 1;
		}
	}
	return 0;
}

static IMP_S32 ipIsInZone(IpRecordInfo *pstRecord,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex,IpPerimeterPara *pstParams)
{
	IMP_S32 s32Ret, s32Temp;
	IMP_U8 u8PixValue;
	IMP_POINT_S stPoint;
	IMP_S32 s32Width = pstImage->s32W;
	if (pstParams->s32PerimeterUseBottom)
	{
		stPoint.s16X = pstRecord->stRect.s16X1;
		stPoint.s16Y = pstRecord->stRect.s16Y2;
		u8PixValue = pstImage->pu8Data[stPoint.s16Y * s32Width + stPoint.s16X];
		s32Temp = u8PixValue >> s32ZoneIndex & 0x01;
		if (!s32Temp)
			return 0;
		stPoint.s16X = pstRecord->stRect.s16X2;
		stPoint.s16Y = pstRecord->stRect.s16Y2;
		u8PixValue = pstImage->pu8Data[stPoint.s16Y * s32Width + stPoint.s16X];
		s32Ret = u8PixValue >> s32ZoneIndex & 0x01;
	}
	else
	{
		stPoint.s16X = pstRecord->stCentroid.s16X;	stPoint.s16Y = pstRecord->stCentroid.s16Y;
		u8PixValue = pstImage->pu8Data[stPoint.s16Y*s32Width + stPoint.s16X];
		s32Ret = u8PixValue >> s32ZoneIndex & 0x01;
	}
	return s32Ret;
}




static IMP_S32 ipInitialStaticList(IpStaticList *pstList,IMP_S32 s32NodeNum)
{
	IMP_S32 i;
	IpNode *pstNode;
	if (s32NodeNum > (IMP_MAX_NODE - 2))
	{
		return 0;
	}
	pstList->s32NumTotal = s32NodeNum;
	pstList->s32NumUsed = 0;
	pstList->s32NumFree = s32NodeNum;
	pstList->pstFirstFreeNode = &pstList->astNode[0];
	pstList->pstFirstUsedNode = &pstList->astNode[1];

	pstList->pstFirstUsedNode->s32Id = -1;
	pstList->pstFirstUsedNode->s32Next = -1;
	pstList->pstFirstUsedNode->s32Up = -2;

	pstList->pstFirstFreeNode->s32Id = -1;
	pstList->pstFirstFreeNode->s32Next = -1;
	pstList->pstFirstFreeNode->s32Up = -2;

	for (i = 2; i < IMP_MAX_NODE; i++)
	{
		pstList->astNode[i].s32Id = i - 2;
		pstList->astNode[i].s32Next = -1;
		pstList->astNode[i].s32Up = -2;
	}

	pstNode = pstList->pstFirstFreeNode;
	for (i = 2; i < s32NodeNum + 2; i++)
	{
		pstNode->s32Next = i;
		pstNode = &pstList->astNode[i];
	}
	return 1;
}
static IMP_VOID ipRemoveFreeNode(IpStaticList *pstList)
{
	IMP_S32 s32NodeIndex;
	IMP_S32 s32Next;
	if (pstList->s32NumFree <= 0)
	{
		return;
	}
	s32NodeIndex = pstList->pstFirstFreeNode->s32Next;
	if(s32NodeIndex != -1)
	{
		s32Next = pstList->astNode[s32NodeIndex].s32Next;
		pstList->pstFirstFreeNode->s32Next = s32Next;
		pstList->astNode[s32NodeIndex].s32Next = -1;
		pstList->s32NumFree--;
	}
}
static IMP_S32 ipGetNextFreeNode(IpStaticList *pstList)
{
	IMP_S32 s32NodeIndex;
	IMP_S32 s32Next;

	if (pstList->s32NumFree <= 0)
	{
		return -1;
	}
	s32NodeIndex = pstList->pstFirstFreeNode->s32Next;
	if(s32NodeIndex != -1)
	{
		s32Next = pstList->astNode[s32NodeIndex].s32Next;
		pstList->pstFirstFreeNode->s32Next = s32Next;
		pstList->astNode[s32NodeIndex].s32Next = -1;
		pstList->astNode[s32NodeIndex].s32Up = -2;
		pstList->s32NumFree--;
		return pstList->astNode[s32NodeIndex].s32Id;
	}
	else
		return -1;
}

static IMP_S32 ipAddUsedNode(IpStaticList *pstList,IMP_S32 s32NodeId)
{
	IMP_S32 s32NodeIndex;
	IMP_S32 s32Next;
	if (pstList->s32NumUsed >= pstList->s32NumTotal)
	{
		return 0;
	}

	s32NodeIndex = s32NodeId + 2;
	pstList->astNode[s32NodeIndex].s32Up = -2;
	pstList->astNode[s32NodeIndex].s32Next = -1;


	s32Next = pstList->pstFirstUsedNode->s32Next;//niceify firstNode and save s32Next astNode index
	pstList->pstFirstUsedNode->s32Next = s32NodeIndex;

	pstList->astNode[s32NodeIndex].s32Up = -1;//niceify the astNode which index is s32NodeIndex
	pstList->astNode[s32NodeIndex].s32Next = s32Next;

	if(s32Next != -1)
		pstList->astNode[s32Next].s32Up = s32NodeIndex; //niceify the s32Next astNode
	pstList->s32NumUsed++;
	return 1;
}
static IMP_S32 ipGetFirstUsedNode(IpStaticList *pstList)
{
	IMP_S32 s32Next;
	if (pstList->s32NumUsed <= 0)
	{
		return -1;
	}
	s32Next = pstList->pstFirstUsedNode->s32Next;
	return pstList->astNode[s32Next].s32Id;
}
static IMP_S32 ipGetNextUsedNode(IpStaticList *pstList,IMP_S32 s32NodeId)
{
	IMP_S32 s32NodeIndex, s32NextIndex, s32NodeIdRet;
 	IMP_S32 s32Next;

	if ( pstList->s32NumUsed <= 0)
	{
		return -1;
	}
	if (s32NodeId == -1)
	{
		s32Next = pstList->pstFirstUsedNode->s32Next;
		return pstList->astNode[s32Next].s32Id;
	}

	s32NodeIndex = s32NodeId + 2;
	s32NextIndex = pstList->astNode[s32NodeIndex].s32Next;
	if (s32NextIndex != -1)
	{
		s32NodeIdRet = pstList->astNode[s32NextIndex].s32Id;
	}
	else
		s32NodeIdRet = -1;
	return s32NodeIdRet;
}
static IMP_S32 ipRemoveUsedNode(IpStaticList *pstList,IMP_S32 s32NodeId)
{
	IMP_S32 s32NodeIndex;
	IMP_S32 s32Next, s32Up;
	IpNode *pstNode;
	if (pstList->s32NumUsed <= 0)
	{
		return 0;
	}
	s32NodeIndex = s32NodeId + 2;
	pstNode = &pstList->astNode[s32NodeIndex];

	s32Next = pstNode->s32Next;
	s32Up = pstNode->s32Up;
	if (s32Next == -1)
	{
		if (s32Up == -1)
		{
			pstList->pstFirstUsedNode->s32Next = -1;
		}
		else
		{
			pstList->astNode[s32Up].s32Next = -1;
		}
	}
	else
	{
		if( s32Up == -1 )
			pstList->pstFirstUsedNode->s32Next = s32Next;
		else
			pstList->astNode[s32Up].s32Next = s32Next;

		pstList->astNode[s32Next].s32Up = s32Up;
	}

	pstList->s32NumUsed--;
	pstNode->s32Next = -1;
	pstNode->s32Up = -2;
	return 1;
}

static IMP_S32 ipAddFreeNode(IpStaticList *pstList,IMP_S32 s32NodeId)
{
	IMP_S32 s32NodeIndex;
	IMP_S32 s32Next;
	if (pstList->s32NumFree >= pstList->s32NumTotal)
	{
		return 0;
	}

	s32NodeIndex = s32NodeId + 2;
	s32Next = pstList->pstFirstFreeNode->s32Next;
	pstList->pstFirstFreeNode->s32Next = s32NodeIndex;
	pstList->astNode[s32NodeIndex].s32Next = s32Next;
	pstList->astNode[s32NodeIndex].s32Up = -2;
	pstList->s32NumFree++;
	return 1;
}
static IMP_S32 ipGetFreeNum(IpStaticList *pstList)
{
	return pstList->s32NumFree;
}
static IMP_S32 ipGetUsedNum(IpStaticList *pstList)
{
	return pstList->s32NumUsed;
}
static IMP_VOID ipDeterminDisappearStatus( IpPerimeterAnalyst *pstAnalyst,IMP_S32 s32ZoneIndex )
{
	IpRecordInfo *pstRecord = pstAnalyst->astRecordInfo;
	IpStaticList *pstList = &pstAnalyst->stStaticList;
	IMP_S32 s32Index;
	IMP_S32 i,s32UsedNum;
	IMP_S32 s32Tmp = 0;
	IMP_S32 s32TimeCur = pstAnalyst->pstResult->u32FrmTimeCur;
	IMP_S32 s32Flag;

	s32UsedNum = ipGetUsedNum(pstList);
	s32Index = -1;
	for (i = 0; i < s32UsedNum; i++)
	{
		s32Index = ipGetNextUsedNode(pstList,s32Index);
		if (s32Index == -1)
			return ;
		if (pstRecord[s32Index].s32Status)
			continue;

		s32Tmp = ipIsInTargetSet(pstAnalyst,pstRecord+s32Index);
		s32Flag = pstRecord[s32Index].aZoneFlag[s32ZoneIndex];//ipIsInZone(pstRecord+s32Index,pstImage,s32ZoneIndex,pstAnalyst->pstParams);
		if (!s32Tmp && s32Flag)
		{

			if(pstRecord[s32Index].s32Status == 0)
			{
				pstRecord[s32Index].s32DisappearTime = s32TimeCur;
				pstRecord[s32Index].s32Status = 1;
			}
		}
	}
}
static IMP_VOID ipRemoveUnusedRecord( IpPerimeterAnalyst *pstAnalyst )
{
	IpRecordInfo *pstRecord = pstAnalyst->astRecordInfo;
	IpStaticList *pstList = &pstAnalyst->stStaticList;
	IMP_S32 s32Index;
	IMP_S32 i,s32UsedNum;
	IMP_S32 s32Tmp = 0;

	s32UsedNum = ipGetUsedNum(pstList);
	s32Index = -1;
	for (i=0; i<s32UsedNum; i++)
	{
		s32Index = ipGetNextUsedNode(pstList,s32Index);
		if (s32Index==-1)
			return;
		if (pstRecord[s32Index].s32Status)
			continue;

		s32Tmp = ipIsInTargetSet(pstAnalyst,pstRecord+s32Index);
		if (!s32Tmp && !pstRecord[s32Index].s32Status)
		{

			memset(pstRecord+s32Index,0,sizeof(IpRecordInfo));
			ipRemoveUsedNode(pstList,s32Index);
			ipAddFreeNode(pstList,s32Index);
			s32UsedNum = ipGetUsedNum(pstList);
		}
	}
}

static IMP_S32 ipCheckTargetType(RULE_PARA_PERIMETER_S *pstPeriPara,IMP_S32 s32TargetType);
static IMP_S32 ipGetZoneIndex(IpRecordInfo *pstRecord,IMP_S32 s32Index);
static IMP_VOID ipOutputDisppearEvent( IpPerimeterAnalyst *pstAnalyst )
{
	PEA_RESULT_S *pstResult = pstAnalyst->pstResult;
	IMP_OUTPUT *pstOutput = &pstResult->stOutput;
	EVT_MGR_S *pstEvtMgr = &pstResult->stEvtMgr;
	EVT_ITEM_S *pstEvtItem;
	EVT_DATA_PERIMETER_S *pstData;

	IpRecordInfo *pstRecord = pstAnalyst->astRecordInfo;
	IpStaticList *pstList = &pstAnalyst->stStaticList;
	IMP_S32 i;
	IMP_S32 s32TimeCur = pstAnalyst->pstResult->u32FrmTimeCur;
	IMP_S32 u32Time;
	IMP_S32 s32UsedNum,s32Index;

	IpPerimeterPara *pstParams = pstAnalyst->pstParams;
	RULE_ZONES_S *zones = &pstAnalyst->pstParams->pstRule->stZones;
	RULE_ZONE_S *pstZone;
	RULE_PARA_PERIMETER_S *pstPeriPara ;
	IMP_S32 s32TypeFlag=IMP_TGT_TYPE_UNKNOWN;
	IMP_S32 s32ZoneIndex;

	s32UsedNum = ipGetUsedNum(pstList);
	s32Index = -1;
	for (i=0; i<s32UsedNum; i++)
	{
		s32Index = ipGetNextUsedNode(pstList,s32Index);
		if (s32Index==-1)
			return;
		if (pstRecord[s32Index].s32Status)
		{
			s32ZoneIndex = ipGetZoneIndex(pstRecord,s32Index);
			if (s32ZoneIndex==-1)
				continue;
			pstZone = zones->astZone+s32ZoneIndex;
			pstPeriPara = &pstZone->stPara.stPerimeter;
		}
		else
			continue;

		u32Time = s32TimeCur - pstRecord[s32Index].s32DisappearTime;
		s32TypeFlag = ipCheckTargetType(pstPeriPara,pstRecord[s32Index].s32TargetType);
		if (pstRecord[s32Index].s32Status && pstRecord[s32Index].u32EndId==0 && s32TypeFlag )
		{
			pstEvtItem = IMP_EVT_SET_AddEventNoID( pstEvtMgr, &pstOutput->stEventSet, IMP_EVT_TYPE_AlarmPerimeter,&pstRecord[s32Index].u32EndId, pstZone->u32Level, IMP_EVT_STATUS_START,s32ZoneIndex );
			pstData = (EVT_DATA_PERIMETER_S *)pstEvtItem->au8Data;
			memcpy(&pstData->stRect,&pstRecord[s32Index].stRect,sizeof(IMP_RECT_S));
		//	pstData->rule.mode = PMODE_DISAPPEAR;
		}
		else if (pstRecord[s32Index].s32Status==1 && pstRecord[s32Index].u32EndId && u32Time > pstParams->s32PerimeterDisppearAlarmTime)
		{
			IMP_EVT_SET_AddEvent( &pstOutput->stEventSet, IMP_EVT_TYPE_AlarmPerimeter, pstRecord[s32Index].u32EndId,pstZone->u32Level, IMP_EVT_STATUS_END,s32ZoneIndex );

			memset(pstRecord+s32Index,0,sizeof(IpRecordInfo));
			ipRemoveUsedNode(pstList,s32Index);
			ipAddFreeNode(pstList,s32Index);
		}
		else if ( pstRecord[s32Index].s32Status==1 && pstRecord[s32Index].u32EndId==0 )
		{
			memset(pstRecord+s32Index,0,sizeof(IpRecordInfo));
			ipRemoveUsedNode(pstList,s32Index);
			ipAddFreeNode(pstList,s32Index);
		}
	}
}

static IMP_S32 ipGetZoneIndex(IpRecordInfo *pstRecord,IMP_S32 s32Index)
{
	IMP_S32 i = 0;
	for (i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++)
	{
		if (pstRecord[s32Index].aZoneFlag[i])
		{
			return i;
		}
	}
	return -1;
}
static IMP_S32 ipCheckTargetType(RULE_PARA_PERIMETER_S *pstPeriPara,IMP_S32 s32TargetType)
{
	IMP_S32 s32Ret = 0;
//	if (pstPeriPara->perimeter_type_constrain)
//	{
//		if (pstPeriPara->perimeter_type_human && s32TargetType == IMP_TGT_TYPE_HUMAN)
//		{
//			s32Ret = 1;
//		}
//		if (pstPeriPara->perimeter_type_vehicle && s32TargetType == IMP_TGT_TYPE_VEHICLE)
//		{
//			s32Ret = 1;
//		}
//	}
//	else
		s32Ret = 1;
	return s32Ret;
}
static IMP_VOID ipFillTargets(TGT_SET_S *pstTarget,IMP_U32 u32TargetId,IMP_S32 s32TargetType,IMP_U32 u32EventType,IMP_RECT_S *pstRect,IMP_POINT_S *pstCentroid);
static IMP_VOID ipOutputDisppearTarget( IpPerimeterAnalyst *pstAnalyst )
{
	PEA_RESULT_S *pstResult = pstAnalyst->pstResult;
	IMP_OUTPUT *pstOutput = &pstResult->stOutput;

	TGT_SET_S *pstTargetSet = &pstOutput->stTargetSet;
	IpRecordInfo *pstRecord = pstAnalyst->astRecordInfo;
	IpStaticList *pstList = &pstAnalyst->stStaticList;
	IMP_S32 s32UsedNum,s32Index,i;

	s32UsedNum = ipGetUsedNum(pstList);
	s32Index = -1;
	for (i = 0; i < s32UsedNum; i++)
	{
		s32Index = ipGetNextUsedNode(pstList,s32Index);
		if (s32Index == -1)
			return;
		if (pstRecord[s32Index].s32Status)
		{
			ipFillTargets(pstTargetSet,pstRecord[s32Index].u32TargetId,pstRecord[s32Index].s32TargetType,IMP_EVT_TYPE_AlarmPerimeter ,&pstRecord[s32Index].stRect,&pstRecord[s32Index].stCentroid);
		}
	}

}
static IMP_VOID ipFillTargets(TGT_SET_S *pstTargetSet,IMP_U32 u32TargetId,IMP_S32 s32TargetType,IMP_U32 u32EventType,IMP_RECT_S *pstRect,IMP_POINT_S *pstCentroid)
{
	TGT_ITEM_S *pstTarget = pstTargetSet->astTargets;
	IMP_S32 k;
	k = pstTargetSet->s32TargetNum;
	if( k < IMP_MAX_TGT_CNT - 1 )
	{
		pstTarget[k].u32Id = u32TargetId;
		pstTarget[k].u32Type = s32TargetType;
		pstTarget[k].u32Event = u32EventType;
		memcpy(&pstTarget[k].stRect,pstRect,sizeof(IMP_RECT_S));
		memcpy(&pstTarget[k].stPoint,pstCentroid,sizeof(IMP_POINT_S));
		{
			TGT_MOTION_ITEM_S *pstItemData = (TGT_MOTION_ITEM_S*)(pstTarget[k].au8Data);
			pstItemData->s32Velocity = 0;
			pstItemData->s32Direction = 0;
			pstItemData->stTraject.s32Length = 0;
		}
		k++;
	}
	pstTargetSet->s32TargetNum = k;
}


IMP_VOID ipFillTargetEventDataPerimeter( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{
	IMP_S32 s32ZoneIndex = pstEvent->u32Zone;
	RULE_ZONE_S *pstZone = &pstRule->stZones.astZone[s32ZoneIndex];
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
	EVT_DATA_PERIMETER_S *pstData = (EVT_DATA_PERIMETER_S *)pstEvent->au8Data;

	memcpy(&pstData->stRect,&pstPos->stRg,sizeof(IMP_RECT_S));
	pstData->stRule.u32Mode = pstZone->stPara.stPerimeter.s32Mode;
}


IMP_VOID ipFillTargetEventDataOSC( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{
	IMP_S32 s32ZoneIndex = pstEvent->u32Zone;
	RULE_ZONE_S *pstZone = &pstRule->stZones.astZone[s32ZoneIndex];
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,0);
	EVT_DATA_TARGET_S *pstData = (EVT_DATA_TARGET_S*)pstEvent->au8Data;

	memcpy(&pstData->stRect,&pstPos->stRg,sizeof(IMP_RECT_S));
}

#endif

