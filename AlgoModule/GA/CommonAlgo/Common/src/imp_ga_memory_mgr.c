#include "imp_ga_memory_mgr.h"


IMP_VOID IMP_MemsAlloc( MEM_SET_S *pstMems )
{
	IMP_S32 i, mem_num;

	mem_num = pstMems->u32MemNum;
	for( i=0; i<mem_num; i++ )
	{
		pstMems->stMems[i].pMem = malloc( pstMems->stMems[i].u32Size );
	//	printf("pstMems->stMems[%d].u32Size:%d\n", i, pstMems->stMems[i].u32Size);
	}
}

IMP_VOID IMP_MemsFree( MEM_SET_S *pstMems )
{
	IMP_S32 i, mem_num;

	mem_num = pstMems->u32MemNum;
	for( i=0; i<mem_num; i++ )
		free( pstMems->stMems[i].pMem );
}

static IMP_VOID IMP_MemMgrInit( MEM_MGR_S *pstMgr, IMP_VOID *node_mem, IMP_S32 node_num, IMP_VOID *mem, IMP_S32 bufsize )
{
	IMP_S32 i;

	memset( mem, 0, bufsize );
	pstMgr->s8MemAddr = mem;
	pstMgr->s32MemLen = bufsize;
	pstMgr->s32NodeAvail = pstMgr->s32NodeNum = node_num;
	pstMgr->pstNodes = (IMP_MEM_NODE*)node_mem;
	pstMgr->ppstNodep = (IMP_MEM_NODE**)( (IMP_MEM_NODE*)node_mem + node_num );
	for( i=0; i<node_num; ++i )
		pstMgr->ppstNodep[i] = pstMgr->pstNodes+i;
	--pstMgr->s32NodeAvail;
	pstMgr->pstMemNodeHead = pstMgr->ppstNodep[pstMgr->s32NodeAvail];
	memset( pstMgr->pstMemNodeHead, 0, sizeof(IMP_MEM_NODE) );
	pstMgr->pstMemNodeHead->ps8MemStart = pstMgr->s8MemAddr;
	pstMgr->pstMemNodeHead->ps8MemEnd = pstMgr->s8MemAddr + pstMgr->s32MemLen;
	pstMgr->pstMemNodeHead->s8Used = IMP_FALSE;
	pstMgr->s32MemUsed = 0;
	pstMgr->s32MemMax = 0;
	pstMgr->s32NodeMin = pstMgr->s32NodeAvail;
}

static IMP_VOID IMP_MemMgrTerminate( MEM_MGR_S *pstMgr )
{

	if( pstMgr->s32MemUsed )
	{
		IMP_MEM_NODE *p_node;
		printf( "there is memory leak in mem_mgr!\n" );
		p_node = pstMgr->pstMemNodeHead;
		while( p_node )
		{
			if( p_node->s8Used )
			{
				printf( "%x!\n", p_node->ps8MemStart );
			}
			p_node = p_node->pstNext;
		}
	}
	printf( "max used mem is (%d, %d)\n", pstMgr->s32MemLen, pstMgr->s32MemMax );

	memset( pstMgr, 0, sizeof(MEM_MGR_S) );
}

static IMP_VOID *IMP_MemMgrAlloc( MEM_MGR_S *pstMgr, IMP_S32 sz )
{
	IMP_MEM_NODE *p_node_new, *p_node_old, *p_node_cur;
	IMP_S32 sz_cur, sz_min;
	IMP_VOID *res;

	sz = (sz + 63) & (~63);
	if( !pstMgr->s32NodeAvail )
	{
		printf( "memory allocation NODE_T is not enough!\n" );
		return NULL;
	}

	p_node_cur = NULL; sz_min = 0x7FFFFFFFL;
	p_node_old = pstMgr->pstMemNodeHead;
	
	while( p_node_old )
	{
		if( !(p_node_old->s8Used) )
		{
			sz_cur = p_node_old->ps8MemEnd - p_node_old->ps8MemStart;
			if(  sz_cur>=sz && sz_cur<sz_min  )
			{
				p_node_cur = p_node_old;
				sz_min = sz_cur;
			}
		}
		p_node_old = p_node_old->pstNext;
	}

	res = NULL;
//	printf("p_node_cur:%d\n", (int)p_node_cur);
	if( p_node_cur )
	{
		pstMgr->s32MemUsed += sz;
		p_node_cur->ps8MemEnd -= sz;
		res = p_node_cur->ps8MemEnd;
		
		if( p_node_cur->ps8MemStart == p_node_cur->ps8MemEnd )
		{
			p_node_cur->s8Used = IMP_TRUE;
			p_node_cur->ps8MemEnd = p_node_cur->ps8MemStart + sz;
		}
		else if( p_node_cur->ps8MemStart > p_node_cur->ps8MemEnd )
		{
			printf("memmgr_alloc error: >\n");
		}
		else
		{
			--(pstMgr->s32NodeAvail);
			p_node_new = pstMgr->ppstNodep[pstMgr->s32NodeAvail];
			memset( p_node_new, 0, sizeof(IMP_MEM_NODE) );
			p_node_new->ps8MemStart = p_node_cur->ps8MemEnd;
			p_node_new->ps8MemEnd = p_node_new->ps8MemStart + sz;
			p_node_new->s8Used = IMP_TRUE;
			p_node_new->pstPre = p_node_cur;
			p_node_new->pstNext = p_node_cur->pstNext;
			p_node_cur->pstNext = p_node_new;
			if( p_node_new->pstNext )
				p_node_new->pstNext->pstPre = p_node_new;
		}
	}
	if( pstMgr->s32MemMax < pstMgr->s32MemUsed )
		pstMgr->s32MemMax = pstMgr->s32MemUsed;
	if( pstMgr->s32NodeAvail < pstMgr->s32NodeMin )
		pstMgr->s32NodeMin = pstMgr->s32NodeAvail;
	if( res )
	{
		memset( res, 0, sz );
	}
	else
		printf( "general memory allocation failed!\n" );

	return res;
}

static IMP_S8 IMP_MemMgrFree( MEM_MGR_S *pstMgr, IMP_VOID *p )
{
	IMP_MEM_NODE *p_node_old;

	p_node_old = pstMgr->pstMemNodeHead;
	while( p_node_old )
	{
		if( p_node_old->s8Used
			&& ((IMP_S32)p) == ((IMP_S32)(p_node_old->ps8MemStart)) )
		{
			p_node_old->s8Used = IMP_FALSE;
			pstMgr->s32MemUsed -= (p_node_old->ps8MemEnd-p_node_old->ps8MemStart);
			break;
		}
		p_node_old = p_node_old->pstNext;
	}
	if( p_node_old )
	{
		if( p_node_old->pstPre && !(p_node_old->pstPre->s8Used) )
		{
			pstMgr->ppstNodep[pstMgr->s32NodeAvail] = p_node_old;
			++(pstMgr->s32NodeAvail);

			p_node_old->pstPre->ps8MemEnd = p_node_old->ps8MemEnd;

			p_node_old->pstPre->pstNext = p_node_old->pstNext;
			if( p_node_old->pstNext )
				p_node_old->pstNext->pstPre = p_node_old->pstPre;

			p_node_old = p_node_old->pstPre;
 			memset( pstMgr->ppstNodep[pstMgr->s32NodeAvail-1], 0, sizeof(IMP_MEM_NODE) );
		}
		if( p_node_old->pstNext && !(p_node_old->pstNext->s8Used) )
		{
			pstMgr->ppstNodep[pstMgr->s32NodeAvail] = p_node_old->pstNext;
			++(pstMgr->s32NodeAvail);

			p_node_old->ps8MemEnd = p_node_old->pstNext->ps8MemEnd;

			p_node_old->pstNext = p_node_old->pstNext->pstNext;
			if( p_node_old->pstNext )
				p_node_old->pstNext->pstPre = p_node_old;

 			memset( pstMgr->ppstNodep[pstMgr->s32NodeAvail-1], 0, sizeof(IMP_MEM_NODE) );
		}
	}
	else
	{
		printf( "free general memory failed!\n" );
		return IMP_FALSE;
	}
	return IMP_TRUE;
}


IMP_VOID IMP_MMGetMgrReq( MEM_MGR_REQ_S *pstReq )
{
	IMP_S32 nodsz = sizeof(IMP_MEM_NODE)+sizeof(IMP_MEM_NODE*);
	pstReq->pNodMem = NULL;
	pstReq->u32NodSize = nodsz * ( pstReq->au32NodNum[IMP_MEMBLK_TYPE_FAST] + pstReq->au32NodNum[IMP_MEMBLK_TYPE_SLOW] ) + 256;
}

IMP_VOID IMP_MMInit( MEM_MGR_ARRAY_S *pstMgr, MEM_MGR_REQ_S *pstReq, MEM_SET_S *pstReqs )
{
	IMP_S32 nodsz = sizeof(IMP_MEM_NODE)+sizeof(IMP_MEM_NODE*);
	IMP_MemMgrInit( &pstMgr->astMemMgrs[IMP_MEMBLK_TYPE_FAST],
						pstReq->pNodMem, pstReq->au32NodNum[IMP_MEMBLK_TYPE_FAST],
						pstReqs->stMems[IMP_MEMBLK_TYPE_FAST].pMem, pstReqs->stMems[IMP_MEMBLK_TYPE_FAST].u32Size );
	IMP_MemMgrInit( &pstMgr->astMemMgrs[IMP_MEMBLK_TYPE_SLOW],
						(IMP_U8*)pstReq->pNodMem+nodsz*(pstReq->au32NodNum[IMP_MEMBLK_TYPE_FAST])+128, pstReq->au32NodNum[IMP_MEMBLK_TYPE_SLOW],
						pstReqs->stMems[IMP_MEMBLK_TYPE_SLOW].pMem, pstReqs->stMems[IMP_MEMBLK_TYPE_SLOW].u32Size );
}

IMP_VOID IMP_MMTerminate( MEM_MGR_ARRAY_S *pstMgr )
{
	IMP_MemMgrTerminate( &pstMgr->astMemMgrs[IMP_MEMBLK_TYPE_FAST] );
	IMP_MemMgrTerminate( &pstMgr->astMemMgrs[IMP_MEMBLK_TYPE_SLOW] );
}

IMP_VOID *IMP_MMAlloc( MEM_MGR_ARRAY_S *pstMgr, MEM_BLOCK_TYPE_E enMemType, IMP_S32 sz )
{
	IMP_VOID *mem;
	if( pstMgr )
		mem = IMP_MemMgrAlloc( &pstMgr->astMemMgrs[enMemType], sz );
	else
		mem = malloc( sz );
	return mem;
}

IMP_VOID IMP_MMFree( MEM_MGR_ARRAY_S *pstMgr, MEM_BLOCK_TYPE_E enMemType, IMP_VOID *p )
{
	if( pstMgr )
		IMP_MemMgrFree( &pstMgr->astMemMgrs[enMemType], p );
	else
		free( p );
}
