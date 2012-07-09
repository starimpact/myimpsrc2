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


STATUS_E IMP_OSC_HD_GetMemReq(IMP_HANDLE hModule, MEM_SET_S *pstMems)
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
	
	IMP_S32 s32MemSize = 0;
	
	IMP_FLOAT fSlap = 52.04f;
	IMP_S32 s32B0 = 4311488, s32A0 = 176*144;
	IMP_S32 s32NB = 0, s32NA = pstSubMems->u32ImgH * pstSubMems->u32ImgW;
	
	
	s32NB = s32B0 + (s32NA - s32A0) * fSlap;
	s32MemSize = s32NB;
//	printf("memsize:%d\n", s32MemSize);

#if 0
	switch (pstMems->u32ImgH)
	{
	case 144://qcif
		s32MemSize = 4400000;//4311488;
		break;
	case 288://cif
		s32MemSize = 8300000; //8267776
		break;
	case 576://d1
		s32MemSize = 8300000;//8267776;
		break;
	case 720://720p
		s32MemSize = 15000000; //14981312
		break;
	case 1080://1080p
		s32MemSize = 30000000; //29962112
		break;
	default:
		pstHDInfo->s32IfQuad = 2;
	}
#endif

	if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
	pstSubMems->stMems[IMP_MEMBLK_TYPE_SLOW].u32Size = s32MemSize;
//	printf("s32MemSize:%d\n", s32MemSize);
	
	memcpy(pstMems, pstSubMems, sizeof(MEM_SET_S));
	pstMems->u32ImgW = s32OriW;
	pstMems->u32ImgH = s32OriH;
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_OSC_HD_Create(IMP_HANDLE hModule, MEM_SET_S *pstMems)
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
	
	IMP_OSC_Create(hSubModule, pstSubMems);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_OSC_HD_Release(IMP_HANDLE hModule)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
	IMP_OSC_Release(hSubModule);
	
	IMP_YUVImage422Destroy(&pstHDInfo->stImageDst, NULL);
	
	free(pstHDInfo->pstSubModule);
	free(pstHDInfo->pstSubResult);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_HD_ReSampleRuleDate(URP_PARA_S *pURPpara);

STATUS_E IMP_OSC_HD_ConfigArmPeaParameter(IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara ,URP_PARA_S *pstURPPara)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	if (pstHDInfo->s32IfQuad == 2)
	{
		return IMP_STATUS_SKIP;
	}
	
	IMP_OSC_ConfigArmPeaParameter(hSubModule, pstExtnalPara, pstURPPara);
	
	if (pstHDInfo->s32IfQuad == 1)
	{
		IMP_HD_ReSampleRuleDate(pstURPPara);
	}
	
	return IMP_STATUS_OK;
}


IMP_VOID IMP_YUV422ImageClone_Y(YUV_IMAGE422_S *pstImageSrc, YUV_IMAGE422_S *pstImageDst);
IMP_VOID IMP_HD_QuadDownSample_Intensity(IMP_U8 *pu8YSrc, IMP_S32 s32W, IMP_S32 s32H, IMP_U8 *pu8YDst);
STATUS_E IMP_OSC_HD_ProcessImage(IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage)
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
	
	IMP_OSC_ProcessImage(hSubModule, &pstHDInfo->stImageDst);
	
	return IMP_STATUS_OK;
}


IMP_VOID IMP_UpSampleResult(RESULT_S *pstResult);

STATUS_E IMP_OSC_HD_GetResults(IMP_HANDLE hModule, RESULT_S *pstResult)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = pstHDInfo->pstSubModule;
	
	IMP_OSC_GetResults(hSubModule, pstHDInfo->pstSubResult);
	
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




