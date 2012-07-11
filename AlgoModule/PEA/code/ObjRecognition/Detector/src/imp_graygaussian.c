#include "time.h"
#include "stdlib.h"

#include "imp_graygaussian.h"


IMP_S32 IMP_GetMemSizeGrayGaussian(PEA_RESULT_S *pstResult)
{
	
	return 0;
}


IMP_MODULE_HANDLE IMP_CreateGrayGaussian(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	IMP_MODULE_HANDLE hModule = 0;
	
	printf("hello CreateGrayGaussian.\n");
	
	return hModule;
}


IMP_S32 IMP_ProcessGrayGaussian(IMP_MODULE_HANDLE hModule)
{
	printf("hello ProcessGrayGaussian.\n");
	return 0;
}


IMP_S32 IMP_ReleaseGrayGaussian(IMP_MODULE_HANDLE hModule)
{
	printf("bye bye ProcessGrayGaussian.\n");
	return 0;
}


