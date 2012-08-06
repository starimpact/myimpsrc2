
#ifndef _IPDETECTOR_H_
#define _IPDETECTOR_H_

#include "imp_avd.h"
#include "imp_pea_common.h"
#include "imp_pea_detector_common.h"
#include "imp_pea_bgm_common.h"
#include "imp_vibe_bgmodel.h"
#include "imp_osc_detect.h"
#include "imp_pea_watermark.h"
#include "imp_pea_bgm_stabilizer.h"
#include "imp_pea_region_extract.h"



#ifdef IMP_ARM_PEA
	#define USE_LI_FAST_BGMODEL
#else
	#define USE_CODEBOOK_MODELING
#endif

#define USE_WATERMARK_DETECOTR


#define IMP_SCENE_CHANGE_RADIO 40
#define IMP_SCENE_CHANGE_CNT 8
#define IMP_DETECTOR_METHOD_RADIO 5

#ifdef __cplusplus
extern "C"
{
#endif



typedef struct  impPEA_DETCTOR_PREPRO_PARA_S
{
	IMP_S32 s32DoSmooth;
} PEA_DETCTOR_PREPRO_PARA_S;


typedef struct impPEA_DETECTOR_PARA_S
{
	PEA_DETCTOR_PREPRO_PARA_S stPreProPara;
	PEA_REGION_EXTRACT_PARA_S stRgePara;
//	IpCodebookBgModelPara stCodebookPara;
//	IpCodebookPara stCbPara;
	IMP_S32 s32DetMotion;
	IMP_S32 s32DetStaticLeft;
	IMP_S32 s32DetStaticRemove;
	IMP_S32	s32FrmDura;
	RULE_S *pstRule;
} PEA_DETECTOR_PARA_S;


typedef struct impPEA_TARGET_DETECTOR_S
{
	GA_HARDWARE_RS_S *pstHwResource;
	PEA_RESULT_S *pstResult;
	PEA_DETECTOR_PARA_S stPara;
	PEA_DETECTOR_DATA_S stData;

	IMP_MODULE_HANDLE hViBeModel; //ViBe Model
	IMP_MODULE_HANDLE hOSCDModel; //OSCD Model

	PEA_REGION_EXTRACT_MODULE_S stRgExtract;


#ifdef USE_WATERMARK_DETECOTR
	IpWaterMarkDetector stWaterMarkDetector;
#endif


	GRAY_IMAGE_S stImgInGrayDiff;
	GRAY_IMAGE_S stImgBgGray;
#ifndef IMP_ARM_PEA
	GRAY_IMAGE_S stImgStBgGray;
	GRAY_IMAGE_S stImgBgDiff;
#endif
	GRAY_IMAGE_S stImgBgEdge;
	GRAY_IMAGE_S stImgBgEdgeMag;
#if defined( USE_CODEBOOK_MODELING ) || defined(USE_CODEBOK_MODELING_OPTIMAZIED )
#ifdef USE_MAG_DIFF
	GRAY_IMAGE_S stImgBgInDiffMag;
#endif
#endif

	GRAY_IMAGE16_S stImgBgEdgeGrad;
	GRAY_IMAGE_S stImgInEdge;
	GRAY_IMAGE_S stImgInEdgeMag;
	GRAY_IMAGE16_S stImgInEdgeGrad;
	GRAY_IMAGE_S stImgInEdgeMagPre;
	GRAY_IMAGE_S stImgFgOrgMotion;
	GRAY_IMAGE_S stImgFgRgnMotion;
	GRAY_IMAGE_S stImgContour;

	GRAY_IMAGE_S stImgFgMorph;
	GRAY_IMAGE_S stImgMediate;
#ifdef ACCUM_BGEDGE
	GRAY_IMAGE_S pstImgBgEdgeG;
#endif

} PEA_TARGET_DETECTOR_S;


IMP_S32 IMP_GetMemSizeDetector(IMP_S32 s32Width, IMP_S32 s32Height);

IMP_VOID ipParseDetectorParaData(PEA_DETECTOR_PARA_S *pstPara );

IMP_VOID ipCreateTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pHwResouce );

IMP_VOID ipReleaseTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector );

IMP_VOID ipConfigTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector, PEA_DETECTOR_PARA_S *pstPara);

IMP_S32 ipProcessTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector );

IMP_S32 ipPostProcessTargetDetectorInternal( PEA_TARGET_DETECTOR_S *pstTargetDetector );

IMP_EXPORTS IMP_MODULE_HANDLE ipCreateDetector( PEA_RESULT_S *pResult, GA_HARDWARE_RS_S *pHwResouce );

IMP_VOID ipReleaseDetector( IMP_MODULE_HANDLE hModule );

IMP_EXPORTS IMP_VOID ipConfigDetector( IMP_MODULE_HANDLE hModule, INNER_PARA_S *pstParaInner );

IMP_EXPORTS IMP_S32 IMP_PEA_ProcessDetector( IMP_MODULE_HANDLE hModule );

IMP_EXPORTS IMP_S32 ipPostProcessDetector( IMP_MODULE_HANDLE hModule );


typedef struct
{
	PEA_TARGET_DETECTOR_S *pDetector;
} IpModuleDetector;

#ifdef __cplusplus
}
#endif

#endif
