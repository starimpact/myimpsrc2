#include "imp_tracker.h"
#include "time.h"
#include "stdlib.h"
#include "highgui.h"
#include "cv.h"
#include "cxcore.h"
#include "stdio.h"
#include "math.h"

#define IMP_MIN_TGT_AREA          20
#define IMP_STABLE_THRESHOLD      20
#define IMP_Max_SplitDistance     3



typedef struct impTgtPosition_S
{
	IMP_U8 u8Used;
	IMP_POINT_S stPt;
	IMP_RECT_S stRg;
	IMP_U32 u32AreaPixel;
	IMP_S32 s32Velocity;
	IMP_S32 s32Direction;
	double VX;
	double VY;
	double ErrorX;
	double ErrorY;
}IMP_TgtPosition_S;

typedef struct impTgtTrajectory_S
{
	IMP_U16 u16Num;
	IMP_TgtPosition_S astPositions[IMP_MAX_TRAJECTORY_LEN];
}IMP_TgtTrajectory_S;//目标轨迹

typedef struct impTarget_S
{
	IMP_U8 u8Sign;
	IMP_U8 u8Used;
	IMP_U8 u8Stable;
	IMP_U8 u8MoveEstimate;
	IMP_U8 u8Special;
	IMP_U16 SpecialID[10];
	IMP_U8 SpecialCount;
	IMP_TgtTrajectory_S stTrajectory;
	IMP_U32 Pixels[10];
	IMP_U8 Signs[10];
	IMP_U16 TargetID;
}IMP_Target_S;

typedef struct impTargetSet_S
{
	IMP_U8 u8Used;
	IMP_U32 TargetTotal;//目标总数
	IMP_Target_S astTarget[IMP_MAX_TGT_CNT];
}IMP_TargetSet_S;

typedef struct
{
	IMP_U8 u8Used;
	IMP_RECT_S Rect;
	IMP_U32 Pixels[10];
	IMP_U8 Signs[10];
}RectTmp_S;

IMP_S32 IMP_DeleteTarget(IMP_Target_S*, PEA_RESULT_S*);
IMP_S32 IMP_CreateTarget(IMP_Target_S*, PEA_RESULT_S*);
IMP_S32 impFindTargetOutById(IpTrackedTargetSet *pstTTS, IMP_U32 u32ID);
IMP_S32 impOutputTargetOut(IMP_Target_S *pstTarget,  IMP_TargetSet_S*, PEA_RESULT_S *pstResult);
double ImpGetDist(IMP_RECT_S*, IMP_RECT_S*);
void GetCentroid(PEA_RESULT_S*, IMP_U8*, IMP_RECT_S*, IMP_POINT_S*);
void MoveEstimate(IMP_Target_S*, IMP_TargetSet_S*, PEA_RESULT_S*, IMP_U8);//运动估计
IMP_U8 OverlapAnalysis(IMP_Target_S*, PEA_RESULT_S*, IMP_RECT_S*, IMP_U8*, IMP_U8*, RectTmp_S*, IMP_U8*);//重叠分析
IMP_U8 SplitAnalysis(IMP_Target_S*, PEA_RESULT_S*, IMP_RECT_S*, IMP_U8*, IMP_U8*, IMP_U8*, RectTmp_S*);
void SeparateAnalysis(IMP_Target_S*, IMP_TargetSet_S*, PEA_RESULT_S*, RectTmp_S*, IMP_U8*, IMP_U8*);//分离分析
void MergeAnalysis(PEA_RESULT_S*, IMP_TargetSet_S*);//合并分析
void IMP_PushNewPosition(IMP_TgtTrajectory_S*, IMP_TgtPosition_S*);//轨迹记录
void GetRegion(PEA_RESULT_S*, IMP_U8*, IMP_RECT_S*, IMP_U8*, IMP_U8*);

#if 0
IMP_S32 IMP_ShowTarget(IMP_TargetSet_S*, GRAY_IMAGE_S*);
double ColorTable[] = {0,255,0,255,0,0,0,0,255,255,255,0,255,0,255,0,255,255,0,0,128,0,128,0,128,0,128,128,128,0};
IMP_U32 frame;//记录帧数
#endif

//算法数据存放
typedef struct impTracker_S
{
	//系统成员
	GA_HARDWARE_RS_S  *pstHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult; //系统公共数据
	
	IpTrackedTargetSet *pstOutput;
	
	//中间数据
	IMP_U8 UsedRegionFlag[IMP_MAX_TGT_CNT];
	IMP_U8 TrackedTgtFlag[IMP_MAX_TGT_CNT];
	RectTmp_S RectTmp[10];
	IMP_U8 *LastFrame;//用于记录前一帧数据
	IMP_U8 *TmpFrame;
	IMP_U8 *TmpFrame0;
	IMP_TargetSet_S stTrackTgtSet;
	
	//算法私有自定义成员
	IMP_S32 s32Width;
	IMP_S32 s32Height;
	//参数配置
	
}IMP_Tracker_S;


/***************************************************
name:IMP_GetMemSizeTracker
do:得到算法所需要的内存大小
input:
	pstResult:系统公共数据
output:算法需要的内存大小
history:
****************************************************/
IMP_S32 IMP_GetMemSizeTracker( PEA_RESULT_S *pstResult )
{
	IMP_S32 width = pstResult->s32Width;
	IMP_S32 height = pstResult->s32Height;
	
 	IMP_S32 size = width * height * 30;
	
	return size;
}


/***************************************************
name:IMP_CreateTracker
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
IMP_MODULE_HANDLE IMP_CreateTracker( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource )
{
	IMP_MODULE_HANDLE hModule;
	int dwI;
	IMP_S32 s32Height, s32Width;
	IMP_U8 **ppu8Bkgs = 0; //max background number is 10
	MEM_MGR_ARRAY_S *pstMemMgr;
	IMP_Tracker_S *pstModule;
	IpTrackedTargetSet *pstOutput;
	s32Height = pstResult->s32Height;
	s32Width = pstResult->s32Width;
	pstOutput = &pstResult->stTrackedTargetSet; //记录输出结果地址	
	pstMemMgr = &pstHwResource->stMemMgr;
	pstModule = (IMP_Tracker_S*)IMP_MMAlloc(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IMP_Tracker_S));	

	pstModule->pstHwResource = pstHwResource; //记录硬件信息地址
	pstModule->pstResult = pstResult; //记录系统信息地址	
	pstModule->s32Width = s32Width;
	pstModule->s32Height = s32Height;
	pstModule->LastFrame = (IMP_U8*)malloc(s32Width*s32Height);	
	pstModule->TmpFrame = (IMP_U8*)malloc(s32Width*s32Height);	
	pstModule->TmpFrame0 = (IMP_U8*)malloc(s32Width*s32Height);		

//	memset(&pstResult->stTrackedTargetSet, 0, sizeof(IpTrackedTargetSet));
	
	ipTrackedTargetSetClear(&pstResult->stTrackedTargetSet);

	hModule = (IMP_MODULE_HANDLE)pstModule;

	return hModule;
}


/***************************************************
name:IMP_ReleaseTracker
do:释放算法内存
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ReleaseTracker( IMP_MODULE_HANDLE hModule )
{
	int dwI;
	IMP_Tracker_S *pstModule = (IMP_Tracker_S*)hModule;
	
	IMP_S32 s32Height, s32Width;
	IMP_U8 **ppu8Bkgs = 0;
	MEM_MGR_ARRAY_S *pstMemMgr;	
	s32Width = pstModule->s32Width;
	s32Height = pstModule->s32Height;	
	pstMemMgr = &pstModule->pstHwResource->stMemMgr;

	free(pstModule->LastFrame);
	free(pstModule->TmpFrame);
	free(pstModule->TmpFrame0);
	IMP_MMFree(pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstModule);

	return 0;
}


/***************************************************
name:IMP_ConfigTracker
do:配置算法参数
input:
	hModule:算法模块句柄
	pstPara:参数配置结构体
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ConfigTracker( IMP_MODULE_HANDLE hModule, IMP_TrackerPara_S *pstPara )
{
	IMP_Tracker_S *pstModule = (IMP_Tracker_S*)hModule;	
	return 0;
}


/***************************************************
name:IMP_ProcessTracker
do:算法主过程
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ProcessTracker( IMP_MODULE_HANDLE hModule )
{
//	frame++;//记录帧数
	IMP_Tracker_S *pstModule = (IMP_Tracker_S*)hModule;
	
	GA_HARDWARE_RS_S  *pstHwResource = pstModule->pstHwResource; //系统硬件资源
	PEA_RESULT_S      *pstResult = pstModule->pstResult; //系统公共数据
	
	IMP_U8 *UsedRegionFlag = pstModule->UsedRegionFlag;
	IMP_U8 *TrackedTgtFlag = pstModule->TrackedTgtFlag;
	RectTmp_S *RectTmp = pstModule->RectTmp;
	IMP_U8 *LastFrame = pstModule->LastFrame;
	IMP_U8 *TmpFrame = pstModule->TmpFrame;
	IMP_U8 *TmpFrame0 = pstModule->TmpFrame0;
	IMP_TargetSet_S *pstTrackTgtSet = &pstModule->stTrackTgtSet;

	GRAY_IMAGE_S *pstInGray = &pstResult->stImgInGray;
	IMP_S32 s32Width = pstResult->s32Width, s32Height = pstResult->s32Height;
	
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstResult->stDRegionSet;

	PEA_DETECTED_REGION_S *pstDRegion = pstDRegionSet->astDrg;
	
	GRAY_IMAGE_S *pstImgBgGray = pstDRegionSet->pstImgBgGray;
	GRAY_IMAGE_S *pstImgFgOrg = pstDRegionSet->pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn = pstDRegionSet->pstImgFgRgn;
	
	//the max number of region is IMP_MAX_TGT_CNT
	PEA_DETECTED_REGION_S *pstDrg = pstDRegionSet->astDrg;
	IMP_U8 i, j, k, n;
	IMP_Target_S *pstTarget = pstTrackTgtSet->astTarget;
	memset(UsedRegionFlag, 0, IMP_MAX_TGT_CNT);
	memset(TrackedTgtFlag, 0, IMP_MAX_TGT_CNT);
	for(i=0; i<IMP_MAX_TGT_CNT; i++)
	{
		if(pstTarget->u8Used && (!pstTarget->u8MoveEstimate) && (!TrackedTgtFlag[i]))
		{
			IMP_RECT_S stRt;
			IMP_U8 Signs[10] = {0,0,0,0,0,0,0,0,0,0};
			IMP_U8 TrackFlag = OverlapAnalysis(pstTarget, pstResult, &stRt, Signs, LastFrame, RectTmp, TmpFrame);//重叠分析
			IMP_U8 SeparateNum = 0;
			if(TrackFlag>1) SeparateNum = SplitAnalysis(pstTarget, pstResult, &stRt, Signs, TmpFrame, TmpFrame0, RectTmp);//分裂分析
			if(SeparateNum>1 && pstTarget->u8Special)//分离处理
			{
				SeparateAnalysis(pstTarget, pstTrackTgtSet, pstResult, RectTmp, UsedRegionFlag, TrackedTgtFlag);
			}
			else
			{
				if(!TrackFlag)
				{ 
					pstTarget->u8Used = 0;
					IMP_DeleteTarget(pstTarget, pstResult);
				}
				else
				{
					if(pstTarget->u8Special)
					{
						pstTarget->SpecialCount++;
						if(pstTarget->SpecialCount>100)
						{
							pstTarget->u8Special = 0;
							IMP_Target_S *pstTgt = pstTrackTgtSet->astTarget;
							for(n=0; n<IMP_MAX_TGT_CNT; n++)
							{
								if(pstTgt->u8Used)
								{
									IMP_U8 flag = 0;
									for(k=0; k<10; k++)
									if(pstTgt->TargetID==pstTarget->SpecialID[k])
										flag =1;
									if(flag) { pstTgt->u8Used = 0; IMP_DeleteTarget(pstTgt, pstResult); }
								}
								pstTgt++;
							}
						}
					}
					n = 0;
					IMP_POINT_S stPt1;
					IMP_POINT_S stPt2 = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].stPt;
					GetCentroid(pstResult, pstImgFgRgn->pu8Data, &RectTmp[0].Rect, &stPt1);
					double dist = pow(stPt1.s16X-stPt2.s16X, 2)+pow(stPt1.s16Y-stPt2.s16Y, 2);
					double dist0 = 0;
					for(k=1; k<10; k++)
					{
						if(RectTmp[k].u8Used)
						{
							GetCentroid(pstResult, pstImgFgRgn->pu8Data, &RectTmp[k].Rect, &stPt1);
							dist0 = pow(stPt1.s16X-stPt2.s16X, 2)+pow(stPt1.s16Y-stPt2.s16Y, 2);
							if(dist0<dist){ n = k; dist = dist0; }
						}
					}
					IMP_TgtPosition_S stPositions;
					stPositions.stRg = RectTmp[n].Rect;
					IMP_U32 SumPixels = 0;
					IMP_U32 TmpPixels = RectTmp[n].Pixels[0];
					IMP_U8  TmpSign = RectTmp[n].Signs[0];
					for(k=0; k<10; k++)
					{
						pstTarget->Signs[k] = RectTmp[n].Signs[k];
						pstTarget->Pixels[k] = RectTmp[n].Pixels[k];
						SumPixels += RectTmp[n].Pixels[k];
						if(RectTmp[n].Pixels[k]>TmpPixels) { TmpPixels = RectTmp[n].Pixels[k]; TmpSign = RectTmp[n].Signs[k]; }
					}
					pstTarget->u8Sign = TmpSign;
					stPositions.u32AreaPixel = SumPixels;
					stPositions.u8Used = 1;

					for(k=0; RectTmp[n].Signs[k]!=0&&k<10; k++)
					{
						pstDrg = pstDRegionSet->astDrg;
						for(j=0; j<IMP_MAX_TGT_CNT; j++)
						{
							if(pstDrg->u8Used && pstDrg->u8Sign==RectTmp[n].Signs[k]) 
							{
								UsedRegionFlag[j] = 1;
								break;
							}
							pstDrg++;
						}		
					}

					GetCentroid(pstResult, pstImgFgRgn->pu8Data, &stPositions.stRg, &stPositions.stPt);
					if(pstTarget->stTrajectory.u16Num>=IMP_STABLE_THRESHOLD) 
					{
						if(pstTarget->u8Stable==0)
						{
							pstTarget->u8Stable = 1;
						}
						else//计算速度和方向
						{
							int n;
							IMP_S32 SumX = 0;
							IMP_S32 SumY = 0;
							for(n=0; n<20; n++)
							{
								SumX += (IMP_S32)(pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1-n].stPt.s16X-pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-2-n].stPt.s16X);
								SumY += (IMP_S32)(pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1-n].stPt.s16Y-pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-2-n].stPt.s16Y);
							}
							stPositions.VX = SumX/20.0;
							stPositions.VY = SumY/20.0;
							stPositions.ErrorX = 0.0;
							stPositions.ErrorY = 0.0;
							stPositions.s32Velocity = (IMP_S32)(sqrt(pow(stPositions.VX,2)+pow(stPositions.VY,2))*128);
							stPositions.s32Direction = (IMP_S32)(atan2(stPositions.VY,stPositions.VX)*180.0/3.1415926);
						}
					}
					else
					{
						stPositions.s32Velocity = 0;
						stPositions.s32Direction = 0;
						stPositions.VX = 0.0;
						stPositions.VY = 0.0;
						stPositions.ErrorX = 0.0;
						stPositions.ErrorY = 0.0;
					}
					IMP_PushNewPosition(&pstTarget->stTrajectory, &stPositions);
				}
			}
		}
		pstTarget++;
	}

	pstTarget = pstTrackTgtSet->astTarget;
	for(i=0; i<IMP_MAX_TGT_CNT; i++)
	{
		if(pstTarget->u8Used && pstTarget->u8MoveEstimate)
		{
			MoveEstimate(pstTarget, pstTrackTgtSet, pstResult, 0);//运动估计
		}
		pstTarget++;
	}

	pstDrg = pstDRegionSet->astDrg;
	for(j=0; j<IMP_MAX_TGT_CNT; j++)
	{
		if(UsedRegionFlag[j]==1) 
			pstDrg->u8Used = 0;
		pstDrg++;
	}

	MergeAnalysis(pstResult, pstTrackTgtSet);//合并分析

//======================================================================================================加入新目标
	i = 0; j = 0;
	pstDrg = pstDRegionSet->astDrg;
	pstTarget = pstTrackTgtSet->astTarget;
	IMP_POINT_S TrackPoint;
	while(i<IMP_MAX_TGT_CNT)
	{
		if(pstDrg->u8Used && pstDrg->s32AreaPixel>=IMP_MIN_TGT_AREA)
		{
			while(j<IMP_MAX_TGT_CNT)
			{
				if(!pstTarget->u8Used)
				{
					pstTarget->TargetID = pstTrackTgtSet->TargetTotal;
					pstTrackTgtSet->TargetTotal++;
					pstTarget->u8Used = 1;
					pstTarget->u8Sign = pstDrg->u8Sign;
					pstTarget->Signs[0] = pstDrg->u8Sign;
					pstTarget->u8Stable = 0;
					pstTarget->u8Special = 0;
					pstTarget->u8MoveEstimate = 0;
					pstTarget->stTrajectory.u16Num = 0;
					IMP_TgtPosition_S stPositions;
					stPositions.u8Used = 1;
					stPositions.stRg = pstDrg->stRect;
					GetCentroid(pstResult, pstImgFgRgn->pu8Data, &stPositions.stRg, &stPositions.stPt);
					stPositions.u32AreaPixel = (IMP_U32)pstDrg->s32AreaPixel;
					stPositions.VX = 0.0;
					stPositions.VY = 0.0;
					stPositions.ErrorX = 0.0;
					stPositions.ErrorY = 0.0;
					stPositions.s32Velocity = 0;
					stPositions.s32Direction = 0;
					IMP_PushNewPosition(&pstTarget->stTrajectory, &stPositions);
					IMP_CreateTarget(pstTarget, pstResult);
					break;
				}
				pstTarget++;
				j++;
			}
		}
		pstDrg++;
		i++;
	}

	IMP_U32 m;
	IMP_U8 *ptrData = pstImgFgRgn->pu8Data;
	for(m=0; m<pstResult->s32Width*pstResult->s32Height; m++) LastFrame[m] = ptrData[m];

	impOutputTargetOut(pstTrackTgtSet->astTarget, pstTrackTgtSet, pstResult);//结果保存

//printf("output target number:%d\n", Total);
//====================================================================================================结果显示
#if 0


//printf("%d\n", frame);
	IMP_ShowTarget(pstTrackTgtSet, pstImgFgRgn);
	
	ipShowGrayImage(s32Width, s32Height, pstInGray->pu8Data, "tracker_inframe");
	ipShowGrayImage(s32Width, s32Height, pstImgBgGray->pu8Data, "tracker_bkg");
	ipShowBinImage(s32Width, s32Height, pstImgFgOrg->pu8Data, "tracker_frg");
//	ipShowBinImage(s32Width, s32Height, pstImgFgRgn->pu8Data, "tracker_rgn");
	
//	cvMoveWindow("tracker_inframe", 0, 0);
//	cvMoveWindow("tracker_bkg", 360, 0);
//	cvMoveWindow("tracker_frg", 0, 360);
//	cvMoveWindow("tracker_rgn", 0, 720);

#endif

	return 0;
}

void SeparateAnalysis(IMP_Target_S *pstTarget, IMP_TargetSet_S *pstTrackTgtSet, PEA_RESULT_S *pstResult, RectTmp_S *RectTmp, IMP_U8 *UsedRegionFlag, IMP_U8 *TrackedTgtFlag)//分离分析
{
	IMP_S32 j, k, m, n, q;
	PEA_DETECTED_REGION_S *pstDrg;
	IMP_U8 *ptrImgNow = (&pstResult->stDRegionSet)->pstImgFgRgn->pu8Data;
	m = 0;
	for(k=0; k<10; k++)//滤除小区域
	{
		IMP_U16 SumTmp = 0;
		if(RectTmp[k].u8Used)
		{
			for(n=0; n<10; n++) SumTmp += RectTmp[k].Pixels[n];
			if(SumTmp>=IMP_MIN_TGT_AREA) m++;
			else RectTmp[k].u8Used = 0;
		}
	}

	n = 0;
	IMP_POINT_S stPt1;
	IMP_POINT_S stPt2 = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].stPt;
	GetCentroid(pstResult, ptrImgNow, &RectTmp[0].Rect, &stPt1);
	double dist = pow(stPt1.s16X-stPt2.s16X, 2)+pow(stPt1.s16Y-stPt2.s16Y, 2);
	double dist0 = 0;
	for(k=1; k<10; k++)
	{
		if(RectTmp[k].u8Used)
		{
			GetCentroid(pstResult, ptrImgNow, &RectTmp[k].Rect, &stPt1);
			dist0 = pow(stPt1.s16X-stPt2.s16X, 2)+pow(stPt1.s16Y-stPt2.s16Y, 2);
			if(dist0<dist){ n = k; dist = dist0; }
		}
	}
	IMP_TgtPosition_S stPositions;
	stPositions.stRg = RectTmp[n].Rect;
	GetCentroid(pstResult, ptrImgNow, &stPositions.stRg, &stPositions.stPt);
	stPositions.s32Velocity = 0;
	stPositions.s32Direction = 0;
	stPositions.VX = 0.0;
	stPositions.VY = 0.0;
	stPositions.ErrorX = 0.0;
	stPositions.ErrorY = 0.0;
	IMP_U32 SumPixels = 0;
	IMP_U32 TmpPixels = RectTmp[n].Pixels[0];
	IMP_U8  TmpSign = RectTmp[n].Signs[0];
	for(k=0; k<10; k++)
	{
		pstTarget->Signs[k] = RectTmp[n].Signs[k];
		pstTarget->Pixels[k] = RectTmp[n].Pixels[k];
		SumPixels += RectTmp[n].Pixels[k];
		if(RectTmp[n].Pixels[k]>TmpPixels) { TmpPixels = RectTmp[n].Pixels[k]; TmpSign = RectTmp[n].Signs[k]; }
	}
	pstTarget->u8Sign = TmpSign;
	stPositions.u32AreaPixel = SumPixels;
	for(k=0; RectTmp[n].Signs[k]!=0&&k<10; k++)
	{
		pstDrg = (&pstResult->stDRegionSet)->astDrg;
		for(m=0; m<IMP_MAX_TGT_CNT; m++)
		{
			if(pstDrg->u8Used && pstDrg->u8Sign==RectTmp[n].Signs[k]) 
			{
				UsedRegionFlag[m] = 1;
				break;
			}
			pstDrg++;
		}		
	}
	RectTmp[n].u8Used = 0;
	IMP_PushNewPosition(&pstTarget->stTrajectory, &stPositions);
	for(k=0; k<10; k++)
	if(RectTmp[k].u8Used)
	{
		IMP_U8 flag = 0;
		double dist, dist1;
		for(n=0; n<10; n++) 
		if(pstTarget->SpecialID[n])
		{
			IMP_Target_S *pstTgt = pstTrackTgtSet->astTarget;
			for(j=0; j<IMP_MAX_TGT_CNT; j++)
			{
				if(pstTgt->TargetID==pstTarget->SpecialID[n])
				{
					IMP_POINT_S stPt1;
					IMP_POINT_S stPt = pstTgt->stTrajectory.astPositions[pstTgt->stTrajectory.u16Num-1].stPt;
					GetCentroid(pstResult, ptrImgNow, &RectTmp[k].Rect, &stPt1);
					dist1 = pow(stPt.s16X-stPt1.s16X, 2)+pow(stPt.s16Y-stPt1.s16Y, 2);
					if(!flag)
					{
						dist = dist1;
						flag = 1;
						m = 0;
					}
					else
					{
						if(dist1<dist)
						{
							dist = dist1;
							m = n;
						}
					}
				}
				pstTgt++;
			}
		}
		IMP_Target_S *pstTgt = pstTrackTgtSet->astTarget;
		for(j=0; j<IMP_MAX_TGT_CNT; j++)
		{
			if(pstTgt->TargetID==pstTarget->SpecialID[m])
			{
				TrackedTgtFlag[j] = 1;
				pstTarget->SpecialID[m] = 0;
				pstTgt->u8MoveEstimate = 0;
				IMP_TgtPosition_S stPositions;
				stPositions.stRg = RectTmp[k].Rect;
				GetCentroid(pstResult, ptrImgNow, &stPositions.stRg, &stPositions.stPt);
				stPositions.s32Velocity = 0;
				stPositions.s32Direction = 0;
				stPositions.VX = 0.0;
				stPositions.VY = 0.0;
				stPositions.ErrorX = 0.0;
				stPositions.ErrorY = 0.0;
				IMP_U32 SumPixels = 0;
				IMP_U32 TmpPixels = RectTmp[k].Pixels[0];
				IMP_U8  TmpSign = RectTmp[k].Signs[0];
				for(n=0; n<10; n++)
				{
					pstTgt->Signs[n] = RectTmp[k].Signs[n];
					pstTgt->Pixels[n] = RectTmp[k].Pixels[n];
					SumPixels += RectTmp[k].Pixels[n];
					if(RectTmp[k].Pixels[n]>TmpPixels) { TmpPixels = RectTmp[k].Pixels[n]; TmpSign = RectTmp[k].Signs[n]; }
				}
				pstTgt->u8Sign = TmpSign;
				stPositions.u32AreaPixel = SumPixels;
				for(q=0; RectTmp[k].Signs[q]!=0&&q<10; q++)
				{
					pstDrg = (&pstResult->stDRegionSet)->astDrg;
					for(n=0; n<IMP_MAX_TGT_CNT; n++)
					{
						if(pstDrg->u8Used && pstDrg->u8Sign==RectTmp[k].Signs[q]) 
						{
							UsedRegionFlag[n] = 1;
							break;
						}
						pstDrg++;
					}		
				}
				RectTmp[k].u8Used = 0;
				IMP_PushNewPosition(&pstTgt->stTrajectory, &stPositions);
			}
			pstTgt++;
		}
		m = 0;
		IMP_U16 TmpID;
		for(n=0; n<10; n++) if(pstTarget->SpecialID[n]) { TmpID = pstTarget->SpecialID[n]; m++; }
		if(m==1)
		{
			IMP_Target_S *pstTgt = pstTrackTgtSet->astTarget;
			for(j=0; j<IMP_MAX_TGT_CNT; j++)
			{
				if(pstTgt->TargetID==TmpID)
				{
					TrackedTgtFlag[j] = 1;
					pstTarget->u8Used = 0;
					IMP_DeleteTarget(pstTarget, pstResult);
					pstTgt->u8MoveEstimate = 0;
					IMP_TgtPosition_S stPositions;
					stPositions.stRg = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].stRg;
					GetCentroid(pstResult, ptrImgNow, &stPositions.stRg, &stPositions.stPt);
					stPositions.s32Velocity = 0;
					stPositions.s32Direction = 0;
					stPositions.VX = 0.0;
					stPositions.VY = 0.0;
					stPositions.ErrorX = 0.0;
					stPositions.ErrorY = 0.0;
					stPositions.u32AreaPixel = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].u32AreaPixel;
					pstTgt->u8Sign = pstTarget->u8Sign;
					for(n=0; n<10; n++)
					{
						pstTgt->Signs[n] = pstTarget->Signs[n];
						pstTgt->Pixels[n] = pstTarget->Pixels[n];
					}
					IMP_PushNewPosition(&pstTgt->stTrajectory, &stPositions);
				}
					pstTgt++;
			}
		}
	}
}

void MergeAnalysis(PEA_RESULT_S *pstResult, IMP_TargetSet_S *pstTrackTgtSet)//合并分析
{
	IMP_U8 flag0 = 1;
	IMP_U8 i, j;
	IMP_U8 *ptrImgNow = (&pstResult->stDRegionSet)->pstImgFgRgn->pu8Data;
	while(flag0)
	{
		flag0 = 0;
		IMP_Target_S *pstTarget1 = pstTrackTgtSet->astTarget;
		for(i=0; i<IMP_MAX_TGT_CNT; i++)
		{
			if(pstTarget1->u8Used && pstTarget1->u8MoveEstimate==0)
			{
				IMP_Target_S *pstTarget2 = pstTarget1+1;
				for(j=i+1; j<IMP_MAX_TGT_CNT; j++)
				{
					if(pstTarget2->u8Used && pstTarget2->u8MoveEstimate==0)
					{
						IMP_U8 m, n, k;
						IMP_U8 flag = 0;
						for(m=0; pstTarget1->Signs[m]!=0&&m<10; m++)
						{
							for(n=0; pstTarget2->Signs[n]!=0&&n<10; n++)
								if(pstTarget1->Signs[m]==pstTarget2->Signs[n]) {  flag0 = 1; flag = 1; break; }
							if(flag) break;
						}
						if(flag)
						{
							//去除小目标
							if(pstTarget1->stTrajectory.astPositions[pstTarget1->stTrajectory.u16Num-1].u32AreaPixel<IMP_MIN_TGT_AREA) { pstTarget1->u8Used = 0; IMP_DeleteTarget(pstTarget1, pstResult); break;}
							if(pstTarget2->stTrajectory.astPositions[pstTarget2->stTrajectory.u16Num-1].u32AreaPixel<IMP_MIN_TGT_AREA) { pstTarget2->u8Used = 0; IMP_DeleteTarget(pstTarget1, pstResult); continue;}

							if(pstTarget1->u8Stable==0 && pstTarget2->u8Stable==0)
							{
								if(pstTarget1->stTrajectory.astPositions[pstTarget1->stTrajectory.u16Num-1].u32AreaPixel<pstTarget2->stTrajectory.astPositions[pstTarget2->stTrajectory.u16Num-1].u32AreaPixel)
								{ pstTarget1->u8Used = 0; IMP_DeleteTarget(pstTarget1, pstResult); }
								else
								{ pstTarget2->u8Used = 0; IMP_DeleteTarget(pstTarget2, pstResult); }
							}
							else
							{
								if(pstTarget1->u8Stable && pstTarget2->u8Stable)
								{
									if(pstTarget1->u8Special==0 && pstTarget2->u8Special==0)
									{
										IMP_Target_S *pstTarget = pstTrackTgtSet->astTarget;
										for(n = 0; n<IMP_MAX_TGT_CNT; n++)
										{

											if(!pstTarget->u8Used)
											{
												pstTarget->TargetID = pstTrackTgtSet->TargetTotal;
												pstTrackTgtSet->TargetTotal++;
												pstTarget->u8Used = 1;
												pstTarget->u8Special = 1;
												pstTarget->SpecialCount = 1;
												pstTarget->u8MoveEstimate = 0;
												for(m=0; m<10; m++)
												{
													pstTarget->Signs[m] = pstTarget1->Signs[m];
													pstTarget->Pixels[m] = pstTarget1->Pixels[m];
												}
												for(m=0; m<10; m++)
												{
													if(pstTarget2->Signs[m]!=0)
													{
														for(k=0; k<10; k++)
														{
															if(pstTarget->Signs[k]==pstTarget2->Signs[m])
																break;
															if(pstTarget->Signs[k]==0)
															{
																pstTarget->Signs[k] = pstTarget2->Signs[m];
																pstTarget->Pixels[k] = pstTarget2->Pixels[m];
																break;
															}
														}
													}
												}
												for(m=0; m<10; m++) pstTarget1->Signs[m] = 0;
												for(m=0; m<10; m++) pstTarget2->Signs[m] = 0;
												for(m=0; m<10; m++) pstTarget->SpecialID[m] = 0;
												pstTarget->SpecialID[0] = pstTarget1->TargetID;
												pstTarget->SpecialID[1] = pstTarget2->TargetID;
												pstTarget->u8Stable = 1;
												pstTarget->stTrajectory.u16Num = 0;
												IMP_TgtPosition_S stPositions;
												stPositions.u8Used = 1;
												stPositions.stRg = pstTarget1->stTrajectory.astPositions[pstTarget1->stTrajectory.u16Num-1].stRg;
												IMP_RECT_S Rt = pstTarget2->stTrajectory.astPositions[pstTarget2->stTrajectory.u16Num-1].stRg;
												if(Rt.s16X1<stPositions.stRg.s16X1) stPositions.stRg.s16X1 = Rt.s16X1;
												if(Rt.s16Y1<stPositions.stRg.s16Y1) stPositions.stRg.s16Y1 = Rt.s16Y1;
												if(Rt.s16X2>stPositions.stRg.s16X2) stPositions.stRg.s16X2 = Rt.s16X2;
												if(Rt.s16Y2>stPositions.stRg.s16Y2) stPositions.stRg.s16Y2 = Rt.s16Y2;
												GetCentroid(pstResult, ptrImgNow, &stPositions.stRg, &stPositions.stPt);
												IMP_U32 SumPixels = pstTarget->Pixels[0];
												IMP_U32 TmpPixels = pstTarget->Pixels[0];
												IMP_U8  TmpSign = pstTarget->Signs[0];
												for(m=1; m<10; m++)
												{
													SumPixels += pstTarget->Pixels[m];
													if(pstTarget->Pixels[m]>TmpPixels) { TmpPixels = pstTarget->Pixels[m]; TmpSign = pstTarget->Signs[m]; }
												}
												pstTarget->u8Sign = TmpSign;
												stPositions.u32AreaPixel = SumPixels;
												stPositions.s32Velocity = 0;
												stPositions.s32Direction = 0;
												stPositions.VX = 0.0;
												stPositions.VY = 0.0;
												stPositions.ErrorX = 0.0;
												stPositions.ErrorY = 0.0;
												IMP_PushNewPosition(&pstTarget->stTrajectory, &stPositions);
												IMP_CreateTarget(pstTarget, pstResult);
												pstTarget1->SpecialID[0] = pstTarget->TargetID;
												pstTarget2->SpecialID[0] = pstTarget->TargetID;
												break;
											}
											pstTarget++;
										}
										pstTarget1->u8MoveEstimate = 1;
										pstTarget2->u8MoveEstimate = 1;
										MoveEstimate(pstTarget1, pstTrackTgtSet, pstResult, 1);
										MoveEstimate(pstTarget2, pstTrackTgtSet, pstResult, 1);
									}
									else
									{
										if(pstTarget1->u8Special&&pstTarget2->u8Special)
										{
											for(m=0; m<10; m++)
											{
												if(pstTarget2->Signs[m]!=0)
												{
													for(k=0; k<10; k++)
													{
														if(pstTarget1->Signs[k]==pstTarget2->Signs[m])
															break;
														if(pstTarget1->Signs[k]==0)
														{
															pstTarget1->Signs[k] = pstTarget2->Signs[m];
															pstTarget1->Pixels[k] = pstTarget2->Pixels[m];
															break;
														}
													}
												}
											}
											for(m=0; m<10; m++)
											{
												if(pstTarget2->SpecialID[m]!=0)
												{
													for(k=0; k<10; k++)
													{
														if(pstTarget1->SpecialID[k]==pstTarget2->SpecialID[m])
															break;
														if(pstTarget1->SpecialID[k]==0)
														{
															pstTarget1->SpecialID[k] = pstTarget2->SpecialID[m];
															break;
														}
													}
												}
											}
											IMP_TgtPosition_S stPositions;
											stPositions.u8Used = 1;
											stPositions.stRg = pstTarget1->stTrajectory.astPositions[pstTarget1->stTrajectory.u16Num-1].stRg;
											IMP_RECT_S Rt = pstTarget2->stTrajectory.astPositions[pstTarget2->stTrajectory.u16Num-1].stRg;
											if(Rt.s16X1<stPositions.stRg.s16X1) stPositions.stRg.s16X1 = Rt.s16X1;
											if(Rt.s16Y1<stPositions.stRg.s16Y1) stPositions.stRg.s16Y1 = Rt.s16Y1;
											if(Rt.s16X2>stPositions.stRg.s16X2) stPositions.stRg.s16X2 = Rt.s16X2;
											if(Rt.s16Y2>stPositions.stRg.s16Y2) stPositions.stRg.s16Y2 = Rt.s16Y2;
											GetCentroid(pstResult, ptrImgNow, &stPositions.stRg, &stPositions.stPt);
											IMP_U32 SumPixels = pstTarget1->Pixels[0];
											IMP_U32 TmpPixels = pstTarget1->Pixels[0];
											IMP_U8  TmpSign = pstTarget1->Signs[0];
											for(m=1; m<10; m++)
											{
												SumPixels += pstTarget1->Pixels[m];
												if(pstTarget1->Pixels[m]>TmpPixels) { TmpPixels = pstTarget1->Pixels[m]; TmpSign = pstTarget1->Signs[m]; }
											}
											pstTarget1->u8Sign = TmpSign;
											stPositions.u32AreaPixel = SumPixels;
											stPositions.s32Velocity = 0;
											stPositions.s32Direction = 0;
											stPositions.VX = 0.0;
											stPositions.VY = 0.0;
											stPositions.ErrorX = 0.0;
											stPositions.ErrorY = 0.0;
											IMP_PushNewPosition(&pstTarget1->stTrajectory, &stPositions);

											IMP_Target_S *pstTarget = pstTrackTgtSet->astTarget;
											for(n = 0; n<IMP_MAX_TGT_CNT; n++)
											{
												if(pstTarget->u8Used)
												{
													IMP_U8 flag = 0;
													for(m=0; m<10; m++)
														if(pstTarget->TargetID==pstTarget1->SpecialID[m])
															flag =1;
													if(flag) pstTarget->SpecialID[0] = pstTarget1->TargetID;
												}
												pstTarget++;
											}
											pstTarget2->u8Used = 0;
											IMP_DeleteTarget(pstTarget2, pstResult);
										}
										if(pstTarget1->u8Special==0)
										{
											pstTarget1->u8MoveEstimate = 1;
											MoveEstimate(pstTarget1, pstTrackTgtSet, pstResult, 1);
											for(m=0; m<10; m++)
											{
												if(pstTarget1->Signs[m]!=0)
												{
													for(k=0; k<10; k++)
													{
														if(pstTarget2->Signs[k]==pstTarget1->Signs[m])
															break;
														if(pstTarget2->Signs[k]==0)
														{
															pstTarget2->Signs[k] = pstTarget1->Signs[m];
															pstTarget2->Pixels[k] = pstTarget1->Pixels[m];
															break;
														}
													}
												}
											}
											for(n=0; n<10; n++) pstTarget1->Signs[n] = 0;
											for(n=0; n<10; n++)
											if(pstTarget2->SpecialID[n]==0)
											{
												pstTarget2->SpecialID[n] = pstTarget1->TargetID;
												break;
											}
											pstTarget1->SpecialID[0] = pstTarget2->TargetID;
										}
										if(pstTarget2->u8Special==0)
										{
											pstTarget2->u8MoveEstimate = 1;
											MoveEstimate(pstTarget2, pstTrackTgtSet, pstResult, 1);
											for(m=0; m<10; m++)
											{
												if(pstTarget2->Signs[m]!=0)
												{
													for(k=0; k<10; k++)
													{
														if(pstTarget1->Signs[k]==pstTarget2->Signs[m])
															break;
														if(pstTarget1->Signs[k]==0)
														{
															pstTarget1->Signs[k] = pstTarget2->Signs[m];
															pstTarget1->Pixels[k] = pstTarget2->Pixels[m];
															break;
														}
													}
												}
											}
											for(n=0; n<10; n++) pstTarget2->Signs[n] = 0;
											for(n=0; n<10; n++) if(pstTarget1->SpecialID[n]==0) { pstTarget1->SpecialID[n] = pstTarget2->TargetID; break; }
											pstTarget2->SpecialID[0] = pstTarget1->TargetID;
										}	
									}
								}
								else
								{
									if(pstTarget1->u8Stable==0) { pstTarget1->u8Used = 0; IMP_DeleteTarget(pstTarget1, pstResult); }
									if(pstTarget2->u8Stable==0) { pstTarget2->u8Used = 0; IMP_DeleteTarget(pstTarget2, pstResult); }
								}
							}
						}
					}
					pstTarget2++;
				}
			}
			pstTarget1++;
		}
	}
}

IMP_U8 SplitAnalysis(IMP_Target_S *pstTarget, PEA_RESULT_S *pstResult, IMP_RECT_S *pstRt, IMP_U8 *Signs, IMP_U8 *TmpFrame, IMP_U8 *TmpFrame0, RectTmp_S *RectTmp)//分裂分析
{
	IMP_U8 *ptrImgNow = (&pstResult->stDRegionSet)->pstImgFgRgn->pu8Data;
	GetRegion(pstResult, ptrImgNow, pstRt, Signs, TmpFrame);
	memcpy(TmpFrame0,TmpFrame,pstResult->s32Width*pstResult->s32Height);

	IMP_S32 j, k, n = 0;			
	IMP_U8 m = 0;
	while(m<IMP_Max_SplitDistance)//距离
	{
		IMP_U8* ptrPixel1[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };//中心和邻域
		IMP_U8* ptrPixel2[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
		for(j=pstRt->s16Y1+1; j<pstRt->s16Y2; j++)
		for(k=pstRt->s16X1+1; k<pstRt->s16X2; k++)
		{
			ptrPixel1[0] = TmpFrame+j*pstResult->s32Width+k;
			ptrPixel1[1] = TmpFrame+j*pstResult->s32Width+k-1;
			ptrPixel1[2] = TmpFrame+j*pstResult->s32Width+k+1;
			ptrPixel1[3] = TmpFrame+(j-1)*pstResult->s32Width+k;
			ptrPixel1[4] = TmpFrame+(j+1)*pstResult->s32Width+k;
			ptrPixel1[5] = TmpFrame+(j-1)*pstResult->s32Width+k-1;
			ptrPixel1[6] = TmpFrame+(j+1)*pstResult->s32Width+k-1;
			ptrPixel1[7] = TmpFrame+(j-1)*pstResult->s32Width+k+1;
			ptrPixel1[8] = TmpFrame+(j+1)*pstResult->s32Width+k+1;

			ptrPixel2[0] = TmpFrame0+j*pstResult->s32Width+k;
			ptrPixel2[1] = TmpFrame0+j*pstResult->s32Width+k-1;
			ptrPixel2[2] = TmpFrame0+j*pstResult->s32Width+k+1;
			ptrPixel2[3] = TmpFrame0+(j-1)*pstResult->s32Width+k;
			ptrPixel2[4] = TmpFrame0+(j+1)*pstResult->s32Width+k;
			ptrPixel2[5] = TmpFrame0+(j-1)*pstResult->s32Width+k-1;
			ptrPixel2[6] = TmpFrame0+(j+1)*pstResult->s32Width+k-1;
			ptrPixel2[7] = TmpFrame0+(j-1)*pstResult->s32Width+k+1;
			ptrPixel2[8] = TmpFrame0+(j+1)*pstResult->s32Width+k+1;
			if(*ptrPixel1[0])
			{
				IMP_U8 flag = 0;
				for(n=1; n<9; n++)
				{
					if((*ptrPixel1[n])!=(*ptrPixel1[0]))
					{
						if((*ptrPixel1[n])!=0) { flag = 2; break; }
						else flag = 1;
					}
				}
				if(flag==2)
				{
					IMP_U8 p, q;
					for(p=0; p<10; p++){ if(RectTmp[p].Signs[0]==(*ptrPixel1[0])) break; }
					for(q=0; q<10; q++)
					{
						if(RectTmp[p].Signs[q]==(*ptrPixel1[n])) break; 
						else if(RectTmp[p].Signs[q]==0){ RectTmp[p].Signs[q] = *ptrPixel1[n]; break; }
					}
					*ptrPixel2[1] = *ptrPixel1[0];
					*ptrPixel2[2] = *ptrPixel1[0];
					*ptrPixel2[3] = *ptrPixel1[0];
					*ptrPixel2[4] = *ptrPixel1[0];
					*ptrPixel2[5] = *ptrPixel1[0];
					*ptrPixel2[6] = *ptrPixel1[0];
					*ptrPixel2[7] = *ptrPixel1[0];
					*ptrPixel2[8] = *ptrPixel1[0];
				}
				else
				{
					if(flag==1)
					{
						*ptrPixel2[1] = *ptrPixel1[0];
						*ptrPixel2[2] = *ptrPixel1[0];
						*ptrPixel2[3] = *ptrPixel1[0];
						*ptrPixel2[4] = *ptrPixel1[0];
						*ptrPixel2[5] = *ptrPixel1[0];
						*ptrPixel2[6] = *ptrPixel1[0];
						*ptrPixel2[7] = *ptrPixel1[0];
						*ptrPixel2[8] = *ptrPixel1[0];
					}
				}
			}
		}
		memcpy(TmpFrame,TmpFrame0,pstResult->s32Width*pstResult->s32Height);
		m++;
	}

	IMP_U8 p, q, r;
	for(j=0; j<10; j++)
	{
		if(RectTmp[j].u8Used)
		{
			for(k=1; k<10; k++)
			for(p=j+1; p<10; p++)
			{ 
				if(RectTmp[p].u8Used && RectTmp[p].Signs[0]==RectTmp[j].Signs[k]) 
				{
					RectTmp[p].u8Used = 0;
					if(RectTmp[p].Rect.s16X1<RectTmp[j].Rect.s16X1) RectTmp[j].Rect.s16X1 = RectTmp[p].Rect.s16X1;
					if(RectTmp[p].Rect.s16Y1<RectTmp[j].Rect.s16Y1) RectTmp[j].Rect.s16Y1 = RectTmp[p].Rect.s16Y1;
					if(RectTmp[p].Rect.s16X2>RectTmp[j].Rect.s16X2) RectTmp[j].Rect.s16X2 = RectTmp[p].Rect.s16X2;
					if(RectTmp[p].Rect.s16Y2>RectTmp[j].Rect.s16Y2) RectTmp[j].Rect.s16Y2 = RectTmp[p].Rect.s16Y2;
					for(r=1; r<10; r++) if(RectTmp[j].Pixels[r]==0) { RectTmp[j].Pixels[r] = RectTmp[p].Pixels[0]; break; }
					for(r=1; r<10; r++)
					{
						if(RectTmp[p].Signs[r]!=0)
						{
							for(q=0; q<10; q++)
							{
								if(RectTmp[j].Signs[q]==RectTmp[p].Signs[r]) break;
								if(RectTmp[j].Signs[q]==0) { RectTmp[j].Signs[q]=RectTmp[p].Signs[r]; break; }
							}
						}
					}
					break;
				}
			}
		}
	}
	IMP_U8 SeparateNum = 0;
	for(p=0; p<10; p++)
	{ 
		if(RectTmp[p].u8Used) SeparateNum++;
	}
	return SeparateNum;
}

IMP_U8 OverlapAnalysis(IMP_Target_S *pstTarget, PEA_RESULT_S *pstResult, IMP_RECT_S *pstRt, IMP_U8 *Signs, IMP_U8 *LastFrame, RectTmp_S *RectTmp, IMP_U8 *TmpFrame)//重叠分析
{
	IMP_S32 i, j, n, k;
	PEA_DETECTED_REGION_S *pstDrg;
	IMP_U8 *ptrImgNow = (&pstResult->stDRegionSet)->pstImgFgRgn->pu8Data;
	IMP_RECT_S stRt = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].stRg;
	GetRegion(pstResult, LastFrame, &stRt, pstTarget->Signs, TmpFrame);
	IMP_S32 overlapPixel[10] = {1,1,1,1,1,1,1,1,1,1};
	IMP_U8 overlapFlag = 0, TargetSign = 0, TrackFlag = 0;
	for(j=stRt.s16Y1; j<=stRt.s16Y2; j++)
	{
		for(k=stRt.s16X1; k<=stRt.s16X2; k++)
		{
			if(ptrImgNow[j*pstResult->s32Width+k]!=0 && TmpFrame[j*pstResult->s32Width+k]!=0)
			{
				if(overlapFlag==0) { Signs[0] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[0]) overlapPixel[0]++;
				else if(overlapFlag==1) { Signs[1] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[1]) overlapPixel[1]++;
				else if(overlapFlag==2) { Signs[2] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[2]) overlapPixel[2]++;
				else if(overlapFlag==3) { Signs[3] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[3]) overlapPixel[3]++;
				else if(overlapFlag==4) { Signs[4] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[4]) overlapPixel[4]++;
				else if(overlapFlag==5) { Signs[5] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[5]) overlapPixel[5]++;
				else if(overlapFlag==6) { Signs[6] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[6]) overlapPixel[6]++;
				else if(overlapFlag==7) { Signs[7] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[7]) overlapPixel[7]++;
				else if(overlapFlag==8) { Signs[8] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[8]) overlapPixel[8]++;
				else if(overlapFlag==9) { Signs[9] = ptrImgNow[j*pstResult->s32Width+k]; overlapFlag++; }
				else if(ptrImgNow[j*pstResult->s32Width+k]==Signs[9]) overlapPixel[9]++; 
			}
		}
	}

	j = 0;
	i = (IMP_S32)overlapFlag;
	memset(RectTmp, 0, sizeof(RectTmp_S)*10);
	while(i>0)
	{
		pstDrg = (&pstResult->stDRegionSet)->astDrg;
		for(n=0; n<IMP_MAX_TGT_CNT; n++)
		{
			if(pstDrg->u8Used && pstDrg->u8Sign==Signs[i-1])
			{
				if(!TrackFlag)
				{
					*pstRt = pstDrg->stRect;
					RectTmp[j].u8Used = 1;
					RectTmp[j].Rect = pstDrg->stRect; 
					RectTmp[j].Pixels[0] = (IMP_U32)pstDrg->s32AreaPixel;
					RectTmp[j].Signs[0] = pstDrg->u8Sign;
					TrackFlag = 1;	
					j++;
					break;
				}
				else
				{
					if(pstDrg->stRect.s16X1<pstRt->s16X1) pstRt->s16X1 = pstDrg->stRect.s16X1;
					if(pstDrg->stRect.s16Y1<pstRt->s16Y1) pstRt->s16Y1 = pstDrg->stRect.s16Y1;
					if(pstDrg->stRect.s16X2>pstRt->s16X2) pstRt->s16X2 = pstDrg->stRect.s16X2;
					if(pstDrg->stRect.s16Y2>pstRt->s16Y2) pstRt->s16Y2 = pstDrg->stRect.s16Y2;
					RectTmp[j].u8Used = 1;
					RectTmp[j].Rect = pstDrg->stRect; 
					RectTmp[j].Pixels[0] = (IMP_U32)pstDrg->s32AreaPixel;
					RectTmp[j].Signs[0] = pstDrg->u8Sign;	
					j++;
					break;
				}
			}
			pstDrg++;
		}
		i--;
	}
	if(TrackFlag) return overlapFlag;
	else return 0;
}

void GetRegion(PEA_RESULT_S *pstResult, IMP_U8 *ptrImg, IMP_RECT_S *pstRt, IMP_U8 *Signs, IMP_U8 *TmpFrame)
{
	IMP_S32 j, k, n;
	memset(TmpFrame, 0, pstResult->s32Width*pstResult->s32Height);
	for(j=pstRt->s16Y1; j<=pstRt->s16Y2; j++)
	{
		for(k=pstRt->s16X1; k<=pstRt->s16X2; k++)
		{
			if(ptrImg[j*pstResult->s32Width+k]!=0)
			{
				for(n=0; n<10; n++)
				if(ptrImg[j*pstResult->s32Width+k]==Signs[n])
				{ 
					TmpFrame[j*pstResult->s32Width+k] = ptrImg[j*pstResult->s32Width+k];
					break;
				}
			}
		}
	}
}

void MoveEstimate(IMP_Target_S *pstTarget, IMP_TargetSet_S *pstTrackTgtSet, PEA_RESULT_S *pstResult, IMP_U8 flag)//运动估计
{
	IMP_Target_S *pstTgt;
	IMP_U8 i, j;
	if(flag)
	{
		IMP_TgtPosition_S stPositions = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-2];
		IMP_TgtPosition_S* pstPositions = &pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1];
		pstPositions->stRg.s16X1 = (IMP_S16)(stPositions.stRg.s16X1-stPositions.ErrorX+stPositions.VX);
		pstPositions->stRg.s16Y1 = (IMP_S16)(stPositions.stRg.s16Y1-stPositions.ErrorY+stPositions.VY);
		pstPositions->stRg.s16X2 = (IMP_S16)(stPositions.stRg.s16X2-stPositions.ErrorX+stPositions.VX);
		pstPositions->stRg.s16Y2 = (IMP_S16)(stPositions.stRg.s16Y2-stPositions.ErrorY+stPositions.VY);
		pstPositions->stPt.s16X = (IMP_S16)(stPositions.stPt.s16X-stPositions.ErrorX+stPositions.VX);
		pstPositions->stPt.s16Y = (IMP_S16)(stPositions.stPt.s16Y-stPositions.ErrorY+stPositions.VY);
		pstPositions->u32AreaPixel = stPositions.u32AreaPixel;
		pstPositions->s32Velocity = stPositions.s32Velocity;
		pstPositions->s32Direction = stPositions.s32Direction;
		pstPositions->VX = stPositions.VX;
		pstPositions->VY = stPositions.VY;
		pstPositions->ErrorX = (double)(pstPositions->stRg.s16X1-(stPositions.stRg.s16X1-stPositions.ErrorX+stPositions.VX));
		pstPositions->ErrorY = (double)(pstPositions->stRg.s16Y1-(stPositions.stRg.s16Y1-stPositions.ErrorY+stPositions.VY));
	}
	else
	{
		IMP_TgtPosition_S stPositions1 = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1];
		IMP_TgtPosition_S stPositions2 = stPositions1;
		stPositions2.stRg.s16X1 = (IMP_S16)(stPositions1.stRg.s16X1-stPositions1.ErrorX+stPositions1.VX);
		stPositions2.stRg.s16Y1 = (IMP_S16)(stPositions1.stRg.s16Y1-stPositions1.ErrorY+stPositions1.VY);
		stPositions2.stRg.s16X2 = (IMP_S16)(stPositions1.stRg.s16X2-stPositions1.ErrorX+stPositions1.VX);
		stPositions2.stRg.s16Y2 = (IMP_S16)(stPositions1.stRg.s16Y2-stPositions1.ErrorY+stPositions1.VY);
		stPositions2.stPt.s16X = (IMP_S16)(stPositions1.stPt.s16X-stPositions1.ErrorX+stPositions1.VX);
		stPositions2.stPt.s16Y = (IMP_S16)(stPositions1.stPt.s16Y-stPositions1.ErrorY+stPositions1.VY);
		stPositions2.ErrorX = (double)(stPositions2.stRg.s16X1-(stPositions1.stRg.s16X1-stPositions1.ErrorX+stPositions1.VX));
		stPositions2.ErrorY = (double)(stPositions2.stRg.s16Y1-(stPositions1.stRg.s16Y1-stPositions1.ErrorY+stPositions1.VY));
		if(stPositions2.stRg.s16X1<0||stPositions2.stRg.s16Y1<0||stPositions2.stRg.s16X2>=pstResult->s32Width||stPositions2.stRg.s16Y2>=pstResult->s32Height) 			{
			pstTgt = pstTrackTgtSet->astTarget;
			IMP_U8 i, j;
			for(i=0; i<IMP_MAX_TGT_CNT; i++)
			{
				if(pstTgt->u8Used && pstTgt->TargetID==pstTarget->SpecialID[0])
				{
					for(j=0; j<10; j++)
						if(pstTgt->SpecialID[j]==pstTarget->TargetID)
							pstTgt->SpecialID[j] = 0;
					break;
				}
				pstTgt++;
			}
			pstTarget->u8Used = 0;
			IMP_DeleteTarget(pstTarget, pstResult);
		}
		IMP_PushNewPosition(&pstTarget->stTrajectory, &stPositions2);
	}

	pstTgt = pstTrackTgtSet->astTarget;
	for(i=0; i<IMP_MAX_TGT_CNT; i++)
	{
		if(pstTgt->u8Used && pstTgt->TargetID==pstTarget->SpecialID[0])
		{
			IMP_RECT_S stRt1 = pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].stRg;
			IMP_RECT_S stRt2 = pstTgt->stTrajectory.astPositions[pstTgt->stTrajectory.u16Num-1].stRg;
			if(ImpGetDist(&stRt1,&stRt2)>0)
			{
				for(j=0; j<10; j++)
				{
					if(pstTgt->SpecialID[j]==pstTarget->TargetID)
					{
						pstTgt->SpecialID[j] = 0;
					}
				}
				pstTarget->u8Used = 0;
				IMP_DeleteTarget(pstTarget, pstResult);
			}
			break;
		}
		pstTgt++;
	}
}

void GetCentroid(PEA_RESULT_S *pstResult, IMP_U8 *ptrImg, IMP_RECT_S *pstRect, IMP_POINT_S *pstCentroid)
{
	int j,k;
	int X = 0; int Y = 0; int i = 0;
	for(j=pstRect->s16Y1; j<=pstRect->s16Y2; j++)
	{
		for(k=pstRect->s16X1; k<=pstRect->s16X2; k++)
		{
			if(ptrImg[j*pstResult->s32Width+k]!=0)
			{
				X += k;
				Y += j;
				i++;
			}
		}
	}
	pstCentroid->s16X = (IMP_S16)((double)X/(double)i);
	pstCentroid->s16Y = (IMP_S16)((double)Y/(double)i);
}

double ImpGetDist(IMP_RECT_S *pstRect1, IMP_RECT_S *pstRect2)
{
	double dist = 0;
	if(pstRect1->s16X2<pstRect2->s16X1)
	{
		if(pstRect1->s16Y2<pstRect2->s16Y1) dist = sqrt(pow(pstRect2->s16X1-pstRect1->s16X2,2)+pow(pstRect2->s16Y1-pstRect1->s16Y2,2));
		else 
		{
			if(pstRect1->s16Y1>pstRect2->s16Y2) dist = sqrt(pow(pstRect2->s16X1-pstRect1->s16X2,2)+pow(pstRect1->s16Y1-pstRect2->s16Y2,2));
			else dist = pstRect2->s16X1-pstRect1->s16X2;
		}
	}
	else
	{
		if(pstRect1->s16X1>pstRect2->s16X2)
		{
			if(pstRect1->s16Y2<pstRect2->s16Y1) dist = sqrt(pow(pstRect1->s16X1-pstRect2->s16X2,2)+pow(pstRect2->s16Y1-pstRect1->s16Y2,2));
			else 
			{
				if(pstRect1->s16Y1>pstRect2->s16Y2) dist = sqrt(pow(pstRect1->s16X1-pstRect2->s16X2,2)+pow(pstRect1->s16Y1-pstRect2->s16Y2,2));
				else dist = pstRect1->s16X1-pstRect2->s16X2;
			}
		}
		else
		{
			if(pstRect1->s16Y2<pstRect2->s16Y1) dist = pstRect2->s16Y1-pstRect1->s16Y2;
			else
			{
				if(pstRect1->s16Y1>pstRect2->s16Y2) dist = pstRect1->s16Y1-pstRect2->s16Y2;
			}
		}
	}
	return dist;	
}

IMP_S32 impFindTargetOutById(IpTrackedTargetSet *pstTTS, IMP_U32 u32ID)
{
	IMP_S32 s32TI;
	IpTrackedTarget *pstTargetOut = 0;
	
	for (s32TI = 1; s32TI < IMP_MAX_TGT_CNT; s32TI++)
	{
		pstTargetOut = &pstTTS->astTargets[s32TI];
		if (pstTargetOut->s32Used && pstTargetOut->u32TargetId == u32ID)
		{
			return s32TI;
		}
	}
	
	return 0; //cannot find object
}


IMP_S32 impOutputTargetOut(IMP_Target_S *pstTarget, IMP_TargetSet_S *pstTrackTgtSet, PEA_RESULT_S *pstResult)
{
	IpTrackedTarget *pstTargetOut = 0;
	IpTrackedTarget *pstTargets = pstResult->stTrackedTargetSet.astTargets;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstResult->stDRegionSet;
	IMP_Target_S *pstTgts = pstTrackTgtSet->astTarget;
	IMP_S32 i = 0, j = 0; 
	IMP_S32 Total = 0;
	while(i<IMP_MAX_TGT_CNT)
	{
		if(pstTgts->u8Used && (!pstTgts->u8Special))
		{
			IMP_S32 s32TI = impFindTargetOutById(&pstResult->stTrackedTargetSet, (IMP_U32)pstTgts->TargetID);
			if (s32TI == 0) { pstTgts++; i++; continue; } //cannot find object
			pstTargetOut = &pstTargets[s32TI];
		//	pstTargets->s32Used = 1;
			if(pstTgts->u8Stable) 
			{
				IMP_SET_TGT_STABLE(pstTargetOut->stTargetInfo.u32Status);
				IMP_SET_TGT_MEASURED(pstTargetOut->stTargetInfo.u32Status);
				IMP_SET_TGT_MTREND(pstTargetOut->stTargetInfo.u32Status);
			}
		//	pstTargets->u32TargetId = (IMP_U32)pstTgts->TargetID;
			pstTargetOut->stTargetImage.pstDRegions = pstDRegionSet;
			pstTargetOut->stTargetImage.s32DrgIndex = (IMP_S32)pstTgts->u8Sign;
			for(j=0; j<pstTgts->stTrajectory.u16Num; j++)
			{
				pstTargetOut->stTrajectory.astPositions[j].s32Used = (IMP_S32)pstTgts->stTrajectory.astPositions[j].u8Used;
				pstTargetOut->stTrajectory.astPositions[j].stPt = pstTgts->stTrajectory.astPositions[j].stPt;
				pstTargetOut->stTrajectory.astPositions[j].stRg = pstTgts->stTrajectory.astPositions[j].stRg;
				pstTargetOut->stTrajectory.astPositions[j].u32AreaPixel = pstTgts->stTrajectory.astPositions[j].u32AreaPixel;
				pstTargetOut->stTrajectory.astPositions[j].s32Velocity = pstTgts->stTrajectory.astPositions[j].s32Velocity;
				pstTargetOut->stTrajectory.astPositions[j].s32Direction = pstTgts->stTrajectory.astPositions[j].s32Direction;
			}
			pstTargetOut->stTrajectory.s32Num = pstTgts->stTrajectory.u16Num;
			Total++;
		}
		pstTgts++;
		i++;
	}
	pstResult->stTrackedTargetSet.s32UsedTotal = Total;
	
	return 0;
}


IMP_S32 IMP_DeleteTarget(IMP_Target_S *pstTarget, PEA_RESULT_S *pstResult)
{
	IMP_S32 s32TI;
	
	s32TI = impFindTargetOutById(&pstResult->stTrackedTargetSet, (IMP_U32)pstTarget->TargetID);
	if (s32TI == 0) return -1; //cannot find object
	
	ipTrackedTargetSetFree(&pstResult->stTrackedTargetSet, s32TI);
	
	return 0;
}

void IMP_PushNewPosition(IMP_TgtTrajectory_S *pstTrajectory, IMP_TgtPosition_S *pstPositions)//轨迹记录
{
	int i;
	if(pstTrajectory->u16Num<IMP_MAX_TRAJECTORY_LEN) 
	{
		pstTrajectory->astPositions[pstTrajectory->u16Num] = *pstPositions;
		for(i=pstTrajectory->u16Num+1; i<IMP_MAX_TRAJECTORY_LEN; i++) pstTrajectory->astPositions[i].u8Used = 0;
		pstTrajectory->u16Num++;
	}
	else
	{
		if(!pstTrajectory->astPositions[IMP_MAX_TRAJECTORY_LEN-1].u8Used) pstTrajectory->astPositions[pstTrajectory->u16Num-1] = *pstPositions;
		else
		{
			for(i=0; i<IMP_MAX_TRAJECTORY_LEN-1; i++) pstTrajectory->astPositions[i] = pstTrajectory->astPositions[i+1];
			pstTrajectory->astPositions[IMP_MAX_TRAJECTORY_LEN-1] = *pstPositions;
		}
	}

}

IMP_S32 IMP_CreateTarget(IMP_Target_S *pstTarget, PEA_RESULT_S *pstResult)
{
	IMP_S32 s32TI;
	IpTrackedTarget *pstTargetOut = 0;
	
	s32TI = ipTrackedTargetSetAlloc(&pstResult->stTrackedTargetSet);
	if (s32TI == 0) return -1; //cannot alloc memory for object
	
	pstTargetOut = &pstResult->stTrackedTargetSet.astTargets[s32TI];
	pstTargetOut->s32Used = 1;
	pstTargetOut->u32TargetId = (IMP_U32)pstTarget->TargetID;
//	printf("-----------------------TargetID:%d\n", pstTargetOut->u32TargetId);
	return 0;
}

#if 0
IMP_S32 IMP_ShowTarget(IMP_TargetSet_S *pstTrackTgtSet, GRAY_IMAGE_S *pstGrayImg)
{	
	IplImage *pstImgForShow = cvCreateImage(cvSize((int)pstGrayImg->s32W,(int)pstGrayImg->s32H),IPL_DEPTH_8U,3);
	uchar *ptrGrayData = (uchar*)pstGrayImg->pu8Data;

	int i,j;
	for(i=0; i<pstImgForShow->height; i++)
	{
		uchar *ptr_img = (uchar*)(pstImgForShow->imageData+i*pstImgForShow->widthStep);
		for(j=0; j<pstImgForShow->width; j++)
		{
#if 0
			ptr_img[3*j] = *ptrGrayData;
			ptr_img[3*j+1] = *ptrGrayData;
			ptr_img[3*j+2] = *ptrGrayData;
#endif
#if 1
			if(*ptrGrayData)
			{
				ptr_img[3*j] = 255;
				ptr_img[3*j+1] = 255;
				ptr_img[3*j+2] = 255;
			}
			else
			{
				ptr_img[3*j] = 0;
				ptr_img[3*j+1] = 0;
				ptr_img[3*j+2] = 0;
			}
#endif
			ptrGrayData++;
		}
	}

	IMP_Target_S *pstTarget = pstTrackTgtSet->astTarget;
	IMP_RECT_S *pstRect;
	CvFont font;  
	cvInitFont(&font, 0, 0.5, 0.5, 0, 1, 8); 
       i = 0;
	while(i<10)
	{
		if(pstTarget->u8Used && pstTarget->u8Stable)// && (!pstTarget->u8Special))
		{
			pstRect = &pstTarget->stTrajectory.astPositions[pstTarget->stTrajectory.u16Num-1].stRg;
			cvRectangle(pstImgForShow,cvPoint((int)pstRect->s16X1,(int)pstRect->s16Y1),cvPoint((int)pstRect->s16X2,(int)pstRect->s16Y2),cvScalar(ColorTable[i*3],ColorTable[i*3+1],ColorTable[i*3+2],0),1,8,0);

			for(j=0; j<pstTarget->stTrajectory.u16Num; j++)
			{
				IMP_S16 X = pstTarget->stTrajectory.astPositions[j].stPt.s16X;
				IMP_S16 Y = pstTarget->stTrajectory.astPositions[j].stPt.s16Y;
				pstImgForShow->imageData[Y*pstImgForShow->widthStep + 3*X] = ColorTable[i*3];
				pstImgForShow->imageData[Y*pstImgForShow->widthStep + 3*X + 1] = ColorTable[i*3+1];
				pstImgForShow->imageData[Y*pstImgForShow->widthStep + 3*X + 2] = ColorTable[i*3+2];
			}


			char tmp1[10];
			sprintf(tmp1,"%d",pstTarget->TargetID); 
			cvPutText(pstImgForShow, tmp1, cvPoint((int)pstRect->s16X1,(int)pstRect->s16Y2), &font, cvScalar(ColorTable[i*3],ColorTable[i*3+1],ColorTable[i*3+2],0));
//			char tmp4[10];
//			sprintf(tmp4,"%d",pstTarget->stTrajectory.astPositions[0].s32Velocity); 
//			cvPutText(pstImgForShow, tmp4, cvPoint((int)pstRect->s16X1,(int)(pstRect->s16Y2-20)), &font, cvScalar(ColorTable[i*3],ColorTable[i*3+1],ColorTable[i*3+2],0));
//			char tmp5[10];
//			sprintf(tmp5,"%d",pstTarget->stTrajectory.astPositions[0].s32Direction); 
//			cvPutText(pstImgForShow, tmp5, cvPoint((int)pstRect->s16X1,(int)pstRect->s16Y2-40), &font, cvScalar(ColorTable[i*3],ColorTable[i*3+1],ColorTable[i*3+2],0));
		}
		pstTarget++;
		i++;
	}

	char tmp2[10];
	sprintf(tmp2,"frame:%d",frame);
	cvPutText(pstImgForShow, tmp2, cvPoint(240,20), &font, cvScalar(0,255,0,0));
	cvShowImage("test",pstImgForShow);

#if 0
	char tmp3[10];
	sprintf(tmp3,"%d.jpg",frame);
	if(frame>2900 && frame<3100)	cvSaveImage(tmp3,pstImgForShow,0);
#endif

	cvReleaseImage(&pstImgForShow);

	return 0;
}
#endif


















