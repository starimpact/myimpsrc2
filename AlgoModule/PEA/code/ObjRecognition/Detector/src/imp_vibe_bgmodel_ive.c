#include "imp_vibe_bgmodel.h"
#include "time.h"
#include "stdlib.h"

//#include "sample_comm.h"
//#include "hi_comm_sys.h"
//#include "mpi_sys.h"
#include "hi_comm_ive.h"
#include "mpi_ive.h"

#define IVESTRIDE 176     //80
#define IVEHEIGHT 144     //64
#define IVEWIDTH  176	  //64
#define HISTSPACE 256


#if 0
	IVE_SRC_INFO_S stSrc1;
	IVE_SRC_INFO_S stSrc2;

	IVE_MEM_INFO_S stDstData1;
	IVE_MEM_INFO_S stDstData2;
	IVE_MEM_INFO_S stDstSub1;
	IVE_MEM_INFO_S stDstSub2;
	IVE_MEM_INFO_S stDstHist;

	//HI_VOID *pVirtSrc1;
	//HI_VOID *pVirtSrc2;

	HI_VOID *pVirtDstData1;
	HI_VOID *pVirtDstData2;

	HI_VOID *pVirtDstSub1;
	HI_VOID *pVirtDstSub2;
	HI_VOID *pVirtDstHist;

	IVE_THRESH_CTRL_S stIveThreshCtrl;

#endif


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

	GRAY_IMAGE_S stImgPreGray; //pre gray image

	IMP_S32 s32Width;
	IMP_S32 s32Height;
	IMP_U8 **ppu8Bkgs; //max background number is 20

	IMP_S32 s32FrmNum;


//ive  data:
	IVE_SRC_INFO_S gstSrc1;
	IVE_SRC_INFO_S gstSrc2;

	IVE_MEM_INFO_S gstDstData1;
	IVE_MEM_INFO_S gstDstData2;
	IVE_MEM_INFO_S gstDstSub1;
	IVE_MEM_INFO_S gstDstSub2;
	IVE_MEM_INFO_S gstDstHist;

	HI_VOID *gpVirtDstData1;
	HI_VOID *gpVirtDstData2;

	HI_VOID *gpVirtDstSub1;
	HI_VOID *gpVirtDstSub2;
	HI_VOID *gpVirtDstHist;

	IVE_THRESH_CTRL_S gstIveThreshCtrl;

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
IMP_S32 IMP_GetMemSizeViBe( PEA_RESULT_S *pstResult )
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;

 	IMP_S32 size = width * height * 30;

	return size;
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
#if 1
	HI_S32 s32Ret = HI_SUCCESS;
#endif
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

	//ppu8Bkgs = (IMP_U8**)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, s32Width * s32Height * sizeof(IMP_U8*));
	ppu8Bkgs = (IMP_U8**)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->s32N * sizeof(IMP_U8*));
	pstModule->ppu8Bkgs = ppu8Bkgs;
	IMP_U8 *pu8Tmp = (IMP_U8*)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule->s32N * s32Width * s32Height);
	/**
	for (dwI = 0; dwI < s32Width * s32Height; dwI++)
	{
		ppu8Bkgs[dwI] = pu8Tmp;
		pu8Tmp += pstModule->s32N;
	}
	*/
	for (dwI = 0; dwI < pstModule->s32N; dwI++)
	{
		ppu8Bkgs[dwI] = pu8Tmp;
		pu8Tmp += s32Width * s32Height;
	}

	IMP_GrayImageCreate( &pstModule->stImgBg, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgFg, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgFgTmp, s32Width, s32Height, pstMemMgr );
	IMP_GrayImageCreate( &pstModule->stImgPreGray, s32Width, s32Height, pstMemMgr );
//	memset(pstModule->stImgFgTmp, 0, s32Height * s32Width);

	pstOutput->stImgBg = pstModule->stImgBg;
	pstOutput->stImgFg = pstModule->stImgFg;

//	printf("\n--------------ViBe-End--------------\n");
/**
	stSrc1.stSrcMem.u32Stride = IVESTRIDE;
	stSrc1.u32Height = IVEHEIGHT;
	stSrc1.u32Width = IVEWIDTH;
	stSrc1.enSrcFmt = IVE_SRC_FMT_SINGLE;

	stSrc2.stSrcMem.u32Stride = IVESTRIDE;
	stSrc2.u32Height = IVEHEIGHT;
	stSrc2.u32Width = IVEWIDTH;
	stSrc2.enSrcFmt = IVE_SRC_FMT_SINGLE;
*/

	pstModule->gstSrc2.u32Height = IVEHEIGHT;
	pstModule->gstSrc2.u32Width = IVEWIDTH;
	pstModule->gstSrc2.enSrcFmt = IVE_SRC_FMT_SINGLE;

	pstModule->gstSrc1.u32Height = IVEHEIGHT;
	pstModule->gstSrc1.u32Width = IVEWIDTH;
	pstModule->gstSrc1.enSrcFmt = IVE_SRC_FMT_SINGLE;


	pstModule->gstDstData1.u32Stride = IVESTRIDE;
	pstModule->gstDstData2.u32Stride = IVESTRIDE;

	pstModule->gstDstSub1.u32Stride = IVESTRIDE;
	pstModule->gstDstSub2.u32Stride = IVESTRIDE;

	pstModule->gstDstHist.u32Stride = HISTSPACE<<2;

	pstModule->gstIveThreshCtrl.enOutFmt=IVE_THRESH_OUT_FMT_BINARY;
	pstModule->gstIveThreshCtrl.u32MinVal=0;
	pstModule->gstIveThreshCtrl.u32MaxVal=1;



	s32Ret = HI_MPI_SYS_MmzAlloc(&pstModule->gstDstData1.u32PhyAddr, &pstModule->gpVirtDstData1,"User", HI_NULL, pstModule->gstSrc1.u32Height * pstModule->gstDstData1.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc stDstData1 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}

	s32Ret = HI_MPI_SYS_MmzAlloc(&pstModule->gstDstData2.u32PhyAddr, &pstModule->gpVirtDstData2,"User", HI_NULL, pstModule->gstSrc1.u32Height * pstModule->gstDstData2.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc stDstData2 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}

/**
	s32Ret = HI_MPI_SYS_MmzAlloc(&stSrc1.stSrcMem.u32PhyAddr, &pVirtSrc1,"User", HI_NULL, stSrc1.u32Height * stSrc1.stSrcMem.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc Src1 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}

	s32Ret = HI_MPI_SYS_MmzAlloc(&stSrc2.stSrcMem.u32PhyAddr, &pVirtSrc2,"User", HI_NULL, stSrc2.u32Height * stSrc2.stSrcMem.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc Src2 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
	*/

	s32Ret = HI_MPI_SYS_MmzAlloc(&pstModule->gstDstSub1.u32PhyAddr, &pstModule->gpVirtDstSub1,"User", HI_NULL, pstModule->gstSrc1.u32Height * pstModule->gstDstSub1.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc stDstSub1 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}

	s32Ret = HI_MPI_SYS_MmzAlloc(&pstModule->gstDstSub2.u32PhyAddr, &pstModule->gpVirtDstSub2,"User", HI_NULL, pstModule->gstSrc1.u32Height * pstModule->gstDstSub2.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc stDstSub2 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}


	s32Ret = HI_MPI_SYS_MmzAlloc(&pstModule->gstDstHist.u32PhyAddr, &pstModule->gpVirtDstHist,"User", HI_NULL, 1 * pstModule->gstDstHist.u32Stride);

	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc DstHist failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}



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
/**
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, ppu8Bkgs[0]);
	for (dwI = 0; dwI < s32Width * s32Height; dwI++)
	{
		ppu8Bkgs[dwI] = 0;
	}
	*/

	HI_MPI_SYS_MmzFree(pstModule->gstDstHist.u32PhyAddr,pstModule->gpVirtDstHist);
	HI_MPI_SYS_MmzFree(pstModule->gstDstSub2.u32PhyAddr,pstModule->gpVirtDstSub2);
	HI_MPI_SYS_MmzFree(pstModule->gstDstSub1.u32PhyAddr,pstModule->gpVirtDstSub1);
	HI_MPI_SYS_MmzFree(pstModule->gstDstData2.u32PhyAddr,pstModule->gpVirtDstData2);
	HI_MPI_SYS_MmzFree(pstModule->gstDstData1.u32PhyAddr,pstModule->gpVirtDstData1);
	//HI_MPI_SYS_MmzFree(stSrc2.stSrcMem.u32PhyAddr, pVirtSrc2);
	//HI_MPI_SYS_MmzFree(stSrc1.stSrcMem.u32PhyAddr, pVirtSrc1);


	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, ppu8Bkgs[0]);
	for (dwI = 0; dwI < pstModule->s32N; dwI++)
	{
		ppu8Bkgs[dwI] = 0;
	}

	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, ppu8Bkgs);
	ppu8Bkgs = 0;

	IMP_GrayImageDestroy( &pstModule->stImgBg, pstMemMgr );
	IMP_GrayImageDestroy( &pstModule->stImgFg, pstMemMgr );
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
	return impProcessViBe0(hModule);
}

IMP_S32 impGetAvgBkg_2(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 s32Width, IMP_S32 s32Height);

#define VIBE_SHW_TIME 1
//two backgrounds
IMP_S32 impProcessViBe0( IMP_MODULE_HANDLE hModule )
{
	IMP_U32 s32RI, s32CI;
	IMP_S32 s32PI;
	//IMP_S32 s32RI, s32CI, s32PI;
	IMP_S32 s32Oft, s32Oft2;
	IMP_ViBe_S *pstModule = (IMP_ViBe_S*)hModule;
	IMP_U8 **ppu8Bkgs = 0, *pu8InGray = 0, *pu8PreGray = 0, *pu8Fg = 0, *pu8FgTmp = 0, *pu8Bkg = 0, *pu8Tmp1, *pu8Tmp2;
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
				ppu8Bkgs[0][s32Oft] = pu8InGray[s32Oft];
				ppu8Bkgs[1][s32Oft] = pu8InGray[s32Oft]; // + as32Oft[impRand(9)]];
			}
		}
	}

	PEA_DETECTED_REGIONSET_S *pRegionSet = &pstResult->stDRegionSet;

	IMP_U16 u16Val = 0, u16Val2;

#if 1
	HI_S32 s32Ret = HI_SUCCESS;
	HI_BOOL bInstant;
	HI_BOOL bFinish;
	HI_BOOL bMatch = HI_TRUE;
	IVE_HANDLE IveHandle;
	/**
	IMP_U32 *pSrcData;
	IMP_U32 *pDstData;
	int count;
	*/
	//IMP_U32 *pSrcData1;
	//IMP_U32 *pDstData1;

/**
	IVE_SRC_INFO_S stSrc1;
	IVE_SRC_INFO_S stSrc2;

	IVE_MEM_INFO_S stDstSub1;
	IVE_MEM_INFO_S stDstSub2;
	//IVE_MEM_INFO_S stDstThresh;
	IVE_MEM_INFO_S stDstHist;

	HI_VOID *pVirtSrc1;
	HI_VOID *pVirtSrc2;
	//unsigned char (*pDataSrc1)[IVESTRIDE];
	//unsigned char (*pDataSrc2)[IVESTRIDE];
	//IMP_U8 *pDataSrc1;
	//IMP_U8 *pDataSrc2;

	HI_VOID *pVirtDstSub1;
	HI_VOID *pVirtDstSub2;
	//HI_VOID *pVirtDstThresh;
	HI_VOID *pVirtDstHist;

	//unsigned char pDataDst[IVEHEIGHT][IVEWIDTH];

	stSrc1.stSrcMem.u32Stride = IVESTRIDE;
	stSrc1.u32Height = IVEHEIGHT;
	stSrc1.u32Width = IVEWIDTH;
	stSrc1.enSrcFmt = IVE_SRC_FMT_SINGLE;

	stSrc2.stSrcMem.u32Stride = IVESTRIDE;
	stSrc2.u32Height = IVEHEIGHT;
	stSrc2.u32Width = IVEWIDTH;
	stSrc2.enSrcFmt = IVE_SRC_FMT_SINGLE;

	stDstSub1.u32Stride = IVESTRIDE;
	stDstSub2.u32Stride = IVESTRIDE;

	//stDstThresh.u32Stride = IVESTRIDE;

	stDstHist.u32Stride = HISTSPACE<<2;

	IVE_THRESH_CTRL_S stIveThreshCtrl;
*/
	bInstant = HI_TRUE;

	int algoPicSize = IVEWIDTH * IVEHEIGHT;

#endif


#if 1
	IMP_U8 **ppBkgs = 0, *pGray = 0, *pPreGray = 0, *pFg = 0, *pFgTmp = 0, *pline1 = 0, *pline2 = 0, *pline3 = 0;
	IMP_U16 tempsum0,tempsum1,tempsum2;
	IMP_U32 *pu32Gray;
	IMP_U32 *pu32Fg;
	IMP_S32 s32Num;
	IMP_S32 flag_long;
	IMP_U32 u32GrayVal;
#endif



//printf("333444555\n");

#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	//generate binary map
	s32NowPixelNum=0;
#if 0
	memset(pu8Fg, 0, s32Height * s32Width);
	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			u8GrayVal = pu8InGray[s32Oft];
			pu8Tmp1 = ppu8Bkgs[s32Oft];

			s32GNum = 0;
			if (abs(pu8Tmp1[0] - u8GrayVal) < s32R || abs(pu8Tmp1[1] - u8GrayVal) < s32R) s32GNum = 1;

			if (s32GNum == 0)
			{
				pu8Fg[s32Oft] = 1;s32NowPixelNum++;
			}
		}
	}

#else
/**
		memset(pu8Fg, 0, s32Height * s32Width);

		s32Oft=s32Width+1;
		s32RI=s32Height-2;
		s32CI=s32Width-2;
		do
		{
			do
			{
				u8GrayVal = pu8InGray[s32Oft];
				pu8Tmp1 = ppu8Bkgs[s32Oft];

				if ((abs(pu8Tmp1[0] - u8GrayVal) >= s32R) && (abs(pu8Tmp1[1] - u8GrayVal) >= s32R))
				{
					pu8Fg[s32Oft] = 1;s32NowPixelNum++;
				}

				s32Oft++;

			}
			while((--s32CI)!=0);
			s32Oft+=2;
			s32CI=s32Width-2;

		}
		while((--s32RI)!=0);
		*/
/**nnn
	s32Ret = HI_MPI_SYS_MmzAlloc(&stSrc1.stSrcMem.u32PhyAddr, &pVirtSrc1,"User", HI_NULL, stSrc1.u32Height * stSrc1.stSrcMem.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc Src1 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}

	s32Ret = HI_MPI_SYS_MmzAlloc(&stSrc2.stSrcMem.u32PhyAddr, &pVirtSrc2,"User", HI_NULL, stSrc2.u32Height * stSrc2.stSrcMem.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc Src2 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
*/
	//pDataSrc1=(unsigned char (*)[IVESTRIDE])pVirtSrc1;
	//pDataSrc2=(unsigned char (*)[IVESTRIDE])pVirtSrc2;

	//pDataSrc1=(IMP_U8 *)pVirtSrc1;
	//pDataSrc2=(IMP_U8 *)pVirtSrc2;


	//gettimeofday(&t1, NULL);

	memcpy(pstModule->gpVirtDstData1,ppu8Bkgs[0],algoPicSize);
	memcpy(pstModule->gpVirtDstData2,pu8InGray,algoPicSize);
	/**
	count = algoPicSize>>2;
	pSrcData=(IMP_U32 *)ppu8Bkgs[0];
	pDstData=(IMP_U32 *)pVirtDstData1;
	//pSrcData1=(IMP_U32 *)pu8InGray;
	//pDstData1=(IMP_U32 *)pVirtDstData2;

	do
	{
		(*pDstData++)=(*pSrcData++);
		//(*pDstData1++)=(*pSrcData1++);
	}while(--count);


	count = algoPicSize>>2;
	pSrcData=(IMP_U32 *)pu8InGray;
	pDstData=(IMP_U32 *)pVirtDstData2;
	do
	{
		(*pDstData++)=(*pSrcData++);
	}while(--count);
	*/
	//gettimeofday(&t2, NULL);
	//printf("memcpy time :%d us\n", (t2.tv_usec - t1.tv_usec));

	pstModule->gstSrc1.stSrcMem	= pstModule->gstDstData1;
	pstModule->gstSrc2.stSrcMem	= pstModule->gstDstData2;
/**nnn
	s32Ret = HI_MPI_SYS_MmzAlloc(&stDstSub1.u32PhyAddr, &pVirtDstSub1,"User", HI_NULL, stSrc1.u32Height * stDstSub1.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc stDstSub1 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
*/

	s32Ret = HI_MPI_IVE_SUB(&IveHandle, &pstModule->gstSrc1, &pstModule->gstSrc2, &pstModule->gstDstSub1, IVE_SUB_OUT_FMT_ABS, bInstant);
	if (HI_SUCCESS != s32Ret)
	{
        //printf("%s: HI_MPI_IVE_SUB 1 vibe-binary failed with %#x!\n",\
        //       __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
#if 1
	do
	{
		s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_FALSE);
		if (HI_SUCCESS != s32Ret)
		{
			//printf("%s: HI_MPI_IVE_Query 1 vibe-binary failed with %#x!\n",\
			//	   __FUNCTION__,  s32Ret);
			return HI_FAILURE;
		}
		//printf("Bool Finish %d\n",bFinish);
		if(bFinish)
			break;
		//sleep(2);
		//printf("Can't Finished! looping ....\n");
	}while(1);
#else
	usleep(1);
#endif

	memcpy(pstModule->gpVirtDstData1,ppu8Bkgs[1],algoPicSize);
/**nnn
	s32Ret = HI_MPI_SYS_MmzAlloc(&stDstSub2.u32PhyAddr, &pVirtDstSub2,"User", HI_NULL, stSrc1.u32Height * stDstSub2.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc stDstSub2 failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
*/
	s32Ret = HI_MPI_IVE_SUB(&IveHandle, &pstModule->gstSrc1, &pstModule->gstSrc2, &pstModule->gstDstSub2, IVE_SUB_OUT_FMT_ABS, bInstant);
	if (HI_SUCCESS != s32Ret)
	{
        //printf("%s: HI_MPI_IVE_SUB 2 vibe-binary failed with %#x!\n",\
        //       __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
#if 1
	do
	{
		s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_FALSE);
		if (HI_SUCCESS != s32Ret)
		{
			//printf("%s: HI_MPI_IVE_Query 2 vibe-binary failed with %#x!\n",\
			//	   __FUNCTION__,  s32Ret);
			return HI_FAILURE;
		}
		//printf("Bool Finish %d\n",bFinish);
		if(bFinish)
			break;
		//sleep(2);
		//printf("Can't Finished! looping ....\n");
	}while(1);
#else
	usleep(1);
#endif

	//memcpy(pVirtSrc1,pVirtDstSub1,algoPicSize);
	//memcpy(pVirtSrc2,pVirtDstSub2,algoPicSize);
	pstModule->gstSrc1.stSrcMem	= pstModule->gstDstSub1;
	pstModule->gstSrc2.stSrcMem	= pstModule->gstDstSub2;



	//stIveThreshCtrl.enOutFmt=IVE_THRESH_OUT_FMT_BINARY;
	pstModule->gstIveThreshCtrl.u32Thresh=s32R;
	//stIveThreshCtrl.u32MinVal=0;
	//stIveThreshCtrl.u32MaxVal=1;
/**
	s32Ret = HI_MPI_SYS_MmzAlloc(&stDstThresh.u32PhyAddr, &pVirtDstThresh,"User", HI_NULL, stSrc1.u32Height * stDstThresh.u32Stride);
	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc Dst failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}

	s32Ret = HI_MPI_IVE_THRESH(&IveHandle, &stSrc1, &stDstThresh, &stIveThreshCtrl, bInstant);
*/
	s32Ret = HI_MPI_IVE_THRESH(&IveHandle, &pstModule->gstSrc1, &pstModule->gstDstData1, &pstModule->gstIveThreshCtrl, bInstant);
	if (HI_SUCCESS != s32Ret)
	{
        //printf("%s: HI_MPI_IVE_ATHRESH failed with %#x!\n",\
        //       __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
#if 1
	do
	{
		s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_FALSE);
		if (HI_SUCCESS != s32Ret)
		{
			//printf("%s: HI_MPI_IVE_Query failed with %#x!\n",\
			//	   __FUNCTION__,  s32Ret);
			return HI_FAILURE;
		}
		//printf("Bool Finish %d\n",bFinish);
		if(bFinish)
			break;
		//sleep(2);
		//printf("Can't Finished! looping ....\n");
	}while(1);
#else
	usleep(1);
#endif

	s32Ret = HI_MPI_IVE_THRESH(&IveHandle, &pstModule->gstSrc2, &pstModule->gstDstData2, &pstModule->gstIveThreshCtrl, bInstant);
	if (HI_SUCCESS != s32Ret)
	{
        //printf("%s: HI_MPI_IVE_ATHRESH failed with %#x!\n",\
        //       __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
#if 1
	do
	{
		s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_FALSE);
		if (HI_SUCCESS != s32Ret)
		{
			//printf("%s: HI_MPI_IVE_Query failed with %#x!\n",\
			//	   __FUNCTION__,  s32Ret);
			return HI_FAILURE;
		}
		//printf("Bool Finish %d\n",bFinish);
		if(bFinish)
			break;
		//sleep(2);
		//printf("Can't Finished! looping ....\n");
	}while(1);
#else
	usleep(1);
#endif

	//memcpy(pVirtSrc1,pVirtDstSub1,algoPicSize);
	//memcpy(pVirtSrc2,pVirtDstSub2,algoPicSize);
	pstModule->gstSrc1.stSrcMem	= pstModule->gstDstData1;
	pstModule->gstSrc2.stSrcMem	= pstModule->gstDstData2;


	s32Ret = HI_MPI_IVE_AND(&IveHandle, &pstModule->gstSrc1, &pstModule->gstSrc2, &pstModule->gstDstSub1, bInstant);
	if (HI_SUCCESS != s32Ret)
	{
        //printf("%s: HI_MPI_IVE_AND failed with %#x!\n",\
        //       __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
#if 1
	do
	{
		s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_FALSE);
		if (HI_SUCCESS != s32Ret)
		{
			//printf("%s: HI_MPI_IVE_Query failed with %#x!\n",\
			//	   __FUNCTION__,  s32Ret);
			return HI_FAILURE;
		}
		//printf("Bool Finish %d\n",bFinish);
		if(bFinish)
			break;
		//sleep(2);
		//printf("Can't Finished! looping ....\n");
	}while(1);
#else
	usleep(1);
#endif

	memcpy(pu8Fg,pstModule->gpVirtDstSub1,algoPicSize);
	//memcpy(pVirtSrc1,pVirtDstSub1,algoPicSize);
	pstModule->gstSrc1.stSrcMem	= pstModule->gstDstSub1;
/**nnn
	s32Ret = HI_MPI_SYS_MmzAlloc(&stDstHist.u32PhyAddr, &pVirtDstHist,"User", HI_NULL, 1 * stDstHist.u32Stride);

	if (HI_SUCCESS != s32Ret)
	{
        printf("%s: HI_MPI_SYS_MmzAlloc DstHist failed with %#x!\n",\
               __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
	*/

	s32Ret = HI_MPI_IVE_HIST(&IveHandle, &pstModule->gstSrc1, &pstModule->gstDstHist, bInstant);
	if (HI_SUCCESS != s32Ret)
	{
        //printf("%s: HI_MPI_IVE_HIST failed with %#x!\n",\
        //       __FUNCTION__,  s32Ret);
        return HI_FAILURE;
	}
#if 1
	do
	{
		s32Ret = HI_MPI_IVE_Query(IveHandle, &bFinish, HI_FALSE);

		if (HI_SUCCESS != s32Ret)
		{
			//printf("%s: HI_MPI_IVE_Query failed with %#x!\n",\
			//	   __FUNCTION__,  s32Ret);
			return HI_FAILURE;
		}
		//printf("Bool Finish %d\n",bFinish);
		if(bFinish)
			break;
		//sleep(2);
		//printf("Can't Finished! looping ....\n");
	}while(1);
#else
	usleep(1);
#endif

	s32NowPixelNum=*((unsigned int *)(pstModule->gpVirtDstHist)+1);

/**222
	HI_MPI_SYS_MmzFree(stDstHist.u32PhyAddr,pVirtDstHist);
	HI_MPI_SYS_MmzFree(stDstSub2.u32PhyAddr,pVirtDstSub2);
	HI_MPI_SYS_MmzFree(stDstSub1.u32PhyAddr,pVirtDstSub1);
	HI_MPI_SYS_MmzFree(stSrc2.stSrcMem.u32PhyAddr, pVirtSrc2);
	HI_MPI_SYS_MmzFree(stSrc1.stSrcMem.u32PhyAddr, pVirtSrc1);
	*/
#endif

#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);

printf("vibe-binary:%d us\n", (t2.tv_usec - t1.tv_usec));

#endif

#if 1 //cat

#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif


#if 0
	//get conour of object
	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			u16Val2 = 0;

			u16Val2 += pu8Fg[s32Oft + as32Oft[0]] + pu8Fg[s32Oft + as32Oft[1]] + pu8Fg[s32Oft + as32Oft[2]];
			u16Val2 += pu8Fg[s32Oft + as32Oft[3]] + pu8Fg[s32Oft + as32Oft[4]] + pu8Fg[s32Oft + as32Oft[5]];
			u16Val2 += pu8Fg[s32Oft + as32Oft[6]] + pu8Fg[s32Oft + as32Oft[7]] + pu8Fg[s32Oft + as32Oft[8]];

			u8Diff = abs(pu8InGray[s32Oft] - pu8PreGray[s32Oft]);

			if (u16Val2 < 3 && pu8Fg[s32Oft]) {pu8Fg[s32Oft] = 0; s32NowPixelNum--;}//remove dust

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

#else

	s32Oft=s32Width+1;
	pline1=pu8Fg+1;
	pline2=pu8Fg+s32Width+1;
	pline3=pu8Fg+(s32Width<<1)+1;

	pGray=pu8InGray+s32Oft;
	pPreGray=pu8PreGray+s32Oft;
	pFg=pu8Fg+s32Oft;
	pFgTmp=pu8FgTmp+s32Oft;

	s32RI=s32Height-2;
	s32CI=s32Width-2;
	do
	{
		tempsum1=(*pline1++)+(*pline2++)+(*pline3++);
		tempsum0=0;
		do
		{
			tempsum2=(*pline1++)+(*pline2++)+(*pline3++);
			u16Val2=tempsum0+tempsum1+tempsum2;
			tempsum0=tempsum1;
			tempsum1=tempsum2;

			u8Diff = abs((*pGray++)-(*pPreGray++));

			if (u16Val2 < 3 && (*pFg) ) {(*pFg) = 0; s32NowPixelNum--;tempsum0--;}//remove dust

			if (u8Diff < s32R && (*pFg))
			{
				if ((*pFgTmp) < 127 && ((pstModule->s32FrmNum & 0x1) == 0))
				{
					(*pFgTmp)++;
				}
			}

			if (!(*pFg) || u8Diff > s32R)
			{
				(*pFgTmp) = (*pFgTmp) - 2 < 0 ? 0 : (*pFgTmp) - 2; //decreasing quickly
			}


			pFg++;
			pFgTmp++;
		}
		while((--s32CI)!=0);
		pFg+=2;
		pFgTmp+=2;
		pGray+=2;
		pPreGray+=2;

		pline1++;
		pline2++;
		pline3++;
		s32CI=s32Width-2;
	}
	while((--s32RI)!=0);

#endif



#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
//printf("vibe-contour:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
printf("vibe-contour:%d us\n", (t2.tv_usec - t1.tv_usec));
#endif
#endif	//cat

#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif
#if 0
	s32NowPixelNum = 0;
	for (s32RI = 0; s32RI < s32Width * s32Height; s32RI++)
	{
		if (pu8Fg[s32RI])
		{
			pu8Fg[s32RI] = 255;
			s32NowPixelNum++;
		}
	}
#else

	pu32Fg=(IMP_U32 *)pu8Fg;
	s32Num = (s32Width * s32Height)>>2;
	do
	{
		(*pu32Fg++)=((*pu32Fg)<<8)-(*pu32Fg);
	}while(--s32Num);

#endif

#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);

printf("vibe-pixel:%d us\n", (t2.tv_usec - t1.tv_usec));
#endif

	if (abs(s32NowPixelNum - s32PrePixelNum) * 100 > s32Width * s32Height * 20)
	{
		s32PrePixelNum = 0;
		s32NowPixelNum = 0;
		for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
		{
			for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
			{
				s32Oft = s32RI * s32Width + s32CI;
				ppu8Bkgs[0][s32Oft] = pu8InGray[s32Oft];
				ppu8Bkgs[1][s32Oft] = pu8InGray[s32Oft]; // + as32Oft[impRand(9)]];
			}
		}

		//impGetAvgBkg_2(ppu8Bkgs, pu8Bkg, s32Width, s32Height);
		memcpy(pu8Bkg,ppu8Bkgs[0],algoPicSize);

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

#if 0

	for (s32RI = 1; s32RI < s32Height - 1; s32RI++)
	{
		for (s32CI = 1; s32CI < s32Width - 1; s32CI++)
		{
			s32Oft = s32RI * s32Width + s32CI;
			u8GrayVal = pu8InGray[s32Oft];
			pu8Tmp1 = ppu8Bkgs[s32Oft];

			#if 1

				if ((!pu8Fg[s32Oft] && 1) || (pu8FgTmp[s32Oft] >= pstModule->s32BkgUR2 && pu8Fg[s32Oft])) //illumination background update
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
				//
				}
			#else

				//update background value
				if ((!pu8Fg[s32Oft]) || (pu8FgTmp[s32Oft] >= pstModule->s32BkgUR2 && pu8Fg[s32Oft])) //illumination background update
				{
					u8Bkg = pu8Tmp1[0];
					u16Val = ((u8GrayVal-u8Bkg)*50+(u8Bkg<<7)+64)>>7;
					pu8Tmp1[0] = u16Val > 255 ? 255 : u16Val;//u8GrayVal;
				}

				if (!pu8Fg[s32Oft] && ((pstModule->s32FrmNum & 0x0000001F ) == 0)) //long background update
				{
					u8Bkg = pu8Tmp1[1];
					u16Val = (((u8GrayVal-u8Bkg)<<4)+ (u8Bkg<<7) + 64) >> 7;
					pu8Tmp1[1] = u16Val > 255 ? 255 : u16Val;
				//
				}

			#endif

		}

	}
#else

	flag_long = pstModule->s32FrmNum & 0x0000001F;
	s32Oft=s32Width+1;
	s32RI=s32Height-2;
	s32CI=s32Width-2;
	do
	{
		do
		{
			u8GrayVal = pu8InGray[s32Oft];
			pu8Tmp1 = ppu8Bkgs[0]+s32Oft;

			if(pu8Fg[s32Oft]!=0)
			{
				if(pu8FgTmp[s32Oft]<pstModule->s32BkgUR2)
				{
					s32Oft++;
					continue;
				}
			}

			pu8Tmp1[0]+=(((u8GrayVal-pu8Tmp1[0])*50+64)>>7);

			if(flag_long != 0)
			{
				s32Oft++;
				continue;
			}
			pu8Tmp2 = pu8Tmp1+algoPicSize;
			pu8Tmp2[0] += ((pu8Tmp1[0]-pu8Tmp2[0]+4)>>3);

			s32Oft++;

		}while((--s32CI)!=0);

		s32Oft+=2;
		s32CI=s32Width-2;

	}while((--s32RI)!=0);


#endif

#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-diffuse:%d us\n", (t2.tv_usec - t1.tv_usec));
#endif


#if VIBE_SHW_TIME
gettimeofday(&t1, NULL);
#endif
	//impGetAvgBkg_2(ppu8Bkgs, pu8Bkg, s32Width, s32Height);
	memcpy(pu8Bkg,ppu8Bkgs[0],algoPicSize);
#if VIBE_SHW_TIME
gettimeofday(&t2, NULL);
printf("vibe-bkgavg:%d us\n", (t2.tv_usec - t1.tv_usec));
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

#if 0
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
#else

IMP_S32 impGetAvgBkg_2(IMP_U8 **ppu8Bkgs, IMP_U8 *pu8Bkg, IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Num = (s32Width * s32Height)>>2;

	IMP_U32 *p32Bkg=(IMP_U32 *)(*ppu8Bkgs);

	IMP_U32 *pBkg=(IMP_U32 *)pu8Bkg;

	IMP_U32 preWord,nextWord;

	preWord=(*p32Bkg++);
	nextWord=(*p32Bkg++);
	do
	{
		//(*pBkg++)=((*(*ppu8Bkgs++))|((*(*++ppu8Bkgs))<<8)|((*(*++ppu8Bkgs))<<16)|((*(*++ppu8Bkgs))<<24));
		(*pBkg++)=(preWord&0x000000FF)|((preWord&0x00FF0000)>>8)|((nextWord&0x000000FF)<<16)|((nextWord&0x00FF0000)<<8);
		preWord=(*p32Bkg++);
		nextWord=(*p32Bkg++);

	}while((--s32Num)!=0);

}

#endif


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





