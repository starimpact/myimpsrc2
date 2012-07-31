#ifndef _IMP_PEA_OBJRECOGNITION_H_
#define _IMP_PEA_OBJRECOGNITION_H_

#include "imp_pea_detector.h"
#include "imp_pea_tracker.h"


#ifdef __cplusplus
extern "C"
{
#endif

typedef struct impPEA_ModuleObjRecognition
{
	IMP_MODULE_HANDLE hDetector;
	IMP_MODULE_HANDLE hTracker;
	IMP_MODULE_HANDLE hClassifier;
	PEA_RESULT_S *pstResult;
	GA_HARDWARE_RS_S *pstHwResouce;
} PEA_ModuleObjRecognition;


IMP_MODULE_HANDLE IMP_PEA_CreateObjRecognition( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResouce );
IMP_VOID IMP_PEA_ReleaseObjRecognition( IMP_MODULE_HANDLE hModule );
IMP_VOID IMP_PEA_ConfigObjRecognition( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstVapara );
IMP_VOID IMP_PEA_StartObjRecognition( IMP_MODULE_HANDLE hModule );
IMP_VOID IMP_PEA_StopObjRecognition( IMP_MODULE_HANDLE hModule );
IMP_S32 IMP_PEA_ProcessObjRecognition( IMP_MODULE_HANDLE hModule );
IMP_S32 IMP_PEA_PostProcessObjRecognition( IMP_MODULE_HANDLE hModule );


#ifdef __cplusplus
}
#endif

#endif
