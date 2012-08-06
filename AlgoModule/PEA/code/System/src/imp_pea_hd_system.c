#include "imp_algo_interface.h"
#include "imp_pea_system.h"


typedef struct impHD_Info
{
	IMP_S32 s32IfQuad;
	MEM_SET_S stSubMems;
	YUV_IMAGE422_S stImageDst;
	OBJ_S *pstSubModule;
	RESULT_S *pstSubResult;
}IMP_HD_Info;


STATUS_E IMP_PEA_HD_GetMemReq(IMP_HANDLE hModule, MEM_SET_S *pstMems)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
//	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	MEM_SET_S *pstSubMems = &pstHDInfo->stSubMems;
	IMP_S32 s32OriW, s32OriH;
	
	printf("RESULT_S:%d\n", sizeof(RESULT_S));
	
	s32OriW = pstMems->u32ImgW;
	s32OriH = pstMems->u32ImgH;
	pstHDInfo->s32IfQuad = 0;
	if (pstMems->u32ImgH > 288)
	{
		pstSubMems->u32ImgW = pstMems->u32ImgW / 2;
		pstSubMems->u32ImgH = pstMems->u32ImgH / 2;
		pstHDInfo->s32IfQuad = 1;
	}
	else
	{
		pstSubMems->u32ImgW = pstMems->u32ImgW;
		pstSubMems->u32ImgH = pstMems->u32ImgH;
	}
	
	IMP_GetMemReq(0, pstSubMems);
	
	memcpy(pstMems, pstSubMems, sizeof(MEM_SET_S));
	pstMems->u32ImgW = s32OriW;
	pstMems->u32ImgH = s32OriH;
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_Create(IMP_HANDLE hModule, MEM_SET_S *pstMems)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	
	pstHDInfo->pstSubModule = malloc(sizeof(OBJ_S));
	pstHDInfo->pstSubResult = malloc(sizeof(RESULT_S));
	
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	MEM_SET_S *pstSubMems = &pstHDInfo->stSubMems;
	
	IMP_S32 s32OriW, s32OriH;
	
	s32OriW = pstSubMems->u32ImgW;
	s32OriH = pstSubMems->u32ImgH;
	memcpy(pstSubMems, pstMems, sizeof(MEM_SET_S));
	pstSubMems->u32ImgW = s32OriW;
	pstSubMems->u32ImgH = s32OriH;
	
	if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
	IMP_YUVImage422Create(&pstHDInfo->stImageDst, pstHDInfo->stSubMems.u32ImgW, pstHDInfo->stSubMems.u32ImgH, NULL);
	
	IMP_Create(hSubModule, pstSubMems);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_Release(IMP_HANDLE hModule)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
	IMP_Release(hSubModule);
	
	IMP_YUVImage422Destroy(&pstHDInfo->stImageDst, NULL);
	
	free(pstHDInfo->pstSubModule);
	free(pstHDInfo->pstSubResult);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_HD_ReSampleRuleDate(URP_PARA_S *pURPpara);

STATUS_E IMP_PEA_HD_ConfigArmPeaParameter(IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara ,URP_PARA_S *pstURPPara)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
	IMP_ConfigArmPeaParameter(hSubModule, pstExtnalPara, pstURPPara);
	
	if (pstHDInfo->s32IfQuad == 1)
	{
		IMP_HD_ReSampleRuleDate(pstURPPara);
	}
	
	return IMP_STATUS_OK;
}


IMP_VOID IMP_YUV422ImageClone_Y(YUV_IMAGE422_S *pstImageSrc, YUV_IMAGE422_S *pstImageDst);
IMP_VOID IMP_HD_QuadDownSample_Intensity(IMP_U8 *pu8YSrc, IMP_S32 s32W, IMP_S32 s32H, IMP_U8 *pu8YDst);

STATUS_E IMP_PEA_HD_ProcessImage(IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	if (pstHDInfo->s32IfQuad == 1)
	{
		//down sample the input image
		IMP_HD_QuadDownSample_Intensity(pstImage->pu8Y, pstImage->s32W, pstImage->s32H, pstHDInfo->stImageDst.pu8Y);
		pstHDInfo->stImageDst.u32Time = pstImage->u32Time;
		pstHDInfo->stImageDst.u32Flag = pstImage->u32Flag;
	}
	else if (pstHDInfo->s32IfQuad == 0)
	{
		IMP_YUV422ImageClone_Y(pstImage, &pstHDInfo->stImageDst);
	}
	else if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
//	ipShowGrayImage(pstHDInfo->stImageDst.s32W, pstHDInfo->stImageDst.s32H, pstHDInfo->stImageDst.pu8Y, "in");
	
	IMP_ProcessImage(hSubModule, &pstHDInfo->stImageDst);
	
	return IMP_STATUS_OK;
}


IMP_VOID IMP_UpSampleResult(RESULT_S *pstResult);

STATUS_E IMP_PEA_HD_GetResults(IMP_HANDLE hModule, RESULT_S *pstResult)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	IMP_GetResults(hSubModule, pstHDInfo->pstSubResult);
	
	//resize the result's value
	memcpy(pstResult, pstHDInfo->pstSubResult, sizeof(RESULT_S));
		
	if (pstHDInfo->s32IfQuad == 1)
	{
		IMP_UpSampleResult(pstResult);
	}
//	else if (pstHDInfo->s32IfQuad == 0)
//	{
//		memcpy(pstResult, pstHDInfo->pstSubResult, sizeof(RESULT_S));
//	}
	else if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}

#if 0	
	MEM_SET_S *pstSubMems = &pstHDInfo->stSubMems;
	IMP_S32 s32ImgW = pstSubMems->u32ImgW;
	IMP_S32 s32ImgH = pstSubMems->u32ImgH;
	PEA_MODULE *pPEA = (PEA_MODULE*)hSubModule;
    IMP_MODULE_HANDLE hObjRecg = pPEA->hObjRecg;
    PEA_ModuleObjRecognition *pObjRecg = (PEA_ModuleObjRecognition*)hObjRecg;
	PEA_TARGET_DETECTOR_S *pDetector = (PEA_TARGET_DETECTOR_S*)(*(IMP_U32*)pObjRecg->hDetector);
	
	ipShowBinImage(s32ImgW, s32ImgH, pDetector->stImgFgRgnMotion.pu8Data, "hello");
#endif
	
	return IMP_STATUS_OK;
}


IMP_VOID IMP_HD_QuadDownSample_Intensity(IMP_U8 *pu8YSrc, IMP_S32 s32W, IMP_S32 s32H, IMP_U8 *pu8YDst)
{
	IMP_S32 s32RI, s32CI;
	IMP_S32 s32RI1, s32CI1;
	IMP_U8 *pu8RSrc = 0, *pu8RDst = 0;
	
	s32RI1 = 0;
	s32CI1 = 0;
	pu8RSrc = pu8YSrc;
	pu8RDst = pu8YDst;
	for (s32RI = 0; s32RI < s32H / 2; s32RI++)
	{
		for (s32CI = 0; s32CI < s32W / 2; s32CI++)
		{
			pu8RDst[s32CI] = pu8RSrc[s32CI * 2];
		}
		pu8RSrc += 2 * s32W;
		pu8RDst += s32W / 2;
	}
}



IMP_VOID IMP_UpSampleResult(RESULT_S *pstResult)
{
	IMP_S32 s32PI, s32TI;
	
	TGT_SET_S *pstTargetSet = &pstResult->stTargetSet;
	EVT_SET_S *pstEventSet = &pstResult->stEventSet;
	
	EVT_ITEM_S *pstItem = 0;
	
	for (s32PI = 0; s32PI < pstEventSet->s32EventNum; s32PI++)
	{
		pstItem = &pstEventSet->astEvents[s32PI];
		EVT_DATA_TARGET_S *pstEDT = (EVT_DATA_TARGET_S*)(pstItem->au8Data);
		IMP_RECT_S	*pstRect = &pstEDT->stRect;
		pstRect->s16X1 *= 2;
		pstRect->s16Y1 *= 2;
		pstRect->s16X2 *= 2;
		pstRect->s16Y2 *= 2;
	}


	for (s32PI = 0; s32PI < pstTargetSet->s32TargetNum; s32PI++)
	{
		TGT_ITEM_S *pstTgtItem = &pstTargetSet->astTargets[s32PI];
		
		IMP_POINT_S	*pstPoint = &pstTgtItem->stPoint;
		IMP_RECT_S	*pstRect = &pstTgtItem->stRect;
		
		pstPoint->s16X *= 2;
		pstPoint->s16Y *= 2;
		
		pstRect->s16X1 *= 2;
		pstRect->s16Y1 *= 2;
		pstRect->s16X2 *= 2;
		pstRect->s16Y2 *= 2;
		
		TGT_MOTION_ITEM_S *pstMItem = (TGT_MOTION_ITEM_S*)(pstTgtItem->au8Data);
		TGT_TRAJECT_S *pstTraject = &pstMItem->stTraject;
		
		
		for (s32TI = 0; s32TI < pstTraject->s32Length; s32TI++)
		{
			pstTraject->stPoints[s32TI].s16X *= 2;
			pstTraject->stPoints[s32TI].s16Y *= 2;
		}
	}

}


IMP_VOID IMP_YUV422ImageClone_Y(YUV_IMAGE422_S *pstImageSrc, YUV_IMAGE422_S *pstImageDst)
{
	IMP_S32 s32W, s32H;
	
	s32W = pstImageSrc->s32W;
	s32H = pstImageSrc->s32H;
	pstImageDst->s32W = pstImageSrc->s32W;
	pstImageDst->s32H = pstImageSrc->s32H;
	pstImageDst->u32Time = pstImageSrc->u32Time;
	pstImageDst->u32Flag = pstImageSrc->u32Flag;
	
	memcpy(pstImageDst->pu8Y, pstImageSrc->pu8Y, s32W * s32H);
}


STATUS_E IMP_HD_ReSampleOneRgnRuleDate(URP_POLYGON_REGION_S *pstRgn, IMP_S32 s32SrcImgW, IMP_S32 s32SrcImgH, IMP_S32 s32DstImgW, IMP_S32 s32DstImgH)
{
	STATUS_E enRet = IMP_STATUS_OK;
	int i;
#if PRINT_DBG
	printf("s32Valid:%d\n", pstRgn->s32Valid);
#endif
	if (pstRgn->s32Valid)
	{
		for (i = 0; i < IMP_MAX_BOUNDARY_POINT_NUM;i++)
		{
			URP_IMP_POINT_S *pPnt = &pstRgn->astPoint[i];
			IMP_S32 x = pPnt->s16X;
			IMP_S32 y = pPnt->s16Y;
			pPnt->s16X = (short)(x * (s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pPnt->s16Y = (short)(y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
		}
	}

	return enRet;
}


//do resample operations int the URP_PARA_S based on the two different sizes of images
STATUS_E IMP_HD_ReSampleRuleDate(URP_PARA_S *pURPpara)
{
	STATUS_E enRet = IMP_STATUS_OK;

	IMP_S32 s32SrcImgW = pURPpara->stConfigPara.s32ImgW;
	IMP_S32 s32SrcImgH = pURPpara->stConfigPara.s32ImgH;

	IMP_S32 s32DstImgW = s32SrcImgW * 2;
	IMP_S32 s32DstImgH = s32SrcImgH * 2;
	IMP_S32 s32ZoneIndex = 0;
	IMP_S32 i = 0;
	

	
	for( s32ZoneIndex=0; s32ZoneIndex<8; s32ZoneIndex++ )
	{
		if( 0
			||	pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Valid == 0
			||	pURPpara->stRuleSet.astRule[s32ZoneIndex].u32Enable == 0
		   )
			continue;

//#ifdef SUPPORT_OSC //bird
		//added by mzhang 2011-11-15 13:25:11
		{
			URP_OSC_SPECL_REGIONS_S *psr =  &(pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stOscRulePara.astSpclRgs[0]);
			IMP_HD_ReSampleOneRgnRuleDate(&psr->stOscRg, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
			IMP_HD_ReSampleOneRgnRuleDate(&psr->astSubRgA, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
			IMP_HD_ReSampleOneRgnRuleDate(&psr->astSubRgA, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
			IMP_HD_ReSampleOneRgnRuleDate(&psr->astSubRgA, s32SrcImgW, s32SrcImgH, s32DstImgW, s32DstImgH);
		}
//#endif //bird

#if PRINT_DBG
printf("Perimeter Boundary Number:%d\n", pURPpara->stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum);
#endif
		for (i = 0; i < IMP_MAX_BOUNDARY_POINT_NUM;i++)
		{
			IMP_S32 x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16X;
			IMP_S32 y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16X = (short)(x * (s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[i].s16Y = (short)(y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
		}
		for (i = 0; i < IMP_URP_MAX_TRIPWIRE_CNT;i++)
		{
			IMP_S32 s_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16X;
			IMP_S32 s_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16Y;
			IMP_S32 e_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16X;
			IMP_S32 e_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16X = (short)(s_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stStartPt.s16Y = (short)(s_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16X = (short)(e_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stTripwireRulePara.astLines[i].stLine.stEndPt.s16Y = (short)(e_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
		}

		for (i = 0; i < IMP_URP_MAX_MTRIPWIRE_CNT;i++)
		{
			IMP_S32 s_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16X;
			IMP_S32 s_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16Y;
			IMP_S32 e_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16X;
			IMP_S32 e_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16X = (short)(s_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stStartPt.s16Y = (short)(s_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16X = (short)(e_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[0].stEndPt.s16Y = (short)(e_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));

			s_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16X;
			s_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16Y;
			e_x = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16X;
			e_y = pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16Y;
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16X = (short)(s_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stStartPt.s16Y = (short)(s_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16X = (short)(e_x*(s32DstImgW/(IMP_FLOAT)s32SrcImgW));
			pURPpara->stRuleSet.astRule[s32ZoneIndex].stPara.stMTripwireRulePara.astLines[i].astLine[1].stEndPt.s16Y = (short)(e_y*(s32DstImgH/(IMP_FLOAT)s32SrcImgH));

		}

	}
	return enRet;
}



