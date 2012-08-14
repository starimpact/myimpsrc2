#include "imp_vibe_bgmodel.h"
#include "time.h"
#include "stdlib.h"

//算法数据存放
typedef struct impViBe_S
{
	//系统成员
    GA_HARDWARE_RS_S  *pstHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult; //系统公共数据
	
	IMP_OutputViBe_S *pstOutput;
	
	//算法私有自定义成员
	
	//参数配置
	IMP_S32 s32N; //number of samples per pixel, default value:10
	IMP_S32 s32R; //radius of sphere, default value:20
	IMP_S32 s32G; //number of close samples for being part of the background, default value:2
	IMP_S32 s32S; //amount of subsampling, default value:2
	
	IMP_S32 s32BkgUR; //背景更新速度1~120, disable vibe when it is 1
	
	IMP_S32 s32BkgUR2; //背景更新速度2, 1~120
	
	IMP_S32 s32PrePixelNum;
	
	GRAY_IMAGE_S stImgBg; //background image
	GRAY_IMAGE_S stImgFg; //binary image
	GRAY_IMAGE_S stImgFgTmp; //tmporary binary image
	
	GRAY_IMAGE_S stImgFgDilate; //Dilate binary image
	
	GRAY_IMAGE_S stImgPreGray; //pre gray image
	
	IMP_S32 s32Width;
	IMP_S32 s32Height;
	IMP_U8 **ppu8Bkgs; //max background number is 20
	
	IMP_S32 s32FrmNum;
	
	
}IMP_ViBe_S;

//generate random number
IMP_U32 impRand(IMP_U32 u32R);

//get bakcground of one layer
IMP_S32 impGetOneBkg(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 sN, IMP_S32 s32Width, IMP_S32 s32Height, IMP_S32 s32Layer);
//get avg bakcground
IMP_S32 impGetAvgBkg(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 sN, IMP_S32 s32Width, IMP_S32 s32Height);


/***************************************************
name:IMP_GetMemSizeViBe
do:得到算法所需要的内存大小
input:
	pstResult:系统公共数据
output:算法需要的内存大小
history:
****************************************************/
IMP_S32 IMP_GetMemSizeViBe(IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Size = 0;
	
	s32Size += sizeof(IMP_ViBe_S);
	s32Size += s32Width * s32Height * sizeof(IMP_U8*);
	s32Size += 2 * s32Width * s32Height;
	s32Size += 4 * s32Width * s32Height;
	
	return s32Size;
}


/***************************************************
name:IMP_CreateViBe
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
IMP_MODULE_HANDLE IMP_CreateViBe( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	IMP_MODULE_HANDLE hModule;
	int dwI;
	IMP_S32 s32Height, s32Width;
	IMP_U8 **ppu8Bkgs = 0; //max background number is 10
	MEM_MGR_ARRAY_S *pstMemMgr;
	IMP_ViBe_S *pstModule;
	IMP_OutputViBe_S *pstOutput;
	
	s32Height = pstResult->s32Height;
	s32Width = pstResult->s32Width;
	
	pstOutput = &pstResult->stOutPutViBeModel; //记录输出结果地址
	
	pstMemMgr = &pstHwResource->stMemMgr;

//	printf("\n--------------ViBe-Start--------------\n");
	
	pstModule = (IMP_ViBe_S*)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_ViBe_S));
	
	pstModule->pstHwResource = pstHwResource; //记录硬件信息地址
	pstModule->pstResult = pstResult; //记录系统信息地址
	
	pstModule->s32N = 2;
	pstModule->s32R = 10;
	pstModule->s32G = 2;
	pstModule->s32S = 8;
	
	pstModule->s32BkgUR = 1;
	
	pstModule->s32BkgUR2 = 64;
	
	pstModule->s32FrmNum = 0;
	
	pstModule->s32Width = s32Width;
	pstModule->s32Height = s32Height;
	
	ppu8Bkgs = (IMP_U8**)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, s32Width * s32Height * sizeof(IMP_U8*));
	pstModule->ppu8Bkgs = ppu8Bkgs;
	IMP_U8 *pu8Tmp = (IMP_U8*)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->s32N * s32Width * s32Height);
	for (dwI = 0; dwI < s32Width * s32Height; dwI++)
	{
		ppu8Bkgs[dwI] = pu8Tmp;
		pu8Tmp += pstModule->s32N;
	}
	
	IMP_GrayImageCreate( &pstModule->stImgBg, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgFg, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgFgDilate, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgFgTmp, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgPreGray, s32Width, s32Height, pstMemMgr );
//	memset(pstModule->stImgFgTmp, 0, s32Height * s32Width);
	
//	printf("create-vibe-bkg:%x\n", pstModule->stImgBg.pu8Data);
	
	pstOutput->stImgBg = pstModule->stImgBg;
	pstOutput->stImgFg = pstModule->stImgFg;
	
//	printf("\n--------------ViBe-End--------------\n");
	
	hModule = (IMP_MODULE_HANDLE)pstModule;
	return hModule;
}


/***************************************************
name:IMP_ReleaseViBe
do:释放算法内存
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ReleaseViBe( IMP_MODULE_HANDLE hModule )
{
	int dwI;
	IMP_ViBe_S *pstModule = (IMP_ViBe_S*)hModule;
	
	IMP_S32 s32Height, s32Width;
	IMP_U8 **ppu8Bkgs = 0;
	MEM_MGR_ARRAY_S *pstMemMgr;
	
	s32Width = pstModule->s32Width;
	s32Height = pstModule->s32Height;
	
	pstMemMgr = &pstModule->pstHwResource->stMemMgr;
	
	ppu8Bkgs = pstModule->ppu8Bkgs;
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, ppu8Bkgs[0]);
	for (dwI = 0; dwI < s32Width * s32Height; dwI++)
	{
		ppu8Bkgs[dwI] = 0;
	}
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, ppu8Bkgs);
	ppu8Bkgs = 0;
	
//	printf("release-vibe-bkg:%x\n", pstModule->stImgBg.pu8Data);
	
	IMP_GrayImageDestroy( &pstModule->stImgBg, pstMemMgr );
	IMP_GrayImageDestroy( &pstModule->stImgFg, pstMemMgr );
	IMP_GrayImageDestroy( &pstModule->stImgFgDilate, pstMemMgr );
	IMP_GrayImageDestroy( &pstModule->stImgFgTmp, pstMemMgr );
	IMP_GrayImageDestroy( &pstModule->stImgPreGray, pstMemMgr );
	
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);
	pstModule = 0;
	
	return 0;
}
IMP_ViBe_S *pstModule;

/***************************************************
name:IMP_ConfigViBe
do:配置算法参数
input:
	hModule:算法模块句柄
	pstPara:参数配置结构体
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ConfigViBeModel( IMP_MODULE_HANDLE hModule, IMP_ViBePara_S *pstPara )
{
	IMP_ViBe_S *pstModule = (IMP_ViBe_S*)hModule;
	
	pstModule->s32BkgUR = pstPara->s32BkgUR;
	
	return 0;
}


IMP_S32 impProcessViBe1( IMP_MODULE_HANDLE hModule );
IMP_S32 impProcessViBe2( IMP_MODULE_HANDLE hModule );
IMP_S32 impProcessViBe0( IMP_MODULE_HANDLE hModule );
/***************************************************
name:IMP_ProcessViBe
do:算法主过程
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ProcessViBe( IMP_MODULE_HANDLE hModule )
{
	printf("hello ProcessViBe.\n");
	return impProcessViBe0(hModule);
}

IMP_S32 impGetAvgBkg_2(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 s32Width, IMP_S32 s32Height);

#define VIBE_SHW_TIME 0
//two backgrounds
IMP_S32 impProcessViBe0( IMP_MODULE_HANDLE hModule )
{
	IMP_S32 s32RI, s32CI, s32PI;
	IMP_S32 s32Oft, s32Oft2;
	IMP_ViBe_S *pstModule = (IMP_ViBe_S*)hModule;
	IMP_U8 **ppu8Bkgs = 0, *pu8InGray = 0, *pu8PreGray = 0, *pu8Fg = 0, *pu8FgDilate = 0, *pu8FgTmp = 0, *pu8Bkg = 0, *pu8Tmp1;
	IMP_U8 u8GrayVal;
	IMP_S32 as32Oft[9];
	IMP_S32 s32N; //number of samples per pixel, default value:10
	IMP_S32 s32R; //radius of sphere, default value:20
	IMP_S32 s32G; //number of close samples for being part of the background, default value:2
	IMP_S32 s32S; //amount of subsampling, default value:2
	IMP_S32 s32Rnd1, s32Rnd2, s32Rnd3, s32GNum, s32BkgUR, s32PrePixelNum, s32NowPixelNum;
	GRAY_IMAGE_S *pstImgBg;
	IMP_U8 u8Bkg, u8Diff;
	
	int i;
#if VIBE_SHW_TIME
	struct timeval t1, t2;
#endif
	
    GA_HARDWARE_RS_S  *pstHwResource = pstModule->pstHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult = pstModule->pstResult; //系统公共数据
	
	GRAY_IMAGE_S *pstInGray = &pstResult->stImgInGray;
	IMP_S32 s32Width = pstResult->s32Width, s32Height = pstResult->s32Height;
	
	
	as32Oft[0] = -s32Width - 1; as32Oft[1] = -s32Width; as32Oft[2] = -s32Width + 1;
	as32Oft[3] = 1; as32Oft[4] = s32Width + 1; as32Oft[5] = s32Width; 
	as32Oft[6] = s32Width - 1; as32Oft[7] = -1; as32Oft[8] = 0;
	
	s32N = pstModule->s32N;
	s32R = pstModule->s32R;
	s32G = pstModule->s32G;
	s32S = pstModule->s32S;
	
	pstModule->s32FrmNum++;
	
	s32BkgUR = pstModule->s32BkgUR;
	
	s32PrePixelNum = pstModule->s32PrePixelNum;
	
	s32R = pstResult->s32Noise;

	ppu8Bkgs = pstModule->ppu8Bkgs;
	pu8InGray = pstInGray->pu8Data;
	pu8Bkg = pstModule->stImgBg.pu8Data;
	pu8Fg = pstModule->stImgFg.pu8Data;
	pu8FgDilate = pstModule->stImgFgDilate.pu8Data;
	pu8FgTmp = pstModule->stImgFgTmp.pu8Data;
	pu8PreGray = pstModule->stImgPreGray.pu8Data;
	
	//it will be used for the static object detection.
	IMP_CalcMotionDiffImage(s32Width, s32Height, pstResult->s32Noise, pu8InGray, pu8PreGray, pstResult->stDRegionSet.pstImgFrmDiff->pu8Data);
//	printf("RAND_MAX:%d(%d)\n", RAND_MAX, 0x7fff);

	//initialize
	if (pstResult->u32FrmTimeCur == 0)
	{
		s32PrePixelNum = 0;
		s32NowPixelNum = 0;
		for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
		{
			for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
			{
				s32Oft = s32RI * s32Width + s32CI;
				ppu8Bkgs[s32Oft][0] = pu8InGray[s32Oft];
				ppu8Bkgs[s32Oft][1] = pu8InGray[s32Oft]; // + as32Oft[impRand(9)]];
			}
		}
		
#if 0
		for (s32PI = 0; s32PI < s32N; s32PI++)
		{
			impGetOneBkg(ppu8Bkgs, pu8Bkg, s32N, s32Width, s32Height, s32PI);
			ipShowGrayImage(s32Width, s32Height, pu8Bkg, "ViBe111");
			cvWaitKey(0);
		}
#endif
	}
	
	PEA_DETECTED_REGIONSET_S *pRegionSet = &pstResult->stDRegionSet;

	
//	IMP_OutputLightRemove_S *pstOutputLR =&pstResult->stOutPutLR;
//	IMP_U8 *pu8Illum = pstOutputLR->pu8img;
//	ipShowBinImage(s32Width, s32Height, pu8Illum, "illum");
	
	
	IMP_U16 u16Val = 0, u16Val2;
#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	//generate binary map
	memset(pu8Fg, 0, s32Height * s32Width);
	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			u8GrayVal = pu8InGray[s32Oft];
			pu8Tmp1 = ppu8Bkgs[s32Oft];
		
			s32GNum = 0;			
			if (
			//	pu8Illum[s32Oft] != 1 || 
				abs(pu8Tmp1[0] - u8GrayVal) < s32R || abs(pu8Tmp1[1] - u8GrayVal) < s32R
			) s32GNum = 1;

			if (s32GNum == 0)
			{
				pu8Fg[s32Oft] = 1;
			}
		}
	}
#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-binary:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
//	memcpy(pu8Fg, pu8FgTmp, s32Height * s32Width);
#endif
	
#if 1 //cat

#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	
	//get conour of object
	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;	
//	s32RI = (s32Height - 1) * s32Width;
//	for (s32Oft = s32Width; s32Oft < s32RI; s32Oft++)
//	{
//		s32CI = s32Oft % s32Width;
//		if (s32CI > 0 && s32CI < s32Width - 1)
//		{
			u16Val2 = 0;
			
			u16Val2 += pu8Fg[s32Oft + as32Oft[0]] + pu8Fg[s32Oft + as32Oft[1]] + pu8Fg[s32Oft + as32Oft[2]];
			u16Val2 += pu8Fg[s32Oft + as32Oft[3]] + pu8Fg[s32Oft + as32Oft[4]] + pu8Fg[s32Oft + as32Oft[5]];
			u16Val2 += pu8Fg[s32Oft + as32Oft[6]] + pu8Fg[s32Oft + as32Oft[7]] + pu8Fg[s32Oft + as32Oft[8]];
			
			u8Diff = abs(pu8InGray[s32Oft] - pu8PreGray[s32Oft]);

			if (u16Val2 < 3 && pu8Fg[s32Oft]) pu8Fg[s32Oft] = 0; //remove dust
			
			if (u8Diff < s32R && pu8Fg[s32Oft])
			{
				if (pu8FgTmp[s32Oft] < 127 && pstModule->s32FrmNum % 2 == 0) 
				{
					pu8FgTmp[s32Oft]++;
				}
			}
			
			if (!pu8Fg[s32Oft] || u8Diff > s32R)
			{
				pu8FgTmp[s32Oft] = pu8FgTmp[s32Oft] - 2 < 0 ? 0 : pu8FgTmp[s32Oft] - 2; //decreasing quickly
			}
		}
	}
	
	
#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-contour:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif

#endif	//cat

	IMP_U8 au8Mask[9] = {1, 1, 1, 1, 1, 1, 1, 1, 1};
	memset(pu8FgDilate, 0, s32Width * s32Height);
	ipMorphDilateImage(pu8Fg, s32Width, pu8FgDilate, s32Width, s32Width, s32Height, au8Mask);
//	ipShowGrayImage(s32Width, s32Height, pu8FgDilate, "FgDilate");
	
#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif

	s32NowPixelNum = 0;
	for (s32RI = 0; s32RI < s32Width * s32Height; s32RI++)
	{
		if (pu8Fg[s32RI])
		{
			pu8Fg[s32RI] = 255;
			s32NowPixelNum++;
		}
	}

#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-pixel:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif

//	printf("%.2f%%\n", abs(s32NowPixelNum - s32PrePixelNum) * 100.f / (s32Width * s32Height));
	pstResult->stSysStatus.u32NeedReInit = 0;
	if (abs(s32NowPixelNum - s32PrePixelNum) * 100 > s32Width * s32Height * 20)
	{
		pstResult->stSysStatus.u32NeedReInit = 1;
		s32PrePixelNum = 0;
		s32NowPixelNum = 0;
		for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
		{
			for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
			{
				s32Oft = s32RI * s32Width + s32CI;
				ppu8Bkgs[s32Oft][0] = pu8InGray[s32Oft];
				ppu8Bkgs[s32Oft][1] = pu8InGray[s32Oft]; // + as32Oft[impRand(9)]];
			}
		}
		
		impGetAvgBkg_2(ppu8Bkgs, pu8Bkg, s32Width, s32Height);
		memset(pu8Fg, 0, s32Height * s32Width);
		IMP_GrayImageClone(&pstModule->stImgFg, pstResult->stDRegionSet.pstImgFgRgn);
		IMP_GrayImageClone(&pstModule->stImgFg, pstResult->stDRegionSet.pstImgFgOrg);
		IMP_GrayImageClone(&pstModule->stImgBg, pstResult->stDRegionSet.pstImgBgGray);
	
		memcpy(pu8PreGray, pu8InGray, s32Width * s32Height);
		pstModule->s32PrePixelNum = 0;
		return 0;
	}

#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif

	//update bkg value except the first one.
	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
//	s32RI = (s32Height - 1) * s32Width;
//	for (s32Oft = s32Width; s32Oft < s32RI; s32Oft++)
//	{
//		s32CI = s32Oft % s32Width;
//		if (s32CI > 0 && s32CI < s32Width - 1)
//		{
			u8GrayVal = pu8InGray[s32Oft];
			pu8Tmp1 = ppu8Bkgs[s32Oft];
			
			//update background value
			if (
			(!pu8Fg[s32Oft]) || // && !pu8FgDilate[s32Oft]) || 
			(pu8FgTmp[s32Oft] >= pstModule->s32BkgUR2 && pu8Fg[s32Oft])
			) //illumination background update
			{
				u16Val = (u8GrayVal * 50 + pu8Tmp1[0] * (128 - 50) + 64) >> 7;
				u8GrayVal = u16Val > 255 ? 255 : u16Val;
				pu8Tmp1[0] = u8GrayVal;
			}
			
			if (!pu8Fg[s32Oft] && pstModule->s32FrmNum % 32 == 0) //long background update
			{
				u8Bkg = pu8Tmp1[1];
			//	u16Val = (u8GrayVal * 4 + u8Bkg * (128 - 4) + 64) >> 7; //can prevent the dust
				u16Val = (u8GrayVal * 16 + u8Bkg * (128 - 16) + 64) >> 7; //can prevent the dust
				u8GrayVal = u16Val > 255 ? 255 : u16Val;
				pu8Tmp1[1] = u8GrayVal;
			}
		}
	}
#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-diffuse:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif


#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	impGetAvgBkg_2(ppu8Bkgs, pu8Bkg, s32Width, s32Height);
#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-bkgavg:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif

#if 0
//	ipShowGrayImage(s32Width, s32Height, pu8FgTmp, "static_count");

	ipShowBinImageLargerVal(s32Width, s32Height, pu8FgTmp, 25, "static");
	
//	ipShowBinImageLargerVal(s32Width, s32Height, pu8FgTmp, pstModule->s32BkgUR2 - 10, "Contour");
	
//	ipMorphCloseImage( &pstModule->stImgFg, &pstModule->stImgFg, &pstModule->stImgFg, 1 );
#if 0
//	for (s32PI = 0; s32PI < s32N; s32PI++)
	for (s32PI = 0; s32PI < 2; s32PI++)
	{
		IMP_U8 aubyS[100];
		impGetOneBkg(ppu8Bkgs, pu8Bkg, s32N, s32Width, s32Height, s32PI);
		sprintf(aubyS, "ViBe_Bkg_%d", s32PI);
		ipShowGrayImage(s32Width, s32Height, pu8Bkg, aubyS);
		cvMoveWindow(aubyS, (s32PI % 3) * s32Width, s32Height * 2 + (s32PI / 3) * s32Height / 2);
	}
#endif
	
//	impGetOneBkg(ppu8Bkgs, pu8Bkg, s32N, s32Width, s32Height, 0);
//	s32PI = 0;
//	ipShowGrayImage(s32Width, s32Height, pu8Bkg, "ViBe_Bkg");
//	cvMoveWindow("ViBe_Bkg", (s32PI % 3) * s32Width, s32Height * 2 + (s32PI / 3) * s32Height / 2);
	
//	ipShowGrayImage(s32Width, s32Height, pu8PreGray, "pu8PreGray");
//	ipShowGrayImage(s32Width, s32Height, pu8InGray, "pu8InGray");
//	s32PI = 1;
//	cvMoveWindow("pu8InGray", (s32PI % 3) * s32Width, s32Height * 2 + (s32PI / 3) * s32Height / 2);

//	ipShowBinImage(s32Width, s32Height, pu8Fg, "ViBe_Bin");
//	s32PI = 2;
//	cvMoveWindow("ViBe_Bin", (s32PI % 3) * s32Width, s32Height * 2 + (s32PI / 3) * s32Height / 2);
#endif



	IMP_GrayImageClone(&pstModule->stImgFg, pstResult->stDRegionSet.pstImgFgRgn);
	IMP_GrayImageClone(&pstModule->stImgFg, pstResult->stDRegionSet.pstImgFgOrg);
	IMP_GrayImageClone(&pstModule->stImgBg, pstResult->stDRegionSet.pstImgBgGray);
	
	memcpy(pu8PreGray, pu8InGray, s32Width * s32Height);
	pstModule->s32PrePixelNum = s32NowPixelNum;
	
	if (pstModule->s32FrmNum == 100) pstModule->s32FrmNum = 0;
	
	return 0;
}


//get bakcground of one layer
IMP_S32 impGetOneBkg(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 sN, IMP_S32 s32Width, IMP_S32 s32Height, IMP_S32 s32Layer)
{
	IMP_S32 s32RI, s32CI;
	IMP_S32 s32Oft;
	
	if (s32Layer >= sN) return -1;
	
	for (s32RI = 0; s32RI < s32Height; s32RI++)
	{
		for (s32CI = 0; s32CI < s32Width; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			pu8Bkg[s32Oft] = ppu8Bkgs[s32Oft][s32Layer];
		}
	}
	
	return 0;
}


IMP_S32 impGetAvgBkg_2(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32PI;
	IMP_S32 s32Num, s32Val;
	
	s32Num = s32Width * s32Height;
	for (s32PI = 0; s32PI < s32Num; s32PI++)
	{
		s32Val = 0;
	//	s32Val = ppu8Bkgs[s32PI][0] + ppu8Bkgs[s32PI][1];
	//	pu8Bkg[s32PI] = s32Val >> 1;
		
		pu8Bkg[s32PI] = ppu8Bkgs[s32PI][0];
	}
	
	return 0;
}

//get avg bakcground
IMP_S32 impGetAvgBkg(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 sN, IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32RI, s32CI, s32Chn;
	IMP_S32 s32Oft, s32Val;
	
	
	for (s32RI = 0; s32RI < s32Height; s32RI++)
	{
		for (s32CI = 0; s32CI < s32Width; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			s32Val = 0;
			for (s32Chn = 0; s32Chn < sN; s32Chn++) s32Val += ppu8Bkgs[s32Oft][s32Chn];
			pu8Bkg[s32Oft] = s32Val / sN;
		}
	}
	
	return 0;
}

//generate random number
IMP_U32 impRand(IMP_U32 u32R)
{
//	srand(time(0));
//	float f = rand();
//	
//	IMP_U32 val = (IMP_U32)(f / RAND_MAX * u32R);
	IMP_U32 ret, val;
	val = rand();
	ret = val % u32R;
//	printf("rand:%d,%d\n", ret, val);
	return ret;
}




