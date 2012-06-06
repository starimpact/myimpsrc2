#include "imp_pea_regionset_rmabnml.h"
#include "imp_pea_regionset_label.h"

IMP_VOID IMP_PEA_RGE_RemoveAbnormalRegions( PEA_RGE_REMOVE_ABNORMAL_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, s32Cnt = 0, s32Del;
	IMP_S32 s32Width, s32Height;
	IMP_S32 s32AreaPixel;
	PEA_DETECTED_REGION_S *pstDrg;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S32 s32ImgH = pstRgs->pstImgFgRgn->s32H;
	IMP_S32 s32ThDrgMinSizeWh = pstPara->s32ThDrgMinSizeWh;
	IMP_S32 s32ThDrgMinSizePixel = pstPara->s32ThDrgMinSizePixel;
	IMP_S32 s32ThDrgMaxSizeRatio = pstPara->s32ThDrgMaxSizeRatio;
	IMP_S32 s32ThDrgFillRatio = pstPara->s32ThDrgFillRatio;
	IMP_S32 s32ThDrgAspectRatio = pstPara->s32ThDrgAspectRatio;

	IMP_ITERATE_RGS_START
		IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	IMP_ASSERT( IMP_DRG_IS_LIGHT(pstDrg->u8Used) || IMP_DRG_IS_DIFF(pstDrg->u8Used) || IMP_DRG_IS_MOTION(pstDrg->u8Used) || IMP_DRG_IS_STATIC_L(pstDrg->u8Used) || IMP_DRG_IS_STATIC_R(pstDrg->u8Used) );
	{
		s32AreaPixel = pstDrg->s32AreaPixel;
		s32Width = pstDrg->stRect.s16X2 - pstDrg->stRect.s16X1 + 1;
		s32Height = pstDrg->stRect.s16Y2 - pstDrg->stRect.s16Y1 + 1;

		s32Del = ( 0
			|| ( s32Width < s32ThDrgMinSizeWh )
			|| ( s32Height < s32ThDrgMinSizeWh )
			|| ( s32AreaPixel < s32ThDrgMinSizePixel )
			|| ( s32AreaPixel * s32ThDrgMaxSizeRatio > s32ImgW * s32ImgH )
			|| ( !IMP_DRG_IS_DIFF(pstDrg->u8Used) && ( s32AreaPixel * 100 < s32ThDrgFillRatio * (s32Width * s32Height) ) )
			|| ( s32Height > s32ThDrgAspectRatio * s32Width )
			|| ( s32Width > s32ThDrgAspectRatio * s32Height )
		
			);
		if( s32Del )
			IMP_PEA_RGE_RemoveRegion( pstRgs, i, 0 );
		else
		{
			if( IMP_DRG_IS_DIFF(pstDrg->u8Used) || IMP_DRG_IS_MOTION(pstDrg->u8Used) )
				IMP_SET_RGN_RMOTION( pstDrg->u16Status);
			else if( IMP_DRG_IS_STATIC_L(pstDrg->u8Used) )
				IMP_SET_RGN_RSTATIC_L( pstDrg->u16Status);
			else if( IMP_DRG_IS_STATIC_R(pstDrg->u8Used) )
				IMP_SET_RGN_RSTATIC_R( pstDrg->u16Status);
			else if( IMP_DRG_IS_LIGHT(pstDrg->u8Used) )
				IMP_SET_RGN_RLIGHT( pstDrg->u16Status);
			else
				pstDrg->u16Status = 0;
		}
	}
	IMP_ITERATE_RGS_END

}

