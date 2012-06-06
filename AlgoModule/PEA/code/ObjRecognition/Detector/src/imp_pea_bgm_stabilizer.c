
#include "imp_pea_detector.h"




#ifndef _TIDSP
#define DEBUG_MATCH
#endif
#ifdef DEBUG_MATCH
PEA_BGMODEL_STABILIZER_S *g_ies;
IMP_S32 ies_region_match_num;
#endif

#define IMP_PEA_TH_MATCH_NUM_H	3
#define IMP_PEA_TH_MATCH_NUM_L	2

static IMP_S32 ipCalculateMvectorCoarse( PEA_BGMODEL_STABILIZER_S *pModule );
static IMP_S32 ipCalculateMvectorRefine( PEA_BGMODEL_STABILIZER_S *pModule );
static IMP_S32 ipEvaluateMatchData( PEA_BGMODEL_STABILIZER_S *pModule );
static IMP_S32 ipCalcbackgroundDifference( GRAY_IMAGE_S *pstImage, IMP_RECT_S *pstRect );
static IMP_VOID ipCalcMatchData( GRAY_IMAGE_S *pstImageRef, GRAY_IMAGE_S *pstImageCur,
						   PEA_STABILIZE_MATCH_DATA_S *pstData, PEA_STABILIZE_PARA_S *pstPara,
						   IMP_S32 s32Refine );

static IMP_VOID ipCalculateMvector( PEA_BGMODEL_STABILIZER_S *pModule )
{

	pModule->stMvPre = pModule->stMvCurrent;

	if( !ipCalculateMvectorCoarse( pModule ) )
	{
		ipCalculateMvectorRefine( pModule );
	}

	ipEvaluateMatchData( pModule );
}

static IMP_VOID ipDetermineImageShaking( PEA_BGMODEL_STABILIZER_S *pModule )
{
	PEA_BG_MODEL_DATA_S *pstBgModelData = pModule->pstBgModelData;
	IMP_S32 i;
	IMP_S32 s32Cond1, s32Cond2;
	IMP_S32 s32EdgeNumBg, s32EdgeNumFg;
	IMP_S32 s32EdgeLike, s32ThEdgeNum;
	IMP_S32 s32Height, s32Width;
	IMP_S32 s32IsShaking, s32ValidLen;
	PEA_STABILIZE_MATCH_DATA_S *pstData = &pModule->astMatchData[0];
	PEA_STABILIZE_PARA_S *pstPara = &pModule->stStabPara;
	GRAY_IMAGE_S *pstImgBgEdge, *pstImgFgEdge;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pModule->pstPeaResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgRefEdge = pstBgModelData->pstDetectorData->pstImgBgEdgeG;
	GRAY_IMAGE_S *pstImgFg = pstRgs->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgDiff = pstRgs->pstImgFrmDiff;

	pstImgBgEdge = pstImgRefEdge;
	if( pstPara->s32ShakingUseFgEdge )
		pstImgFgEdge = pstImgFg;
	else
		pstImgFgEdge = pstImgDiff;

	s32IsShaking = IMP_FALSE;

	if( pModule->s32SuccessFlag )
	{
		s32Cond1 = !( pModule->stMvCurrent.s16X == 0 && pModule->stMvCurrent.s16Y == 0 );

		if( !s32Cond1 )
		{
			s32Cond2 = 0;
			for( i=0; i<IMP_PEA_MATCH_REGION_NUM; i++ )
			{
				if( !pstData[i].s32SuccessFlag)
					continue;

				s32EdgeLike = 100;
				if( !pstData[i].s32Moved )
				{
					s32Width = pstData[i].stRegion.s16X2 - pstData[i].stRegion.s16X1 + 1;
					s32Height = pstData[i].stRegion.s16Y2 - pstData[i].stRegion.s16Y1 + 1;
					s32ThEdgeNum = s32Height + s32Width;
					s32EdgeNumFg = ipCalcRegionFgNum( pstImgFgEdge, &pstData[i].stRegion );
					s32EdgeNumBg = pstData[i].s32BgEdge;

					if( s32EdgeNumBg < s32ThEdgeNum || s32EdgeNumFg < s32ThEdgeNum )
					{
						s32EdgeLike = 0;
					}
					else
					{
						s32EdgeLike = ipCalcRegionEdgeLikehood( pstImgFgEdge,
							pstImgBgEdge, NULL, &pstData[i].stRegion, 0, 1, 0, 100,
							pstPara->s32ShakingEdgeLikehoodTh, pstPara->s32ShakingEdgeLikehoodTh );
					}
				}

				if( s32EdgeLike )
					s32Cond2++;
			}
			if( pModule->s32MatchNum >= IMP_PEA_TH_MATCH_NUM_H )
				s32Cond2 = ( (s32Cond2+1) >= pModule->s32MatchNum );
			else
				s32Cond2 = ( s32Cond2 >= pModule->s32MatchNum );
		}

		s32IsShaking = ( s32Cond1 || s32Cond2 );
	}
	pModule->s32ShakingCurFlag = s32IsShaking;

	IMP_GA_CArrayPushElem( &pModule->stShakingHistory, s32IsShaking );
	s32ValidLen = 0;
	for( i = 0; i < pstPara->s32ShakingHistoryLenTh; i++ )
	{
		if( IMP_GA_CArrayGetElem( &pModule->stShakingHistory, -i ) )
			s32ValidLen++;
	}
	pModule->s32ShakingFlag = s32ValidLen >= pstPara->s32ShakingValidLenTh;
	pstBgModelData->s32IsShaking = pModule->s32ShakingFlag;
}

static IMP_VOID ipTranslateImagei( GRAY_IMAGE_S *pstImage, IMP_POINT_S *pstMv )
{
	IMP_S32 s32ImageWidth, s32ImageHeight, s32MvX, s32MvY;
	IMP_U8 *pu8Data, *pu8Data2;
	IMP_S32 xx, yy, lb_x, lb_y, ru_x, ru_y;
	IMP_S32 s32Offset, i, s32CpyLen, n, s32CpyLen2;

	s32ImageWidth = pstImage->s32W;
	s32ImageHeight = pstImage->s32H;
	s32MvX = pstMv->s16X; s32MvY = pstMv->s16Y;
	pu8Data2 = pstImage->pu8Data;
	s32Offset = s32ImageWidth*s32MvY+s32MvX;

	if( s32Offset > 0 )
	{
		IMP_U8 * RESTRICT pu8DataS = pu8Data2 + s32Offset;
		IMP_U8 * RESTRICT pu8DataD = pu8Data2;
		s32CpyLen = s32ImageWidth * s32ImageHeight - s32Offset;
		s32CpyLen = (s32CpyLen + 3) & (~3);
		s32CpyLen2 = (s32CpyLen >> 2);

		for( n = 0; n < 4; n++ )
		{
#ifdef _TIDSP
#pragma MUST_ITERATE(256,,1);
#endif
			for( i = 0; i < s32CpyLen2; i++ )
				*pu8DataD++ = *pu8DataS++;
		}
		s32CpyLen2 = s32CpyLen - s32CpyLen2 * 4;
	}
	else
	{
		IMP_U8 * RESTRICT pu8DataS = pu8Data2 + s32ImageWidth * s32ImageHeight - 1 - (-s32Offset);
		IMP_U8 * RESTRICT pu8DataD = pu8Data2 + s32ImageWidth * s32ImageHeight - 1;
		s32CpyLen = s32ImageWidth * s32ImageHeight - (-s32Offset);
		s32CpyLen = (s32CpyLen + 3) & (~3);
		s32CpyLen2 = (s32CpyLen >> 2);

		for( n = 0; n < 4; n++ )
		{
#ifdef _TIDSP
#pragma MUST_ITERATE(256,,1);
#endif
			for( i = 0; i < s32CpyLen2; i++ )
				*pu8DataD-- = *pu8DataS--;
		}
		s32CpyLen2 = s32CpyLen - s32CpyLen2 * 4;
	}

	if( s32MvY )
	{
		lb_x = 0; ru_x = s32ImageWidth - 1;
		if( s32MvY > 0 )
		{
			lb_y = s32ImageHeight - s32MvY;
			ru_y = s32ImageHeight - 1;
		}
		else
		{
			lb_y = 0;
			ru_y = -s32MvY;
		}
		for( yy = lb_y; yy <=ru_y; yy++ )
		{
			pu8Data = pu8Data2 + s32ImageWidth * yy;
			for( xx = lb_x; xx <= ru_x; xx++ )
				pu8Data[xx] = 0;
		}
	}
	if( s32MvX )
	{
		lb_y = 0; ru_y = s32ImageHeight - 1;
		if( s32MvX > 0 )
		{
			lb_x = s32ImageWidth - s32MvX;
			ru_x = s32ImageWidth - 1;
		}
		else
		{
			lb_x = 0;
			ru_x = -s32MvX;
		}
		for( yy = lb_y; yy <= ru_y; yy++ )
		{
			pu8Data = pu8Data2 + s32ImageWidth * yy;
			for( xx = lb_x; xx <= ru_x; xx++ )
				pu8Data[xx] = 0;
		}
	}
}
static IMP_VOID ipTranslateImage( PEA_BGMODEL_STABILIZER_S *pModule )
{
	IMP_POINT_S *pstMv = &pModule->stMvCurrent;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pModule->pstPeaResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgCur = pstRgs->pstImgInGray;

	if( pstMv->s16X == 0 && pstMv->s16Y == 0 )
		return;

	ipTranslateImagei( pstImgCur, pstMv );
}
static IMP_VOID ipDetectConstNon0Mv( PEA_BGMODEL_STABILIZER_S *pModule )
{
	PEA_BG_MODEL_DATA_S *pstBgModelData = pModule->pstBgModelData;
	IMP_U32 u32FrmTimeCur = pModule->u32FrmTimeCur;

	if( !pModule->u32CntMvStartTime
		&& ( pModule->stMvCurrent.s16X || pModule->stMvCurrent.s16Y )
		&& ( pModule->stMvCurrent.s16X==pModule->stMvPre.s16X && pModule->stMvCurrent.s16Y==pModule->stMvPre.s16Y )
		)
		pModule->u32CntMvStartTime = u32FrmTimeCur;

	if( pModule->u32CntMvStartTime
		&& !( pModule->stMvCurrent.s16X==pModule->stMvPre.s16X && pModule->stMvCurrent.s16Y==pModule->stMvPre.s16Y )
		)
		pModule->u32CntMvStartTime = 0;

	pstBgModelData->s32ConstMvDetected = IMP_FALSE;
	if( pModule->u32CntMvStartTime )
	{
		if( ( u32FrmTimeCur - pModule->u32CntMvStartTime ) >= (IMP_U32)pModule->stStabPara.s32ConstMvTimeTh )
		{
			pstBgModelData->s32ConstMvDetected = IMP_TRUE;
			pstBgModelData->stEstimateMv = pModule->stMvCurrent;
			pModule->u32CntMvStartTime = 0;
		}
	}
}
static IMP_VOID ipCalcbackgroundData( GRAY_IMAGE_S *pstImageGray, GRAY_IMAGE_S *pstImageEdge,
								 PEA_STABILIZE_MATCH_DATA_S *pstData, PEA_STABILIZE_PARA_S *pstPara )
{
	IMP_S32 s32DiffBg, s32EdgeBg;
	IMP_S32 s32ThDiffMatchL, s32ThDiffMatchH;
	IMP_S32 s32ThDiffRatioMatchH = pstPara->s32DiffRatioMatchHighTh;
	IMP_S32 s32ThDiffRatioMatchL = pstPara->s32DiffRatioMatchLowTh;
	IMP_S32 s32ThDiffMaxMatchH = pstPara->s32DiffMaxMatchHighTh;
	IMP_S32 s32ThDiffMaxMatchL = pstPara->s32DiffMinMatchLowTh;

	s32DiffBg = ipCalcbackgroundDifference( pstImageGray, &pstData->stRegion );
	s32EdgeBg = ipCalcRegionFgNum( pstImageEdge, &pstData->stRegion );

	s32ThDiffMatchH = s32DiffBg * s32ThDiffRatioMatchH;
	if( s32ThDiffMatchH < s32ThDiffMaxMatchH * 100 )
		s32ThDiffMatchH = s32ThDiffMaxMatchH * 100;

	s32ThDiffMatchL = s32DiffBg * s32ThDiffRatioMatchL;
	if( s32ThDiffMatchL < s32ThDiffMaxMatchL * 100 )
		s32ThDiffMatchL = s32ThDiffMaxMatchL * 100;

	pstData->s32BgDiff = s32DiffBg;
	pstData->s32BgEdge = s32EdgeBg;
	pstData->s32DiffMatchHighTh = s32ThDiffMatchH;
	pstData->s32DiffMatchLowTh = s32ThDiffMatchL;
}

static IMP_S32 ipCalculateMvectorCoarse( PEA_BGMODEL_STABILIZER_S *pModule )
{
	IMP_S32 i;
	IMP_POINT_S *pstMv;
	IMP_S32 s32Diff, s32ThDiff, s32ZeroMvNum;
	PEA_BG_MODEL_DATA_S *pstBgModelData = pModule->pstBgModelData;
	PEA_STABILIZE_PARA_S *pstPara = &pModule->stStabPara;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pModule->pstPeaResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgRef = pstRgs->pstImgBgGray;				//参考背景
	GRAY_IMAGE_S *pstImgRefEdge = pstBgModelData->pstDetectorData->pstImgBgEdgeG;	//参考背景边缘
	GRAY_IMAGE_S *pstImgCur = pstRgs->pstImgInGray;					//当前图像
	PEA_STATUS_S *pstStatus = &pModule->pstPeaResult->stSysStatus;

	for( i = 0; i < IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		if( !pModule->s32MatchDataUpdated || IMP_STAT_BG_IS_UPDATED(pstStatus) )
		{
			ipCalcbackgroundData( pstImgRef, pstImgRefEdge, &pModule->astMatchData[i], pstPara );
		}
		ipCalcMatchData( pstImgRef, pstImgCur, &pModule->astMatchData[i], &pModule->stStabPara, 0 );
	}
	pModule->s32MatchDataUpdated = IMP_TRUE;

	s32ZeroMvNum = 0;
	for( i = 0; i < IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		pstMv = &pModule->astMatchData[i].stMv;
		s32Diff = pModule->astMatchData[i].s32Diff;
		s32ThDiff = pModule->astMatchData[i].s32DiffMatchLowTh;
		if( pstMv->s16X == 0 && pstMv->s16Y == 0 && s32Diff * 100 <= s32ThDiff )
			s32ZeroMvNum++;
	}

	return ( s32ZeroMvNum >= IMP_PEA_TH_MATCH_NUM_H );
}

static IMP_S32 ipCalculateMvectorRefine( PEA_BGMODEL_STABILIZER_S *pModule )
{
	IMP_S32 i;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pModule->pstPeaResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgRef = pstRgs->pstImgBgGray;
	GRAY_IMAGE_S *pstImgCur = pstRgs->pstImgInGray;

	for( i = 0; i < IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		ipCalcMatchData( pstImgRef, pstImgCur, &pModule->astMatchData[i], &pModule->stStabPara, 1 );
	}

	return IMP_TRUE;
}

static IMP_VOID ipRegisterImageRegion( IMP_U8 *pu8DataRef, IMP_U8 *pu8DataCur,
						   IMP_S32 s32Width, IMP_S32 s32Height, IMP_S32 s32ImageWidth,
						   IMP_S32 s32Scope, IMP_S32 s32Step,
						   IMP_POINT_S *pstMv, IMP_S32 *pdiff );
static IMP_VOID ipCalcMatchData( GRAY_IMAGE_S *pstImageRef, GRAY_IMAGE_S *pstImageCur,
						   PEA_STABILIZE_MATCH_DATA_S *pstData, PEA_STABILIZE_PARA_S *pstPara,
						   IMP_S32 s32Refine )
{
	IMP_U8 *pu8DataRef, *pu8DataCur;
	IMP_S32 s32Width, s32Height, s32ImageWidth;
	IMP_POINT_S stMvC, stMvR;
	IMP_S32 s32Diff;
	IMP_S32 s32MvX, s32MvY;
	IMP_RECT_S *pstRg = &pstData->stRegion;

	stMvC.s16X = stMvC.s16Y = 0;
	stMvR.s16X = stMvR.s16Y = 0;
	s32Diff = 0;

	s32ImageWidth = pstImageRef->s32W;
	s32Width = pstRg->s16X2 - pstRg->s16X1 + 1;
	s32Height = pstRg->s16Y2 - pstRg->s16Y1 + 1;
	pu8DataRef = pstImageRef->pu8Data + (s32ImageWidth * (pstRg->s16Y1) + pstRg->s16X1);
	pu8DataCur = pstImageCur->pu8Data + (s32ImageWidth * (pstRg->s16Y1) + pstRg->s16X1);

	if( s32Refine )
	{
		ipRegisterImageRegion( pu8DataRef, pu8DataCur, s32Width, s32Height, s32ImageWidth,
							 pstPara->s32SearchScope, 2, &stMvC, &s32Diff );
	}

 	if( !( s32Refine && stMvC.s16X == 0 && stMvC.s16Y == 0 ) )
	{
		ipRegisterImageRegion( pu8DataRef, pu8DataCur+(s32ImageWidth*stMvC.s16Y+stMvC.s16X), s32Width, s32Height, s32ImageWidth,
							 1, 1, &stMvR, &s32Diff );

		s32MvX = stMvC.s16X + stMvR.s16X;
		s32MvY = stMvC.s16Y + stMvR.s16Y;

		pstData->s32Diff = s32Diff;
		pstData->stMv.s16X = s32MvX;
		pstData->stMv.s16Y = s32MvY;
	}
}

static IMP_S32 ipMatchImageRegion( IMP_U8 *pu8Data1, IMP_U8 *pu8Data2,
								IMP_S32 s32Width, IMP_S32 s32Height, IMP_S32 s32ImageWidth );
static IMP_VOID ipRegisterImageRegion( IMP_U8 *pu8DataRef, IMP_U8 *pu8DataCur,
								IMP_S32 s32Width, IMP_S32 s32Height, IMP_S32 s32ImageWidth,
								IMP_S32 s32Scope, IMP_S32 s32Step,
								IMP_POINT_S *pstMv, IMP_S32 *pdiff )
{
	IMP_S32 yy, xx;
	IMP_S32 s32MvX, s32MvY;
	IMP_S32 s32Diff, s32DiffMin;
#ifdef DEBUG_MATCH
	FILE *fil = fopen( "c:\\match.txt", "w" );
#endif

	s32DiffMin = 0x7fffffff;
	s32MvX = s32MvY = 0;
	for( yy = -s32Scope; yy <= s32Scope; yy += s32Step )
	{
		for( xx = -s32Scope; xx <=s32Scope; xx += s32Step )
		{
			s32Diff = ipMatchImageRegion( pu8DataRef, pu8DataCur + (s32ImageWidth * yy + xx), s32Width, s32Height, s32ImageWidth );
			if( s32Diff < s32DiffMin )
			{
				s32DiffMin = s32Diff;
				s32MvX = xx; s32MvY = yy;
			}
#ifdef DEBUG_MATCH
			fprintf( fil, "%d\t\t", s32Diff );
#endif
		}
#ifdef DEBUG_MATCH
		fprintf( fil, "\n" );
#endif
	}

	*pdiff = s32DiffMin / (s32Width * s32Height);
	pstMv->s16X = s32MvX;
	pstMv->s16Y = s32MvY;

#ifdef DEBUG_MATCH
	fclose( fil );
#endif
}

static IMP_S32 ipEvaluateMatchData( PEA_BGMODEL_STABILIZER_S *pModule )
{
	IMP_S32 i, j, k;
	IMP_S32 s32MatchNum, s32MatchNum2;
	IMP_S32 s32MvX, s32MvY;
	IMP_POINT_S *mv1, *mv2;
	IMP_S32 s32Cond1, s32Cond2, s32Cond3;
	IMP_S32 s32Matched, s32Moved;
	PEA_STABILIZE_MATCH_DATA_S *pstData = &pModule->astMatchData[0];
	PEA_STABILIZE_PARA_S *pstPara = &pModule->stStabPara;
	IMP_S32 s32ThMatchDist = pstPara->s32MatchDistTh;
	IMP_S32 s32EvalSuc = IMP_FALSE;

	pModule->s32SuccessFlag = IMP_FALSE;
	pModule->s32ShakingFlag = IMP_FALSE;
	pModule->s32MatchNum = 0;
	pModule->stMvCurrent.s16X = 0;
	pModule->stMvCurrent.s16Y = 0;

	s32MatchNum = 0;
	for( i = 0; i < IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		s32Cond1 = ( abs(pstData[i].stMv.s16X) <= pstPara->s32SearchScope
				&& abs(pstData[i].stMv.s16Y) <= pstPara->s32SearchScope );
		s32Cond2 = ( pstData[i].s32Diff * 100 <= pstData[i].s32DiffMatchHighTh );
		s32Cond3 = ( pstData[i].stMv.s16X || pstData[i].stMv.s16Y );

		s32Matched = ( s32Cond1 && s32Cond2 );
		s32Moved = s32Cond3;

		pstData[i].s32Matched = s32Matched;
		pstData[i].s32Moved = s32Moved;
		pstData[i].s32SuccessFlag= IMP_FALSE;

 		if( pstData[i].s32Matched )
			s32MatchNum++;
	}
	if( s32MatchNum < IMP_PEA_TH_MATCH_NUM_L )
		return IMP_FALSE;

	s32MvX = s32MvY = 0;
	s32MatchNum2 = 0;

	if( s32MatchNum >= IMP_PEA_TH_MATCH_NUM_H )
	for( i = 0; i < IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		if( s32EvalSuc )
			break;

		for( k = 0; k < IMP_PEA_MATCH_REGION_NUM; k++ )
			pstData[k].s32SuccessFlag= IMP_FALSE;

		if( !pstData[i].s32Matched )
			continue;

		mv1 = &pstData[i].stMv;

		s32MvX = pstData[i].stMv.s16X;
		s32MvY = pstData[i].stMv.s16Y;
		s32MatchNum2 = 1;
		pstData[i].s32SuccessFlag= IMP_TRUE;

		for( j = 0; j < IMP_PEA_MATCH_REGION_NUM; j++ )
		{
			if( i==j || !pstData[j].s32Matched )
				continue;

			mv2 = &pstData[j].stMv;
			if( abs(mv1->s16X - mv2->s16X) <= s32ThMatchDist && abs(mv1->s16Y - mv2->s16Y) <= s32ThMatchDist )
			{
				s32MvX += pstData[j].stMv.s16X;
				s32MvY += pstData[j].stMv.s16Y;
				s32MatchNum2++;
				pstData[j].s32SuccessFlag = IMP_TRUE;

				if( s32MatchNum2 >= IMP_PEA_TH_MATCH_NUM_H )
				{
					s32EvalSuc = IMP_TRUE;
				}
			}
		}
	}

	if( !s32EvalSuc && pstPara->s32MatchNumTh == IMP_PEA_TH_MATCH_NUM_L )
	for( i = 0; i < IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		if( s32EvalSuc )
			break;

		for( k = 0; k < IMP_PEA_MATCH_REGION_NUM; k++ )
			pstData[k].s32SuccessFlag = IMP_FALSE;

		if( !pstData[i].s32Matched )
			continue;

		mv1 = &pstData[i].stMv;

		s32MvX = pstData[i].stMv.s16X;
		s32MvY = pstData[i].stMv.s16Y;
		s32MatchNum2 = 1;
		pstData[i].s32SuccessFlag = IMP_TRUE;

		for( j = 0; j < IMP_PEA_MATCH_REGION_NUM; j++ )
		{
			if( i == j || !pstData[j].s32Matched )
				continue;

			mv2 = &pstData[j].stMv;
			if( (mv1->s16X == mv2->s16X) && (mv1->s16Y == mv2->s16Y) )
			{
				s32MvX += pstData[j].stMv.s16X;
				s32MvY += pstData[j].stMv.s16Y;
				s32MatchNum2++;
				pstData[j].s32SuccessFlag = IMP_TRUE;

				if( s32MatchNum2 >= IMP_PEA_TH_MATCH_NUM_L )
				{
					s32EvalSuc = IMP_TRUE;
				}
			}
		}
	}

	if( s32EvalSuc )
	{
		if( s32MvX >= 0 )
			s32MvX = ( s32MvX + (s32MatchNum2 >> 1) ) / s32MatchNum2;
		else
			s32MvX = ( s32MvX - (s32MatchNum2 >> 1) ) / s32MatchNum2;
		if( s32MvY >= 0 )
			s32MvY = ( s32MvY + (s32MatchNum2 >> 1) ) / s32MatchNum2;
		else
			s32MvY = ( s32MvY - (s32MatchNum2 >> 1) ) / s32MatchNum2;
		pModule->stMvCurrent.s16X = s32MvX;
		pModule->stMvCurrent.s16Y = s32MvY;
		pModule->s32MatchNum = s32MatchNum2;
		pModule->s32SuccessFlag = IMP_TRUE;
	}

	return pModule->s32SuccessFlag;
}



static IMP_S32 ipCalcbackgroundDifference( GRAY_IMAGE_S *pstImage, IMP_RECT_S *pstRect )
{
	IMP_U8 *pu8Data1, *pu8Data2;
	IMP_S32 xs, ys, s32Width, s32Height;
	IMP_S32 s32Diff;

	xs = pstRect->s16X1;
	ys = pstRect->s16Y1;
	s32Width = pstRect->s16X2 - pstRect->s16X1 + 1;
	s32Height = pstRect->s16Y2 - pstRect->s16Y1 + 1;
	pu8Data1 = pstImage->pu8Data + pstImage->s32W * ys + xs;
	pu8Data2 = pu8Data1 + pstImage->s32W + 1;
	s32Diff = ipMatchImageRegion( pu8Data1, pu8Data2, s32Width, s32Height, pstImage->s32W );
	s32Diff = s32Diff / (s32Width * s32Height);

	return s32Diff;
}




static IMP_S32 ipMatchImageRegion( IMP_U8 *pu8Data1, IMP_U8 *pu8Data2, IMP_S32 s32Width, IMP_S32 s32Height, IMP_S32 s32ImageWidth )
{
	IMP_S32 s32Diff, s32Sum = 0;
	IMP_S32 yy, xx;

	for( yy = 0; yy < s32Height; yy++ )
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(32,64,4);
#endif
		for( xx = 0; xx < s32Width; xx++ )
		{
			s32Diff = abs( pu8Data1[xx] - pu8Data2[xx] );
			s32Sum += s32Diff;
		}
		pu8Data1 += s32ImageWidth;
		pu8Data2 += s32ImageWidth;
	}

#ifdef DEBUG_MATCH
	ies_region_match_num++;
#endif
	return s32Sum;
}



static IMP_VOID ipClearBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule )
{
	pModule->s32SuccessFlag = IMP_FALSE;
	pModule->s32MatchDataUpdated = IMP_FALSE;

	pModule->u32CntMvStartTime = 0;

	pModule->s32ShakingFlag = IMP_FALSE;
	IMP_GA_CArrayClear( &pModule->stShakingHistory );
}

IMP_VOID ipCreateBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule, PEA_BG_MODEL_DATA_S *pstBgModelData, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	memset( pModule, 0, sizeof(PEA_BGMODEL_STABILIZER_S) );
	pModule->pstHwRes = pstHwResource;
	pModule->pstPeaResult = pstResult;
	pModule->pstBgModelData = pstBgModelData;

#ifdef DEBUG_MATCH
	g_ies = pModule;
#endif
	ipClearBgModelStabilizer( pModule );
}

IMP_VOID ipReleaseBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule )
{
	memset( pModule, 0, sizeof(PEA_BGMODEL_STABILIZER_S) );

#ifdef DEBUG_MATCH
	g_ies = NULL;
#endif
}

IMP_VOID ipConfigBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule, PEA_STABILIZE_PARA_S *pstPara )
{
	IMP_S32 i, s32ValidRgCnt;

	memcpy( &pModule->stStabPara, pstPara, sizeof(PEA_STABILIZE_PARA_S) );

	s32ValidRgCnt = 0;
	for( i=0; i<IMP_PEA_MATCH_REGION_NUM; i++ )
	{
		memcpy( &pModule->astMatchData[i].stRegion, &pstPara->astRegions[i], sizeof(IMP_RECT_S) );
		if( pModule->astMatchData[i].stRegion.s16X1 != 0 )
			s32ValidRgCnt++;
	}
	pModule->s32ValidRegionCnt = s32ValidRgCnt;
}


IMP_S32 ipProcessBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule )
{
	PEA_BG_MODEL_DATA_S *pstBgModelData = pModule->pstBgModelData;

	pModule->s32SuccessFlag = IMP_FALSE;
	pModule->s32ShakingFlag = IMP_FALSE;
	pstBgModelData->s32ConstMvDetected = IMP_FALSE;
#ifdef DEBUG_MATCH
	ies_region_match_num = 0;
#endif

	pModule->pstBgModelData->s32DoStabilize = pModule->stStabPara.s32DoStabilize;
	if( !pModule->stStabPara.s32DoStabilize || !IMP_BG_IS_WORKING_NORMAL(pModule->pstBgModelData->pstDetectorData->u32BgStatusCur) )
		return IMP_FALSE;
	if( pModule->s32ValidRegionCnt != IMP_PEA_MATCH_REGION_NUM )
		return IMP_FALSE;
	//当背景模型处于非正常工作状态时，计算运动矢量
	ipCalculateMvector( pModule );
	if( !pModule->stStabPara.s32ShakingUseFgEdge )
		ipDetermineImageShaking( pModule );
	if( pModule->stStabPara.s32ConstMvTimeTh )
		ipDetectConstNon0Mv( pModule );
	if( !pstBgModelData->s32ConstMvDetected )
		ipTranslateImage( pModule );

	return IMP_TRUE;
}

IMP_VOID ipPostProcessBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule )
{
	if( !pModule->stStabPara.s32DoStabilize || !IMP_BG_IS_WORKING_NORMAL(pModule->pstBgModelData->pstDetectorData->u32BgStatusCur) )
		return;
	if( pModule->s32ValidRegionCnt != IMP_PEA_MATCH_REGION_NUM )
		return;

	if( pModule->stStabPara.s32ShakingUseFgEdge )
		ipDetermineImageShaking( pModule );
}







