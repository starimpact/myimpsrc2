
#ifndef _IMP_PEA_REGION_EXTRACT_H_
#define _IMP_PEA_REGION_EXTRACT_H_

#include "imp_pea_regionset_label.h"
#include "imp_pea_regionset_suppress_fg.h"
#include "imp_pea_shadow_model.h"
#include "imp_pea_regionset_type.h"
#include "imp_pea_regionset_merge.h"
#include "imp_pea_regionset_rmabnml.h"

typedef struct impPEA_REGION_EXTRACT_PARA_S
{
	PEA_RGE_LABLE_PARA_S stLabelPara;
	PEA_RGE_SUPPRESS_FG_PARA_S stSuppressFgPara;
	IpRGEShadowPara stShadowPara;
	IpRGETypePara stTypePara;
	PEA_RGE_MERGE_PARA_S stMergePara;
	PEA_RGE_REMOVE_ABNORMAL_PARA_S stRmAbnmlPara;
} PEA_REGION_EXTRACT_PARA_S;

typedef struct
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	PEA_REGION_EXTRACT_PARA_S stPara;
	PEA_REGION_EXTRACT_DATA_S stData;
	IpShadowImage stShadowImage;
} PEA_REGION_EXTRACT_MODULE_S;

IMP_VOID ipCreateRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule, PEA_DETECTOR_DATA_S *detdata, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule );
IMP_VOID ipConfigRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule, PEA_REGION_EXTRACT_PARA_S *pstPara );
IMP_S32 ipProcessRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule );
IMP_VOID ipPostprocessRegionExtract( PEA_REGION_EXTRACT_MODULE_S *pModule );

#endif
