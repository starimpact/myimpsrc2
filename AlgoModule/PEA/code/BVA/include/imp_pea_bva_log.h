#ifndef _IMP_PEA_BVA_LOG_H_
#define _IMP_PEA_BVA_LOG_H_

#include "imp_ga_common.h"
#include "imp_pea_common.h"
#include "imp_pea_bva.h"

#ifdef __cplusplus
extern "C"
{
#endif

IMP_S32 ipOpenAnalystLogFile();
IMP_S32 ipOutputAnalystDataFrame( BEHAVIOR_ANALYSIS_S *pstAnalyst);
IMP_S32 ipCloseAnalystLogFile();



#ifdef __cplusplus
}
#endif

#endif
