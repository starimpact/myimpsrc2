#include "imp_pea_detector.h"
#include "imp_pea_system.h"

IMP_VOID ipParseDetectorParaData(PEA_DETECTOR_PARA_S *pstPara )
{
	PEA_DETCTOR_PREPRO_PARA_S *pstDetPara = &pstPara->stPreProPara;
	PEA_REGION_EXTRACT_PARA_S *pstRgExPara = &pstPara->stRgePara;

	pstDetPara->s32DoSmooth = IMP_DETECTOR_DO_SMOOTH;

	pstRgExPara->stLabelPara.s32ThDrgMinSizeWh = IMP_DETECTOR_TH_DRG_MIN_SIZE_WH;
	pstRgExPara->stLabelPara.s32ThDrgMinSizePixelInit = IMP_DETECTOR_TH_DRG_MIN_SIZE_PIXEL_INIT;
	pstRgExPara->stLabelPara.s32ThDrgMinSizePixelMax = IMP_DETECTOR_TH_DRG_MIN_SIZE_PIXEL_MAX;
	pstRgExPara->stLabelPara.s32ThDrgMinSizePixelStep = IMP_DETECTOR_TH_DRG_MIN_SIZE_PIXEL_STEP;
	pstRgExPara->stLabelPara.s32ThDrgMaxSizeRatio = IMP_DETECTOR_TH_DRG_MAX_SIZE_RATIO;


	pstRgExPara->stRmAbnmlPara.s32ThDrgFillRatio = IMP_DETECTOR_TH_DRG_FILL_RATIO;
	pstRgExPara->stRmAbnmlPara.s32ThDrgAspectRatio = IMP_DETECTOR_TH_DRG_ASPECT_RATIO;
	pstRgExPara->stRmAbnmlPara.s32ThDrgMinSizeActual = IMP_DETECTOR_TH_DRG_MIN_SIZE_ACTUAL;
	pstRgExPara->stRmAbnmlPara.s32ThDrgMaxSizeActual = IMP_DETECTOR_TH_DRG_MAX_SIZE_ACTUAL;


	pstRgExPara->stTypePara.s32MotionIsLeft = IMP_DETECTOR_MOTION_IS_LEFT;
	pstRgExPara->stTypePara.s32ThRgTmDifgRatio = IMP_DETECTOR_TH_RGTM_DIFG_RATIO;
	pstRgExPara->stTypePara.s32ThRgTmWidhRatio = IMP_DETECTOR_TH_RGTM_WIDH_RATIO;
	pstRgExPara->stTypePara.s32ThRgTsSizeMin = IMP_DETECTOR_TH_RGTS_SIZE_MIN;
	pstRgExPara->stTypePara.s32ThRgTsSizeMax = IMP_DETECTOR_TH_RGTS_SIZE_MAX;
	pstRgExPara->stTypePara.s32ThRgTsEdgeMin = IMP_DETECTOR_TH_RGTS_EDGE_MIN;
	pstRgExPara->stTypePara.s32ThRgTsEdgeMax = IMP_DETECTOR_TH_RGTS_EDGE_MAX;
	pstRgExPara->stTypePara.s32ThRgTsWidhRatio = IMP_DETECTOR_TH_RGTS_WIDH_RATIO;
	pstRgExPara->stTypePara.s32ThRgTsInBgSize = IMP_DETECTOR_TH_RGTS_INBG_SIZE;
	pstRgExPara->stTypePara.s32ThRgTsInBgDiff = IMP_DETECTOR_TH_RGTS_INBG_DIFF;
	pstRgExPara->stTypePara.s32ThRgTsStrengthRatioL = IMP_DETECTOR_TH_RGTS_STRENGTH_RATIO_L;
	pstRgExPara->stTypePara.s32ThRgTsShapeRatioL = IMP_DETECTOR_TH_RGTS_SHAPE_RATIO_L;
	pstRgExPara->stTypePara.s32ThRgTsEdgeRatioL = IMP_DETECTOR_TH_RGTS_EDGE_RATIO_L;
	pstRgExPara->stTypePara.s32ThRgTsStrengthRatioR = IMP_DETECTOR_TH_RGTS_STRENGTH_RATIO_R;
	pstRgExPara->stTypePara.s32ThRgTsShapeRatioR = IMP_DETECTOR_TH_RGTS_SHAPE_RATIO_R;
	pstRgExPara->stTypePara.s32ThRgTsEdgeRatioR = IMP_DETECTOR_TH_RGTS_EDGE_RATIO_R;



	pstRgExPara->stMergePara.s32MergeRegion = IMP_DETECTOR_MERGE_REGION;
	pstRgExPara->stMergePara.s32MergeInner = IMP_DETECTOR_MERGE_INNER;
	pstRgExPara->stMergePara.s32MergeNeighbor = IMP_DETECTOR_MERGE_NEIGHBOR;
	pstRgExPara->stMergePara.s32MergeParallel = IMP_DETECTOR_MERGE_PARALLEL;
	pstRgExPara->stMergePara.stNeighborPara.s32NeighborDiff = IMP_DETECTOR_NEIGHBOR_DIFF;
	pstRgExPara->stMergePara.stNeighborPara.s32ThNeighborRatio = IMP_DETECTOR_TH_NEIGHBOR_RATIO;
	pstRgExPara->stMergePara.stParallelPara.s32ThMergeParalleDist = IMP_DETECTOR_TH_MERGE_PALELDIS;


	pstRgExPara->stSuppressFgPara.s32Enable = IMP_DETECTOR_SUPPRESS_FOREGROUND;
	pstRgExPara->stSuppressFgPara.s32MinPixelArea = IMP_DETECTOR_SUPFG_MIN_AREA_PIXEL;
	pstRgExPara->stSuppressFgPara.s32MaxPixelArea = IMP_DETECTOR_SUPFG_MAX_AREA_PIXEL;
	pstRgExPara->stSuppressFgPara.s32LightEnable = IMP_DETECTOR_SUPFG_LIGHT;
	pstRgExPara->stSuppressFgPara.s32MinLightArea = IMP_DETECTOR_SUPFG_MIN_AREA_LIGHT;
	pstRgExPara->stSuppressFgPara.s32MaxLightArea = IMP_DETECTOR_SUPFG_MAX_AREA_LIGHT;
	pstRgExPara->stSuppressFgPara.s32LightEdgeShapeRatio = IMP_DETECTOR_SUPFGLT_EDGE_SHAPE_RATIO;
	pstRgExPara->stSuppressFgPara.s32LightEdgeDistRatio = IMP_DETECTOR_SUPFGLT_EDGE_DIST_RATIO;
	pstRgExPara->stSuppressFgPara.s32LightEdgeLikehoodRatioFg = IMP_DETECTOR_SUPFGLT_EDGE_LIKE_RATIO_FG;
	pstRgExPara->stSuppressFgPara.s32LightEdgeLikehoodRatioBg = IMP_DETECTOR_SUPFGLT_EDGE_LIKE_RATIO_BG;
	pstRgExPara->stSuppressFgPara.s32LightUseDiff = IMP_DETECTOR_SUPFGLT_USE_DIFF;
	pstRgExPara->stSuppressFgPara.s32PenumbraEnable = IMP_DETECTOR_SUPFG_PENUMBRA;
	pstRgExPara->stSuppressFgPara.s32MinPenuArea = IMP_DETECTOR_SUPFG_MIN_AREA_PENU;
	pstRgExPara->stSuppressFgPara.s32MaxPenuArea = IMP_DETECTOR_SUPFG_MAX_AREA_PENU;
	pstRgExPara->stSuppressFgPara.s32PenuDiffRatio = IMP_DETECTOR_SUPFG_DIFF_RATIO;
	pstRgExPara->stSuppressFgPara.s32PenuDiffMax = IMP_DETECTOR_SUPFG_DIFF_MAX;
	pstRgExPara->stSuppressFgPara.s32PenuThreshold = IMP_DETECTOR_SUPFG_PENU_THRESHOLD;
	pstRgExPara->stSuppressFgPara.s32MagThreshold = IMP_DETECTOR_MAG_THRETH;
	pstRgExPara->stSuppressFgPara.s32MinMagDiffNum = IMP_DETECTOR_MIN_MAG_DIFF_NUM;
	pstRgExPara->stSuppressFgPara.s32MinMagDiffRatio = IMP_DETECTOR_MIN_MAG_DIFF_RATIO;

	pstRgExPara->stShadowPara.s32DetectShadow = IMP_DETECTOR_DETECT_SHADOW;
	pstRgExPara->stShadowPara.s32ThShapeHeightRatio = IMP_DETECTOR_TH_SHAPE_HEIGHT_RATIO;
	pstRgExPara->stShadowPara.s32ThShadowNumRatio = IMP_DETECTOR_TH_SHADOW_NUM_RATIO;
	pstRgExPara->stShadowPara.s32ThShadowHeightRatio = IMP_DETECTOR_TH_SHADOW_HEIGHT_RATIO;
	pstRgExPara->stShadowPara.s32ThShadowMinWidth = IMP_DETECTOR_TH_SHADOW_MIN_WIDTH;
	pstRgExPara->stShadowPara.s32ThShadowMinHeight = IMP_DETECTOR_TH_SHADOW_MIN_HEIGHT;
	pstRgExPara->stShadowPara.s32ThShadowMaxAreaPixel = IMP_DETECTOR_TH_SHADOW_MAX_AREA_PIXEL;
	pstRgExPara->stShadowPara.s32ThShadowMerge = IMP_DETECTOR_TH_SHADOW_MERGE;
	pstRgExPara->stShadowPara.s32ThShadowDel = IMP_DETECTOR_TH_SHADOW_DELETE;


	pstPara->s32FrmDura = IMP_PROCESSOR_FRM_DURA;

	pstRgExPara->stRmAbnmlPara.s32ThDrgMinSizeWh = pstRgExPara->stLabelPara.s32ThDrgMinSizeWh;
	pstRgExPara->stRmAbnmlPara.s32ThDrgMinSizePixel = pstRgExPara->stLabelPara.s32ThDrgMinSizePixelInit;
	pstRgExPara->stRmAbnmlPara.s32ThDrgMaxSizeRatio = pstRgExPara->stLabelPara.s32ThDrgMaxSizeRatio;

}
