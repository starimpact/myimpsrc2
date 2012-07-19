#include "imp_pea_detector.h"

IMP_VOID IMP_PEA_DetectorDataClear( PEA_DETECTOR_DATA_S *pstData )
{
	pstData->s32ThCurrent = (pstData->s32ThMax + pstData->s32ThMin) / 2;

	pstData->u32BgStatusPre = IMP_BGM_STA_WORK;
	pstData->u32BgStatusCur = IP_BGM_STA_INIT;
}


IMP_VOID ipCreateTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResouce )
{
	IMP_S32 s32Height, s32Width;
	MEM_MGR_ARRAY_S *pstMemMgr;
    PEA_DETECTED_REGIONSET_S *pstRegionSet;
	memset( pstTargetDetector, 0, sizeof(PEA_TARGET_DETECTOR_S) );
	pstTargetDetector->pstHwResource = pHwResouce;
	pstMemMgr = &pHwResouce->stMemMgr;

	s32Height = pstResult->s32Height;
	s32Width = pstResult->s32Width;
	pstTargetDetector->pstResult = pstResult;
	
	
//	pstTargetDetector->hGGModel = IMP_CreateGrayGaussian(pstResult, pHwResouce);
	
	//create and init ViBe model
	pstTargetDetector->hViBeModel = IMP_CreateViBe(pstResult, pHwResouce);
	
	pstTargetDetector->hSwingModel = IMP_CreateSwing(pstResult, pHwResouce);
	
	pstTargetDetector->hOSCDModel = IMP_CreateOSCD(pstResult, pHwResouce);
	
	pstTargetDetector->hLFModel = IMP_CreateLightRemove(pstResult, pHwResouce);

#ifdef USE_WATERMARK_DETECOTR
	ipCreateWaterMarkDetector(&pstTargetDetector->stWaterMarkDetector,pstResult,pHwResouce);
#endif
 	ipCreateRegionExtract( &pstTargetDetector->stRgExtract, &pstTargetDetector->stData, pstResult, pHwResouce );

	IMP_GrayImageCreate( &pstTargetDetector->stImgInGrayDiff, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgBgGray, s32Width, s32Height, pstMemMgr );
#ifndef IMP_ARM_PEA
	IMP_GrayImageCreate( &pstTargetDetector->stImgStBgGray, s32Width, s32Height, pstMemMgr );///NEW
	IMP_GrayImageCreate( &pstTargetDetector->stImgBgDiff, s32Width, s32Height, pstMemMgr );	///NEW
#endif
	IMP_GrayImageCreate( &pstTargetDetector->stImgBgEdge, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgBgEdgeMag, s32Width, s32Height, pstMemMgr );
#if defined( USE_CODEBOOK_MODELING )
	#ifdef USE_MAG_DIFF
	IMP_GrayImageCreate( &pstTargetDetector->stImgBgInDiffMag, s32Width, s32Height, pstMemMgr );
	#endif
#endif
	IMP_GrayImage16Create( &pstTargetDetector->stImgBgEdgeGrad, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgInEdge, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgInEdgeMag, s32Width, s32Height, pstMemMgr );

	IMP_GrayImageCreate( &pstTargetDetector->stImgInEdgeMagPre, s32Width, s32Height, pstMemMgr );

	IMP_GrayImage16Create( &pstTargetDetector->stImgInEdgeGrad, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgFgOrgMotion, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgFgRgnMotion, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstTargetDetector->stImgContour, s32Width, s32Height, pstMemMgr );

	IMP_GrayImageCreate( &pstTargetDetector->stImgFgMorph, s32Width, s32Height, pstMemMgr );
 	IMP_GrayImageCreate( &pstTargetDetector->stImgMediate, s32Width, s32Height, pstMemMgr );
#ifdef ACCUM_BGEDGE
	IMP_GrayImageCreate( &pstTargetDetector->pstImgBgEdgeG, s32Width, s32Height, pstMemMgr );
#endif

	
	{
		
		pstRegionSet = &pstTargetDetector->pstResult->stDRegionSet;
		pstRegionSet->pstImgInGray = &pstTargetDetector->pstResult->stImgInGray;
		pstRegionSet->pstImgInEdge = &pstTargetDetector->stImgInEdge;
		pstRegionSet->pstImgInEdgeMag = &pstTargetDetector->stImgInEdgeMag;

		pstRegionSet->pstImgInEdgeMagPre = &pstTargetDetector->stImgInEdgeMagPre;

		pstRegionSet->pstImgInEdgeGrad = &pstTargetDetector->stImgInEdgeGrad;

		//use vibe background
		pstRegionSet->pstImgBgGray = &pstTargetDetector->stImgBgGray; //pstResult->stOutPutViBeModel.stImgBg;
		
#ifndef IMP_ARM_PEA
		pstRegionSet->pstImgStBgGray = &pstTargetDetector->stImgStBgGray;
		pstRegionSet->pstImgBgDiff = &pstTargetDetector->stImgBgDiff;
#endif
#if defined( USE_CODEBOOK_MODELING )
	#ifdef USE_MAG_DIFF
        pstRegionSet->pstImgBgInDiffMag = &pstTargetDetector->stImgBgInDiffMag;
	#endif
#endif
		pstRegionSet->pstImgBgEdge = &pstTargetDetector->stImgBgEdge;
		pstRegionSet->pstImgBgEdgeMag = &pstTargetDetector->stImgBgEdgeMag;

		pstRegionSet->pstImgBgEdgeGrad = &pstTargetDetector->stImgBgEdgeGrad;
		pstRegionSet->pstImgFgOrg = &pstTargetDetector->stImgFgOrgMotion;
		pstRegionSet->pstImgFgRgn = &pstTargetDetector->stImgFgRgnMotion;
		pstRegionSet->pstImgFgLife = NULL;
		pstRegionSet->pstImgFrmDiff = &pstTargetDetector->stImgInGrayDiff;
		pstRegionSet->pstImgFgMorph = &pstTargetDetector->stImgFgMorph;
		pstRegionSet->pstImgMediate = &pstTargetDetector->stImgMediate;
		pstRegionSet->pstImgContour = &pstTargetDetector->stImgContour;
	}
	

#ifdef SUPPORT_CHANGE
	pstRegionSet = &pstTargetDetector->pstResult->stDRegionSetChg;
	pstRegionSet->pstImgInGray = &pstTargetDetector->pstResult->stImgInGray;
	pstRegionSet->pstImgInEdgeMag = &pstTargetDetector->stImgInEdgeMag;
	pstRegionSet->pstImgInEdgeMagPre = &pstTargetDetector->stImgInEdgeMagPre;
	pstRegionSet->pstImgInEdgeGrad = &pstTargetDetector->stImgInEdgeGrad;
	pstRegionSet->pstImgFgOrg = &pstTargetDetector->stImgFgOrgMotion;
	pstRegionSet->pstImgFgRgn = &pstTargetDetector->stCodebookBgModel.stImgFgChg;
	pstRegionSet->pstImgFrmDiff = &pstTargetDetector->stImgInGrayDiff;
	pstRegionSet->pstImgInEdge = &pstTargetDetector->stImgInEdge;
	pstRegionSet->pstImgBgEdge = &pstTargetDetector->stImgBgEdge;
	pstRegionSet->pstImgBgEdgeMag = &pstTargetDetector->stImgBgEdgeMag;
	pstRegionSet->pstImgBgEdgeGrad = &pstTargetDetector->stImgBgEdgeGrad;

#ifdef  USE_CODEBOOK_MODELING
	pstRegionSet->pstImgBgGray = &pstTargetDetector->stCodebookBgModel.stImgBg;
#endif

#endif

}

// terminate
IMP_VOID ipReleaseTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &pstTargetDetector->pstHwResource->stMemMgr;
	
#ifdef ACCUM_BGEDGE
	IMP_GrayImageDestroy( &pstTargetDetector->pstImgBgEdgeG, pstMemMgr );
#endif
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgMediate, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgFgMorph, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgContour, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgFgRgnMotion, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgFgOrgMotion, pstMemMgr );
	
	IMP_GrayImage16Destroy( &pstTargetDetector->stImgInEdgeGrad, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgInEdgeMagPre, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgInEdgeMag, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgInEdge, pstMemMgr );
	
	IMP_GrayImage16Destroy( &pstTargetDetector->stImgBgEdgeGrad, pstMemMgr );
	
#if defined( USE_CODEBOOK_MODELING )
	#ifdef USE_MAG_DIFF
	IMP_GrayImageDestroy( &pstTargetDetector->stImgBgInDiffMag, pstMemMgr );
	#endif
#endif	
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgBgEdgeMag, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgBgEdge, pstMemMgr );
	
#ifndef IMP_ARM_PEA
	IMP_GrayImageDestroy( &pstTargetDetector->stImgStBgGray, pstMemMgr );///NEW
	IMP_GrayImageDestroy( &pstTargetDetector->stImgBgDiff, pstMemMgr );///NEW
#endif

	IMP_GrayImageDestroy( &pstTargetDetector->stImgBgGray, pstMemMgr );
	
	IMP_GrayImageDestroy( &pstTargetDetector->stImgInGrayDiff, pstMemMgr );
	
	ipReleaseRegionExtract( &pstTargetDetector->stRgExtract );
	
#ifdef USE_WATERMARK_DETECOTR
	ipReleaseWaterMarkDetector(&pstTargetDetector->stWaterMarkDetector);
#endif
	
	//release image
	IMP_ReleaseViBe(pstTargetDetector->hViBeModel);
	
	IMP_ReleaseSwing(pstTargetDetector->hSwingModel);
	
	IMP_ReleaseOSCD(pstTargetDetector->hOSCDModel);
	
	IMP_ReleaseLightRemove(pstTargetDetector->hLFModel);
	
//	IMP_ReleaseGrayGaussian(pstTargetDetector->hGGModel);
	
	memset( pstTargetDetector, 0, sizeof(PEA_TARGET_DETECTOR_S) );
}

IMP_VOID ipConfigTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector, PEA_DETECTOR_PARA_S *pstPara )
{
	IMP_S32 s32FrmDura = pstPara->s32FrmDura;
	PEA_DETECTOR_PARA_S *pstDetPara = &pstTargetDetector->stPara;
	PEA_DETECTOR_DATA_S *pstDetData = &pstTargetDetector->stData;
	IMP_OSCDPara_S stOSCDPara;
	
	stOSCDPara.pstRule = pstPara->pstRule;
	
	IMP_ConfigOSCD(pstTargetDetector->hOSCDModel, &stOSCDPara);
	
	memcpy( pstDetPara, pstPara, sizeof(PEA_DETECTOR_PARA_S) );
	{
		PEA_STATUS_S *pstStatus = &pstTargetDetector->pstResult->stSysStatus;
		pstStatus->u32ProcessChg = ( pstPara->s32DetStaticLeft || pstPara->s32DetStaticRemove );
	}


#ifdef USE_WATERMARK_DETECOTR

	ipConfigWaterMarkDetector(&pstTargetDetector->stWaterMarkDetector,NULL);
#endif
	// PEA_REGION_EXTRACT_PARA_S
	{
		PEA_REGION_EXTRACT_PARA_S *pstRgePara = &pstDetPara->stRgePara;
		ipConfigRegionExtract( &pstTargetDetector->stRgExtract, pstRgePara );
	}

}

// clear
static IMP_VOID ipClearTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector )
{
#ifdef ACCUM_BGEDGE
	pstTargetDetector->stData.pstImgBgEdgeG = &pstTargetDetector->pstImgBgEdgeG;
#else
	pstTargetDetector->stData.pstImgBgEdgeG = &pstTargetDetector->stImgBgEdge;
#endif

	IMP_GrayImageClear( &pstTargetDetector->stImgInGrayDiff );
	IMP_GrayImageClear( &pstTargetDetector->stImgBgGray );
#ifndef IMP_ARM_PEA
	IMP_GrayImageClear( &pstTargetDetector->stImgStBgGray );///NEW
	IMP_GrayImageClear( &pstTargetDetector->stImgBgDiff );///NEW
#endif
	IMP_GrayImageClear( &pstTargetDetector->stImgBgEdge );
	IMP_GrayImageClear( &pstTargetDetector->stImgBgEdgeMag );
#if defined( USE_CODEBOOK_MODELING )
	#ifdef USE_MAG_DIFF
	IMP_GrayImageClear( &pstTargetDetector->stImgBgInDiffMag );
	#endif
#endif
	IMP_GrayImage16Clear( &pstTargetDetector->stImgBgEdgeGrad );
	IMP_GrayImageClear( &pstTargetDetector->stImgInEdge );
	IMP_GrayImageClear( &pstTargetDetector->stImgInEdgeMag );

	IMP_GrayImageClear( &pstTargetDetector->stImgInEdgeMagPre );

	IMP_GrayImage16Clear( &pstTargetDetector->stImgInEdgeGrad );
	IMP_GrayImageClear( &pstTargetDetector->stImgFgOrgMotion );
	IMP_GrayImageClear( &pstTargetDetector->stImgFgRgnMotion );
	IMP_GrayImageClear( &pstTargetDetector->stImgContour );

	IMP_GrayImageClear( &pstTargetDetector->stImgFgMorph );
 	IMP_GrayImageClear( &pstTargetDetector->stImgMediate );
#ifdef ACCUM_BGEDGE
 	IMP_GrayImageClear( &pstTargetDetector->pstImgBgEdgeG );
#endif
	IMP_PEA_DetectorDataClear( &pstTargetDetector->stData );
}

static IMP_VOID ipPreProcessCurrentImage( PEA_TARGET_DETECTOR_S *pstTargetDetector );

IMP_S32 ipProcessTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector )
{
#define PTDI_TIME 0
	PEA_STATUS_S *pstStatus = &pstTargetDetector->pstResult->stSysStatus;
    PEA_DETECTED_REGIONSET_S *pstRegionSet = 0;
	GRAY_IMAGE_S *pstImgFrmDiff = 0;
	
	PEA_RESULT_S *pstResult = pstTargetDetector->pstResult;
	
	int s32ImgW, s32ImgH;
	
	PEA_REGION_EXTRACT_MODULE_S *pstRgExtract = &pstTargetDetector->stRgExtract;
	GRAY_IMAGE_S *pstImgIn = &pstTargetDetector->pstResult->stImgInGray;
#ifndef IMP_ARM_PEA
	GRAY_IMAGE_S *pstImgBgDiff = &pstTargetDetector->stImgBgDiff;
#endif
	pstStatus->u32DetectorStatus = IMP_MOUDLE_STATUS_SUCCESS;
	IMP_GrayImageClear( &pstTargetDetector->stImgContour );
	
	pstRegionSet = &pstTargetDetector->pstResult->stDRegionSet;
	
	pstImgFrmDiff = pstRegionSet->pstImgFrmDiff;
	
	s32ImgW = pstTargetDetector->pstResult->s32Width;
	s32ImgH = pstTargetDetector->pstResult->s32Height;
	
#if PTDI_TIME || 1
struct timeval t1, t2;
#endif

#ifdef USE_WATERMARK_DETECOTR

#if PTDI_TIME
gettimeofday(&t1, NULL);
#endif

	ipProcessWaterMarkDetector(&pstTargetDetector->stWaterMarkDetector);

#if PTDI_TIME
gettimeofday(&t2, NULL);
printf("ProcessWaterMarkDetector:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);	
#endif

#endif

#if PTDI_TIME
gettimeofday(&t1, NULL);
#endif
//	ipPreProcessCurrentImage( pstTargetDetector );

#if PTDI_TIME
gettimeofday(&t2, NULL);
printf("ipPreProcessCurrentImage:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif

#if PTDI_TIME
gettimeofday(&t1, NULL);
#endif	
	//noise estimation
	
//	ipNoiseEstimateByBox(pstImgIn, pstImgFrmDiff, &pstTargetDetector->pstResult->s32Noise);
	ipNoiseEstimateByBox_25(pstImgIn, pstImgFrmDiff, &pstTargetDetector->pstResult->s32Noise);
//	pstTargetDetector->pstResult->s32Noise = 10;
#if PTDI_TIME
gettimeofday(&t2, NULL);
printf("noise:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif
	
//	IMP_ProcessGrayGaussian(pstTargetDetector->hGGModel);

	IMP_ProcessLightRemove(pstTargetDetector->hLFModel);
	
#if 1 //VIBE

#if PTDI_TIME || 1
gettimeofday(&t1, NULL);
#endif
	//process ViBe
	IMP_ProcessViBe(pstTargetDetector->hViBeModel);
	
#if PTDI_TIME || 1
gettimeofday(&t2, NULL);
printf("vibe:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
	
	IMP_ProcessSwing(pstTargetDetector->hSwingModel);
	
/*
	{
		IMP_OutputViBe_S *pstViBe = &pstResult->stOutPutViBeModel;
		IMP_OutputLightRemove_S *pstLR = &pstResult->stOutPutLR;
		IMP_UCHAR *pu8img = pstLR->pu8img;
	//	GRAY_IMAGE_S *pstImgBg = pstViBe;
		GRAY_IMAGE_S *pstImgFg = &pstViBe->stImgFg;
		IMP_S32 s32RI, s32CI;
		
		for (s32RI = 0; s32RI < s32ImgH; s32RI++)
		{
			for (s32CI = 0; s32CI < s32ImgW; s32CI++)
			{
				if (pu8img[s32RI * s32ImgW + s32CI] == 1 || pu8img[s32RI * s32ImgW + s32CI] == 3)
				{
					pstImgFg->pu8Data[s32RI * s32ImgW + s32CI] = 0;
				}
			}
		}
	}
*/

#endif //VIBE

	

#if PTDI_TIME
gettimeofday(&t1, NULL);
#endif
	IMP_ProcessOSCD(pstTargetDetector->hOSCDModel);
#if PTDI_TIME
gettimeofday(&t2, NULL);
printf("IMP_ProcessOSCD:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif


if (pstResult->s32ModuleSwitch & 1)
{
#if PTDI_TIME
gettimeofday(&t1, NULL);
#endif

 	ipProcessRegionExtract( pstRgExtract );

#if PTDI_TIME
gettimeofday(&t2, NULL);
printf("ipProcessRegionExtract:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif
}
//printf("ipProcessTargetDetectorInternal:8\n");
	return 1;
}

// postprocess
IMP_S32 ipPostProcessTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector )
{
	PEA_REGION_EXTRACT_MODULE_S *pstRgExtract = &pstTargetDetector->stRgExtract;

#ifndef IMP_ARM_PEA
	GRAY_IMAGE_S *pstImgStBgGray = &pstTargetDetector->stImgStBgGray;
	GRAY_IMAGE_S *pstImgBgDiff = &pstTargetDetector->stImgBgDiff;
#endif
	GRAY_IMAGE_S *pstImgIn = &pstTargetDetector->pstResult->stImgInGray;
	IMP_S32 s32Width = pstImgIn->s32W;
	IMP_S32 s32Height = pstImgIn->s32H;

	ipPostprocessRegionExtract( pstRgExtract );

#ifdef USE_WATERMARK_DETECOTR
	ipPostProcessWaterMarkDetector(&pstTargetDetector->stWaterMarkDetector);
#endif
	return 1;
}

static IMP_VOID ipPreProcessCurrentImage( PEA_TARGET_DETECTOR_S *pstTargetDetector )
{
	IMP_S32 s32Height = pstTargetDetector->pstResult->s32Height;
	IMP_S32 s32Width = pstTargetDetector->pstResult->s32Width;
	PEA_DETECTOR_PARA_S *pstPara = &pstTargetDetector->stPara;


	if( pstPara->stPreProPara.s32DoSmooth )
		ipMedianImage( pstTargetDetector->pstResult->stImgInGray.pu8Data, pstTargetDetector->stImgMediate.pu8Data, s32Width, s32Height);
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
IMP_MODULE_HANDLE ipCreateDetector( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce )
{
	IMP_MODULE_HANDLE hModule=NULL;
	IpModuleDetector *pModule=NULL;
	PEA_TARGET_DETECTOR_S *pDetector=NULL;

	pModule = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpModuleDetector) );
	pModule->pDetector = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(PEA_TARGET_DETECTOR_S) );
	pDetector = pModule->pDetector;
	ipCreateTargetDetectorInternal( pDetector, pResult, pHwResouce );

	hModule = (IMP_MODULE_HANDLE)pModule;
	return hModule;
}

IMP_VOID ipReleaseDetector( IMP_MODULE_HANDLE hModule )
{
	IpModuleDetector *pModule = (IpModuleDetector*)hModule;
	PEA_TARGET_DETECTOR_S *pDetector = pModule->pDetector;
	GA_HARDWARE_RS_S *pHwResouce = pDetector->pstHwResource;

	ipReleaseTargetDetectorInternal( pDetector );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pDetector );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule );
}

static IMP_VOID ipForceVapara2DetectorPara( INNER_PARA_S *pstParaInner, PEA_DETECTOR_PARA_S *pstPara );
IMP_VOID ipConfigDetector( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstParaInner )
{
	IpModuleDetector *pModule = (IpModuleDetector*)hModule;
	PEA_TARGET_DETECTOR_S *pDetector = pModule->pDetector;
	DATA_BUF_S *pstDatBuf;
	PEA_DETECTOR_PARA_S pstPara;
	
	
//	IMP_ConfigViBeModel( IMP_MODULE_HANDLE hModule, IMP_ViBePara_S *pstPara );

	pstDatBuf = &pstParaInner->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_DETECTOR];
	ipParseDetectorParaData(&pstPara );
	ipForceVapara2DetectorPara( pstParaInner, &pstPara );
 	ipConfigTargetDetectorInternal( pDetector, &pstPara );
}


IMP_S32 IMP_PEA_ProcessDetector( IMP_MODULE_HANDLE hModule )
{
	IpModuleDetector *pModule = (IpModuleDetector*)hModule;
	PEA_TARGET_DETECTOR_S *pDetector = pModule->pDetector;
//printf("ProcessDetector\n");
	return ipProcessTargetDetectorInternal( pDetector );
}


IMP_S32 ipPostProcessDetector( IMP_MODULE_HANDLE hModule )
{
	IpModuleDetector *pModule = (IpModuleDetector*)hModule;
	PEA_TARGET_DETECTOR_S *pDetector = pModule->pDetector;

	return ipPostProcessTargetDetectorInternal( pDetector );
}

static IMP_VOID ipForceVapara2DetectorPara( INNER_PARA_S *pstParaInner, PEA_DETECTOR_PARA_S *pstPara )
{
	GLOBAL_PARA_S *pstGlobalPara = &pstParaInner->stGlobal;
	RULE_S *pstRule;
	RULE_ZONES_S *pstZones;
	RULE_ZONE_S *pstZone;
	USR_ADV_PARA_S *pstUsrAdvPara;
	IMP_U32 u32FuncUseMotion, u32FuncUseStaticRemove, u32FuncUseStaticLeft;
	IMP_U32 u32Mode;
	IMP_S32 i;

	pstPara->pstRule = pstParaInner->pRule;
	pstRule = pstPara->pstRule;
	pstZones = &pstPara->pstRule->stZones;
	pstUsrAdvPara = (USR_ADV_PARA_S*)pstRule->u8AdvParaData;

	pstPara->s32FrmDura = pstGlobalPara->s32TimeUnit;


	u32FuncUseMotion = IMP_FUNC_PERIMETER
					| IMP_FUNC_TRIPWIRE
					| IMP_FUNC_MTRIPWIRE
					| IMP_FUNC_LOITER;
	u32FuncUseStaticRemove = IMP_FUNC_OBJSTOLEN;
	u32FuncUseStaticLeft = IMP_FUNC_NOPARKING;

	u32Mode = pstRule->u32Enacode;
 	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
 	{
 		pstZone = &pstZones->astZone[i];
 		if( pstZone->u32Valid && pstZone->u32Enable )
 			u32Mode |= pstZone->u32Mode;
 	}

	pstPara->s32DetMotion = ( u32Mode & u32FuncUseMotion );
	pstPara->s32DetStaticLeft = ( u32Mode & u32FuncUseStaticLeft );
	pstPara->s32DetStaticRemove = ( u32Mode & u32FuncUseStaticRemove );

	if( pstPara->s32DetStaticLeft||pstPara->s32DetStaticRemove )
	{
		if( pstPara->stRgePara.stSuppressFgPara.s32LightEnable == 2 )
			pstPara->stRgePara.stSuppressFgPara.s32LightEnable = 0;
		if( pstPara->stRgePara.stSuppressFgPara.s32PenumbraEnable == 2 )
			pstPara->stRgePara.stSuppressFgPara.s32PenumbraEnable = 0;
	}
}


