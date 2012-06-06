#include "imp_pea_regionset_suppress_fg.h"


IMP_VOID ipGetMagDiffImage(PEA_RESULT_S *pstResult,PEA_RGE_SUPPRESS_FG_PARA_S *pstParam)
{
	GRAY_IMAGE_S *pstImgInMag = pstResult->stDRegionSet.pstImgBgEdgeMag;
	GRAY_IMAGE_S *pstImgBgMag = pstResult->stDRegionSet.pstImgInEdgeMag;
	GRAY_IMAGE_S *pstImgMagDiff = pstResult->stDRegionSet.pstImgBgInDiffMag;
	IMP_S32 s32DiffThreth = pstParam->s32MagThreshold;
	IMP_S32 s32ImgW = pstImgInMag->s32W;
	IMP_S32 s32ImgH = pstImgInMag->s32H;
	IMP_S32 x,y;
	IMP_U8 *pu8DataS1,*pu8DataS2;
	IMP_U8 *pu8DataD1;

	IMP_S32 u32Val11, u32Val12, u32Val13;
	IMP_S32 u32Val21, u32Val22, u32Val23;
	IMP_S32 val31, val32, val33;
	IMP_S32 val41, val42, val43;

	for( y = 0; y < s32ImgH; y++ )
	{
		pu8DataS1 = pstImgInMag->pu8Data + s32ImgW * y;
		pu8DataS2 = pstImgBgMag->pu8Data + s32ImgW * y;

		pu8DataD1 = pstImgMagDiff->pu8Data + s32ImgW * y;

#ifdef _TIDSP
#pragma MUST_ITERATE(64,,4);
#endif
		for( x = 0; x < s32ImgW; x += 4 )
		{
			u32Val11 = pu8DataS1[0];
			u32Val12 = pu8DataS2[0];

			u32Val21 = pu8DataS1[1];
			u32Val22 = pu8DataS2[1];

			val31 = pu8DataS1[2];
			val32 = pu8DataS2[2];

			val41 = pu8DataS1[3];
			val42 = pu8DataS2[3];

			u32Val13 = abs(u32Val12-u32Val11)>s32DiffThreth ? 255 : 0;
			u32Val23 = abs(u32Val22-u32Val21)>s32DiffThreth ? 255 : 0;
			val33 = abs(val32-val31)>s32DiffThreth ? 255 : 0;
			val43 = abs(val42-val41)>s32DiffThreth ? 255 : 0;

			//			u32Val13 = (u32Val11-u32Val12)>s32DiffThreth ? 255 : 0;
			//			u32Val23 = (u32Val21-u32Val22)>s32DiffThreth ? 255 : 0;
			//			val33 = (val31-val32)>s32DiffThreth ? 255 : 0;
			//			val43 = (val41-val42)>s32DiffThreth ? 255 : 0;

			pu8DataD1[0] = u32Val13;
			pu8DataD1[1] = u32Val23;
			pu8DataD1[2] = val33;
			pu8DataD1[3] = val43;

			pu8DataS1 += 4; pu8DataS2 += 4;
			pu8DataD1 += 4;
		}
	}
}

IMP_VOID ipRemoveRegionSetLight(PEA_RESULT_S *pstResult,PEA_RGE_SUPPRESS_FG_PARA_S *pstParam, PEA_DETECTED_REGIONSET_S *pstRgs)
{
	GRAY_IMAGE_S *pstImgMagDiff = pstResult->stDRegionSet.pstImgBgInDiffMag;
	GRAY_IMAGE_S *pstImgInMag = pstResult->stDRegionSet.pstImgInEdgeMag;
	GRAY_IMAGE_S *pstImgBgMag = pstResult->stDRegionSet.pstImgBgEdgeMag;
	IMP_S32 x,y;

	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_U8 *pu8DataS1;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 i, s32Cnt, s32Sn0;
	IMP_S32 s32Width, s32Height;
	IMP_S32 s32AreaPixel = 0;
	IMP_S32 s32MinMagDiffNum = pstParam->s32MinMagDiffNum;
	IMP_S32 s32MinMagDiffRatio = pstParam->s32MinMagDiffRatio;
	IMP_S32 s32ImgW = pstImgMagDiff->s32W;
	IMP_S32 s32ImgH = pstImgMagDiff->s32H;
	IMP_S32 s32DiagonalLen = 0;
	ipGetMagDiffImage(pstResult,pstParam);
	ipMorphCloseImage( pstImgMagDiff, pstImgMagDiff, pstResult->stDRegionSet.pstImgMediate, 1 );
	IMP_ITERATE_RGS_START
		if( IMP_DRG_IS_LABELED(pstDrg->u8Used) || IMP_DRG_IS_COMPLETE(pstDrg->u8Used) )
		{
			s32AreaPixel = 0;
			s32Width = pstDrg->stRect.s16X2-pstDrg->stRect.s16X1+1;
			s32Height = pstDrg->stRect.s16Y2-pstDrg->stRect.s16Y1+1;
			lb_x = pstDrg->stRect.s16X1;
			ru_x = pstDrg->stRect.s16X2;
			lb_y = pstDrg->stRect.s16Y1;
			ru_y = pstDrg->stRect.s16Y2;
			s32DiagonalLen = abs(lb_x-ru_x) + abs(lb_y-ru_y);
			s32Sn0 = pstDrg->u8Sign;
			pu8DataS1 = pstImgMagDiff->pu8Data + pstDrg->stRect.s16Y1*s32ImgW;
			for( y = lb_y; y <= ru_y; y++ )
			{
				for( x = lb_x; x <= ru_x; x++ )
				{
					if( pu8DataS1[x] == 255 )
						s32AreaPixel++;
					//data[xx] = s32Sn;
				}
				pu8DataS1 += s32ImgW;
			}
			if (s32AreaPixel < s32MinMagDiffNum || s32AreaPixel < s32MinMagDiffRatio * 2 * (s32Width + s32Height))
			{
				pu8DataS1 = pstRgs->pstImgFgRgn->pu8Data + pstDrg->stRect.s16Y1 * s32ImgW;
				for( y = lb_y; y <= ru_y; y++ )
				{
					for( x = lb_x; x <= ru_x; x++ )
					{
						if( pu8DataS1[x] == s32Sn0 )
							pu8DataS1[x] = 0;
					}
					pu8DataS1 += s32ImgW;
				}
				ipDetectedRegionSetFree( pstRgs, i );
			}
		}
		IMP_ITERATE_RGS_END
}

IMP_VOID ipSuppressRegionSetForeground( PEA_RGE_SUPPRESS_FG_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, s32Cnt;
	PEA_DETECTED_REGION_S *pstDrg;

	if( !pstPara->s32LightEnable && !pstPara->s32PenumbraEnable)
		return;

	IMP_ITERATE_RGS_START
		IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	{
		if( IMP_DRG_IS_DIFF(pstDrg->u8Used) || IMP_DRG_IS_LIGHT(pstDrg->u8Used) )
			break;

		ipSuppressRegionForeground( pstPara, pstRgs, i );
	}
	IMP_ITERATE_RGS_END
}
IMP_VOID ipSuppressRegionForeground( PEA_RGE_SUPPRESS_FG_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgInEdge = pstRgs->pstImgInEdge;
	GRAY_IMAGE_S *pstImgBgEdge = pstRgs->pstImgBgEdge;
	PEA_DETECTED_REGION_S *pstDrg = pstRgs->astDrg+s32Index;
	IMP_S32 s32Width, s32Height;
	IMP_S32 s32DiffMeanB, s32CntB;
	IMP_S32 s32DiffMeanF, s32CntF;
	IMP_S32 s32EdgeLike;
	IMP_S32 s32DiffCalced = IMP_FALSE;
	IMP_RECT_S *pstRect = (IMP_RECT_S*)(&pstDrg->stRect.s16X1);

	// 	if(	pstDrg->s32AreaPixel < pstPara->supfg_min_area_pixel
	// 		|| pstDrg->s32AreaPixel > pstPara->supfg_max_area_pixel
	// 		)
	// 		return;

	if( pstPara->s32LightEnable &&
		( pstDrg->s32AreaPixel >= pstPara->s32MinLightArea && pstDrg->s32AreaPixel <= pstPara->s32MaxLightArea )
		)
	{
		s32Width = pstDrg->stRect.s16X2-pstDrg->stRect.s16X1 + 1;
		s32Height = pstDrg->stRect.s16Y2-pstDrg->stRect.s16Y1 + 1;

		if( pstPara->s32LightUseDiff )
		{
			ipCalcRegionGrayDiff( pstRgs, s32Index, &s32DiffMeanB, &s32CntB, &s32DiffMeanF, &s32CntF );
			s32DiffCalced = IMP_TRUE;
			if( s32CntF >= (s32Width + s32Height) && s32DiffMeanF >= s32DiffMeanB )
			{
				IMP_DRG_SET_LIGHT(pstDrg->u8Used);
				return;
			}
		}

#if 0
		int w = pstImgInEdge->s32W , h = pstImgInEdge->s32H;
		ipShowBinImage(w, h, pstImgInEdge->pu8Data, "pstImgInEdge");
		ipShowBinImage(w, h, pstImgBgEdge->pu8Data, "pstImgBgEdge");
#endif

		s32EdgeLike = ipCalcRegionEdgeLikehood( pstImgInEdge, pstImgBgEdge,
			pstImgFgRgn, pstRect, pstDrg->u8Sign, 0,
			pstPara->s32LightEdgeShapeRatio,
			pstPara->s32LightEdgeDistRatio,
			pstPara->s32LightEdgeLikehoodRatioFg,
			pstPara->s32LightEdgeLikehoodRatioBg );
		if( s32EdgeLike )
		{
			IMP_DRG_SET_LIGHT(pstDrg->u8Used);
			return;
		}
	}

	if( pstPara->s32PenumbraEnable &&
		( pstDrg->s32AreaPixel >= pstPara->s32MinPenuArea && pstDrg->s32AreaPixel <= pstPara->s32MaxPenuArea )
		)
	{
		if( !s32DiffCalced )
		{
			ipCalcRegionGrayDiff( pstRgs, s32Index, &s32DiffMeanB, &s32CntB, &s32DiffMeanF, &s32CntF );
			s32DiffCalced = IMP_TRUE;
		}

		s32DiffMeanB /= pstPara->s32PenuDiffRatio;
		if( s32DiffMeanB > pstPara->s32PenuDiffMax )
			s32DiffMeanB = pstPara->s32PenuDiffMax;
		if( s32DiffMeanB <= pstPara->s32PenuThreshold )
			return;

		ipReCalcDregionRect( pstRgs, s32Index, s32DiffMeanB );
	}
}


IMP_VOID ipReCalcDregionRect( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index, IMP_S32 s32ThDiff )
{
	GRAY_IMAGE_S *pstImgFgOrg = pstRgs->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	PEA_DETECTED_REGION_S *pstDrg = pstRgs->astDrg+s32Index;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S8 *ps8DatFgo;
	IMP_U8 *ps8DatFgr;
	IMP_S32 s32Sn, s32Fgo, s32Fgr;
	IMP_S32 s32Cnt, x, y;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 ys, ye, xs, xe, s32LineFg;

	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Sn = pstDrg->u8Sign;
	s32Cnt = 0;

	xs = 10000; xe = 0;
	ys = 10000; ye = 0;
	lb_x = lb_x & (~1);
	ru_x = ru_x | 1;
	ps8DatFgo = (IMP_S8*)pstImgFgOrg->pu8Data + s32ImgW * lb_y;
	ps8DatFgr = pstImgFgRgn->pu8Data + s32ImgW * lb_y;
	for( y = lb_y; y <= ru_y; y++ )
	{
		s32LineFg = 0;

		for( x = lb_x; x <= ru_x; x += 2 )
		{
			IMP_S32 s32Fgo2, s32Fgr2;
			s32Fgo = ps8DatFgo[x];
			s32Fgr = ps8DatFgr[x];
			s32Fgo2 = ps8DatFgo[x+1];
			s32Fgr2 = ps8DatFgr[x+1];

			if( s32Fgr == s32Sn && abs(s32Fgo) < s32ThDiff )
				s32Fgr = 0;
			ps8DatFgr[x] = s32Fgr;
			if( s32Fgr==s32Sn )
			{
				s32Cnt++;
				s32LineFg = 1;
				if( xs > x )
					xs = x;
				if( xe < x )
					xe = x;
			}

			if( s32Fgr2 == s32Sn && abs(s32Fgo2) < s32ThDiff )
				s32Fgr2 = 0;
			ps8DatFgr[x + 1] = s32Fgr2;
			if( s32Fgr2 == s32Sn )
			{
				s32Cnt++;
				s32LineFg = 1;
				if( xs > x + 1 )
					xs = x + 1;
				if( xe < x + 1 )
					xe = x + 1;
			}
		}

		if( s32LineFg )
		{
			if( ys > y )
				ys = y;
			if( ye < y )
				ye = y;
		}

		ps8DatFgo += s32ImgW;
		ps8DatFgr += s32ImgW;
	}

	pstDrg->s32AreaPixel = s32Cnt;
	lb_x = xs; ru_x = xe;
	lb_y = ys; ru_y = ye;
	pstDrg->stRect.s16X1 = lb_x;
	pstDrg->stRect.s16X2 = ru_x;
	pstDrg->stRect.s16Y1 = lb_y;
	pstDrg->stRect.s16Y2 = ru_y;
}

IMP_VOID ipCalcRegionGrayDiff( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index,
								 IMP_S32 *ps32DiffB, IMP_S32 *ps32CntB,
								 IMP_S32 *ps32DiffF, IMP_S32 *ps32CntF )
{
	GRAY_IMAGE_S *pstImgFgOrg = pstRgs->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgFrmDiff = pstRgs->pstImgFrmDiff;
	PEA_DETECTED_REGION_S *pstDrg = pstRgs->astDrg+s32Index;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S32 x, y;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 s32Sn, s32IsFg;
	IMP_U8 *ps8DatFgr;
	IMP_S32 s32Fgr;
	IMP_S8 *ps8DatFgo;
	IMP_S32 s32DiffMeanB, s32CntB, s32DiffB, s32Fgo;
	IMP_S8 *dat_diff;
	IMP_S32 s32DiffMeanF, s32CntF, s32DiffF, s32Fgd;

	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Sn = pstDrg->u8Sign;

	s32DiffMeanB = 0; s32CntB = 0;
	s32DiffMeanF = 0; s32CntF = 0;
	dat_diff = (IMP_S8*)pstImgFrmDiff->pu8Data + s32ImgW*lb_y;
	ps8DatFgo = (IMP_S8*)pstImgFgOrg->pu8Data + s32ImgW*lb_y;
	ps8DatFgr = pstImgFgRgn->pu8Data + s32ImgW*lb_y;

	for( y = lb_y; y <= ru_y; ++y )
	{
		for( x = lb_x; x <= ru_x; ++x )
		{
			s32Fgr = ps8DatFgr[x];
			s32Fgo = ps8DatFgo[x];
			s32Fgd = dat_diff[x];
			s32IsFg = (s32Fgr == s32Sn);

			if( s32IsFg && s32Fgo )
			{
				s32DiffB = abs(s32Fgo);
				s32DiffMeanB += s32DiffB;
				s32CntB++;
			}

			if( s32IsFg && s32Fgd )
			{
				s32DiffF = abs(s32Fgd);
				s32DiffMeanF += s32DiffF;
				s32CntF++;
			}
		}

		ps8DatFgo += s32ImgW;
		ps8DatFgr += s32ImgW;
		dat_diff += s32ImgW;
	}
	if( s32CntB )
		s32DiffMeanB /= s32CntB;
	if( s32CntF )
		s32DiffMeanF /= s32CntF;

	*ps32DiffB = s32DiffMeanB;
	*ps32DiffF = s32DiffMeanF;
	*ps32CntB = s32CntB;
	*ps32CntF = s32CntF;
}



IMP_VOID ipMatchRegionEdgeM0( GRAY_IMAGE_S *pstImage1, GRAY_IMAGE_S *pstImage2,
					   GRAY_IMAGE_S *pstImageFg, IMP_RECT_S *pstRect, IMP_U32 u32Sn,
					   IMP_S32 *ps32EdgeNum1, IMP_S32 *ps32EmatNum1,
					   IMP_S32 *pEdgeNum2, IMP_S32 *ps32EmatNum2,
					   IMP_POINT_S *pstEdgeCenter1, IMP_POINT_S *pstEdgeCenter2 )
{
#ifndef _TIDSP
#define DEBUG_EMAT
#endif
	IMP_S32 yy, xx, s32Oft, s32ImgW;
	IMP_S32 xs, xe, ys, ye;
	IMP_U8 *pu8Data1, *pu8Data2, *pu8Data3;
	IMP_S32 s32EdgeNum1, s32EMatNum1;
	IMP_S32 s32EdgeNum2, s32EMatNum2;
	IMP_S32 s32EdgeCen1X, s32EdgeCen1Y;
	IMP_S32 s32EdgeCen2X, s32EdgeCen2Y;
	IMP_U32 u32Val00, s32IsFg;
	IMP_U32 u32Val12, u32Val22;
	IMP_U32 u32Cond11a, u32Cond21a;
	IMP_U32 u32IsEdge1, u32IsEdge2;

	s32ImgW = pstImage1->s32W;
	xs = pstRect->s16X1;
	xe = pstRect->s16X2;
	ys = pstRect->s16Y1;
	ye = pstRect->s16Y2;
	s32Oft = s32ImgW * ys;
	pu8Data1 = pstImage1->pu8Data + s32Oft;
	pu8Data2 = pstImage2->pu8Data + s32Oft;
	if( pstImageFg )
		pu8Data3 = pstImageFg->pu8Data + s32Oft;
	else
		pu8Data3 = NULL;
	s32EdgeNum1 = s32EMatNum1 = 0;
	s32EdgeNum2 = s32EMatNum2 = 0;
	s32EdgeCen1X = s32EdgeCen1Y = 0;
	s32EdgeCen2X = s32EdgeCen2Y = 0;
	u32Val00 = u32Sn;

	for( yy = ys; yy <= ye; yy++ )
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(32,,4);
#endif
		for( xx = xs; xx <= xe; xx += 1 )
		{
			if( pu8Data3 )
				u32Val00 = pu8Data3[xx];
			u32Val12 = pu8Data1[xx];
			u32Val22 = pu8Data2[xx];
			s32IsFg = (u32Val00 == u32Sn);
			u32Cond21a = u32Val12;
			u32Cond11a = u32Val22;
			u32IsEdge1 = ( s32IsFg && u32Val12 );
			u32IsEdge2 = ( s32IsFg && u32Val22 );

			if( u32IsEdge1 )
			{
				s32EdgeNum1++;
				s32EdgeCen1X += xx;
				s32EdgeCen1Y += yy;
#ifdef DEBUG_EMAT
				pu8Data1[xx] = 0xFE;
#endif
			}
			if( u32IsEdge1 && u32Cond11a )
			{
				s32EMatNum1++;
#ifdef DEBUG_EMAT
				pu8Data1[xx] |= 0x01;
#endif
			}
			if( u32IsEdge2 )
			{
				s32EdgeNum2++;
				s32EdgeCen2X += xx;
				s32EdgeCen2Y += yy;
#ifdef DEBUG_EMAT
				pu8Data2[xx] = 0xFE;
#endif
			}
			if( u32IsEdge2 && u32Cond21a )
			{
				s32EMatNum2++;
#ifdef DEBUG_EMAT
				pu8Data2[xx] |= 0x01;
#endif
			}
		}

		pu8Data1 += s32ImgW;
		pu8Data2 += s32ImgW;
		pu8Data3 += ( pu8Data3 ? s32ImgW : 0 );
	}

	if( s32EdgeNum1 )
	{
		s32EdgeCen1X = (s32EdgeCen1X + (s32EdgeNum1 >> 1)) / s32EdgeNum1;
		s32EdgeCen1Y = (s32EdgeCen1Y  +(s32EdgeNum1 >> 1)) / s32EdgeNum1;
	}
	if( s32EdgeNum2 )
	{
		s32EdgeCen2X = (s32EdgeCen2X + (s32EdgeNum2 >> 1)) / s32EdgeNum2;
		s32EdgeCen2Y = (s32EdgeCen2Y + (s32EdgeNum2 >> 1)) / s32EdgeNum2;
	}

	*ps32EdgeNum1 = s32EdgeNum1;
	*ps32EmatNum1 = s32EMatNum1;
	*pEdgeNum2 = s32EdgeNum2;
	*ps32EmatNum2 = s32EMatNum2;
	if( pstEdgeCenter1 )
	{
		pstEdgeCenter1->s16X = s32EdgeCen1X;
		pstEdgeCenter1->s16Y = s32EdgeCen1Y;
	}
	if( pstEdgeCenter2 )
	{
		pstEdgeCenter2->s16X = s32EdgeCen2X;
		pstEdgeCenter2->s16Y = s32EdgeCen2Y;
	}
}

IMP_VOID ipMatchRegionEdgeM1( GRAY_IMAGE_S *pstImage1, GRAY_IMAGE_S *pstImage2,
					   GRAY_IMAGE_S *pstImageFg, IMP_RECT_S *pstRect, IMP_U32 u32Sn,
					   IMP_S32 *ps32EdgeNum1, IMP_S32 *ps32EmatNum1,
					   IMP_S32 *pEdgeNum2, IMP_S32 *ps32EmatNum2,
					   IMP_POINT_S *pstEdgeCenter1, IMP_POINT_S *pstEdgeCenter2 )
{
#ifndef _TIDSP
#define DEBUG_EMAT
#endif
	IMP_S32 yy, xx, s32Oft, s32ImgW;
	IMP_S32 xs, xe, ys, ye;
	IMP_U8 *pu8Data1, *pu8Data2, *pu8Data3;
	IMP_S32 s32EdgeNum1, s32EMatNum1;
	IMP_S32 s32EdgeNum2, s32EMatNum2;
	IMP_S32 s32EdgeCen1X, s32EdgeCen1Y;
	IMP_S32 s32EdgeCen2X, s32EdgeCen2Y;
	IMP_U32 u32Val00, s32IsFg;
	IMP_U32 u32Val11, u32Val13;
	IMP_U32 u32Val21, u32Val23;
	IMP_U32 u32Val12, u32Val22;
	IMP_U32 u32Cond11a, u32Cond21a;
	IMP_U32 u32IsEdge1, u32IsEdge2;

	s32ImgW = pstImage1->s32W;
	xs = pstRect->s16X1;
	xe = pstRect->s16X2;
	ys = pstRect->s16Y1;
	ye = pstRect->s16Y2;
	if( xs < 1 )
		xs = 1;
	if( xe > s32ImgW - 2 )
		xe = s32ImgW - 2;
	s32Oft = s32ImgW * ys;
	pu8Data1 = pstImage1->pu8Data + s32Oft;
	pu8Data2 = pstImage2->pu8Data + s32Oft;
	if( pstImageFg )
		pu8Data3 = pstImageFg->pu8Data + s32Oft;
	else
		pu8Data3 = NULL;
	s32EdgeNum1 = s32EMatNum1 = 0;
	s32EdgeNum2 = s32EMatNum2 = 0;
	s32EdgeCen1X = s32EdgeCen1Y = 0;
	s32EdgeCen2X = s32EdgeCen2Y = 0;
	u32Val00 = u32Sn;

	for( yy = ys; yy <= ye; yy++ )
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(32,,4);
#endif
		for( xx = xs; xx <= xe; xx += 1 )
		{
			if( pu8Data3 )
				u32Val00 = pu8Data3[xx];
			u32Val11 = pu8Data1[xx - 1];
			u32Val12 = pu8Data1[xx];
			u32Val13 = pu8Data1[xx + 1];
			u32Val21 = pu8Data2[xx - 1];
			u32Val22 = pu8Data2[xx];
			u32Val23 = pu8Data2[xx + 1];
			s32IsFg = (u32Val00 == u32Sn);
			u32Cond21a = u32Val12 + u32Val13 + u32Val11;
			u32Cond11a = u32Val22 + u32Val23 + u32Val21;
			u32IsEdge1 = ( s32IsFg && u32Val12 );
			u32IsEdge2 = ( s32IsFg && u32Val22 );

			if( u32IsEdge1 )
			{
				s32EdgeNum1++;
				s32EdgeCen1X += xx;
				s32EdgeCen1Y += yy;
#ifdef DEBUG_EMAT
				pu8Data1[xx] = 0xFE;
#endif
			}
			if( u32IsEdge1 && u32Cond11a )
			{
				s32EMatNum1++;
#ifdef DEBUG_EMAT
				pu8Data1[xx] |= 0x01;
#endif
			}
			if( u32IsEdge2 )
			{
				s32EdgeNum2++;
				s32EdgeCen2X += xx;
				s32EdgeCen2Y += yy;
#ifdef DEBUG_EMAT
				pu8Data2[xx] = 0xFE;
#endif
			}
			if( u32IsEdge2 && u32Cond21a )
			{
				s32EMatNum2++;
#ifdef DEBUG_EMAT
				pu8Data2[xx] |= 0x01;
#endif
			}
		}

		pu8Data1 += s32ImgW;
		pu8Data2 += s32ImgW;
		pu8Data3 += ( pu8Data3 ? s32ImgW : 0 );
	}

	if( s32EdgeNum1 )
	{
		s32EdgeCen1X = (s32EdgeCen1X + (s32EdgeNum1 >> 1)) / s32EdgeNum1;
		s32EdgeCen1Y = (s32EdgeCen1Y + (s32EdgeNum1 >> 1)) / s32EdgeNum1;
	}
	if( s32EdgeNum2 )
	{
		s32EdgeCen2X = (s32EdgeCen2X + (s32EdgeNum2 >> 1)) / s32EdgeNum2;
		s32EdgeCen2Y = (s32EdgeCen2Y + (s32EdgeNum2 >> 1)) / s32EdgeNum2;
	}

	*ps32EdgeNum1 = s32EdgeNum1;
	*ps32EmatNum1 = s32EMatNum1;
	*pEdgeNum2 = s32EdgeNum2;
	*ps32EmatNum2 = s32EMatNum2;
	if( pstEdgeCenter1 )
	{
		pstEdgeCenter1->s16X = s32EdgeCen1X;
		pstEdgeCenter1->s16Y = s32EdgeCen1Y;
	}
	if( pstEdgeCenter2 )
	{
		pstEdgeCenter2->s16X = s32EdgeCen2X;
		pstEdgeCenter2->s16Y = s32EdgeCen2Y;
	}
}

IMP_S32 ipCalcRegionEdgeLikehood( GRAY_IMAGE_S *pstImage1, GRAY_IMAGE_S *pstImage2,
					GRAY_IMAGE_S *pstImageFg, IMP_RECT_S *pstRect, IMP_S32 s32Sn, IMP_S32 s32Flag,
					IMP_S32 s32EdgeShapeRatio, IMP_S32 s32EdgeDistRation,
					IMP_S32 s32EdgeLikeRatio1, IMP_S32 s32EdgeLikeRatio2 )
{
	IMP_S32 s32EdgeNum1, s32EMatNum1;
	IMP_S32 s32EdgeNum2, s32EMatNum2;
	IMP_S32 s32EdgeLike1, s32EdgeLike2, s32EdgeLike;
	IMP_POINT_S stEdgeCen1, stEdgeCen2;
	IMP_S32 s32DistX, s32DistY;
	IMP_S32 s32Width, s32Height;

	if( s32Flag==0 )
	{
		ipMatchRegionEdgeM0( pstImage1, pstImage2, pstImageFg, pstRect, s32Sn, &s32EdgeNum1, &s32EMatNum1,
							&s32EdgeNum2, &s32EMatNum2, &stEdgeCen1, &stEdgeCen2 );
	}
	else
	{
		ipMatchRegionEdgeM1( pstImage1, pstImage2, pstImageFg, pstRect, s32Sn, &s32EdgeNum1, &s32EMatNum1,
							&s32EdgeNum2, &s32EMatNum2, &stEdgeCen1, &stEdgeCen2 );
	}

	s32Width = pstRect->s16X2-pstRect->s16X1 + 1;
	s32Height = pstRect->s16Y2-pstRect->s16Y1 + 1;
	s32EdgeLike1 = 100;
	s32EdgeLike2 = 100;
	if( s32EdgeNum1 * 100 > (s32Width + s32Height) * s32EdgeShapeRatio )
		s32EdgeLike1 = s32EMatNum1 * 100 / s32EdgeNum1;
	if( s32EdgeNum2 * 100 > (s32Width + s32Height) * s32EdgeShapeRatio )
		s32EdgeLike2 = s32EMatNum2 * 100 / s32EdgeNum2;

	s32DistX = abs( stEdgeCen1.s16X - stEdgeCen2.s16X);
	s32DistY = abs( stEdgeCen1.s16Y - stEdgeCen2.s16Y);

	s32EdgeLike = ( 1
				&& ( s32EdgeLike1 >= s32EdgeLikeRatio1 )
				&& ( s32EdgeLike2 >= s32EdgeLikeRatio2 )
				&& ( s32DistX * 100 <= s32Width * s32EdgeDistRation )
				&& ( s32DistY * 100 <= s32Height * s32EdgeDistRation )
				);

	return s32EdgeLike;
}
