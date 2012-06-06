#include "imp_pea_regionset_merge.h"
#include "imp_pea_regionset_label.h"


static IMP_VOID ipMergeTwoRegions( PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2 );
static IMP_S32 ipIsInDregion( PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2 );
static IMP_S32 ipIsDregionNeighbor( PEA_MERGE_NEIGHBOR_PARA_S *mpara, PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2 );
static IMP_S32 ipDrgsIsParallelVert( PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2, IMP_S32 th_paleldis );
static IMP_VOID ipMergeParallelDrgsVert(PEA_DETECTED_REGIONSET_S *rgs, IMP_S32 idx1, IMP_S32 idx2 );



IMP_VOID IMP_PEA_RGE_MergeRegionSetInner( PEA_RGE_MERGE_INNER_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, j;
	PEA_DETECTED_REGION_S *pstDrg1, *pstDrg2;
	IMP_S32 s32InnerLight = pstPara->s32InnerLight;

	for( i = 1; i < IMP_MAX_TGT_CNT - 1; i++ )
	{
		pstDrg1 = pstRgs->astDrg + i;
		if( !IMP_DRG_IS_USED(pstDrg1->u8Used) || ( s32InnerLight && !IMP_DRG_IS_LIGHT(pstDrg1->u8Used) ) )
			continue;

		for( j = i + 1; j < IMP_MAX_TGT_CNT - 1; j++ )
		{
			pstDrg2 = pstRgs->astDrg + j;
			if( !IMP_DRG_IS_USED(pstDrg2->u8Used) || ( s32InnerLight && !IMP_DRG_IS_LIGHT(pstDrg2->u8Used) ) )
				continue;

			if( ipIsInDregion( pstDrg2, pstDrg1 ) )
			{
				ipMergeTwoRegions( pstDrg1, pstDrg2 );
				IMP_PEA_RGE_RemoveRegion( pstRgs, j, pstDrg1->u8Sign );
				continue;
			}
			else if( ipIsInDregion( pstDrg1, pstDrg2 ) )
			{
				ipMergeTwoRegions( pstDrg2, pstDrg1 );
				IMP_PEA_RGE_RemoveRegion( pstRgs, i, pstDrg2->u8Sign );
				break;
			}
		}
	}
}

IMP_VOID IMP_PEA_RGE_MergeRegionSetNeighbor( PEA_MERGE_NEIGHBOR_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, j;
	PEA_DETECTED_REGION_S *pstDrg1, *pstDrg2;
	IMP_S32 s32NeighborDiff = pstPara->s32NeighborDiff;

	for( i = 1; i < IMP_MAX_TGT_CNT - 1; i++ )
	{
		pstDrg1 = pstRgs->astDrg + i;
		if( !IMP_DRG_IS_USED(pstDrg1->u8Used) || ( !IMP_DRG_IS_DIFF(pstDrg1->u8Used) && s32NeighborDiff ) )
			continue;

		for( j = i + 1; j < IMP_MAX_TGT_CNT - 1; j++ )
		{
			pstDrg2 = pstRgs->astDrg + j;
			if( !IMP_DRG_IS_USED(pstDrg2->u8Used) || ( !IMP_DRG_IS_DIFF(pstDrg2->u8Used) && s32NeighborDiff ) )
				continue;

			if( ipIsDregionNeighbor( pstPara, pstDrg2, pstDrg1 ) )
			{
				if( pstDrg1->s32AreaPixel >= pstDrg2->s32AreaPixel )
				{
					ipMergeTwoRegions( pstDrg1, pstDrg2 );
					IMP_PEA_RGE_RemoveRegion( pstRgs, j, pstDrg1->u8Sign );
					continue;
				}
				else
				{
					ipMergeTwoRegions( pstDrg2, pstDrg1 );
					IMP_PEA_RGE_RemoveRegion( pstRgs, i, pstDrg2->u8Sign );
					break;
				}
			}
		}
	}
}

IMP_VOID IMP_PEA_RGE_MergeRegionSetParallel( PEA_RGE_MERGE_PARALLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, j;
	PEA_DETECTED_REGION_S *pstDrg1, *pstDrg2;
	IMP_S32 s32ThMergePaleDist = pstPara->s32ThMergeParalleDist;

	for( i = 1; i < IMP_MAX_TGT_CNT - 1; i++ )
	{
		pstDrg1 = pstRgs->astDrg + i;
		if( !IMP_DRG_IS_USED(pstDrg1->u8Used) )
			continue;

		for( j = i + 1; j < IMP_MAX_TGT_CNT - 1; j++ )
		{
			pstDrg2 = pstRgs->astDrg + j;
			if( !IMP_DRG_IS_USED(pstDrg2->u8Used) )
				continue;

			if( ipDrgsIsParallelVert( pstDrg1, pstDrg2, s32ThMergePaleDist ) )
			{
				if( pstDrg1->s32AreaPixel >= pstDrg2->s32AreaPixel )
				{
					ipMergeParallelDrgsVert( pstRgs, i, j );
					continue;
				}
				else
				{
					ipMergeParallelDrgsVert( pstRgs, j, i );
					break;
				}
			}
		}
	}
}

IMP_VOID IMP_PEA_RGE_MergeRegionSet( PEA_RGE_MERGE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	if( pstPara->s32MergeInner )
		IMP_PEA_RGE_MergeRegionSetInner( &pstPara->stInnerPara, pstRgs );

	if( pstPara->s32MergeNeighbor )
		IMP_PEA_RGE_MergeRegionSetNeighbor( &pstPara->stNeighborPara, pstRgs );

	if( pstPara->s32MergeParallel )
		IMP_PEA_RGE_MergeRegionSetParallel( &pstPara->stParallelPara, pstRgs );
}

IMP_VOID IMP_PEA_RGE_MergeRegionSet_Static( PEA_RGE_MERGE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	pstPara->stInnerPara.s32InnerLight = IMP_FALSE;
	//if( pstPara->s32MergeInner )
		IMP_PEA_RGE_MergeRegionSetInner( &pstPara->stInnerPara, pstRgs );

	//if( pstPara->s32MergeNeighbor )
		IMP_PEA_RGE_MergeRegionSetNeighbor( &pstPara->stNeighborPara, pstRgs );

	//if( pstPara->s32MergeParallel )
		IMP_PEA_RGE_MergeRegionSetParallel( &pstPara->stParallelPara, pstRgs );
}

static IMP_VOID ipMergeTwoRegions( PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2 )
{
	pstDrg1->stRect.s16X1 = pstDrg1->stRect.s16X1<=pstDrg2->stRect.s16X1 ? pstDrg1->stRect.s16X1 : pstDrg2->stRect.s16X1;
	pstDrg1->stRect.s16X2 = pstDrg1->stRect.s16X2>=pstDrg2->stRect.s16X2 ? pstDrg1->stRect.s16X2 : pstDrg2->stRect.s16X2;
	pstDrg1->stRect.s16Y1 = pstDrg1->stRect.s16Y1<=pstDrg2->stRect.s16Y1 ? pstDrg1->stRect.s16Y1 : pstDrg2->stRect.s16Y1;
	pstDrg1->stRect.s16Y2 = pstDrg1->stRect.s16Y2>=pstDrg2->stRect.s16Y2 ? pstDrg1->stRect.s16Y2 : pstDrg2->stRect.s16Y2;
	pstDrg1->s32AreaPixel += pstDrg2->s32AreaPixel;
}

static IMP_S32 ipIsInDregion( PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2 )
{
	return ( 1
			&& pstDrg1->stRect.s16X1 >= pstDrg2->stRect.s16X1
			&& pstDrg1->stRect.s16X2 <= pstDrg2->stRect.s16X2
			&& pstDrg1->stRect.s16Y1 >= pstDrg2->stRect.s16Y1
			&& pstDrg1->stRect.s16Y2 <= pstDrg2->stRect.s16Y2
			);
}

static IMP_S32 ipIsDregionNeighbor( PEA_MERGE_NEIGHBOR_PARA_S *mpara, PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2 )
{
	IMP_S32 cx1, cy1, w1, h1;
	IMP_S32 cx2, cy2, w2, h2;
	IMP_S32 s32Cond1, s32Cond2;

	cx1 = ( pstDrg1->stRect.s16X1 + pstDrg1->stRect.s16X2 ) >> 1;
	cy1 = ( pstDrg1->stRect.s16Y1 + pstDrg1->stRect.s16Y2 ) >> 1;
	w1 = pstDrg1->stRect.s16X2 - pstDrg1->stRect.s16X1 + 1;
	h1 = pstDrg1->stRect.s16Y2 - pstDrg1->stRect.s16Y1 + 1;

	cx2 = ( pstDrg2->stRect.s16X1 + pstDrg2->stRect.s16X2 ) >> 1;
	cy2 = ( pstDrg2->stRect.s16Y1 + pstDrg2->stRect.s16Y2 ) >> 1;
	w2 = pstDrg2->stRect.s16X2 - pstDrg2->stRect.s16X1 + 1;
	h2 = pstDrg2->stRect.s16Y2 - pstDrg2->stRect.s16Y1 + 1;

	s32Cond1 = ( abs(cx1 - cx2) * 100 <= (w1 + w2) * mpara->s32ThNeighborRatio );
	s32Cond2 = ( abs(cy1 - cy2) * 100 <= (h1 + h2) * mpara->s32ThNeighborRatio );

	return ( s32Cond1 && s32Cond2 );
}

static IMP_S32 ipDrgsIsParallelVert( PEA_DETECTED_REGION_S *pstDrg1, PEA_DETECTED_REGION_S *pstDrg2, IMP_S32 th_paleldis )
{
	IMP_S32 cx1, cy1, w1, h1;
	IMP_S32 cx2, cy2, w2, h2;
	IMP_S32 s32Cond1, s32Cond2, s32Cond3;

	cx1 = ( pstDrg1->stRect.s16X1 + pstDrg1->stRect.s16X2 ) >> 1;
	cy1 = ( pstDrg1->stRect.s16Y1 + pstDrg1->stRect.s16Y2 ) >> 1;
	w1 = pstDrg1->stRect.s16X2 - pstDrg1->stRect.s16X1 + 1;
	h1 = pstDrg1->stRect.s16Y2 - pstDrg1->stRect.s16Y1 + 1;

	cx2 = ( pstDrg2->stRect.s16X1 + pstDrg2->stRect.s16X2 ) >> 1;
	cy2 = ( pstDrg2->stRect.s16Y1 + pstDrg2->stRect.s16Y2 ) >> 1;
	w2 = pstDrg2->stRect.s16X2 - pstDrg2->stRect.s16X1 + 1;
	h2 = pstDrg2->stRect.s16Y2 - pstDrg2->stRect.s16Y1 + 1;

	s32Cond1 = ( w1 == w2 )
		||	( w1 > w2 && w1 <= w2 * 2 )
		||	( w2 > w1 && w2 <= w1 * 2 );

	s32Cond2 = ( cx1 >= pstDrg2->stRect.s16X1 && cx1 <= pstDrg2->stRect.s16X2 )
		||	( cx2 >= pstDrg1->stRect.s16X1 && cx2 <= pstDrg1->stRect.s16X2 );

	s32Cond3 = ( ( ((h1 + h2) >> 1) + th_paleldis ) >= abs(cy1 - cy2) );

	return ( s32Cond1 && s32Cond2 && s32Cond3 );
}

static IMP_VOID ipMergeParallelDrgsVert( PEA_DETECTED_REGIONSET_S *rgs, IMP_S32 idx1, IMP_S32 idx2 )
{
	IMP_S32 cx1, cy1;
	IMP_S32 cx2, cy2;
	IMP_S32 s32Tmp, x, y;
	IMP_U8 *pu8Data, u8Sn;
	PEA_DETECTED_REGION_S *pstDrg1, *pstDrg2;
	IMP_S32 s32ImgW = rgs->pstImgFgRgn->s32W;
	GRAY_IMAGE_S *pstImgFgRgn = rgs->pstImgFgRgn;

	pstDrg1 = rgs->astDrg + idx1;
	pstDrg2 = rgs->astDrg + idx2;

	cx1 = ( pstDrg1->stRect.s16X1 + pstDrg1->stRect.s16X2 ) >> 1;
	cy1 = ( pstDrg1->stRect.s16Y1 + pstDrg1->stRect.s16Y2 ) >> 1;
	cx2 = ( pstDrg2->stRect.s16X1 + pstDrg2->stRect.s16X2 ) >> 1;
	cy2 = ( pstDrg2->stRect.s16Y1 + pstDrg2->stRect.s16Y2 ) >> 1;

	if( cx1 > cx2 )
	{
		s32Tmp = cx1; cx1 = cx2; cx2 = s32Tmp;
	}
	if( cy1 > cy2 )
	{
		s32Tmp = cy1; cy1 = cy2; cy2 = s32Tmp;
	}
	u8Sn = pstDrg1->u8Sign;
	for( y = cy1; y <= cy2; y++ )
	{
		pu8Data = pstImgFgRgn->pu8Data + y*s32ImgW;
		for( x = cx1; x <= cx2; x++ )
		{
			pu8Data[x] = u8Sn;
		}
	}

	ipMergeTwoRegions( pstDrg1, pstDrg2 );

	IMP_PEA_RGE_RemoveRegion( rgs, idx2, pstDrg1->u8Sign );
}

