
#include "imp_ga_common.h"


IMP_VOID ipParaClear( EXTERNAL_PARA_S *pstPara, IMP_S32 s32AdvBufNum )
{
	pstPara->u32Type = 0;
	memset( &pstPara->stGlobal, 0, sizeof(GLOBAL_PARA_S) );
	ipDataBufferClear( &pstPara->stRule.stVanaData );
}


IMP_VOID ipDataBufferAlloc( DATA_BUF_S *pstDataBuf, IMP_S32 s32BufLen, IMP_VOID *pMemMgr )
{
	MEM_MGR_ARRAY_S *pstMMgr = (MEM_MGR_ARRAY_S*)pMemMgr;
	memset( pstDataBuf, 0, sizeof(DATA_BUF_S) );
	pstDataBuf->s32BufLen = s32BufLen;
	pstDataBuf->s32DatLen = 0;
	pstDataBuf->pu8Buf = IMP_MMAlloc( pstMMgr, IMP_MEMBLK_TYPE_SLOW, pstDataBuf->s32BufLen );
	memset( pstDataBuf->pu8Buf, 0, pstDataBuf->s32BufLen );
}

IMP_VOID ipDataBufferFree( DATA_BUF_S *pstDataBuf, IMP_VOID *pMemMgr )
{
	MEM_MGR_ARRAY_S *pstMMgr = (MEM_MGR_ARRAY_S*)pMemMgr;
	IMP_MMFree( pstMMgr, IMP_MEMBLK_TYPE_SLOW, pstDataBuf->pu8Buf );
	memset( pstDataBuf, 0, sizeof(DATA_BUF_S) );
}

IMP_VOID ipDataBufferClear( DATA_BUF_S *pstDataBuf )
{
	pstDataBuf->s32DatLen = 0;
	memset( pstDataBuf->pu8Buf, 0, pstDataBuf->s32BufLen );
}

IMP_VOID ipDataBufferClone( DATA_BUF_S *pstSrc, DATA_BUF_S *pstDst )
{
	pstDst->s32DatLen = pstSrc->s32DatLen;
	memcpy( pstDst->pu8Buf, pstSrc->pu8Buf, pstDst->s32BufLen );
}

IMP_VOID ipReadString( IMP_S8 *s8Str, DATA_BUF_S *pstDataBuf )
{
	IMP_S32 s32Len = strlen(s8Str);
	memcpy( &pstDataBuf->pu8Buf[pstDataBuf->s32DatLen], s8Str, s32Len );
	pstDataBuf->s32DatLen += s32Len;
}

IMP_VOID ipWriteString( DATA_BUF_S *pstDataBuf, IMP_S8 *s8Str )
{
	IMP_S32 s32Len = pstDataBuf->s32DatLen;
	memcpy( s8Str, &pstDataBuf->pu8Buf[0], s32Len );
	s8Str[s32Len] = 0;
}

IMP_VOID ipReadFileData( IMP_S8 *s8FileName, DATA_BUF_S *pstDataBuf )
{
	FILE *fil;
	fil = fopen( s8FileName, "rb" );
	if(fil==NULL)
	{
		printf("Failed to open file %s\n!",s8FileName);
	}
	pstDataBuf->s32DatLen = fread( pstDataBuf->pu8Buf, 1, pstDataBuf->s32BufLen, fil );
	fclose( fil );
}

IMP_VOID ipWriteFileData( DATA_BUF_S *pstDataBuf, IMP_S8 *s8FileName )
{
	FILE *fil;
	fil = fopen( s8FileName, "wb" );
	if(fil==NULL)
	{
		printf("Failed to open file %s\n!",s8FileName);
	}
	fwrite( pstDataBuf->pu8Buf, 1, pstDataBuf->s32DatLen, fil );
	fclose( fil );
}

IMP_VOID *ipMemAlloc( IMP_S32 sz, IMP_VOID *pMemMgr )
{
	MEM_MGR_ARRAY_S *pstMMgr = (MEM_MGR_ARRAY_S*)pMemMgr;
	return IMP_MMAlloc( pstMMgr, IMP_MEMBLK_TYPE_SLOW, sz );
}

IMP_VOID ipMemFree( IMP_VOID *mem, IMP_VOID *pMemMgr )
{
	MEM_MGR_ARRAY_S *pstMMgr = (MEM_MGR_ARRAY_S*)pMemMgr;
	IMP_MMFree( pstMMgr, IMP_MEMBLK_TYPE_SLOW, mem );
}


