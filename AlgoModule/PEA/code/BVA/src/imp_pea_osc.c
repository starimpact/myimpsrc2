
#include "imp_pea_osc.h"
//#include "imp_pea_system.h"
//////////////////////////////////////////////////////////////////////////
//OSC分析
IMP_S32 lx[2];
IMP_S32 ly[2];
IMP_S32 rx[2];
IMP_S32 ry[2];


static IMP_VOID ipClearOscAnalysti( IpOscAnalyst *pstAnalyst )
{
	//	IMP_GrayImageClear( &pstAnalyst->stImgBgEdge );
	IMP_GrayImageClear( &pstAnalyst->stImgBgInDiffMag );

	IMP_GrayImageClear( &pstAnalyst->stImgBgInDiff );
	IMP_GrayImageClear( &pstAnalyst->stImgLeftLife );
	IMP_GrayImageClear( &pstAnalyst->stImgRemovedLife );
	IMP_GrayImageClear( &pstAnalyst->stImgLeft );
	IMP_GrayImageClear( &pstAnalyst->stImgRemoved );
	IMP_GrayImageClear( &pstAnalyst->stImgLeftMorph );
	IMP_GrayImageClear( &pstAnalyst->stImgRemovedMorph );
	IMP_GrayImageClear( &pstAnalyst->stImgMediate );
	IMP_GrayImageClear( &pstAnalyst->stImgStatistic );
}

IMP_VOID ipCreateOscAnalysti( IpOscAnalyst *pstAnalyst, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	IMP_S32 s32ImgH,s32ImgW;
	MEM_MGR_ARRAY_S *pstMemMgr;
	memset( pstAnalyst, 0, sizeof(IpOscAnalyst) );
	pstAnalyst->pstResult = pstResult;
	pstAnalyst->pstHwResource = pstHwResource;
	pstMemMgr = &pstAnalyst->pstHwResource->stMemMgr;

	s32ImgH = pstResult->s32Height;
	s32ImgW = pstResult->s32Width;

	IMP_GrayImageCreate( &pstAnalyst->stImgBgEdge, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgBgInDiffMag, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgBgInDiff, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgLeftLife, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgRemovedLife, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgLeft, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgRemoved, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgLeftMorph, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgRemovedMorph, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgMediate, s32ImgW, s32ImgH, pstMemMgr );
	IMP_GrayImageCreate( &pstAnalyst->stImgStatistic, s32ImgW, s32ImgH, pstMemMgr );

	ipClearOscAnalysti( pstAnalyst );
}

IMP_VOID ipReleaseOscAnalysti( IpOscAnalyst *pstAnalyst )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &pstAnalyst->pstHwResource->stMemMgr;

	IMP_GrayImageDestroy( &pstAnalyst->stImgBgEdge, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgBgInDiffMag, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgBgInDiff, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgLeftLife, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgRemovedLife, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgLeft, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgRemoved, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgLeftMorph, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgRemovedMorph, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgMediate, pstMemMgr );
	IMP_GrayImageDestroy( &pstAnalyst->stImgStatistic, pstMemMgr );
}

IMP_VOID ipConfigOscAnalysti( IpOscAnalyst *pstAnalyst, IpOscPara *pstPara )
{
	IMP_S32 s32FrmDura;
	USR_ADV_PARA_S *pstUsrAdvPara = (USR_ADV_PARA_S*)pstPara->pstRule->u8AdvParaData;

	pstAnalyst->pstPara = pstPara;
	if (pstUsrAdvPara->s32UseUsrAdvPara)
	{
		pstAnalyst->pstPara->s32UseBorderConstrain = pstUsrAdvPara->stParaAnalyst.stOscPara.s32UseBorderConstrain;
		pstAnalyst->pstPara->s32BorderWidth = pstUsrAdvPara->stParaAnalyst.stOscPara.s32BorderWidth;
		pstAnalyst->pstPara->s32DoSpecialRegionProcess = pstUsrAdvPara->stParaAnalyst.stOscPara.s32DoSpecialRegionProcess;
		pstAnalyst->pstPara->s32SpclMinTime = pstUsrAdvPara->stParaAnalyst.stOscPara.s32SpclMinTime;
	}
	
	s32FrmDura= pstAnalyst->pstPara->s32FrmDura;
	pstAnalyst->pstPara->s32InitTime /= s32FrmDura;
	pstAnalyst->pstPara->s32BgUpdateTime /= s32FrmDura;
	pstAnalyst->pstPara->stLabelPara.s32ThDrgMaxSizeRatio = 1;
	pstAnalyst->pstPara->stLabelPara.s32ThDrgMinSizePixelInit = 8;
	pstAnalyst->pstPara->stLabelPara.s32ThDrgMinSizePixelMax = 64;
	pstAnalyst->pstPara->stLabelPara.s32ThDrgMinSizePixelStep = 8;
	pstAnalyst->pstPara->stLabelPara.s32ThDrgMinSizeWh = 5;
	
//	printf("OSC:%d,%d\n", pstAnalyst->pstPara->s32SpclMinTime, s32FrmDura);
	pstAnalyst->pstPara->s32SpclMinTime /= s32FrmDura;
}



extern IpOscTargetData* ipGetOscTargetData( IMP_VOID *pData );
static IMP_VOID ipInitingBgEdgeImage(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara);
static IMP_VOID ipInitedBgEdgeImage(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara);
static IMP_VOID ipAnalysisOscAbnormal(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara);
IMP_S32 ipProcessOscAnalysti( IpOscAnalyst *pstAnalyst )
{
	IMP_S32 s32CurFrm = pstAnalyst->pstResult->u32FrmTimeCur;
	IMP_S32 s32BgRatio = 50;
	IMP_S32 s32BgMinCnt = 0;
	IpOscPara *pstPara = pstAnalyst->pstPara;
	GRAY_IMAGE_S *pstImgEdgeIn = pstAnalyst->pstResult->stDRegionSet.pstImgInEdge;
	GRAY_IMAGE_S *pstImgEdgeBg = &pstAnalyst->stImgBgEdge;
	GRAY_IMAGE_S *stImgBgInDiff = &pstAnalyst->stImgBgInDiff;
	GRAY_IMAGE_S *pstImgLeftLife = &pstAnalyst->stImgLeftLife;
	GRAY_IMAGE_S *stImgRemovedLife = &pstAnalyst->stImgRemovedLife;
	IMP_S32 s32InitTotalCnt = pstAnalyst->s32InitTotalCnt;
	IMP_S32 s32BgEdgeInited = pstAnalyst->s32BgEdgeInited;
	IMP_S32 s32NeedInitTime;
	if (pstAnalyst->pstPara==NULL)
	{
		return 0;
	}
	s32NeedInitTime = pstAnalyst->pstPara->s32InitTime;

	if (pstAnalyst->s32InitStartFrm == 0)
	{
		
		pstAnalyst->s32InitStartFrm = s32CurFrm;
	}
	else
	{
		ipAnalysisOscAbnormal( pstAnalyst,pstPara );
	}

	return 1;
}
static IMP_VOID ipInitingBgEdgeImage(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara)
{
	GRAY_IMAGE_S *pstImgEdgeIn = pstAnalyst->pstResult->stDRegionSet.pstImgInEdge;
	GRAY_IMAGE_S *pstImgEdgeBg = &pstAnalyst->stImgBgEdge;
	IMP_S32 s32ImgW = pstImgEdgeIn->s32W;
	IMP_S32 s32ImgH = pstImgEdgeIn->s32H;
	IMP_S32 x,y;
	IMP_U8 *pu8DataD1, *pu8DataS1;
	IMP_S32 val11, val12;
	IMP_S32 val21, val22;
	IMP_S32 val31, val32;
	IMP_S32 val41, val42;

	for( y=0; y<s32ImgH; y++ )
	{
		pu8DataS1 = pstImgEdgeIn->pu8Data + s32ImgW*y;
		pu8DataD1 = pstImgEdgeBg->pu8Data + s32ImgW*y;

#ifdef _TIDSP
	#pragma MUST_ITERATE(64,,4);
#endif
		for( x=0; x<s32ImgW; x+=4 )
		{
			val11 = pu8DataD1[0];
			val12 = pu8DataS1[0];

			val21 = pu8DataD1[1];
			val22 = pu8DataS1[1];

			val31 = pu8DataD1[2];
			val32 = pu8DataS1[2];

			val41 = pu8DataD1[3];
			val42 = pu8DataS1[3];

			val12 = val12 ? val11+1 : val11;
			val22 = val22 ? val21+1 : val21;
			val32 = val32 ? val31+1 : val31;
			val42 = val42 ? val41+1 : val41;

			pu8DataD1[0] = val12;
			pu8DataD1[1] = val22;
			pu8DataD1[2] = val32;
			pu8DataD1[3] = val42;

			pu8DataD1+=4; pu8DataS1+=4;
		}
	}
}
static IMP_VOID ipInitedBgEdgeImage(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara)
{
	GRAY_IMAGE_S *pstImgEdgeIn = pstAnalyst->pstResult->stDRegionSet.pstImgInEdge;
	GRAY_IMAGE_S *pstImgEdgeBg = &pstAnalyst->stImgBgEdge;
	IMP_S32 s32BgMinCnt;
	IMP_S32 s32InitTotalCnt = pstAnalyst->s32InitTotalCnt;
	IMP_S32 s32BgRatio = pstPara->s32InitBgEdgeRatioTh;
	IMP_S32 s32ImgW = pstImgEdgeIn->s32W;
	IMP_S32 s32ImgH = pstImgEdgeIn->s32H;
	IMP_S32 x,y;
	IMP_U8 *pu8DataD1;
	IMP_S32 val11, val12;
	IMP_S32 val21, val22;
	IMP_S32 val31, val32;
	IMP_S32 val41, val42;
	s32BgMinCnt = s32InitTotalCnt * s32BgRatio/100;
	if (s32BgMinCnt <= 0 )
	{
		s32BgMinCnt = 20;
	}

	for( y=0; y<s32ImgH; y++ )
	{
		pu8DataD1 = pstImgEdgeBg->pu8Data + s32ImgW*y;

#ifdef _TIDSP
	#pragma MUST_ITERATE(64,,4);
#endif
		for( x=0; x<s32ImgW; x+=4 )
		{
			val11 = pu8DataD1[0];

			val21 = pu8DataD1[1];

			val31 = pu8DataD1[2];

			val41 = pu8DataD1[3];

			val12 = val11>s32BgMinCnt ? 255 : 0;
			val22 = val21>s32BgMinCnt ? 255 : 0;
			val32 = val31>s32BgMinCnt ? 255 : 0;
			val42 = val41>s32BgMinCnt ? 255 : 0;

			pu8DataD1[0] = val12;
			pu8DataD1[1] = val22;
			pu8DataD1[2] = val32;
			pu8DataD1[3] = val42;

			pu8DataD1+=4;
		}
	}
}
static IMP_VOID ipGetMagDiffImage(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara);
static IMP_VOID ipGetLeftRemovedLifeImage(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara);
static IMP_VOID ipGetLeftRemovedImage(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara);
static IMP_S32 ipJudgePeopleEnter(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara);
static IMP_VOID ipRemoveRegionSetLight(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs);
static IMP_VOID ipRemovedRegionSetNoise(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs);
// IMP_S32 g_peopel_in = 0;
 extern 	IMP_S32 g_person_in;
 extern 	IMP_S32 g_person_leave;

static IMP_S32 	ipCalcTargetDiffEnergy(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImgFrmDiff);
static IMP_VOID ipPrintOscTarget(  IpTrackedTarget *ttarget );
static IMP_S32  ipAnalysisBehaviorOsc( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex );
static IMP_VOID ipAnalysisOscAbnormal(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara)
{
	IMP_S32  i, s32ZoneIndex;
	IpTrackedTarget *pstTarget;
	IpTrackedTargetSet *pstTTgtset = &pstAnalyst->pstResult->stTrackedTargetSetOsc;
	RULE_ZONE_S *pstZone1 = pstAnalyst->pstPara->pstRule->stZones.astZone;
	GRAY_IMAGE_S *pstImage = &pstAnalyst->pstPara->pstRule->stZones.stImage;
	GRAY_IMAGE_S *pstImgFrmDiff = pstAnalyst->pstResult->stDRegionSetOsc.pstImgFrmDiff;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pstAnalyst->stDRegionSet;
#define DBG_AAA (1&PRINT_DBG)
	IMP_S32 s32ImgW = pstAnalyst->pstResult->s32Width;
	IMP_S32 s32ImgH = pstAnalyst->pstResult->s32Height;
	IMP_S32 s32Ret = 0;
	IMP_S32 s32MorphLevel = pstPara->s32MorphLevel;
	IMP_S32 s32BgUpdateTime = pstPara->s32BgUpdateTime;
	IMP_S32 s32CurFrm = pstAnalyst->pstResult->u32FrmTimeCur;
	RULE_ZONE_S *pstZone = &pstPara->pstRule->stZones.astZone[0];
	RULE_PARA_OSC_S *pstOscPara = &pstZone->stPara.stOsc;

	for( i=0; i<IMP_MAX_TGT_CNT; i++ )
	{
	//	continue; //debug by mzhang;
		pstTarget = &pstTTgtset->astTargets[i];
		
		if (ipTrackedTargetIsValid(pstTarget))
		{
			ipCalcTargetDiffEnergy(pstTarget,pstImgFrmDiff);
		}
	//	printf("ipAnalysisOscAbnormal[3]:%d\n", pstTarget->u32TargetId);
		if( !ipTrackedTargetIsActive( pstTarget ) ) //object is active if it is used and stable
		{
			continue;
		}
//		printf("ipAnalysisOscAbnormal[4]:%d\n", pstTarget->u32TargetId);
		if( ipTrackedTargetIsMeasured( pstTarget ) ) //measured? used??
		{
	//	printf("ipAnalysisOscAbnormal[5]:%d\n", pstTarget->u32TargetId);
			for( s32ZoneIndex=0; s32ZoneIndex<IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
			{
				if( !IMP_IS_ACTIVE_ZONE( &pstZone1[s32ZoneIndex], (IMP_FUNC_OSC) ) )
					continue;

				//   根据模式设置进行行为分析
				pstOscPara = &pstPara->pstRule->stZones.astZone[s32ZoneIndex].stPara.stOsc;
				
				//OSC检测
				if  (pstZone1[s32ZoneIndex].u32Mode&IMP_FUNC_OSC)
				{
					
					ipAnalysisBehaviorOsc( pstOscPara, pstPara,pstTarget, s32ZoneIndex );
				}
			}

		}
		else
		{
			pstTarget->stTargetInfo.u32TgtEvent = IMP_TGT_EVENT_UNKNOWN;
			memset( pstTarget->stTargetInfo.au32Events, 0, sizeof(pstTarget->stTargetInfo.au32Events) );
		}
	}

}

//calc difference energy from pstImgFrmDiff
static IMP_S32 	ipCalcTargetDiffEnergy(IpTrackedTarget *pstTarget,GRAY_IMAGE_S *pstImgFrmDiff)
{
	IpTargetPosition  *pstPos;
	IMP_S32 x,y;
	IMP_U8 *diff_dat = pstImgFrmDiff->pu8Data;
	IMP_S32 s32DiffEnergy = 0;
	IMP_S32 w = pstImgFrmDiff->s32W;
	IMP_S32 s32RgW ,s32RgH;
	pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	s32RgW = abs(pstPos->stRg.s16X2 - pstPos->stRg.s16X1);
	s32RgH = abs(pstPos->stRg.s16Y2 - pstPos->stRg.s16Y1);
	for (y=pstPos->stRg.s16Y1; y<pstPos->stRg.s16Y2; y++)
	{
		diff_dat = pstImgFrmDiff->pu8Data + w*y;
		for (x=pstPos->stRg.s16X1; x<pstPos->stRg.s16X2; x++)
		{
			if (diff_dat[x])
			{
				s32DiffEnergy++;
			}
		}
	}
//	if (pstPos->u32AreaPixel > 500) printf("\033[31mDE:%d\033[0m\n", s32DiffEnergy);
//	pstPos->s32DiffEnergy = (IMP_S32)((IMP_FLOAT)s32DiffEnergy * 1000/(s32RgW*s32RgH)+0.5);
	pstPos->s32DiffEnergy = s32DiffEnergy;
	return s32DiffEnergy;
}

static IMP_S32 ipGetTargetPixelArea( IpTrackedTarget *pstTarget )
{
	IpTargetPosition *pstPos;
	IMP_S32 i;
	IMP_S32 s32Len;
	IMP_S32 s32Area=0;
	s32Len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	if (s32Len > 5)
		s32Len = 5;
	for (i=0; i<s32Len; i++)
	{
		pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		s32Area += pstPos->u32AreaPixel;
	}
	s32Area /= s32Len;

	return s32Area;
}

static IMP_S32 ipOscIsAccordantSize( RULE_PARA_OSC_S *pstOscPara, IpTrackedTarget *pstTarget);
static IMP_S32 ipOscIsAccordantTime( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara,IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex);
static IMP_S32 ipOscIsDetectedEvent( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara,IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex);
static IMP_S32 ipOscIsAccordantBorder( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara,IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex);
static IMP_S32 ipOscIsAccordantSpclRg( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara,IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex);
static IMP_S32 ipOscIsAccordantDrgNormal( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara,IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex);
static IMP_S32 ipAnalysisBehaviorOsc( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex )
{
	IMP_U32  u32Status;
	RULE_ZONE_S *pstZone = &pstPara->pstRule->stZones.astZone[s32ZoneIndex];
	RULE_PARA_OSC_S *osc_para = &pstZone->stPara.stOsc;
#if PRINT_DBG
printf("-----------------------\n");
#endif
#if 0
	if (pstTarget->u32TargetId == -1610612592)
	{
		if (ipOscIsAccordantSpclRg( osc_para, pstPara, pstTarget,s32ZoneIndex ))
			printf("Hi car!!\n");
	}
#endif
	
//	printf("Hi removing car!!\n");
	if(	1
		&& ipOscIsAccordantSize( osc_para, pstTarget ) //if size is enough
		&& ipOscIsAccordantTime( osc_para, pstPara,pstTarget,s32ZoneIndex ) //if time is enough
	//	&& ipOscIsDetectedEvent( osc_para, pstPara, pstTarget,s32ZoneIndex ) //if event is it.
	//	&& ipOscIsAccordantBorder( osc_para, pstPara, pstTarget,s32ZoneIndex )
		&& ipOscIsAccordantSpclRg( osc_para, pstPara, pstTarget,s32ZoneIndex )
		&& ipOscIsAccordantDrgNormal( osc_para, pstPara, pstTarget,s32ZoneIndex )
	//	&& (!pstPara->s32LeaveLimit)
		)
	{
#if PRINT_DBG || 0
		printf("ipAnalysisBehaviorOsc All passed!\n");
#endif
		u32Status = ipOscIsDetectedEvent(osc_para, pstPara, pstTarget,s32ZoneIndex); //if event is it.
		if (IMP_IS_RGN_RSTATIC_L(u32Status))
		{
			
			pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_OSC;
		}
		else if (IMP_IS_RGN_RSTATIC_R(u32Status))
		{
			pstTarget->stTargetInfo.au32Events[s32ZoneIndex] |= IMP_TGT_EVENT_OSC_R;
		}
		else
		{
			return 0;
		}
		pstTarget->stTargetInfo.u32AlarmLevel = pstZone->u32Level;
#if PRINT_DBG & 0
		ipPrintOscTarget(pstTarget);
#endif
		return 1;
	}
	return 0;
}

static IMP_S32 ipOscIsAccordantSize( RULE_PARA_OSC_S *pstOscPara,IpTrackedTarget *pstTarget)
{
	IMP_S32 s32TargetArea = ipGetTargetPixelArea(pstTarget);
	IMP_S32 s32Ret = 0;

	if (s32TargetArea >= pstOscPara->stOscPara.s32SizeMin  && s32TargetArea<= pstOscPara->stOscPara.s32SizeMax)
	{
		s32Ret = 1;
	}
	
//	if (pstTarget->u32TargetId == -1610612732)
//	{
//		printf("%d:%d\n", pstTarget->u32TargetId, s32TargetArea);
//	}
#if PRINT_DBG
printf("ipOscIsAccordantSize:%d, %d\n", s32TargetArea, s32Ret);
#endif
	return s32Ret;
}
static IMP_S32 ipOscIsAccordantTime( RULE_PARA_OSC_S *pstOscPara,IpOscPara *pstPara,IpTrackedTarget *pstTarget,IMP_S32 s32ZoneIndex)
{
	IMP_S32 s32Ret = 0;
	IMP_S32 s32Life = 0;

	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 s32Len;
	IMP_S32 s32Index = 0;
	IpOscTargetData *pstTargetData = ipGetOscTargetData( pstTarget->stPrivateData.pDataAnalyst );
	s32Len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
	if( pstTargetData->s32IsFirstPos == 0 )
	{
		pstTargetData->s32IsFirstPos = 1;
		pstTargetData->pastFirstPos[s32ZoneIndex] = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -s32Len+1 );
		pstTargetData->s32FirstPosTime = pstTargetData->pastFirstPos[s32ZoneIndex]->u32Time;
	}
	pstPos1 = pstTargetData->pastFirstPos[s32ZoneIndex];
	pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	
	s32Life = pstPos2->u32Time - pstTargetData->s32FirstPosTime;
	
	s32Life /= 20; //temporary for debug, same to the s32FrmDura, IMP_ANALYST_FRM_DURA
	
	s32Ret = (s32Life >= pstOscPara->stOscPara.s32TimeMin);
	
//	if (pstTarget->u32TargetId == -1610612732)
//	{
//		printf("%d:%d\n", pstTarget->u32TargetId, s32Life);
//	}
#if PRINT_DBG || 0
printf("id:%d ipOscIsAccordantTime:%d,%d, ret:%d\n", pstTarget->u32TargetId, s32Life, pstOscPara->stOscPara.s32TimeMin, s32Ret);
#endif

	return s32Ret;
}

static IMP_U32 ipDetermineTRegionStatusOnTraject( IpOscPara *pstPara, IpTrackedTarget *pstTarget );
static IMP_S32 ipOscIsDetectedEvent( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex)
{
	IMP_U32  u32Status = ipDetermineTRegionStatusOnTraject( pstPara, pstTarget );
//	return ( IMP_IS_RGN_RSTATIC_L(u32Status) );
//	printf("ipOscIsDetectedEvent:%d\n", IMP_IS_RGN_RSTATIC_R(u32Status));
	return u32Status; //( IMP_IS_RGN_RSTATIC_R(u32Status) );
}
static IMP_U32 ipDetermineTRegionStatusOnTraject( IpOscPara *pstPara, IpTrackedTarget *pstTarget )
{
	IMP_U32 u32Status = 0;
	IpTargetPosition *pstPos;
	IpTargetPosition *pstPos1;
	IMP_S32 i, s32Len, s32LenR, s32LenL, s32Len2, s32Len3;
	IMP_S32 s32DiffEnergy = 0;
	IMP_S32 s32ChgVal = 0;
	IMP_S32 s32LastChgVal = 0;
	IMP_S32 s32Flickering = 0;
	IMP_S32 s32FlickerCnt = 0;
	IMP_S32 s32MeanEnergy = 0;

	s32Len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );

	s32Len = s32Len < pstPara->s32TrgnTrajectLen ? s32Len : pstPara->s32TrgnTrajectLen;



	s32LenR = s32LenL = 0;
	i = 0;
	while( i<s32Len )
	{
		pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );

		i++;

		if( IMP_IS_RGN_RSTATIC_L( pstPos->u32DrgType ) )
		{
		//	printf(" static_l ");
			++s32LenL;
		}
		if( IMP_IS_RGN_RSTATIC_R( pstPos->u32DrgType ) )
			++s32LenR;
	}
	
	
	s32Len2 = i;
	i = 0;
	s32Len = s32Len < pstPara->s32TrgnTrajectLen*2 ? s32Len : pstPara->s32TrgnTrajectLen*2;

#if PRINT_DBG || 0
printf("ipDetermineTRegionStatusOnTraject:\033[33m%d\033[0m \n", s32Len);
#endif
	
#if 0 //teddy
	while( i<s32Len-1)
	{
		pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );
				pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i+1 );
		if (abs(pstPos->s32DiffEnergy) > 5 && abs(pstPos1->s32DiffEnergy) > 5)
		{
			s32ChgVal = pstPos->s32DiffEnergy - pstPos1->s32DiffEnergy;
		}
		else
		{
			s32ChgVal = 0;
		}

		if (s32ChgVal > 0 && s32LastChgVal > 0 && abs(s32ChgVal)<=5)
		{
			s32Flickering--;
		}
		else if (s32ChgVal <= 0 && s32LastChgVal <= 0&& abs(s32ChgVal)<=5)
		{
			s32Flickering--;
		}
		else
		{
			s32Flickering = s32Flickering + 2*abs(s32ChgVal);
			//s32Flickering = s32Flickering + 3;
			s32FlickerCnt++;
		}

		s32LastChgVal = s32ChgVal;
		i++;
		s32MeanEnergy +=pstPos->s32DiffEnergy;
		if (pstPos->s32DiffEnergy < 5)
			s32DiffEnergy++;
		else
			s32DiffEnergy--;
	}
	
	if (pstTarget->u32TargetId == -1610612114)
	{
		printf("%d:%d,%d, %d, %d, \033[32mflk:%d,%d\033[0m, me:%d\n", pstTarget->u32TargetId, s32LenL, s32LenR, s32Len2, pstPara->s32TrgnTrajectRatio, i, s32Flickering, s32MeanEnergy);
	}
	
	s32Len3 = i;
	if (s32Len3)
	{
		s32MeanEnergy = s32MeanEnergy/s32Len3;
	}
	if (s32Flickering > 0 && s32FlickerCnt * 100 >= s32Len3 * 5)
	{
		s32Flickering = 1;
	}
	else
	{
		s32Flickering = 0;
	}
#endif //teddy
	
	
	if( s32LenL * 100 >= s32Len2 * pstPara->s32TrgnTrajectRatio) // && !s32Flickering &&  s32MeanEnergy < 5 && s32DiffEnergy * 100 >= s32Len3 * 70  )
		IMP_SET_RGN_RSTATIC_L(u32Status);
	if( s32LenR * 100 >= s32Len2 * pstPara->s32TrgnTrajectRatio )
	{
	//	if (pstTarget->u32TargetId == -1610612732) 
	//	{
	//		printf("%d:removing OK!!\n", pstTarget->u32TargetId);
	//	}
		IMP_SET_RGN_RSTATIC_R(u32Status);
	}
#if PRINT_DBG || 0
printf("ipDetermineTRegionStatusOnTraject_Status:%d \n", u32Status);
#endif

	return u32Status;
}

////////////////////////////////////////////////////////////////////////////
//函数功能：检测当前的blob矩形是不是满足边界条件
//参数：pstRectRegion：检测的目标区域，pstRectImage：图像的区域
//		s32BorderWidth:距离边界的距离
//返回值：如果检测出的blob距离边界比s32BorderWidth大，返回0，否则返回1
//////////////////////////////////////////////////////////////////////////
static IMP_S32 ipIsRectNeighborBorder( IMP_RECT_S *pstRectRegion, IMP_RECT_S *pstRectImage, IMP_S32 s32BorderWidth )
{
	IMP_S32 s32Ret = 0;

	s32Ret = ( 0
		|| pstRectImage->s16X1 + s32BorderWidth >= pstRectRegion->s16X1
		|| pstRectImage->s16Y1 + s32BorderWidth >= pstRectRegion->s16Y1
		|| pstRectImage->s16X2 - s32BorderWidth <= pstRectRegion->s16X2
		|| pstRectImage->s16Y2 - s32BorderWidth <= pstRectRegion->s16Y2


		);

	return s32Ret;
}

static IMP_S32 ipOscIsAccordantBorder( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex)
{
 	IMP_S32 s32Ret = 0;
	IMP_S32 s32UseBorderConstrain = pstPara->s32UseBorderConstrain;
	IMP_S32 s32BorderWidth = pstPara->s32BorderWidth;
	OSC_OBJ_LMT_SPECL_REGIONS_S *pstSpclRgs = pstOscPara->astSpclRgs;
	IMP_S32 s32SpclRgsNum = 0;
	IMP_S32 i = 0;

	for (i = 0; i<IMP_MAX_OSC_NUM;i++)
	{
		if (pstOscPara->astSpclRgs[i].s32Valid)
		{
			s32SpclRgsNum++;
		}
	}
	s32BorderWidth = s32BorderWidth/s32SpclRgsNum;

	if( s32UseBorderConstrain )
 	{
 		IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
 		IMP_RECT_S pstRectImage;
 		pstRectImage.s16X1 = 0;
 		pstRectImage.s16Y1 = 0;
 		pstRectImage.s16X2 = 352 - 1;
 		pstRectImage.s16Y2 = 288 - 1;
 		if( !ipIsRectNeighborBorder( &pstPos->stRg, &pstRectImage, s32BorderWidth ) )
			s32Ret = 1;
 	}
	return s32Ret;
}
static IMP_S32 ipInOscRegion(IpTrackedTarget *pstTarget,POLYGON_REGION_S stOscRegion);

IMP_S32 ipOutsideOfRegion(POLYGON_REGION_S region, PEA_DETECTED_REGION_S *pstDrg, IMP_U8 *pu8Data, IMP_S32 s32W);


static IMP_S32 ipOscIsAccordantSpclRg( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex)
{
	Point apolygon[10], point;
	IMP_S32 dwPI, s32PntNum;
	IMP_S32 s32SizeFlag = 0;
	IMP_S32 s32TimeFlag = 0;
	IMP_S32 s32SpclFlag = 0;
	IMP_S32 s32Valid = 0;
	IMP_S32 s32Ret = 0;
	IMP_S32 s32TargetArea;
	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 s32Len, s32Life;
	OSC_OBJ_LMT_SPECL_REGIONS_S *pstSpclRgs = pstOscPara->astSpclRgs;
	IMP_S32 s32Index = 0;
	IMP_S32 s32InOsc = 0;
	IpOscTargetData *pstTargetData = ipGetOscTargetData( pstTarget->stPrivateData.pDataAnalyst );
	IMP_S32 s32LeaveLimit = pstPara->s32LeaveLimit;
	
	GRAY_IMAGE_S *pstImgFg = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn;
	PEA_DETECTED_REGION_S	*pstDrg = NULL;
	IMP_U8 *pu8Data;
	IMP_S32 s32ImgW = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->s32W;
	IMP_S32 s32ImgH = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->s32H;
	
	s32Len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
	if( pstTargetData->s32IsFirstPos == 0 )
	{
		pstTargetData->s32IsFirstPos = 1;
		pstTargetData->pastFirstPos[s32ZoneIndex] = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -s32Len+1 );
		pstTargetData->s32FirstPosTime = pstTargetData->pastFirstPos[s32ZoneIndex]->u32Time;
	}

	pstPos1 = pstTargetData->pastFirstPos[s32ZoneIndex];
//	pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -s32Len );
	pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );

	s32Life = pstPos2->u32Time - pstTargetData->s32FirstPosTime;
	for (s32Index = 0;s32Index<IMP_MAX_OSC_NUM;s32Index++)
	{
		if (!pstSpclRgs[s32Index].s32Valid)
			continue;
		

		s32PntNum = pstSpclRgs[s32Index].stOscRg.s32PointNum;
		for (dwPI = 0; dwPI < s32PntNum; dwPI++)
		{
			apolygon[dwPI].x = pstSpclRgs[s32Index].stOscRg.astPoint[dwPI].s16X;
			apolygon[dwPI].y = pstSpclRgs[s32Index].stOscRg.astPoint[dwPI].s16Y;
		}
		point.x = pstPos2->stPt.s16X;
		point.y = pstPos2->stPt.s16Y;
		s32InOsc = !InsidePolygon(apolygon, s32PntNum, point);
		if (s32InOsc)
			break;
	}
	
//	printf("%d:%d\n", pstTarget->u32TargetId, s32InOsc);
	if (!s32InOsc)
	{
#if PRINT_DBG || 0
printf("ipOscIsAccordantSpclRg:%d\n", s32Ret);
#endif		
		return s32Ret;
	}
	
	s32SpclFlag = 1;
	
	for (s32Index = 0; s32Index < IMP_MAX_OSC_NUM; s32Index++)
	{
		if (!pstSpclRgs[s32Index].s32Valid)
			continue;
		
		if (pstSpclRgs[s32Index].stSpclRgA.s32Valid)
		{
			s32SpclFlag = ipOutsideOfRegion(pstSpclRgs[s32Index].stSpclRgA, pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex, pstImgFg->pu8Data, pstImgFg->s32W);
//			printf("---A:%d\n", s32SpclFlag);
			if (!s32SpclFlag) break;
		}
		
		if (pstSpclRgs[s32Index].stSpclRgB.s32Valid)
		{	
			s32SpclFlag = ipOutsideOfRegion(pstSpclRgs[s32Index].stSpclRgB, pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex, pstImgFg->pu8Data, pstImgFg->s32W);
//			printf("---B:%d\n", s32SpclFlag);
			if (!s32SpclFlag) break;
		}
		
		if (pstSpclRgs[s32Index].stSpclRgC.s32Valid)
		{	
			s32SpclFlag = ipOutsideOfRegion(pstSpclRgs[s32Index].stSpclRgC, pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex, pstImgFg->pu8Data, pstImgFg->s32W);
//			printf("---C:%d\n", s32SpclFlag);
			if (!s32SpclFlag) break;
		}
	}

//	printf("---%d, %d\n", s32Life, pstPara->s32SpclMinTime);
//	if( s32Life >= pstPara->s32SpclMinTime )
//		s32TimeFlag =1;

//	s32TargetArea = ipGetTargetPixelArea(pstTarget);
//	if ((IMP_FLOAT)s32TargetArea >= 0.75*(s32MaxX-s32MinX)*(s32MaxY-s32MinY)   && s32TargetArea<= (s32MaxX-s32MinX)*(s32MaxY-s32MinY))
//	{
//		s32SizeFlag = 1;
//	}
	if (!s32SpclFlag)
	{
		return s32Ret;
	}
	else
	{
//		if (s32TimeFlag)// && s32SizeFlag)
//		{
			s32Ret = 1;
//		}
//		else
//		{
//			s32Ret = 0;
//		}
	}
	
#if PRINT_DBG || 0
printf("ipOscIsAccordantSpclRg:%d\n", s32Ret);
#endif
	return s32Ret;
}


IMP_S32 ipOutsideOfRegion(POLYGON_REGION_S region, PEA_DETECTED_REGION_S *pstDrg, IMP_U8 *pu8Data, IMP_S32 s32W)
{
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 xx, yy, dwPI;
	IMP_S32 s32SpclFlag = 1;
	IMP_S32 s32Sn0, s32PntNum;
	Point apolygon[10], point;
	
	
	s32PntNum = region.s32PointNum;
	for (dwPI = 0; dwPI < s32PntNum; dwPI++)
	{
		apolygon[dwPI].x = region.astPoint[dwPI].s16X;
		apolygon[dwPI].y = region.astPoint[dwPI].s16Y;
	}
	
//	pstDrg = pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex;
	
	//check if the point inside the Polygon
	
	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Sn0 = pstDrg->u8Sign;
//	pu8Data = pstImgFg->pu8Data + lb_y*pstImgFg->s32W;
	pu8Data += lb_y * s32W;
	s32SpclFlag = 1;
	for( yy=lb_y; yy<=ru_y; yy++ )
	{
		for( xx=lb_x; xx<=ru_x; xx++ )
		{
			if( pu8Data[xx]==s32Sn0)
			{
				point.x = xx;
				point.y = yy;
				
				if (!InsidePolygon(apolygon, s32PntNum, point))
				{
					s32SpclFlag = 0;
					return s32SpclFlag;
				}
			}
		}
		pu8Data += s32W;
	}
	
	return s32SpclFlag;
}


IMP_S32 ipInOscRegion(IpTrackedTarget *pstTarget,POLYGON_REGION_S stOscRegion)
{
	Point apolygon[10], point;
	IMP_S32 dwPI;
	IMP_S32 s32PntNum = 0;
	IMP_S32 s32Ret = 0;
	IpTargetPosition *pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	IMP_S32 s32Count=0;                   //交点数

	
	s32PntNum = stOscRegion.s32PointNum;
	for (dwPI = 0; dwPI < s32PntNum; dwPI++)
	{
		apolygon[dwPI].x = stOscRegion.astPoint[dwPI].s16X;
		apolygon[dwPI].y = stOscRegion.astPoint[dwPI].s16Y;
	}
	
	point.x = pstPos->stPt.s16X;
	point.y = pstPos->stPt.s16Y;

	if (!InsidePolygon(apolygon, s32PntNum, point))
	{
		s32Ret = 1;
	}
	
	return s32Ret;
}


//--------------------------------------------------------------------------
IMP_S32  ipIntersect( const IMP_POINT_S stBegin, const IMP_POINT_S stEnd, const IMP_POINT_S stPoint)
{
	IMP_POINT_S stRigthPt;
	IMP_S32 s32Val1,s32Val2;

	stRigthPt.s16X = 1024;
	stRigthPt.s16Y = stPoint.s16Y;
	s32Val1   =   (stPoint.s16Y-stBegin.s16Y)*(stEnd.s16X-stBegin.s16X)-(stEnd.s16Y-stBegin.s16Y)*(stPoint.s16X-stBegin.s16X);
	s32Val2   =   (stRigthPt.s16Y-stBegin.s16Y)*(stEnd.s16X-stBegin.s16X)-(stEnd.s16Y-stBegin.s16Y)*(stRigthPt.s16X-stBegin.s16X);

	if(s32Val1 * s32Val2 < 0)
		return   1;       //有交点
	else
		return   0;
}
static IMP_S32 ipOscIsAccordantDrgNormal( RULE_PARA_OSC_S *pstOscPara, IpOscPara *pstPara, IpTrackedTarget *pstTarget, IMP_S32 s32ZoneIndex)
{
	IMP_S32 s32Ret = 0;
	IMP_S32 s32Del = 0;
	IMP_S32 width, height;
	IMP_S32 u32AreaPixel;
	PEA_DETECTED_REGION_S	*pstDrg = NULL;
	IMP_S32 s32ImgW = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->s32W;
	IMP_S32 s32ImgH = pstTarget->stTargetImage.pstDRegions->pstImgFgRgn->s32H;
	IMP_S32 s32ThDrgMinSizeWh = 5;
	IMP_S32 s32DrgMinSizePixel = 30;
	IMP_S32 s32ThDrgMaxSizeRatio = 3;
	IMP_S32 s32ThDrgAspectRatio = 6;
	IMP_S32 s32ThDrgFillRatio = 40;

	if( pstTarget->stTargetImage.pstDRegions )
	{
		pstDrg = pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex;
				u32AreaPixel = pstDrg->s32AreaPixel;
		width = pstDrg->stRect.s16X2-pstDrg->stRect.s16X1+1;
		height = pstDrg->stRect.s16Y2-pstDrg->stRect.s16Y1+1;

		s32Del = ( 0
			|| ( width < s32ThDrgMinSizeWh )
			|| ( height < s32ThDrgMinSizeWh )
			|| ( u32AreaPixel < s32DrgMinSizePixel )
			|| ( u32AreaPixel*s32ThDrgMaxSizeRatio > s32ImgW*s32ImgH )
 			|| ( !IMP_DRG_IS_DIFF(pstDrg->u8Used) && ( u32AreaPixel*100 < s32ThDrgFillRatio*(width*height) ) )
			|| ( height > s32ThDrgAspectRatio*width )
			|| ( width > s32ThDrgAspectRatio*height )
			);
	}
	s32Ret = !s32Del;
	
#if PRINT_DBG || 0
printf("ipOscIsAccordantDrgNormal:%d\n", s32Ret);
#endif

	return s32Ret;
}

IMP_VOID ipRemoveRegionHelper11( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 i, IMP_U8 sn )
{
	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_U8 *pu8Data;
	IMP_S32 s32Sn0;
	IMP_S32 xx, yy;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	GRAY_IMAGE_S *pstImgFg = pstRgs->pstImgFgRgn;

	pstDrg = pstRgs->astDrg+ i;
	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Sn0 = pstDrg->u8Sign;
	pu8Data = pstImgFg->pu8Data + pstDrg->stRect.s16Y1*pstImgFg->s32W;
	for( yy=lb_y; yy<=ru_y; yy++ )
	{
		for( xx=lb_x; xx<=ru_x; xx++ )
		{
			if( pu8Data[xx]==s32Sn0 )
				pu8Data[xx] = sn;
		}
		pu8Data += pstImgFg->s32W;
	}
	ipDetectedRegionSetFree( pstRgs, i );
}


static IMP_VOID ipRemovedRegionSetNoise(IpOscAnalyst *pstAnalyst, IpOscPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs)
{
	GRAY_IMAGE_S *pstImgLeft = &pstAnalyst->stImgLeft;
	GRAY_IMAGE_S *pstImgRemoved = &pstAnalyst->stImgRemoved;

	IMP_S32 x,y;

	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_U8 *pu8DataS1;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 i, s32Cnt,s32Sn0;
	IMP_S32 width, height;
	IMP_S32 u32AreaPixel = 0;
	IMP_S32 s32MinArea = pstPara->s32MinArea;
	IMP_S32 s32ImgW = pstAnalyst->stImgBgEdge.s32W;
	IMP_S32 s32ImgH = pstAnalyst->stImgBgEdge.s32H;
	IMP_S32 s32DiagonalLen = 0;
	IMP_ITERATE_RGS_START
	if( IMP_DRG_IS_LABELED(pstDrg->u8Used) || IMP_DRG_IS_COMPLETE(pstDrg->u8Used) )
	{
		u32AreaPixel = 0;
		width = pstDrg->stRect.s16X2-pstDrg->stRect.s16X1+1;
		height = pstDrg->stRect.s16Y2-pstDrg->stRect.s16Y1+1;
		lb_x = pstDrg->stRect.s16X1;
		ru_x = pstDrg->stRect.s16X2;
		lb_y = pstDrg->stRect.s16Y1;
		ru_y = pstDrg->stRect.s16Y2;
		s32DiagonalLen = abs(lb_x-ru_x) + abs(lb_y-ru_y);
		s32Sn0 = pstDrg->u8Sign;
		pu8DataS1 = pstImgLeft->pu8Data + pstDrg->stRect.s16Y1*s32ImgW;
		for( y=lb_y; y<=ru_y; y++ )
		{
			for( x=lb_x; x<=ru_x; x++ )
			{
				if( pu8DataS1[x]==255 )
					u32AreaPixel++;
					//pu8Data[xx] = sn;
			}
			pu8DataS1 += s32ImgW;
		}
		if (u32AreaPixel < s32MinArea || u32AreaPixel < s32DiagonalLen)
		{
			pu8DataS1 = pstRgs->pstImgFgRgn->pu8Data + pstDrg->stRect.s16Y1*s32ImgW;
			for( y=lb_y; y<=ru_y; y++ )
			{
				for( x=lb_x; x<=ru_x; x++ )
				{
					if( pu8DataS1[x]==s32Sn0 )
						pu8DataS1[x] = 0;
				}
				pu8DataS1 += s32ImgW;
			}
			ipDetectedRegionSetFree( pstRgs, i );
		}
	}
	IMP_ITERATE_RGS_END
}
static IMP_VOID ipGetMagDiffImage(IpOscAnalyst *pstAnalyst, IpOscPara *pstPara)
{
	GRAY_IMAGE_S *pstImgInMag = pstAnalyst->pstResult->stDRegionSet.pstImgBgEdgeMag;
	GRAY_IMAGE_S *pstImgBgMag = pstAnalyst->pstResult->stDRegionSet.pstImgInEdgeMag;
	GRAY_IMAGE_S *pstImgMagDiff = &pstAnalyst->stImgBgInDiffMag;
	IMP_S32 s32DiffTh = 20;
	IMP_S32 s32ImgW = pstImgInMag->s32W;
	IMP_S32 s32ImgH = pstImgInMag->s32H;
	IMP_S32 x,y;
	IMP_U8 *pu8DataS1,*pu8DataS2;
	IMP_U8 *pu8DataD1;

	IMP_S32 val11, val12, val13;

	IMP_S32 val21, val22, val23;
	IMP_S32 val31, val32, val33;
	IMP_S32 val41, val42, val43;

	for( y=0; y<s32ImgH; y++ )
	{
		pu8DataS1 = pstImgInMag->pu8Data + s32ImgW*y;
		pu8DataS2 = pstImgBgMag->pu8Data + s32ImgW*y;

		pu8DataD1 = pstImgMagDiff->pu8Data + s32ImgW*y;

#ifdef _TIDSP
#pragma MUST_ITERATE(64,,4);
#endif
		for( x=0; x<s32ImgW; x+=4 )
		{
			val11 = pu8DataS1[0];
			val12 = pu8DataS2[0];

			val21 = pu8DataS1[1];
			val22 = pu8DataS2[1];

			val31 = pu8DataS1[2];
			val32 = pu8DataS2[2];

			val41 = pu8DataS1[3];
			val42 = pu8DataS2[3];

			val13 = abs(val12-val11)>s32DiffTh ? 255 : 0;
			val23 = abs(val22-val21)>s32DiffTh ? 255 : 0;
			val33 = abs(val32-val31)>s32DiffTh ? 255 : 0;
			val43 = abs(val42-val41)>s32DiffTh ? 255 : 0;

			pu8DataD1[0] = val13;
			pu8DataD1[1] = val23;
			pu8DataD1[2] = val33;
			pu8DataD1[3] = val43;

			pu8DataS1+=4; pu8DataS2+=4;
			pu8DataD1+=4;
		}
	}
}

static IMP_VOID ipGetLeftRemovedLifeImage(IpOscAnalyst *pstAnalyst,	IpOscPara *pstPara)
{
	GRAY_IMAGE_S *pstImgEdgeIn = pstAnalyst->pstResult->stDRegionSet.pstImgInEdge;
	GRAY_IMAGE_S *pstImgEdgeBg = &pstAnalyst->stImgBgEdge;

	GRAY_IMAGE_S *pstImgBgInDiff = &pstAnalyst->stImgBgInDiff;
	GRAY_IMAGE_S *pstImgLeftLife = &pstAnalyst->stImgLeftLife;
	GRAY_IMAGE_S *pstImgRemovedLife = &pstAnalyst->stImgRemovedLife;
	IMP_S32 add_weight = pstPara->s32AddWeight;
	IMP_S32 sub_weight = pstPara->s32SubWeight;
	IMP_S32 s32ImgW = pstImgEdgeIn->s32W;
	IMP_S32 s32ImgH = pstImgEdgeIn->s32H;
	IMP_S32 x,y;
	IMP_U8 *pu8DataS1,*pu8DataS2;
	IMP_U8 *pu8DataD1, *pu8DataD2,*data_d3;
	IMP_S32 val11, val12, val13, val14;
	IMP_S32 val21, val22, val23, val24;
	IMP_S32 in_val11, in_val12;
	IMP_S32 in_val21, in_val22;
	IMP_S32 val31, val32, val33, val34;
	IMP_S32 val41, val42, val43, val44;
	IMP_S32 in_val31, in_val32;
	IMP_S32 in_val41, in_val42;

	IMP_S32 val15, val25, val35, val45;
	IMP_S32 val16, val26, val36, val46;
	IMP_S32 val17, val27, val37, val47;

	IMP_S32 val18, val28, val38, val48;
	for( y=0; y<s32ImgH; y++ )
	{
		pu8DataS1 = pstImgEdgeIn->pu8Data + s32ImgW*y;
		pu8DataS2 = pstImgEdgeBg->pu8Data + s32ImgW*y;
		pu8DataD1 = pstImgBgInDiff->pu8Data + s32ImgW*y;
		pu8DataD2 = pstImgLeftLife->pu8Data + s32ImgW*y;
		data_d3 = pstImgRemovedLife->pu8Data + s32ImgW*y;

#ifdef _TIDSP
	#pragma MUST_ITERATE(64,,4);
#endif
		for( x=0; x<s32ImgW; x+=4 )
		{

			val11 = pu8DataS1[0] ? 1 : 0;
			val12 = pu8DataS2[0] ? 1 : 0;

			val21 = pu8DataS1[1] ? 1 : 0;
			val22 = pu8DataS2[1] ? 1 : 0;

			val31 = pu8DataS1[2] ? 1 : 0;
			val32 = pu8DataS2[2] ? 1 : 0;

			val41 = pu8DataS1[3] ? 1 : 0;
			val42 = pu8DataS2[3] ? 1 : 0;

			in_val11 = pu8DataS1[0] ? 0 : 1;
			in_val12 = pu8DataS2[0] ? 0 : 1;

			in_val21 = pu8DataS1[1] ? 0 : 1;
			in_val22 = pu8DataS2[1] ? 0 : 1;

			in_val31 = pu8DataS1[2] ? 0 : 1;
			in_val32 = pu8DataS2[2] ? 0 : 1;

			in_val41 = pu8DataS1[3] ? 0 : 1;
			in_val42 = pu8DataS2[3] ? 0 : 1;

			val13 = ( val11 && in_val12 ); // 物品遗留
			val23 = ( val21 && in_val22 );
			val33 = ( val31 && in_val32 );
			val43 = ( val41 && in_val42 );

			val14 = ( in_val11 && val12 ); // 物品被盗
			val24 = ( in_val21 && val22 );
			val34 = ( in_val31 && val32 );
			val44 = ( in_val41 && val42 );

			val15 = val13 ? 255 : 0;
			val25 = val23 ? 255 : 0;
			val35 = val33 ? 255 : 0;
			val45 = val43 ? 255 : 0;

			val16 = val14 ? 255 : 0;
			val26 = val24 ? 255 : 0;
			val36 = val34 ? 255 : 0;
			val46 = val44 ? 255 : 0;

			val17 = pu8DataD2[0];
			val27 = pu8DataD2[1];
			val37 = pu8DataD2[2];
			val47 = pu8DataD2[3];

			pu8DataD1[0] = val15;
			pu8DataD1[1] = val25;
			pu8DataD1[2] = val35;
			pu8DataD1[3] = val45;

			val18 = val13 ? (val17+add_weight):val17-sub_weight;
			val28 = val23 ? (val27+add_weight):val17-sub_weight;
			val38 = val33 ? (val37+add_weight):val17-sub_weight;
			val48 = val43 ? (val47+add_weight):val17-sub_weight;
			val18 = max(val18,0);
			val18 = min(val18,255);
			val28 = max(val28,0);
			val28 = min(val28,255);
			val38 = max(val38,0);
			val38 = min(val38,255);
			val48 = max(val48,0);
			val48 = min(val48,255);

			pu8DataD2[0] = val18;
			pu8DataD2[1] = val28;
			pu8DataD2[2] = val38;
			pu8DataD2[3] = val48;

			pu8DataS1+=4;pu8DataS2+=4;
			pu8DataD1+=4;pu8DataD2+=4;data_d3+=4;
		}
	}
}
static IMP_VOID ipGetLeftRemovedImage(IpOscAnalyst *pstAnalyst,IpOscPara *pstPara)
{
	IMP_S32 s32LeftAccumLevel = pstPara->s32LeftAccumLevel;
	IMP_S32 s32RemovedAccumLevel = pstPara->s32RemovedAccumLevel;
	IMP_S32 s32LeftAccum = (2<<(s32LeftAccumLevel-1)) - 1;
	IMP_S32 s32RemovedAccum = (2<<(s32RemovedAccumLevel-1)) - 1;

	GRAY_IMAGE_S *pstImgLeftLife = &pstAnalyst->stImgLeftLife;
	GRAY_IMAGE_S *stImgRemovedLife = &pstAnalyst->stImgRemovedLife;
	GRAY_IMAGE_S *pstImgLeft = &pstAnalyst->stImgLeft;
	GRAY_IMAGE_S *pstImgRemoved = &pstAnalyst->stImgRemoved;

	IMP_S32 s32ImgW = pstImgLeftLife->s32W;
	IMP_S32 s32ImgH = pstImgLeftLife->s32H;
	IMP_S32 x,y;
	IMP_U8 *pu8DataS1,*pu8DataS2;
	IMP_U8 *pu8DataD1, *pu8DataD2;
	IMP_S32 val11, val12, val13;
	IMP_S32 val21, val22, val23;
	IMP_S32 val31, val32, val33;
	IMP_S32 val41, val42, val43;

	for( y=0; y<s32ImgH; y++ )
	{
		pu8DataS1 = pstImgLeftLife->pu8Data + s32ImgW*y;
		pu8DataS2 = stImgRemovedLife->pu8Data + s32ImgW*y;

		pu8DataD1 = pstImgLeft->pu8Data + s32ImgW*y;
		pu8DataD2 = pstImgRemoved->pu8Data + s32ImgW*y;

#ifdef _TIDSP
	#pragma MUST_ITERATE(64,,4);
#endif
		for( x=0; x<s32ImgW; x+=4 )
		{
			val11 = pu8DataS1[0];
			val12 = pu8DataS2[0];

			val21 = pu8DataS1[1];
			val22 = pu8DataS2[1];

			val31 = pu8DataS1[2];
			val32 = pu8DataS2[2];

			val41 = pu8DataS1[3];
			val42 = pu8DataS2[3];

			val12 = val11>=s32LeftAccum ? 255 : 0;
			val22 = val21>=s32LeftAccum ? 255 : 0;
			val32 = val31>=s32LeftAccum ? 255 : 0;
			val42 = val41>=s32LeftAccum ? 255 : 0;

			val13 = val12>=s32RemovedAccum ? 255 : 0;
			val23 = val22>=s32RemovedAccum ? 255 : 0;
			val33 = val32>=s32RemovedAccum ? 255 : 0;
			val43 = val42>=s32RemovedAccum ? 255 : 0;

			pu8DataD1[0] = val12;
			pu8DataD1[1] = val22;
			pu8DataD1[2] = val32;
			pu8DataD1[3] = val42;

			pu8DataD1[0] = val13;
			pu8DataD1[1] = val23;
			pu8DataD1[2] = val33;
			pu8DataD1[3] = val43;

			pu8DataS1+=4; pu8DataS2+=4;
			pu8DataD1+=4; pu8DataD2+=4;
		}
	}
}
static IMP_S32 ipGetObjectPixelArea( IpTrackedTarget *pstTarget )
{
	IpTargetPosition *pstPos;
	IMP_S32 i;
	IMP_S32 s32Len;
	IMP_S32 s32Area = 0;
	s32Len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	if (s32Len > 5)
		s32Len = 5;
	for (i=0; i<s32Len; i++)
	{
		pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory,-i);
		s32Area += pstPos->u32AreaPixel;
	}
	s32Area /= s32Len;

	return s32Area;
}


IMP_VOID ipPrintOscTarget( IpTrackedTarget *ttarget )
{
	PEA_DETECTED_REGION_S *pstDrg=NULL;
	int i, s32Len;
	IpTargetPosition *pstPos;
	FILE *log_file = fopen( "pstTarget.txt", "w" );

	s32Len = ipTargetTrajectoryGetLength(&ttarget->stTrajectory);
	pstPos = ipTargetTrajectoryGetPosition(&ttarget->stTrajectory, 0);

	fprintf( log_file,
		"direction	=	%d\n"
		"velocity	=	%d\n"
		"diffenergy	=	%d\n"
		"objectID	=	%d\n"
		"objStatus	=	%d\n"
		"minRect	=	(%d, %d, %d, %d)\n",
		pstPos->s32Direction,
		pstPos->s32Velocity,
		pstPos->s32DiffEnergy,
		ttarget->u32TargetId&0x0FFFFFFF,
		ttarget->stTargetInfo.u32Status,
		pstPos->stRg.s16X1, pstPos->stRg.s16Y1, pstPos->stRg.s16X2, pstPos->stRg.s16Y2
		);

	i = 0;
	fprintf(log_file, "drgtype:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->u32DrgType );
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "posx:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->stPt.s16X);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "posy:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->stPt.s16Y);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "w:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->stRg.s16X2-pstPos->stRg.s16X1);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "h:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->stRg.s16Y2-pstPos->stRg.s16Y1);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "area_p:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->u32AreaPixel);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "area_a:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->u32AreaActual);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "vel:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->s32Velocity);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "dir:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->s32Direction);
		i++;
	}
	fprintf(log_file, "\n");

	i = 0;
	fprintf(log_file, "s32DiffEnergy:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(ttarget->stTrajectory), -i);
		fprintf(log_file, "%d, ", pstPos->s32DiffEnergy);
		i++;
	}

	fprintf(log_file, "\n");

	fclose( log_file );
}

IMP_S32 ipPostProcessOscAnalysti( IpOscAnalyst *pstAnalyst )
{
	return 1;
}

//#endif
//OSC分析
//////////////////////////////////////////////////////////////////////////
