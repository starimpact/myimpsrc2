#include "imp_pea_regionset_label.h"

static IMP_VOID ipSetRegionsLabeled( PEA_DETECTED_REGIONSET_S *pstRgs );
static IMP_VOID ipRemoveSmallRegions( PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );
static IMP_VOID ipRemoveSmallRegionsOnFull( PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );
static IMP_VOID ipRemoveSmallRegionsOnLine( PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_VOID IMP_PEA_RGE_ExtractRegionSetFromFgImage(
												 PEA_RGE_LABLE_PARA_S *pstPara,
												 PEA_DETECTED_REGIONSET_S *pstRgs,
												 IMP_RECT_S *pstRc, IMP_S32 s32Diff,
												 PEA_REGION_EXTRACT_DATA_S *pstRgeData
												 )
{
	IMP_S32 x, y;
	IMP_S32 rl_s, rl_e;
	IMP_U8 *pu8DataPre, *pu8DataCur;
	PEA_DETECTED_REGION_S *pstDrg, *pstDrg0;
	IMP_S32 s32RgSn, s32Sn0, x1;
	IMP_S32 xs, xe, ys, ye;
	IMP_S32 s32RgsStatus, s32FgArea;
	IMP_S32 s32ThDrgMinSizeWh = pstPara->s32ThDrgMinSizeWh;
	GRAY_IMAGE_S *img_fg = pstRgs->pstImgFgRgn;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
//	IMP_S32 s32ImgH = pstRgs->pstImgFgRgn->s32H;

	pstPara->s32ThDrgMinSizePixelCur = pstPara->s32ThDrgMinSizePixelInit;

	xs = pstRc->s16X1; ys = pstRc->s16Y1;
	xe = pstRc->s16X2; ye = pstRc->s16Y2;

	s32RgsStatus = 2; s32FgArea = 0;
	pu8DataPre = pu8DataCur = img_fg->pu8Data + s32ImgW * ys;
	for( y = ys; y <= ye; y++ )
	{
		ipSetRegionsLabeled( pstRgs );

		x = xs;
		do
		{
			while( x <= xe && pu8DataCur[x] != 255 ) ++x;
			if( x > xe ) break;
			rl_s = x;
			while( x <= xe && pu8DataCur[x] == 255 ) ++x;
			rl_e = x-1;

			s32FgArea += (rl_e - rl_s + 1);

			if( y>ys )
			{
				x1 = rl_s;
				s32Sn0 = 0;
				do
				{
					while( x1 <= rl_e && pu8DataPre[x1] == 0 ) ++x1;
					if( x1 > rl_e ) break;
					while( x1 <= rl_e && pu8DataPre[x1] != 0 ) ++x1;
					s32RgSn = pu8DataPre[x1 - 1];
					if( pu8DataCur[rl_s] == 255 )
					{
						memset( &pu8DataCur[rl_s], s32RgSn, rl_e - rl_s + 1 );
						s32Sn0 = s32RgSn;
						pstDrg0 = &pstRgs->astDrg[s32Sn0];
						IMP_ASSERT(pstDrg0->u8Used);
						pstDrg0->stRect.s16Y2 = y;
						pstDrg0->stRect.s16X1 = pstDrg0->stRect.s16X1 <= rl_s ? pstDrg0->stRect.s16X1 : rl_s;
						pstDrg0->stRect.s16X2 = pstDrg0->stRect.s16X2 >= rl_e ? pstDrg0->stRect.s16X2 : rl_e;
						pstDrg0->s32AreaPixel += (rl_e - rl_s + 1);
						IMP_DRG_SET_LABELING(pstDrg0->u8Used);
					}
					else if( s32RgSn != s32Sn0 )
					{
						pstDrg0 = &pstRgs->astDrg[s32Sn0];
						pstDrg = &pstRgs->astDrg[s32RgSn];
						if( pstDrg0->stRect.s16X1>pstDrg->stRect.s16X1 ) pstDrg0->stRect.s16X1=pstDrg->stRect.s16X1;
						if( pstDrg0->stRect.s16Y1>pstDrg->stRect.s16Y1 ) pstDrg0->stRect.s16Y1=pstDrg->stRect.s16Y1;
						if( pstDrg0->stRect.s16X2<pstDrg->stRect.s16X2 ) pstDrg0->stRect.s16X2=pstDrg->stRect.s16X2;
						if( pstDrg0->stRect.s16Y2<pstDrg->stRect.s16Y2 ) pstDrg0->stRect.s16Y2=pstDrg->stRect.s16Y2;
						pstDrg0->s32AreaPixel += pstDrg->s32AreaPixel;
						IMP_PEA_RGE_RemoveRegion( pstRgs, s32RgSn, s32Sn0 );
					}

				} while( x1 <= rl_e );
			}

			if( pu8DataCur[rl_s] == 255 )
			{
				IMP_S32 s32RunLen = rl_e - rl_s + 1;
				IMP_S32 s32IsBig = (s32RunLen >= s32ThDrgMinSizeWh);

				s32RgSn = 0;
				if( s32RgsStatus == 2 || s32IsBig )
					s32RgSn = ipDetectedRegionSetAlloc( pstRgs );
				if( !s32RgSn && s32RgsStatus && s32IsBig )
				{
					ipRemoveSmallRegionsOnFull( pstPara, pstRgs );
					s32RgSn = ipDetectedRegionSetAlloc( pstRgs );
					s32RgsStatus = 1;
					if( !s32RgSn )
						s32RgsStatus = 0;
				}

				if( s32RgSn )
				{
					pstDrg = &pstRgs->astDrg[s32RgSn];
					memset( &pu8DataCur[rl_s], s32RgSn, s32RunLen );
					pstDrg->stRect.s16X1 = rl_s;
					pstDrg->stRect.s16Y1 = y;
					pstDrg->stRect.s16X2 = rl_e;
					pstDrg->stRect.s16Y2 = y;
					pstDrg->s32AreaPixel = s32RunLen;
					pstDrg->u8Sign = s32RgSn;
				//	printf("s32RgSn:%d ", s32RgSn);
					IMP_DRG_SET_LABELING(pstDrg->u8Used);
					if( s32Diff )
						IMP_DRG_SET_DIFF(pstDrg->u8Used);
				}
				else
				{
					memset( &pu8DataCur[rl_s], 0, s32RunLen );
				}
			}

		} while( x <= xe );
		pu8DataPre = pu8DataCur;
		pu8DataCur += s32ImgW;

		if( y == ye )
			ipSetRegionsLabeled( pstRgs );

		ipRemoveSmallRegionsOnLine( pstPara, pstRgs );
	}

	if( pstRgeData )
	{
// 		rgedata->detdata->noise_area = s32FgArea - calc_region_fg_area( pstRgs );
	}

	ipRemoveSmallRegions( pstPara, pstRgs );
}

IMP_VOID IMP_PEA_RGE_RemoveRegion( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index, IMP_U8 u8Sn )
{
	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_U8 *pu8Data;
	IMP_S32 s32Sn0;
	IMP_S32 xx, yy;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	GRAY_IMAGE_S *img_fg = pstRgs->pstImgFgRgn;

	pstDrg = pstRgs->astDrg + s32Index;
	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32Sn0 = pstDrg->u8Sign;
	pu8Data = img_fg->pu8Data + pstDrg->stRect.s16Y1*img_fg->s32W;
	for( yy = lb_y; yy <= ru_y; yy++ )
	{
		for( xx = lb_x; xx <= ru_x; xx++ )
		{
			if( pu8Data[xx] == s32Sn0 )
				pu8Data[xx] = u8Sn;
		}
		pu8Data += img_fg->s32W;
	}
	ipDetectedRegionSetFree( pstRgs, s32Index );
}

static IMP_VOID ipSetRegionsLabeled( PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, s32Cnt;
	PEA_DETECTED_REGION_S	*pstDrg;

	IMP_ITERATE_RGS_START
	if( IMP_DRG_IS_LABELING(pstDrg->u8Used) )
	{
		IMP_DRG_SET_LABELED(pstDrg->u8Used);
	}
	IMP_ITERATE_RGS_END
}

// static IMP_S32 calc_region_fg_area( PEA_DETECTED_REGIONSET_S *pstRgs )
// {
// 	IMP_S32	*pstDrg;
// 	IMP_S32 i, s32Cnt;
// 	IMP_S32 s32AreaPixel=0;
//
// 	ITERATE_RGS_START
// 	if( IP_DRG_IS_COMPLETE(pstDrg->u8Used) )
// 	{
// 		s32AreaPixel += pstDrg->s32AreaPixel;
// 	}
// 	ITERATE_RGS_END
//
// 	return s32AreaPixel;
// }
//
static IMP_VOID ipRemoveSmallRegionsOnFull( PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_S32 i, s32Cnt;
	IMP_S32 s32Del, s32AreaPixel;
	IMP_S32 s32ThDrgMinSizeWh = pstPara->s32ThDrgMinSizeWh;
	IMP_S32 s32ThDrgMinSizePixel = pstPara->s32ThDrgMinSizePixelCur;

	IMP_ITERATE_RGS_START
	if( IMP_DRG_IS_LABELING(pstDrg->u8Used) || IMP_DRG_IS_LABELED(pstDrg->u8Used) )
	{
		s32AreaPixel = pstDrg->s32AreaPixel;
		s32Del = ( s32AreaPixel < s32ThDrgMinSizeWh );
		if( s32Del )
			IMP_PEA_RGE_RemoveRegion( pstRgs, i, 0 );
	}
	IMP_ITERATE_RGS_END

	while( ipDetectedRegionSetIsEmpty(pstRgs) )
	{
		if( !pstPara->s32ThDrgMinSizePixelStep
			|| ( pstPara->s32ThDrgMinSizePixelCur + pstPara->s32ThDrgMinSizePixelStep ) > pstPara->s32ThDrgMinSizePixelMax )
			break;
		pstPara->s32ThDrgMinSizePixelCur += pstPara->s32ThDrgMinSizePixelStep;
		s32ThDrgMinSizePixel = pstPara->s32ThDrgMinSizePixelCur;

		IMP_ITERATE_RGS_START
		if( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) )
		{
			s32AreaPixel = pstDrg->s32AreaPixel;
			s32Del = ( s32AreaPixel < s32ThDrgMinSizePixel );
			if( s32Del )
				IMP_PEA_RGE_RemoveRegion( pstRgs, i, 0 );
		}
		IMP_ITERATE_RGS_END
	}
}

static IMP_VOID ipRemoveSmallRegionsOnLine( PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	PEA_DETECTED_REGION_S *pstDrg;
	IMP_S32 i, s32Cnt;
	IMP_S32 s32Del, s32AreaPixel;
	IMP_S32 s32ThDrgMinSizePixel = pstPara->s32ThDrgMinSizePixelCur;

	IMP_ITERATE_RGS_START
	if( IMP_DRG_IS_LABELED(pstDrg->u8Used) )
	{
		s32AreaPixel = pstDrg->s32AreaPixel;

		s32Del = ( 0
			|| ( s32AreaPixel < s32ThDrgMinSizePixel )
			);

		if( s32Del )
			IMP_PEA_RGE_RemoveRegion( pstRgs, i, 0 );
		else
			IMP_DRG_SET_COMPLETE(pstDrg->u8Used);
	}
	IMP_ITERATE_RGS_END
}

static IMP_VOID ipRemoveSmallRegions( PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_S32 i, s32Cnt;
	IMP_S32 width, height;
	IMP_S32 s32Del, s32AreaPixel;
	IMP_S32 s32ThDrgMinSizeWh = pstPara->s32ThDrgMinSizeWh;
	IMP_S32 s32ThDrgMinSizePixel = pstPara->s32ThDrgMinSizePixelCur;
	IMP_S32 s32ThDrgMaxSizeRatio = pstPara->s32ThDrgMaxSizeRatio;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S32 s32ImgH = pstRgs->pstImgFgRgn->s32H;

	IMP_ITERATE_RGS_START
	if( IMP_DRG_IS_LABELED(pstDrg->u8Used) || IMP_DRG_IS_COMPLETE(pstDrg->u8Used) )
	{
		width = pstDrg->stRect.s16X2-pstDrg->stRect.s16X1+1;
		height = pstDrg->stRect.s16Y2-pstDrg->stRect.s16Y1+1;

		s32AreaPixel = pstDrg->s32AreaPixel;

		s32Del = ( 0
			|| ( width < s32ThDrgMinSizeWh )
			|| ( height < s32ThDrgMinSizeWh )
			|| ( s32AreaPixel < s32ThDrgMinSizePixel )
			|| ( s32AreaPixel * s32ThDrgMaxSizeRatio > s32ImgW * s32ImgH )
			);

		if( s32Del )
			IMP_PEA_RGE_RemoveRegion( pstRgs, i, 0 );
		else
			IMP_DRG_SET_COMPLETE(pstDrg->u8Used);
	}
	IMP_ITERATE_RGS_END
}
