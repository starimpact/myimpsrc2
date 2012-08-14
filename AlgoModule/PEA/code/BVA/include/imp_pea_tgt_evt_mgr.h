#ifndef _IMP_PEA_TGT_EVT_MGR_H_
#define _IMP_PEA_TGT_EVT_MGR_H_

#include "imp_pea_common.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define IMP_EVENT_CODE_MIN	IMP_EVT_TYPE_AlarmAbabdum
#define IMP_EVENT_CODE_NUM	(IMP_EVT_TYPE_AlarmOsc_R - IMP_EVT_TYPE_AlarmAbabdum + 1)
#define IMP_MAX_CHILD_RULE_NUM 8
#define IMP_MAX_START_BUFFER_LEN 1024


typedef struct impTGTEVT_START_BUFFER_ELEM_S
{
	IMP_S32 s32ZoneIndex;
	IMP_S32 s32LineIndex;
	IMP_S32 s32Id;
} TGTEVT_START_BUFFER_ELEM_S;


typedef struct impTGTEVT_START_BUFFER_S
{
	TGTEVT_START_BUFFER_ELEM_S astEvtStartBufElem[IMP_MAX_START_BUFFER_LEN];
	IMP_U32 u32Length;
} TGTEVT_START_BUFFER_S;


typedef struct
{
	IMP_U32 u32TargetId;
	IMP_U32 au32EvtId[IMP_MAX_NUM_RULE_ZONE][IMP_EVENT_CODE_NUM];
} IpTargetEvtRecord;

typedef struct
{
	PEA_RESULT_S *pstResult;                              /**< 处理结果 */
	IMP_S32 s32Flag;
	RULE_S *pstRule;                                      /**< 规则 */
	TGTEVT_START_BUFFER_S stEvtStartBuf;
	IpTargetEvtRecord astRecords[IMP_MAX_TGT_CNT];

#ifdef SUPPORT_CHANGE
	IpTargetEvtRecord astRecordsChg[IMP_MAX_TGT_CNT];
#endif
	IpTargetEvtRecord astRecordsOsc[IMP_MAX_TGT_CNT]; //osc event record for target

	IMP_U32 au32TgtEvtIds[IMP_EVENT_CODE_NUM];
} IpTargetEvtMgr;

IMP_S32 IMP_GetMemSizeTargetEvtMgr(IMP_S32 s32Width, IMP_S32 s32Height);
IMP_VOID ipCreateTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr, PEA_RESULT_S *pstResult );
IMP_VOID ipReleaseTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr );
IMP_VOID ipConfigTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr, IMP_S32 s32Flag, RULE_S *pstRule );
IMP_VOID ipStartTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr );
IMP_VOID ipStopTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr );
IMP_VOID ipProcessTargetEvtMgr( IpTargetEvtMgr *pstEvtMgr );


#ifdef __cplusplus
}
#endif

#endif
