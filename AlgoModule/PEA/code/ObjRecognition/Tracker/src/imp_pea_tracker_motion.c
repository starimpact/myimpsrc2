#include "imp_pea_tracker.h"


IMP_VOID ipCreateMotionTrackerInternal( IpTargetTrackerMotion *pstTracker, GA_HARDWARE_RS_S *pstHwResource, PEA_RESULT_S *pstResult )
{
	IMP_S32 s32ImgW, s32ImgH;
	memset( pstTracker, 0, sizeof(IpTargetTrackerMotion) );

	pstTracker->pstHwResource = pstHwResource;
	pstTracker->pstResult = pstResult;
	pstTracker->pstPara = NULL;
	s32ImgW = pstResult->s32Width;
	s32ImgH = pstResult->s32Height;

	// TARDAT
	{
		IpTrackedTargetSet *pstTargetSet = &pstResult->stTrackedTargetSet;
		PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstResult->stDRegionSet;
		IMP_S32 i;
		IpTrackedTarget *pstTarget;
		PEA_DETECTED_REGION_S *pstDRegion;
		IpTargetDataTrackerM *pstTargetData;
		IpDrgDataTrackerM *pstDrgData;
//		IpTargetSetDataTrackerM *pstTargetSetData;
		MEM_MGR_ARRAY_S *pstMemMgr = &pstHwResource->stMemMgr;
		IMP_S32 s32GroupSetSize = sizeof(IpTargetSetDataTrackerM);
		IMP_S32 s32TarDateSize = sizeof(IpTargetDataTrackerM) * IMP_MAX_TGT_CNT;
		IMP_S32 s32DrgDatSize = sizeof(IpDrgDataTrackerM) * IMP_MAX_TGT_CNT;
		pstTracker->pstTargetSetData = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpTargetSetDataTrackerM) );
		IMP_ASSERT(pstTracker->pstTargetSetData);
		pstTracker->pstTargetDatas = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpTargetDataTrackerM)*IMP_MAX_TGT_CNT );
		IMP_ASSERT(pstTracker->pstTargetDatas);
		pstTracker->pstDRegionDatas = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpDrgDataTrackerM)*IMP_MAX_TGT_CNT );
		IMP_ASSERT(pstTracker->pstDRegionDatas);

		pstTarget = pstTargetSet->astTargets;
		pstDRegion = pstRegionSet->astDrg;
		pstTargetData = pstTracker->pstTargetDatas;
		pstDrgData = pstTracker->pstDRegionDatas;
		for ( i = 0; i < IMP_MAX_TGT_CNT; ++i )
		{
			pstTarget->stPrivateData.pDataTracker = pstTargetData;
			pstTarget->stPrivateData.u32TrackerSz = sizeof(IpTargetDataTrackerM);
			pstDRegion->stPrivateData.pDataTracker = pstDrgData;
			pstDRegion->stPrivateData.u32TrackerSz = sizeof(IpDrgDataTrackerM);
			pstTarget++; pstTargetData++;
			pstDRegion++; pstDrgData++;
		}

		pstResult->stTrackedTargetSet.stPrivateData.pDataTracker = pstTracker->pstTargetSetData;
		pstResult->stTrackedTargetSet.stPrivateData.u32TrackerSz = sizeof(IpTargetSetDataTrackerM);

		IMP_GrayImageCreate(&pstTracker->pstImgInGrayLast,s32ImgW,s32ImgH,pstMemMgr);
		IMP_GrayImageCreate(&pstTracker->pstImgFgRgnLast,s32ImgW,s32ImgH,pstMemMgr);
		IMP_GrayImageSet(&pstTracker->pstImgInGrayLast,0);
		IMP_GrayImageSet(&pstTracker->pstImgFgRgnLast,0);


	}
}

IMP_VOID ipReleaseMotionTrackerInternal( IpTargetTrackerMotion *pstTracker )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &pstTracker->pstHwResource->stMemMgr;

	// TARDAT
	{
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstTracker->pstTargetSetData );
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstTracker->pstTargetDatas );
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstTracker->pstDRegionDatas );

		IMP_GrayImageDestroy(&pstTracker->pstImgInGrayLast,pstMemMgr);
		IMP_GrayImageDestroy(&pstTracker->pstImgFgRgnLast,pstMemMgr);


	}

	memset( pstTracker, 0, sizeof(IpTargetTrackerMotion) );
}

IMP_VOID ipConfigMotionTrackerInternal( IpTargetTrackerMotion *pstTracker, IpTrackerPara *pstPara )
{
	pstTracker->pstPara = pstPara;
}

static IMP_VOID ipClearMotionTrackerInternal( IpTargetTrackerMotion *pstTracker )
{
	pstTracker->u32TargetCurId = 0;
	memset( pstTracker->afMatchCoeff, 0, sizeof(pstTracker->afMatchCoeff) );
	memset( pstTracker->pstTargetSetData, 0, sizeof(IpTargetSetDataTrackerM) );
	memset( pstTracker->pstTargetDatas, 0, sizeof(IpTargetDataTrackerM)*IMP_MAX_TGT_CNT );
	memset( pstTracker->pstDRegionDatas, 0, sizeof(IpDrgDataTrackerM)*IMP_MAX_TGT_CNT );
}

IMP_VOID ipStartMotionTrackerInternal( IpTargetTrackerMotion *pstTracker )
{
	ipClearMotionTrackerInternal( pstTracker );
}



IMP_VOID ipMTrackerFeatureExtraction( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerPredictOldObject( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerProcessFeaturesInitially( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerProcessCollisionSplit( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerCalculateMatchCoefficient( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerMatchRegionToTarget( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerMotionMeanshiftTraker( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerTargetInfoUpdate( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerPostProcess( IpTargetTrackerMotion *pstTracker );

IMP_S32 ipProcessMotionTrackerInternal( IpTargetTrackerMotion *pstTracker )
{
	PEA_STATUS_S *pstStatus = &pstTracker->pstResult->stSysStatus;
	pstStatus->u32TrackerStatus = IMP_MOUDLE_STATUS_SUCCESS;

	if( !pstTracker->pstResult->u32FrmTimePre )
		return 1;

	ipMTrackerFeatureExtraction( pstTracker );
	ipMTrackerPredictOldObject( pstTracker );
	if( pstTracker->pstPara->s32MergeDRegion )
 		ipMTrackerProcessFeaturesInitially( pstTracker );
	if( pstTracker->pstPara->s32CsCollisionSplit )
		ipMTrackerProcessCollisionSplit( pstTracker );
	ipMTrackerCalculateMatchCoefficient( pstTracker );
	ipMTrackerMatchRegionToTarget( pstTracker );

	ipMTrackerTargetInfoUpdate( pstTracker );
	ipMTrackerPostProcess( pstTracker );
	
	IMP_GrayImageClone( pstTracker->pstResult->stDRegionSet.pstImgInGray, &pstTracker->pstImgInGrayLast );
	IMP_GrayImageClone( pstTracker->pstResult->stDRegionSet.pstImgFgRgn, &pstTracker->pstImgFgRgnLast );


	return 1;
}



#define ALPHA_AREA_COEF_PLUS  1.1
#define ALPHA_AREA_COEF_MINUS 0.55
#define STABLE_COLLISION_IMP_MAX_TIME 5


#ifdef DEBUG_OBJRECG_TRACKER_INFO2
#include "windows.h"
#endif

static IMP_VOID ipUpdateTargetFromRegionM( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
									PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex, IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipUpdateTargetFromMS( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
										  PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex, IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipUpdateTargetFromCollision( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
									PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex, IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipUpdateTargetFromSplit( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
										PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex, IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipNewTargetFromRegionM( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
								PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex,
								IMP_U32 u32TargetId ,IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipUpdateTargetFromCollisionStable( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipUpdateTargetFromCollisionDestroy( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,IpTargetTrackerMotion *pstTracker);
//static IMP_VOID ipUpdateAbnormalCollisionTarget( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex, IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipUpdateTargetFromPredictM( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
									PEA_DETECTED_REGIONSET_S *pstRegionSet,
									IMP_U32 u32Time );
extern IMP_S32 ipCsHistFeatureValidJudge(IpDrgDataTrackerM *pstDrgDat1,IpDrgDataTrackerM *pstDrgDat2,IpTargetDataTrackerM *pstTarData1,IpTargetDataTrackerM *pstTarData2);
extern IMP_S32 ipCalculateAngleofVector(IMP_POINT_S stStart,IMP_POINT_S stEnd);
static IMP_FLOAT ipCalculateDistanceCoefficientM( const IMP_POINT_S stCurPos, const IMP_POINT_S stPredictPos, IpDrgDataTrackerM *pstDrgData );
extern IMP_FLOAT ipCalculateHistoCoefficient( const IMP_U32 *pu32CurObj, const IMP_U32 *pu32TrackObj, IMP_S32 s32GrayLevel );
extern IMP_FLOAT ipCalculateHistoCoefficientRGB( const IMP_U32 *pu32CurObj, const IMP_U32 *pu32TrackObj, IMP_S32 s32RgbLevel );
static IMP_FLOAT ipCalculateHistoCoefficientMoments( IpDrgDataTrackerM *pstDrgData,  IpTargetDataTrackerM *pstTargetData);
extern IMP_FLOAT ipCalculateAreaCoefMinus(IMP_S32 s32AreaT,IMP_S32 s32AreaD,IpGroup* pstGroup);
extern IMP_FLOAT ipCalculateAreaCoefficientM( const IMP_S32 s32a, const IMP_S32 s32b );
extern IMP_VOID ipCalculateTrendCoefficient(IMP_POINT_S stPtT1,IMP_S32 s32IdxT1,
									  IMP_POINT_S stPtT2,IMP_S32 s32IdxT2,
									  IMP_POINT_S stPtM1,IMP_S32 s32IdxM1,
									  IMP_POINT_S stPtM2,IMP_S32 s32IdxM2,
									  IMP_S32 s32DirT1, IMP_S32 s32DirT2,
									  IMP_FLOAT afMatchCoeff[][IMP_MAX_TGT_CNT]);
extern IMP_VOID ipCalculateHistoCoefficientBalance(IpDrgDataTrackerM *pstDrgDat1,IMP_S32 s32IdxM1,
											 IpDrgDataTrackerM *pstDrgDat2,IMP_S32 s32IdxM2,
											 IpTargetDataTrackerM *pstTarData1,IMP_S32 s32IdxT1,
											 IpTargetDataTrackerM *pstTarData2,IMP_S32 s32IdxT2,
											 IMP_FLOAT afMatchCoeff[][IMP_MAX_TGT_CNT]);


static IMP_S32	ipAssignObjectIDM( IpTargetTrackerMotion *pstTracker );
extern IMP_VOID	ipPointCopy( const IMP_POINT_S *pstSrc, IMP_POINT_S *pstDst );
extern IMP_VOID	ipRectCopy( const IMP_RECT_S *pstSrc, IMP_RECT_S *pstDst );
static IMP_VOID	ipCalculateObjVelocityAndDirection( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, const IMP_S32 s32CurrentTime );
extern IMP_S32	ipCalcDiagonalLengthM( IMP_RECT_S *pstRect );
static IMP_VOID ipCalcMaxMatchDistanceM( IpTrackerPara *pstPara, IpDrgDataTrackerM *pstDrgData );
static IMP_S32 ipCalcDirectionDistCoef( const IMP_POINT_S stPtT, const IMP_POINT_S stPtM, IpDrgDataTrackerM *pstDrgData, IMP_S32 stDirT );
static IMP_S32 ipDeterminTargetStatusS( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara );
static IMP_VOID ipCheckPositionValidM( IpTargetTrackerMotion *pstTracker, IpTargetPosition *pstPos );
static IMP_VOID ipCorrectCollisionTrajectory(IpTrackedTarget *pstTarget,PEA_DETECTED_REGIONSET_S *pstRegionSet);
static IMP_VOID ipCorrectCollisionTrajectory2(IpTrackedTarget *pstTarget,PEA_DETECTED_REGIONSET_S *pstRegionSet);
static IMP_VOID ipCalcTargetVelocity( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget );
static IMP_VOID ipMergeSimilarTargets( IpTargetTrackerMotion *pstTracker );

static IMP_VOID ipGroupClear(IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipGroupUpdateRegister(IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipGroupAddMember(IpTargetTrackerMotion *pstTracker,IMP_S32 s32Frame,IMP_U32 u32TgtId,IMP_S32 s32GroupId,IMP_S32 s32MemberId);//测试使用，组成员强制编制
static IMP_VOID ipGroupDeleteMember(IpTargetTrackerMotion *pstTracker,IMP_S32 s32Frame,IMP_S32 s32GroupId,IMP_S32 s32MemberId);			  //测试使用，组成员强制开除
static IMP_VOID ipGroupTest(IpTargetTrackerMotion *pstTracker);
static IMP_VOID ipGroupUpdateAreaInfo(IpTargetTrackerMotion *pstTracker);
#ifdef DEBUG_OBJRECG_TRACKER_INFO2
static IMP_VOID ipGroupInfoDisplay(IpTargetTrackerMotion *pstTracker);
#endif

static IMP_VOID ipSmoothTrajectory(IpTrackedTarget *pstTarget, IpTrackerPara *pstPara);
static IMP_VOID ipSmoothTrajectory2(IpTrackedTarget *pstTarget, IpTrackerPara *pstPara);
static IMP_VOID ipSmoothTrajectory3(IpTrackedTarget *pstTarget, IpTrackerPara *pstPara);


#if !defined(_TIDSP) && !defined(IMP_ARM_PEA)
#define DEBUG_TRACKER
#endif

#ifdef DEBUG_TRACKER
extern IMP_VOID print_target( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget );
#define PRINT_TARGET(x,y)
#else
#define PRINT_TARGET(x,y)
#endif
#define PRINT_TARGET(x,y)
//#ifdef DEBUG_TRACKER
//extern IMP_VOID print_target2( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget );
//#define PRINT_TARGET2(x,y) print_target2(x,y)
//#else
//#define PRINT_TARGET2(x,y)
//#endif
#define FPRECISE 256



static ipCalcDRegionFeature(const RGB_IMAGE_S *pstRgbImage,const GRAY_IMAGE_S *pstFgImage,IpDrgDataTrackerM*pstDrgData,IpTargetTrackerMotion *pstTracker);


IMP_VOID ipMTrackerFeatureExtraction( IpTargetTrackerMotion *pstTracker )
{
	PEA_RESULT_S *pstResult = pstTracker->pstResult;
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetDataTrackerM *pstTargetData;
	IpDrgDataTrackerM *pstDrgData;
	IMP_S32 xlo, xhi, ylo, yhi, i;

	IMP_S32 s32RgIndex = 1;
	IMP_S32 s32NoRg = 0;

	IMP_S32 s32ImgW = pstTracker->pstResult->s32Width;
//	IMP_S32 s32ImgH = pstTracker->pstResult->s32ImgH;
#ifndef TRACKER_USE_RGB_INFO
	IMP_S32 x, y;
	IMP_U32 *au32ObjectHistogram;
	IMP_S32 s32SumX, s32SumY;
	IMP_U8	*pu8DatFg;
	IMP_U8 u8GrayLevel, *pu8DatGray;
	IMP_S32 s32XIndex, s32YIndex;
	IMP_S32 s32NoDivide;
#endif
	GRAY_IMAGE_S *pstFgImage = pstResult->stDRegionSet.pstImgFgRgn;
	GRAY_IMAGE_S *pstImgGray = pstResult->stDRegionSet.pstImgInGray;
	GRAY_IMAGE_S *pstImgDiff = pstResult->stDRegionSet.pstImgFrmDiff;
	IMP_S32 s32CurrentTime = pstResult->u32FrmTimeCur;
	IMP_S32 s32FrmTime = s32CurrentTime;


	for ( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( pstTarget[i].s32Used )
		{
			pstTargetData = (IpTargetDataTrackerM*)pstTarget[i].stPrivateData.pDataTracker;
			pstTargetData->fMatchCoefficient = 0;
			pstTargetData->s32MatchedWith = 0;
			pstTargetData->s32TrackedFrameNum++;
		}

		if ( pstDRegion[i].u8Used )
		{
			pstDrgData = (IpDrgDataTrackerM*)pstDRegion[i].stPrivateData.pDataTracker;
			memset( pstDrgData, 0, sizeof(IpDrgDataTrackerM) );

// 			printf("+++ Ti=%d,sn=%d\n",i,pstDRegion[i].u8Sign);
// 			IMP_ASSERT(i==pstDRegion[i].u8Sign);
		}
	}

	while ( (s32NoRg < pstRegionSet->s32UsedTotal) && (s32RgIndex < (IMP_MAX_TGT_CNT - 1)) )
    {
        if ( pstDRegion[s32RgIndex].u8Used )
		{
			pstDrgData = (IpDrgDataTrackerM*)pstDRegion[s32RgIndex].stPrivateData.pDataTracker;


 			pstDrgData->s32CurrentTime = s32FrmTime;
			pstDrgData->u16Status = pstDRegion[s32RgIndex].u16Status;


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
			au32ObjectHistogram = pstDrgData->au32ObjectHistogram;
			pu8DatFg = pstFgImage->pu8Data + ylo*s32ImgW;
			pu8DatGray = pstImgGray->pu8Data + ylo*s32ImgW;
			for ( s32YIndex = ylo; s32YIndex <= yhi; s32YIndex++ )
			{
        		for ( s32XIndex = xlo; s32XIndex <= xhi; s32XIndex++ )
				{
					// 像素点属于本区域则统计坐标与灰度
					if ( pu8DatFg[s32XIndex] )
					{   // 计算坐标
						s32NoDivide++;
			            s32SumX += s32XIndex;
			            s32SumY += s32YIndex;
                        // 统计灰度直方图
						u8GrayLevel = pu8DatGray[s32XIndex];
						u8GrayLevel = u8GrayLevel>>IMP_GRAY_LEVEL_B;
		                au32ObjectHistogram[u8GrayLevel]++;  //mzhang, statistic image histogram
					}
				}

				pu8DatFg += s32ImgW; pu8DatGray += s32ImgW;
			}
			// 计算区域质心
			if( pstTracker->pstPara->s32TrajectUseBottom == 1 )
			{
				pstDrgData->stCurCentroid.s16X = ((xlo + xhi) >> 1);
				pstDrgData->stCurCentroid.s16Y = yhi;
			}
			else
			{
				if (s32NoDivide)
				{
					x = s32SumX / s32NoDivide;
					y = s32SumY / s32NoDivide;
					pstDrgData->stCurCentroid.s16X = x > 1 ? x : 1;
					pstDrgData->stCurCentroid.s16Y = y > 1 ? y : 1;
				}

			}

			s32NoRg++;
		}//the stEnd of if ( pstDRegion[s32RgIndex].s32Used )
		s32RgIndex++;
	}//the stEnd of while
}



IMP_VOID ipMTrackerPredictOldObject( IpTargetTrackerMotion *pstTracker )
{
	PEA_RESULT_S *pstResult = pstTracker->pstResult;
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetDataTrackerM *pstTargetData;
	IMP_S32 s32Nodei = 0;
	IMP_S32 s32NoPredicted = 0;
	IpTargetPosition *pstPos1;

	IMP_FLOAT fVelocityX = 0,fVelocityY = 0;
	IMP_S32 iii = 0;
	IMP_S32 s32CurrentTime = pstResult->u32FrmTimeCur;
	IMP_S32 s32PreTime = pstResult->u32FrmTimePre;
	IMP_S32 s32TimeSlot = (s32CurrentTime - s32PreTime) * pstTracker->pstPara->s32FrmDura;

	while( s32Nodei < IMP_MAX_TGT_CNT && s32NoPredicted < pstTargetSet->s32UsedTotal )
	{
		if (pstTarget[s32Nodei].s32Used)
		{
			pstTargetData = (IpTargetDataTrackerM*)pstTarget[s32Nodei].stPrivateData.pDataTracker;
			ipTargetTrajectoryNewPosition( &pstTarget[s32Nodei].stTrajectory );
//			pstPos0 = ipTargetTrajectoryGetPosition( &pstTarget[s32Nodei].stTrajectory, 0 );
			pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget[s32Nodei].stTrajectory, -1 );

			//FDEPTH_get_point_resolution( &pstTracker->pstPara->scene_model->calibration->fdepth, pstPos1->stPt.s16X, pstPos1->stPt.s16Y,
			//							&dResX, &dResY );
			//ptResolusion = dResX;
			//fAngle = pstPos1->s32Direction*IMP_PI/180;
			//s32Velocity = pstPos1->s32Velocity/1000.0;
			fVelocityX = pstTargetData->fVelocityX;
			fVelocityY = pstTargetData->fVelocityY;

			pstTargetData->fPredictedIncrementX = (IMP_FLOAT)(fVelocityX * s32TimeSlot);
			pstTargetData->fPredictedIncrementY = (IMP_FLOAT)(fVelocityY * s32TimeSlot);

//			pstTargetData->fPredictedIncrementX = (IMP_FLOAT)(s32Velocity/ptResolusion*s32TimeSlot*cos(fAngle));
//			pstTargetData->fPredictedIncrementY = (IMP_FLOAT)(s32Velocity/ptResolusion*s32TimeSlot*sin(fAngle));


			if (IMP_IS_TGT_BLOB_MEASURED( pstTarget[s32Nodei].stTargetInfo.u32Status )

				)
			{
				pstTargetData->fPredictedCentroidX = pstPos1->stPt.s16X + pstTargetData->fPredictedIncrementX;
				pstTargetData->fPredictedCentroidY = pstPos1->stPt.s16Y + pstTargetData->fPredictedIncrementY;
			}
			else
			{
				pstTargetData->fPredictedCentroidX += pstTargetData->fPredictedIncrementX;
				pstTargetData->fPredictedCentroidY += pstTargetData->fPredictedIncrementY;

			}

			pstTargetData->stPredictedCentroid.s16X = (IMP_S16)(pstTargetData->fPredictedCentroidX + 0.5);
			pstTargetData->stPredictedCentroid.s16Y = (IMP_S16)(pstTargetData->fPredictedCentroidY + 0.5);

			s32NoPredicted++;

#ifdef DEBUG_TRACKER
			{
				IMP_S32 width,height,length,dx,dy,r2;
				width = pstTargetData->stMinRect.s16X2 - pstTargetData->stMinRect.s16X1 + 1;
				height = pstTargetData->stMinRect.s16Y2 - pstTargetData->stMinRect.s16Y1 + 1;
				dx = pstTargetData->stPredictedCentroid.s16X-pstTargetData->stCurCentroid.s16X;
				dy = pstTargetData->stPredictedCentroid.s16Y-pstTargetData->stCurCentroid.s16Y;
				r2 = dx*dx+dy*dy;
				length = (IMP_S32)ipSqrtInt(r2);
// 				IMP_ASSERT((IMP_FLOAT)(length)/ipCalcDiagonalLengthM(&pstTargetData->stMinRect)<1.0);
			}
#endif

		}
		s32Nodei++;
	}
}

extern IMP_VOID ipCsGroupUpdateRegister(IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsGroupClearFlag (IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsDetermineCollision(IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsDetermineSplit (IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsUniformTgtStatusInSplitGroup(IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsCollisionStableProcess(IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsGroupUpdateAreaInfo(IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsGroupUpdateRectInfo(IpTargetTrackerMotion *pstTracker);
extern IMP_VOID ipCsGroupInfoDisplay(IpTargetTrackerMotion *pstTracker);

IMP_VOID ipMTrackerProcessCollisionSplit( IpTargetTrackerMotion *pstTracker )
{

	ipCsGroupUpdateRegister(pstTracker);


	ipCsGroupClearFlag(pstTracker);


	ipCsDetermineCollision(pstTracker);


    ipCsDetermineSplit(pstTracker);


	ipCsUniformTgtStatusInSplitGroup(pstTracker);


	ipCsCollisionStableProcess(pstTracker);


	ipCsGroupUpdateAreaInfo(pstTracker);



}


IMP_VOID ipMTrackerProcessFeaturesInitially(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetDataTrackerM *pstTargetData;
	IpDrgDataTrackerM *pstDrgData;
	IMP_S32 i, j, k, l, ic, jc;
	IMP_S32 s32Dist;
	IMP_S32 s32IntersectArea = 0;

	IMP_S32 s32SplitNum, s32Cnt, s32MergedId;
	IMP_S32 cols, rows;
	IMP_U8 au8SplitMember[IMP_MAX_TGT_CNT];
	IMP_U8 *pu8Data;
	IMP_FLOAT fAreaRatio;
	IMP_RECT_S stTmpRg;
	IpDrgDataTrackerM *pstTmpDrgData, *pstMergedDrgData;

	for ( i = 0, ic = 0; i < IMP_MAX_TGT_CNT && ic < (pstTargetSet->s32UsedTotal); i++ )
	{
		if( !pstTarget[i].s32Used )
			continue;
		ic++;

		if( IMP_IS_TGT_MEASURED(pstTarget[i].stTargetInfo.u32Status) )
		{
			pstTargetData = (IpTargetDataTrackerM*)pstTarget[i].stPrivateData.pDataTracker;
			stTmpRg.s16X1 = pstTargetData->stMinRect.s16X1 - 2;
			if (stTmpRg.s16X1 < 0)
			{
				stTmpRg.s16X1 = 0;
			}
			stTmpRg.s16Y1 = pstTargetData->stMinRect.s16Y1 - 2;
			if (stTmpRg.s16Y1 < 0)
			{
				stTmpRg.s16Y1 = 0;
			}
			stTmpRg.s16X2 = pstTargetData->stMinRect.s16X2 + 2;
			if (stTmpRg.s16X2 > pstTracker->pstResult->stDRegionSet.pstImgInGray->s32W)
			{
				stTmpRg.s16X2 = pstTracker->pstResult->stDRegionSet.pstImgInGray->s32W - 1;
			}
			stTmpRg.s16Y2 = pstTargetData->stMinRect.s16Y2 + 2;
			if (stTmpRg.s16Y2 > pstTracker->pstResult->stDRegionSet.pstImgInGray->s32H)
			{
				stTmpRg.s16Y2 = pstTracker->pstResult->stDRegionSet.pstImgInGray->s32H - 1;
			}
//			s16X1=pstTracker->s32ImgW, s16Y1=pstTracker->s32ImgH, s16X2=0, s16Y2=0;

			memset(au8SplitMember, 0, IMP_MAX_TGT_CNT);
			s32SplitNum = 0;
			for ( j = 1, jc = 0; j < (IMP_MAX_TGT_CNT - 1) && jc < (pstRegionSet->s32UsedTotal); j++ )
			{
				if( !pstDRegion[j].u8Used )
					continue;
				jc++;

				pstDrgData = (IpDrgDataTrackerM*)pstDRegion[j].stPrivateData.pDataTracker;
				s32Dist = ipGetMinimumRegionDistance(&(pstDrgData->stMinRect), &(stTmpRg), &s32IntersectArea);
				if ( s32Dist == 0 )
				{
					fAreaRatio = (IMP_FLOAT)s32IntersectArea/((pstDrgData->stMinRect.s16X2 - pstDrgData->stMinRect.s16X1 + 1) * (pstDrgData->stMinRect.s16Y2-pstDrgData->stMinRect.s16Y1+1));
					if ( fAreaRatio > 0.4 )
					{
						au8SplitMember[s32SplitNum++] = j;
					}
				}

			}
			if ( s32SplitNum > 1 )
			{
				for ( k = 0; k < s32SplitNum; k++ )
				{
					pstDrgData = (IpDrgDataTrackerM*)pstDRegion[au8SplitMember[k]].stPrivateData.pDataTracker;
					s32Cnt = 0;
					for ( l = 0; l < s32SplitNum; l++ )
					{
						if ( k != l )
						{
							pstTmpDrgData = (IpDrgDataTrackerM*)pstDRegion[au8SplitMember[l]].stPrivateData.pDataTracker;
							s32Dist = ipGetMinimumRegionDistance(&(pstDrgData->stMinRect), &(pstTmpDrgData->stMinRect), &s32IntersectArea);
							if ( s32Dist > 8 )
							{
								s32Cnt++;
							}
						}
					}
					if ( s32Cnt == s32SplitNum - 1 )
					{
						au8SplitMember[k] = 0;
					}
				}

				pstMergedDrgData = NULL;
				for ( k = 1; k < (IMP_MAX_TGT_CNT - 1); k++)
				{
					if(au8SplitMember[k] != 0)
					{
						if ( pstMergedDrgData == NULL )
						{
							pstMergedDrgData = (IpDrgDataTrackerM*)pstDRegion[au8SplitMember[k]].stPrivateData.pDataTracker;
							s32MergedId = au8SplitMember[k];
							continue;
						}

						pstDrgData = (IpDrgDataTrackerM*)pstDRegion[au8SplitMember[k]].stPrivateData.pDataTracker;
						pstDRegion[s32MergedId].s32AreaActual += pstDRegion[au8SplitMember[k]].s32AreaActual;
						pstDRegion[s32MergedId].s32AreaPixel += pstDRegion[au8SplitMember[k]].s32AreaPixel;
						pstDRegion[s32MergedId].stRect.s16X1 = pstDRegion[s32MergedId].stRect.s16X1<pstDRegion[au8SplitMember[k]].stRect.s16X1?pstDRegion[s32MergedId].stRect.s16X1:pstDRegion[au8SplitMember[k]].stRect.s16X1;
						pstDRegion[s32MergedId].stRect.s16Y1 = pstDRegion[s32MergedId].stRect.s16Y1<pstDRegion[au8SplitMember[k]].stRect.s16Y1?pstDRegion[s32MergedId].stRect.s16Y1:pstDRegion[au8SplitMember[k]].stRect.s16Y1;
						pstDRegion[s32MergedId].stRect.s16X2 = pstDRegion[s32MergedId].stRect.s16X2>pstDRegion[au8SplitMember[k]].stRect.s16X2?pstDRegion[s32MergedId].stRect.s16X2:pstDRegion[au8SplitMember[k]].stRect.s16X2;
						pstDRegion[s32MergedId].stRect.s16Y2 = pstDRegion[s32MergedId].stRect.s16Y2>pstDRegion[au8SplitMember[k]].stRect.s16Y2?pstDRegion[s32MergedId].stRect.s16Y2:pstDRegion[au8SplitMember[k]].stRect.s16Y2;
						pstMergedDrgData->stCurCentroid.s16X = (pstDRegion[s32MergedId].stRect.s16X1 + pstDRegion[s32MergedId].stRect.s16X2)/2;
						pstMergedDrgData->stCurCentroid.s16Y = (pstDRegion[s32MergedId].stRect.s16Y1 + pstDRegion[s32MergedId].stRect.s16Y2)/2;
						pstMergedDrgData->stMinRect.s16X1 = pstDRegion[s32MergedId].stRect.s16X1;
						pstMergedDrgData->stMinRect.s16Y1 = pstDRegion[s32MergedId].stRect.s16Y1;
						pstMergedDrgData->stMinRect.s16X2 = pstDRegion[s32MergedId].stRect.s16X2;
						pstMergedDrgData->stMinRect.s16Y2 = pstDRegion[s32MergedId].stRect.s16Y2;
						pstDRegion[au8SplitMember[k]].u8Used = IMP_FALSE;
						for ( l = 0; l < IMP_GRAY_LEVEL; l++ )
						{
							pstMergedDrgData->au32ObjectHistogram[l] = pstMergedDrgData->au32ObjectHistogram[l] + pstDrgData->au32ObjectHistogram[l];
						}
						pstRegionSet->s32UsedTotal--;
						for (rows=pstDRegion[au8SplitMember[k]].stRect.s16Y1; rows<pstDRegion[au8SplitMember[k]].stRect.s16Y2; rows++)
						{
							for(cols=pstDRegion[au8SplitMember[k]].stRect.s16X1; cols<pstDRegion[au8SplitMember[k]].stRect.s16X2; cols++)
							{
								pu8Data = pstRegionSet->pstImgFgRgn->pu8Data + rows * pstRegionSet->pstImgFgRgn->s32W + cols;
								if(*pu8Data == pstDRegion[au8SplitMember[k]].u8Sign)
								{
									*pu8Data = pstDRegion[s32MergedId].u8Sign;
								}
							}
						}

					}

				}
			}
		}
	}
}

static IMP_FLOAT ipDeterminPredictedTargetDirectConform(	IpTargetTrackerMotion *pstTracker,IpDrgDataTrackerM *pstDrgData,IpTargetDataTrackerM *pstTargetData);

extern IMP_VOID ipCalculateSplitObjectMatchCoefficient(IpTargetTrackerMotion *pstTracker);
IMP_VOID ipMTrackerCalculateMatchCoefficient( IpTargetTrackerMotion *pstTracker )
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTargetSetDataTrackerM *pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetDataTrackerM *pstTargetData;
	IpDrgDataTrackerM *pstDrgData;
	IMP_S32 i, j, ic, jc;

	IMP_FLOAT fDistCoef, fHistoCoef, fAreaCoef,fDirCoef, fTotalCoef;
	IMP_U32 u32IsStaticRg;
	IMP_S32 s32StaticMatchDisMax;
	IMP_S32 s32Width, s32Height;
	IMP_S32 flag = 0;

	IMP_FLOAT min_match_coef = pstTracker->pstPara->fMinMatchCoefLow;



	memset( pstTracker->afMatchCoeff, 0, sizeof(pstTracker->afMatchCoeff) );


	for( i=1, ic=0; i<(IMP_MAX_TGT_CNT-1) && ic<(pstRegionSet->s32UsedTotal); i++ )
	{
		if( !pstDRegion[i].u8Used )
			continue;
		ic++;
		pstDrgData = (IpDrgDataTrackerM*)pstDRegion[i].stPrivateData.pDataTracker;
		if ( pstDrgData->pstGroup )
//		if ( pstDrgData->s32Collision || pstDrgData->s32Split )
			continue;
		u32IsStaticRg = ( IMP_IS_RGN_RSTATIC_L(pstDRegion[i].u16Status) || IMP_IS_RGN_RSTATIC_R(pstDRegion[i].u16Status) );
		s32Width = pstDRegion[i].stRect.s16X2 - pstDRegion[i].stRect.s16X1 + 1;
		s32Height = pstDRegion[i].stRect.s16Y2 - pstDRegion[i].stRect.s16Y1 + 1;
		s32StaticMatchDisMax = ((s32Width+s32Height)>>4)+1;
		if( s32StaticMatchDisMax > pstTracker->pstPara->s32StaticMatchDisMax )
			s32StaticMatchDisMax = pstTracker->pstPara->s32StaticMatchDisMax;

		ipCalcMaxMatchDistanceM( pstTracker->pstPara, pstDrgData );


		for( j=0, jc=0; j<IMP_MAX_TGT_CNT && jc<(pstTargetSet->s32UsedTotal); j++ )
		{
			if(!pstTarget[j].s32Used)
				continue;
			jc++;

			pstTargetData = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;

			if (pstTargetData->cs.pstGroup )
				continue;

			fTotalCoef = 0.0;
			pstTracker->afMatchCoeff[j][i] = 0;


			if( u32IsStaticRg && pstTracker->pstPara->s32StaticMatchDisMax )
			{
				IMP_S32 dx, dy, tx, ty;
				dx = pstDrgData->stCurCentroid.s16X;
				dy = pstDrgData->stCurCentroid.s16Y;
				tx = pstTargetData->stCurCentroid.s16X;
				ty = pstTargetData->stCurCentroid.s16Y;
				if( abs(tx-dx)>s32StaticMatchDisMax || abs(ty-dy)>s32StaticMatchDisMax )
					continue;
			}


			fDistCoef = ipCalculateDistanceCoefficientM( pstDrgData->stCurCentroid, pstTargetData->stPredictedCentroid, pstDrgData );
			if( fDistCoef < 0.2 )
				continue;


			if(pstTracker->pstPara->s32DirectionDistanceLimit)
			{
				IpTargetPosition  *posT1;
				posT1 =  ipTargetTrajectoryGetPosition( &pstTarget[j].stTrajectory, -1 );
				if(!ipCalcDirectionDistCoef( pstTargetData->stPredictedCentroid, pstDrgData->stCurCentroid, pstDrgData, posT1->s32Direction ))
					continue;
			}


			fAreaCoef = ipCalculateAreaCoefficientM( pstDRegion[i].s32AreaPixel, pstTargetData->s32ObjArea );
			if( fAreaCoef < pstTracker->pstPara->fAreaCoefMin )
				continue;



			fHistoCoef = ipCalculateHistoCoefficient( pstDrgData->au32ObjectHistogram, pstTargetData->au32ObjectHistogram, IMP_GRAY_LEVEL );//gray

			if (fHistoCoef <  pstTracker->pstPara->fHistCoefMin)
				continue;


			if (ipTrackedTargetIsPredicted(&pstTarget[j]))
			{
				fTotalCoef = (IMP_FLOAT)( pstTracker->pstPara->fPredictTargetDistWeight * fDistCoef
					                    +  pstTracker->pstPara->fPredictTargetHistWeight * fHistoCoef
										+  pstTracker->pstPara->fPredictTargetAreaWeight * fAreaCoef );
			}
			else
			{
				fTotalCoef = (IMP_FLOAT)(pstTracker->pstPara->fDistWeight*fDistCoef + pstTracker->pstPara->fHistWeight*fHistoCoef + pstTracker->pstPara->fAreaWeight*fAreaCoef);
			}


			if( pstTracker->pstPara->s32WetStableTgt && fTotalCoef > min_match_coef )
			{
				if (pstTracker->pstPara->s32ProcessDisappearTarget)
				{
					if (ipTrackedTargetIsActiveMeasured( &pstTarget[j]))
					{
						fTotalCoef += 0.5;
						if (ipTrackedTargetIsActiveMeasuredMtrend( &pstTarget[j]))
							fTotalCoef += 0.5;
					}
					else if (ipTrackedTargetIsActivePredicted( &pstTarget[j]))
					{
						{//NEW
							if ( pstTargetData->s32FirstPredictPosAfterTrackedFlag)
							{
								fDirCoef = ipDeterminPredictedTargetDirectConform(pstTracker,pstDrgData,pstTargetData);
								if (fDirCoef > 0.)
									fTotalCoef += fDirCoef;
							}
						}
						fTotalCoef += 0.5;
					}
				}
				else
				{
					if (ipTrackedTargetIsActive( &pstTarget[j]))
					{
						fTotalCoef += 1;
					}
				}
			}
			pstTracker->afMatchCoeff[j][i] = fTotalCoef;
		}
	}


	if (pstTracker->pstPara->s32CsCollisionSplit)
	{
		ipCalculateSplitObjectMatchCoefficient(pstTracker);
	}
}
static IMP_FLOAT ipDeterminPredictedTargetDirectConform(	IpTargetTrackerMotion *pstTracker,IpDrgDataTrackerM *pstDrgData,IpTargetDataTrackerM *pstTargetData)
{
	IMP_FLOAT deta_x = 0,deta_y = 0;
	IMP_S32 s32Quadrant = 0;
	IMP_FLOAT fMovingAngle = 0;
	IMP_S32 current_direction = 0;
	IMP_FLOAT max_direct_diff = pstTracker->pstPara->fMaxDirectionDiff;
	IMP_S32 min_distance_diff = 50;
	IMP_FLOAT ret = 0;
	IMP_POINT_S *drgdat_current_centroid = &pstDrgData->stCurCentroid;
	IMP_POINT_S *tardat_current_centroid = &pstTargetData->stCurCentroid;
	IMP_POINT_S *tardat_first_predict_pos_centroid = &pstTargetData->stFirstPredictPosAfterTracked.stPt;
	IMP_S32 first_predict_pos_direction = pstTargetData->stFirstPredictPosAfterTracked.s32Direction;

	deta_x = (IMP_FLOAT)(drgdat_current_centroid->s16X - tardat_first_predict_pos_centroid->s16X);
	deta_y = (IMP_FLOAT)(drgdat_current_centroid->s16Y - tardat_first_predict_pos_centroid->s16Y);
	if ( fabs(deta_x) > min_distance_diff || fabs(deta_y) > min_distance_diff)
	{
		return ret;
	}
	if ( deta_x >= 0 )
	{
		if ( deta_y >= 0 )
			s32Quadrant = 1;
		else
			s32Quadrant = 4;
	}
	else
	{
		if ( deta_y >= 0 )
			s32Quadrant = 2;
		else
			s32Quadrant = 3;
	}
	if(fabs(deta_x) > 0.000001)
		fMovingAngle = (IMP_FLOAT)atan(deta_y/deta_x);
	else
	{
		fMovingAngle = 0;
		//ret = 1;
	}
	switch (s32Quadrant)
	{
		case 1:
			break;
		case 2:
		case 3:
			fMovingAngle += (IMP_FLOAT)IMP_PI;
			break;
		case 4:
			fMovingAngle += (IMP_FLOAT)IMP_PI * 2;
			break;
		default:
			break;
	}
	if ( fMovingAngle < 0 )
		fMovingAngle += (IMP_FLOAT)IMP_PI*2;
	current_direction = (IMP_S32)(fMovingAngle*180/IMP_PI);//单位：度
	//if (abs(current_direction - first_predict_pos_direction) < min_direct_diff)
	ret = 1 - abs(current_direction - first_predict_pos_direction) / max_direct_diff;
	return ret;
}

extern IMP_VOID ipMatchRegionToSplitObject(IpTargetTrackerMotion *pstTracker);

IMP_VOID ipMTrackerMatchRegionToTarget( IpTargetTrackerMotion *pstTracker )
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetDataTrackerM *pstTargetData;
	IpDrgDataTrackerM *pstDrgData;
	IMP_S32 indexM, indexN, maxIndexN=0,tempIndexM, s32Cnt;
	IMP_FLOAT maxCoef, tempMaxCoef;
	IMP_S32 loopTimes;
	IMP_S32 flag_max_exist=IMP_FALSE;
	IMP_S32 flag_multiTarget_split=IMP_FALSE;


 	for( indexM=1, s32Cnt=0; indexM<(IMP_MAX_TGT_CNT-1) && s32Cnt<pstRegionSet->s32UsedTotal; indexM++ )
	{
		if( !pstDRegion[indexM].u8Used )
			continue;
		s32Cnt++;

		pstDrgData = (IpDrgDataTrackerM*)pstDRegion[indexM].stPrivateData.pDataTracker;
		pstDrgData->s32MatchedWith = 0;
		if ( pstDrgData->pstGroup )
//		if ( pstDrgData->s32Collision || pstDrgData->s32Split)
			continue;

		loopTimes = 2;
LineLoopStart:
		maxCoef = 0;
		for ( indexN=0; indexN<IMP_MAX_TGT_CNT; indexN++ )
		{
			pstTargetData = (IpTargetDataTrackerM*)pstTarget[indexN].stPrivateData.pDataTracker;
			if ( pstTargetData->cs.pstGroup )
//			if ( pstTargetData->s32CurrentCollisionStatus  || pstTargetData->s32Split)
				continue;
			if ( pstTracker->afMatchCoeff[indexN][indexM] > maxCoef )
			{
				maxCoef = pstTracker->afMatchCoeff[indexN][indexM];
				maxIndexN = indexN;
			}
		}

		if (  maxCoef < pstTracker->pstPara->fMinMatchCoefHigh )
				continue;

		tempMaxCoef = maxCoef;
		for( tempIndexM=1; tempIndexM<(IMP_MAX_TGT_CNT-1); tempIndexM++ )
		{
			if( pstTracker->afMatchCoeff[maxIndexN][tempIndexM] > tempMaxCoef )
			{
				pstTracker->afMatchCoeff[maxIndexN][indexM] = 0.0;
				if( (loopTimes--) > 0)
					goto LineLoopStart;
			}
		}

		pstDrgData->s32MatchedWith = maxIndexN;

		pstTargetData = (IpTargetDataTrackerM*)pstTarget[maxIndexN].stPrivateData.pDataTracker;
		pstTargetData->s32MatchedWith = indexM;
		pstTargetData->fMatchCoefficient = maxCoef;


	}//the stEnd of 1
//------------------------------------------------------------------------
	if (pstTracker->pstPara->s32CsCollisionSplit)
	{
		ipMatchRegionToSplitObject(pstTracker);
	}
}


extern IMP_VOID ipCalcuCollisionLength( IpTargetTrackerMotion *pstTracker );
IMP_VOID ipMTrackerTargetInfoUpdate( IpTargetTrackerMotion *pstTracker )
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTargetSetDataTrackerM *pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetDataTrackerM *pstTargetData;
	IpTrackerPara *pstPara = pstTracker->pstPara;
	IMP_S32 indexT, indexD, s32Cnt, s32CurrentTime;
	s32CurrentTime = pstTracker->pstResult->u32FrmTimeCur;


	if (pstPara->s32CsCollisionSplit)
	{
		ipCalcuCollisionLength(pstTracker);
	}



	for ( indexT=0, s32Cnt=0; indexT<IMP_MAX_TGT_CNT && s32Cnt<pstTargetSet->s32UsedTotal; indexT++ )
	{
		if( !pstTarget[indexT].s32Used )
			continue;
		s32Cnt++;

		pstTargetData = (IpTargetDataTrackerM*)pstTarget[indexT].stPrivateData.pDataTracker;
		indexD = pstTargetData->s32MatchedWith;

		if (pstTargetData->cs.pstGroup && pstTracker->pstPara->s32CsCollisionSplit)
		{

			if( indexD != 0 )
			{

				if (pstTargetData->cs.s32CurCollision)
				{
					ipUpdateTargetFromCollision( &pstTarget[indexT], indexT, pstRegionSet, indexD, pstTracker);
				}

				else
				{
					ipUpdateTargetFromSplit( &pstTarget[indexT], indexT, pstRegionSet, indexD, pstTracker);
				}
			}

			else
			{

				if (pstTracker->pstPara->s32CsStableCollision && pstTargetData->cs.pstGroup->s32StablishCollision )
				{
					ipUpdateTargetFromCollisionStable( &pstTarget[indexT], indexT,pstTracker);
				}

				else
				{
					ipUpdateTargetFromCollisionDestroy( &pstTarget[indexT], indexT,pstTracker);
				}
			}

		}

		else
		{
					if( indexD != 0 )
					{
						ipUpdateTargetFromRegionM( &pstTarget[indexT], indexT, pstRegionSet, indexD, pstTracker );
					}

					else
					{
						ipUpdateTargetFromPredictM( &pstTarget[indexT], indexT, pstRegionSet, s32CurrentTime );
					}
		}


		{

			if (pstTargetData->cs.s32Split && indexD != 0 && pstTracker->pstPara->s32CsCollisionSplit && pstTracker->pstPara->s32CsCorrectCollisionTrajectory)
			{
				switch(pstTracker->pstPara->s32CsCorrectCollisionTrajectory)
				{
					case 1:
						{
							ipCorrectCollisionTrajectory( &pstTarget[indexT],pstRegionSet);
							break;
						}
					case 2:
						{
							ipCorrectCollisionTrajectory2( &pstTarget[indexT],pstRegionSet);
							break;
						}
					default:break;
				}
			}


			if (IMP_IS_TGT_MTREND( pstTarget[indexT].stTargetInfo.u32Status ) && !IMP_IS_TGT_COLLISION( pstTarget[indexT].stTargetInfo.u32Status ) && pstPara->s32SmoothTrajectory)
			{
				if ( pstPara->s32SmoothTrajectory == 1 )
				{
					ipSmoothTrajectory( &pstTarget[indexT], pstPara );
				}
				else if ( pstPara->s32SmoothTrajectory == 2 )
				{
					ipSmoothTrajectory2( &pstTarget[indexT], pstPara );
				}
				else if ( pstPara->s32SmoothTrajectory == 3 )
				{
					ipSmoothTrajectory3( &pstTarget[indexT], pstPara );
				}
			}
			{
				IpTargetPosition *pstPos;
				pstPos = ipTargetTrajectoryGetPosition( &pstTarget[indexT].stTrajectory, 0 );
				ipCheckPositionValidM( pstTracker, pstPos );
			}//
		}
	}

}


IMP_VOID ipMTrackerPostProcess( IpTargetTrackerMotion *pstTracker )
{
//	PEA_RESULT_S *pstResult = pstTracker->pstResult;
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetDataTrackerM *pstTargetData;
	IpDrgDataTrackerM *pstDrgData;
	IMP_S32 i, j, s32Cnt;
	IMP_S32 u32Status;
	IpTrackerPara *pstPara = pstTracker->pstPara;



	for( i=0; i<IMP_MAX_TGT_CNT; i++ )
	{
		if( !pstTarget[i].s32Used )
			continue;

		pstTargetData = (IpTargetDataTrackerM*)pstTarget[i].stPrivateData.pDataTracker;
		u32Status = ipDeterminTargetStatusS( pstTracker, &pstTarget[i], pstTracker->pstPara );
		if( IMP_TTGT_STATUS_INIT == u32Status )
		{
		}
		else if( IMP_TTGT_STATUS_STABLE == u32Status )
		{
			if( pstTargetData->s32MatchedWith != 0  )
			{
				if( IMP_IS_TGT_STATIC( pstTarget[i].stTargetInfo.u32Status ) )
					IMP_SET_RGN_TSTATIC(pstRegionSet->astDrg[pstTargetData->s32MatchedWith].u16Status);
				if( IMP_IS_TGT_MOTION( pstTarget[i].stTargetInfo.u32Status ) )
					IMP_SET_RGN_TMOTION(pstRegionSet->astDrg[pstTargetData->s32MatchedWith].u16Status);
			}
		}
		else
		{
			ipTrackedTargetSetFree( pstTargetSet, i );
		}
	}


	for( i=1, s32Cnt=0; i<(IMP_MAX_TGT_CNT-1) && s32Cnt<pstRegionSet->s32UsedTotal; i++ )
	{
		if( !pstDRegion[i].u8Used )
			continue;
		s32Cnt++;

		pstDrgData = (IpDrgDataTrackerM*)pstDRegion[i].stPrivateData.pDataTracker;
		if( pstDrgData->s32MatchedWith == 0 )
		{

			j = ipTrackedTargetSetAlloc( pstTargetSet );
			if( j==0 )
			{
// 				IMP_ASSERT(0);
				break;
			}
			ipNewTargetFromRegionM( &pstTarget[j], j, pstRegionSet, i, ipAssignObjectIDM(pstTracker) ,pstTracker);
		}
	}

	if( pstTracker->pstPara->s32MergeTarget )
		ipMergeSimilarTargets( pstTracker );



#ifdef DEBUG_TRACKER

	for ( i=1, j=0; i<IMP_MAX_TGT_CNT; i++ )
	{
		if ( pstTarget[i].s32Used )
			j++;
	}
	IMP_ASSERT( pstTargetSet->astTargets[0].s32Used == 0 );
	IMP_ASSERT( pstTargetSet->s32UsedTotal == j );
#endif
#ifdef DEBUG_OBJRECG_TRACKER_INFO2
	ipGroupInfoDisplay(pstTracker);
#endif

}

//////////////////////////////////////////////////////////////////////////

static IMP_VOID ipDetermineTargetStatusStableM( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara );
static IMP_VOID ipDetemineTargetStatusStatic( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara );
static IMP_VOID ipDetemineTargetStatusMtrend( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara );
static IMP_S32 ipDeterminTargetStatusS( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara )
{
	if( IMP_IS_TGT_PREDICTED( pstTarget->stTargetInfo.u32Status) )
	{
		IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
		PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
		PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
		IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
		IpDrgDataTrackerM *pstDrgData = (IpDrgDataTrackerM*)pstDRegion[pstTargetData->s32MatchedWith].stPrivateData.pDataTracker;
		IMP_S32 s32MaxDist = pstDrgData->s32MaxMatchDist/8;
		IMP_S32 max_distance1 = pstTracker->pstDRegionDatas->s32MaxMatchDist;
		IMP_S32 stable_border_width = pstPara->s32StableBorderWidth;
		PEA_RESULT_S *pstResult = pstTracker->pstResult;
		IMP_S32 s32CurrentTime = pstResult->u32FrmTimeCur;
		IMP_S32 timeInterval = (s32CurrentTime-pstTargetData->s32LastTime)*(pstTracker->pstPara->s32FrmDura);

		if( 0
 			|| ( timeInterval > pstPara->s32StableOcclusionTime )
			|| ( pstTargetData->stPredictedCentroid.s16X < stable_border_width + s32MaxDist
				|| pstTargetData->stPredictedCentroid.s16X > (pstTracker->pstResult->s32Width-stable_border_width - s32MaxDist)
				|| pstTargetData->stPredictedCentroid.s16Y < stable_border_width + s32MaxDist
				|| pstTargetData->stPredictedCentroid.s16Y > (pstTracker->pstResult->s32Height-stable_border_width - s32MaxDist) )
			)
		{
			IMP_SET_TGT_DELETE( pstTarget->stTargetInfo.u32Status );
			if( IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ))
				PRINT_TARGET( pstTracker, pstTarget );
		}
	}

	if( IMP_IS_TGT_INIT( pstTarget->stTargetInfo.u32Status ) )
	{
		ipDetermineTargetStatusStableM( pstTracker, pstTarget, pstPara );
	}

	if( IMP_IS_TGT_MEASURED( pstTarget->stTargetInfo.u32Status ) )
		ipCalcTargetVelocity( pstTracker, pstTarget );

	if( IMP_IS_TGT_STABLE( pstTarget->stTargetInfo.u32Status ) )
	{

		if(pstTracker->pstPara->s32ProcessDisappearTarget)
		{
			IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
			IpTargetPosition *pstPos1,*pstPos2 ;
			IMP_S32	len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
			IMP_S32 used_traject_len = pstPara->s32UsedTrajectLength;
			IMP_S32 mid_len;
			len = len<=used_traject_len ? len : used_traject_len;
			mid_len = len>>1;
			pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );
			pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -mid_len );
			if (IMP_IS_TGT_PREDICTED( pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ) && !pstTargetData->s32FirstPredictPosAfterTrackedFlag)
			{
				pstTargetData->s32FirstPredictPosAfterTrackedFlag = 1;
				memcpy( &pstTargetData->stFirstPredictPosAfterTracked, pstPos1, sizeof(IpTargetPosition) );
				pstTargetData->stFirstPredictPosAfterTracked.s32Direction = pstPos2->s32Direction;
				//PRINT_TARGET( pstTracker, pstTarget );

			}
			else if(IMP_IS_TGT_MEASURED( pstTarget->stTargetInfo.u32Status ))
			{
				if (pstTargetData->s32FirstPredictPosAfterTrackedFlag)
				{
					pstTargetData->s32PredictTargetTrackedNum++;
				}
				if (pstTargetData->s32PredictTargetTrackedNum > 2)
				{
					pstTargetData->s32FirstPredictPosAfterTrackedFlag = 0;
					pstTargetData->s32PredictTargetTrackedNum = 0;
				}
			}
		}

		ipDetemineTargetStatusStatic( pstTracker, pstTarget, pstPara );

		if( !IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ) && pstPara->s32AnalysisMtrend )
			ipDetemineTargetStatusMtrend( pstTracker, pstTarget, pstPara );
	}

	return IMP_GET_TGT_STATUS( pstTarget->stTargetInfo.u32Status );
}

static IMP_VOID ipDetermineTargetStatusStableM( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara )
{
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 s32Life;
	IMP_S32 s32FstTime, s32LstTime;

	pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	pstPos2 = &pstTargetData->stFirstPos;
	s32FstTime = pstPos2->u32Time;
	s32LstTime = pstPos1->u32Time;
	s32Life = abs( s32LstTime - s32FstTime ) * pstPara->s32FrmDura;

	if( IMP_IS_TGT_MEASURED( pstTarget->stTargetInfo.u32Status) )
	{
		if( s32Life >= pstPara->s32StableMinTime
			&& pstTargetData->s32TrackedFrameNum >= pstPara->s32StableMinFrmCnt )
			IMP_SET_TGT_STABLE( pstTarget->stTargetInfo.u32Status );
	}
	else
	{
		IMP_SET_TGT_DELETE( pstTarget->stTargetInfo.u32Status );
		if( IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ))
			PRINT_TARGET( pstTracker, pstTarget );
	}
}

static IMP_VOID ipDetemineTargetStatusStatic( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara )
{
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;

	PEA_DETECTED_REGION_S *astDrg;
	IpTargetPosition *pstPos, *pstPos2;
	IMP_U32 u32IsStaticRg, u32IsStaticTgt;
	IMP_U32 u32FrmTimeCur = pstTracker->pstResult->u32FrmTimeCur;
	IpTargetFeatureVal *pstFtureVal = (IpTargetFeatureVal*)pstTarget->stTargetFeat.au8Data;

	if( IMP_IS_TGT_PREDICTED( pstTarget->stTargetInfo.u32Status )

	  )
		return;

	u32IsStaticTgt = IMP_FALSE;
	pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );

	if (!(pstTargetData->cs.pstGroup && pstTargetData->cs.pstGroup->s32StablishCollision))
	{
		astDrg = pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex;
		u32IsStaticRg = IMP_IS_RGN_RSTATIC_L(astDrg->u16Status) || IMP_IS_RGN_RSTATIC_R(astDrg->u16Status);
	}
	if(
		(

		   u32IsStaticRg

		   ||(pstTargetData->cs.pstGroup && pstTargetData->cs.pstGroup->s32StablishCollision)
		)

		&& ( pstPos->s32Velocity<pstPara->s32TimeStaticVelocityMax || pstPos2->s32Velocity<pstPara->s32TimeStaticVelocityMax )
	  )
	{
		if( pstFtureVal->u32StaticTimeStart == 0 )
		{
			pstFtureVal->u32StaticTimeStart = u32FrmTimeCur;
		}
		if( IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ) )
		{
			if( pstFtureVal->u32StaticTimeStart + pstPara->s32TimeStaticVelocityMin <= u32FrmTimeCur )
				u32IsStaticTgt = IMP_TRUE;
		}
		else
		{
			u32IsStaticTgt = IMP_TRUE;
		}
	}
	else
	{
		pstFtureVal->u32StaticTimeStart = 0;
	}

	if( u32IsStaticTgt )
	{
		IMP_SET_TGT_STATIC( pstTarget->stTargetInfo.u32Status );
		IMP_RESET_TGT_MOTION( pstTarget->stTargetInfo.u32Status );
	}
	else
	{
		IMP_RESET_TGT_STATIC( pstTarget->stTargetInfo.u32Status );
		IMP_SET_TGT_MOTION( pstTarget->stTargetInfo.u32Status );
	}
}


static IMP_VOID ipDetemineTargetStatusMtrend( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, IpTrackerPara *pstPara )
{
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpTargetPosition *pstPos1, *pstPos2;
	IMP_S32 is_valid, valid_cnt, stable_flag;
	IMP_S32 s32StableMinFrmCnt, s32StableValidFrmCnt;
	IMP_S32 i, len, w, h, len2, s32Life;
	IMP_S32 x1, y1, x2, y2;
	IMP_S32 trd_neg_x, trd_pot_x, trd_zero_x, trd_neg_y, trd_pot_y, trd_zero_y;
	IMP_S32 trd_total_xy;
// 	IMP_S32 trd_zero_xy, trd_zero_motion;
	IMP_S32 trd_non0_motion_x, trd_non0_motion_y;
	IMP_S32 pot_dis_x, neg_dis_x, pot_dis_y, neg_dis_y;
	IMP_S32 is_trend, is_trend_x_pot, is_trend_x_neg, is_trend_y_pot, is_trend_y_neg;
	IMP_S32 stable_dis_x, stable_dis_y, dis_x, dis_y, signx, signy, diff_w, diff_h;
	IMP_S32 w_sum, h_sum, w_max, w_min, h_max, h_min;
	IMP_S32 w_mean, h_mean;
	IMP_S32 disx_mean, disy_mean;
// 	IMP_S32 potx_sum, poty_sum, negx_sum, negy_sum;

///////////////////////////////////////////////////////////////////////////////////////////////////
	w_sum = pstTargetData->w_sum;
	h_sum = pstTargetData->h_sum;
	w_max = pstTargetData->w_max;
	h_max = pstTargetData->h_max;
	w_min = pstTargetData->w_min;
	h_min = pstTargetData->h_min;
	if( w_min == 0 ) w_min = 1000;
	if( h_min == 0 ) h_min = 1000;
// 	potx_sum = pstTargetData->potx_sum;
// 	poty_sum = pstTargetData->poty_sum;
// 	negx_sum = pstTargetData->negx_sum;
// 	negy_sum = pstTargetData->negy_sum;
	trd_neg_x = pstTargetData->trd_neg_x;
	trd_pot_x = pstTargetData->trd_pot_x;
	trd_zero_x = pstTargetData->trd_zero_x;
	trd_neg_y = pstTargetData->trd_neg_y;
	trd_pot_y = pstTargetData->trd_pot_y;
	trd_zero_y = pstTargetData->trd_zero_y;
	trd_total_xy = pstTargetData->trd_total_xy;
	trd_non0_motion_x = pstTargetData->trd_non0_motion_x;
	trd_non0_motion_y = pstTargetData->trd_non0_motion_y;
// 	trd_zero_xy = pstTargetData->trd_zero_xy;
// 	trd_zero_motion = pstTargetData->trd_zero_motion;
	pot_dis_x = pstTargetData->pot_dis_x;
	neg_dis_x = pstTargetData->neg_dis_x;
	pot_dis_y = pstTargetData->pot_dis_y;
	neg_dis_y = pstTargetData->neg_dis_y;

	len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
	if( trd_total_xy == 0 )
		len2 = len - 1;
	else
		len2 = 1;
	i = -1;
	while( ++i < len2 )
	{
		pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );
		if( IMP_IS_TPOS_MEASURED(pstPos1->s32Used) )
		{
			pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-1 );

			w = pstPos1->stRg.s16X2 - pstPos1->stRg.s16X1 + 1;
			h = pstPos1->stRg.s16Y2 - pstPos1->stRg.s16Y1 + 1;
			w_sum += w;
			h_sum += h;
			w_max = w > w_max ? w : w_max;
			w_min = w < w_min ? w : w_min;
			h_max = h > h_max ? h : h_max;
			h_min = h < h_min ? h : h_min;

			trd_total_xy++;

			if( pstPara->s32TrajectUseBottom == 2 )
			{
				x1 = ( ( pstPos1->stRg.s16X1 + pstPos1->stRg.s16X2 ) >> 1 );
				y1 = pstPos1->stRg.s16Y2;
				x2 = ( ( pstPos2->stRg.s16X1 + pstPos2->stRg.s16X2 ) >> 1 );
				y2 = pstPos2->stRg.s16Y2;
			}
			else
			{
				x1 = pstPos1->stPt.s16X;
				y1 = pstPos1->stPt.s16Y;
				x2 = pstPos2->stPt.s16X;
				y2 = pstPos2->stPt.s16Y;
			}
			signx = x1 - x2;
			signy = y1 - y2;

			if( signx>0 )
			{
				if( pot_dis_x < signx )
					pot_dis_x = signx;
				//potx_sum += signx;
				trd_pot_x++;
			}
			else if( signx<0 )
			{
				if( neg_dis_x > signx )
					neg_dis_x = signx;
				//negx_sum += signx;
				trd_neg_x++;
			}
			else
				trd_zero_x++;

			if( signy>0 )
			{
				if( pot_dis_y < signy )
					pot_dis_y = signy;
				//poty_sum += signy;
				trd_pot_y++;
			}
			else if( signy<0 )
			{
				if( neg_dis_y > signy )
					neg_dis_y = signy;
				//negy_sum += signy;
				trd_neg_y++;
			}
			else
				trd_zero_y++;

			if( IMP_IS_RGN_RMOTION(pstPos1->u32DrgType) )
			{
				if( signx )
					trd_non0_motion_x++;
				if( signy )
					trd_non0_motion_y++;
			}

// 			if( signy==0 && signx==0 )
// 			{
// 				trd_zero_xy++;
// 				if( IMP_IS_RGN_RMOTION(pstPos1->u32DrgType) )
// 					trd_zero_motion++;
// 			}
		}
	}

	pstTargetData->w_sum = w_sum;
	pstTargetData->h_sum = h_sum;
	pstTargetData->w_max = w_max;
	pstTargetData->h_max = h_max;
	pstTargetData->w_min = w_min;
	pstTargetData->h_min = h_min;
// 	pstTargetData->potx_sum = potx_sum;
// 	pstTargetData->poty_sum = poty_sum;
// 	pstTargetData->negx_sum = negx_sum;
// 	pstTargetData->negy_sum = negy_sum;
	pstTargetData->trd_neg_x = trd_neg_x;
	pstTargetData->trd_pot_x = trd_pot_x;
	pstTargetData->trd_zero_x = trd_zero_x;
	pstTargetData->trd_neg_y = trd_neg_y;
	pstTargetData->trd_pot_y = trd_pot_y;
	pstTargetData->trd_zero_y = trd_zero_y;
	pstTargetData->trd_total_xy = trd_total_xy;
	pstTargetData->trd_non0_motion_x = trd_non0_motion_x;
	pstTargetData->trd_non0_motion_y = trd_non0_motion_y;
// 	pstTargetData->trd_zero_xy = trd_zero_xy;
// 	pstTargetData->trd_zero_motion = trd_zero_motion;
	pstTargetData->pot_dis_x = pot_dis_x;
	pstTargetData->neg_dis_x = neg_dis_x;
	pstTargetData->pot_dis_y = pot_dis_y;
	pstTargetData->neg_dis_y = neg_dis_y;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
	len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );
	s32StableMinFrmCnt = pstPara->s32StableMinFrmCnt;
	s32StableValidFrmCnt = pstPara->s32StableValidFrmCnt;
	if( s32StableValidFrmCnt > len )
		s32StableValidFrmCnt = len;
	valid_cnt = 0; stable_flag = 1;
	for( i=0; i<s32StableValidFrmCnt; i++ )
	{
		pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );
		if( IMP_IS_TPOS_MEASURED(pstPos1->s32Used) )
		{
			valid_cnt++;
		}
		else// if( IS_TPOS_PREDICTED(pstPos1->s32Used) )
		{
			if( i < s32StableMinFrmCnt )
				stable_flag = 0;
		}
	}
	is_valid = ( valid_cnt * 100 >= s32StableValidFrmCnt * pstPara->s32StableValidRatio );
	if( !is_valid )
	{
		IMP_SET_TGT_DELETE( pstTarget->stTargetInfo.u32Status );
		if( IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ))
			PRINT_TARGET( pstTracker, pstTarget );
		return;
	}
	else if( !stable_flag )
	{
		return;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
	pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	pstPos2 = &pstTargetData->stFirstPos;

	if( pstPara->s32TrajectUseBottom == 2 )
	{
		x1 = ( ( pstPos1->stRg.s16X1 + pstPos1->stRg.s16X2 ) >> 1 );
		y1 = pstPos1->stRg.s16Y2;
		x2 = ( ( pstPos2->stRg.s16X1 + pstPos2->stRg.s16X2 ) >> 1 );
		y2 = pstPos2->stRg.s16Y2;
	}
	else
	{
		x1 = pstPos1->stPt.s16X;
		y1 = pstPos1->stPt.s16Y;
		x2 = pstPos2->stPt.s16X;
		y2 = pstPos2->stPt.s16Y;
	}
	dis_x = x1 - x2;
	dis_y = y1 - y2;
	disx_mean = dis_x*FPRECISE/trd_total_xy;
	disy_mean = dis_y*FPRECISE/trd_total_xy;

	w_mean = w_sum / trd_total_xy;
	h_mean = h_sum / trd_total_xy;
	diff_w = ( w_max - w_min ) / 2;
	diff_h = ( h_max - h_min ) / 2;
// 	diff_w = abs( ( pstPos1->stRg.s16X2-pstPos1->stRg.s16X1+1) - w_mean ) / 2;
// 	diff_h = abs( ( pstPos1->stRg.s16Y2-pstPos1->stRg.s16Y1+1) - h_mean ) / 2;
// 	diff_w = abs( ( pstPos1->stRg.s16X2-pstPos1->stRg.s16X1+1) - ( pstPos2->stRg.s16X2-pstPos2->stRg.s16X1+1) );
// 	diff_h = abs( ( pstPos1->stRg.s16Y2-pstPos1->stRg.s16Y1+1) - ( pstPos2->stRg.s16Y2-pstPos2->stRg.s16Y1+1) );
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
// 	stable_dis_x = w_mean / pstPara->s32StableShapeRatio;
// 	stable_dis_y = h_mean / pstPara->s32StableShapeRatio;
	stable_dis_y = stable_dis_x = (w_mean+h_mean) / 2 / pstPara->s32StableShapeRatio;
	if( stable_dis_x < pstPara->s32StableMinDistance )
		stable_dis_x = pstPara->s32StableMinDistance;
	else if( stable_dis_x > pstPara->s32StableMaxDistance )
		stable_dis_x = pstPara->s32StableMaxDistance;
	if( stable_dis_y < pstPara->s32StableMinDistance )
		stable_dis_y = pstPara->s32StableMinDistance;
	else if( stable_dis_y > pstPara->s32StableMaxDistance )
		stable_dis_y = pstPara->s32StableMaxDistance;
	if( abs(dis_x)<stable_dis_x && abs(dis_y)<stable_dis_y )
		return;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
	is_trend_x_pot = ( (dis_x>0) && (trd_pot_x>trd_neg_x) );
	is_trend_x_neg = ( (dis_x<0) && (trd_pot_x<trd_neg_x) );
	is_trend_y_pot = ( (dis_y>0) && (trd_pot_y>trd_neg_y) );
	is_trend_y_neg = ( (dis_y<0) && (trd_pot_y<trd_neg_y) );
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
	if( trd_non0_motion_x*100 <= (trd_pot_x+trd_neg_x)*(pstPara->s32StableNon0Ratio) )
	{
		if( abs(dis_x)<=diff_w )
		{
			is_trend_x_pot = 0;
			is_trend_x_neg = 0;
		}
		else
		{
			if( dis_x >= 0 )
				dis_x -= diff_w;
			else
				dis_x += diff_w;
		}
	}
	if( trd_non0_motion_y*100 <= (trd_pot_y+trd_neg_y)*(pstPara->s32StableNon0Ratio) )
	{
		if( abs(dis_y)<=diff_h )
		{
			is_trend_y_pot = 0;
			is_trend_y_neg = 0;
		}
		else
		{
			if( dis_y >= 0 )
				dis_y -= diff_h;
			else
				dis_y += diff_h;
		}
	}
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
	if( pot_dis_x>=2 && abs(pot_dis_x*FPRECISE)>abs(disx_mean*2) )
		pot_dis_x *= pstPara->s32StableDistRatio;
	else
		pot_dis_x = 0;
	if( neg_dis_x<=-2 && abs(neg_dis_x*FPRECISE)>abs(disx_mean*2) )
		neg_dis_x *= pstPara->s32StableDistRatio;
	else
		neg_dis_x = 0;
	if( pot_dis_y>=2 && abs(pot_dis_y*FPRECISE)>abs(disy_mean*2) )
		pot_dis_y *= pstPara->s32StableDistRatio;
	else
		pot_dis_y = 0;
	if( neg_dis_y<=-2 && abs(neg_dis_y*FPRECISE)>abs(disy_mean*2) )
		neg_dis_y *= pstPara->s32StableDistRatio;
	else
		neg_dis_y = 0;
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
	is_trend = ( 0
			||
			(	is_trend_x_pot
				&&	( dis_x > pot_dis_x )
				&&	( (dis_x - pot_dis_x) >= stable_dis_x )
			)
			||
			(	is_trend_x_neg
				&&	( dis_x < neg_dis_x )
				&&	( (neg_dis_x - dis_x) >= stable_dis_x )
			)
			||
			(	is_trend_y_pot
				&&	( dis_y > pot_dis_y )
				&&	( (dis_y - pot_dis_y) >= stable_dis_y )
			)
			||
			(	is_trend_y_neg
				&&	( dis_y < neg_dis_y )
				&&	( (neg_dis_y - dis_y) >= stable_dis_y )
			)
			);
///////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////
 	if( is_trend )
	{
		// PRINT_TARGET( pstTracker, pstTarget );
		IMP_SET_TGT_MTREND( pstTarget->stTargetInfo.u32Status );
		return;
	}
	s32Life = abs( pstPos1->u32Time - pstPos2->u32Time ) * pstPara->s32FrmDura;
	if( s32Life > pstPara->s32StableMaxTime )
	{
		IMP_SET_TGT_DELETE( pstTarget->stTargetInfo.u32Status );
		if( IMP_IS_TGT_MTREND( pstTarget->stTargetInfo.u32Status ))
			PRINT_TARGET( pstTracker, pstTarget );
		return;
	}
///////////////////////////////////////////////////////////////////////////////////////////////////
}

static IMP_VOID ipCheckPositionValidM( IpTargetTrackerMotion *pstTracker, IpTargetPosition *pstPos )
{
	IMP_RECT_S stRect;
	stRect.s16X1 = stRect.s16Y1 = 0;
	stRect.s16X2 = pstTracker->pstResult->s32Width-1;
	stRect.s16Y2 = pstTracker->pstResult->s32Height-1;

	if( pstPos->stPt.s16X<stRect.s16X1 ) pstPos->stPt.s16X = stRect.s16X1;
	if( pstPos->stPt.s16X>stRect.s16X2 ) pstPos->stPt.s16X = stRect.s16X2;
	if( pstPos->stPt.s16Y<stRect.s16Y1 ) pstPos->stPt.s16Y = stRect.s16Y1;
	if( pstPos->stPt.s16Y>stRect.s16Y2 ) pstPos->stPt.s16Y = stRect.s16Y2;

	if( pstPos->stRg.s16X1<stRect.s16X1 ) pstPos->stRg.s16X1 = stRect.s16X1;
	if( pstPos->stRg.s16X2>stRect.s16X2 ) pstPos->stRg.s16X2 = stRect.s16X2;
	if( pstPos->stRg.s16Y1<stRect.s16Y1 ) pstPos->stRg.s16Y1 = stRect.s16Y1;
	if( pstPos->stRg.s16Y2>stRect.s16Y2 ) pstPos->stRg.s16Y2 = stRect.s16Y2;
}


static IMP_VOID ipCorrectCollisionTrajectory(IpTrackedTarget *pstTarget,PEA_DETECTED_REGIONSET_S *pstRegionSet)
{

	IpTargetDataTrackerM* pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	PEA_DETECTED_REGION_S* pstDRegion = &pstRegionSet->astDrg[pstTargetData->s32MatchedWith];
	IpDrgDataTrackerM* pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;

	IpTargetPosition *pstPos;
	IMP_S32 s32Len;
	IMP_S32 i;
	IMP_POINT_S stPtStart,stPtEnd;
	IMP_FLOAT dx,dy;
	s32Len = pstTargetData->cs.s32CollisionLength;


	{
		stPtStart.s16X=pstTargetData->cs.sampleTardat.stCentroid.s16X;
		stPtStart.s16Y=pstTargetData->cs.sampleTardat.stCentroid.s16Y;
		stPtEnd.s16X=pstDrgData->stCurCentroid.s16X;
		stPtEnd.s16Y=pstDrgData->stCurCentroid.s16Y;

		dx = (IMP_FLOAT)(stPtEnd.s16X-stPtStart.s16X)/(s32Len+1);
		dy = (IMP_FLOAT)(stPtEnd.s16Y-stPtStart.s16Y)/(s32Len+1);

		for (i=1;i<=s32Len && i<=255;i++)
		{
			pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );

			pstPos->stPt.s16X = stPtStart.s16X+(IMP_S32)((i+1)*dx+0.5);
			pstPos->stPt.s16Y = stPtStart.s16Y+(IMP_S32)((i+1)*dy+0.5);
		}
	}

}


static IMP_VOID ipCorrectCollisionTrajectory2(IpTrackedTarget *pstTarget,PEA_DETECTED_REGIONSET_S *pstRegionSet)
{

	IpTargetDataTrackerM* pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	PEA_DETECTED_REGION_S* pstDRegion = &pstRegionSet->astDrg[pstTargetData->s32MatchedWith];
	IpDrgDataTrackerM* pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;

	IpTargetPosition pstTmpPos;
	IpTargetPosition *pstPos, *pstPrePos = &pstTmpPos;
	IMP_S32 s32Len;
	IMP_S32 s32Width, s32Height;
	IMP_S32 s32OffsetX, s32OffsetY;
	IMP_S32 i;
	IMP_RECT_S stBoundary;

	s32Len = pstTargetData->cs.s32CollisionLength;



	{
		ipCorrectCollisionTrajectory( pstTarget,pstRegionSet);
	}

	{
		for (i=1;i<=s32Len && i<=255;i++)
		{

			pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );

			pstPrePos->stRg = pstTargetData->cs.sampleTardat.stRect;
			pstPrePos->stPt = pstTargetData->cs.sampleTardat.stCentroid;

 			s32Width = pstPrePos->stRg.s16X2 - pstPrePos->stRg.s16X1;
			s32Height = pstPrePos->stRg.s16Y2 - pstPrePos->stRg.s16Y1;
			stBoundary.s16X1 = pstPos->stRg.s16X1 + (s32Width >> 1);
			stBoundary.s16Y1 = pstPos->stRg.s16Y1 + (s32Height >> 1);
			stBoundary.s16X2 = pstPos->stRg.s16X2 - (s32Width >> 1);
			stBoundary.s16Y2 = pstPos->stRg.s16Y2 - (s32Height >> 1);

			if (stBoundary.s16X1 > stBoundary.s16X2)
			{
				stBoundary.s16X1 = (pstPos->stRg.s16X1 + pstPos->stRg.s16X2) >> 1;
				stBoundary.s16X2 = (pstPos->stRg.s16X1 + pstPos->stRg.s16X2) >> 1;
			}

			if (stBoundary.s16Y1 > stBoundary.s16Y2)
			{
				stBoundary.s16Y1 = (pstPos->stRg.s16Y1 + pstPos->stRg.s16Y2) >> 1;
				stBoundary.s16Y2 = (pstPos->stRg.s16Y1 + pstPos->stRg.s16Y2) >> 1;
			}


			s32OffsetX = pstPos->stPt.s16X < stBoundary.s16X1 ? (stBoundary.s16X1-pstPos->stPt.s16X):0;
			s32OffsetX += pstPos->stPt.s16X > stBoundary.s16X2 ? (stBoundary.s16X2-pstPos->stPt.s16X):0;
			s32OffsetY = pstPos->stPt.s16Y < stBoundary.s16Y1 ? (stBoundary.s16Y1-pstPos->stPt.s16Y):0;
			s32OffsetY += pstPos->stPt.s16Y > stBoundary.s16Y2 ? (stBoundary.s16Y2-pstPos->stPt.s16Y):0;


			pstPos->stPt.s16X = pstPos->stPt.s16X + s32OffsetX;
			pstPos->stPt.s16Y = pstPos->stPt.s16Y + s32OffsetY;


			s32OffsetX = pstPrePos->stRg.s16X1 - pstPrePos->stPt.s16X;
			s32OffsetY = pstPrePos->stRg.s16Y1 - pstPrePos->stPt.s16Y;

			pstPos->stRg.s16X1 = pstPos->stPt.s16X + s32OffsetX;
			pstPos->stRg.s16Y1 = pstPos->stPt.s16Y + s32OffsetY;
			pstPos->stRg.s16X2 = pstPos->stRg.s16X1 + s32Width;
			pstPos->stRg.s16Y2 = pstPos->stRg.s16Y1 + s32Height;
		}
	}
}


static IMP_VOID ipSmoothTrajectory(IpTrackedTarget *pstTarget, IpTrackerPara *pstPara)
{
	IpTargetPosition *pstPosCur, *pstPosNxt, *pstPosLst;

	IMP_S32 i;
	IMP_S32 loop = pstPara->s32SmoothTimeWnd;
    IMP_S32	len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );

	for (i=loop;i>0 && i<len-3;i--)
	{
		pstPosLst = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-2 );
		pstPosCur = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-1 );
		pstPosNxt = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i   );

		pstPosCur->stPt.s16X = (pstPosLst->stPt.s16X + pstPosNxt->stPt.s16X)>>1;
		pstPosCur->stPt.s16Y = (pstPosLst->stPt.s16Y + pstPosNxt->stPt.s16Y)>>1;

	}
}

static IMP_VOID ipSmoothTrajectory2(IpTrackedTarget *pstTarget, IpTrackerPara *pstPara)
{
	IpTargetPosition *pstPos0, *pstPos1, *pstPos2, *pstPos3, *pstPos4;

	IMP_S32 i;
	IMP_S32 loop = pstPara->s32SmoothTimeWnd;
    IMP_S32	len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );

	IMP_FLOAT w0 = 0.15f;
	IMP_FLOAT w1 = 0.20f;
	IMP_FLOAT w2 = 0.30f;
	IMP_FLOAT w3 = 0.20f;
	IMP_FLOAT w4 = 0.15f;


	for (i=loop;i>0 && i<len-5;i--)
	{
		pstPos0 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-4 );
		pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-3 );
		pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-2 );
		pstPos3 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-1 );
		pstPos4 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-0 );

		pstPos2->stPt.s16X = (IMP_S16)(pstPos0->stPt.s16X * w0 + pstPos1->stPt.s16X * w1 + pstPos2->stPt.s16X * w2 + pstPos3->stPt.s16X * w3 + pstPos4->stPt.s16X * w4 +0.5);
		pstPos2->stPt.s16Y = (IMP_S16)(pstPos0->stPt.s16Y * w0 + pstPos1->stPt.s16Y * w1 + pstPos2->stPt.s16Y * w2 + pstPos3->stPt.s16Y * w3 + pstPos4->stPt.s16Y * w4 +0.5);
	}
}

static IMP_VOID ipSmoothTrajectory3(IpTrackedTarget *pstTarget,IpTrackerPara *pstPara)
{
	IpTargetPosition *pstPos0, *pstPos1, *pstPos2, *pstPos3, *pstPos4, *pstPos5, *pstPos6;

	IMP_S32 i;
	IMP_S32 loop = pstPara->s32SmoothTimeWnd;
    IMP_S32	len = ipTargetTrajectoryGetLength( &pstTarget->stTrajectory );

	IMP_FLOAT w0 = 0.05f;
	IMP_FLOAT w1 = 0.10f;
	IMP_FLOAT w2 = 0.20f;
	IMP_FLOAT w3 = 0.30f;
	IMP_FLOAT w4 = 0.20f;
	IMP_FLOAT w5 = 0.10f;
	IMP_FLOAT w6 = 0.05f;

	for (i=loop;i>0 && i<len-7;i--)
	{
		pstPos0 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-6 );
		pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-5 );
		pstPos2 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-4 );
		pstPos3 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-3 );
		pstPos4 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-2 );
		pstPos5 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-1 );
		pstPos6 = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i-0 );

		pstPos3->stPt.s16X = (IMP_S16)(pstPos0->stPt.s16X * w0 + pstPos1->stPt.s16X * w1 + pstPos2->stPt.s16X * w2 + pstPos3->stPt.s16X * w3 + pstPos4->stPt.s16X * w4 + pstPos5->stPt.s16X * w5 + pstPos6->stPt.s16X * w6 +0.5);
		pstPos3->stPt.s16Y = (IMP_S16)(pstPos0->stPt.s16Y * w0 + pstPos1->stPt.s16Y * w1 + pstPos2->stPt.s16Y * w2 + pstPos3->stPt.s16Y * w3 + pstPos4->stPt.s16Y * w4 + pstPos5->stPt.s16Y * w5 + pstPos6->stPt.s16Y * w6 +0.5);
	}
}

static IMP_VOID ipCalcTargetVelocity( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget )
{
	ipCalculateObjVelocityAndDirection( pstTracker, pstTarget, pstTracker->pstResult->u32FrmTimeCur );
}

static IMP_VOID ipMergeSimilarTargets( IpTargetTrackerMotion *pstTracker )
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetDataTrackerM *pstTargetData;
	IpTargetDataTrackerM *pstTmpTgtData;
	IpTargetPosition  *pstPos0, *pstPos1;
	IMP_RECT_S stTmpRg1, stTmpRg2;
	IMP_S32 s32Dist, s32IntersectArea;
	IMP_S32 i, j, k, s32Cnt, s32StableMinFrmCnt;

	s32StableMinFrmCnt = pstTracker->pstPara->s32StableMinFrmCnt;

	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( !pstTarget[i].s32Used )
			continue;

		if( IMP_IS_TGT_STABLE( pstTarget[i].stTargetInfo.u32Status ) && IMP_IS_TGT_MEASURED( pstTarget[i].stTargetInfo.u32Status ) )
		{
			pstTargetData = (IpTargetDataTrackerM*)pstTarget[i].stPrivateData.pDataTracker;
			for( j = 0; j < IMP_MAX_TGT_CNT; j++)
			{
				if( !pstTarget[j].s32Used )
					continue;

				s32Cnt = 0;
				pstTmpTgtData = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;
				if( j != i && ( pstTmpTgtData->s32TrackedFrameNum == s32StableMinFrmCnt ) )
				{
					for( k = 0; k < s32StableMinFrmCnt; k++ )
					{
						pstPos0 = ipTargetTrajectoryGetPosition( &pstTarget[i].stTrajectory, -k );
						pstPos1 = ipTargetTrajectoryGetPosition( &pstTarget[j].stTrajectory, -k );
						stTmpRg1.s16X1 = pstPos0->stRg.s16X1;
						stTmpRg1.s16Y1 = pstPos0->stRg.s16Y1;
						stTmpRg1.s16X2 = pstPos0->stRg.s16X2;
						stTmpRg1.s16Y2 = pstPos0->stRg.s16Y2;
						stTmpRg2.s16X1 = pstPos1->stRg.s16X1;
						stTmpRg2.s16Y1 = pstPos1->stRg.s16Y1;
						stTmpRg2.s16X2 = pstPos1->stRg.s16X2;
						stTmpRg2.s16Y2 = pstPos1->stRg.s16Y2;
						s32Dist = ipGetMinimumRegionDistance( &stTmpRg1, &stTmpRg2, &s32IntersectArea );
						if ( s32Dist < 8 )
						{
							s32Cnt++;
						}
					}
					if( s32Cnt > s32StableMinFrmCnt - 2 )
					{
						pstTargetData->s32ActualArea += pstTmpTgtData->s32ActualArea;
						pstTargetData->stMinRect.s16X1 = pstTargetData->stMinRect.s16X1<pstTmpTgtData->stMinRect.s16X1 ? pstTargetData->stMinRect.s16X1:pstTmpTgtData->stMinRect.s16X1;
						pstTargetData->stMinRect.s16Y1 = pstTargetData->stMinRect.s16Y1<pstTmpTgtData->stMinRect.s16Y1 ? pstTargetData->stMinRect.s16Y1:pstTmpTgtData->stMinRect.s16Y1;
						pstTargetData->stMinRect.s16X2 = pstTargetData->stMinRect.s16X2>pstTmpTgtData->stMinRect.s16X2 ? pstTargetData->stMinRect.s16X2:pstTmpTgtData->stMinRect.s16X2;
						pstTargetData->stMinRect.s16Y2 = pstTargetData->stMinRect.s16Y2>pstTmpTgtData->stMinRect.s16Y2 ? pstTargetData->stMinRect.s16Y2:pstTmpTgtData->stMinRect.s16Y2;
						pstTargetData->s32ObjArea += pstTmpTgtData->s32ObjArea;
						for( k = 0; k < IMP_GRAY_LEVEL; k++)
							pstTargetData->au32ObjectHistogram[k] += pstTmpTgtData->au32ObjectHistogram[k];

						ipTrackedTargetSetFree( pstTargetSet, j );
					}
				}
			}
		}
	}
}



IMP_VOID ipCorrectPosition(IpDrgDataTrackerM * pstDrgData,IpTrackedTarget *pstTarget,IMP_POINT_S* TcorrectCentroid,IMP_RECT_S* TcorrectRect)
{
	IMP_RECT_S	stMergeRc;
	IMP_RECT_S	stTmpRc;
	IMP_POINT_S  stTmpPt;
	IMP_S32 s32WidthT, s32HeightT;


	IMP_S32 s32OffsetX, s32OffsetY, s32Width, s32Height;
	IpTargetPosition *pstCurPos, *pstPrePos;
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;

	pstPrePos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );
	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );

	s32OffsetX = pstPrePos->stRg.s16X1 - pstPrePos->stPt.s16X;
	s32OffsetY = pstPrePos->stRg.s16Y1 - pstPrePos->stPt.s16Y;
	s32Width  = pstPrePos->stRg.s16X2 - pstPrePos->stRg.s16X1;
	s32Height = pstPrePos->stRg.s16Y2 - pstPrePos->stRg.s16Y1;

	ipPointCopy( &pstTargetData->stPredictedCentroid, &stTmpPt );
	stTmpRc.s16X1 = stTmpPt.s16X + s32OffsetX;
	stTmpRc.s16Y1 = stTmpPt.s16Y + s32OffsetY;
	stTmpRc.s16X2 = stTmpRc.s16X1 + s32Width;
	stTmpRc.s16Y2 = stTmpRc.s16Y1 + s32Height;

	ipRectCopy( &stTmpRc, TcorrectRect );
	ipPointCopy( &pstTargetData->stPredictedCentroid, TcorrectCentroid );



	s32WidthT = TcorrectRect->s16X2 - TcorrectRect->s16X1;
	s32HeightT = TcorrectRect->s16Y2 - TcorrectRect->s16Y1;

	ipRectCopy(&pstDrgData->stMinRect,&stMergeRc);


	if (TcorrectRect->s16X1 < stMergeRc.s16X1)
	{
		s32OffsetX = stMergeRc.s16X1-TcorrectRect->s16X1;
		TcorrectRect->s16X1 = stMergeRc.s16X1;
		TcorrectRect->s16X2 +=  s32OffsetX;
		TcorrectCentroid->s16X += s32OffsetX;
	}

	if (TcorrectRect->s16X2 > stMergeRc.s16X2)
	{
		s32OffsetX = stMergeRc.s16X2-TcorrectRect->s16X2;
		TcorrectRect->s16X2 = stMergeRc.s16X2;
		TcorrectRect->s16X1 +=  s32OffsetX;
		TcorrectCentroid->s16X += s32OffsetX;
	}

	if (TcorrectRect->s16Y1 < stMergeRc.s16Y1)
	{
		s32OffsetY = stMergeRc.s16Y1-TcorrectRect->s16Y1;
		TcorrectRect->s16Y1 = stMergeRc.s16Y1;
		TcorrectRect->s16Y2 +=  s32OffsetY;
		TcorrectCentroid->s16Y += s32OffsetY;
	}

	if (TcorrectRect->s16Y2 > stMergeRc.s16Y2)
	{
		s32OffsetY = stMergeRc.s16Y2-TcorrectRect->s16Y2;
		TcorrectRect->s16Y2 = stMergeRc.s16Y2;
		TcorrectRect->s16Y1 +=  s32OffsetY;
		TcorrectCentroid->s16Y += s32OffsetY;
	}


}


IMP_VOID ipCalcMaxMatchDistanceM( IpTrackerPara *pstPara, IpDrgDataTrackerM *pstDrgData )
{
	IMP_S32 s32IsStatic, s32DiagonalLen, s32MaxMatchDist, s32MaxDist;

	s32IsStatic = IMP_IS_RGN_RSTATIC_L(pstDrgData->u16Status) || IMP_IS_RGN_RSTATIC_R(pstDrgData->u16Status);
	if( s32IsStatic )
		s32MaxDist = pstPara->s32MaxDistanceStatic;
	else
		s32MaxDist = pstPara->s32MaxDistanceMotion;

	if( pstPara->s32DisCrlSize )
	{
		s32DiagonalLen = ipCalcDiagonalLengthM( &pstDrgData->stMinRect );
		s32MaxMatchDist = (IMP_S32)( s32DiagonalLen / ( 1 - pstPara->fMinMatchCoefLow ) );
		if( s32MaxMatchDist > s32MaxDist )
			s32MaxMatchDist = s32MaxDist;
	}
	else
	{
		s32MaxMatchDist = s32MaxDist;
	}
	pstDrgData->s32MaxMatchDist = s32MaxMatchDist;
}

IMP_FLOAT ipCalculateDistanceCoefficientM( const IMP_POINT_S stCurPos, const IMP_POINT_S stPredictPos, IpDrgDataTrackerM *pstDrgData )
{
	IMP_FLOAT s32Dist;
	IMP_S32 s32MaxDist = pstDrgData->s32MaxMatchDist;

	s32Dist = (IMP_FLOAT)ipSqrtInt((IMP_U32)( (stCurPos.s16X - stPredictPos.s16X) * (stCurPos.s16X - stPredictPos.s16X)
					   +(stCurPos.s16Y - stPredictPos.s16Y) * (stCurPos.s16Y - stPredictPos.s16Y) ));
	return (IMP_FLOAT)( 1 - s32Dist / s32MaxDist );
}

IMP_S32 ipCalcDirectionDistCoef( const IMP_POINT_S stPtT, const IMP_POINT_S stPtM, IpDrgDataTrackerM *pstDrgData, IMP_S32 stDirT )
{
	IMP_FLOAT s32Dist;
	IMP_S32 s32Alpha;
	IMP_S32 s32MaxDist = pstDrgData->s32MaxMatchDist;

	s32Alpha = abs(ipCalculateAngleofVector(stPtT,stPtM) - stDirT);

	if(s32Alpha > 180)
		s32Alpha = 360 - s32Alpha;

	if (s32Alpha > 45)
	{
		s32Dist = (IMP_FLOAT)ipSqrtInt((IMP_U32)( (stPtT.s16X - stPtM.s16X)*(stPtT.s16X - stPtM.s16X)+(stPtT.s16Y - stPtM.s16Y)*(stPtT.s16Y - stPtM.s16Y) ));
		if (s32Dist > (s32MaxDist >> 1))
		{
			return 0;
		}
	}
	return 1;
}

#define TARGET_ID_MIN_MOTION	IMP_TGT_ID_NICE_MIN_MOTION
#define TARGET_ID_IMP_MAX_MOTION	IMP_TGT_ID_NICE_MAX_MOTION

IMP_S32 ipAssignObjectIDM( IpTargetTrackerMotion *pstTracker )
{
	pstTracker->u32TargetCurId++;
	if( pstTracker->u32TargetCurId < TARGET_ID_MIN_MOTION || pstTracker->u32TargetCurId > TARGET_ID_IMP_MAX_MOTION )
		pstTracker->u32TargetCurId = TARGET_ID_MIN_MOTION;
	return pstTracker->u32TargetCurId;
}


IMP_VOID ipCalculateObjVelocityAndDirection( IpTargetTrackerMotion *pstTracker, IpTrackedTarget *pstTarget, const IMP_S32 s32CurrentTime )
{
	IMP_S32 i = 0, j = 0;
	IMP_S32 s32CntValid = 0, s32TrajLen;
	IMP_S32 s32StartTime, s32EndTime, s32DeltaTime;
	IMP_FLOAT fVelocityX = 0, fVelocityY = 0;
	IMP_FLOAT fVelXPix = 0,fVelYPix = 0;

	IMP_S32 s32SumX = 0, s32SumY = 0, s32SumXY = 0, s32SumXX = 0;
	IMP_FLOAT fMovingAngle;
	IMP_S32 s32Quadrant = -1;
	IMP_S32 s32IsFstValidPt = IMP_TRUE;
	IMP_POINT_S stStartPt, stEndPt, stFirstPt;
	IpTargetPosition *pstPos0;
	IpTargetPosition *pstPosFillVel;

	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpTargetTrajectory *pstTraject = &pstTarget->stTrajectory;

	s32TrajLen = ipTargetTrajectoryGetLength(pstTraject);
	if( s32TrajLen < pstTracker->pstPara->s32FrmCntCalVelocity )
		return;
	memset( &stStartPt, 0, sizeof(IMP_POINT_S) );
	memset( &stEndPt, 0, sizeof(IMP_POINT_S) );
	memset( &stFirstPt, 0, sizeof(IMP_POINT_S) );
	pstPos0 = ipTargetTrajectoryGetPosition( pstTraject, i );
	while( (-i) < s32TrajLen )
	{
		if( (s32CntValid + 1) >= pstTracker->pstPara->s32FrmCntCalVelocity
			&& (IMP_U32)(s32CurrentTime - pstPos0->u32Time) * (pstTracker->pstPara->s32FrmDura) >= (IMP_U32)pstTracker->pstPara->s32TimeCalVelocity )
			break;

// 		if ( pstPos0->s32Used )
		{
			if ( s32IsFstValidPt )
			{
				s32IsFstValidPt = IMP_FALSE;
				s32StartTime = pstPos0->u32Time;
				s32EndTime = pstPos0->u32Time;
				memcpy( &stFirstPt, &(pstPos0->stPt), sizeof(IMP_POINT_S) );
				memcpy( &stStartPt, &(pstPos0->stPt), sizeof(IMP_POINT_S) );
				memcpy( &stEndPt, &(pstPos0->stPt), sizeof(IMP_POINT_S) );
			}
			else
			{
				++s32CntValid;
				s32EndTime = s32StartTime;
				s32StartTime = pstPos0->u32Time;
				s32DeltaTime = (s32EndTime-s32StartTime)*pstTracker->pstPara->s32FrmDura;
				memcpy( &stEndPt, &stStartPt, sizeof(IMP_POINT_S) );
				memcpy( &stStartPt, &(pstPos0->stPt), sizeof(IMP_POINT_S) );

				//FDEPTH_get_point_resolution( fdpeth, stEndPt.s16X, stEndPt.s16Y, &dResX, &dResY );
				//ptResolusion = dResX;
				//fVelocityX += (IMP_FLOAT)( (stEndPt.s16X - stStartPt.s16X)*ptResolusion/s32DeltaTime );
				//fVelocityY += (IMP_FLOAT)( (stEndPt.s16Y - stStartPt.s16Y)*ptResolusion/s32DeltaTime );

				fVelXPix += ((IMP_FLOAT) (stEndPt.s16X - stStartPt.s16X) / s32DeltaTime );
				fVelYPix += ((IMP_FLOAT) (stEndPt.s16Y - stStartPt.s16Y) / s32DeltaTime );

				s32SumX += stEndPt.s16X;
				s32SumY += stEndPt.s16Y;
				s32SumXX += (stEndPt.s16X) * (stEndPt.s16X);
				s32SumXY += stEndPt.s16X * stEndPt.s16Y;
			}

		}
		i--;
		pstPos0 = ipTargetTrajectoryGetPosition( pstTraject, i );
	}


	if ( fVelXPix >= 0 )
	{
		if ( fVelYPix >= 0 )
			s32Quadrant = 1;
		else
			s32Quadrant = 4;
	}
	else
	{
		if ( fVelYPix >= 0 )
			s32Quadrant = 2;
		else
			s32Quadrant = 3;
	}
	if(fabs(fVelXPix) > 0.000001)
		fMovingAngle = (IMP_FLOAT)atan(fVelYPix / fVelXPix);
	else
		fMovingAngle = 0;
	switch (s32Quadrant)
	{
		case 1:
			break;
		case 2:
		case 3:
			fMovingAngle += (IMP_FLOAT)IMP_PI;
			break;
		case 4:
			fMovingAngle += (IMP_FLOAT)IMP_PI * 2;
			break;
		default:
			break;
	}
	if ( fMovingAngle < 0 )
		fMovingAngle += (IMP_FLOAT)IMP_PI * 2;

    fVelocityX = fVelocityX / s32CntValid;
	fVelocityY = fVelocityY / s32CntValid;

	fVelXPix = fVelXPix / s32CntValid;
	fVelYPix = fVelYPix / s32CntValid;

	pstTargetData->fVelocityX = fVelXPix ;
	pstTargetData->fVelocityY = fVelYPix ;

	pstPos0 = ipTargetTrajectoryGetPosition( pstTraject, 0 );
	pstPos0->s32Direction = (IMP_S32)(fMovingAngle * 180 / IMP_PI);

	pstPos0->s32Velocity = (IMP_S32)(1000 * sqrt(fVelXPix * fVelXPix + fVelYPix * fVelYPix));
	if (!pstTargetData->s32HaveFilledVelocity)
	{
		pstTargetData->s32HaveFilledVelocity = 1;
		for (j = s32TrajLen - 1; j > 0; j--)
		{
			pstPosFillVel = ipTargetTrajectoryGetPosition( pstTraject, -j );
			pstPosFillVel->s32Direction = pstPos0->s32Direction;
			pstPosFillVel->s32Velocity = pstPos0->s32Velocity;
		}
	}
}


static IMP_VOID ipGroupClear(IpTargetTrackerMotion *pstTracker)
{

	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	IpTargetSetDataTrackerM* pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IMP_S32* ps32GroupUsedTotal = &pstTargetSetData->stGroupSet.s32UsedTotal;
	IMP_S32 k;

	*ps32GroupUsedTotal = 0;

	for (k = 0; k < IMP_MAX_GROUP_CNT; k++)
	{
		pstTargetSetData->stGroupSet.astGroups[k].s32UsedTotal = 0;
	}

}


static IMP_VOID ipGroupUpdateRegister(IpTargetTrackerMotion *pstTracker)
{

	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetSetDataTrackerM* pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;

	IpTargetDataTrackerM *pstTargetData;
	IMP_S32 s32FlagTgtIdExist;
	IMP_S32 s32FlagMemberValid;
	IMP_S32 s32FlagGroupValid;
	IMP_S32 i, j, k, jc,m,mc;


	{

		for (i = 0; i < pstTargetSetData->stGroupSet.s32UsedTotal; i++)
		{
			for (j = 0; j < pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal; j++)
			{
				if (pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j] == 0)
					continue;
				pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
				if(pstTargetSetData->stGroupSet.astGroups[i].s32ToBeDeleted[j] == IMP_TRUE)
				{
					pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j] = NULL;
					pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j] = 0;
					pstTargetSetData->stGroupSet.astGroups[i].s32ToBeDeleted[j] = IMP_FALSE;
					pstTargetData->cs.pstGroup = NULL;
					pstTargetData->cs.s32CollisionLength = 0;
				}
			}
		}


		for (i = 0; i < pstTargetSetData->stGroupSet.s32UsedTotal; i++)
		{

			for (j = 0; j < pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal; j++)
			{

				s32FlagTgtIdExist=IMP_FALSE;
				if (pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j] != 0)
				{
					for( m = 0, mc = 0; m < IMP_MAX_TGT_CNT && mc < (pstTargetSet->s32UsedTotal); m++ )
					{
						if(!pstTarget[m].s32Used)
							continue;
						mc++;

						if (pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j] == pstTarget[m].u32TargetId)
						{
							s32FlagTgtIdExist = IMP_TRUE;
							break;
						}
					}
				}


				if(!s32FlagTgtIdExist)
				{
					if(pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal)
					{
						pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal--;
						for (k=j;k<pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal;k++)
						{
							pstTargetSetData->stGroupSet.astGroups[i].pastTargets[k]=pstTargetSetData->stGroupSet.astGroups[i].pastTargets[k+1];
							pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[k]=pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[k+1];
						}
						j--;
					}
				}

			}

			if (pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal == 1)
			{
 				pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[0]->stPrivateData.pDataTracker;
				pstTargetData->cs.pstGroup = NULL;
				pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal--;
			}


			if (pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal == 0)
			{
				if(pstTargetSetData->stGroupSet.s32UsedTotal)
				{
					pstTargetSetData->stGroupSet.s32UsedTotal--;
					for (k = i; k < pstTargetSetData->stGroupSet.s32UsedTotal; k++)
					{
						pstTargetSetData->stGroupSet.astGroups[k]=pstTargetSetData->stGroupSet.astGroups[k+1];
						for( m = 0, mc = 0; m < IMP_MAX_TGT_CNT && mc < (pstTargetSet->s32UsedTotal); m++ )
						{
							if(!pstTarget[m].s32Used)
								continue;
							mc++;
							pstTargetData = (IpTargetDataTrackerM*)pstTarget[m].stPrivateData.pDataTracker;
			 				if (&pstTargetSetData->stGroupSet.astGroups[k+1] == pstTargetData->cs.pstGroup)
							{
								pstTargetData->cs.pstGroup = &pstTargetSetData->stGroupSet.astGroups[k];
							}
						}
					}
					pstTargetSetData->stGroupSet.astGroups[k].s32UsedTotal = 0;
					i--;
				}
			}
		}


		for (i = 0; i < IMP_MAX_GROUP_CNT; i++)
		{
			if (pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal == 0)
			{

				pstTargetSetData->stGroupSet.astGroups[i].s32GroupArea = 0;
				pstTargetSetData->stGroupSet.astGroups[i].s32MemberMaxArea = 0;
				pstTargetSetData->stGroupSet.astGroups[i].s32MemberMinArea = 0;
				pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaRegions = 0;
				pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaTargets = 0;

				for (j = 0; j < IMP_MAX_TARGET_CNT_IN_GROUP; j++)
				{
					pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j] = 0;
					pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j] = 0;
					pstTargetSetData->stGroupSet.astGroups[i].s32ToBeDeleted[j] = IMP_FALSE;
				}
			}
		}




		{
			for (i = 0; i < IMP_MAX_GROUP_CNT; i++)
			{
				for (j = 0; j < IMP_MAX_TARGET_CNT_IN_GROUP; j++)
				{
					pstTargetSetData->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j] = 0;
				}
			}
		}

	}//s
}

static IMP_VOID ipGroupAddMember(IpTargetTrackerMotion *pstTracker,IMP_S32 s32Frame,IMP_U32 u32TgtId,IMP_S32 s32GroupId,IMP_S32 s32MemberId)
{


	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetSetDataTrackerM* pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTargetData;
	IMP_S32 j, jc;

	if( pstTracker->pstResult->u32FrmTimeCur == (IMP_U32)s32Frame)
	for( j = 0, jc = 0; j < IMP_MAX_TGT_CNT && jc < (pstTargetSet->s32UsedTotal); j++ )
	{
		pstTargetData = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;

		if( !pstTarget[j].s32Used)
			continue;
		jc++;
		if (pstTarget[j].u32TargetId-0x90000000 == u32TgtId)
		{
			pstTargetData->cs.pstGroup=&pstTargetSetData->stGroupSet.astGroups[s32GroupId];
			pstTargetSetData->stGroupSet.astGroups[s32GroupId].pastTargets[s32MemberId] = &pstTarget[j];
			pstTargetSetData->stGroupSet.astGroups[s32GroupId].au32TargetIds[s32MemberId]=pstTarget[j].u32TargetId;
			pstTargetSetData->stGroupSet.astGroups[s32GroupId].s32UsedTotal++;
			if (pstTargetSetData->stGroupSet.astGroups[s32GroupId].s32UsedTotal == 1)
			{
				pstTargetSetData->stGroupSet.s32UsedTotal++;
			}
		}
	}
}

static IMP_VOID ipGroupDeleteMember(IpTargetTrackerMotion *pstTracker,IMP_S32 s32Frame,IMP_S32 s32GroupId,IMP_S32 s32MemberId)
{


	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetSetDataTrackerM* pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTargetData;
	IpGroup *pstTmpGroup;
	IMP_S32 j;

	if(pstTracker->pstResult->u32FrmTimeCur == (IMP_U32)s32Frame)
	{
		pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[s32GroupId].pastTargets[s32MemberId]->stPrivateData.pDataTracker;
		pstTmpGroup = pstTargetData->cs.pstGroup;
		for (j = 0; j < pstTargetSetData->stGroupSet.astGroups[s32GroupId].s32UsedTotal; j++)
		{
			if (j != s32MemberId && pstTargetSetData->stGroupSet.astGroups[s32GroupId].pastTargets[j] == pstTargetSetData->stGroupSet.astGroups[s32GroupId].pastTargets[s32MemberId])
			{
				pstTargetData->cs.pstGroup = pstTmpGroup;
			}
			else
			{
				pstTargetData->cs.pstGroup = NULL;
			}
		}
		pstTargetSetData->stGroupSet.astGroups[s32GroupId].pastTargets[s32MemberId] = NULL;
		pstTargetSetData->stGroupSet.astGroups[s32GroupId].au32TargetIds[s32MemberId] = 0;
	}
}

static IMP_VOID ipGroupTest(IpTargetTrackerMotion *pstTracker)
{


	ipGroupUpdateRegister(pstTracker);

	{
		ipGroupAddMember(pstTracker,125,77,0,0);
	}

	{
		ipGroupAddMember(pstTracker,125,85,0,1);
		ipGroupAddMember(pstTracker,125,77,0,2);
		ipGroupAddMember(pstTracker,125,85,0,3);
	}

	{
		ipGroupDeleteMember(pstTracker,127,0,1);
	}

	{
		ipGroupDeleteMember(pstTracker,129,0,2);
	}
}

static IMP_VOID ipGroupUpdateAreaInfo(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetSetDataTrackerM* pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTargetData;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IMP_S32 s32TmpMax, s32TmpMin;
	IMP_S32 i, j;

	for (i = 0;i<pstTargetSetData->stGroupSet.s32UsedTotal;i++)
	{
			pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[0]->stPrivateData.pDataTracker;
			s32TmpMax = pstTargetData->cs.sampleTardat.s32ObjArea;
			s32TmpMin = pstTargetData->cs.sampleTardat.s32ObjArea;
			pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaTargets=pstTargetData->cs.sampleTardat.s32ObjArea;
			pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaRegions=0;


			for (j = 1; j < pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal; j++)
			{
				pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
				pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaTargets += pstTargetData->cs.sampleTardat.s32ObjArea;
				if (pstTargetData->cs.sampleTardat.s32ObjArea > s32TmpMax)
				{
					s32TmpMax = pstTargetData->cs.sampleTardat.s32ObjArea;
				}
				else if (pstTargetData->cs.sampleTardat.s32ObjArea < s32TmpMin)
				{
					s32TmpMin = pstTargetData->cs.sampleTardat.s32ObjArea;
				}

			}
			pstTargetSetData->stGroupSet.astGroups[i].s32MemberMaxArea = s32TmpMax;
			pstTargetSetData->stGroupSet.astGroups[i].s32MemberMinArea = s32TmpMin;


			if (pstTargetSetData->stGroupSet.astGroups[i].s32ToBeDeleted[0])
			{
				for (j = 0; j < IMP_MAX_TARGET_CNT_IN_GROUP && pstTargetSetData->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j]; j++)
				{
					pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaRegions += pstDRegion[ pstTargetSetData->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j] ].s32AreaPixel;
				}
#ifdef DEBUG_OBJRECG_TRACKER_INFO2
				info2 += sprintf( info2, "IdxD: ");
				for (j = 0; j < IMP_MAX_TARGET_CNT_IN_GROUP; j++)
				{
					if (pstTargetSetData->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j])
					{
						info2 += sprintf( info2, "%03d_%03d ",pstTargetSetData->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j],
														 pstDRegion[pstTargetSetData->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j]].s32AreaPixel);
					}
				}
				info2 += sprintf( info2, "IdxT: ");
				for (j = 0; j < IMP_MAX_TARGET_CNT_IN_GROUP; j++)
				{
					if (pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j])
					{
 						pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
						info2 += sprintf( info2, "%03d_%03d ",pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j]-0x90000000, pstTargetData->cs.sampleTardat.s32ObjArea);
					}
				}
				info2 += sprintf( info2, "\n");
#endif
			}
	}
}

#ifdef DEBUG_OBJRECG_TRACKER_INFO2
static IMP_VOID ipGroupInfoDisplay(IpTargetTrackerMotion *pstTracker)
{

	////////////////////////////////////////////////////
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstRegionSet->astDrg;
	IpTargetSetDataTrackerM* pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IMP_S32* ps32GroupUsedTotal = &pstTargetSetData->stGroupSet.s32UsedTotal;
	IpDrgDataTrackerM *pstDrgData;
	IpTargetDataTrackerM *pstTargetData;
	IMP_S32 i,j;


	{
		IMP_S32 s32CollisionCnt=0;
		IMP_S32 s32SplitCnt = 0;

		for( i = 1; i < IMP_MAX_TGT_CNT - 1; i++ )
		{
			pstDrgData = (IpDrgDataTrackerM*)pstDRegion[i].stPrivateData.pDataTracker;
			if(pstDrgData->s32Collision)
				s32CollisionCnt++;
			if(pstDrgData->s32Split)
				s32SplitCnt++;
		}


		info2 += sprintf( info2, "RGC:%01d RGS:%01d   \n",s32CollisionCnt,s32SplitCnt);
	}


	{

		info2 += sprintf( info2, "Groups:%01d    ",pstTargetSetData->stGroupSet.s32UsedTotal);

		info2 += sprintf( info2, "Members: ",pstTargetSetData->stGroupSet.s32UsedTotal);
		for (i = 0; i < IMP_MAX_GROUP_CNT; i++)
		{

			info2 += sprintf( info2, "%01d->%02d    ",i,pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal);
		}

		//info2 += sprintf( info2, "AR:%.3f",fSplitAreaRate);
		info2 += sprintf( info2, "\n");


		for (i = 0; i < IMP_MAX_GROUP_CNT; i++)
		{
//			if (!pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal)
// 				continue;


			info2 += sprintf( info2, "%01d (%03d,%03d,%03d),S(%01d)%01d,[%03d,%03d] - ",
																							i,
																							pstTargetSetData->stGroupSet.astGroups[i].s32GroupArea,
																							pstTargetSetData->stGroupSet.astGroups[i].s32MemberMaxArea,
																							pstTargetSetData->stGroupSet.astGroups[i].s32MemberMinArea,
																							pstTargetSetData->stGroupSet.astGroups[i].s32StablishCollision,
																							pstTargetSetData->stGroupSet.astGroups[i].s32StableTime,
																							pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaTargets,
																							pstTargetSetData->stGroupSet.astGroups[i].s32SumAreaRegions);//组号

			for (j = 0; j <pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal; j++)
			{
 				pstTargetData = (IpTargetDataTrackerM *)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;


				info2 += sprintf( info2, " %d(%X)%01d_%04d_%03d ",
																	pstTargetSetData->stGroupSet.astGroups[i].au32TargetIds[j] - 0x90000000,
																	pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j],
																	pstTargetSetData->stGroupSet.astGroups[i].s32ToBeDeleted[j],
																	pstTargetData->cs.s32CollisionLength,
																	pstTargetData->cs.sampleTardat.s32ObjArea);
			}
			info2 += sprintf( info2, "\n");
		}
	}
}
#endif
///////////////////////////////////////////////////////////////


extern IMP_VOID ipUpdateHist( IMP_U32* hist, IMP_U32* newHist, IMP_FLOAT alpha, IMP_U32 bins);
extern IMP_VOID ipUpdateInt16_t( IMP_S16* value, IMP_S16* newValue, IMP_FLOAT alpha, IMP_U16 num);
extern IMP_VOID ipUpdateInt32_t( IMP_S32* value, IMP_S32* newValue, IMP_FLOAT alpha, IMP_U32 num);
extern IMP_VOID ipUpdateReal( IMP_FLOAT* value, IMP_FLOAT* newValue, IMP_FLOAT alpha, IMP_U32 num);
extern IMP_VOID ipUpdateRect( IMP_RECT_S* stRect, IMP_RECT_S* newRect, IMP_FLOAT alpha, IMP_S32 imgWidth, IMP_S32 imgHeight);



static IMP_VOID ipUpdateTargetFromRegionM( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
										  PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex,IpTargetTrackerMotion *pstTracker)
{
	IpTargetPosition *pstCurPos,*pstPrePos;
	PEA_DETECTED_REGION_S *pstDRegion = &pstRegionSet->astDrg[s32DrgIndex];
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpDrgDataTrackerM *pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;
	IMP_FLOAT fAreaUpdateRate = pstTracker->pstPara->fAreaUpdateRate;
	IMP_FLOAT fSizeUpdateRate = pstTracker->pstPara->fSizeUpdateRate;
	IMP_FLOAT fHistUpdateRate = pstTracker->pstPara->fHistUpdateRate;


	pstTargetData->s32LastTime = pstDrgData->s32CurrentTime;
	pstTargetData->stCurCentroid.s16X = pstDrgData->stCurCentroid.s16X;
	pstTargetData->stCurCentroid.s16Y = pstDrgData->stCurCentroid.s16Y;

	pstPrePos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );


	if( IMP_IS_TPOS_BLOB_MEASURED(pstPrePos->s32Used) && IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) )
	{
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
		IMP_S32 ap = pstTargetData->s32ObjArea;
		IMP_S32 aa = pstTargetData->s32ActualArea;
#endif
		ipUpdateInt32_t( &pstTargetData->s32ObjArea, &pstDRegion->s32AreaPixel, fAreaUpdateRate, 1);
		ipUpdateInt32_t( &pstTargetData->s32ActualArea, &pstDRegion->s32AreaActual, fAreaUpdateRate, 1);
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
//	info2 += sprintf( info2, "ap0:%d, aa0:%d | newAP:%d, newAA:%d | ap1:%d, aa1:%d | Δap:%d, Δaa:%d\n",
//								ap,aa,
//								pstDRegion->s32AreaPixel,pstDRegion->s32AreaPixel,
//								pstTargetData->s32ObjArea,pstTargetData->s32ActualArea,
//								pstDRegion->s32AreaPixel-ap,pstDRegion->s32AreaPixel-aa);
#endif

		ipUpdateRect( &pstTargetData->stMinRect, &pstDrgData->stMinRect, fSizeUpdateRate, pstTracker->pstResult->s32Width, pstTracker->pstResult->s32Height);     //更新尺寸

#ifdef	TRACKER_USE_RGB_INFO
		ipUpdateHist( pstTargetData->au32ObjectHistogram, pstDrgData->au32ObjectHistogram, fHistUpdateRate, RGB_LEVEL);
		ipUpdateReal( pstTargetData->E, pstDrgData->E, fHistUpdateRate, 3);
		ipUpdateReal( pstTargetData->sigma,pstTargetData->sigma, fHistUpdateRate, 3);
		ipUpdateReal( pstTargetData->skewness, pstTargetData->skewness, fHistUpdateRate, 3);
#else
		ipUpdateHist( pstTargetData->au32ObjectHistogram, pstDrgData->au32ObjectHistogram, fHistUpdateRate, IMP_GRAY_LEVEL);
#endif

	}

	else
	{
		pstTargetData->s32ObjArea = pstDRegion->s32AreaPixel;
		pstTargetData->s32ActualArea = pstDRegion->s32AreaPixel;
		pstTargetData->stMinRect = pstDrgData->stMinRect;


		memcpy( pstTargetData->au32ObjectHistogram, pstDrgData->au32ObjectHistogram, IMP_GRAY_LEVEL*sizeof(IMP_U32) );


	}


	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	IMP_SET_TPOS_MEASURED(pstCurPos->s32Used);
	pstCurPos->u32Time = pstDrgData->s32CurrentTime;

	pstCurPos->u32AreaPixel = pstTargetData->s32ObjArea;
	pstCurPos->u32AreaActual = pstTargetData->s32ActualArea;
	pstCurPos->u32DrgType = pstDRegion->u16Status;
	pstCurPos->stPt = pstTargetData->stCurCentroid;
	pstCurPos->stRg = pstTargetData->stMinRect;


	pstTarget->stTargetImage.pstDRegions = pstRegionSet;
	pstTarget->stTargetImage.s32DrgIndex = s32DrgIndex;
	IMP_ASSERT(pstTarget->stTargetImage.pstDRegions!=0);
	IMP_ASSERT(pstTarget->stTargetImage.s32DrgIndex!=0);

	IMP_SET_TGT_MEASURED( pstTarget->stTargetInfo.u32Status );					//OK
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_COLLISION( pstTarget->stTargetInfo.u32Status );



	{
		pstTargetData->cs.sampleTardat.s32ObjArea = pstCurPos->u32AreaPixel;
		pstTargetData->cs.sampleTardat.s32ActualArea = pstCurPos->u32AreaActual;
 	}



}


static IMP_VOID CorrectPosition(IpDrgDataTrackerM * pstDrgData,IpTrackedTarget *pstTarget,IMP_POINT_S* TcorrectCentroid,IMP_RECT_S* TcorrectRect)
{
	IMP_RECT_S	mergeRect;
	IMP_RECT_S	tmpRect;
	IMP_POINT_S  tmpPt;
//	IMP_S32 TWidth,THeight;
//	IMP_S32 *minx, *miny, *maxx, *maxy;


	//////////////////////////////
	IMP_S32 offsetX, offsetY, width, height;
//	IpTargetPosition *pstCurPos;
	IpTargetPosition *pstPrePos;
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;

	pstPrePos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );
//	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );

	offsetX = pstPrePos->stRg.s16X1 - pstPrePos->stPt.s16X;
	offsetY = pstPrePos->stRg.s16Y1 - pstPrePos->stPt.s16Y;
	width  = pstPrePos->stRg.s16X2 - pstPrePos->stRg.s16X1 + 1;
	height = pstPrePos->stRg.s16Y2 - pstPrePos->stRg.s16Y1 + 1;

	ipPointCopy( &pstTargetData->stPredictedCentroid, &tmpPt );
	tmpRect.s16X1 = tmpPt.s16X + offsetX;
	tmpRect.s16Y1 = tmpPt.s16Y + offsetY;
	tmpRect.s16X2 = tmpRect.s16X1 + width -1;
	tmpRect.s16Y2 = tmpRect.s16Y1 + height -1;

	ipRectCopy( &tmpRect, TcorrectRect );
	ipPointCopy( &tmpPt, TcorrectCentroid );


	/////////////////////////

//	TWidth = TcorrectRect->s16X2 - TcorrectRect->s16X1 + 1;
//	THeight= TcorrectRect->s16Y2 - TcorrectRect->s16Y1 + 1;

	ipRectCopy(&pstDrgData->stMinRect,&mergeRect);


	if (TcorrectRect->s16X1 < mergeRect.s16X1)
	{
		offsetX = mergeRect.s16X1-TcorrectRect->s16X1;
		TcorrectRect->s16X1 = mergeRect.s16X1;
		TcorrectRect->s16X2 +=  offsetX;
		TcorrectCentroid->s16X += offsetX;
	}

	if (TcorrectRect->s16X2 > mergeRect.s16X2)
	{
		offsetX = mergeRect.s16X2-TcorrectRect->s16X2;
		TcorrectRect->s16X2 = mergeRect.s16X2;
		TcorrectRect->s16X1 +=  offsetX;
		TcorrectCentroid->s16X += offsetX;
	}

	if (TcorrectRect->s16Y1 < mergeRect.s16Y1)
	{
		offsetY = mergeRect.s16Y1-TcorrectRect->s16Y1;
		TcorrectRect->s16Y1 = mergeRect.s16Y1;
		TcorrectRect->s16Y2 +=  offsetY;
		TcorrectCentroid->s16Y += offsetY;
	}

	if (TcorrectRect->s16Y2 > mergeRect.s16Y2)
	{
		offsetY = mergeRect.s16Y2-TcorrectRect->s16Y2;
		TcorrectRect->s16Y2 = mergeRect.s16Y2;
		TcorrectRect->s16Y1 +=  offsetY;
		TcorrectCentroid->s16Y += offsetY;
	}


}

static IMP_VOID ipUpdateTargetFromCollision( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
										PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex, IpTargetTrackerMotion *pstTracker)
{
	IpTargetPosition *pstCurPos;
	PEA_DETECTED_REGION_S *pstDRegion = &pstRegionSet->astDrg[s32DrgIndex];
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpDrgDataTrackerM *pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;
    IMP_RECT_S	correctRect;
	IMP_POINT_S  correctCentroid;


	pstTargetData->s32LastTime = pstDrgData->s32CurrentTime;
	pstTargetData->stCurCentroid.s16X = pstDrgData->stCurCentroid.s16X;
	pstTargetData->stCurCentroid.s16Y = pstDrgData->stCurCentroid.s16Y;
	pstTargetData->s32ObjArea = pstTargetData->cs.sampleTardat.s32ObjArea;
	pstTargetData->s32ActualArea = pstTargetData->cs.sampleTardat.s32ActualArea;

	memcpy( &pstTargetData->stMinRect, &pstDrgData->stMinRect, sizeof(IMP_RECT_S) );


	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	IMP_SET_TPOS_COLLISION(pstCurPos->s32Used);
	IMP_SET_TPOS_MEASURED(pstCurPos->s32Used);
	pstCurPos->u32Time = pstDrgData->s32CurrentTime;
	pstCurPos->u32AreaPixel = pstTargetData->cs.sampleTardat.s32ObjArea;
	pstCurPos->u32AreaActual = pstTargetData->cs.sampleTardat.s32ActualArea;
	pstCurPos->u32DrgType = pstDRegion->u16Status;


	ipPointCopy( &pstTargetData->stCurCentroid, &pstCurPos->stPt );
	ipRectCopy( &pstTargetData->stMinRect, &pstCurPos->stRg );


	if (pstTracker->pstPara->s32CsRefinedTargetDisplay)
	{
		CorrectPosition(pstDrgData,pstTarget,&correctCentroid,&correctRect);

		ipPointCopy( &correctCentroid, &pstCurPos->stPt );
		ipRectCopy( &correctRect, &pstCurPos->stRg );
	}


	pstTarget->stTargetImage.pstDRegions = pstRegionSet;
	pstTarget->stTargetImage.s32DrgIndex = s32DrgIndex;
	IMP_ASSERT(pstTarget->stTargetImage.pstDRegions!=0);
	IMP_ASSERT(pstTarget->stTargetImage.s32DrgIndex!=0);

	IMP_SET_TGT_MEASURED( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
	IMP_SET_TGT_COLLISION( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_ABNORMAL( pstTarget->stTargetInfo.u32Status );


	ipRectPointImgPositionCorrect(&pstTargetData->stMinRect, &pstTargetData->stCurCentroid, pstTracker->pstResult->s32Width, pstTracker->pstResult->s32Height, DO_CHECK);

}

static IMP_VOID ipUpdateTargetFromSplit( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
										  PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex,IpTargetTrackerMotion *pstTracker)
{
	IpTargetPosition *pstCurPos;
//	IpTargetPosition *pstPrePos;
	PEA_DETECTED_REGION_S *pstDRegion = &pstRegionSet->astDrg[s32DrgIndex];
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpDrgDataTrackerM *pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;
//	IMP_FLOAT fAreaUpdateRate = pstPara->fAreaUpdateRate;
//	IMP_FLOAT fSizeUpdateRate = pstPara->fSizeUpdateRate;
//	IMP_FLOAT fHistUpdateRate = pstPara->fHistUpdateRate;


	pstTargetData->s32LastTime = pstDrgData->s32CurrentTime;
	pstTargetData->stCurCentroid.s16X = pstDrgData->stCurCentroid.s16X;
	pstTargetData->stCurCentroid.s16Y = pstDrgData->stCurCentroid.s16Y;
//	pstPrePos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );


	pstTargetData->s32ObjArea = pstDRegion->s32AreaPixel;
	pstTargetData->s32ActualArea = pstDRegion->s32AreaPixel;
	pstTargetData->stMinRect = pstDrgData->stMinRect;





	memcpy( pstTargetData->au32ObjectHistogram, pstDrgData->au32ObjectHistogram, IMP_GRAY_LEVEL*sizeof(IMP_U32) );




	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	IMP_SET_TPOS_MEASURED(pstCurPos->s32Used);
	pstCurPos->u32Time = pstDrgData->s32CurrentTime;

	pstCurPos->u32AreaPixel = pstTargetData->s32ObjArea;
	pstCurPos->u32AreaActual = pstTargetData->s32ActualArea;
	pstCurPos->u32DrgType = pstDRegion->u16Status;
	pstCurPos->stPt = pstTargetData->stCurCentroid;
	pstCurPos->stRg = pstTargetData->stMinRect;


	pstTarget->stTargetImage.pstDRegions = pstRegionSet;
	pstTarget->stTargetImage.s32DrgIndex = s32DrgIndex;
	IMP_ASSERT(pstTarget->stTargetImage.pstDRegions!=0);
	IMP_ASSERT(pstTarget->stTargetImage.s32DrgIndex!=0);

	IMP_SET_TGT_MEASURED( pstTarget->stTargetInfo.u32Status );					//OK
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_COLLISION( pstTarget->stTargetInfo.u32Status );


 	{
		pstTargetData->cs.sampleTardat.s32ObjArea = pstCurPos->u32AreaPixel;
		pstTargetData->cs.sampleTardat.s32ActualArea = pstCurPos->u32AreaActual;
 	}



}

static IMP_VOID ipNewTargetFromRegionM( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
									   PEA_DETECTED_REGIONSET_S *pstRegionSet, IMP_S32 s32DrgIndex,
									   IMP_U32 u32TargetId ,IpTargetTrackerMotion *pstTracker)
{
	PEA_DETECTED_REGION_S *pstDRegion = &pstRegionSet->astDrg[s32DrgIndex];
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	IpDrgDataTrackerM *pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;
	IpTargetPosition *pstCurPos;

	IMP_SET_TGT_INIT( pstTarget->stTargetInfo.u32Status );
	pstTarget->s32Used = IMP_TRUE;
	pstTarget->u32TargetId = u32TargetId;
	pstTargetData->s32FirstTime = pstDrgData->s32CurrentTime;
	pstTargetData->s32LastTime = pstDrgData->s32CurrentTime;
	pstTargetData->s32MatchedWith = s32DrgIndex;
	pstTargetData->fMatchCoefficient = 1;
	pstTargetData->s32TrackedFrameNum = 1;
	ipTargetTrajectoryNewPosition( &pstTarget->stTrajectory );

	ipUpdateTargetFromRegionM( pstTarget, s32TargetIndex, pstRegionSet, s32DrgIndex, pstTracker );

	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	memcpy( &pstTargetData->stFirstPos, pstCurPos, sizeof(IpTargetPosition) );

	IMP_SET_TGT_MEASURED( pstTarget->stTargetInfo.u32Status );					//OK
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
	IMP_ASSERT(pstTarget->stTargetImage.pstDRegions!=0);
	IMP_ASSERT(pstTarget->stTargetImage.s32DrgIndex!=0);

	IMP_SET_TPOS_MEASURED(pstCurPos->s32Used);
}

static IMP_VOID ipUpdateTargetFromCollisionStable( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex, IpTargetTrackerMotion *pstTracker)
{
//	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
//	TARSETDAT_TRACKER_M* tarsetdat = (TARSETDAT_TRACKER_M*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetPosition *pstCurPos,*last_pos;
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;


	pstTargetData->s32LastTime = pstTracker->pstResult->u32FrmTimeCur;


	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	last_pos= ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );

	IMP_SET_TPOS_PREDICTED(pstCurPos->s32Used);//?
	IMP_SET_TPOS_COLLISION(pstCurPos->s32Used);
	pstCurPos->u32Time = pstTracker->pstResult->u32FrmTimeCur;
	pstCurPos->u32AreaPixel = last_pos->u32AreaPixel;
	pstCurPos->u32AreaActual = last_pos->u32AreaActual;
	pstCurPos->u32DrgType = last_pos->u32DrgType;

	ipPointCopy( &last_pos->stPt, &pstCurPos->stPt );
	ipRectCopy( &last_pos->stRg, &pstCurPos->stRg );


	pstTarget->stTargetImage.pstDRegions = NULL;
	pstTarget->stTargetImage.s32DrgIndex = 0;


	IMP_SET_TGT_PREDICTED( pstTarget->stTargetInfo.u32Status );					//OK
	IMP_SET_TGT_COLLISION( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
}

static IMP_VOID ipUpdateTargetFromCollisionDestroy( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex, IpTargetTrackerMotion *pstTracker)
{
//	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
//	TARSETDAT_TRACKER_M* tarsetdat = (TARSETDAT_TRACKER_M*)pstTargetSet->stPrivateData.pDataTracker;
	IMP_S32 offsetX, offsetY, width, height;
	IpTargetPosition tmp_pos;
	IpTargetPosition *pstCurPos, *pstPrePos, *collision_pos=&tmp_pos;
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;

//	PRINT_TARGET2( pstTracker, pstTarget );

	pstTargetData->stCurCentroid.s16X = pstTargetData->stPredictedCentroid.s16X;
	pstTargetData->stCurCentroid.s16Y = pstTargetData->stPredictedCentroid.s16Y;


	IMP_ASSERT(pstTargetData->cs.s32CollisionLength>=0);
//	collision_pos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -(pstTargetData->cs.s32CollisionLength+1) );//样本保存问题
	pstPrePos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );
	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );

	collision_pos->u32AreaPixel = pstTargetData->cs.sampleTardat.s32ObjArea;
	collision_pos->u32AreaActual = pstTargetData->cs.sampleTardat.s32ActualArea;
	collision_pos->s32Direction = pstTargetData->cs.sampleTardat.s32Direction;
	collision_pos->stPt = pstTargetData->cs.sampleTardat.stCentroid;
	collision_pos->stRg = pstTargetData->cs.sampleTardat.stRect;


	IMP_SET_TPOS_PREDICTED(pstCurPos->s32Used);
	pstCurPos->u32Time = pstTracker->pstResult->u32FrmTimeCur;
	pstCurPos->u32AreaActual = collision_pos->u32AreaActual;
	pstCurPos->u32AreaPixel = collision_pos->u32AreaPixel;
	pstCurPos->s32Direction = pstPrePos->s32Direction;
	pstCurPos->s32Velocity = pstPrePos->s32Velocity;
	pstCurPos->stPt = pstTargetData->stCurCentroid;
	offsetX = collision_pos->stRg.s16X1 - collision_pos->stPt.s16X;
	offsetY = collision_pos->stRg.s16Y1 - collision_pos->stPt.s16Y;
	width = collision_pos->stRg.s16X2 - collision_pos->stRg.s16X1 + 1;
	height = collision_pos->stRg.s16Y2 - collision_pos->stRg.s16Y1 + 1;
	IMP_ASSERT( width>0 );
	IMP_ASSERT( height>0 );
	pstCurPos->stRg.s16X1 = pstCurPos->stPt.s16X + offsetX;
	pstCurPos->stRg.s16Y1 = pstCurPos->stPt.s16Y + offsetY;
	pstCurPos->stRg.s16X2 = pstCurPos->stRg.s16X1 + width - 1;
	pstCurPos->stRg.s16Y2 = pstCurPos->stRg.s16Y1 + height - 1;
//	IMP_ASSERT( pstCurPos->stRg.s16X1>=0 );
//	IMP_ASSERT( pstCurPos->stRg.s16Y1>=0 );
//	IMP_ASSERT( pstCurPos->stRg.s16X2>=0 );
//	IMP_ASSERT( pstCurPos->stRg.s16Y2>=0 );
	pstTargetData->stMinRect = pstCurPos->stRg;
	pstCurPos->u32DrgType = pstPrePos->u32DrgType;

	pstTarget->stTargetImage.pstDRegions = NULL;
	pstTarget->stTargetImage.s32DrgIndex = 0;
	IMP_SET_TGT_PREDICTED( pstTarget->stTargetInfo.u32Status );					//OK
	IMP_RESET_TGT_COLLISION( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
}

static IMP_VOID ipUpdateTargetFromPredictM( IpTrackedTarget *pstTarget, IMP_S32 s32TargetIndex,
										   PEA_DETECTED_REGIONSET_S *pstRegionSet,
									IMP_U32 u32Time )
{
	IMP_S32 offsetX, offsetY, width, height;
	IpTargetPosition *pstCurPos, *pstPrePos;
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;

//	if (pstTarget->u32TargetId-0x90000000 == 70 && (u32Time) >= 762)
//	{ int x;
//		x++;
//	}


	pstTargetData->stCurCentroid.s16X = pstTargetData->stPredictedCentroid.s16X;
	pstTargetData->stCurCentroid.s16Y = pstTargetData->stPredictedCentroid.s16Y;


	pstPrePos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -1 );
	pstCurPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
	IMP_SET_TPOS_PREDICTED(pstCurPos->s32Used);
	pstCurPos->u32Time = u32Time;
	pstCurPos->u32AreaActual = pstPrePos->u32AreaActual;
	pstCurPos->u32AreaPixel = pstPrePos->u32AreaPixel;
	pstCurPos->s32Direction = pstPrePos->s32Direction;
	pstCurPos->s32Velocity = pstPrePos->s32Velocity;
	pstCurPos->stPt = pstTargetData->stCurCentroid;
	offsetX = pstPrePos->stRg.s16X1 - pstPrePos->stPt.s16X;
	offsetY = pstPrePos->stRg.s16Y1 - pstPrePos->stPt.s16Y;
	width = pstPrePos->stRg.s16X2 - pstPrePos->stRg.s16X1 + 1;
	height = pstPrePos->stRg.s16Y2 - pstPrePos->stRg.s16Y1 + 1;
	pstCurPos->stRg.s16X1 = pstCurPos->stPt.s16X + offsetX;
	pstCurPos->stRg.s16Y1 = pstCurPos->stPt.s16Y + offsetY;
	pstCurPos->stRg.s16X2 = pstCurPos->stRg.s16X1 + width - 1;
	pstCurPos->stRg.s16Y2 = pstCurPos->stRg.s16Y1 + height - 1;
	pstTargetData->stMinRect = pstCurPos->stRg;
	pstCurPos->u32DrgType = pstPrePos->u32DrgType;

	pstTarget->stTargetImage.pstDRegions = NULL;
	pstTarget->stTargetImage.s32DrgIndex = 0;
	IMP_SET_TGT_PREDICTED( pstTarget->stTargetInfo.u32Status );					//OK
	IMP_RESET_TGT_COLLISION( pstTarget->stTargetInfo.u32Status );
	IMP_RESET_TGT_MEANSHIFT( pstTarget->stTargetInfo.u32Status );
}

static IMP_VOID check_position_valid( IpTargetTrackerMotion *pstTracker, IpTargetPosition *pos )
{
	IMP_RECT_S stRect;
	stRect.s16X1 = stRect.s16Y1 = 0;
	stRect.s16X2 = pstTracker->pstResult->s32Width-1;
	stRect.s16Y2 = pstTracker->pstResult->s32Height-1;


	if( pos->stRg.s16X1<stRect.s16X1 ) pos->stRg.s16X1 = stRect.s16X1;
	if( pos->stRg.s16X2<stRect.s16X1 ) pos->stRg.s16X2 = stRect.s16X1;
	if( pos->stRg.s16X1>stRect.s16X2 ) pos->stRg.s16X1 = stRect.s16X2;
	if( pos->stRg.s16X2>stRect.s16X2 ) pos->stRg.s16X2 = stRect.s16X2;


	if( pos->stRg.s16Y1<stRect.s16Y1 ) pos->stRg.s16Y1 = stRect.s16Y1;
	if( pos->stRg.s16Y2<stRect.s16Y1 ) pos->stRg.s16Y2 = stRect.s16Y1;
	if( pos->stRg.s16Y1>stRect.s16Y2 ) pos->stRg.s16Y1 = stRect.s16Y2;
	if( pos->stRg.s16Y2>stRect.s16Y2 ) pos->stRg.s16Y2 = stRect.s16Y2;


	if( pos->stPt.s16X<pos->stRg.s16X1 ) pos->stPt.s16X = pos->stRg.s16X1;
	if( pos->stPt.s16X>pos->stRg.s16X2 ) pos->stPt.s16X = pos->stRg.s16X2;
	if( pos->stPt.s16Y<pos->stRg.s16Y1 ) pos->stPt.s16Y = pos->stRg.s16Y1;
	if( pos->stPt.s16Y>pos->stRg.s16Y2 ) pos->stPt.s16Y = pos->stRg.s16Y2;

	IMP_ASSERT( pos->stRg.s16X1 >=0 && pos->stRg.s16X1 <= stRect.s16X2 );
	IMP_ASSERT( pos->stRg.s16X2 >=0 && pos->stRg.s16X2 <= stRect.s16X2 );
	IMP_ASSERT( pos->stRg.s16Y1 >=0 && pos->stRg.s16Y1 <= stRect.s16Y2 );
	IMP_ASSERT( pos->stRg.s16Y2 >=0 && pos->stRg.s16Y2 <= stRect.s16Y2 );


	ipRectPointImgPositionCorrect(&pos->stRg, &pos->stPt, pstTracker->pstResult->s32Width, pstTracker->pstResult->s32Height, DO_CORRECT);

}


static IMP_VOID correct_collision_trajectory(IpTrackedTarget *pstTarget,PEA_DETECTED_REGIONSET_S *pstRegionSet)
{

	IpTargetDataTrackerM* pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	PEA_DETECTED_REGION_S* pstDRegion = &pstRegionSet->astDrg[pstTargetData->s32MatchedWith];
	IpDrgDataTrackerM* pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;

	IpTargetPosition *pos;
	IMP_S32 length;
	IMP_S32 i;
	IMP_POINT_S ptstart,ptend;
	IMP_FLOAT dx,dy;
	length = pstTargetData->cs.s32CollisionLength;

// 	if (length<256)
	{
//		ptstart.s16X=pstTargetData->cs.sampleTardat.stCentroid.s16X;
//		ptstart.s16Y=pstTargetData->cs.sampleTardat.stCentroid.s16Y;
//		ptend.s16X=pstDrgData->stCurCentroid.s16X;
//		ptend.s16Y=pstDrgData->stCurCentroid.s16Y;
//
//		dx = (IMP_FLOAT)(ptend.s16X-ptstart.s16X)/(length+1);
//		dy = (IMP_FLOAT)(ptend.s16Y-ptstart.s16Y)/(length+1);
//
//		for (i=0;i<length;i++)
//		{
//			pos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, i-length );
//
//			pos->stPt.s16X = ptstart.s16X+(IMP_S32)((i+1)*dx+0.5);//四舍五入
//			pos->stPt.s16Y = ptstart.s16Y+(IMP_S32)((i+1)*dy+0.5);
//		}

		ptstart.s16X=pstTargetData->cs.sampleTardat.stCentroid.s16X;
		ptstart.s16Y=pstTargetData->cs.sampleTardat.stCentroid.s16Y;
		ptend.s16X=pstDrgData->stCurCentroid.s16X;
		ptend.s16Y=pstDrgData->stCurCentroid.s16Y;

		dx = (IMP_FLOAT)(ptend.s16X-ptstart.s16X)/(length+1);
		dy = (IMP_FLOAT)(ptend.s16Y-ptstart.s16Y)/(length+1);

		for (i=1;i<=length && i<=255;i++)
		{
			pos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );
			pos->stPt.s16X = ptend.s16X-(IMP_S32)((i+1)*dx+0.5);
			pos->stPt.s16Y = ptend.s16Y-(IMP_S32)((i+1)*dy+0.5);
		}
	}
}


static IMP_VOID correct_collision_trajectory2(IpTrackedTarget *pstTarget,PEA_DETECTED_REGIONSET_S *pstRegionSet)
{

	IpTargetDataTrackerM* pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
//	PEA_DETECTED_REGION_S* pstDRegion = &pstRegionSet->astDrg[pstTargetData->s32MatchedWith];
//	IpDrgDataTrackerM* pstDrgData = (IpDrgDataTrackerM*)pstDRegion->stPrivateData.pDataTracker;

	IpTargetPosition tmp_pos;
	IpTargetPosition *pos,*pstPrePos = &tmp_pos;
	IMP_S32 length;
	IMP_S32 width,height;
	IMP_S32 offsetX,offsetY;
	IMP_S32 i;
	IMP_RECT_S boundary;

	length = pstTargetData->cs.s32CollisionLength;



	{
		correct_collision_trajectory(pstTarget,pstRegionSet);
	}

	{
		for (i=1;i<=length && i<=255;i++)
		{

			pos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, -i );

			pstPrePos->stRg = pstTargetData->cs.sampleTardat.stRect;
			pstPrePos->stPt = pstTargetData->cs.sampleTardat.stCentroid;

 			width	= pstPrePos->stRg.s16X2 - pstPrePos->stRg.s16X1 + 1;
			height	= pstPrePos->stRg.s16Y2 - pstPrePos->stRg.s16Y1 + 1;
			boundary.s16X1=pos->stRg.s16X1+(width>>1);
			boundary.s16Y1=pos->stRg.s16Y1+(height>>1);
			boundary.s16X2=pos->stRg.s16X2-(width>>1);
			boundary.s16Y2=pos->stRg.s16Y2-(height>>1);

			if (boundary.s16X1>boundary.s16X2)
			{
				boundary.s16X1=(pos->stRg.s16X1+pos->stRg.s16X2)>>1;
				boundary.s16X2=(pos->stRg.s16X1+pos->stRg.s16X2)>>1;
			}

			if (boundary.s16Y1>boundary.s16Y2)
			{
				boundary.s16Y1=(pos->stRg.s16Y1+pos->stRg.s16Y2)>>1;
				boundary.s16Y2=(pos->stRg.s16Y1+pos->stRg.s16Y2)>>1;
			}


			offsetX = pos->stPt.s16X < boundary.s16X1 ? (boundary.s16X1-pos->stPt.s16X):0;
			offsetX += pos->stPt.s16X > boundary.s16X2 ? (boundary.s16X2-pos->stPt.s16X):0;
			offsetY = pos->stPt.s16Y < boundary.s16Y1 ? (boundary.s16Y1-pos->stPt.s16Y):0;
			offsetY += pos->stPt.s16Y > boundary.s16Y2 ? (boundary.s16Y2-pos->stPt.s16Y):0;


			pos->stPt.s16X = pos->stPt.s16X + offsetX;
			pos->stPt.s16Y = pos->stPt.s16Y + offsetY;


			offsetX = pstPrePos->stRg.s16X1 - pstPrePos->stPt.s16X;
			offsetY = pstPrePos->stRg.s16Y1 - pstPrePos->stPt.s16Y;

			pos->stRg.s16X1 = pos->stPt.s16X + offsetX;
			pos->stRg.s16Y1 = pos->stPt.s16Y + offsetY;
			pos->stRg.s16X2 = pos->stRg.s16X1 + width - 1;
			pos->stRg.s16Y2 = pos->stRg.s16Y1 + height - 1;

		}
	}
}



