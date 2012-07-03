
#ifndef _LIGHTREMOVE_H_
#define _LIGHTREMOVE_H_

#include "imp_pea_common.h"

typedef struct impLightRemovePara_S
{
	IMP_FLOAT fthesh1;
	IMP_FLOAT fthesh2;
	IMP_FLOAT fdelay;
}IMP_LightRemovePara_S;


IMP_MODULE_HANDLE IMP_CreateLightRemove(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);

IMP_S32 IMP_ConfigLightRemove(IMP_MODULE_HANDLE hModule, IMP_LightRemovePara_S *pstLRP);

IMP_S32 IMP_ProcessLightRemove(IMP_MODULE_HANDLE hModule);

IMP_S32 IMP_ReleaseLightRemove(IMP_MODULE_HANDLE hModule);


#endif


