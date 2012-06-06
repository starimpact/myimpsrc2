

#ifndef _IMP_GA_MEMORY_MGR_H_
#define _IMP_GA_MEMORY_MGR_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct impMEM_NODE_S
{
	struct impMEM_NODE_S *pstPre;
	struct impMEM_NODE_S *pstNext;
	IMP_S8 *ps8MemStart;
	IMP_S8 *ps8MemEnd;
	IMP_S8 s8Used;
}MEM_NODE_S;

typedef struct impMEM_NODE_S IMP_MEM_NODE;


typedef struct impMEM_MGR_S
{
	IMP_S8 *s8MemAddr;
	IMP_S32 s32MemLen;
	IMP_MEM_NODE *pstNodes;
	IMP_MEM_NODE **ppstNodep;
	IMP_S32 s32NodeNum;
	IMP_S32 s32NodeAvail;
	IMP_S32 s32NodeMin;
	IMP_S32 s32MemUsed;
	IMP_S32 s32MemMax;
	IMP_MEM_NODE *pstMemNodeHead;
} MEM_MGR_S;


typedef enum impMEM_BLOCK_TYPE_E
{
	IMP_MEMBLK_TYPE_FAST = 0,
	IMP_MEMBLK_TYPE_SLOW,
	IMP_MEMBLK_TYPE_MMGR,
	IMP_MEMBLK_NUM = IMP_MEMBLK_TYPE_MMGR
} MEM_BLOCK_TYPE_E;


typedef struct impMEM_MGR_REQ_S
{
	IMP_U32 au32NodNum[IMP_MEMBLK_NUM];
	IMP_U32 u32NodSize;
	IMP_VOID *pNodMem;
} MEM_MGR_REQ_S;


typedef struct
{
	MEM_MGR_S astMemMgrs[IMP_MEMBLK_NUM];
} MEM_MGR_ARRAY_S;


IMP_EXPORTS IMP_VOID IMP_MMGetMgrReq( MEM_MGR_REQ_S *pstReq );
IMP_EXPORTS IMP_VOID IMP_MMInit( MEM_MGR_ARRAY_S *pstMgr, MEM_MGR_REQ_S *pstReq, MEM_SET_S *pstReqs );
IMP_EXPORTS IMP_VOID IMP_MMTerminate( MEM_MGR_ARRAY_S *pstMgr );

IMP_EXPORTS IMP_VOID *IMP_MMAlloc( MEM_MGR_ARRAY_S *pstMgr, MEM_BLOCK_TYPE_E enMemType, IMP_S32 s32Size );

IMP_EXPORTS IMP_VOID IMP_MMFree( MEM_MGR_ARRAY_S *pstMgr, MEM_BLOCK_TYPE_E enMemType, IMP_VOID *p );


#ifdef __cplusplus
}
#endif

#endif
