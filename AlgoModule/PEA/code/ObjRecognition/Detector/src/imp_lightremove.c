#include "time.h"
#include "stdlib.h"

#include "imp_lightremove.h"


IMP_MODULE_HANDLE IMP_CreateLightRemove(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	IMP_MODULE_HANDLE hModule;
	
	printf("hello CreateLightRemove\n");
	
	return hModule;
}

IMP_S32 IMP_ConfigLightRemove(IMP_MODULE_HANDLE hModule, IMP_LightRemovePara_S *pstLRP)
{
	printf("hello ConfigLightRemove\n");	
	
	return 0;
}

IMP_S32 IMP_ProcessLightRemove(IMP_MODULE_HANDLE hModule)
{
	printf("hello ProcessLightRemove\n");
	return 0;
}

IMP_S32 IMP_ReleaseLightRemove(IMP_MODULE_HANDLE hModule)
{
	printf("hello ReleaseLightRemove\n");
	return 0;
}


