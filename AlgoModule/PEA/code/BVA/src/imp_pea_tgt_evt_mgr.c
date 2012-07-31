#include "imp_pea_tgt_evt_mgr.h"
#include "imp_pea_tripwire.h"


static IMP_VOID ipClearTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr );
static IMP_VOID ipCollectTargetEvent( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts, PEA_RESULT_S *pstResult, IMP_S32 s32Flag );
static IMP_VOID ipClearTargetEvent( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts, PEA_RESULT_S *pstResult, IMP_S32 s32Flag );
extern IpTripwireTargetData* ipGetTripwireTargetData( IMP_VOID *pData );
IMP_VOID ipCreateTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr, PEA_RESULT_S *pstResult )
{
	memset( pstEvtMgr, 0, sizeof(IpTargetEvtMgr) );
	pstEvtMgr->pstResult = pstResult;
}

IMP_VOID ipReleaseTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr )
{
	memset( pstEvtMgr, 0, sizeof(IpTargetEvtMgr) );
}

IMP_VOID ipConfigTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr, IMP_S32 s32Flag, RULE_S *pstRule )
{
	pstEvtMgr->s32Flag = s32Flag;
	pstEvtMgr->pstRule = pstRule;
}

IMP_VOID ipStartTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr )
{
	ipClearTargetEvtMgr( pstEvtMgr );
}

IMP_VOID ipStopTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr )
{
	ipClearTargetEvent( pstEvtMgr, &pstEvtMgr->pstResult->stOutput.stEventSet, pstEvtMgr->pstResult, pstEvtMgr->s32Flag );
	ipClearTargetEvtMgr( pstEvtMgr );
}

IMP_VOID ipProcessTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr )
{
	ipCollectTargetEvent( pstEvtMgr, &pstEvtMgr->pstResult->stOutput.stEventSet, pstEvtMgr->pstResult, pstEvtMgr->s32Flag );
}

static IMP_VOID ipClearTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr )
{
	memset( pstEvtMgr->astRecords, 0, sizeof(pstEvtMgr->astRecords) );

	memset( pstEvtMgr->au32TgtEvtIds, 0, sizeof(pstEvtMgr->au32TgtEvtIds) );
}

static IMP_U32 ipGetEventID( IpTargetEvtMgr *pstEvtMgr )
{
	return IMP_EVTMGR_AllocID( &pstEvtMgr->pstResult->stEvtMgr );
}

static IMP_VOID ipFillTargetEventData( EVT_ITEM_S *pstEvt, IpTrackedTarget *pstTarget, RULE_S *pstRule );
static EVT_ITEM_S* ipAddTargetEvent( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts,
					IMP_U32 u32Type, IMP_U32 u32EvtId, IMP_U32 u32Level, IMP_U32 u32Status, IMP_U32 u32Zone,
					IMP_U32 u32TargetId, IpTrackedTarget *pstTarget )
{
	EVT_ITEM_S *pstEvt;
	IMP_S32 i;
	EVT_DATA_TARGET_S *pstEvtData;
	IpTripwireTargetData *pstTargetData;
	TGTEVT_START_BUFFER_S *stEvtStartBuf = &pstEvtMgr->stEvtStartBuf;
    EVT_DATA_TRIPWIRE_S *pData;


	pstEvt = IMP_EVT_SET_AddEvent( pstEvts, u32Type, u32EvtId, u32Level, u32Status, u32Zone );
	if( !pstEvt ) return NULL;

	if(IMP_EVT_STATUS_START == u32Status && IMP_EVT_TYPE_AlarmTripwire == pstEvt->u32Type)
	{
		pstTargetData = ipGetTripwireTargetData( pstTarget->stPrivateData.pDataAnalyst );
		for( i = 0; i < IMP_MAX_START_BUFFER_LEN; i++)
		{
			if(stEvtStartBuf->astEvtStartBufElem[i].s32Id == 0)
			{
                stEvtStartBuf->astEvtStartBufElem[i].s32LineIndex = pstTargetData->as32LineIndex[u32Zone];
                stEvtStartBuf->astEvtStartBufElem[i].s32Id = u32EvtId;
                stEvtStartBuf->astEvtStartBufElem[i].s32ZoneIndex = u32Zone;
                stEvtStartBuf->u32Length++;
                break;
			}
		}
	}

	pstEvtData = (EVT_DATA_TARGET_S*)pstEvt->au8Data;
	pstEvtData->u32TId = u32TargetId;
//printf("ipAddTargetEvent u32TargetId:%d\n", u32TargetId);
	if(IMP_EVT_STATUS_END == u32Status  && IMP_EVT_TYPE_AlarmTripwire == pstEvt->u32Type)
    {
        for(i = 0; i < IMP_MAX_START_BUFFER_LEN ; i++)
        {
            if(stEvtStartBuf->astEvtStartBufElem[i].s32Id == u32EvtId)
            {
				break;
            }
        }
        pData = (EVT_DATA_TRIPWIRE_S *)pstEvt->au8Data;
        pData->u8LineIndex = stEvtStartBuf->astEvtStartBufElem[i].s32LineIndex;
        pData->stRule.stLine = pstEvtMgr->pstRule->stZones.astZone[u32Zone].stPara.stTripwire.astLines[pData->u8LineIndex].stLine;
        stEvtStartBuf->u32Length--;
        stEvtStartBuf->astEvtStartBufElem[i].s32LineIndex = -1;
        stEvtStartBuf->astEvtStartBufElem[i].s32Id = 0;
        stEvtStartBuf->astEvtStartBufElem[i].s32ZoneIndex = -1;

        return pstEvt;

    }
	if( !pstTarget /*|| IMP_EVT_STATUS_END == u32Status*/)
	{
        return pstEvt;
	}

	ipFillTargetEventData( pstEvt, pstTarget, pstEvtMgr->pstRule );

	return pstEvt;
}

typedef struct
{
	IMP_U32 u32TgtEvent, u32VEvent;
} MAP4TVEVT;

static MAP4TVEVT map4tvevts[] =
{
	{ IMP_TGT_EVENT_ABANDUM	 	,		IMP_EVT_TYPE_AlarmAbabdum		},
	{ IMP_TGT_EVENT_OBJSTOLEN	,		IMP_EVT_TYPE_AlarmObjStolen		},
	{ IMP_TGT_EVENT_NOPARKING	,		IMP_EVT_TYPE_AlarmNoParking		},
	{ IMP_TGT_EVENT_PERIMETER	,		IMP_EVT_TYPE_AlarmPerimeter		},
	{ IMP_TGT_EVENT_TRIPWIRE	,		IMP_EVT_TYPE_AlarmTripwire		},
	{ IMP_TGT_EVENT_MTRIPWIRE	,		IMP_EVT_TYPE_AlarmMTripwire		},
	{ IMP_TGT_EVENT_ABNORMALVEL ,		IMP_EVT_TYPE_AlarmAbnmlvel		},
	{ IMP_TGT_EVENT_LOITER		,		IMP_EVT_TYPE_AlarmLoiter		},
	{ IMP_TGT_EVENT_OSC	 		,		IMP_EVT_TYPE_AlarmOsc			},
	{ IMP_TGT_EVENT_OSC_R 		,		IMP_EVT_TYPE_AlarmOsc_R			}
};


static IMP_VOID ipCollectHelper11( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts,
									IpTargetEvtRecord *pstRecord, IpTrackedTarget *pstTarget )
{
	IMP_S32 i, j;
	IMP_U32 u32TgtEvent, u32VEvent;
	IMP_U32 u32EvtId, u32TargetId, u32Level;
	RULE_ZONE_S *u32Zone;

	u32TargetId = pstTarget->u32TargetId;
	u32TgtEvent = pstTarget->stTargetInfo.u32TgtEvent;

	for( i=0; i<IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		u32Zone = &pstEvtMgr->pstRule->stZones.astZone[i];
		u32Level = u32Zone->u32Level;
		u32TgtEvent = pstTarget->stTargetInfo.au32Events[i];
		if( !u32Zone->u32Valid || !u32Zone->u32Enable )
			continue;

		for( j=0; j<IMP_EVENT_CODE_NUM; j++ )
		{
			u32VEvent = map4tvevts[j].u32VEvent;
			u32EvtId = pstRecord->au32EvtId[i][j];
			if( u32TgtEvent & map4tvevts[j].u32TgtEvent )
			{
				//			if( u32EvtId &&
				//				( ( (u32VEvent==IMP_EVT_TYPE_AlarmTripwire) && target_new_tripwire(pstTarget,i) )
				//				|| ( (u32VEvent==IMP_EVT_TYPE_AlarmMTripwire) && target_new_mtripwire(pstTarget,i) ) )
				//				)
				//			{
				//				ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, u32Level, IMP_EVT_STATUS_END, i, u32TargetId, pstTarget );
				//				pstRecord->au32EvtId[i][j] = 0;
				//				u32EvtId = 0;
				//			}

				if( !u32EvtId )
				{
					u32EvtId = ipGetEventID(pstEvtMgr);
					ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, u32Level, IMP_EVT_STATUS_START, i, u32TargetId, pstTarget );
					pstRecord->au32EvtId[i][j] = u32EvtId;
				}
				else
				{
					//u32EvtId = ipGetEventID(pstEvtMgr);
					ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, u32Level, IMP_EVT_STATUS_PROCEDURE, i, u32TargetId, pstTarget );
					//pstRecord->au32EvtId[i][j] = u32EvtId;
				}
			}
			else
			{
				if( u32EvtId )
				{
					ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, u32Level, IMP_EVT_STATUS_END, i, u32TargetId, pstTarget );
					pstRecord->au32EvtId[i][j] = 0;
				}
			}
		}
	}
}

static IMP_VOID ipCollectHelper10( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts,
									IpTargetEvtRecord *pstRecord, IpTrackedTarget *pstTarget )
{
	IMP_S32 i, j;
	IMP_U32 u32VEvent;
	IMP_U32 u32EvtId, u32Level;
	RULE_ZONE_S *u32Zone;

	for( i=0; i<IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		u32Zone = &pstEvtMgr->pstRule->stZones.astZone[i];
		u32Level = u32Zone->u32Level;
		if( !u32Zone->u32Valid || !u32Zone->u32Enable )
			continue;

		for( j=0; j<IMP_EVENT_CODE_NUM; j++ )
		{
			u32VEvent = map4tvevts[j].u32VEvent;
			u32EvtId = pstRecord->au32EvtId[i][j];
			if( u32EvtId )
			{
				ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, u32Level, IMP_EVT_STATUS_END, i, pstRecord->u32TargetId, pstTarget );
				pstRecord->au32EvtId[i][j] = 0;
			}
		}
	}
	pstRecord->u32TargetId = 0;
}

static IMP_VOID ipCollectHelper01( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts,
									  IpTargetEvtRecord *pstRecord, IpTrackedTarget *pstTarget )
{
	IMP_S32 i, j;
	IMP_U32 u32TgtEvent, u32VEvent;
	IMP_U32 u32EvtId, u32TargetId, u32Level;
	RULE_ZONE_S *u32Zone;

	pstRecord->u32TargetId = 0;
	u32TargetId = pstTarget->u32TargetId;
	u32TgtEvent = pstTarget->stTargetInfo.u32TgtEvent;
	if( u32TgtEvent==IMP_TGT_EVENT_UNKNOWN )
		return;

	for( i=0; i<IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		u32Zone = &pstEvtMgr->pstRule->stZones.astZone[i];
		u32Level = u32Zone->u32Level;
		u32TgtEvent = pstTarget->stTargetInfo.au32Events[i];
		if( !u32Zone->u32Valid || !u32Zone->u32Enable || u32TgtEvent==IMP_TGT_EVENT_UNKNOWN )
			continue;

		for( j=0; j<IMP_EVENT_CODE_NUM; j++ )
		{
			u32VEvent = map4tvevts[j].u32VEvent;
			if( u32TgtEvent & map4tvevts[j].u32TgtEvent )
			{
				u32EvtId = ipGetEventID(pstEvtMgr);
				ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, u32Level, IMP_EVT_STATUS_START, i, u32TargetId, pstTarget );
				pstRecord->au32EvtId[i][j] = u32EvtId;
				pstRecord->u32TargetId = u32TargetId;
			}
		}
	}
}

static IMP_VOID ipCollectTargetEventEvery( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts,
									 IpTrackedTargetSet *pstTargetSet, IpTargetEvtRecord *pstRecord )
{
	IMP_S32 i;
	IpTrackedTarget *pstTarget;

	pstTarget = pstTargetSet->astTargets;
	for( i=0; i<IMP_MAX_TGT_CNT; i++ )
	{
		if( pstRecord->u32TargetId && pstTarget->s32Used )
		{
			if( pstRecord->u32TargetId == pstTarget->u32TargetId )
			{
				ipCollectHelper11( pstEvtMgr, pstEvts, pstRecord, pstTarget );
			}
			else
			{
				ipCollectHelper10( pstEvtMgr, pstEvts, pstRecord, pstTarget );
			//	printf("pstTarget_[%d]_ID:%d;  ", i, pstTarget->u32TargetId);
				ipCollectHelper01( pstEvtMgr, pstEvts, pstRecord, pstTarget );
			}
		}
		else if( pstRecord->u32TargetId && !pstTarget->s32Used )
		{
			ipCollectHelper10( pstEvtMgr, pstEvts, pstRecord, NULL );
		}
		else if( !pstRecord->u32TargetId && pstTarget->s32Used )
		{
			ipCollectHelper01(pstEvtMgr, pstEvts, pstRecord, pstTarget);
		//	printf("pstTarget_[%d]_ID:%d;  ", i, pstTarget->u32TargetId);
		}



		pstRecord++;
		pstTarget++;
	}
}

static IMP_VOID ipClearAllTargetEventEvery( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts, IpTargetEvtRecord *pstRecord )
{
	IMP_S32 i;

	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		if( pstRecord->u32TargetId )
		{
			ipCollectHelper10( pstEvtMgr, pstEvts, pstRecord, NULL );
		}
		pstRecord++;
	}
}


static IMP_VOID ipCollectTargetEventWhole( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts, IpTrackedTargetSet *pstTargetSet )
{
	IMP_S32 i, j;
	IMP_U32 u32EvtId, u32TgtEvent, u32VEvent;
	IpTrackedTarget *pstTarget;
	IMP_S32 as32EvtFlags[IMP_EVENT_CODE_NUM];

	memset( as32EvtFlags, 0, sizeof(as32EvtFlags) );

	pstTarget = pstTargetSet->astTargets - 1;
	for( i = 0; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget++;
		u32TgtEvent = pstTarget->stTargetInfo.u32TgtEvent;
		if( !pstTarget->s32Used || u32TgtEvent==IMP_TGT_EVENT_UNKNOWN )
			continue;

		for( j = 0; j < IMP_EVENT_CODE_NUM; j++ )
		{
			if( u32TgtEvent & map4tvevts[j].u32TgtEvent )
				as32EvtFlags[j] = 1;
		}
	}

	for( i = 0; i < IMP_EVENT_CODE_NUM; i++ )
	{
		u32VEvent = map4tvevts[i].u32VEvent;
		u32EvtId = pstEvtMgr->au32TgtEvtIds[i];
		if( !u32EvtId && as32EvtFlags[i] )
		{
			u32EvtId = ipGetEventID(pstEvtMgr);
			ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, IMP_GA_ALARM_LEVEL_HIGH, IMP_EVT_STATUS_START, -1, 0, NULL );
			pstEvtMgr->au32TgtEvtIds[i] = u32EvtId;
		}
		else if( u32EvtId && !as32EvtFlags[i] )
		{
			ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, IMP_GA_ALARM_LEVEL_HIGH, IMP_EVT_STATUS_END, -1, 0, NULL );
			pstEvtMgr->au32TgtEvtIds[i] = 0;
		}
	}
}

static IMP_VOID ipClearAllTargetEventWhole( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts )
{
	IMP_S32 i;
	IMP_U32 u32EvtId, u32VEvent;

	for( i = 0; i < IMP_EVENT_CODE_NUM; i++ )
	{
		u32VEvent = map4tvevts[i].u32VEvent;
		u32EvtId = pstEvtMgr->au32TgtEvtIds[i];
		if( u32EvtId )
		{
			ipAddTargetEvent( pstEvtMgr, pstEvts, u32VEvent, u32EvtId, IMP_GA_ALARM_LEVEL_HIGH, IMP_EVT_STATUS_END, -1, 0, NULL );
		}
		pstEvtMgr->au32TgtEvtIds[i] = 0;
	}
}


static IMP_VOID ipCollectTargetEvent( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts, PEA_RESULT_S *pstResult, IMP_S32 s32Flag )
{
	PEA_STATUS_S *pstStatus = &pstResult->stSysStatus;

	if( s32Flag == 0 )
	{
		ipCollectTargetEventEvery( pstEvtMgr, pstEvts, &pstResult->stTrackedTargetSet, pstEvtMgr->astRecords );
//		ipCollectTargetEventEvery( pstEvtMgr, pstEvts, &pstResult->stTrackedTargetSetOsc, pstEvtMgr->astRecordsOsc ); //fill osc event 

	}
	else
	{
		ipCollectTargetEventWhole( pstEvtMgr, pstEvts, &pstResult->stTrackedTargetSet );
//		ipCollectTargetEventWhole( pstEvtMgr, pstEvts, &pstResult->stTrackedTargetSetOsc ); //fill osc event 
	}
}

static IMP_VOID ipClearTargetEvent( IpTargetEvtMgr *pstEvtMgr, EVT_SET_S *pstEvts, PEA_RESULT_S *pstResult, IMP_S32 s32Flag )
{
	PEA_STATUS_S *pstStatus = &pstResult->stSysStatus;

	if( s32Flag == 0 )
	{
		ipClearAllTargetEventEvery( pstEvtMgr, pstEvts, pstEvtMgr->astRecords );
		ipClearAllTargetEventEvery( pstEvtMgr, pstEvts, pstEvtMgr->astRecordsOsc ); //clear osc event
	}
	else
	{
		ipClearAllTargetEventWhole( pstEvtMgr, pstEvts );
	}
}

extern IMP_VOID ipFillTargetEventDataPerimeter( EVT_ITEM_S *pstEvt, IpTrackedTarget *pstTarget, RULE_S *pstRule );

extern IMP_VOID ipFillTargetEventDataTripwire( EVT_ITEM_S *pstEvt, IpTrackedTarget *pstTarget, RULE_S *pstRule );

extern IMP_VOID ipFillTargetEventDataMTripwire( EVT_ITEM_S *pstEvt, IpTrackedTarget *pstTarget, RULE_S *pstRule );

extern IMP_VOID ipFillTargetEventDataOSC( EVT_ITEM_S *pstEvent, IpTrackedTarget *pstTarget, RULE_S *pstRule );

static IMP_VOID ipFillTargetEventData( EVT_ITEM_S *pstEvt, IpTrackedTarget *pstTarget, RULE_S *pstRule )
{
	switch( pstEvt->u32Type )
	{
    case IMP_EVT_TYPE_AlarmOsc:
        ipFillTargetEventDataOSC( pstEvt, pstTarget, pstRule );
        break;
    case IMP_EVT_TYPE_AlarmOsc_R:
        ipFillTargetEventDataOSC( pstEvt, pstTarget, pstRule );
        break;
	case IMP_EVT_TYPE_AlarmPerimeter:
		ipFillTargetEventDataPerimeter( pstEvt, pstTarget, pstRule );
	    break;
	case IMP_EVT_TYPE_AlarmTripwire:
#ifdef SUPPORT_TRIPWIRE
		ipFillTargetEventDataTripwire( pstEvt, pstTarget, pstRule );
#endif
	    break;
	case IMP_EVT_TYPE_AlarmMTripwire:
#ifdef SUPPORT_MTRIPWIRE
		ipFillTargetEventDataMTripwire( pstEvt, pstTarget, pstRule );
#endif
	    break;
	default:
	    break;
	}
}


