#include "time.h"
#include "stdlib.h"

#include "imp_osc_detect.h"
#include "imp_osc_detect_inner.h"
#include "imp_pea_regionset_label.h"




/***************************************************
name:IMP_GetMemSizeOSCD
do:得到算法所需要的内存大小
input:
	pstResult:系统公共数据
output:算法需要的内存大小
history:
****************************************************/
IMP_S32 IMP_GetMemSizeOSCD( PEA_RESULT_S *pstResult )
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	
 	IMP_S32 size = width * height * 30;
	
	return size;
}


/***************************************************
name:IMP_CreateOSCD
do:申请算法内存并始化算法参数
input:
	pstModule:算法模块
	pstResult:系统公共数据
	pstHwResource:系统硬件资源
output:
	0:失败
	非0:句柄
history:
****************************************************/
IMP_MODULE_HANDLE IMP_CreateOSCD( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	IMP_MODULE_HANDLE hModule;
	int dwI;
	IMP_S32 s32Height, s32Width;
	IMP_U8 **ppu8Bkgs = 0; //max background number is 10
	MEM_MGR_ARRAY_S *pstMemMgr;
	IMP_OSCD_S *pstModule;
	
	s32Height = pstResult->s32Height;
	s32Width = pstResult->s32Width;
	
	pstMemMgr = &pstHwResource->stMemMgr;

	pstModule = (IMP_OSCD_S*)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_OSCD_S));
	
	pstModule->pstHwResource = pstHwResource; //记录硬件信息地址
	pstModule->pstResult = pstResult; //记录系统信息地址
	
	pstModule->s32FrmNum = 0;
	pstModule->u32ObjNumber = 0;
	pstModule->s32Width = s32Width;
	pstModule->s32Height = s32Height;
	
	pstModule->s32MinSize = 100; 
	pstModule->s32MaxSize = s32Width * s32Height / 4; 
	pstModule->s32AlertTime = 100;
	pstModule->u32IDCounter = 0x10; //0x70000000;
	
	pstModule->s32First = 1;
	
	memset(pstModule->astRMVRgns, 0, sizeof(OSCD_REGION_S) * IMP_MAX_OSC_NUM);
	
	IMP_GrayImageCreate(&pstModule->stImgFgStatic, s32Width, s32Height, pstMemMgr);
	IMP_GrayImageCreate(&pstModule->stImgFgStatic2, s32Width, s32Height, pstMemMgr);
	IMP_GrayImageCreate(&pstModule->stImgSBkg, s32Width, s32Height, pstMemMgr);
	IMP_GrayImageCreate(&pstModule->stImgPreGray, s32Width, s32Height, pstMemMgr);
	
	IMP_GrayImageCreate(&pstModule->stImgSObjSign, s32Width, s32Height, pstMemMgr);
	IMP_GrayImageCreate(&pstModule->stImgSObjEdge, s32Width, s32Height, pstMemMgr);
	
	IMP_GrayImageCreate(&pstModule->stImgRObjSign, s32Width, s32Height, pstMemMgr);
	IMP_GrayImageCreate(&pstModule->stImgRObjEdge, s32Width, s32Height, pstMemMgr);

	IMP_GrayImageCreate(&pstModule->stImgBkgEdge, s32Width, s32Height, pstMemMgr);
	IMP_GrayImageCreate(&pstModule->stImgGrayEdge, s32Width, s32Height, pstMemMgr);
	
	
	hModule = (IMP_MODULE_HANDLE)pstModule;
	return hModule;
}


/***************************************************
name:IMP_ReleaseOSCD
do:释放算法内存
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ReleaseOSCD(IMP_MODULE_HANDLE hModule)
{
	int dwI;
	IMP_OSCD_S *pstModule = (IMP_OSCD_S*)hModule;
	
	IMP_S32 s32Height, s32Width;
	MEM_MGR_ARRAY_S *pstMemMgr;
	
	s32Width = pstModule->s32Width;
	s32Height = pstModule->s32Height;
	
	pstMemMgr = &pstModule->pstHwResource->stMemMgr;
	
	IMP_GrayImageDestroy(&pstModule->stImgFgStatic, pstMemMgr);
	IMP_GrayImageDestroy(&pstModule->stImgFgStatic2, pstMemMgr);
	IMP_GrayImageDestroy(&pstModule->stImgSBkg, pstMemMgr);
	IMP_GrayImageDestroy(&pstModule->stImgPreGray, pstMemMgr);
	
	IMP_GrayImageDestroy(&pstModule->stImgSObjSign, pstMemMgr);
	IMP_GrayImageDestroy(&pstModule->stImgSObjEdge, pstMemMgr);
	
	IMP_GrayImageDestroy(&pstModule->stImgRObjSign, pstMemMgr);
	IMP_GrayImageDestroy(&pstModule->stImgRObjEdge, pstMemMgr);
	
	IMP_GrayImageDestroy(&pstModule->stImgBkgEdge, pstMemMgr);
	
	IMP_GrayImageDestroy(&pstModule->stImgGrayEdge, pstMemMgr);
	
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
	pstModule = 0;
	
	return 0;
}

//crate objects for remove areas in config function
IMP_S32 impCreateRObjects(IMP_OSCD_S *pstModule);

/***************************************************
name:IMP_ConfigOSCD
do:配置算法参数
input:
	hModule:算法模块句柄
	pstPara:参数配置结构体
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ConfigOSCD(IMP_MODULE_HANDLE hModule, IMP_OSCDPara_S *pstPara)
{
	IMP_OSCD_S *pstModule = (IMP_OSCD_S*)hModule;
	
	pstModule->s32First = 1;
	
	pstModule->pstRule = pstPara->pstRule;
	
//	impCreateRObjects(pstModule);
	
	return 0;
}


IMP_S32 impProcessOSCD0( IMP_MODULE_HANDLE hModule );
IMP_S32 impRule2Inner(IMP_OSCD_S *pstModule);
IMP_S32 impUpdateSBkg(IMP_MODULE_HANDLE hModule);
/***************************************************
name:IMP_ProcessOSCD
do:算法主过程
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ProcessOSCD(IMP_MODULE_HANDLE hModule)
{
	IMP_OSCD_S *pstModule = (IMP_OSCD_S*)hModule;
	IMP_U8 *pu8PreGray = 0, *pu8InGray = 0;
	PEA_RESULT_S *pstResult = pstModule->pstResult; //系统公共数据
	IMP_S32 s32Width = pstResult->s32Width, s32Height = pstResult->s32Height;
	
	pu8InGray = pstResult->stImgInGray.pu8Data;
	pu8PreGray = pstModule->stImgPreGray.pu8Data;
	if (pstResult->u32FrmTimeCur == 0)
	{
		memcpy(pu8PreGray, pu8InGray, s32Width * s32Height);
	}
	
#if OSCD_DBG_SHW_TIME || 1	
	struct timeval t1, t2;
#endif

	if (pstResult->stSysStatus.u32NeedReInit)
	{
	//	impOSCD_Clear(pstModule);
		return 0;
	}

#if OSCD_DBG_SHW_TIME || 1
	gettimeofday(&t1, NULL);
#endif

	impDetectEdges((IMP_OSCD_S*)hModule);

#if OSCD_DBG_SHW_TIME || 1
	gettimeofday(&t2, NULL);
	printf("impDetectEdges:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif

#if 0
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	IMP_S32 s32I, s32DL = 8000, s32NCC = 0;
	IMP_U8 au8Data1[9], au8Data2[9];
	for (s32I = 0; s32I < s32DL; s32I++)
	{
		au8Data1[0] = 50; au8Data1[1] = 100; au8Data1[2] = 200;
		au8Data1[3] = 50; au8Data1[4] = 100; au8Data1[5] = 200;
		au8Data1[6] = 50; au8Data1[7] = 100; au8Data1[8] = 200;
		
		au8Data2[0] = 50; au8Data2[1] = 100; au8Data2[2] = 200;
		au8Data2[3] = 50; au8Data2[4] = 100; au8Data2[5] = 200;
		au8Data2[6] = 50; au8Data2[7] = 100; au8Data2[8] = 200;
		s32NCC += impNCC_U8(au8Data1, au8Data2, 9);
	}
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("impNCC_U8:%.1f ms, num:%d, ncc:%d\n", (t2.tv_usec - t1.tv_usec) / 1000.f, s32DL, s32NCC / s32DL);
#endif
	return 0;
#endif

#if 0
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	IMP_S32 s32I, s32DL = 1019;
	IMP_POINT_S stPos1, stPos2;
	for (s32I = 0; s32I < s32DL; s32I++)
	{	
		//get position along direction
		stPos1.s16X = 0; stPos1.s16Y = 0;
		stPos2.s16X = 0; stPos2.s16Y = 0;
		impGetPosAlongDir(12, 6, 2, &stPos1, &stPos2);
	}
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("impGetPosAlongDir_U8(%d):%.1f ms\n", s32DL, (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
	return 0;
#endif
	
	impUpdateSBkg(hModule);
	
#if 1
	if (1 && pstModule->s32First)
	{
		pstModule->s32First = 0;
		impRule2Inner(pstModule);
		impCreateRObjects(pstModule);
	}

#if OSCD_DBG_SHW_TIME	
	gettimeofday(&t1, NULL);
#endif

	impRMVDetect(pstModule);

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("impRMVDetect:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
#endif

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif

//	impProcessOSCD0(pstModule);

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("ProcessOSCD:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif	
//	printf("***pstModule->pstRule->stZones.astZone[0].stPara.stOsc.stOscPara.s32TimeMin:%d\n", pstModule->pstRule->stZones.astZone[0].stPara.stOsc.stOscPara.s32TimeMin);
	
	impOutPutResult(pstModule);
	
	memcpy(pu8PreGray, pu8InGray, s32Width * s32Height); //save current image as preimage
	
	return 0;
}


IMP_S32 impOSCD_Clear(IMP_OSCD_S *pstModule)
{
	PEA_RESULT_S *pstResult = pstModule->pstResult; //系统公共数据
	
	IMP_GrayImageClear(&pstModule->stImgFgStatic);
	IMP_GrayImageClear(&pstModule->stImgFgStatic2);
	IMP_GrayImageClear(&pstModule->stImgSBkg);
	IMP_GrayImageClear(&pstModule->stImgPreGray);
	
	IMP_GrayImageClear(&pstModule->stImgSObjSign);
	IMP_GrayImageClear(&pstModule->stImgSObjEdge);
	
	IMP_GrayImageClear(&pstModule->stImgRObjSign);
	IMP_GrayImageClear(&pstModule->stImgRObjEdge);
	
	IMP_GrayImageClear(&pstModule->stImgBkgEdge);
	IMP_GrayImageClear(&pstModule->stImgGrayEdge);
	
	memset(&pstModule->stSObjSet, 0, sizeof(IMP_SOBJ_SET_S));
	
	return 0;
}


IMP_S32 impRule2Inner(IMP_OSCD_S *pstModule)
{
	IMP_S32 s32ZI, s32OI;
	IMP_S32 s32RMVNum;
	OSCD_REGION_S *pstRMVRgns = pstModule->astRMVRgns;
	RULE_S *pstRule = pstModule->pstRule;
	
	memset(pstRMVRgns, 0, sizeof(OSCD_REGION_S) * OSCD_R_MAX_NUM);
	s32RMVNum = 0;
	for (s32ZI = 0; s32ZI < IMP_MAX_NUM_RULE_ZONE; s32ZI++)
	{
		RULE_ZONE_S *pstZone = &pstRule->stZones.astZone[s32ZI];
		
		if (!pstZone->u32Valid || !pstZone->u32Enable || !(pstZone->u32Mode & IMP_FUNC_OSC_R)) continue;
		
		for (s32OI = 0; s32OI < IMP_MAX_OSC_NUM; s32OI++)
		{
			OSC_OBJ_LMT_SPECL_REGIONS_S *pstOscRgn = &pstZone->stPara.stOsc.astSpclRgs[s32OI];
			if (pstOscRgn->s32Valid && s32RMVNum < OSCD_R_MAX_NUM)
			{
				OSCD_REGION_S *pstRMVRgn = &pstRMVRgns[s32RMVNum];
				
				pstRMVRgn->s32Valid = 1;
				pstRMVRgn->stRgn = pstOscRgn->stOscRg;
				
				s32RMVNum++;
			}
		}
	}
	
	return 0;
}


IMP_S32 impUpdateSBkg(IMP_MODULE_HANDLE hModule)
{
	IMP_S32 s32RI, s32CI;
	IMP_S32 s32Oft;
	IMP_OSCD_S *pstModule = (IMP_OSCD_S*)hModule;
	PEA_RESULT_S *pstResult = pstModule->pstResult; //系统公共数据
	IMP_U8 *pu8SBkg = 0, *pu8InGray = 0;

	GRAY_IMAGE_S *pstInGray = &pstResult->stImgInGray;
	IMP_S32 s32Width = pstResult->s32Width, s32Height = pstResult->s32Height;
	
	pu8InGray = pstInGray->pu8Data;
	pu8SBkg = pstModule->stImgSBkg.pu8Data;
	
	if (pstResult->u32FrmTimeCur == 0)
	{
		memcpy(pu8SBkg, pu8InGray, s32Width * s32Height);
		return 0;
	}
	
	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			
			if (pstModule->s32FrmNum % 2 == 0)
			{
				pu8SBkg[s32Oft] = (pu8SBkg[s32Oft] * (128 - 32) + pu8InGray[s32Oft] * 32) >> 7;
			}
		}
	}
	
#if OSCD_DBG_SHW	
	ipShowGrayImage(s32Width, s32Height, pu8SBkg, "s_bkg");
#endif		
	
	return 0;
}


//two backgrounds
IMP_S32 impProcessOSCD0(IMP_MODULE_HANDLE hModule)
{
	IMP_S32 s32RI, s32CI, s32PI;
	IMP_S32 s32Oft, s32Oft2, s32R;
	IMP_OSCD_S *pstModule = (IMP_OSCD_S*)hModule;
	IMP_U8 *pu8InGray = 0, *pu8PreGray = 0, *pu8Fg = 0, *pu8Bkg = 0, *pu8SBkg = 0;
	IMP_U8 *pu8Static = 0, *pu8Static2 = 0, *pu8SObjSign = 0;
	IMP_U8 u8GrayVal;
	GRAY_IMAGE_S *pstImgBg;
	IMP_U8 u8Bkg, u8Diff;
	IMP_S32 s32MinSize, s32MaxSize, s32AlertTime;
	
    GA_HARDWARE_RS_S  *pstHwResource = pstModule->pstHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult = pstModule->pstResult; //系统公共数据
	
	PEA_DETECTED_REGIONSET_S *pstDRS_OSC = &pstResult->stDRegionSetOsc;
	IpTrackedTargetSet *pstTTS_OSC = &pstResult->stTrackedTargetSetOsc;
	
	GRAY_IMAGE_S *pstInGray = &pstResult->stImgInGray;
	IMP_S32 s32Width = pstResult->s32Width, s32Height = pstResult->s32Height;

#if OSCD_DBG_SHW_TIME
	struct timeval t1, t2;
#endif
	
	s32MinSize = pstModule->s32MinSize;
	s32MaxSize = pstModule->s32MaxSize;
	s32AlertTime = pstModule->s32AlertTime;
	
	s32R = pstResult->s32Noise;
	
	pu8InGray = pstInGray->pu8Data;
	pu8Bkg = pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	pu8Fg = pstResult->stDRegionSet.pstImgFgRgn->pu8Data;
	pu8PreGray = pstModule->stImgPreGray.pu8Data;
	pu8Static = pstModule->stImgFgStatic.pu8Data;
	pu8Static2 = pstModule->stImgFgStatic2.pu8Data;
	pu8SObjSign = pstModule->stImgSObjSign.pu8Data;
	pu8SBkg = pstModule->stImgSBkg.pu8Data;
	
	if (pstResult->u32FrmTimeCur == 0)
	{
		memcpy(pu8PreGray, pu8InGray, s32Width * s32Height);
		return 0;
	}
	
	
#if OSCD_DBG_SHW	
	ipShowGrayImage(s32Width, s32Height, pstModule->stImgBkgEdge.pu8Data, "bkg_edge");
	ipShowGrayImage(s32Width, s32Height, pstModule->stImgGrayEdge.pu8Data, "gray_edge");
#endif
	
//	printf("s32R:%d\n", s32R);
//	s32R = 10;
	//get conour of object
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif

	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
						
			u8Diff = abs(pu8InGray[s32Oft] - pu8SBkg[s32Oft]);
			
			if (pu8Fg[s32Oft] && u8Diff < s32R * 2)
			{
				if (pu8Static[s32Oft] < 32) 
				{
					pu8Static[s32Oft]++;
				}
			}
			
			if (!pu8Fg[s32Oft] || u8Diff > s32R)
			{
				pu8Static[s32Oft] = pu8Static[s32Oft] - 1 < 0 ? 0 : pu8Static[s32Oft] - 1; //decreasing quickly
			}
		}
	}

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("ProcessOSCD_1:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
	


#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	
	memset(pu8Static2, 0, s32Width * s32Height);
	for (s32RI = 6; s32RI < s32Height - 6; s32RI++)
	{
		for (s32CI = 6; s32CI < s32Width - 6; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			if (pu8Static[s32Oft] > 16) //16)
			{
				pu8Static2[s32Oft] = 255;
			}
		}
	}
	
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("ProcessOSCD_2:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
//	ipShowBinImageLargerVal(s32Width, s32Height, pu8Static, 16, "static");
	
#if 0
	//debug for speed
	memset(pu8Static2, 0, s32Width * s32Height);
	for (s32RI = 6; s32RI < s32Height/2 - 6; s32RI++)
	{
		for (s32CI = 6; s32CI < s32Width/2 - 6; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			pu8Static2[s32Oft] = 255;
		}
	}
	
	for (s32RI = 6; s32RI < s32Height/2 - 6; s32RI++)
	{
		for (s32CI = s32Width/2; s32CI < s32Width - 6; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			pu8Static2[s32Oft] = 255;
		}
	}
	
	for (s32RI = s32Height/2; s32RI < s32Height - 6; s32RI++)
	{
		for (s32CI = 6; s32CI < s32Width/2 - 6; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			pu8Static2[s32Oft] = 255;
		}
	}
	
	for (s32RI = s32Height/2; s32RI < s32Height - 6; s32RI++)
	{
		for (s32CI = s32Width/2; s32CI < s32Width - 6; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			pu8Static2[s32Oft] = 255;
		}
	}
#endif

	PEA_RGE_LABLE_PARA_S stPara;
	PEA_DETECTED_REGIONSET_S *pstRgs = &pstModule->stRgs;
	IMP_RECT_S stRc;
	
	stRc.s16X1 = 1;
	stRc.s16Y1 = 1;
	stRc.s16X2 = s32Width - 1;
	stRc.s16Y2 = s32Height - 1;
	
	stPara.s32ThDrgMinSizeWh = 5;
	stPara.s32ThDrgMinSizePixelInit = 4;
	stPara.s32ThDrgMinSizePixelMax = 32;
	stPara.s32ThDrgMinSizePixelStep = 4;
	stPara.s32ThDrgMinSizePixelCur = 4;
	stPara.s32ThDrgMaxSizeRatio = 1; //2=50%
	
	memset(pstRgs, 0, sizeof(PEA_DETECTED_REGIONSET_S));
	ipDetectedRegionSetClear(pstRgs);
	pstRgs->pstImgFgRgn = &pstModule->stImgFgStatic2;
	IMP_PEA_RGE_ExtractRegionSetFromFgImage(&stPara, pstRgs, &stRc, 0, 0);
	

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif

	//compare static area sign image with static object sign image
	impCmp_SArea_SOBJSign(pstModule);
	
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("ProcessOSCD_3:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif

#if OSCD_DBG_SHW_TIME	
	gettimeofday(&t1, NULL);
#endif
	
	IMP_S32 s32ObjNum = 0;
	for (s32RI = 0; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
	{
	//	IMP_STATIC_OBJ_S *pstSObj = impGetSObjBySign(pstModule->stSObjSet.aSObjs, s32RI);
		IMP_STATIC_OBJ_S *pstSObj = &pstModule->stSObjSet.aSObjs[s32RI];
		
		if (pstSObj->u8Used && pstSObj->u32EvtLife > 0)
		{
			pstSObj->u32EvtLife++;
		//	printf("++++++++++++++++++++++++++++++++%d:%d\n", pstSObj->u32TargetId, pstSObj->u32EvtLife);
			if (pstSObj->u32EvtLife > 32)
			{
				if (OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used))
					impDeleteObject(pstModule, pstSObj);
				else if (OSCD_ROBJ_IS_CANDIDATE(pstSObj->u8Used))
					impDeleteRObject(pstModule, pstSObj);
			}
		}
		
		if (OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used) && OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used))
		{
			impMatchSObject(pstModule, pstSObj);
			impUpdatePosition(pstSObj);
			pstSObj->u32Life++;
		}
		
		if (OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used))
		{
			s32ObjNum++;
		}
		
		//add life
	//	if (pstSObj)
	//	{
	//		pstSObj->u32Life++;
	//	}
	}
	printf("s32ObjNum:%d\n", s32ObjNum);
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("ProcessOSCD_4:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif

#if OSCD_DBG_SHW //	
	IMP_S32 s32BRI, s32BCI;
	IplImage *pstImgMark = cvCreateImage(cvSize(s32Width, s32Height), 8, 3);
	char abyText[100];
	CvFont font;
	IMP_STATIC_OBJ_S *pstSObj = 0;
    cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.35f,0.35f,0,1,CV_AA);
    
	for (s32RI = 0; s32RI < s32Height; s32RI++)
	{
		s32Oft = s32RI * pstImgMark->widthStep;
		s32Oft2 = s32RI * s32Width;
		for (s32CI = 0; s32CI < s32Width; s32CI++)
		{
			pstImgMark->imageData[s32Oft + s32CI * 3 + 0] = pu8InGray[s32Oft2 + s32CI];
			pstImgMark->imageData[s32Oft + s32CI * 3 + 1] = pu8InGray[s32Oft2 + s32CI];
			pstImgMark->imageData[s32Oft + s32CI * 3 + 2] = pu8InGray[s32Oft2 + s32CI];
		}
	}
	for (s32RI = 0; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
	{
	//	IMP_STATIC_OBJ_S *pstSObj = impGetSObjBySign(pstModule->stSObjSet.aSObjs, s32RI);
		pstSObj = &pstModule->stSObjSet.aSObjs[s32RI];
		
		if (pstSObj && OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used))
		{
			IMP_RECT_S stRect = pstSObj->stRect;
			cvRectangle(pstImgMark, cvPoint(stRect.s16X1, stRect.s16Y1), cvPoint(stRect.s16X2, stRect.s16Y2), CV_RGB(0, 0, 255), 1, 8, 0);
		}
		
		if (pstSObj && OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used))
		{
			IMP_RECT_S stRect = pstSObj->stRect;
			cvRectangle(pstImgMark, cvPoint(stRect.s16X1, stRect.s16Y1), cvPoint(stRect.s16X2, stRect.s16Y2), CV_RGB(0, 255, 0), 1, 8, 0);
		}
		
		if (pstSObj && OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used))
		{
			IMP_RECT_S stRect = pstSObj->stRect;
			sprintf(abyText, "%d,(%d,%d,%d)_%d", pstSObj->u8Sign, pstSObj->s32PosLen, pstSObj->u32Life, pstSObj->u32EvtLife, pstSObj->u32TargetId);
			cvPutText(pstImgMark, abyText, cvPoint((stRect.s16X1 + stRect.s16X2) / 2, (stRect.s16Y1 + stRect.s16Y2) / 2), &font, CV_RGB(0, 255, 0));
		}
	}
	cvShowImage("OBJ_INFO", pstImgMark);
	cvReleaseImage(&pstImgMark);
#endif //

	PEA_DETECTED_REGION_S *pstDrg = pstRgs->astDrg;
	for (s32RI = 0; s32RI < IMP_MAX_TGT_CNT; s32RI++)
	{
		if (IMP_DRG_IS_USED(pstDrg->u8Used) 
			&& pstDrg->s32AreaPixel > s32MinSize && pstDrg->s32AreaPixel < s32MaxSize)
		{
			IMP_RECT_S stRect = pstDrg->stRect;
			IMP_U8 u8Sign = pstDrg->u8Sign;
			IMP_S32 areaSize = pstDrg->s32AreaPixel;
		//	printf("id:%d, size:%d\n", u8Sign, areaSize);
		}
		pstDrg++;
	}
#if OSCD_DBG_SHW //monkey
//	ipShowGrayImage(s32Width, s32Height, pu8Static2, "static_count");

	
//	ipShowBinImageLargerVal(s32Width, s32Height, pu8SObjSign, 1, "obj_static");
	ipShowGrayImage(s32Width, s32Height, pstModule->stImgSObjEdge.pu8Data, "static_edge");
	
//	ipMorphCloseImage( &pstModule->stImgFg, &pstModule->stImgFg, &pstModule->stImgFg, 1 );
	
//	ipShowGrayImage(s32Width, s32Height, pu8PreGray, "pu8PreGray");
//	ipShowGrayImage(s32Width, s32Height, pu8InGray, "pu8InGray");
//	s32PI = 1;
//	cvMoveWindow("pu8InGray", (s32PI % 3) * s32Width, s32Height * 2 + (s32PI / 3) * s32Height / 2);

//	ipShowBinImage(s32Width, s32Height, pu8Static2, "OSC_Bin");
#endif //monkey
	
	pstModule->s32FrmNum++;
	if (pstModule->s32FrmNum > 1024) pstModule->s32FrmNum = 0;
	memcpy(pu8PreGray, pu8InGray, s32Width * s32Height);
	
	return 0;
}


//compare static area sign image with static object sign image
IMP_S32 impCmp_SArea_SOBJSign(IMP_OSCD_S *pstModule)
//IMP_S32 impCmp_SArea_SOBJSign(PEA_DETECTED_REGIONSET_S stRgs, GRAY_IMAGE_S *pstSAreaSign, IMP_SOBJ_SET_S *pstSObjSet, GRAY_IMAGE_S *pstSObjSign, IMP_U16 *pu16MatchMatrix, IMP_U32 *pu32IDCounter)
{
	IMP_S32 s32OI, s32RI, s32CI;
	IMP_U8 *pu8SObjSign = 0, *pu8SAreaSign = 0;
	IMP_S32 s32W, s32H;
	IMP_S32 s32Oft, s32Oft2;
	PEA_DETECTED_REGIONSET_S stRgs = pstModule->stRgs;
	GRAY_IMAGE_S *pstSAreaSign = &pstModule->stImgFgStatic2;
	IMP_SOBJ_SET_S *pstSObjSet = &pstModule->stSObjSet;
	GRAY_IMAGE_S *pstSObjSign = &pstModule->stImgSObjSign;
	IMP_U16 *pu16MatchMatrix = pstModule->au16MatchMatrix;
	
	PEA_DETECTED_REGION_S *pstDrg = stRgs.astDrg;

#if OSCD_DBG_SHW_TIME
	struct timeval t1, t2;
#endif
	
	
	
	pu8SObjSign = pstSObjSign->pu8Data;
	pu8SAreaSign = pstSAreaSign->pu8Data;
	s32W = pstSObjSign->s32W;
	s32H = pstSObjSign->s32H;
	
	
	//statistic match matrix between area and object
	/*
	for (s32OI = 0; s32OI < IMP_MAX_TGT_CNT; s32OI++)
	{
		if (IMP_DRG_IS_USED(pstDrg->u8Used))
		{
			IMP_RECT_S stRect = pstDrg->stRect;
			IMP_U8 u8Sign = pstDrg->u8Sign;
			for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
			{
				for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
				{
					s32Oft = s32RI * s32W + s32CI;
					if (pu8SAreaSign[s32Oft] == u8Sign)
					{
						if (pu8SObjSign[s32Oft])
						{
							pu16MatchMatrix[pu8SObjSign[s32Oft] * u8Sign]++;
						}
					}
				}
			}
		}
		pstDrg++;
	}
	*/
#if OSCD_DBG_SHW	
	ipShowBinImage(s32W, s32H, pu8SAreaSign, "SAreaSign_Bin");
	ipShowBinImage(s32W, s32H, pu8SObjSign, "SObjSign_Bin");
#endif

#if OSCD_DBG_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	
	memset(pu16MatchMatrix, 0, OSCD_MAX_TGT_CNT * IMP_MAX_TGT_CNT * 2);
	for (s32RI = 0; s32RI < s32H; s32RI++)
	{
		for (s32CI = 0; s32CI < s32W; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8SAreaSign[s32Oft] && pu8SObjSign[s32Oft])
			{
				s32Oft2 = pu8SObjSign[s32Oft] * IMP_MAX_TGT_CNT + pu8SAreaSign[s32Oft];
				pu16MatchMatrix[s32Oft2] = pu16MatchMatrix[s32Oft2] > 0x7FFE ? 0x7FFE : pu16MatchMatrix[s32Oft2] + 1;
			}
		}
	}
/*	
	for (s32Oft = 0; s32Oft < s32W * s32H; s32Oft++)
	{
		if (pu8SObjSign[s32Oft] == 14)
		{
			printf("14Error!!!!!!!!!!!");
		}
	}
*/
#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impCmp_SArea_SOBJSign_1:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
		
#if 0
	for (s32RI = 0; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
	{
		for (s32CI = 0; s32CI < IMP_MAX_TGT_CNT; s32CI++)
		{
			s32Oft = s32RI * IMP_MAX_TGT_CNT + s32CI;
			if (pu16MatchMatrix[s32Oft] > 0)
				printf("(%d,%d):%d,",s32RI, s32CI, pu16MatchMatrix[s32Oft]);
		//	else
			//	printf(" ,");
				
		}
	//	printf("\n");
	}
	printf("\n");
#endif

#if OSCD_DBG_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	//replace the old object
	for (s32CI = 0; s32CI < IMP_MAX_TGT_CNT; s32CI++)
	{
		IMP_STATIC_OBJ_S *pstSObj = 0, *pstSObj2 = 0, *pstSObjMax = 0;
		IMP_U8 u8Done = 0;
		IMP_S32 s32MaxRI = 0, s32MaxScore = 0;
		
		for (s32RI = 1; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
		{
			s32Oft = s32RI * IMP_MAX_TGT_CNT + s32CI;
			
		//	pstSObj = impGetSObjBySign(pstSObjSet->aSObjs, s32RI);
			pstSObj = &pstSObjSet->aSObjs[s32RI];
			
			//如果区域与真OSC目标相交,那么就忽略此区域的处理.
			if (OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used) && pu16MatchMatrix[s32Oft] > 0)
			{
				IMP_S32 s32OPI;
				u8Done = 1;
				//删除其它配对目标
				for (s32OPI = 1; s32OPI < OSCD_MAX_TGT_CNT; s32OPI++)
				{
					s32Oft2 = s32OPI * IMP_MAX_TGT_CNT + s32CI;
				//	pstSObj = impGetSObjBySign(pstSObjSet->aSObjs, s32OPI);
					pstSObj2 = &pstSObjSet->aSObjs[s32OPI];
					if (pu16MatchMatrix[s32Oft2] > 0 && OSCD_SOBJ_IS_CANDIDATE(pstSObj2->u8Used) && !OSCD_SOBJ_IS_REALOBJ(pstSObj2->u8Used)) 
					{
					//	printf("DEL_A:");
						impDeleteObject(pstModule, pstSObj2);
					}
				}
				break;
			}
			
			if (OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used) //验证目标是否还存在,防止目标在前面被删掉了
			&& s32MaxScore < pu16MatchMatrix[s32Oft])
			{
				s32MaxScore = pu16MatchMatrix[s32Oft];
				s32MaxRI = s32RI;
				pstSObjMax = pstSObj;
			//	if (pstSObj->u32TargetId == 0) 
			//	{
			//		printf("u8Done:%d\n", u8Done);
			//		exit(0);
			//	}
			}

		}

		if (u8Done) continue;
		
		if (s32MaxScore > 0)
		{
		//	printf("s32MaxScore:%d\n", s32MaxScore);
			s32Oft = s32MaxRI * IMP_MAX_TGT_CNT + s32CI;
		//	pstSObj = impGetSObjBySign(pstSObjSet->aSObjs, s32MaxRI);
			pstSObj = pstSObjMax;
		//	printf("CRT_ID3:%d, %d, %d\n", pstSObjSet->aSObjs[1].u32TargetId, pstSObjSet->aSObjs[1].u8Used, pstSObjSet->aSObjs[1].u8Sign);
		//	printf("s32MaxScore:%d, s32MaxRI:%d, %x\n", s32MaxScore, s32MaxRI, pstSObj);
			if (pstSObj && !OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used))// && pu16MatchMatrix[s32Oft] * 100 > pstSObj->u32AreaSize * 95)
			{
				PEA_DETECTED_REGION_S *pstRgn = impGetRgnBySign(stRgs.astDrg, s32CI);
#if OSCD_DBG_SHW				
				printf("RPL_ID:%d\n", pstSObj->u32TargetId);
#endif
			//	printf("RPL_ID:%d; %d, score:%d\n", pstSObj->u32TargetId, s32MaxRI, s32MaxScore);
				impReplaceObject(pstModule, pstRgn, pstSObj);
				impCheckRealSObj(pstSObj);
			}
			
			//删除其它配对目标
			for (s32RI = 1; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
			{
				s32Oft = s32RI * IMP_MAX_TGT_CNT + s32CI;
			//	pstSObj = impGetSObjBySign(pstSObjSet->aSObjs, s32RI);
				pstSObj = &pstSObjSet->aSObjs[s32RI];
				if (pu16MatchMatrix[s32Oft] > 0 && OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used) && s32RI != s32MaxRI && !OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used)) 
				{
				//	printf("RI:%d, CI:%d\n", s32RI, s32CI);
				//	printf("DEL_B: ");
					impDeleteObject(pstModule, pstSObj);
				}
			}
		}

	}

#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impCmp_SArea_SOBJSign_2:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif

#if OSCD_DBG_SHW_TIME
gettimeofday(&t1, NULL);
#endif

	//check if there is new object
	for (s32CI = 0; s32CI < IMP_MAX_TGT_CNT; s32CI++)
	{
		PEA_DETECTED_REGION_S *pstRgn0 = impGetRgnBySign(stRgs.astDrg, s32CI);
	//	PEA_DETECTED_REGION_S *pstRgn0 = &stRgs.astDrg[s32CI];
		if (pstRgn0 && s32CI == pstRgn0->u8Sign)
		{
			for (s32RI = 1; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
			{
				s32Oft = s32RI * IMP_MAX_TGT_CNT + s32CI;
				if (pu16MatchMatrix[s32Oft] > 0)
				{
					break;
				}
			}
			
			//has new static area, create new object
			if (s32RI == OSCD_MAX_TGT_CNT && pstRgn0->s32AreaPixel > 10)
			{
				IMP_STATIC_OBJ_S *pstObj = impGetEmptyObj(pstSObjSet->aSObjs, &pstModule->u32IDCounter);
				//create object
				if (pstObj)
				{
				//	pstModule->u32ObjNumber++;
					impCreateObject(pstModule, pstRgn0, pstObj);
				//	printf("CRT_ID:%d, %d, %d\n", pstObj->u32TargetId, pstObj->u8Used, pstObj->u8Sign);
				}
			}
		}
	}

#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impCmp_SArea_SOBJSign_3:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif

#if OSCD_DBG_SHW_TIME
gettimeofday(&t1, NULL);
#endif

	//check if there is unstable object
	memset(pu16MatchMatrix, 0, OSCD_MAX_TGT_CNT * IMP_MAX_TGT_CNT * 2);
	for (s32RI = 0; s32RI < s32H; s32RI++)
	{
		for (s32CI = 0; s32CI < s32W; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8SAreaSign[s32Oft] && pu8SObjSign[s32Oft])
			{
				s32Oft2 = pu8SObjSign[s32Oft] * IMP_MAX_TGT_CNT + pu8SAreaSign[s32Oft];
				pu16MatchMatrix[s32Oft2] = pu16MatchMatrix[s32Oft2] > 0x7FFE ? 0x7FFE : pu16MatchMatrix[s32Oft2] + 1;
			}
		}
	}

#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impCmp_SArea_SOBJSign_4:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);	
#endif

#if 0
	for (s32RI = 0; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
	{
		for (s32CI = 0; s32CI < IMP_MAX_TGT_CNT; s32CI++)
		{
			s32Oft = s32RI * IMP_MAX_TGT_CNT + s32CI;
			if (pu16MatchMatrix[s32Oft] > 0)
				printf("%d,",pu16MatchMatrix[s32Oft]);
			else
				printf(" ,");
				
		}
		printf("\n");
	}
#endif

#if OSCD_DBG_SHW_TIME
gettimeofday(&t1, NULL);
#endif

	for (s32RI = 1; s32RI < OSCD_MAX_TGT_CNT; s32RI++)
	{
	//	IMP_STATIC_OBJ_S *pstSObj = impGetSObjBySign(pstSObjSet->aSObjs, s32RI);
		IMP_STATIC_OBJ_S *pstSObj = &pstSObjSet->aSObjs[s32RI];
		if (OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used))// && pstObj->s32PosLen > 1)
		{
			for (s32CI = 0; s32CI < IMP_MAX_TGT_CNT; s32CI++)
			{
				s32Oft = s32RI * IMP_MAX_TGT_CNT + s32CI;
				if (pu16MatchMatrix[s32Oft] > 10)
				{
					break;
				}
			}
			
			//no match area with object
			if (s32CI == IMP_MAX_TGT_CNT && !OSCD_SOBJ_IS_REALOBJ(pstSObj->u8Used))
			{
				//delete object
			//	printf("DEL_ID:%d\n", pstObj->u32TargetId);
			//	printf("DEL_C:(%d,%d)_ ", s32RI, pstSObj->u8Sign);
				impDeleteObject(pstModule, pstSObj);
			}
		}
	}
#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impCmp_SArea_SOBJSign_5:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);	
#endif
	
	return 0;
}


//find wanted region by u8Sign
PEA_DETECTED_REGION_S *impGetRgnBySign(PEA_DETECTED_REGION_S *pstDrg, IMP_U8 u8Sign)
{
	IMP_S32 s32PI;
	
	for (s32PI = 0; s32PI < IMP_MAX_TGT_CNT; s32PI++)
	{
		if (pstDrg->u8Used && u8Sign == pstDrg->u8Sign)
		{
			return pstDrg;
		}
		pstDrg++;
	}
	
	return 0;
}


//find wanted static object by u8Sign
IMP_STATIC_OBJ_S *impGetSObjBySign(IMP_STATIC_OBJ_S *pstSObj, IMP_U8 u8Sign)
{
	IMP_S32 s32PI;
	
	for (s32PI = 0; s32PI < OSCD_MAX_TGT_CNT; s32PI++)
	{
		if (OSCD_SOBJ_IS_CANDIDATE(pstSObj->u8Used) && u8Sign == pstSObj->u8Sign)
		{
			return pstSObj;
		}
		pstSObj++;
	}
	
	return 0;
}


//get empty object
IMP_STATIC_OBJ_S *impGetEmptyObj(IMP_STATIC_OBJ_S *pSObjs, IMP_U32 *pu32IDCounter)
{
	IMP_S32 s32PI;
	
	pSObjs++;
	for (s32PI = 1; s32PI < OSCD_MAX_TGT_CNT; s32PI++)
	{
		if (!pSObjs->u8Used)
		{
			memset(pSObjs, 0, sizeof(IMP_STATIC_OBJ_S));
			if ((*pu32IDCounter) < 0x7FFFFFFF) (*pu32IDCounter)++;
			else (*pu32IDCounter) = 0x70000000;
			pSObjs->u8Sign = (IMP_U8)(s32PI);
			pSObjs->u32TargetId = *pu32IDCounter;
		//	printf("+++++++++++++++Sign_%d:%d\n", pSObjs->u32TargetId, pSObjs->u8Sign);
			return pSObjs;
		}
		pSObjs++;
	}
	
	return 0;
}


//update the static object's position and type
IMP_S32 impUpdatePosition(IMP_STATIC_OBJ_S *pstObj);


//check motion type
IMP_SOBJ_TYPE_S impCheckMotion(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj);

//create object
IMP_S32 impCreateObject(IMP_OSCD_S *pstModule, PEA_DETECTED_REGION_S *pstRgn, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8Gray = 0, *pu8GrayEdge = 0, *pu8FgStatic = 0, *pu8ObjSign = 0, *pu8ObjEdge = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult; 
	
	GRAY_IMAGE_S *pstImgGray = 0, *pstImgGrayEdge = 0, *pstImgFgStatic = 0, *pstImgSObjSign = 0, *pstImgSObjEdge = 0;
	
//	printf("\033[33mCRT_%d:%d\033[0m\n", pstObj->u32TargetId, pstObj->s32PosLen);
	
	pstImgGray = &pstResult->stImgInGray;
	pstImgFgStatic = &pstModule->stImgFgStatic2;
	pstImgGrayEdge = &pstModule->stImgGrayEdge;
	pstImgSObjSign = &pstModule->stImgSObjSign;
	pstImgSObjEdge = &pstModule->stImgSObjEdge;
	
	s32W = pstImgGray->s32W;
	s32H = pstImgGray->s32H;
	
	pu8Gray = pstImgGray->pu8Data;
	pu8FgStatic = pstImgFgStatic->pu8Data;
	pu8GrayEdge = pstImgGrayEdge->pu8Data;
	pu8ObjSign = pstImgSObjSign->pu8Data;
	pu8ObjEdge = pstImgSObjEdge->pu8Data;
	
	stRect = pstRgn->stRect;
	u8Sign = pstRgn->u8Sign;
	
	u8ObjSign = pstObj->u8Sign;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8FgStatic[s32Oft] == u8Sign)
			{
				pu8ObjSign[s32Oft] = u8ObjSign;
				pu8ObjEdge[s32Oft] = pu8GrayEdge[s32Oft];
				pu8ObjEdge[s32Oft - 1] = pu8GrayEdge[s32Oft - 1];
				pu8ObjEdge[s32Oft + 1] = pu8GrayEdge[s32Oft + 1];
			}
		}
	}
	
	pstObj->stRect = stRect;
	pstObj->u32AreaSize = pstRgn->s32AreaPixel;
	pstObj->u32MatchScore = 100;
	stPos.s16X = (stRect.s16X1 + stRect.s16X2) / 2;
	stPos.s16Y = (stRect.s16Y1 + stRect.s16Y2) / 2;
	pstObj->stPos = stPos;
	pstObj->stSOBJType = impCheckMotion(pstModule, pstObj);
	impUpdatePosition(pstObj);
	
	OSCD_SOBJ_SET_CANDIDATE(pstObj->u8Used);
	
//	if (impCheckMotion(pstModule, pstObj) == 2)
//	{
//		impDeleteObject(pstModule, pstObj);
//	}
	
	
	return 0;
}


//update the static object's position and type
IMP_S32 impUpdatePosition(IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32PI;
	IMP_POINT_S *pstPos = 0;
	IMP_SOBJ_TYPE_S *penType = 0;
	IMP_U16 *pu16MS = 0, *pu16AS = 0;
	
	pstPos = pstObj->astPos;
	penType = pstObj->astSOBJT;
	pu16MS = pstObj->au16MatchScore;
	pu16AS = pstObj->au16AreaSize;
	
	if (pstObj->s32PosLen == OSCD_SOBJ_ELEM_LEN)
	{
		for (s32PI = 0; s32PI < OSCD_SOBJ_ELEM_LEN - 1; s32PI++)
		{
			pstPos[s32PI] = pstPos[s32PI + 1];
			penType[s32PI] = penType[s32PI + 1];
			pu16MS[s32PI] = pu16MS[s32PI + 1];
			pu16AS[s32PI] = pu16AS[s32PI + 1];
		}
		pstPos[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->stPos;
		penType[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->stSOBJType;
		pu16AS[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->u32AreaSize;
		pu16MS[OSCD_SOBJ_ELEM_LEN - 1] = pstObj->u32MatchScore;
	}
	else
	{
		pstPos[pstObj->s32PosLen] = pstObj->stPos;
		penType[pstObj->s32PosLen] = pstObj->stSOBJType;
		pu16MS[pstObj->s32PosLen] = pstObj->u32MatchScore;
		pu16AS[pstObj->s32PosLen] = pstObj->u32AreaSize;
		pstObj->s32PosLen++;
	}
	
	
	return 0;
}


//create object
IMP_S32 impReplaceObject(IMP_OSCD_S *pstModule, PEA_DETECTED_REGION_S *pstRgn, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8Gray = 0, *pu8GrayEdge = 0, *pu8FgStatic = 0, *pu8ObjSign = 0, *pu8ObjEdge = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft, s32OftR;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	
	GRAY_IMAGE_S *pstImgGray = 0, *pstImgGrayEdge = 0, *pstImgFgStatic = 0, *pstImgSObjSign = 0, *pstImgSObjEdge = 0;
	
#if OSCD_DBG_SHW_TIME	
	struct timeval t1, t2;
#endif


	
	pstImgGray = &pstResult->stImgInGray;
	pstImgFgStatic = &pstModule->stImgFgStatic2;
	pstImgGrayEdge = &pstModule->stImgGrayEdge;
	pstImgSObjSign = &pstModule->stImgSObjSign;
	pstImgSObjEdge = &pstModule->stImgSObjEdge;
	
	s32W = pstImgGray->s32W;
	s32H = pstImgGray->s32H;
	
	pu8Gray = pstImgGray->pu8Data;
	pu8FgStatic = pstImgFgStatic->pu8Data;
	pu8GrayEdge = pstImgGrayEdge->pu8Data;
	pu8ObjSign = pstImgSObjSign->pu8Data;
	pu8ObjEdge = pstImgSObjEdge->pu8Data;

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	//clean old object area
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		s32OftR = s32RI * s32W;
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32OftR + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				pu8ObjSign[s32Oft] = 0;
				pu8ObjEdge[s32Oft] = 0;
				pu8ObjEdge[s32Oft - 1] = 0;
				pu8ObjEdge[s32Oft + 1] = 0;
			}
		}
	}
#if 0	
#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impReplaceObject_1:%.1f ms\n", (t2.tv_usec - t1.tv_usec) / 1000.f);	
#endif		
	
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
#endif
	//set new object area
	stRect = pstRgn->stRect;
	u8Sign = pstRgn->u8Sign;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		s32OftR = s32RI * s32W;
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32OftR + s32CI;
			if (pu8FgStatic[s32Oft] == u8Sign)
			{
				pu8ObjSign[s32Oft] = u8ObjSign;
				pu8ObjEdge[s32Oft] = pu8GrayEdge[s32Oft];
				pu8ObjEdge[s32Oft - 1] = pu8GrayEdge[s32Oft - 1];
				pu8ObjEdge[s32Oft + 1] = pu8GrayEdge[s32Oft + 1];
			}
		}
	}
	
#if OSCD_DBG_SHW_TIME
gettimeofday(&t2, NULL);
printf("impReplaceObject(%d)_2:%.1f ms\n", pstRgn->s32AreaPixel, (t2.tv_usec - t1.tv_usec) / 1000.f);	
#endif
	
	u8ObjSign = pstObj->u8Sign;
	pstObj->stRect = stRect;
	stPos.s16X = (stRect.s16X1 + stRect.s16X2) / 2;
	stPos.s16Y = (stRect.s16Y1 + stRect.s16Y2) / 2;
	pstObj->u32AreaSize = pstRgn->s32AreaPixel;
	pstObj->stPos = stPos;
	impMatchSObject(pstModule, pstObj);
//	pstObj->u32MatchScore = 100;
	pstObj->stSOBJType = impCheckMotion(pstModule, pstObj);
	impUpdatePosition(pstObj);
#define NEED_POS_LEN 5
	if (pstObj->s32PosLen > NEED_POS_LEN)
	{
		IMP_S32 s32PI, s32STPNum = 0, s32RMVNum = 0;
		
		for (s32PI = pstObj->s32PosLen - 1; s32PI >= pstObj->s32PosLen - NEED_POS_LEN; s32PI--)
		{
			if (pstObj->astSOBJT[s32PI] == SOBJT_STP) s32STPNum++;
			if (pstObj->astSOBJT[s32PI] == SOBJT_RMV) s32RMVNum++;
		}
		
#if OSCD_DBG_SHW		
		printf("ID_%d: MatchS:%d, Stay:%d, Lost:%d\n", pstObj->u32TargetId, pstObj->u32MatchScore, s32STPNum, s32RMVNum);
#endif	
	
		//only keep the stay object
		if (s32RMVNum * 4 >= s32STPNum || s32STPNum * 100 < NEED_POS_LEN * 90)
		{
			impDeleteObject(pstModule, pstObj);
		}
	}
	

//	if (impCheckMotion(pstModule, pstObj) == 2)
//	{
//		impDeleteObject(pstModule, pstObj);
//	}
	
	return 0;
}


//delete object
IMP_S32 impDeleteObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8Gray = 0, *pu8FgStatic = 0, *pu8ObjSign = 0, *pu8ObjEdge = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult; 
	
	GRAY_IMAGE_S *pstImgGray = 0, *pstImgFgStatic = 0, *pstImgSObjSign = 0, *pstImgSObjEdge = 0;
	
	if (!OSCD_SOBJ_IS_CANDIDATE(pstObj->u8Used)) return -1;
	
#if OSCD_DBG_SHW
	if (pstObj->u32TargetId == 316)
	{
		printf("hi316\n");
	}
	printf("\033[31mDEL(%d)_%d:%d\033[0m\n", pstObj->u8Sign, pstObj->u32TargetId, pstObj->s32PosLen);
#endif

	pstImgGray = &pstResult->stImgInGray;
	pstImgFgStatic = &pstModule->stImgFgStatic2;
	pstImgSObjSign = &pstModule->stImgSObjSign;
	pstImgSObjEdge = &pstModule->stImgSObjEdge;
	
	s32W = pstImgGray->s32W;
	s32H = pstImgGray->s32H;
	
	pu8Gray = pstImgGray->pu8Data;
	pu8FgStatic = pstImgFgStatic->pu8Data;
	pu8ObjSign = pstImgSObjSign->pu8Data;
	pu8ObjEdge = pstImgSObjEdge->pu8Data;
	
	//clean old object area
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI++)
	{
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI++)
		{
			s32Oft = s32RI * s32W + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				pu8ObjSign[s32Oft] = 0;
				pu8ObjEdge[s32Oft] = 0;
				pu8ObjEdge[s32Oft - 1] = 0;
				pu8ObjEdge[s32Oft + 1] = 0;
			}
		}
	}
	
//	if (pstObj->u8Sign == 14)
//	{
//		for (s32Oft = 0; s32Oft < s32W * s32H; s32Oft++)
//		{
//			if (pu8ObjSign[s32Oft] == 14)
//			{
//				printf("14Error!!!!!!!!!!!!!\n");
//			}
//		}
//	}
//	if (pstObj->u8Sign == 14) exit(0);
	
	memset(pstObj, 0, sizeof(IMP_STATIC_OBJ_S));
	

	
	return 0;
}


//check if the current static object is real object
IMP_S32 impCheckRealSObj(IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32PI;
	IMP_POINT_S *pstPos = 0;
	IMP_U16 *pu16MatchScore = 0, *pu16AreaSize = 0;
	IMP_S32 s32PosLen, s32MeanPosX, s32MeanPosY, s32VarPosX, s32VarPosY;
	IMP_S32 s32MatchTime, s32MeanMT, s32MeanAT, s32VarAT;
	IMP_S32 s32FPS = 8; //16;
	IMP_S32 s32PosThrd = 0, s32MSThrd = 0, s32ASThrd = 0;
	
	if (OSCD_SOBJ_IS_REALOBJ(pstObj->u8Used)) return 0;
	
	pstPos = pstObj->astPos;
	pu16MatchScore = pstObj->au16MatchScore;
	pu16AreaSize = pstObj->au16AreaSize;
	s32PosLen = pstObj->s32PosLen;
	s32MatchTime = pstObj->s32PosLen;
	
	//OSCD_SOBJ_ELEM_LEN
	if (s32PosLen < s32FPS) return -1;
	
	//assume the frame rate is 25fps
	s32MeanPosX = 0; s32MeanPosY = 0;
	s32MeanAT = 0;
	s32MeanMT = 0;
	for (s32PI = s32PosLen - 1; s32PI >= s32PosLen - s32FPS; s32PI--)
	{
		s32MeanPosX += pstPos[s32PI].s16X;
		s32MeanPosY += pstPos[s32PI].s16Y;
		s32MeanAT += pu16AreaSize[s32PI];
		s32MeanMT += pu16MatchScore[s32PI];
	}
	s32MeanPosX /= s32FPS;
	s32MeanPosY /= s32FPS;
	s32MeanAT /= s32FPS;
	s32MeanMT /= s32FPS;
	
	s32VarPosX = 0; s32VarPosY = 0;
	s32VarAT = 0;
	for (s32PI = s32PosLen - 1; s32PI >= s32PosLen - s32FPS; s32PI--)
	{
		s32VarPosX += abs(s32MeanPosX - pstPos[s32PI].s16X) << 2;
		s32VarPosY += abs(s32MeanPosY - pstPos[s32PI].s16Y) << 2;
		s32VarAT += abs(s32MeanAT - pu16AreaSize[s32PI]) << 2;
	}
	s32VarPosX /= s32FPS;
	s32VarPosY /= s32FPS;
	s32VarAT /= s32FPS;
	
	s32PosThrd = 2 << 2;
	s32ASThrd = (pstObj->u32AreaSize / 40) << 2;

#if OSCD_DBG_SHW	
	printf("s32ASThrd:%d\n", s32ASThrd);
#endif

	if (s32VarPosX < s32PosThrd && s32VarPosY < s32PosThrd && s32VarAT < s32ASThrd && s32MeanMT > 60)
	{
#if OSCD_DBG_SHW
		printf("\033[32mREAL_%d: hist:%d (%d,%d,vat:%d,mt:%d)\033[0m\n", pstObj->u32TargetId, pstObj->s32PosLen, s32VarPosX, s32VarPosY, s32VarAT, s32MeanMT);
#endif

		pstObj->stSOBJType = SOBJT_STP; //the real object must be stay(stop) type.
		OSCD_SOBJ_SET_REALOBJ(pstObj->u8Used);
	}
	else
	{
		OSCD_SOBJ_DEL_REALOBJ(pstObj->u8Used);
	}
	
	return 0;
}


//get position along direction
IMP_S32 impGetPosAlongDir(IMP_S32 s32GradX, IMP_S32 s32GradY, IMP_S32 s32Len, IMP_POINT_S *pstPos1, IMP_POINT_S *pstPos2);

//check motion type
IMP_SOBJ_TYPE_S impCheckMotion_(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8Gray = 0, *pu8Bkg = 0, *pu8ObjSign = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft, s32OftR;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_S32 s32GrayDiff = 0, s32BkgDiff = 0;
	IMP_U16 u16Val;
	IMP_U8 au8Bin[9];
	IMP_S32 as32Oft[9], s32GradX, s32GradY;
	IMP_S32 s32ScoreLost, s32ScoreStay, s32ContourLen;
	
#if OSCD_DBG_SHW_TIME	
	struct timeval t1, t2;
#endif

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	pu8Gray = pstResult->stImgInGray.pu8Data;
	pu8Bkg = pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	pu8ObjSign = pstModule->stImgSObjSign.pu8Data;
	
	as32Oft[0] = -s32W - 1; as32Oft[1] = -s32W; as32Oft[2] = -s32W + 1;
	as32Oft[3] = -1; as32Oft[4] = 0; as32Oft[5] = 1;
	as32Oft[6] = s32W - 1; as32Oft[7] = s32W; as32Oft[8] = s32W + 1; 
	
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	
	s32ScoreLost = 0;
	s32ScoreStay = 0;
	s32ContourLen = 0;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI += 2)
	{
		s32OftR = s32RI * s32W;
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI += 1)
		{
			s32Oft = s32OftR + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				au8Bin[0] = pu8ObjSign[s32Oft + as32Oft[0]] > 0;
				au8Bin[1] = pu8ObjSign[s32Oft + as32Oft[1]] > 0;
				au8Bin[2] = pu8ObjSign[s32Oft + as32Oft[2]] > 0;
				au8Bin[3] = pu8ObjSign[s32Oft + as32Oft[3]] > 0;
				au8Bin[4] = pu8ObjSign[s32Oft + as32Oft[4]] > 0;
				au8Bin[5] = pu8ObjSign[s32Oft + as32Oft[5]] > 0;
				au8Bin[6] = pu8ObjSign[s32Oft + as32Oft[6]] > 0;
				au8Bin[7] = pu8ObjSign[s32Oft + as32Oft[7]] > 0;
				au8Bin[8] = pu8ObjSign[s32Oft + as32Oft[8]] > 0;
				
				u16Val = au8Bin[0] + au8Bin[1] + au8Bin[2] + au8Bin[3] + au8Bin[4] + au8Bin[5] + au8Bin[6] + au8Bin[7] + au8Bin[8];
				if (u16Val >= 3 && u16Val <= 7)
				{
					IMP_S32 s32ValF[2], s32ValB[2], s32POft1, s32POft2;
					IMP_POINT_S stPos1, stPos2;
					
					s32ContourLen++;
				//	printf("hi,");
					s32GradX = au8Bin[2] + au8Bin[5] + au8Bin[8] - (au8Bin[0] + au8Bin[3] + au8Bin[6]); 
					s32GradY = au8Bin[6] + au8Bin[7] + au8Bin[8] - (au8Bin[0] + au8Bin[1] + au8Bin[2]);
					
					//get position along direction
					stPos1.s16X = 0; stPos1.s16Y = 0;
					stPos2.s16X = 0; stPos2.s16Y = 0;
					impGetPosAlongDir(s32GradX, s32GradY, 2, &stPos1, &stPos2);
					
					s32POft1 = (stPos1.s16Y + s32RI) * s32W + stPos1.s16X + s32CI;
					s32POft2 = (stPos2.s16Y + s32RI) * s32W + stPos2.s16X + s32CI;
					
					s32ValF[0] = pu8Gray[s32POft1 + as32Oft[0]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[1]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[2]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[3]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[4]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[5]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[6]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[7]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[8]];
					s32ValF[0] /= 9;
					
					s32ValF[1] = pu8Gray[s32POft2 + as32Oft[0]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[1]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[2]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[3]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[4]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[5]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[6]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[7]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[8]];
					s32ValF[1] /= 9;
					
					s32ValB[0] = pu8Bkg[s32POft1 + as32Oft[0]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[1]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[2]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[3]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[4]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[5]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[6]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[7]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[8]];
					s32ValB[0] /= 9;
					
					s32ValB[1] = pu8Bkg[s32POft2 + as32Oft[0]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[1]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[2]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[3]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[4]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[5]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[6]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[7]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[8]];
					s32ValB[1] /= 9;
					
					s32ScoreLost += abs(s32ValB[0] - s32ValB[1]) > abs(s32ValF[0] - s32ValF[1]) * 2;
					s32ScoreStay += abs(s32ValF[0] - s32ValF[1]) > abs(s32ValB[0] - s32ValB[1]) * 2;
				}
			}
		}
	}
	
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("impCheckMotion(%d):%.1f ms\n", s32ContourLen, (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
	
	if (s32ContourLen == 0) return SOBJT_UKN;

#if OSCD_DBG_SHW
	printf("ID_%d: Stay:%d(%d%%), Lost:%d(%d%%)\n", pstObj->u32TargetId, s32ScoreStay, s32ScoreStay * 100 / s32ContourLen, s32ScoreLost, s32ScoreLost * 100 / s32ContourLen);
#endif

	if (s32ScoreStay * 100 > s32ContourLen * 50 && s32ScoreStay > s32ScoreLost * 3) return SOBJT_STP;
	if (s32ScoreLost * 100 > s32ContourLen * 50 && s32ScoreStay * 3 < s32ScoreLost) return SOBJT_RMV;
//	else return SOBJT_UKN;
	
	return SOBJT_UKN;
}


//get position along direction
IMP_S32 impGetPosAlongDir(IMP_S32 s32GradX, IMP_S32 s32GradY, IMP_S32 s32Len, IMP_POINT_S *pstPos1, IMP_POINT_S *pstPos2)
{
	IMP_FLOAT fAngle, fSin, fCos;
	IMP_FLOAT fV1, fV2;
	
	fAngle = atan2(s32GradY, s32GradX);
	fCos = cos(fAngle);
	fSin = sin(fAngle);
//	fAngle = fAngle * 180 / 3.1415926f;
//	if (fAngle < 0) fAngle = 360 + fAngle;
	
	//-22.5~22.5, 22.5~67.5, 67.5~112.5, 112.5~157.5, 157.5~202.5, 202.5~247.5, 247.5~292.5, 292.5~337.5
//	if (fAngle)
//	IMP_S32 s32GX, s32GY;
	
//	s32GX = abs(s32GradX);
//	s32GY = abs(s32GradY);
//	if (s32GX > s32GY)
	{
		fV1 = s32Len * fCos;
		fV2 = s32Len * fSin;
		
		if (fV1 < 0) fV1 -= 0.5f;
		else fV1 += 0.5f;
		if (fV2 < 0) fV2 -= 0.5f;
		else fV2 += 0.5f;
		
		pstPos1->s16X = fV1;
		pstPos1->s16Y = fV2;
		
		pstPos2->s16X = -fV1;
		pstPos2->s16Y = -fV2;
	}
	
	
	return 0;
}



//check motion type
IMP_SOBJ_TYPE_S impCheckMotion(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI;
	IMP_U8 *pu8Gray = 0, *pu8Bkg = 0, *pu8ObjSign = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft, s32OftR;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_S32 s32GrayDiff = 0, s32BkgDiff = 0;
	IMP_U16 u16Val;
	IMP_U8 au8Bin[9];
	IMP_S32 as32Oft[9], s32GradX, s32GradY;
	IMP_S32 s32ScoreLost, s32ScoreStay, s32ContourLen;
	
#if OSCD_DBG_SHW_TIME	
	struct timeval t1, t2;
#endif

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	pu8Gray = pstResult->stImgInGray.pu8Data;
	pu8Bkg = pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	pu8ObjSign = pstModule->stImgSObjSign.pu8Data;
	
	as32Oft[0] = -s32W - 1; as32Oft[1] = -s32W; as32Oft[2] = -s32W + 1;
	as32Oft[3] = -1; as32Oft[4] = 0; as32Oft[5] = 1;
	as32Oft[6] = s32W - 1; as32Oft[7] = s32W; as32Oft[8] = s32W + 1; 
	
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	
	s32ScoreLost = 0;
	s32ScoreStay = 0;
	s32ContourLen = 0;
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI += 2)
	{
		s32OftR = s32RI * s32W;
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI += 1)
		{
			s32Oft = s32OftR + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				au8Bin[0] = pu8ObjSign[s32Oft + as32Oft[0]] > 0;
				au8Bin[1] = pu8ObjSign[s32Oft + as32Oft[1]] > 0;
				au8Bin[2] = pu8ObjSign[s32Oft + as32Oft[2]] > 0;
				au8Bin[3] = pu8ObjSign[s32Oft + as32Oft[3]] > 0;
				au8Bin[4] = pu8ObjSign[s32Oft + as32Oft[4]] > 0;
				au8Bin[5] = pu8ObjSign[s32Oft + as32Oft[5]] > 0;
				au8Bin[6] = pu8ObjSign[s32Oft + as32Oft[6]] > 0;
				au8Bin[7] = pu8ObjSign[s32Oft + as32Oft[7]] > 0;
				au8Bin[8] = pu8ObjSign[s32Oft + as32Oft[8]] > 0;
				
				u16Val = au8Bin[0] + au8Bin[1] + au8Bin[2] + au8Bin[3] + au8Bin[4] + au8Bin[5] + au8Bin[6] + au8Bin[7] + au8Bin[8];
				if (u16Val >= 3 && u16Val <= 7)
				{
					IMP_S32 s32ValF[2], s32ValB[2], s32POft1, s32POft2;
					IMP_POINT_S stPos1, stPos2;
					
					s32ContourLen++;
				//	printf("hi,");
					s32GradX = au8Bin[2] + au8Bin[5] + au8Bin[8] - (au8Bin[0] + au8Bin[3] + au8Bin[6]); 
					s32GradY = au8Bin[6] + au8Bin[7] + au8Bin[8] - (au8Bin[0] + au8Bin[1] + au8Bin[2]);
					
					stPos1.s16X = 0; stPos1.s16Y = 2;
					stPos2.s16X = 0; stPos2.s16Y = -2;
					//get position along direction
					if (abs(s32GradX) > abs(s32GradY))
					{
						stPos1.s16X = 2; stPos1.s16Y = 0;
						stPos2.s16X = -2; stPos2.s16Y = 0;
					}
					
					s32POft1 = (stPos1.s16Y + s32RI) * s32W + stPos1.s16X + s32CI;
					s32POft2 = (stPos2.s16Y + s32RI) * s32W + stPos2.s16X + s32CI;
					
					s32ValF[0] = pu8Gray[s32POft1 + as32Oft[0]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[1]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[2]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[3]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[4]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[5]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[6]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[7]];
					s32ValF[0] += pu8Gray[s32POft1 + as32Oft[8]];
					s32ValF[0] /= 9;
					
					s32ValF[1] = pu8Gray[s32POft2 + as32Oft[0]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[1]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[2]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[3]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[4]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[5]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[6]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[7]];
					s32ValF[1] += pu8Gray[s32POft2 + as32Oft[8]];
					s32ValF[1] /= 9;
					
					s32ValB[0] = pu8Bkg[s32POft1 + as32Oft[0]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[1]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[2]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[3]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[4]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[5]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[6]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[7]];
					s32ValB[0] += pu8Bkg[s32POft1 + as32Oft[8]];
					s32ValB[0] /= 9;
					
					s32ValB[1] = pu8Bkg[s32POft2 + as32Oft[0]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[1]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[2]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[3]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[4]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[5]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[6]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[7]];
					s32ValB[1] += pu8Bkg[s32POft2 + as32Oft[8]];
					s32ValB[1] /= 9;
					
					s32ScoreLost += abs(s32ValB[0] - s32ValB[1]) > abs(s32ValF[0] - s32ValF[1]) * 2;
					s32ScoreStay += abs(s32ValF[0] - s32ValF[1]) > abs(s32ValB[0] - s32ValB[1]) * 2;
				}
			}
		}
	}
	
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("impCheckMotion(%d):%.1f ms\n", s32ContourLen, (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif
	
	if (s32ContourLen == 0) return SOBJT_UKN;
	
#if OSCD_DBG_SHW
	printf("ID_%d: Stay:%d(%d%%), Lost:%d(%d%%)\n", pstObj->u32TargetId, s32ScoreStay, s32ScoreStay * 100 / s32ContourLen, s32ScoreLost, s32ScoreLost * 100 / s32ContourLen);
#endif

	if (s32ScoreStay * 100 > s32ContourLen * 50 && s32ScoreStay > s32ScoreLost * 3) return SOBJT_STP;
	if (s32ScoreLost * 100 > s32ContourLen * 50 && s32ScoreStay * 3 < s32ScoreLost) return SOBJT_RMV;
//	else return SOBJT_UKN;
	
	return SOBJT_UKN;
}



//detect edges of background and gray image.
IMP_S32 impDetectEdges(IMP_OSCD_S *pstModule)
{
	IMP_S32 s32W, s32H;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	ipSobel3_3(pstResult->stDRegionSet.pstImgBgGray->pu8Data, s32W, pstModule->stImgBkgEdge.pu8Data, s32W, s32W, s32H);
	ipSobel3_3(pstResult->stImgInGray.pu8Data, s32W, pstModule->stImgGrayEdge.pu8Data, s32W, s32W, s32H);
	
	return 0;
}



//match static object with current frame, and update
IMP_S32 impMatchSObject(IMP_OSCD_S *pstModule, IMP_STATIC_OBJ_S *pstObj)
{
	IMP_S32 s32RI, s32CI, s32PI;
	IMP_S32 s32R, s32Diff;
	IMP_U8 *pu8Gray = 0, *pu8Bkg = 0, *pu8ObjSign = 0, *pu8ObjEdge = 0, *pu8GrayEdge = 0;
	IMP_S32 s32W = 0, s32H = 0, s32Oft, s32OftR;
	IMP_RECT_S stRect;
	IMP_U8 u8Sign, u8ObjSign, u8Flag;
	IMP_POINT_S stPos;
	PEA_RESULT_S *pstResult = pstModule->pstResult;
	IMP_S32 s32GraySame = 0, s32EdgeSame = 0, s32EdgeNum = 0, as32Oft[9];
	IMP_S32 s32MatchScore, s32GrayScore, s32EdgeScore, s32NCC;
	IMP_U8 au8Data1[9], au8Data2[9];
	
#if OSCD_DBG_SHW_TIME	
	struct timeval t1, t2;
#endif


	
	s32W = pstResult->stImgInGray.s32W;
	s32H = pstResult->stImgInGray.s32H;
	
	as32Oft[0] = -s32W - 1; as32Oft[1] = -s32W; as32Oft[2] = -s32W + 1;
	as32Oft[3] = -1; as32Oft[4] = 0; as32Oft[5] = 1;
	as32Oft[6] = s32W - 1; as32Oft[7] = s32W; as32Oft[8] = s32W + 1;
	
	s32R = pstResult->s32Noise;
	pu8Gray = pstResult->stImgInGray.pu8Data;
	pu8Bkg = pstResult->stDRegionSet.pstImgBgGray->pu8Data;
	pu8GrayEdge = pstModule->stImgGrayEdge.pu8Data;
	pu8ObjSign = pstModule->stImgSObjSign.pu8Data;
	pu8ObjEdge = pstModule->stImgSObjEdge.pu8Data;
	
	stRect = pstObj->stRect;
	u8ObjSign = pstObj->u8Sign;
	
	s32GraySame = 0;
	s32EdgeSame = 0;
	
	s32NCC = 0;
	

#if OSCD_DBG_SHW_TIME
	gettimeofday(&t1, NULL);
#endif
	
	s32EdgeNum = 1;
#if 1	
	
	for (s32RI = stRect.s16Y1; s32RI <= stRect.s16Y2; s32RI += 1)
	{
		s32OftR = s32RI * s32W;
		for (s32CI = stRect.s16X1; s32CI <= stRect.s16X2; s32CI += 1)
		{
			s32Oft = s32OftR + s32CI;
			if (pu8ObjSign[s32Oft] == u8ObjSign)
			{
				if (pu8ObjEdge[s32Oft] > 15 || pu8GrayEdge[s32Oft] > 15)
				{
					
					s32EdgeNum++;
					au8Data1[0] = pu8ObjEdge[s32Oft + as32Oft[0]];
					au8Data1[1] = pu8ObjEdge[s32Oft + as32Oft[1]];
					au8Data1[2] = pu8ObjEdge[s32Oft + as32Oft[2]];
					au8Data1[3] = pu8ObjEdge[s32Oft + as32Oft[3]];
					au8Data1[4] = pu8ObjEdge[s32Oft + as32Oft[4]];
					au8Data1[5] = pu8ObjEdge[s32Oft + as32Oft[5]];
					au8Data1[6] = pu8ObjEdge[s32Oft + as32Oft[6]];
					au8Data1[7] = pu8ObjEdge[s32Oft + as32Oft[7]];
					au8Data1[8] = pu8ObjEdge[s32Oft + as32Oft[8]];
					
					au8Data2[0] = pu8GrayEdge[s32Oft + as32Oft[0]];
					au8Data2[1] = pu8GrayEdge[s32Oft + as32Oft[1]];
					au8Data2[2] = pu8GrayEdge[s32Oft + as32Oft[2]];
					au8Data2[3] = pu8GrayEdge[s32Oft + as32Oft[3]];
					au8Data2[4] = pu8GrayEdge[s32Oft + as32Oft[4]];
					au8Data2[5] = pu8GrayEdge[s32Oft + as32Oft[5]];
					au8Data2[6] = pu8GrayEdge[s32Oft + as32Oft[6]];
					au8Data2[7] = pu8GrayEdge[s32Oft + as32Oft[7]];
					au8Data2[8] = pu8GrayEdge[s32Oft + as32Oft[8]];
					
					s32NCC += impNCC_U8(au8Data1, au8Data2, 9);

				}
			}
		}
	}
#endif
	
	s32EdgeScore = s32EdgeSame * 100 / s32EdgeNum;
	s32MatchScore = (s32GrayScore + s32EdgeScore * 2) / 3;
	s32NCC = s32NCC < 0 ? 0 : s32NCC;
	s32MatchScore = s32NCC / s32EdgeNum;
	if (s32EdgeNum < 5) s32MatchScore = 0; //大梯度太少的目标直接忽略
	pstObj->u32MatchScore = s32MatchScore;
//	printf("edgenum:%d\n", s32EdgeNum);
	
//	printf("ID_%d_MatchScore:%d%%, EdgeScore:%d%%, life:%d, PLen:%d\n", pstObj->u32TargetId, s32GraySame * 100 / pstObj->u32AreaSize, s32EdgeSame * 100 / s32EdgeNum, pstObj->u32Life, pstObj->s32PosLen);

#if OSCD_DBG_SHW
	if (pstObj->u32TargetId == 362)
	{
		printf("\033[34mID_%d_MatchScore_NCC(%d):%d%%\033[0m\n", pstObj->u32TargetId, s32EdgeNum, s32MatchScore);
	}
	else
	{
		printf("ID_%d_MatchScore_NCC(%d):%d%%\n", pstObj->u32TargetId, s32EdgeNum, s32MatchScore);
	}
#endif

	//judge if remove the object
	s32MatchScore = 0;
	for (s32PI = 0; s32PI < pstObj->s32PosLen; s32PI++)
	{
		s32MatchScore += pstObj->au16MatchScore[s32PI];
	}
	
	s32MatchScore /= pstObj->s32PosLen;
	
	if (s32MatchScore < 50) impDeleteObject(pstModule, pstObj);
	
#if OSCD_DBG_SHW_TIME
	gettimeofday(&t2, NULL);
	printf("impMatchSObject(%d)_2:%.1f ms\n", s32EdgeNum, (t2.tv_usec - t1.tv_usec) / 1000.f);
#endif	

	return 0;
}


//normalized cross correlation
IMP_S32 impNCC_U8_(IMP_U8 *pu8Data1, IMP_U8 *pu8Data2, IMP_S32 s32DataLen)
{
	IMP_S32 s32PI;
	IMP_S32 s32NCC = 0;
	IMP_S32 s32Mean1, s32Mean2;
	IMP_S32 s32V1, s32V2, s32V12;
	
	s32Mean1 = 0;
	s32Mean2 = 0;
	for (s32PI = 0; s32PI < s32DataLen; s32PI++)
	{
		s32Mean1 += pu8Data1[s32PI];
		s32Mean2 += pu8Data2[s32PI];
	}
	
	s32Mean1 /= s32DataLen;
	s32Mean2 /= s32DataLen;
	
	if (s32Mean1 == 0 || s32Mean2 == 0) return 0;
	
//	printf("%d, %d; ", s32Mean1, s32Mean2);
	s32V1 = 0;
	s32V2 = 0;
	s32V12 = 0;
	for (s32PI = 0; s32PI < s32DataLen; s32PI++)
	{
		s32V1 += abs(pu8Data1[s32PI] - s32Mean1);
		s32V2 += abs(pu8Data2[s32PI] - s32Mean2);
		s32V12 += (pu8Data1[s32PI] - s32Mean1) * (pu8Data2[s32PI] - s32Mean2);
	}
	s32V1 /= s32DataLen;
	s32V2 /= s32DataLen;
	s32V12 /= s32DataLen;
	
	s32V1 += 1;
	s32V2 += 1;
	s32V12 += 1;
	
	s32NCC = s32V12 * 100 / (s32V1 * s32V2);
	
//	printf("%d,%d,%d,%d; ", s32V1, s32V2, s32V12, s32NCC);
	
	return s32NCC;
}


IMP_S32 impNCC_U8(IMP_U8 *pu8Data1, IMP_U8 *pu8Data2, IMP_S32 s32DataLen)
{
	IMP_S32 s32PI;
	IMP_S32 s32NCC = 0;
	IMP_S32 s32Mean1, s32Mean2;
	IMP_S32 s32V1, s32V2, s32V12;
	
	s32Mean1 = 0;
	s32Mean2 = 0;
	for (s32PI = 0; s32PI < s32DataLen; s32PI++)
	{
		s32Mean1 += pu8Data1[s32PI];
		s32Mean2 += pu8Data2[s32PI];
	}
	
	s32Mean1 /= s32DataLen;
	s32Mean2 /= s32DataLen;
	
	if (s32Mean1 == 0 || s32Mean2 == 0) return 0;
	
//	printf("%d, %d; ", s32Mean1, s32Mean2);
	s32V1 = 0;
	s32V2 = 0;
	s32V12 = 0;
	for (s32PI = 0; s32PI < s32DataLen; s32PI++)
	{
		s32V1 += (pu8Data1[s32PI] - s32Mean1) * (pu8Data1[s32PI] - s32Mean1);
		s32V2 += (pu8Data2[s32PI] - s32Mean2) * (pu8Data2[s32PI] - s32Mean2);
		s32V12 += (pu8Data1[s32PI] - s32Mean1) * (pu8Data2[s32PI] - s32Mean2);
	}
	s32V1 /= s32DataLen;
	s32V2 /= s32DataLen;
	s32V12 /= s32DataLen;
	
	s32V1 += 1;
	s32V2 += 1;
	s32V12 += 1;
	
	s32NCC = s32V12 * 100 / (sqrt(s32V1 * s32V2));
	
//	printf("%d,%d,%d,%d; ", s32V1, s32V2, s32V12, s32NCC);
	
	return s32NCC;
}





