#include "imp_pea_shadow_model.h"
#include "imp_pea_regionset_label.h"


IMP_VOID ipRemoveRegionSetShadow( IpRGEShadowPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IpShadowImage *pstShadow )
{
	IMP_S32 i, s32Cnt;
	PEA_DETECTED_REGION_S *pstDrg;

	IMP_ITERATE_RGS_START
	IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	{
		if( IMP_DRG_IS_DIFF(pstDrg->u8Used) || IMP_DRG_IS_LIGHT(pstDrg->u8Used) )
			continue;

		ipDetectRegionShadow( pstPara, pstRgs, i, pstShadow );
	}
	IMP_ITERATE_RGS_END
}


static IMP_VOID ipDetectRegionShadow( IpRGEShadowPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs,
								      IMP_S32 s32RegionIndex, IpShadowImage *pstShadow )
{
	IMP_S32 i, x, y, s32CntT, s32CntS, s32MaxHeight, s32Index, s32FindInterval, width, height;
	GRAY_IMAGE_S *pstImgFgOrg = pstRgs->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	PEA_DETECTED_REGION_S *pstDrg = pstRgs->astDrg + s32RegionIndex, *pstRgNew;
	IpShadowColumn *pstShadowColumn = pstShadow->pstColumns;
	IpShadowInterval *pstShadowInterval = pstShadow->pstIntervals;
	IMP_U8 *pu8DataFgRgn;
	IMP_S8 *pu8DataFgOrg;
	IMP_S32 s32Sn;
	IMP_U16 u16Status;
	IMP_S32 s32ThShapeHeightRatio, s32ThShadowNumRatio, s32ThShadowHeightRatio, s32ThShadowHeight;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 ys, ye;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;

	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	s32ThShadowNumRatio = pstPara->s32ThShadowNumRatio;
	s32ThShadowHeightRatio = pstPara->s32ThShadowHeightRatio;
	s32ThShapeHeightRatio = pstPara->s32ThShapeHeightRatio;
	width = ru_x - lb_x + 1;
	height = ru_y - lb_y + 1;
	s32Sn = pstDrg->u8Sign;
	u16Status = pstDrg->u16Status;
	s32MaxHeight = 0;
	pstPara->s32ThShadowMaxAreaPixel = 1;

	if( IMP_DRG_IS_SHDRMVD(pstDrg->u8Used)
		||width < pstPara->s32ThShadowMinWidth
		|| height < pstPara->s32ThShadowMinHeight
 		|| pstDrg->s32AreaPixel > pstPara->s32ThShadowMaxAreaPixel
		)
		return;

	ys = lb_y & (~1); ye = ru_y | 1;
	for( x = lb_x; x <= ru_x; x++ )
	{
		pu8DataFgRgn = pstImgFgRgn->pu8Data + s32ImgW * ys + x;
 		pu8DataFgOrg = (IMP_S8*)pstImgFgOrg->pu8Data + s32ImgW * ys + x;

		s32CntS = s32CntT = 0;
		pstShadowColumn[x].s16Bottom = -1;
		pstShadowColumn[x].s16Top = -1;

		for( y = ys; y <= ye; y += 2 )
		{
			IMP_S32 fgr, fgo;
			IMP_S32 fgr2, fgo2;

			fgr = pu8DataFgRgn[0];
			fgo = pu8DataFgOrg[0];
			fgr2 = pu8DataFgRgn[s32ImgW];
			fgo2 = pu8DataFgOrg[s32ImgW];

			if( fgr == s32Sn )
			{
				if( s32CntT == 0 )
					pstShadowColumn[x].s16Bottom = y;
				pstShadowColumn[x].s16Top = y;

				++s32CntT;
 				if( fgo <= 0 )
					++s32CntS;
			}
			if( fgr2 == s32Sn )
			{
				if( s32CntT == 0 )
					pstShadowColumn[x].s16Bottom = y + 1;
				pstShadowColumn[x].s16Top = y + 1;

				++s32CntT;
 				if( fgo2 <= 0 )
					++s32CntS;
			}

			pu8DataFgRgn += (s32ImgW << 1);
 			pu8DataFgOrg += (s32ImgW << 1);
		}
// 		pstShadowColumn[x].fg_num = s32CntT;

		if( s32CntT)
			pstShadowColumn[x].u8IsShadow = ( s32CntS * 100 ) >= ( s32ThShadowNumRatio * s32CntT );
		else
			pstShadowColumn[x].u8IsShadow = 1;

		if( s32MaxHeight<(pstShadowColumn[x].s16Top-pstShadowColumn[x].s16Bottom) )
			s32MaxHeight = (pstShadowColumn[x].s16Top-pstShadowColumn[x].s16Bottom);
	}
	pstShadow->s16MaxHeight = s32MaxHeight;
	s32ThShadowHeight = s32ThShadowHeightRatio * s32MaxHeight;
	if( s32MaxHeight * 100 < height * s32ThShapeHeightRatio )
	{
		IMP_PEA_RGE_RemoveRegion( pstRgs, s32RegionIndex, 0 );
		return;
	}

	s32FindInterval = 0; s32Index = -1;
	pstShadow->s16MaxIntervalIdx = -1;
	for( x = lb_x; x <= ru_x; x++ )
	{
		if( pstShadowColumn[x].u8IsShadow )
		{
			pstShadowColumn[x].u8IsShadow = ( 0
								|| pstShadowColumn[x].s16Top == -1
								|| pstShadowColumn[x].s16Bottom == -1
								|| ( ( pstShadowColumn[x].s16Top - pstShadowColumn[x].s16Bottom + 1) * 100 ) <= s32ThShadowHeight
// 								|| ( ( pstShadowColumn[x].fg_num) * 100 ) <= s32ThShadowHeight
								);
		}

		if( !pstShadowColumn[x].u8IsShadow )
		{
			if( !s32FindInterval )
			{
				s32FindInterval = 1;
				++s32Index;
				pstShadowInterval[s32Index].s16Start = x;
				pstShadowInterval[s32Index].s16End = x;
				pstShadowInterval[s32Index].u8Used = 1;
				pstShadow->s16MaxIntervalIdx = s32Index;
			}
			else
			{
				pstShadowInterval[s32Index].s16End = x;
			}
		}
		else
		{
			s32FindInterval = 0;
		}
	}

	for( s32Index = pstShadow->s16MaxIntervalIdx; s32Index > 0; s32Index-- )
	{
		if( pstShadowInterval[s32Index].s16Start-pstShadowInterval[s32Index - 1].s16End <= pstPara->s32ThShadowMerge )
		{
			pstShadowInterval[s32Index].u8Used = 0;
			if( pstShadow->s16MaxIntervalIdx == s32Index )
				pstShadow->s16MaxIntervalIdx = s32Index - 1;

			for( x=pstShadowInterval[s32Index - 1].s16End + 1; x < pstShadowInterval[s32Index].s16Start; ++x )
				pstShadowColumn[x].u8IsShadow = 0;
			pstShadowInterval[s32Index - 1].s16End = pstShadowInterval[s32Index].s16End;
		}
		else if( (pstShadowInterval[s32Index].s16End - pstShadowInterval[s32Index].s16Start + 1) <= pstPara->s32ThShadowDel )
		{
			pstShadowInterval[s32Index].u8Used = 0;
			if( pstShadow->s16MaxIntervalIdx == s32Index )
				pstShadow->s16MaxIntervalIdx = s32Index - 1;

			for( x = pstShadowInterval[s32Index].s16Start; x <= pstShadowInterval[s32Index].s16End; ++x )
				pstShadowColumn[x].u8IsShadow = 1;
		}
	}
	if( (pstShadowInterval[0].s16End - pstShadowInterval[0].s16Start + 1) <= pstPara->s32ThShadowDel )
	{
		pstShadowInterval[0].u8Used = 0;
		if( pstShadow->s16MaxIntervalIdx == 0 )
			pstShadow->s16MaxIntervalIdx = -1;

		for( x = pstShadowInterval[0].s16Start; x <= pstShadowInterval[0].s16End; ++x )
			pstShadowColumn[x].u8IsShadow = 1;
	}

	for( x = lb_x; x <= ru_x; ++x )
	{
		if( !pstShadowColumn[x].u8IsShadow )
			continue;

		pu8DataFgRgn = pstImgFgRgn->pu8Data + s32ImgW * lb_y + x;
		for( y = lb_y; y <= ru_y; ++y )
		{
			if( pu8DataFgRgn[0] == s32Sn )
				pu8DataFgRgn[0] = 0;
			pu8DataFgRgn += s32ImgW;
		}
	}

	ipDetectedRegionSetFree( pstRgs, s32RegionIndex );
	for( s32Index = 0; s32Index <= pstShadow->s16MaxIntervalIdx; ++s32Index )
	{
		if( !pstShadowInterval[s32Index].u8Used )
			continue;

		pstRgNew = NULL;
		i = ipDetectedRegionSetAlloc( pstRgs );
		if( !i ) break;
		pstRgNew = &pstRgs->astDrg[i];

		IMP_DRG_SET_COMPLETE(pstRgNew->u8Used);
		IMP_DRG_SET_SHDRMVD(pstRgNew->u8Used);
		pstRgNew->u8Sign = s32Sn;
		pstRgNew->u16Status = u16Status;

		pstRgNew->stRect.s16Y1 = pstRgNew->stRect.s16X1 = 10000;
		pstRgNew->stRect.s16Y2 = pstRgNew->stRect.s16X2= -1;
		pstRgNew->stRect.s16X1 = pstShadowInterval[s32Index].s16Start;
		pstRgNew->stRect.s16X2= pstShadowInterval[s32Index].s16End;
		pstRgNew->s32AreaPixel = 0;
		for( x = pstShadowInterval[s32Index].s16Start; x <= pstShadowInterval[s32Index].s16End; x++ )
		{
			if( pstShadowColumn[x].s16Bottom == -1 || pstShadowColumn[x].s16Top == -1 )
				continue;

			if( pstRgNew->stRect.s16Y1 > pstShadowColumn[x].s16Bottom )
				pstRgNew->stRect.s16Y1 = pstShadowColumn[x].s16Bottom;
			if( pstRgNew->stRect.s16Y2 < pstShadowColumn[x].s16Top )
				pstRgNew->stRect.s16Y2 = pstShadowColumn[x].s16Top;
			pstRgNew->s32AreaPixel += ( pstShadowColumn[x].s16Top - pstShadowColumn[x].s16Bottom + 1 );
		}
	}
}


IMP_S32 IMP_GetMemSizeShadowImage(IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Size = 0;
	
	s32Size += s32Width * sizeof(IpShadowInterval);
	s32Size += s32Width * sizeof(IpShadowColumn);

	return s32Size;
}


IMP_VOID ipShadowImageCreate( IpShadowImage *pstShadow, IMP_S32 s32Width, IMP_S32 s32Height, MEM_MGR_ARRAY_S *pstMemMgr )
{
	memset( pstShadow, 0, sizeof(IpShadowImage) );
	pstShadow->s32Width = s32Width;
	pstShadow->s32Height = s32Height;
	pstShadow->pstIntervals = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, s32Width*sizeof(IpShadowInterval) );
	pstShadow->pstColumns = IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, s32Width*sizeof(IpShadowColumn) );
}
IMP_VOID ipShadowImageDestroy( IpShadowImage *pstShadow, MEM_MGR_ARRAY_S *pstMemMgr )
{
	IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstShadow->pstColumns );
	IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstShadow->pstIntervals );
	memset( pstShadow, 0, sizeof(IpShadowImage) );
}
IMP_VOID ipShadowImageClear( IpShadowImage *pstShadow )
{
	memset( pstShadow->pstColumns, 0, pstShadow->s32Width * sizeof(IpShadowColumn) );
	memset( pstShadow->pstIntervals, 0, pstShadow->s32Width * sizeof(IpShadowInterval) );
}
