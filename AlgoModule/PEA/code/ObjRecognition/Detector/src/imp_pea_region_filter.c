#include "imp_pea_region_filter.h"
#include "imp_pea_regionset_label.h"


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 去除目标阴影
IMP_VOID ipRegionSetFilterDetect( IpRGEFilterPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, s32Cnt;
	PEA_DETECTED_REGION_S *pstDrg;

	IMP_ITERATE_RGS_START
	IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	{
		ipRegionFilterDetect( pstPara, pstRgs, i );
	}
	IMP_ITERATE_RGS_END
}

static IMP_VOID ipRegionFilterDetect( IpRGEFilterPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32RegionIndex )
{
	IMP_S32 i, x, y, s32CntT, s32CntS, s32MaxHeight, s32Index, s32FindInterval, width, height;
	GRAY_IMAGE_S *pstImgFgOrg = pstRgs->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgEdgeDiff = pstRgs->pstImgContour;
	PEA_DETECTED_REGION_S *pstDrg = pstRgs->astDrg + s32RegionIndex, *pstRgNew;
	IMP_U8 *pu8DataFgRgn;
	IMP_S8 *pu8DataFgOrg;
	IMP_S8 *pu8DataEdgeD;
	
	IMP_S32 s32Sn;
	IMP_U16 u16Status;	
	IMP_S32 s32ThRgeFilterrMinEdge, s32ThRgeFilterMinWidth, s32ThRgeFilterMinHeight, s32ThRgeFilterMinAreaPixel;		
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 lb_x2 = 10000, lb_y2 = 10000, ru_x2 = -1, ru_y2 = -1;
	IMP_S32 ys, ye;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S32 s32EdgeNum = 0;
	IMP_S32 s32AreaPixel = 0;
	IMP_S32 first = 0;
	lb_x = pstDrg->stRect.s16X1;
	ru_x = pstDrg->stRect.s16X2;
	lb_y = pstDrg->stRect.s16Y1;
	ru_y = pstDrg->stRect.s16Y2;
	
	s32ThRgeFilterrMinEdge = pstPara->s32ThRgeFilterrMinEdge;
	s32ThRgeFilterMinWidth = pstPara->s32ThRgeFilterMinWidth;
	s32ThRgeFilterMinHeight = pstPara->s32ThRgeFilterMinHeight;
	s32ThRgeFilterMinAreaPixel = pstPara->s32ThRgeFilterMinAreaPixel;
	
	s32ThRgeFilterrMinEdge = 1;
	s32ThRgeFilterMinWidth = 1;
	s32ThRgeFilterMinHeight = 1;
	s32ThRgeFilterMinAreaPixel = 1;

	width = ru_x - lb_x + 1;
	height = ru_y - lb_y + 1;
	s32Sn = pstDrg->u8Sign;
	u16Status = pstDrg->u16Status;
	s32MaxHeight = 0;

	if( 0
		|| width < s32ThRgeFilterMinWidth
		|| height < s32ThRgeFilterMinHeight
 		|| pstDrg->s32AreaPixel < s32ThRgeFilterMinAreaPixel
		)
		return;

	ys = lb_y & (~1); ye = ru_y | 1;

	for( x = lb_x; x <= ru_x; x++ )
	{
		pu8DataFgRgn = pstImgFgRgn->pu8Data + s32ImgW * ys + x;
 		pu8DataFgOrg = (IMP_S8*)pstImgFgOrg->pu8Data + s32ImgW * ys + x;
		pu8DataEdgeD = (IMP_S8*)pstImgEdgeDiff->pu8Data + s32ImgW * ys + x;

		for( y = ys; y <= ye; y ++ )
		{
			IMP_S32 fgr, fgo, edgeD;

			fgr = pu8DataFgRgn[0];
			fgo = pu8DataFgOrg[0];
			edgeD = pu8DataEdgeD[0];
			if( fgr == s32Sn && abs(edgeD) >0  )
			{
				if (lb_x2 > x)
				{
					lb_x2 = x;
				}
				if (lb_y2 > y)
				{
					lb_y2 = y;
				}
				if (ru_x2 < x)
				{
					ru_x2 = x;
				}
				if (ru_y2 < y)
				{
					ru_y2 = y;
				}
				++s32EdgeNum;
			}

			pu8DataFgRgn += (s32ImgW );
 			pu8DataFgOrg += (s32ImgW );
			pu8DataEdgeD += (s32ImgW );
		}
	}
	if (s32EdgeNum < 1)
	{
		return;
	}
	if( s32EdgeNum * 100 < height * width *  s32ThRgeFilterrMinEdge)
	{
		IMP_PEA_RGE_RemoveRegion( pstRgs, s32RegionIndex, 0 );
		return;
	}

	for( x = lb_x; x <= ru_x; ++x )
	{
		pu8DataFgRgn = pstImgFgRgn->pu8Data + s32ImgW * lb_y + x;
		for( y = lb_y; y <= ru_y; ++y )
		{
			if( pu8DataFgRgn[0] == s32Sn && ( x < lb_x2 || x > ru_x2 ) || ( y < lb_y2 || y > ru_y2 ))
			{
				pu8DataFgRgn[0] = 0;
			}
			else
			{
			//	pu8DataFgRgn[0] = s32Sn;
				s32AreaPixel++;
			}
			pu8DataFgRgn += s32ImgW;
		}
	}

	ipDetectedRegionSetFree( pstRgs, s32RegionIndex );
	{
		pstRgNew = NULL;
		i = ipDetectedRegionSetAlloc( pstRgs );
		pstRgNew = &pstRgs->astDrg[i];

		IMP_DRG_SET_COMPLETE(pstRgNew->u8Used);
		//IMP_DRG_SET_LABELED(pstRgNew->u8Used);
		pstRgNew->u8Sign = s32Sn;
		pstRgNew->u16Status = u16Status;
		
		pstRgNew->stRect.s16X1 = lb_x2;
		pstRgNew->stRect.s16Y1 = lb_y2; 
		pstRgNew->stRect.s16X2 = ru_x2;
		pstRgNew->stRect.s16Y2 = ru_y2;
		pstRgNew->s32AreaPixel = s32AreaPixel;
	}
}

// 区域过滤
//////////////////////////////////////////////////////////////////////////