#ifndef _IMP_VFD_H_
#define _IMP_VFD_H_

#include "imp_algo_type.h"
#include "imp_ga_common.h"
#include "imp_ga_image_algo.h"
#include "FaceDetectDef.h"
#include "facedetectmain.h"
#include "imp_vfd_para.h"
#include "imp_ga_evt_manager.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct impVFD_MODULE
{
	FaceRecognitionHandle m_pFaceInstance;
	IMP_VFD_PARA_S m_stVfdPara;
	VFD_RESULT_S m_stVfdResult;
}VFD_MODULE;

#ifdef __cplusplus
}
#endif

#endif
