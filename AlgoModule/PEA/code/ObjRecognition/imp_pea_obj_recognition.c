
#include "imp_pea_obj_recognition.h"

IMP_S32 IMP_GetMemSizeOfObjRecognition(IMP_S32 s32Width, IMP_S32 s32Height)
{
	IMP_S32 s32Size = 0, s32Size1 = 0;
	
	s32Size += sizeof(PEA_ModuleObjRecognition);
	s32Size1 = IMP_GetMemSizeDetector(s32Width, s32Height);
//	printf("GetMemSizeDetector:%d\n", s32Size1);
	s32Size += s32Size1;
	s32Size1 = IMP_GetMemSizePEATracker(s32Width, s32Height);
//	printf("GetMemSizeTracker:%d\n", s32Size1);
	s32Size += s32Size1;
	
	return s32Size;
}


IMP_MODULE_HANDLE IMP_PEA_CreateObjRecognition( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResouce )
{
	IMP_MODULE_HANDLE hModule=NULL;
	PEA_ModuleObjRecognition *pModule=NULL;

	pModule = IMP_MMAlloc( &pstHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(PEA_ModuleObjRecognition) );	// 64
	pModule->hDetector = ipCreateDetector( pstResult, pstHwResouce );
//	pModule->hTracker = ipCreateTracker( pstResult, pstHwResouce );
	pModule->hNewTracker = IMP_CreateTracker(pstResult, pstHwResouce);

	pModule->pstResult = pstResult;
	pModule->pstHwResouce = pstHwResouce;

	hModule = (IMP_MODULE_HANDLE)pModule;
	return hModule;
}

IMP_VOID IMP_PEA_ReleaseObjRecognition( IMP_MODULE_HANDLE hModule )
{
	PEA_ModuleObjRecognition *pModule = (PEA_ModuleObjRecognition*)hModule;
	GA_HARDWARE_RS_S *pHwResouce = pModule->pstHwResouce;
	
//	ipReleaseTracker( pModule->hTracker );
	IMP_ReleaseTracker(pModule->hNewTracker);
	
	ipReleaseDetector(pModule->hDetector);

	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule );
}

IMP_VOID IMP_PEA_ConfigObjRecognition( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstVapara )
{
	PEA_ModuleObjRecognition *pModule = (PEA_ModuleObjRecognition*)hModule;

	ipConfigDetector( pModule->hDetector, pstVapara );
//	ipConfigTracker( pModule->hTracker, pstVapara );
}
IMP_VOID IMP_PEA_StartObjRecognition( IMP_MODULE_HANDLE hModule )
{
	PEA_ModuleObjRecognition *pModule = (PEA_ModuleObjRecognition*)hModule;
}

IMP_VOID IMP_PEA_StopObjRecognition( IMP_MODULE_HANDLE hModule )
{
	PEA_ModuleObjRecognition *pModule = (PEA_ModuleObjRecognition*)hModule;
}


IMP_S32 IMP_PEA_TemperaryClassify(IpTrackedTarget *pstTarget);

IMP_S32 IMP_PEA_ProcessObjRecognition( IMP_MODULE_HANDLE hModule )
{
#define POR_TIME 0
	PEA_ModuleObjRecognition *pModule = (PEA_ModuleObjRecognition*)hModule;
	PEA_RESULT_S *pstResult = pModule->pstResult;

#if POR_TIME
struct timeval t1, t2;
#endif

#if POR_TIME
gettimeofday(&t1, NULL);
#endif

	IMP_PEA_ProcessDetector( pModule->hDetector );

#if POR_TIME
gettimeofday(&t2, NULL);
printf("ProcessDetector:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif


if (pstResult->s32ModuleSwitch & 1)
{

#if POR_TIME
gettimeofday(&t1, NULL);
#endif

//	IMP_PEA_ProcessTracker( pModule->hTracker );
	IMP_ProcessTracker(pModule->hNewTracker);

#if POR_TIME
gettimeofday(&t2, NULL);
printf("ProcessTracker:%d ms\n", (t2.tv_usec - t1.tv_usec) / 1000);
#endif

	
	IMP_PEA_TemperaryClassify(pModule->pstResult->stTrackedTargetSet.astTargets);
}

	return 1;
}


IMP_S32 IMP_PEA_TemperaryClassify(IpTrackedTarget *pstTarget)
{
	IpTargetPosition *pstPos0;
	IMP_RECT_S stRg;
	IMP_S32 i, k;
	IMP_U32 areaSize, rgSize;
	IMP_FLOAT fRatio;
	IpTargetInfo *pstTInfo;
	
	for(i=0; i < IMP_MAX_TGT_CNT; i++)
	{
		if (ipTrackedTargetIsActive(pstTarget))
		{
			IMP_S32 hl = 0, vl = 0;
			IMP_S32 len = pstTarget->stTrajectory.s32Num;
			
			pstTInfo = &pstTarget->stTargetInfo;
			
			len = len > 100 ? 100 : len;
			
			pstPos0 = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, 0);
			stRg = pstPos0->stRg;
			fRatio = abs(stRg.s16Y2 - stRg.s16Y1) / (IMP_FLOAT)abs(stRg.s16X2 - stRg.s16X1);
			rgSize = abs(stRg.s16Y2 - stRg.s16Y1) * abs(stRg.s16X2 - stRg.s16X1);
			areaSize = pstPos0->u32AreaPixel;
			
			pstPos0->u32Type = IMP_TGT_TYPE_UNKNOWN;
			
			if (1 || areaSize > 50 && areaSize * 100 > rgSize * 40)
			{
			//	if (fRatio > 1.2f) 
				if (fRatio >= 1.1f)
				{	
					pstPos0->u32Type = IMP_TGT_TYPE_HUMAN;
				//	printf("hi,");
				}
				else if(fRatio < 1.1f) 
				{
					pstPos0->u32Type = IMP_TGT_TYPE_VEHICLE;
				}
			}
			
			for (k = -len; k <= 0; k++)
			{
				pstPos0 = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, k);
//				printf("%d,%d; ", k, pstPos0->u32Type);
				if (pstPos0->u32Type == IMP_TGT_TYPE_HUMAN) hl++;
				else if(pstPos0->u32Type == IMP_TGT_TYPE_VEHICLE) vl++;
			}
			
			
			pstTInfo->s32VehicleLikehood = vl * 100 / len;
			pstTInfo->s32HumanLikehood = hl * 100 / len;
			pstTInfo->u32Type = IMP_TGT_TYPE_UNKNOWN;
			if ((hl + vl) * 100 > len * 30)
			{
				if (vl * 100 > (hl + vl) * 80) pstTInfo->u32Type = IMP_TGT_TYPE_VEHICLE;
				if (hl * 100 > (hl + vl) * 80) pstTInfo->u32Type = IMP_TGT_TYPE_HUMAN;
			}
			
//			printf("%d:[%.1f,%d; v_%d,h_%d] %d\n", pstTarget->u32TargetId, fRatio, areaSize * 100 / rgSize, pstTInfo->s32VehicleLikehood, pstTInfo->s32HumanLikehood, pstTInfo->u32Type);
		}
		
		pstTarget++;
	}
}


IMP_S32 IMP_PEA_PostProcessObjRecognition( IMP_MODULE_HANDLE hModule )
{
	PEA_ModuleObjRecognition *pModule = (PEA_ModuleObjRecognition*)hModule;

//	ipPostProcessTracker( pModule->hTracker );

	ipPostProcessDetector( pModule->hDetector );

	return 1;
}


