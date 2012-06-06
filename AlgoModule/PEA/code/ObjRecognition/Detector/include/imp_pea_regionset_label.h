#ifndef _IMP_PEA_REGIONSET_LABEL_H_
#define _IMP_PEA_REGIONSET_LABEL_H_

#include "imp_pea_common.h"
#include "imp_pea_regionset_common.h"

typedef struct impPEA_RGE_LABLE_PARA_S
{
	IMP_S32 s32ThDrgMinSizeWh;
	IMP_S32 s32ThDrgMinSizePixelInit;
	IMP_S32 s32ThDrgMinSizePixelMax;
	IMP_S32 s32ThDrgMinSizePixelStep;
	IMP_S32 s32ThDrgMinSizePixelCur;
	IMP_S32 s32ThDrgMaxSizeRatio;
} PEA_RGE_LABLE_PARA_S;

IMP_VOID IMP_PEA_RGE_RemoveRegion( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index, IMP_U8 u8Sn );
IMP_VOID IMP_PEA_RGE_ExtractRegionSetFromFgImage(PEA_RGE_LABLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs,
										  IMP_RECT_S *pstRc, IMP_S32 s32Diff, PEA_REGION_EXTRACT_DATA_S *pstRgeData );


#endif
