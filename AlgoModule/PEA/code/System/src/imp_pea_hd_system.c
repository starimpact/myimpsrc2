
typedef struct impHD_Info
{
	IMP_S32 s32IfQuad;
	MEM_SET_S stSubMems;
	YUV_IMAGE422_S stImageDst;
	OBJ_S stSubModule;
	RESULT_S stSubResult;
}IMP_HD_Info;


STATUS_E IMP_PEA_HD_GetMemReq(IMP_HANDLE hModule, MEM_SET_S *pstMems)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	MEM_SET_S *pstSubMems = &pstHDInfo->stSubMems;
	
	pstHDInfo->s32IfQuad = 0;
	if (pstMems->u32ImgH > 288)
	{
		pstSubMems->u32ImgW = pstMems->u32ImgW / 2;
		pstSubMems->u32ImgH = pstMems->u32ImgH / 2;
		pstHDInfo->s32IfQuad = 1;
	}
	
	IMP_GetMemReq(hSubModule, pstSubMems);
	
	IMP_S32 s32MemSize = 0;
	
	switch (pstMems->u32ImgH)
	{
	case 144:
		s32MemSize = 0;
		break;
	case 288:
		s32MemSize = 0;
		break;
	case 576:
		s32MemSize = 0;
		break;
	case 720:
		s32MemSize = 0;
		break;
	case 1080:
		s32MemSize = 0;
		break;
	}
	
	pstSubMems->stMems[IMP_MEMBLK_TYPE_SLOW].u32Size = s32MemSize;
	
	memcpy(pstMems, pstSubMems, sizeof(MEM_SET_S));
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_Create(IMP_HANDLE hModule, MEM_SET_S *pstMems)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	
	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	MEM_SET_S *pstSubMems = &pstHDInfo->stSubMems;
	
	IMP_Create(hSubModule, pstSubMems);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_Release(IMP_HANDLE hModule)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	
	IMP_Release(hSubModule);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_ConfigArmPeaParameter(IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara ,URP_PARA_S *pstURPPara)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	
	IMP_ConfigArmPeaParameter(hSubModule, pstExtnalPara, pstURPPara);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_ProcessImage(IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	
	if (pstHDInfo->s32IfQuad)
	{
		//down sample the input image
	}
	
	IMP_ProcessImage(hSubModule, pstImage);
	
	return IMP_STATUS_OK;
}


STATUS_E IMP_PEA_HD_GetResults(IMP_HANDLE hModule, RESULT_S *pstResult)
{
	IMP_HD_Info *pstHDInfo = (IMP_HD_Info*)hModule;
	IMP_HANDLE hSubModule = &pstHDInfo->stSubModule;
	
	IMP_GetResults(hSubModule, &pstHDInfo->stSubResult);
	
	if (pstHDInfo->s32IfQuad)
	{
		//resize the result's value
	}
	
	return IMP_STATUS_OK;
}



