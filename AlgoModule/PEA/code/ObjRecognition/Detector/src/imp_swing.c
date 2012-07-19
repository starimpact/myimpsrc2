
#include "imp_swing.h"
#include "time.h"
#include "stdlib.h"


typedef struct impSwing_S
{
	PEA_RESULT_S *pstResult;
	GA_HARDWARE_RS_S *pstHwResource
} IMP_Swing_S;



IMP_S32 IMP_GetMemSizeSwing(PEA_RESULT_S *pstResult)
{
	return 0;
}


IMP_MODULE_HANDLE IMP_CreateSwing(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource)
{
	IMP_MODULE_HANDLE hModule = 0;
	
	printf("hello CreateSwing.\n");
	
	return hModule;
}


IMP_S32 IMP_ReleaseSwing(IMP_MODULE_HANDLE hModule)
{
	printf("hello ReleaseSwing.\n");
	
	return 0;
}


IMP_S32 IMP_ConfigSwingModel(IMP_MODULE_HANDLE hModule, IMP_SwingPara_S *pstPara)
{
	printf("hello ConfigSwingModel.\n");

	return 0;
}


IMP_S32 IMP_ProcessSwing(IMP_MODULE_HANDLE hModule)
{
	printf("hello ProcessSwing.\n");
	
	return 0;
}







