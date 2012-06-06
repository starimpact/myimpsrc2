#include "imp_pea_tracker.h"



IMP_VOID ipCreateStaticTrackerInternal( IpTargetTrackerStatic *pstTracker, GA_HARDWARE_RS_S *pstHwResource, PEA_RESULT_S *pstResult, PROCESS_TYPE_E enFlag)
{
	memset( pstTracker, 0, sizeof(IpTargetTrackerStatic) );

	pstTracker->pstHwResource = pstHwResource;
	pstTracker->pstResult = pstResult;
	pstTracker->pstPara = NULL;


	{
		IMP_S32 i;
		IpTrackedTarget *pstTarget;
		PEA_DETECTED_REGION_S *pstDRegion;
		IpTargetDataTrackerS *pstTgtData;
		IpDrgDataTrackerS *pstDrgData;
		IpTrackedTargetSet *pstTgtSet;
		PEA_DETECTED_REGIONSET_S *pstDrgSet;
		MEM_MGR_ARRAY_S *pstMemMgr = &pstHwResource->stMemMgr;

		if (enFlag == IMP_PROCESS_CHG)
		{
			pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
			pstDrgSet = &pstTracker->pstResult->stDRegionSetChg;
		}
		else if (enFlag == IMP_PROCESS_OSC)
		{
			pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
			pstDrgSet = &pstTracker->pstResult->stDRegionSetOsc;
		}
		else
		{
			pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
			pstDrgSet = &pstTracker->pstResult->stDRegionSet;
		}

		pstTracker->pstTargetDatas = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpTargetDataTrackerS)*IMP_MAX_TGT_CNT );
		pstTracker->pstDRegionDatas = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpDrgDataTrackerS)*IMP_MAX_TGT_CNT );
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
		pstTgtData = pstTracker->pstTargetDatas;
		pstDrgData = pstTracker->pstDRegionDatas;
		for ( i = 0; i < IMP_MAX_TGT_CNT; ++i )
		{
			pstTarget->stPrivateData.pDataTracker = pstTgtData;
			pstTarget->stPrivateData.u32TrackerSz = sizeof(IpTargetDataTrackerS);
			pstDRegion->stPrivateData.pDataTracker = pstDrgData;
			pstDRegion->stPrivateData.u32TrackerSz = sizeof(IpDrgDataTrackerS);
			pstTarget++; pstTgtData++;
			pstDRegion++; pstDrgData++;
		}
	}
}

IMP_VOID ipReleaseStaticTrackerInternal( IpTargetTrackerStatic *pstTracker )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &pstTracker->pstHwResource->stMemMgr;


	{
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstTracker->pstTargetDatas );
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstTracker->pstDRegionDatas );
	}

	memset( pstTracker, 0, sizeof(IpTargetTrackerStatic) );
}

IMP_VOID ipConfigStaticTrackerInternal( IpTargetTrackerStatic *pstTracker, IpTrackerPara *pstPara )
{
	pstTracker->pstPara = pstPara;
}

static IMP_VOID ipClearStaticTrackerInternal( IpTargetTrackerStatic *pstTracker )
{
	pstTracker->u32TargetCurId = 0;
	memset( pstTracker->afMatchCoeff, 0, sizeof(pstTracker->afMatchCoeff) );
}

IMP_VOID ipStartStaticTrakerInternal( IpTargetTrackerStatic *pstTracker )
{
	ipClearStaticTrackerInternal( pstTracker );
}



IMP_VOID ipSFeatureExtraction( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag);
IMP_VOID ipSPredictOldObject( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag);
IMP_VOID ipSCalculateMatchCoefficient( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag);
IMP_VOID ipSMatchRegionToObject( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag);
IMP_VOID ipSUpdatTargetInfo( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag);
IMP_VOID ipSPostTrackingProcess( IpTargetTrackerStatic *pstTracker ,PROCESS_TYPE_E enFlag);

IMP_S32 ipProcessStaticTrackerInternal( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag)
{
	PEA_STATUS_S *pstStatus = &pstTracker->pstResult->stSysStatus;
	pstStatus->u32TrackerStatus = IMP_MOUDLE_STATUS_SUCCESS;



	if( !pstTracker->pstResult->u32FrmTimePre )
		return 1;
	


	ipSFeatureExtraction( pstTracker ,enFlag);
	ipSPredictOldObject( pstTracker ,enFlag);
	ipSCalculateMatchCoefficient( pstTracker ,enFlag);
	ipSMatchRegionToObject( pstTracker ,enFlag);
	ipSUpdatTargetInfo( pstTracker ,enFlag);
	ipSPostTrackingProcess( pstTracker ,enFlag);

	return 1;
}

static IMP_VOID ipUpdateTargetFromRegionS( IpTrackedTarget *pstTTarget, IMP_S32 s32TgtIndex,
									PEA_DETECTED_REGIONSET_S *pstDrgSet, IMP_S32 s32DrgIndex );
static IMP_VOID ipNewTargetFromRegionS( IpTrackedTarget *pstTTarget, IMP_S32 s32TgtIndex,
								PEA_DETECTED_REGIONSET_S *pstDrgSet, IMP_S32 s32DrgIndex,
								IMP_U32 u32TgtId );
static IMP_VOID ipUpdateTargetFromPredictS( IpTrackedTarget *pstTTarget, IMP_S32 s32TgtIndex,
									PEA_DETECTED_REGIONSET_S *pstDrgSet,
									IMP_U32 u32Time );
static IMP_FLOAT	ipCalculateDistanceCoefficientS( const IMP_POINT_S stCurPos, const IMP_POINT_S stPredictPos, IpDrgDataTrackerS *pstDrgData );
static IMP_FLOAT	ipCalculateAreaCoefficientS( const IMP_S32 s32a, const IMP_S32 s32b );
static IMP_S32 ipAssignObjectIDS( IpTargetTrackerStatic *pstTracker );
static IMP_S32 ipCalcDiagonalLengthS( IMP_RECT_S *pstRc );
static IMP_VOID ipCalcMaxMatchDistanceS( IpTrackerPara *pstPara, IpDrgDataTrackerS *pstDrgData );
static IMP_S32 ipDeterminTargetStatus( IpTargetTrackerStatic *pstTracker, IpTrackedTarget *pstTTarget, IpTrackerPara *pstPara );
static IMP_VOID ipCheckPositionValid( IpTargetTrackerStatic *pstTracker, IpTargetPosition *pstPos );

#if !defined(_TIDSP) && !defined(IMP_ARM_PEA)
#define DEBUG_TRACKER
#endif

//extract object position and boundbox informations into pstDRegion[s32RgIndex].stPrivateData.pDataTracker
IMP_VOID ipSFeatureExtraction( IpTargetTrackerStatic *pstTracker ,PROCESS_TYPE_E enFlag)
{
	PEA_RESULT_S *pstResult = pstTracker->pstResult;

	IpTrackedTargetSet *pstTgtSet;// = &pstTracker->pstResult->stTrackedTargetSetChg;
	PEA_DETECTED_REGIONSET_S *pstDrgSet;// = &pstTracker->pstResult->stDRegionSetChg;
	IpTrackedTarget *pstTarget;// = pstTgtSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion;// = pstDrgSet->astDrg;
	IpTargetDataTrackerS *pstTgtData;
	IpDrgDataTrackerS *pstDrgData;
	IMP_S32 xlo, xhi, ylo, yhi, i;
	IMP_S32 s32IndexX, s32IndexY;
	IMP_S32 s32NoDivide;
	IMP_S32 s32SumX, s32SumY;
	IMP_S32 s32RgIndex = 1;
	IMP_S32 s32NoRg = 0;     // number of region currently processed
	IMP_U8 *pu8DatFg, *pu8DatGray;
	IMP_S32 s32Width = pstTracker->pstResult->s32Width;
//	IMP_S32 s32Height = pstTracker->pstResult->s32Height;
	IMP_S32 x, y;
	GRAY_IMAGE_S *pstImgFg;// = pstDrgSet->stImgFgRgn;
	GRAY_IMAGE_S *pstImgGray;// = pstDrgSet->stImgInGray;
	IMP_S32 s32CurrentTime = pstResult->u32FrmTimeCur;
//	IMP_S32 s32PreTime = pstResult->u32FrmTimePre;
	IMP_S32 s32FrmTime = s32CurrentTime;
	
	if (enFlag == IMP_PROCESS_CHG)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetChg;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else if (enFlag == IMP_PROCESS_OSC)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetOsc;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
		pstDrgSet = &pstTracker->pstResult->stDRegionSet;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
//printf("ipSFeatureExtraction:1\n");
	pstImgFg = pstDrgSet->pstImgFgRgn;
	pstImgGray = pstDrgSet->pstImgInGray;
	if (pstTgtSet->s32UsedTotal > 0)
	{
		static s32a = 0;
		s32a++;
	}

	for ( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( pstTarget[i].s32Used )
		{
			pstTgtData = (IpTargetDataTrackerS*)pstTarget[i].stPrivateData.pDataTracker;
			pstTgtData->fMatchCoefficient = 0;
			pstTgtData->s32MatchedWidth = 0;
			pstTgtData->s32TrackedFrameNum++;
		}
//		printf("ipSFeatureExtraction:1.1:%d\n", i);
		if ( pstDRegion[i].u8Used )
		{
			pstDrgData = (IpDrgDataTrackerS*)pstDRegion[i].stPrivateData.pDataTracker;
			memset( pstDrgData, 0, sizeof(IpDrgDataTrackerS) );
		}
	}
//printf("ipSFeatureExtraction:2\n");
	while ( (s32NoRg < pstDrgSet->s32UsedTotal) && (s32RgIndex < (IMP_MAX_TGT_CNT - 1)) )
    {
 //   	printf("ipSFeatureExtraction:3:%d\n", s32RgIndex);
        if ( pstDRegion[s32RgIndex].u8Used )
		{
			pstDrgData = (IpDrgDataTrackerS*)pstDRegion[s32RgIndex].stPrivateData.pDataTracker;


 			pstDrgData->s32CurrentTime = s32FrmTime;


			xlo = pstDRegion[s32RgIndex].stRect.s16X1;
        	xhi = pstDRegion[s32RgIndex].stRect.s16X2;
        	ylo = pstDRegion[s32RgIndex].stRect.s16Y1;

        	yhi = pstDRegion[s32RgIndex].stRect.s16Y2;
 			pstDrgData->stMinRect.s16X1 = xlo;
 			pstDrgData->stMinRect.s16Y1 = ylo;
 			pstDrgData->stMinRect.s16X2 = xhi;
 			pstDrgData->stMinRect.s16Y2 = yhi;


			s32NoDivide = 0;
	        s32SumX = s32SumY = 0;
			pu8DatFg = pstImgFg->pu8Data + ylo * s32Width;
			pu8DatGray = pstImgGray->pu8Data + ylo * s32Width;
			for ( s32IndexY = ylo; s32IndexY <= yhi; s32IndexY++ )
			{
        		for ( s32IndexX = xlo; s32IndexX <= xhi; s32IndexX++ )
				{

					if ( pu8DatFg[s32IndexX] )
					{
						s32NoDivide++;
			            s32SumX += s32IndexX;
			            s32SumY += s32IndexY;
					}
				}

				pu8DatFg += s32Width; pu8DatGray += s32Width;
			}

			x = s32SumX / s32NoDivide;
			y = s32SumY / s32NoDivide;
			pstDrgData->stCurCentroid.s16X = x>1 ? x : 1;
			pstDrgData->stCurCentroid.s16Y = y>1 ? y : 1;

			s32NoRg++;
		}
		s32RgIndex++;
	}
}

//add new position which value is equal to the last one.
IMP_VOID ipSPredictOldObject( IpTargetTrackerStatic *pstTracker ,PROCESS_TYPE_E enFlag)
{
	IpTrackedTargetSet *pstTgtSet;// = &pstTracker->pstResult->stTrackedTargetSetChg;
	IpTrackedTarget *pstTarget;// = pstTgtSet->astTargets;
	IpTargetDataTrackerS *pstTgtData;
	IMP_S32 s32Nodei = 0;
	IMP_S32 s32NoPredicted = 0;
	IpTargetPosition *pstPos1;

	if (enFlag == IMP_PROCESS_CHG)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		pstTarget = pstTgtSet->astTargets;
	}
	else if (enFlag == IMP_PROCESS_OSC)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
		pstTarget = pstTgtSet->astTargets;
	}
	else
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
		pstTarget = pstTgtSet->astTargets;
	}


	while( s32Nodei < IMP_MAX_TGT_CNT && s32NoPredicted < pstTgtSet->s32UsedTotal )
	{
		if (pstTarget[s32Nodei].s32Used)
		{
			pstTgtData = (IpTargetDataTrackerS*)pstTarget[s32Nodei].stPrivateData.pDataTracker;
			ipTargetTrajectoryNewPosition( &pstTarget[s32Nodei].stTrajectory );
			pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget[s32Nodei].stTrajectory, -1 );
			pstTgtData->stPredictedCentroid.s16X = (IMP_S16)pstPos1->stPt.s16X;
			pstTgtData->stPredictedCentroid.s16Y = (IMP_S16)pstPos1->stPt.s16Y;

			s32NoPredicted++;
		}
		s32Nodei++;
	}
}

//calc match score between region and object
IMP_VOID ipSCalculateMatchCoefficient( IpTargetTrackerStatic *pstTracker ,PROCESS_TYPE_E enFlag)
{
	IpTrackedTargetSet *pstTgtSet;// = &pstTracker->pstResult->stTrackedTargetSetChg;
	PEA_DETECTED_REGIONSET_S *pstDrgSet;// = &pstTracker->pstResult->stDRegionSetChg;
	IpTrackedTarget *pstTarget;// = pstTgtSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion;// = pstDrgSet->astDrg;
	IpTargetDataTrackerS *pstTgtData;
	IpDrgDataTrackerS *pstDrgData;
	IMP_S32 i, j, ic, jc;
	IMP_FLOAT fDistCoef, fHistoCoef, fAreaCoef, fTotalCoef;


	if (enFlag == IMP_PROCESS_CHG)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetChg;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else if (enFlag == IMP_PROCESS_OSC)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetOsc;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
		pstDrgSet = &pstTracker->pstResult->stDRegionSet;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}

	memset( pstTracker->afMatchCoeff, 0, sizeof(pstTracker->afMatchCoeff) );
	for( i = 1, ic = 0; i < (IMP_MAX_TGT_CNT - 1) && ic < (pstDrgSet->s32UsedTotal); i++ )
	{
		if( !pstDRegion[i].u8Used )
			continue;
		ic++;

//		width = pstDRegion[i].ru_x - pstDRegion[i].lb_x + 1;
//		height = pstDRegion[i].ru_y - pstDRegion[i].lb_y + 1;

		pstDrgData = (IpDrgDataTrackerS*)pstDRegion[i].stPrivateData.pDataTracker;
		ipCalcMaxMatchDistanceS( pstTracker->pstPara, pstDrgData );// something is correponded to diagonal length of rect

		for( j = 0, jc = 0; j < IMP_MAX_TGT_CNT && jc < (pstTgtSet->s32UsedTotal); j++ )
		{
			if(!pstTarget[j].s32Used)
				continue;
			jc++;

			pstTgtData = (IpTargetDataTrackerS*)pstTarget[j].stPrivateData.pDataTracker;
			fTotalCoef = 0.0;
			pstTracker->afMatchCoeff[j][i] = 0;


			fDistCoef = ipCalculateDistanceCoefficientS( pstDrgData->stCurCentroid, pstTgtData->stPredictedCentroid, pstDrgData ); //(distance/MaxMatchDistance)
			if( fDistCoef < 0 )
				continue;


			fAreaCoef = ipCalculateAreaCoefficientS( pstDRegion[i].s32AreaPixel, pstTgtData->s32ObjArea ); //area size ratio
			fAreaCoef = fAreaCoef * 3;
			if( fAreaCoef < pstTracker->pstPara->fAreaCoefMin ) //if the difference of area size if too big.
				continue;


			fHistoCoef = 1;


			fTotalCoef = (IMP_FLOAT)(pstTracker->pstPara->fDistWeight*fDistCoef + pstTracker->pstPara->fHistWeight*fHistoCoef + pstTracker->pstPara->fAreaWeight*fAreaCoef);


			pstTracker->afMatchCoeff[j][i] = fTotalCoef; //something like match score.
		}

	}
}


IMP_VOID ipSMatchRegionToObject( IpTargetTrackerStatic *pstTracker, PROCESS_TYPE_E enFlag)
{
	IpTrackedTargetSet *pstTgtSet;
	PEA_DETECTED_REGIONSET_S *pstDrgSet;
	IpTrackedTarget *pstTarget;
	PEA_DETECTED_REGION_S *pstDRegion;
	IpTargetDataTrackerS *pstTgtData;
	IpDrgDataTrackerS *pstDrgData;
	IMP_S32 s32IndexM, s32IndexN, s32MaxIndexN = 0, s32TmpIndexM, s32Cnt;
	IMP_FLOAT fMaxCoef, fTmpMaxCoef;
	IMP_S32 s32LoopTimes;

	if (enFlag == IMP_PROCESS_CHG)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetChg;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else if (enFlag == IMP_PROCESS_OSC)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetOsc;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
		pstDrgSet = &pstTracker->pstResult->stDRegionSet;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}

	for( s32IndexM = 1, s32Cnt = 0; s32IndexM < (IMP_MAX_TGT_CNT - 1) && s32Cnt<pstDrgSet->s32UsedTotal; s32IndexM++ )//此处IMP_MAX_TGT_CNT可为pstTracker->tcns->currNodeNum
	{
		if( !pstDRegion[s32IndexM].u8Used )
			continue;
		s32Cnt++;

		pstDrgData = (IpDrgDataTrackerS*)pstDRegion[s32IndexM].stPrivateData.pDataTracker;
		pstDrgData->s32MatchedWidth = 0;

		s32LoopTimes = 2;
LineLoopStart:
		fMaxCoef = 0;
		for ( s32IndexN = 0; s32IndexN < IMP_MAX_TGT_CNT; s32IndexN++ )
		{
			if ( pstTracker->afMatchCoeff[s32IndexN][s32IndexM] > fMaxCoef )
			{
				fMaxCoef = pstTracker->afMatchCoeff[s32IndexN][s32IndexM];
				s32MaxIndexN = s32IndexN;
			}
		}
		if ( fMaxCoef < pstTracker->pstPara->fMinMatchCoefHigh )
			continue;

		fTmpMaxCoef = fMaxCoef;
		for( s32TmpIndexM = 1; s32TmpIndexM < (IMP_MAX_TGT_CNT - 1); s32TmpIndexM++ )
		{
			if( pstTracker->afMatchCoeff[s32MaxIndexN][s32TmpIndexM] > fTmpMaxCoef )
			{
				pstTracker->afMatchCoeff[s32MaxIndexN][s32IndexM] = 0.0;
				if( (s32LoopTimes--) > 0)
					goto LineLoopStart;
			}
		}

		pstDrgData->s32MatchedWidth = s32MaxIndexN;

		pstTgtData = (IpTargetDataTrackerS*)pstTarget[s32MaxIndexN].stPrivateData.pDataTracker;
		pstTgtData->s32MatchedWidth = s32IndexM;
		pstTgtData->fMatchCoefficient = fMaxCoef;
	}
}

IMP_VOID ipSUpdatTargetInfo( IpTargetTrackerStatic *pstTracker ,PROCESS_TYPE_E enFlag)
{
	IpTrackedTargetSet *pstTgtSet;
	PEA_DETECTED_REGIONSET_S *pstDrgSet;
	IpTrackedTarget *pstTarget;
	IpTargetDataTrackerS *pstTgtData;
	IMP_S32 s32IndexT, s32IndexD, s32Cnt, s32CurrentTime;
	s32CurrentTime = pstTracker->pstResult->u32FrmTimeCur;
	
	if (enFlag == IMP_PROCESS_CHG)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetChg;
		pstTarget = pstTgtSet->astTargets;
	}
	else if (enFlag == IMP_PROCESS_OSC)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetOsc;
		pstTarget = pstTgtSet->astTargets;
	}
	else
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
		pstDrgSet = &pstTracker->pstResult->stDRegionSet;
		pstTarget = pstTgtSet->astTargets;
	}

	for ( s32IndexT = 0, s32Cnt = 0; s32IndexT < IMP_MAX_TGT_CNT && s32Cnt < pstTgtSet->s32UsedTotal; s32IndexT++ )
	{
		if( !pstTarget[s32IndexT].s32Used )
			continue;
		s32Cnt++;

		pstTgtData = (IpTargetDataTrackerS*)pstTarget[s32IndexT].stPrivateData.pDataTracker;
		s32IndexD = pstTgtData->s32MatchedWidth;
		if( s32IndexD != 0 )
		{
			ipUpdateTargetFromRegionS( &pstTarget[s32IndexT], s32IndexT, pstDrgSet, s32IndexD );
		}
		else
		{
			ipUpdateTargetFromPredictS( &pstTarget[s32IndexT], s32IndexT, pstDrgSet, s32CurrentTime );
		}
		{
			IpTargetPosition *pstPos;
			pstPos = ipTargetTrajectoryGetPosition( &pstTarget[s32IndexT].stTrajectory, 0 );
			ipCheckPositionValid( pstTracker, pstPos );
		}
	}
}


IMP_VOID ipSPostTrackingProcess( IpTargetTrackerStatic *pstTracker ,PROCESS_TYPE_E enFlag)
{
	IpTrackedTargetSet *pstTgtSet;
	PEA_DETECTED_REGIONSET_S *pstDrgSet;
	IpTrackedTarget *pstTarget;
	PEA_DETECTED_REGION_S *pstDRegion;
	IpDrgDataTrackerS *pstDrgData;
	IMP_S32 i, j, s32Cnt;
	IMP_S32 u32Status;

	if (enFlag == IMP_PROCESS_CHG)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetChg;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else if (enFlag == IMP_PROCESS_OSC)
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetOsc;
		pstDrgSet = &pstTracker->pstResult->stDRegionSetOsc;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}
	else
	{
		pstTgtSet = &pstTracker->pstResult->stTrackedTargetSet;
		pstDrgSet = &pstTracker->pstResult->stDRegionSet;
		pstTarget = pstTgtSet->astTargets;
		pstDRegion = pstDrgSet->astDrg;
	}


	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( !pstTarget[i].s32Used )
			continue;

//		pstTgtData = (IpTargetDataTrackerS*)pstTarget[i].stPrivateData.pDataTracker;
		u32Status = ipDeterminTargetStatus( pstTracker, &pstTarget[i], pstTracker->pstPara );
		if( IMP_TTGT_STATUS_INIT == u32Status )
		{
		}
		else if( IMP_TTGT_STATUS_STABLE == u32Status )
		{
		}
		else
		{
			ipTrackedTargetSetFree( pstTgtSet, i );
		}
	}


	for( i = 1, s32Cnt = 0; i < (IMP_MAX_TGT_CNT - 1) && s32Cnt<pstDrgSet->s32UsedTotal; i++ )
	{
		if( !pstDRegion[i].u8Used )
			continue;
		s32Cnt++;

		pstDrgData = (IpDrgDataTrackerS*)pstDRegion[i].stPrivateData.pDataTracker;
		if( pstDrgData->s32MatchedWidth == 0 )
		{
			j = ipTrackedTargetSetAlloc( pstTgtSet );
			if( j == 0 )
			{
// 				IMP_ASSERT(0);
				break;
			}
			ipNewTargetFromRegionS( &pstTarget[j], j, pstDrgSet, i, ipAssignObjectIDS(pstTracker) );
		}
	}


}

static IMP_VOID ipDetermineTargetStatusStable( IpTargetTrackerStatic *pstTracker, IpTrackedTarget *pstTTarget, IpTrackerPara *pstPara );
static IMP_S32 ipDeterminTargetStatus( IpTargetTrackerStatic *pstTracker, IpTrackedTarget *pstTTarget, IpTrackerPara *pstPara )
{
	if( IMP_IS_TGT_PREDICTED( pstTTarget->stTargetInfo.u32Status) )
	{
//		IpTrackedTargetSet *pstTgtSet = &pstTracker->pstResult->stTrackedTargetSetChg;
		IpTargetDataTrackerS *pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;
		IMP_S32 s32StableBorderWidth = pstPara->s32StableBorderWidth;
		PEA_RESULT_S *pstResult = pstTracker->pstResult;
		IMP_S32 s32CurrentTime = pstResult->u32FrmTimeCur;
		IMP_S32 s32TimeInterval = (s32CurrentTime-pstTgtData->s32LastTime) * (pstTracker->pstPara->s32FrmDura);

		if( 0
 			|| ( s32TimeInterval > pstPara->s32ChgStableOcclusionTime )
			|| ( pstTgtData->stPredictedCentroid.s16X < s32StableBorderWidth
				|| pstTgtData->stPredictedCentroid.s16X > (pstTracker->pstResult->s32Width - s32StableBorderWidth)
				|| pstTgtData->stPredictedCentroid.s16Y < s32StableBorderWidth
				|| pstTgtData->stPredictedCentroid.s16Y > (pstTracker->pstResult->s32Height - s32StableBorderWidth) )
			)
		{
			IMP_SET_TGT_DELETE( pstTTarget->stTargetInfo.u32Status );
		}
	}

	if( IMP_IS_TGT_INIT( pstTTarget->stTargetInfo.u32Status ) )
	{
		ipDetermineTargetStatusStable( pstTracker, pstTTarget, pstPara );
	}

	return IMP_GET_TGT_STATUS( pstTTarget->stTargetInfo.u32Status );
}

static IMP_VOID ipDetermineTargetStatusStable( IpTargetTrackerStatic *pstTracker, IpTrackedTarget *pstTTarget, IpTrackerPara *pstPara )
{
	IpTargetDataTrackerS *pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;
	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 s32Life;
	IMP_S32 s32FstTime, s32LastTime;

	pstPos1 = ipTargetTrajectoryGetPosition( &pstTTarget->stTrajectory, 0 );
	pstPos2 = &pstTgtData->stFirstPos;
	s32FstTime = pstPos2->u32Time;
	s32LastTime = pstPos1->u32Time;
	s32Life = abs( s32LastTime - s32FstTime ) * pstPara->s32FrmDura;

	if( IMP_IS_TGT_MEASURED( pstTTarget->stTargetInfo.u32Status) )
	{
#if 0	
		printf("id:%d s32Life:(%d, %d), pstTgtData->s32TrackedFrameNum:(%d, %d)\n", pstTTarget->u32TargetId, 
			s32Life, pstPara->s32ChgStableMinTime, pstTgtData->s32TrackedFrameNum, pstPara->s32ChgStableMinFrmCnt);
#endif
		if( s32Life >= pstPara->s32ChgStableMinTime
			&& pstTgtData->s32TrackedFrameNum >= pstPara->s32ChgStableMinFrmCnt )
		{
			IMP_SET_TGT_STABLE( pstTTarget->stTargetInfo.u32Status );
		}
	}
	else
	{
		IMP_SET_TGT_DELETE( pstTTarget->stTargetInfo.u32Status );
	}
}

static IMP_VOID ipCheckPositionValid( IpTargetTrackerStatic *pstTracker, IpTargetPosition *pstPos )
{
	IMP_RECT_S stRect;
	stRect.s16X1 = stRect.s16Y1 = 0;
	stRect.s16X2 = pstTracker->pstResult->s32Width - 1;
	stRect.s16Y2 = pstTracker->pstResult->s32Height - 1;

	if( pstPos->stPt.s16X<stRect.s16X1 ) pstPos->stPt.s16X = stRect.s16X1;
	if( pstPos->stPt.s16X>stRect.s16X2 ) pstPos->stPt.s16X = stRect.s16X2;
	if( pstPos->stPt.s16Y<stRect.s16Y1 ) pstPos->stPt.s16Y = stRect.s16Y1;
	if( pstPos->stPt.s16Y>stRect.s16Y2 ) pstPos->stPt.s16Y = stRect.s16Y2;

	if( pstPos->stRg.s16X1<stRect.s16X1 ) pstPos->stRg.s16X1 = stRect.s16X1;
	if( pstPos->stRg.s16X2>stRect.s16X2 ) pstPos->stRg.s16X2 = stRect.s16X2;
	if( pstPos->stRg.s16Y1<stRect.s16Y1 ) pstPos->stRg.s16Y1 = stRect.s16Y1;
	if( pstPos->stRg.s16Y2>stRect.s16Y2 ) pstPos->stRg.s16Y2 = stRect.s16Y2;
}



static IMP_VOID ipUpdateTargetFromRegionS( IpTrackedTarget *pstTTarget, IMP_S32 s32TgtIndex,
									PEA_DETECTED_REGIONSET_S *pstDrgSet, IMP_S32 s32DrgIndex )
{
	IpTargetPosition *pstCurPos;
	PEA_DETECTED_REGION_S *pstDRegion = &pstDrgSet->astDrg[s32DrgIndex];
	IpTargetDataTrackerS *pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;
	IpDrgDataTrackerS *pstDrgData = (IpDrgDataTrackerS*)pstDRegion->stPrivateData.pDataTracker;

	pstTgtData->s32LastTime = pstDrgData->s32CurrentTime;
	pstTgtData->stCurCentroid.s16X = pstDrgData->stCurCentroid.s16X;
	pstTgtData->stCurCentroid.s16Y = pstDrgData->stCurCentroid.s16Y;
	pstTgtData->s32ObjArea = pstDRegion->s32AreaPixel;
	pstTgtData->s32ActualArea = pstDRegion->s32AreaActual;
	pstTgtData->stMinRect = pstDrgData->stMinRect;

	pstCurPos = ipTargetTrajectoryGetPosition( &pstTTarget->stTrajectory, 0 );
	IMP_SET_TPOS_MEASURED(pstCurPos->s32Used);
	pstCurPos->u32Time = pstDrgData->s32CurrentTime;
	pstCurPos->u32AreaPixel = pstDRegion->s32AreaPixel; 
	
//	if (pstDRegion->s32AreaPixel > 500)
//	{
//		printf("(%d:L:%d, R:%d)\n", pstDRegion->s32AreaPixel, IMP_DRG_IS_STATIC_L(pstDRegion->u8Used), IMP_DRG_IS_STATIC_R(pstDRegion->u8Used));
//	}
	
	pstCurPos->u32AreaActual = pstDRegion->s32AreaActual;
	pstCurPos->u32DrgType = pstDRegion->u16Status;
	pstCurPos->stPt = pstTgtData->stCurCentroid;
	pstCurPos->stRg = pstTgtData->stMinRect;

	pstTTarget->stTargetImage.pstDRegions = pstDrgSet;
	pstTTarget->stTargetImage.s32DrgIndex = s32DrgIndex;
	IMP_SET_TGT_MEASURED( pstTTarget->stTargetInfo.u32Status );
}

static IMP_VOID ipNewTargetFromRegionS( IpTrackedTarget *pstTTarget, IMP_S32 s32TgtIndex,
								PEA_DETECTED_REGIONSET_S *pstDrgSet, IMP_S32 s32DrgIndex,
								IMP_U32 u32TgtId )
{
	PEA_DETECTED_REGION_S *pstDRegion = &pstDrgSet->astDrg[s32DrgIndex];
	IpTargetDataTrackerS *pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;
	IpDrgDataTrackerS *pstDrgData = (IpDrgDataTrackerS*)pstDRegion->stPrivateData.pDataTracker;
	IpTargetPosition *pstCurPos;

	IMP_SET_TGT_INIT( pstTTarget->stTargetInfo.u32Status );
	pstTTarget->s32Used = IMP_TRUE;
	pstTTarget->u32TargetId = u32TgtId;
	pstTgtData->s32FirstTime = pstDrgData->s32CurrentTime;
	pstTgtData->s32LastTime = pstDrgData->s32CurrentTime;
	pstTgtData->s32MatchedWidth = s32DrgIndex;
	pstTgtData->fMatchCoefficient = 1;
	pstTgtData->s32TrackedFrameNum = 1;
	ipTargetTrajectoryNewPosition( &pstTTarget->stTrajectory );

	ipUpdateTargetFromRegionS( pstTTarget, s32TgtIndex, pstDrgSet, s32DrgIndex );

	pstCurPos = ipTargetTrajectoryGetPosition( &pstTTarget->stTrajectory, 0 );
	memcpy( &pstTgtData->stFirstPos, pstCurPos, sizeof(IpTargetPosition) );
}

static IMP_VOID ipUpdateTargetFromPredictS( IpTrackedTarget *pstTTarget, IMP_S32 s32TgtIndex,
									PEA_DETECTED_REGIONSET_S *pstDrgSet,
									IMP_U32 u32Time )
{
	IMP_S32 s32OftX, s32OftY, s32Width, s32Height;
	IpTargetPosition *pstCurPos, *pstPrePos;
	IpTargetDataTrackerS *pstTgtData = (IpTargetDataTrackerS*)pstTTarget->stPrivateData.pDataTracker;

	pstTgtData->stCurCentroid.s16X = pstTgtData->stPredictedCentroid.s16X;
	pstTgtData->stCurCentroid.s16Y = pstTgtData->stPredictedCentroid.s16Y;

	pstPrePos = ipTargetTrajectoryGetPosition( &pstTTarget->stTrajectory, -1 );
	pstCurPos = ipTargetTrajectoryGetPosition( &pstTTarget->stTrajectory, 0 );
	IMP_SET_TPOS_PREDICTED(pstCurPos->s32Used);
	pstCurPos->u32Time = u32Time;
	pstCurPos->u32AreaActual = pstPrePos->u32AreaActual;
	pstCurPos->u32AreaPixel = pstPrePos->u32AreaPixel;
	pstCurPos->stPt = pstTgtData->stCurCentroid;
	s32OftX = pstPrePos->stRg.s16X1 - pstPrePos->stPt.s16X;
	s32OftY = pstPrePos->stRg.s16Y1 - pstPrePos->stPt.s16Y;
	s32Width = pstPrePos->stRg.s16X2 - pstPrePos->stRg.s16X1;
	s32Height = pstPrePos->stRg.s16Y2 - pstPrePos->stRg.s16Y1;
	pstCurPos->stRg.s16X1 = pstCurPos->stPt.s16X + s32OftX;
	pstCurPos->stRg.s16Y1 = pstCurPos->stPt.s16Y + s32OftY;
	pstCurPos->stRg.s16X2 = pstCurPos->stRg.s16X1 + s32Width;
	pstCurPos->stRg.s16Y2 = pstCurPos->stRg.s16Y1 + s32Height;
	pstTgtData->stMinRect = pstCurPos->stRg;
	pstCurPos->u32DrgType = pstPrePos->u32DrgType;

	pstTTarget->stTargetImage.pstDRegions = NULL;
	pstTTarget->stTargetImage.s32DrgIndex = 0;
	IMP_SET_TGT_PREDICTED( pstTTarget->stTargetInfo.u32Status );
}


static IMP_VOID ipCalcMaxMatchDistanceS( IpTrackerPara *pstPara, IpDrgDataTrackerS *pstDrgData )
{
	IMP_S32 s32DiagLen, s32MaxMatchDist, s32MaxDist;

	s32MaxDist = pstPara->s32MaxDistanceStatic;

	if( pstPara->s32DisCrlSize )
	{
		s32DiagLen = ipCalcDiagonalLengthS( &pstDrgData->stMinRect );
		s32MaxMatchDist = (IMP_S32)( s32DiagLen / ( 1 - pstPara->fMinMatchCoefLow ) ) * 2;
		if( s32MaxMatchDist > s32MaxDist )
			s32MaxMatchDist = s32MaxDist;
	}
	else
	{
		s32MaxMatchDist = s32MaxDist;
	}
	pstDrgData->s32MaxMatchDist = s32MaxMatchDist;
}

static IMP_FLOAT ipCalculateDistanceCoefficientS( const IMP_POINT_S stCurPos, const IMP_POINT_S stPredictPos, IpDrgDataTrackerS *pstDrgData )
{
	IMP_FLOAT fDist;
	IMP_S32 s32MaxDist = pstDrgData->s32MaxMatchDist;

	fDist = (IMP_FLOAT)ipSqrtInt((IMP_U32)( (stCurPos.s16X - stPredictPos.s16X) * (stCurPos.s16X - stPredictPos.s16X)
					   +(stCurPos.s16Y - stPredictPos.s16Y) * (stCurPos.s16Y - stPredictPos.s16Y) ));
	return (IMP_FLOAT)( 1 - fDist / s32MaxDist );
}


static IMP_FLOAT ipCalculateAreaCoefficientS( const IMP_S32 s32a, const IMP_S32 s32b )
{
	IMP_FLOAT fRatioAB;
	fRatioAB = s32a > s32b ? (((IMP_FLOAT)s32b) / s32a) : (((IMP_FLOAT)s32a) / s32b);
	return fRatioAB;
}

#define TARGET_ID_MIN_STATIC	IMP_TGT_ID_NICE_MIN_STATIC
#define TARGET_ID_MAX_STATIC	IMP_TGT_ID_NICE_MAX_STATIC

static IMP_S32 ipAssignObjectIDS( IpTargetTrackerStatic *pstTracker )
{
	pstTracker->u32TargetCurId++;
	if( pstTracker->u32TargetCurId < TARGET_ID_MIN_STATIC || pstTracker->u32TargetCurId > TARGET_ID_MAX_STATIC )
		pstTracker->u32TargetCurId = TARGET_ID_MIN_STATIC;
	return pstTracker->u32TargetCurId;
}

static IMP_S32 ipCalcDiagonalLengthS( IMP_RECT_S *pstRc )
{
	IMP_S32 s32Diagonal;
	IMP_S32 s32TmpX, s32TmpY;

	s32TmpX = pstRc->s16X2 - pstRc->s16X1;
	s32TmpY = pstRc->s16Y2 - pstRc->s16Y1;
	s32Diagonal = ipSqrtInt( s32TmpX*s32TmpX + s32TmpY*s32TmpY );

	return s32Diagonal;
}
