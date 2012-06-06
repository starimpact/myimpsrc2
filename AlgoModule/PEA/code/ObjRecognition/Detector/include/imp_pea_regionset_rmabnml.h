#ifndef _IMP_PEA_REGIONSET_RMABNML_H_
#define _IMP_PEA_REGIONSET_RMABNML_H_

#include "imp_pea_common.h"

typedef struct impPEA_RGE_REMOVE_ABNORMAL_PARA_S
{
	IMP_S32 s32ThDrgMinSizeWh;
	IMP_S32 s32ThDrgMinSizePixel;
	IMP_S32 s32ThDrgMaxSizeRatio;
	IMP_S32 s32ThDrgAspectRatio;
	IMP_S32 s32ThDrgFillRatio;
	IMP_S32 s32ThDrgMinSizeActual;
	IMP_S32 s32ThDrgMaxSizeActual;
} PEA_RGE_REMOVE_ABNORMAL_PARA_S;

IMP_EXPORTS IMP_VOID IMP_PEA_RGE_RemoveAbnormalRegions( PEA_RGE_REMOVE_ABNORMAL_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );


#endif
