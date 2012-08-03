
#include "imp_pea_tracker.h"


IMP_VOID ipCreateTargetTrackerInternal( IpTargetTracker *pstTarget, GA_HARDWARE_RS_S *pstHwResource, PEA_RESULT_S *pstResult )
{
	memset( pstTarget, 0, sizeof(IpTargetTracker) );

	pstTarget->pstResult = pstResult;
	pstTarget->pstHwResource = pstHwResource;
	
//	if (pstResult->s32ModuleSwitch & 1)
	{
		ipCreateMotionTrackerInternal( &pstTarget->stMotionModule, pstTarget->pstHwResource, pstTarget->pstResult );
	}


	if (pstResult->s32ModuleSwitch & 1)
	{
		ipStartMotionTrackerInternal( &pstTarget->stMotionModule );
	}


}

IMP_VOID ipReleaseTargetTrackerInternal( IpTargetTracker *pstTarget )
{
	PEA_RESULT_S *pstResult = pstTarget->pstResult;
	
//	if (pstResult->s32ModuleSwitch & 1)
	{
		ipReleaseMotionTrackerInternal( &pstTarget->stMotionModule );
	}
//#ifdef SUPPORT_OSC


	memset( pstTarget, 0, sizeof(IpTargetTracker) );
}

IMP_VOID ipConfigTargetTrackerInternal( IpTargetTracker *pstTarget, IpTrackerPara *pstTPara )
{
	IMP_S32 frm_dura = pstTPara->s32FrmDura;
	PEA_RESULT_S *pstResult = pstTarget->pstResult;

	memcpy( &pstTarget->stPara, pstTPara, sizeof(IpTrackerPara) );

	{
		pstTarget->stPara.s32TimeStaticVelocityMin /= frm_dura;
	}

	if (pstResult->s32ModuleSwitch & 1)
	{
		ipConfigMotionTrackerInternal( &pstTarget->stMotionModule, &pstTarget->stPara );
	}

}


extern 	IMP_S32 g_person_leave;
static IMP_VOID ipCalcRegionsActualArea( IpTrackerPara *pstTPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_S32 ipProcessTargetTrackerInternal( IpTargetTracker *pstTarget )
{
	PEA_STATUS_S *pstStatus = &pstTarget->pstResult->stSysStatus;
	PEA_RESULT_S *pstResult = pstTarget->pstResult;

	pstStatus->u32TrackerStatus = IMP_MOUDLE_STATUS_SUCCESS;



	ipCalcRegionsActualArea( &pstTarget->stPara, &pstTarget->pstResult->stDRegionSet );
//printf("ModuleSwitch:%d\n", pstResult->s32ModuleSwitch);
	//track motion object
	if (pstResult->s32ModuleSwitch & 1)
	{
		ipProcessMotionTrackerInternal( &pstTarget->stMotionModule );
	}

	return 1;
}

IMP_S32 ipPostProcessTargetTrackerInternal( IpTargetTracker *pstTarget )
{
	return 1;
}


static IMP_VOID ipCalcRegionsActualArea( IpTrackerPara *pstTPara, PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i, s32Cnt = 0, del;
	PEA_DETECTED_REGION_S *pstDrg;

	IMP_S32 s32RemoveCalbFailDRegion = pstTPara->s32RemoveCalbFailDRegion;
	FDEPTH_S *pstFDepth = &pstTPara->pstRule->stFDepth;
	IMP_S32 s32ImgW = pstRgs->pstImgFgRgn->s32W;
	IMP_S32 s32ImgH = pstRgs->pstImgFgRgn->s32H;

	IMP_ITERATE_RGS_START
	IMP_ASSERT( IMP_DRG_IS_COMPLETE(pstDrg->u8Used) );
	{
		del = ( 0
			|| ( IMP_FDEPTH_calcDRegionArea( pstFDepth, pstDrg ) != IMP_CLBR_SUCCESS )
			);
		if( del && s32RemoveCalbFailDRegion)
			ipDetectedRegionSetFree(pstRgs,i);
	}
	IMP_ITERATE_RGS_END
}


IMP_MODULE_HANDLE ipCreateTracker( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResouce )
{
	IMP_MODULE_HANDLE hModule=NULL;
	IpModuleTracker *pModule=NULL;
	IpTargetTracker *pTracker=NULL;

	pModule = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpModuleTracker) );
	pModule->pstTracker = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpTargetTracker) );
	pTracker = pModule->pstTracker;
	ipCreateTargetTrackerInternal( pTracker, pHwResouce, pstResult );
#ifdef LOG_DATA_TRACKER
	ipOpenLogFile();
#endif

	hModule = (IMP_MODULE_HANDLE)pModule;
	return hModule;
}

IMP_VOID ipReleaseTracker( IMP_MODULE_HANDLE hModule )
{
	IpModuleTracker *pModule = (IpModuleTracker*)hModule;
	IpTargetTracker *pTracker = pModule->pstTracker;
	GA_HARDWARE_RS_S *pHwResouce = pTracker->pstHwResource;

#ifdef LOG_DATA_TRACKER
	ipCloseLogFile();
#endif
	ipReleaseTargetTrackerInternal( pTracker );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pTracker );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule );
}

IMP_VOID ipConfigTracker( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstInnerPara )
{
	IpModuleTracker *pModule = (IpModuleTracker*)hModule;
	IpTargetTracker *pTracker = pModule->pstTracker;
	DATA_BUF_S *pDatBuf;
	IpTrackerPara pstTPara;
//	IP_PTZ_PARA ptzpara;

	pDatBuf = &pstInnerPara->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_TRACKER];
	ipParseTrackerParaData(&pstTPara );
	pDatBuf = &pstInnerPara->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_PTZ];
//	ipParseTrackerParaPtz( (IMP_S8*)pDatBuf->buffer, pDatBuf->dat_len, &ptzpara );
//	copy_ptz_para_p2t( &ptzpara, &pstTPara );
	ipForceInnerParaToTrackerPara( pstInnerPara, &pstTPara );
	ipConfigTargetTrackerInternal( pTracker, &pstTPara );
}

IMP_VOID TRACKER_Start( IMP_MODULE_HANDLE hModule )
{
	IpModuleTracker *pModule = (IpModuleTracker*)hModule;
	IpTargetTracker *pTracker = pModule->pstTracker;


}

IMP_VOID TRACKER_Stop( IMP_MODULE_HANDLE hModule )
{
	IpModuleTracker *pModule = (IpModuleTracker*)hModule;
	IpTargetTracker *pTracker = pModule->pstTracker;


}

IMP_S32 IMP_PEA_ProcessTracker( IMP_MODULE_HANDLE hModule )
{
	IpModuleTracker *pModule = (IpModuleTracker*)hModule;
	IpTargetTracker *pTracker = pModule->pstTracker;

	ipProcessTargetTrackerInternal( pTracker );

#ifdef LOG_DATA_TRACKER
	ipOutputTrackDataFrame(pModule->pstTracker);
#endif

	return 1;
}

IMP_S32 ipPostProcessTracker( IMP_MODULE_HANDLE hModule )
{
	IpModuleTracker *pModule = (IpModuleTracker*)hModule;
	IpTargetTracker *pTracker = pModule->pstTracker;

	return ipPostProcessTargetTrackerInternal( pTracker );
}


