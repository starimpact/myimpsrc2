
#ifndef _IMP_GA_EVT_MANAGER_H_
#define _IMP_GA_EVT_MANAGER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "imp_algo_type.h"
#include "imp_algo_command.h"
#include "imp_ga_common.h"
#include "imp_algo_urp_param.h"


#define IMP_EVT_ID_MIN		0x00000000


#define IMP_EVT_ID_MAX		0x1FFFFFFF


typedef struct impEVT_MGR_S
{
	IMP_U32 u32EvtIdCount;
	IMP_U32 u32EvtIdMin;
	IMP_U32 u32EvtIdMax;
} EVT_MGR_S;



IMP_EXPORTS IMP_VOID IMP_EVTMGR_Initialize( EVT_MGR_S *pstMgr, IMP_U32 u32EvtIdMin, IMP_U32 u32EvtIdMax );

IMP_EXPORTS IMP_VOID IMP_EVTMGR_Terminate( EVT_MGR_S *pstMgr );

IMP_EXPORTS IMP_U32 IMP_EVTMGR_AllocID( EVT_MGR_S *pstMgr );
IMP_EXPORTS IMP_VOID IMP_GA_ResultClear( RESULT_S *pstResult );
IMP_EXPORTS CMD_ITEM_S* IMP_CMD_SET_AddCommand( CMD_SET_S *pstCmds, IMP_U32 u32Type );
IMP_EXPORTS TGT_ITEM_S* IMP_TGT_SET_AddTarget( TGT_SET_S *pstTgts,
	IMP_U32 u32Id, IMP_U32 u32Status, IMP_U32 u32Type, IMP_U32 u32Event, IMP_POINT_S stPoint, IMP_RECT_S stRect );
IMP_EXPORTS EVT_ITEM_S* IMP_EVT_SET_AddEvent( EVT_SET_S *pstEvts,
	IMP_U32 u32Type, IMP_U32 u32Id, IMP_U32 u32Level, IMP_U32 u32Status, IMP_U32 u32Zone );
IMP_EXPORTS EVT_ITEM_S* IMP_EVT_SET_AddEventNoID( EVT_MGR_S *pstMgr, EVT_SET_S *pstEvts,
	IMP_U32 u32Type, IMP_U32 *pu32Id, IMP_U32 u32Level, IMP_U32 u32Status, IMP_U32 u32Zone );

#ifdef __cplusplus
}
#endif

#endif

