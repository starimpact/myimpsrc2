#include "imp_ga_evt_manager.h"


IMP_VOID IMP_EVTMGR_Initialize( EVT_MGR_S *pstMgr, IMP_U32 u32EvtIdMin, IMP_U32 u32EvtIdMax )
{
	memset( pstMgr, 0, sizeof(EVT_MGR_S) );
	pstMgr->u32EvtIdMin = u32EvtIdMin;
	pstMgr->u32EvtIdMax = u32EvtIdMax;
	pstMgr->u32EvtIdCount = u32EvtIdMin;
}

IMP_VOID IMP_EVTMGR_Terminate( EVT_MGR_S *pstMgr )
{
	memset( pstMgr, 0, sizeof(EVT_MGR_S) );
}

IMP_U32 IMP_EVTMGR_AllocID( EVT_MGR_S *pstMgr )
{
	IMP_U32 u32EvtIdCount = pstMgr->u32EvtIdCount;
	pstMgr->u32EvtIdCount++;
	if( pstMgr->u32EvtIdCount < pstMgr->u32EvtIdMin || pstMgr->u32EvtIdCount > pstMgr->u32EvtIdMax )
		pstMgr->u32EvtIdCount = pstMgr->u32EvtIdMin;
	return pstMgr->u32EvtIdCount;
}


IMP_VOID IMP_GA_ResultClear( RESULT_S *pstResult )
{
	pstResult->stTargetSet.s32TargetNum = 0;
	pstResult->stEventSet.s32EventNum = 0;
}

CMD_ITEM_S* IMP_CMD_SET_AddCommand( CMD_SET_S *pstCmds, IMP_U32 u32Type )
{
	CMD_ITEM_S *pstCmd;

	if( pstCmds->s32CmdNum >= IMP_MAX_EVT_CNT )
		return NULL;
	pstCmd = &pstCmds->astCmds[pstCmds->s32CmdNum++];
	pstCmd->u32Type = u32Type;

	return pstCmd;
}

TGT_ITEM_S* IMP_TGT_SET_AddTarget( TGT_SET_S *pstTgts,
								  IMP_U32 u32Id, IMP_U32 u32Status, IMP_U32 u32Type, IMP_U32 u32Event, IMP_POINT_S stPoint, IMP_RECT_S stRect )

{
	TGT_ITEM_S *pstTarget;

	if( pstTgts->s32TargetNum >= IMP_MAX_TGT_CNT )
		return NULL;
	pstTarget = &pstTgts->astTargets[pstTgts->s32TargetNum++];
	pstTarget->u32Id = u32Id;
	pstTarget->u32Status = u32Status;
	pstTarget->u32Type = u32Type;
	pstTarget->u32Event = u32Event;
	pstTarget->stPoint = stPoint;
	pstTarget->stRect = stRect;

	return pstTarget;
}

EVT_ITEM_S* IMP_EVT_SET_AddEvent( EVT_SET_S *pstEvts,
								 IMP_U32 u32Type, IMP_U32 u32Id, IMP_U32 u32Level, IMP_U32 u32Status, IMP_U32 u32Zone )
{
	EVT_ITEM_S *pstEvt;

	if( pstEvts->s32EventNum >= IMP_MAX_EVT_CNT )
		return NULL;
	pstEvt = &pstEvts->astEvents[pstEvts->s32EventNum++];
	pstEvt->u32Type = u32Type;
	pstEvt->u32Id = u32Id;
	pstEvt->u32Level = u32Level;
	pstEvt->u32Status = u32Status;
	pstEvt->u32Zone = u32Zone;

	return pstEvt;
}

EVT_ITEM_S* IMP_EVT_SET_AddEventNoID( EVT_MGR_S *pstMgr, EVT_SET_S *pstEvts,
									 IMP_U32 u32Type, IMP_U32 *pu32Id, IMP_U32 u32Level, IMP_U32 u32Status, IMP_U32 u32Zone )
{
	IMP_U32 u32Id = IMP_EVTMGR_AllocID( pstMgr );
	if( pu32Id ) *pu32Id = u32Id;
	return IMP_EVT_SET_AddEvent( pstEvts, u32Type, u32Id, u32Level, u32Status, u32Zone );
}

