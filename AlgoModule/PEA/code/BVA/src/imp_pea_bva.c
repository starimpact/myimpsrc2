
#include "imp_pea_bva.h"


IMP_VOID ipCreateAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResouce )
{
	MEM_MGR_ARRAY_S *pstMemMgr;
	IMP_S32 i;
	IpTrackedTarget *pstTarget;


	TARGET_DATA_ANALYST_OSC_S *pstTarData;


	memset( pstBva, 0, sizeof(BEHAVIOR_ANALYSIS_S) );

	pstBva->pstResult = pstResult;
	pstBva->pHwResouce = pHwResouce;
	pstMemMgr = &pHwResouce->stMemMgr;

	// TARDAT
	{
		TARDAT_ANALYST_S *pstTarData;

		pstBva->pstTargetData = (TARDAT_ANALYST_S*)IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(TARDAT_ANALYST_S)*IMP_MAX_TGT_CNT );
		pstTarget = pstResult->stTrackedTargetSet.astTargets;
		pstTarData = pstBva->pstTargetData;
		for( i = 0; i < IMP_MAX_TGT_CNT; ++i )
		{
			pstTarget->stPrivateData.pDataAnalyst = pstTarData;
			pstTarget->stPrivateData.u32AnalystSz = sizeof(TARDAT_ANALYST_S);
			pstTarget++; pstTarData++;
		}
	}


if (pstResult->s32ModuleSwitch & 2)
{
	pstBva->pstAnalystOSC = (TARGET_DATA_ANALYST_OSC_S*)IMP_MMAlloc( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(TARGET_DATA_ANALYST_OSC_S)*IMP_MAX_TGT_CNT );
	pstTarget = pstResult->stTrackedTargetSetOsc.astTargets;
	pstTarData = pstBva->pstAnalystOSC;
	for( i=0; i<IMP_MAX_TGT_CNT; ++i )
	{
		pstTarget->stPrivateData.pDataAnalyst = pstTarData;
		pstTarget->stPrivateData.u32AnalystSz = sizeof(TARGET_DATA_ANALYST_OSC_S);
		pstTarget++; pstTarData++;
	}
}

#if defined(SUPPORT_PERIMETER) | defined(SUPPORT_ABNORMALVEL) | defined(SUPPORT_CONVERSE)
	ipCreatePerimeterAnalysti( &pstBva->stModulePerimeter, pstResult, pHwResouce );
#endif

#if defined(SUPPORT_TRIPWIRE)
	ipCreateTripwireAnalysti( &pstBva->stModuleTripwire, pstResult, pHwResouce );
#endif

#if defined(SUPPORT_MTRIPWIRE)
	ipCreateMTripwireAnalysti( &pstBva->stModuleMTripwire, pstResult, pHwResouce );
#endif
	
	if (pstResult->s32ModuleSwitch & 2)
	{
		ipCreateOscAnalysti( &pstBva->stModuleOSC, pstResult, pHwResouce );
	}
	
	ipCreateTargetEvtMgr( &pstBva->stTargetEvtMgr, pstResult );

}


IMP_VOID ipReleaseAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva )
{
	MEM_MGR_ARRAY_S *pstMemMgr = &pstBva->pHwResouce->stMemMgr;
	PEA_RESULT_S *pstResult = pstBva->pstResult;

	ipReleaseTargetEvtMgr( &pstBva->stTargetEvtMgr );


#if defined(SUPPORT_PERIMETER) | defined(SUPPORT_ABNORMALVEL) | defined(SUPPORT_CONVERSE)
	ipReleasePerimeterAnalysti( &pstBva->stModulePerimeter );
#endif

#if defined(SUPPORT_TRIPWIRE)
	ipReleaseTripwireAnalysti( &pstBva->stModuleTripwire );
#endif

#if defined(SUPPORT_MTRIPWIRE)
	ipReleaseMTripwireAnalysti( &pstBva->stModuleMTripwire );
#endif

	if (pstResult->s32ModuleSwitch & 2)
	{
		ipReleaseOscAnalysti( &pstBva->stModuleOSC );
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstBva->pstAnalystOSC );
	}

	// TARDAT
	{
		IMP_MMFree( pstMemMgr, IMP_MEMBLK_TYPE_SLOW, pstBva->pstTargetData );
	}

	memset( pstBva, 0, sizeof(BEHAVIOR_ANALYSIS_S) );

}


IMP_VOID ipConfigAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva, IpAnalystPara *pstPara )
{
	IMP_S32 s32FrmDura = pstPara->s32FrmDura;
	PEA_RESULT_S *pstResult = pstBva->pstResult;
	IMP_S32 s32ZoneIndex;
	RULE_ZONE_S *pstZone;
	RULE_PARA_S	*pstRulePara;
	USR_ADV_PARA_S *pstUsrAdvPara = (USR_ADV_PARA_S*)pstPara->pstRule->u8AdvParaData;
   	memcpy( &pstBva->stPara, pstPara, sizeof(IpAnalystPara) );
#if PRINT_DBG || 0
	printf("ipConfigAnalystInternal pstBva->stPara.s32InitEventLife:%d pstPara->s32FrmDura:%d\n", pstBva->stPara.s32InitEventLife, pstPara->s32FrmDura);
#endif
	{
		pstBva->stPara.s32InitEventLife /= s32FrmDura;
	}
{
pstZone = &pstPara->pstRule->stZones.astZone[0];
pstRulePara = &pstZone->stPara;

}
	for( s32ZoneIndex=0; s32ZoneIndex<IMP_MAX_NUM_RULE_ZONE; s32ZoneIndex++ )
	{
		pstZone = &pstPara->pstRule->stZones.astZone[s32ZoneIndex];
		if( !pstZone->u32Valid ) continue;
		pstRulePara = &pstZone->stPara;
	
		if (pstResult->s32ModuleSwitch & 2)
		{
			pstRulePara->stOsc.stOscPara.s32TimeMin /= s32FrmDura;
		}

		pstRulePara->stObjStolen.s32TimeMin /= s32FrmDura;
		pstRulePara->stAbandum.s32TimeMin /= s32FrmDura;
		pstRulePara->stNoParking.s32TimeMin /= s32FrmDura;
		pstRulePara->stPerimeter.stPara.s32TimeMin /= s32FrmDura;
		pstRulePara->stTripwire.stLmtPara.s32TimeMin /= s32FrmDura;
		pstRulePara->stMTripwire.stLmtPara.s32TimeMin /= s32FrmDura;
		pstRulePara->stLoiter.s32TimeMin /= s32FrmDura;
	}

	ipConfigTargetEvtMgr( &pstBva->stTargetEvtMgr, pstBva->stPara.s32OutputEvents, pstBva->stPara.pstRule );


#if defined(SUPPORT_PERIMETER) | defined(SUPPORT_ABNORMALVEL) | defined(SUPPORT_CONVERSE)
	pstBva->stPara.stPerimeterPara.pstRule = pstBva->stPara.pstRule;
	pstBva->stPara.stPerimeterPara.s32ObjectAreaRatio = pstBva->stPara.s32ObjectAreaRatio;
	ipConfigPerimeterAnalysti( &pstBva->stModulePerimeter, &pstBva->stPara.stPerimeterPara );
#endif

#if defined(SUPPORT_TRIPWIRE)
	pstBva->stPara.stTripwirePara.pstRule = pstBva->stPara.pstRule;
	pstBva->stPara.stTripwirePara.s32ObjectAreaRatio = pstBva->stPara.s32ObjectAreaRatio;
	ipConfigTripwireAnalysti( &pstBva->stModuleTripwire, &pstBva->stPara.stTripwirePara );
#endif

#if defined(SUPPORT_MTRIPWIRE)
	pstBva->stPara.stMTripwirePara.pstRule = pstBva->stPara.pstRule;
	pstBva->stPara.stMTripwirePara.astSTripwire[0].s32ObjectAreaRatio=pstBva->stPara.s32ObjectAreaRatio;
	pstBva->stPara.stMTripwirePara.astSTripwire[1].s32ObjectAreaRatio=pstBva->stPara.s32ObjectAreaRatio;
	ipConfigMTripwireAnalysti( &pstBva->stModuleMTripwire, &pstBva->stPara.stMTripwirePara );
#endif

	if (pstResult->s32ModuleSwitch & 2)
	{
		pstBva->stPara.stOscPara.pstRule = pstBva->stPara.pstRule;
		ipConfigOscAnalysti( &pstBva->stModuleOSC, &pstBva->stPara.stOscPara );
	}

}


IMP_VOID ipStartAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva )
{
	ipStartTargetEvtMgr( &pstBva->stTargetEvtMgr );
}


IMP_VOID ipStopAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva )
{
	ipStopTargetEvtMgr( &pstBva->stTargetEvtMgr );
}

static IMP_VOID ipProcessBehaviorAanalystInternalStart( BEHAVIOR_ANALYSIS_S *pstBva );
static IMP_VOID ipProcessBehaviorAanalystInternalEnd( BEHAVIOR_ANALYSIS_S *pstBva );
IMP_S32 IMP_PEA_BVA_ProcessBvaInternal( BEHAVIOR_ANALYSIS_S *pstBva )
{
	IpAnalystPara *pstAnalystPara = &pstBva->stPara;
	PEA_RESULT_S *pstResult = pstBva->pstResult;
	IMP_U32 u32FuncCode = pstAnalystPara->u32FuncCode;
	PEA_STATUS_S *pstStatus = &pstBva->pstResult->stSysStatus;
	pstStatus->u32AnalystStatus = IMP_MOUDLE_STATUS_SUCCESS;
#define DBG_PRBVA (1&PRINT_DBG)

	ipProcessBehaviorAanalystInternalStart( pstBva );


#if defined(SUPPORT_PERIMETER)

	if( u32FuncCode & IMP_FUNC_PERIMETER)
	{

		ipProcessPerimeterAnalystInternal( &pstBva->stModulePerimeter );
	}
#endif

#if defined(SUPPORT_TRIPWIRE)
	if( u32FuncCode & (IMP_FUNC_TRIPWIRE) )
	{
		ipProcessTripwireAnalystInternal( &pstBva->stModuleTripwire );
	}
#endif

#if defined(SUPPORT_MTRIPWIRE)
	if( u32FuncCode & (IMP_FUNC_MTRIPWIRE) )
	{
	//	printf("if( u32FuncCode & (IMP_FUNC_MTRIPWIRE) )\n");
		ipProcessMTripwireAnalystInternal( &pstBva->stModuleMTripwire );
	}
#endif

	if (pstResult->s32ModuleSwitch & 2)
	{
		if( u32FuncCode & (IMP_FUNC_OSC) )
		{
			ipProcessOscAnalysti( &pstBva->stModuleOSC );
		}
	}	

	ipProcessBehaviorAanalystInternalEnd( pstBva );
	ipProcessTargetEvtMgr( &pstBva->stTargetEvtMgr );

	return 1;
}

IMP_S32 ipPostProcessAnalystInternal( BEHAVIOR_ANALYSIS_S *pstBva )
{
	IpAnalystPara *pstAnalystPara = &pstBva->stPara;
	IMP_U32 u32FuncCode = pstAnalystPara->u32FuncCode;


#if defined(SUPPORT_PERIMETER)
	if( u32FuncCode & IMP_FUNC_PERIMETER )
		ipPostProcessPerimeterAnalysti( &pstBva->stModulePerimeter );
#endif

#if defined(SUPPORT_TRIPWIRE)
	if( u32FuncCode & (IMP_FUNC_TRIPWIRE) )
		ipPostProcessTripwireAnalysti( &pstBva->stModuleTripwire );
#endif
#if defined(SUPPORT_MTRIPWIRE)
	if( u32FuncCode & (IMP_FUNC_MTRIPWIRE) )
		ipPostProcessMTripwireAnalysti( &pstBva->stModuleMTripwire );
#endif

	return 1;
}

static IMP_VOID process_behavior_analyst_i_start_helper(
				BEHAVIOR_ANALYSIS_S *pstBva, IpTrackedTargetSet *pstTTgtSet )
{
	IMP_S32 i;
	IpTrackedTarget *pstTarget;

   	for( i=0; i<IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTTgtSet->astTargets[i];
		if( !ipTrackedTargetIsValid( pstTarget ) )
			continue;

		if( ipTrackedTargetIsMeasured( pstTarget ) )
		{
			pstTarget->stTargetInfo.u32TgtEvent = IMP_TGT_EVENT_UNKNOWN;
			memset( pstTarget->stTargetInfo.au32Events, 0, sizeof(pstTarget->stTargetInfo.au32Events) );
		}
	}
}

static IMP_VOID ipProcessBehaviorAanalystInternalStart( BEHAVIOR_ANALYSIS_S *pstBva )
{
	PEA_STATUS_S *pstStatus = &pstBva->pstResult->stSysStatus;
	PEA_RESULT_S *pstResult = pstBva->pstResult;

	process_behavior_analyst_i_start_helper( pstBva, &pstBva->pstResult->stTrackedTargetSet );

	if (pstResult->s32ModuleSwitch & 2)
	{
		process_behavior_analyst_i_start_helper( pstBva, &pstBva->pstResult->stTrackedTargetSetOsc );
	}
}

static const IMP_U32 au32EvtCodes[] =
{
	IMP_TGT_EVENT_PERIMETER,
	IMP_TGT_EVENT_TRIPWIRE,
	IMP_TGT_EVENT_MTRIPWIRE,
	IMP_TGT_EVENT_LOITER,
	IMP_TGT_EVENT_OSC,
};

extern PEA_BVA_TGT_EVT_DATA_S* ipGetEvtTargetData( IMP_VOID *pData );
static IMP_VOID ipProcessBehaviorAnalystInternalEndHelper(
				BEHAVIOR_ANALYSIS_S *pstBva, IpTrackedTargetSet *pstTTgtSet )
{
	IMP_S32 i, j, k, s32EvtCnt;
	IpTrackedTarget *pstTarget;
	IpTargetPosition *pstPos;
	IMP_U32 u32TgtEvent, u32Type, u32TEvtZ;
	IMP_S32 s32InitLife, s32TimeDelta;
	PEA_BVA_TGT_EVT_DATA_S *pstEventData;

	s32EvtCnt = sizeof(au32EvtCodes) / sizeof(IMP_U32);
	s32TimeDelta = pstBva->pstResult->u32FrmTimeCur - pstBva->pstResult->u32FrmTimePre;
	s32InitLife = pstBva->stPara.s32InitEventLife;

   	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget = &pstTTgtSet->astTargets[i];
		if (pstTarget->u32TargetId == 2684356900 && 1741 == pstBva->pstResult->u32FrmTimeCur)
		{
			IMP_S32 m = 0;
		}
		if( !ipTrackedTargetIsValid( pstTarget ) )
			continue;

		if( s32InitLife )
		{
			pstEventData = ipGetEvtTargetData( pstTarget->stPrivateData.pDataAnalyst );

			u32TgtEvent = 0;
			for( k = 0; k < IMP_MAX_NUM_RULE_ZONE; k++ )
			{
				u32TEvtZ = pstTarget->stTargetInfo.au32Events[k];
				for( j = 0; j < s32EvtCnt; j++ )
				{
					if( u32TEvtZ & au32EvtCodes[j] )
					{
						pstEventData->aau8EvtLife[k][j] = s32InitLife;
					}
					else
					{
						if( pstEventData->aau8EvtLife[k][j] > s32TimeDelta )
						{
							pstEventData->aau8EvtLife[k][j] -= s32TimeDelta;
							u32TEvtZ |= au32EvtCodes[j];
						}
						else
						{
							pstEventData->aau8EvtLife[k][j] = 0;
						}
					}
				}
//				if (u32TEvtZ!=0 && k>0 )
//				{
//					u32TEvtZ=0;
//				}
				pstTarget->stTargetInfo.au32Events[k] = u32TEvtZ;
				u32TgtEvent |= u32TEvtZ;
			}

			u32Type = pstTarget->stTargetInfo.u32Type;
			if( u32Type != IMP_TGT_TYPE_UNKNOWN )
				pstEventData->u32TgtTypePre = u32Type;
			if( u32TgtEvent )
			{
				if( u32Type == IMP_TGT_TYPE_UNKNOWN )
					u32Type = pstEventData->u32TgtTypePre;
				pstTarget->stTargetInfo.u32Type = u32Type;
			}
		}

		u32TgtEvent = 0;
		for( k = 0; k < IMP_MAX_NUM_RULE_ZONE; k++ )
		{
			u32TEvtZ = pstTarget->stTargetInfo.au32Events[k];
			u32TgtEvent |= u32TEvtZ;
		}
		pstTarget->stTargetInfo.u32TgtEvent = u32TgtEvent;
		pstPos = ipTargetTrajectoryGetPosition( &pstTarget->stTrajectory, 0 );
        pstPos->u32TgtEvent = pstTarget->stTargetInfo.u32TgtEvent;
	}
}

static IMP_VOID ipProcessBehaviorAanalystInternalEnd( BEHAVIOR_ANALYSIS_S *pstBva )
{
	PEA_STATUS_S *pstStatus = &pstBva->pstResult->stSysStatus;
	PEA_RESULT_S *pstResult = pstBva->pstResult;

	ipProcessBehaviorAnalystInternalEndHelper( pstBva, &pstBva->pstResult->stTrackedTargetSet );

	if (pstResult->s32ModuleSwitch & 2)
	{
		ipProcessBehaviorAnalystInternalEndHelper( pstBva, &pstBva->pstResult->stTrackedTargetSetOsc );
	}
}


PEA_BVA_TGT_EVT_DATA_S* ipGetEvtTargetData( IMP_VOID *pData )
{
	TARDAT_ANALYST_S *pstTarData = (TARDAT_ANALYST_S*)pData;
	return &pstTarData->stEventData;
}


#if defined(SUPPORT_PERIMETER) | defined(SUPPORT_ABNORMALVEL) | defined(SUPPORT_CONVERSE)
IpPerimeterTargetData* ipGetPerimeterTargetData( IMP_VOID *pData )
{
	TARDAT_ANALYST_S *pstTarData = (TARDAT_ANALYST_S*)pData;
	return &pstTarData->stDataPerimeter;
}
#endif

#if defined(SUPPORT_TRIPWIRE)
IpTripwireTargetData* ipGetTripwireTargetData( IMP_VOID *pData )
{
	TARDAT_ANALYST_S *pstTarData = (TARDAT_ANALYST_S*)pData;
	return &pstTarData->stDataTripwire;
}
#endif

#if defined(SUPPORT_MTRIPWIRE)
IpMTripwireTargetData* ipGetMTripwireTargetData( IMP_VOID *pData )
{
	TARDAT_ANALYST_S *pstTarData = (TARDAT_ANALYST_S*)pData;
	return &pstTarData->stDataMTripwire;
}
#endif


IpOscTargetData* ipGetOscTargetData( IMP_VOID *pData )
{
	TARGET_DATA_ANALYST_OSC_S *pstTarData = (TARGET_DATA_ANALYST_OSC_S*)pData;
	return &pstTarData->stDataOSC;
}

IMP_MODULE_HANDLE ipCreateAnalyst( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce )
{
	IMP_MODULE_HANDLE hModule=NULL;
	IpModuleAnalyst *pModule=NULL;
	BEHAVIOR_ANALYSIS_S *pAnalyst=NULL;

	pModule = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpModuleAnalyst) );
	pModule->pstAnalyst = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(BEHAVIOR_ANALYSIS_S) );
	pAnalyst = pModule->pstAnalyst;
	ipCreateAnalystInternal( pAnalyst, pResult, pHwResouce );
#ifdef LOG_DATA_ANALYST
 	ipOpenAnalystLogFile();
#endif

	hModule = (IMP_MODULE_HANDLE)pModule;
	return hModule;
}

IMP_VOID ipReleaseAnalyst( IMP_MODULE_HANDLE hModule )
{
	IpModuleAnalyst *pModule = (IpModuleAnalyst*)hModule;
	BEHAVIOR_ANALYSIS_S *pAnalyst = pModule->pstAnalyst;
	GA_HARDWARE_RS_S *pHwResouce = pAnalyst->pHwResouce;

#ifdef LOG_DATA_ANALYST
 	ipCloseAnalystLogFile();
#endif
	ipReleaseAnalystInternal( pAnalyst );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pAnalyst );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule );
}

static IMP_VOID ipForceInnerPara2AnaylstPara( INNER_PARA_S *pstInnerPara, IpAnalystPara *pstAnalystPara );
IMP_VOID ipConfigAnalyst( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstInnerPara )
{
	IpModuleAnalyst *pModule = (IpModuleAnalyst*)hModule;
	BEHAVIOR_ANALYSIS_S *pAnalyst = pModule->pstAnalyst;
	DATA_BUF_S *pDatBuf;
	IpAnalystPara stAnalystPara;
{
RULE_S* pRule=(RULE_S*)pstInnerPara->pRule;

}
	pDatBuf = &pstInnerPara->stAdvance.astAdvDats[IMP_VPARAADVBUFNUM_ANALYST];
	ipParseAnaystParaData(&stAnalystPara );
	ipForceInnerPara2AnaylstPara( pstInnerPara, &stAnalystPara );
{
RULE_S* pRule=(RULE_S*)pstInnerPara->pRule;
}
	ipConfigAnalystInternal( pAnalyst, &stAnalystPara );
}

IMP_VOID ipStartAnalyst( IMP_MODULE_HANDLE hModule )
{
	IpModuleAnalyst *pModule = (IpModuleAnalyst*)hModule;
	BEHAVIOR_ANALYSIS_S *pAnalyst = pModule->pstAnalyst;

	ipStartAnalystInternal( pAnalyst );
}

IMP_VOID ipStopAnalyst( IMP_MODULE_HANDLE hModule )
{
	IpModuleAnalyst *pModule = (IpModuleAnalyst*)hModule;
	BEHAVIOR_ANALYSIS_S *pAnalyst = pModule->pstAnalyst;

	ipStopAnalystInternal( pAnalyst );
}

IMP_S32 ipProcessAnalyst( IMP_MODULE_HANDLE hModule )
{
	IpModuleAnalyst *pModule = (IpModuleAnalyst*)hModule;
	BEHAVIOR_ANALYSIS_S *pAnalyst = pModule->pstAnalyst;
	IMP_S32 s32Ret = 0;

	s32Ret = IMP_PEA_BVA_ProcessBvaInternal( pAnalyst );
//printf("ipProcessAnalyst:0\n");
#ifdef LOG_DATA_ANALYST
//printf("ipProcessAnalyst:1\n");
	ipOutputAnalystDataFrame( pAnalyst );
//printf("ipProcessAnalyst:2\n");
#endif
//printf("ipProcessAnalyst:3\n");
	return s32Ret;
}

IMP_S32 ipPostProcessAnalyst( IMP_MODULE_HANDLE hModule )
{
	IpModuleAnalyst *pModule = (IpModuleAnalyst*)hModule;
	BEHAVIOR_ANALYSIS_S *pAnalyst = pModule->pstAnalyst;

	return ipPostProcessAnalystInternal( pAnalyst );
}

static IMP_VOID ipForceInnerPara2AnaylstPara( INNER_PARA_S *pstInnerPara, IpAnalystPara *pstAnalystPara )
{
	GLOBAL_PARA_S *pstGlPara = &pstInnerPara->stGlobal;
	RULE_S *pRule;
	USR_ADV_PARA_S *pstUsrAdvPara;
	//IpUsrAdvParaAnalyst *usradv_analyst;
	
	// rule
	pstAnalystPara->pstRule = pstInnerPara->pRule;
	pRule = pstAnalystPara->pstRule;
	pstUsrAdvPara = (USR_ADV_PARA_S*)pRule->u8AdvParaData;

	// s32FrmDura
	pstAnalystPara->s32FrmDura = pstGlPara->s32TimeUnit;

//	if (pstResult->s32ModuleSwitch & 2)
	{
		pstAnalystPara->stOscPara.s32FrmDura = pstAnalystPara->s32FrmDura;
	}

	// u32FuncCode
	pstAnalystPara->u32FuncCode = pstGlPara->s32FuncCode;

}

IMP_MODULE_HANDLE IMP_PEA_BVA_CreateBehaviorAnalysis( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce )
{
	IMP_MODULE_HANDLE hModule=NULL;
	IpModuleBehaviorAnalysis *pModule=NULL;

	pModule = IMP_MMAlloc( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, sizeof(IpModuleBehaviorAnalysis) );
	pModule->hAnalyst = ipCreateAnalyst( pResult, pHwResouce );
	pModule->pstResult = pResult;
	pModule->pstHwResouce = pHwResouce;

	hModule = (IMP_MODULE_HANDLE)pModule;
	return hModule;
}

IMP_VOID IMP_PEA_BVA_ReleaseBehaviorAnalysis( IMP_MODULE_HANDLE hModule )
{
	IpModuleBehaviorAnalysis *pModule = (IpModuleBehaviorAnalysis*)hModule;
	GA_HARDWARE_RS_S *pHwResouce = pModule->pstHwResouce;

	ipReleaseAnalyst( pModule->hAnalyst );
	IMP_MMFree( &pHwResouce->stMemMgr, IMP_MEMBLK_TYPE_SLOW, pModule );
}

IMP_VOID IMP_PEA_BVA_ConfigBehaviorAnalysis( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstInnerPara )
{
	IpModuleBehaviorAnalysis *pModule = (IpModuleBehaviorAnalysis*)hModule;

	ipConfigAnalyst( pModule->hAnalyst, pstInnerPara );
}

IMP_VOID IMP_PEA_BVA_StartBehaviorAnalysis( IMP_MODULE_HANDLE hModule )
{
	IpModuleBehaviorAnalysis *pModule = (IpModuleBehaviorAnalysis*)hModule;

	ipStartAnalyst( pModule->hAnalyst );
}

IMP_VOID IMP_PEA_BVA_StopBehaviorAnalysis( IMP_MODULE_HANDLE hModule )
{
	IpModuleBehaviorAnalysis *pModule = (IpModuleBehaviorAnalysis*)hModule;

	ipStopAnalyst( pModule->hAnalyst );
}

IMP_S32 IMP_PEA_BVA_ProcessBehaviorAnalysis( IMP_MODULE_HANDLE hModule )
{
	IpModuleBehaviorAnalysis *pModule = (IpModuleBehaviorAnalysis*)hModule;

	return ipProcessAnalyst( pModule->hAnalyst );
}

IMP_S32 IMP_PEA_BVA_PostProcessBehaviorAnalysis( IMP_MODULE_HANDLE hModule )
{
	IpModuleBehaviorAnalysis *pModule = (IpModuleBehaviorAnalysis*)hModule;

	return ipPostProcessAnalyst( pModule->hAnalyst );
}
