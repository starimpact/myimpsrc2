
#ifndef _IMP_PEA_REGIONSET_SUPPRESS_FG_H_
#define _IMP_PEA_REGIONSET_SUPPRESS_FG_H_

#include "imp_pea_common.h"


typedef struct impPEA_RGE_SUPPRESS_FG_PARA_S
{
	IMP_S32 s32Enable;

	IMP_S32 s32MinPixelArea;
	IMP_S32 s32MaxPixelArea;

	IMP_S32 s32LightEnable;
	IMP_S32 s32MinLightArea;
	IMP_S32 s32MaxLightArea ;
	IMP_S32 s32LightEdgeShapeRatio;
	IMP_S32 s32LightEdgeDistRatio;
	IMP_S32 s32LightEdgeLikehoodRatioFg;
	IMP_S32 s32LightEdgeLikehoodRatioBg;
	IMP_S32 s32LightUseDiff;

	IMP_S32 s32PenumbraEnable;
	IMP_S32 s32MinPenuArea;
	IMP_S32 s32MaxPenuArea;
	IMP_S32 s32PenuDiffRatio;
	IMP_S32 s32PenuDiffMax;
	IMP_S32 s32PenuThreshold;

	IMP_S32 s32MagThreshold;
	IMP_S32 s32MinMagDiffNum;
	IMP_S32 s32MinMagDiffRatio;

} PEA_RGE_SUPPRESS_FG_PARA_S;


IMP_S32 ipCalcRegionEdgeLikehood( GRAY_IMAGE_S *pstImage1, GRAY_IMAGE_S *pstImage2,
					GRAY_IMAGE_S *pstImageFg, IMP_RECT_S *pstRect, IMP_S32 s32Sn, IMP_S32 s32Flag,
					IMP_S32 s32EdgeShapeRatio, IMP_S32 s32EdgeDistRation,
					IMP_S32 s32EdgeLikeRatio1, IMP_S32 s32EdgeLikeRatio2 );
IMP_EXPORTS IMP_VOID ipReCalcDregionRect( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32RgIndex, IMP_S32 s32ThDiff );

IMP_VOID ipCalcRegionGrayDiff( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index,
								 IMP_S32 *ps32DiffB, IMP_S32 *ps32CntB,
								 IMP_S32 *ps32DiffF, IMP_S32 *ps32CntF );

IMP_EXPORTS IMP_VOID ipGetMagDiffImage(	PEA_RESULT_S *pstResult,PEA_RGE_SUPPRESS_FG_PARA_S *pstParam);


IMP_EXPORTS IMP_VOID ipSuppressRegionForeground( PEA_RGE_SUPPRESS_FG_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );
IMP_EXPORTS IMP_VOID ipSuppressRegionSetForeground( PEA_RGE_SUPPRESS_FG_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );
IMP_EXPORTS IMP_VOID ipRemoveRegionSetLight(PEA_RESULT_S *pstResult, PEA_RGE_SUPPRESS_FG_PARA_S *pstParam, PEA_DETECTED_REGIONSET_S *pstRgs);

#endif
