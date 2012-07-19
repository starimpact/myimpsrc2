
#ifndef _IMP_SWING_MODEL_H_
#define _IMP_SWING_MODEL_H_


#include "imp_pea_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

//算法配置参数
typedef struct impSwingPara_S
{
	IMP_S32 s32Tmp; //
}IMP_SwingPara_S;


/***************************************************
name:
do:
input:
	
output:
history:
****************************************************/
IMP_S32 IMP_GetMemSizeSwing(PEA_RESULT_S *pstResult);



IMP_MODULE_HANDLE IMP_CreateSwing(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);


IMP_S32 IMP_ReleaseSwing(IMP_MODULE_HANDLE hModule);


IMP_S32 IMP_ConfigSwingModel(IMP_MODULE_HANDLE hModule, IMP_SwingPara_S *pstPara);


IMP_S32 IMP_ProcessSwing(IMP_MODULE_HANDLE hModule);


#ifdef __cplusplus
}
#endif

#endif



