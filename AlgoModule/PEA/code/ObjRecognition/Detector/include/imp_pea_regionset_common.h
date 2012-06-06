#ifndef _IMP_PEA_REGIONSET_COMMON_H_
#define _IMP_PEA_REGIONSET_COMMON_H_

#include "imp_pea_bgm_common.h"


typedef struct
{
	PEA_DETECTOR_DATA_S *pstDtData;
} PEA_REGION_EXTRACT_DATA_S;


IMP_VOID IMP_PEA_RGEDataClear( PEA_REGION_EXTRACT_DATA_S *pstData );


#endif
