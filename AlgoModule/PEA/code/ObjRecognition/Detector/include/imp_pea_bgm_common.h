
#ifndef _IMP_PEA_BG_MODEL_COMMON_H_
#define _IMP_PEA_BG_MODEL_COMMON_H_

#include "imp_pea_detector_common.h"

typedef struct impPEA_BG_MODEL_DATA_S
{
	PEA_DETECTOR_DATA_S *pstDetectorData;
	IMP_U32 u32FgRatio;
	IMP_U32 u32BgWorkRatio;
	IMP_S32 s32BgLumi;
	IMP_S32 s32DoStabilize;
	IMP_S32 s32ConstMvDetected;
	IMP_S32 s32IsShaking;
	IMP_POINT_S stEstimateMv;
} PEA_BG_MODEL_DATA_S;

#endif
