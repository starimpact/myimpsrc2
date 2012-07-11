#ifndef _GRAYGAUSSIAN_H_
#define _GRAYGAUSSIAN_H_

#include "imp_pea_common.h"


IMP_S32 IMP_GetMemSizeGrayGaussian(PEA_RESULT_S *pstResult);

IMP_MODULE_HANDLE IMP_CreateGrayGaussian(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);

IMP_S32 IMP_ProcessGrayGaussian(IMP_MODULE_HANDLE hModule);

IMP_S32 IMP_ReleaseGrayGaussian(IMP_MODULE_HANDLE hModule);


#endif




