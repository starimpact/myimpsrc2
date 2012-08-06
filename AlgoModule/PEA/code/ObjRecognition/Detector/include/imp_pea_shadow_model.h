#ifndef _IMP_PEA_SHADOW_MODLE_H_
#define _IMP_PEA_SHADOW_MODLE_H_

#include "imp_pea_common.h"

typedef struct
{
	IMP_S16 s16Bottom;
	IMP_S16 s16Top;
	IMP_U8 u8HeightRatio;
	IMP_U8 u8ShadowRatio;
	IMP_U8 u8IsShadow;
} IpShadowColumn;


typedef struct
{
	IMP_S16 s16Start;
	IMP_S16 s16End;
	IMP_U8 u8Used;
} IpShadowInterval;


typedef struct
{
	IMP_S16 s16MaxHeight;
	IMP_S16 s16MaxIntervalIdx;
	IMP_S32 s32Width;
	IMP_S32 s32Height;
	IpShadowInterval *pstIntervals;
	IpShadowColumn *pstColumns;
} IpShadowImage;


typedef struct
{
	IMP_S32 s32DetectShadow;
	IMP_S32 s32ThShapeHeightRatio;
	IMP_S32 s32ThShadowNumRatio;
	IMP_S32 s32ThShadowHeightRatio;
	IMP_S32 s32ThShadowMinWidth;
	IMP_S32 s32ThShadowMinHeight;
	IMP_S32 s32ThShadowMaxAreaPixel;
	IMP_S32 s32ThShadowMerge;
	IMP_S32 s32ThShadowDel;
} IpRGEShadowPara;


IMP_S32 IMP_GetMemSizeShadowImage(IMP_S32 s32Width, IMP_S32 s32Height);

IMP_VOID ipRemoveRegionSetShadow( IpRGEShadowPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IpShadowImage *pstShadow );

IMP_VOID ipShadowImageCreate( IpShadowImage *pstShadow, IMP_S32 s32Width, IMP_S32 s32Height, MEM_MGR_ARRAY_S *pstMemMgr );

IMP_VOID ipShadowImageDestroy( IpShadowImage *pstShadow, MEM_MGR_ARRAY_S *pstMemMgr );

IMP_VOID ipShadowImageClear( IpShadowImage *pstShadow );

static IMP_VOID ipDetectRegionShadow( IpRGEShadowPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs,
								 IMP_S32 s32RegionIndex, IpShadowImage *pstShadow );
#endif
