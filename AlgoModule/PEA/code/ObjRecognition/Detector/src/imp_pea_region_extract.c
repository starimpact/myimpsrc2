
#include "imp_pea_region_extract.h"


static IMP_VOID ipClearRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	IMP_PEA_RGEDataClear( &pModule->stData );

	ipShadowImageClear( &pModule->stShadowImage );
}


IMP_S32 IMP_GetMemSizeRegionExtract(IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Size = 0;
	
	s32Size += IMP_GetMemSizeShadowImage(s32Width, s32Height);
	
	return s32Size;
}


IMP_VOID ipCreateRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule, PEA_DETECTOR_DATA_S *detdata, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	IMP_S32 s32Height, s32Width;
	MEM_MGR_ARRAY_S *pstMemMgr;

	memset( pModule, 0, sizeof(PEA_REGION_EXTRACT_MODULE_S) );
	pModule->pstHwResource = pstHwResource;
	pModule->pstResult = pstResult;
	pModule->stData.pstDtData = detdata;

	pstMemMgr = &pstHwResource->stMemMgr;
	s32Height = pstResult->s32Height;
	s32Width = pstResult->s32Width;

	ipShadowImageCreate( &pModule->stShadowImage, s32Width, s32Height, pstMemMgr );

	ipClearRegionExtract( pModule );
}
IMP_VOID ipReleaseRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &pModule->pstHwResource->stMemMgr;

	ipShadowImageDestroy( &pModule->stShadowImage, pstMemMgr );

	memset( pModule, 0, sizeof(PEA_REGION_EXTRACT_MODULE_S) );
}
IMP_VOID ipConfigRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule, PEA_REGION_EXTRACT_PARA_S *pstPara )
{
	memcpy( &pModule->stPara, pstPara, sizeof(PEA_REGION_EXTRACT_PARA_S) );
}
static IMP_VOID ipPrepareRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule );
static IMP_VOID ipRegionExtractOnMotion( PEA_REGION_EXTRACT_MODULE_S *pModule );

#ifdef SUPPORT_CHANGE
static IMP_VOID ipCalcDRegionChange( PEA_REGION_EXTRACT_MODULE_S *pModule );
#endif
static IMP_VOID ipCalcDRegionOnOsc( PEA_REGION_EXTRACT_MODULE_S *pModule );

IMP_S32 ipProcessRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	PEA_RESULT_S *pstResult = pModule->pstResult;
	PEA_STATUS_S *pstStatus = &pstResult->stSysStatus;

	ipPrepareRegionExtract( pModule );
	
//	if (pstResult->s32ModuleSwitch & 1)
	{
		ipRegionExtractOnMotion( pModule );
	}

#ifdef SUPPORT_CHANGE
//	if( ISTAT_BG_IS_UPDATED_CHG(pstStatus) )
		ipCalcDRegionChange( pModule );
#endif

	return 0; 
}

#ifdef SUPPORT_CHANGE
IMP_VOID ipPostProcessRegionExtractChange( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	PEA_REGION_EXTRACT_PARA_S *pstPara = &pModule->stPara;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	IMP_S32 s32Width = pstResult->s32Width;
	IMP_S32 s32Height = pstResult->s32Height;
	PEA_DETECTED_REGIONSET_S *rgs_chg = &pstResult->stDRegionSetChg;
	PEA_DETECTED_REGIONSET_S *pstRegions = &pstResult->stDRegionSetChg;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pstResult->stDRegionSet;
	PEA_DETECTED_REGION_S	*pstDrg, *drg_chg;

	IMP_S32 i, j, flag;

	for( i = 1; i < IMP_MAX_TGT_CNT - 1; i++ )
	{
		flag = 1;
		drg_chg = &rgs_chg->astDrg[i];
		if( IMP_DRG_IS_USED(drg_chg->u8Used) )
		{
			for( j = 1; j < IMP_MAX_TGT_CNT - 1; j++ )
			{
				pstDrg = &pstRgs->astDrg[j];
				if( IMP_DRG_IS_USED(pstDrg->u8Used) )
				{

					if(
						(drg_chg->stRect.s16X1 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X1 <= pstDrg->stRect.s16X2
					       && drg_chg->stRect.s16Y1 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y1 <= pstDrg->stRect.s16Y2
						||    drg_chg->stRect.s16X2 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X2 <= pstDrg->stRect.s16X2
						   && drg_chg->stRect.s16Y1 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y1 <= pstDrg->stRect.s16Y2
						||    drg_chg->stRect.s16X1 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X1 <= pstDrg->stRect.s16X2
						   && drg_chg->stRect.s16Y2 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y2 <= pstDrg->stRect.s16Y2
					    ||    drg_chg->stRect.s16X2 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X2 <= pstDrg->stRect.s16X2
						   && drg_chg->stRect.s16Y2 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y2 <= pstDrg->stRect.s16Y2
						 )
						   && drg_chg->s32AreaPixel * 2 >= pstDrg->s32AreaPixel
						   )
					{
						drg_chg->stRect = pstDrg->stRect;
						drg_chg->s32AreaActual = pstDrg->s32AreaActual;
						drg_chg->s32AreaPixel = pstDrg->s32AreaPixel;
						flag = 0;
						break;
					}
				}
			}

		}
	}


	for( i = 1; i < IMP_MAX_TGT_CNT - 1; i++ )
	{
		drg_chg = &rgs_chg->astDrg[i];
		if( IMP_DRG_IS_USED(drg_chg->u8Used) )
		{
			for( j = 1; j < IMP_MAX_TGT_CNT - 1; j++ )
			{
				pstDrg = &rgs_chg->astDrg[j];
				if( IMP_DRG_IS_USED(pstDrg->u8Used) )
				{
					if (i != j)
					{
						if(    drg_chg->stRect.s16X1 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X1 <= pstDrg->stRect.s16X2
							&& drg_chg->stRect.s16Y1 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y1 <= pstDrg->stRect.s16Y2
							|| drg_chg->stRect.s16X2 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X2 <= pstDrg->stRect.s16X2
							&& drg_chg->stRect.s16Y1 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y1 <= pstDrg->stRect.s16Y2
							|| drg_chg->stRect.s16X1 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X1 <= pstDrg->stRect.s16X2
							&& drg_chg->stRect.s16Y2 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y2 <= pstDrg->stRect.s16Y2
							|| drg_chg->stRect.s16X2 >= pstDrg->stRect.s16X1 && drg_chg->stRect.s16X2 <= pstDrg->stRect.s16X2
							&& drg_chg->stRect.s16Y2 >= pstDrg->stRect.s16Y1 && drg_chg->stRect.s16Y2 <= pstDrg->stRect.s16Y2
							)
						{
							IMP_DRG_SET_UNUSED(pstDrg->u8Used);
						}

					}
				}
			}
		}
	}


}
#endif
IMP_VOID ipPostprocessRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule )
{

}

static IMP_VOID ipRegionFillHole(PEA_REGION_EXTRACT_MODULE_S *pModule);
static IMP_VOID ipPrepareRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	PEA_REGION_EXTRACT_PARA_S *pstPara = &pModule->stPara;
	PEA_REGION_EXTRACT_DATA_S *pstData = &pModule->stData;

	pstPara->stSuppressFgPara.s32PenuThreshold = pstData->pstDtData->s32Threshold = 12;
}
static IMP_VOID ipRegionFillHole(PEA_REGION_EXTRACT_MODULE_S *pModule)
{
	PEA_RESULT_S *pstResult = pModule->pstResult;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pstResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgFgOrg = pstRgs->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	GRAY_IMAGE_S *pstImgMediate = pstRgs->pstImgMediate;
	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_S32  s32Cnt = 0;
	IMP_S16 x, y, w, h, sn, i, rl_s, rl_e, x1;
	IMP_S16 lb_x, lb_y, ru_x, ru_y;
	IMP_U8  *pu8DatCtr, *pu8DatPre, *pu8DatCur;

	//sn = pstDrg->sn;
	w = pstImgFgOrg->s32W; h = pstImgFgOrg->s32H;
	IMP_ITERATE_RGS_START
		if( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) )
		{
			lb_x = pstDrg->stRect.s16X1 - 2; lb_y = pstDrg->stRect.s16Y1 - 2;
			ru_x = pstDrg->stRect.s16X2 + 2; ru_y = pstDrg->stRect.s16Y2 + 2;


			pu8DatCtr = pstImgFgOrg->pu8Data + w * lb_y;
			pu8DatPre = pu8DatCtr - w;
			pu8DatCur = pu8DatCtr;
			sn = pstDrg->u8Sign;

			for( y = lb_y; y <= ru_y; y++ )
			{
				x = lb_x;
				do
				{
					while( x<=ru_x && pu8DatCur[x] != sn ) ++x;
					if( x>ru_x ) break;
					rl_s = x;
					while( x<=ru_x && pu8DatCur[x] == 0 ) ++x;
					rl_e = x-1;

					x1 = rl_s;
					while( x1<=rl_e && pu8DatPre[x1] != sn ) ++x1;
					if( x1>rl_e )
						memset( &(pu8DatCur[rl_s]), 128, rl_e - rl_s + 1 );


				} while( x <= ru_x );
				pu8DatPre = pu8DatCur;
				pu8DatCur += w;
			}
			pu8DatCtr = pstImgFgOrg->pu8Data + w * ru_y;
			pu8DatPre = pu8DatCtr + w;
			pu8DatCur = pu8DatCtr;
			for( y = ru_y; y >= lb_y; y-- )
			{
				x = lb_x;
				do
				{
					while( x <= ru_x && pu8DatCur[x] != 128 ) ++x;
					if( x > ru_x ) break;
					rl_s = x;
					while( x <= ru_x && pu8DatCur[x] == 128 ) ++x;
					rl_e = x - 1;

					x1 = rl_s;
					while( x1 <= rl_e && pu8DatPre[x1] != sn ) ++x1;
					if( x1>rl_e )
						memset( &(pu8DatCur[rl_s]), 255, rl_e - rl_s + 1 );
					else
						memset( &(pu8DatCur[rl_s]), 0, rl_e - rl_s + 1 );

				} while( x <= ru_x );
				pu8DatPre = pu8DatCur;
				pu8DatCur -= w;
			}
		}
	IMP_ITERATE_RGS_END
}

static IMP_VOID ipLightRegionsProcess( PEA_REGION_EXTRACT_MODULE_S *pModule );
static IMP_VOID ipRegionExtractOnMotion( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	PEA_REGION_EXTRACT_PARA_S *pstPara = &pModule->stPara;
	PEA_REGION_EXTRACT_DATA_S *pstMData = &pModule->stData;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	IMP_S32 s32Width = pstResult->s32Width;
	IMP_S32 s32Height = pstResult->s32Height;
	PEA_DETECTED_REGIONSET_S *pstRegions = &pstResult->stDRegionSet;
	IMP_S32 s32FgIsDiff = pstMData->pstDtData->s32FgIsDiff;
	IMP_RECT_S stRectLabel;


	ipDetectedRegionSetClear( pstRegions );


	IMP_SET_RECT( stRectLabel, 0, 0, s32Width - 1, s32Height - 1 );
#if 0
	ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion");
	ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFrmDiff->pu8Data, "pstImgFrmDiff");
#endif
	IMP_PEA_RGE_ExtractRegionSetFromFgImage(&pstPara->stLabelPara, pstRegions, &stRectLabel, s32FgIsDiff, pstMData );
	
//	ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion1");
	if( pstPara->stSuppressFgPara.s32Enable )
	{
		ipSuppressRegionSetForeground( &pstPara->stSuppressFgPara, pstRegions );

	}
//ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion2");
	if( pstPara->stShadowPara.s32DetectShadow )
	{
	//	printf("Shadow Detection!!\n");
		ipRemoveRegionSetShadow( &pstPara->stShadowPara, pstRegions, &pModule->stShadowImage );
	}

//ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion3");
	ipLabelRegionSetTypeMotion( &pstPara->stTypePara, pstRegions );
//	ipLightRegionsProcess( pModule ); //it will cause the flaw of region, need to find reason

//ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion4");
	if( pstPara->stMergePara.s32MergeRegion )
		IMP_PEA_RGE_MergeRegionSet( &pstPara->stMergePara, pstRegions );

//ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion5");
	IMP_PEA_RGE_RemoveAbnormalRegions( &pstPara->stRmAbnmlPara, pstRegions );
#if 0
	ipShowBinImage(s32Width, s32Height, pstRegions->pstImgFgRgn->pu8Data, "pstImgFgRgn_Motion6");
#endif
}



static IMP_VOID ipLightRegionsProcess( PEA_REGION_EXTRACT_MODULE_S *pModule )
{
	PEA_REGION_EXTRACT_PARA_S *pstPara = &pModule->stPara;
	PEA_RESULT_S *pstResult = pModule->pstResult;
	IMP_S32 s32Width = pstResult->s32Width;

	PEA_DETECTED_REGIONSET_S *pstRgs = &pstResult->stDRegionSet;
	GRAY_IMAGE_S *pstImgFrmDiff = pstRgs->pstImgFrmDiff;
	GRAY_IMAGE_S *pstImgFgRgn = pstRgs->pstImgFgRgn;
	PEA_RGE_MERGE_INNER_PARA_S stMinnerPara;
	PEA_DETECTED_REGION_S	*pstDrg;
	IMP_RECT_S stRectLabel;
	IMP_S32 i, s32Cnt;
	IMP_S32 lb_x, lb_y, ru_x, ru_y;
	IMP_S32 xx, yy, sn;
	IMP_U8 *pu8Fg, *pu8Diff;

	stMinnerPara.s32InnerLight = IMP_TRUE;
	IMP_PEA_RGE_MergeRegionSetInner( &stMinnerPara, pstRgs );

	IMP_ITERATE_RGS_START
	IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	if( IMP_DRG_IS_LIGHT(pstDrg->u8Used) )
	{

		lb_x = pstDrg->stRect.s16X1;
		ru_x = pstDrg->stRect.s16X2;
		lb_y = pstDrg->stRect.s16Y1;
		ru_y = pstDrg->stRect.s16Y2;
		sn = pstDrg->u8Sign;

		pu8Fg = pstImgFgRgn->pu8Data + lb_y * s32Width;
		pu8Diff = pstImgFrmDiff->pu8Data + lb_y * s32Width;
		for( yy = lb_y; yy <= ru_y; yy++ )
		{
			for( xx = lb_x; xx <= ru_x; xx++ )
			{
				IMP_S32 s32Fg1, s32Fg2;
				s32Fg1 = pu8Fg[xx];
				s32Fg2 = pu8Diff[xx] ? 255 : 0;
				pu8Fg[xx] = s32Fg1==sn ? s32Fg2 : s32Fg1;
			}
			pu8Fg += s32Width;
			pu8Diff += s32Width;
		}
		ipDetectedRegionSetFree( pstRgs, i );

		IMP_SET_RECT( stRectLabel, lb_x, lb_y, ru_x, ru_y );
		IMP_PEA_RGE_ExtractRegionSetFromFgImage(&pstPara->stLabelPara, pstRgs, &stRectLabel, IMP_TRUE, NULL );
	}
	IMP_ITERATE_RGS_END

}
