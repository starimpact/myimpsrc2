/**
* \defgroup 缓冲区操作
* 缓冲区操作
* @ingroup 通用算法
* @author 北京银瀑技术
* @version 1.0
* @data 2009-2010
*/
/*@{*/


#ifndef _IMP_GA_COMMON_H_
#define _IMP_GA_COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#include "imp_algo_type.h"
#include "imp_ga_math.h"
#include "imp_ga_image_mgr.h"
#include "imp_ga_carray.h"
#include "imp_ga_memory_mgr.h"
#include "imp_ga_dma.h"

#ifdef _TIDSP
#include <std.h>
#include <limits.h>
#include <idma2.h>
#include <acpy2.h>
#endif

/** 名称缓冲区长度 */
#define IMP_NAME_BUF_SIZE 16
/** 文件缓冲区长度 */
#define IMP_FILE_BUF_SIZE (1024 * 4)

#ifdef __cplusplus
extern "C"
{
#endif


typedef struct impGA_SYS_TIME_S
{
	IMP_U32 u32Time;
	IMP_U32 u32Date;
} GA_SYS_TIME_S;


#define IMP_GA_STRING_BUF_LEN	(1024 * 4)


typedef struct impSTRING_S
{
	IMP_S32	s32Length;
	IMP_U8	au8Buffer[IMP_GA_STRING_BUF_LEN - sizeof(IMP_S32) * 1];
} GA_STRING_S;


typedef enum impGA_ENABLE_FLAG_E
{
	IMP_GA_ENA_FLAG_DISABLE		= 0,
	IMP_GA_ENA_FLAG_ENABLE		= 1
} GA_ENABLE_FLAG_E;


typedef enum impGA_ALARM_LEVEL_E
{
	IMP_GA_ALARM_LEVEL_NOT		= 0,
	IMP_GA_ALARM_LEVEL_LOW		= 1,
	IMP_GA_ALARM_LEVEL_MEDIUM	= 2,
	IMP_GA_ALARM_LEVEL_HIGH		= 3
} GA_ALARM_LEVEL_E;


#define IMP_MAKE_VERSION( u32TypeM, u32TypeS, u32SerialM, u32SerialS ) \
	( 0	\
	| ( (((IMP_U32)u32TypeM) & 0xFF) << 0 ) \
	| ( (((IMP_U32)u32TypeS) & 0xFF) << 8 ) \
	| ( (((IMP_U32)u32SerialM) & 0xFF) << 16 ) \
	| ( (((IMP_U32)u32SerialS) & 0xFF) << 24 ) \
		)



typedef struct impGA_HARDWARE_RS_S
{
	MEM_MGR_ARRAY_S stMemMgr;
	DMA_CHANNLS_S stIDMAChs;
} GA_HARDWARE_RS_S;


IMP_EXPORTS IMP_VOID ipDataBufferAlloc( DATA_BUF_S *pstDataBuf, IMP_S32 s32BufLen, IMP_VOID *pMemMgr );

IMP_EXPORTS IMP_VOID ipDataBufferFree( DATA_BUF_S *pstDataBuf, IMP_VOID *pMemMgr );

IMP_EXPORTS IMP_VOID ipDataBufferClear( DATA_BUF_S *pstDataBuf );

IMP_EXPORTS IMP_VOID ipDataBufferClone( DATA_BUF_S *pstSrc, DATA_BUF_S *pstDst );

IMP_EXPORTS IMP_VOID ipReadString( IMP_S8 *s8Str, DATA_BUF_S *pstDataBuf );

IMP_EXPORTS IMP_VOID ipWriteString( DATA_BUF_S *pstDataBuf, IMP_S8 *s8Str );

IMP_EXPORTS IMP_VOID ipReadFileData( IMP_S8 *s8FileName, DATA_BUF_S *pstDataBuf );

IMP_EXPORTS IMP_VOID ipWriteFileData( DATA_BUF_S *pstDataBuf, IMP_S8 *s8FileName );

IMP_EXPORTS IMP_VOID *ipMemAlloc( IMP_S32 sz, IMP_VOID *pMemMgr );

IMP_EXPORTS IMP_VOID ipMemFree( IMP_VOID *mem, IMP_VOID *pMemMgr );


#ifdef __cplusplus
}
#endif

#endif
