#include "imp_pea_tracker.h"
#include "imp_pea_system.h"


IMP_VOID ipParseTrackerParaData(IpTrackerPara *pstPara )
{
	pstPara->s32RemoveCalbFailDRegion = IMP_TRACKER_REMOVE_CALB_FAIL_DREGION;
	pstPara->s32MergeDRegion = IMP_TRACKER_MERGE_DREGION;
	pstPara->s32MergeTarget = IMP_TRACKER_MERGE_TTARGET;
	pstPara->s32TimeCalVelocity = IMP_TRACKER_TIME_CALCULATE_VELOCITY;
	pstPara->s32FrmCntCalVelocity = IMP_TRACKER_FRMCNT_CALCULATE_VELOCITY;
	pstPara->s32TimeStaticVelocityMax = IMP_TRACKER_TM_STATIC_VELO_MAX;
	pstPara->s32TimeStaticVelocityMin = IMP_TRACKER_TM_STATIC_TIME_MIN;

	pstPara->s32TrajectUseBottom = IMP_TRACKER_TRAJECT_USE_BOTTOM;
	pstPara->s32WetStableTgt = IMP_TRACKER_WET_STBALE_TGT;
	pstPara->fAreaCoefMin = IMP_TRACKER_AREA_COEF_MIN;
	pstPara->fHistCoefMin = IMP_TRACKER_HIST_COEF_MIN;
	pstPara->s32StaticMatchDisMax = IMP_TRACKER_STATIC_MATCH_DIS_MAX;
	pstPara->s32DisCrlSize = IMP_TRACKER_DIS_CRL_SIZE;
	pstPara->s32MaxDistanceMotion = IMP_TRACKER_MAX_DISTANCE_MOTION;
	pstPara->s32MaxDistanceStatic = IMP_TRACKER_MAX_DISTANCE_STATIC;
	pstPara->fDistWeight = IMP_TRACKER_DIST_WEIGHT;
	pstPara->fHistWeight = IMP_TRACKER_HISTO_WEIGHT;
	pstPara->fAreaWeight = IMP_TRACKER_AREA_WEIGHT;
	pstPara->fPredictTargetDistWeight = IMP_TRACKER_PREDICT_TARGET_DIST_WET;
	pstPara->fPredictTargetHistWeight = IMP_TRACKER_PREDICT_TARGET_HIST_WET;
	pstPara->fPredictTargetAreaWeight = IMP_TRACKER_PREDICT_TARGET_AREA_WET;
	pstPara->s32DirectionDistanceLimit = IMP_TRACKER_DIRECTION_DISTANCE_LIMIT;

	pstPara->fMinMatchCoefLow = IMP_TRACKER_MIN_MATCH_COEF_LOW;
	pstPara->fMinMatchCoefHigh = IMP_TRACKER_MIN_MATCH_COEF_HIGH;

	pstPara->s32ProcessDisappearTarget = IMP_TRACKER_PROCESS_DISAPPEAR_TARGET;
	pstPara->fMaxDirectionDiff = IMP_TRACKER_MAX_DIRECT_DIFF;
	pstPara->s32UsedTrajectLength = IMP_TRACKER_USED_TRAJECT_LEN;

	pstPara->s32DoMultiHyp = IMP_TRACKER_DO_MULTI_HYP;
	pstPara->fMinMatchCoefMultiHypTarget = IMP_TRACKER_MIN_MATCH_COEF_MULTI_HYP_TAG;
	pstPara->fMultiHypRangeRatio = IMP_TRACKER_MULTI_HYP_RANGE_RATIO;


	pstPara->s32CsCollisionSplit = IMP_TRACKER_COLLISION_SPLIT;
	pstPara->s32CsSplitDefaultFeature = IMP_TRACKER_SPLIT_DEFAULT_FEATURE;
	pstPara->s32CsStableCollision = IMP_TRACKER_STABLE_COLLISION;
	pstPara->s32CsMultiTargetSplit = IMP_TRACKER_MULTI_TGT_SPLIT;
	pstPara->s32CsRefinedTargetDisplay = IMP_TRACKER_REFINED_TGT_DISPLAY;
	pstPara->s32CsCorrectCollisionTrajectory = IMP_TRACKER_CORRECT_COLLISION_TRAJECTORY;


	pstPara->s32SmoothTrajectory = IMP_TRACKER_SMOOTH_TRAJECTORY;
	pstPara->s32SmoothTimeWnd = IMP_TRACKER_SMOOTH_TIME_WND;


	pstPara->s32MotionMeanshift = IMP_TRACKER_MOTION_MEANSHIFT;
	pstPara->s32MmsIterMax = IMP_TRACKER_MMS_ITER_MAX;
	pstPara->fMmsDeltaB = IMP_TRACKER_MMS_DELTA_B;
	pstPara->s32MmsFgWeight = IMP_TRACKER_MMS_FG_WEIGHT;
	pstPara->s32MmsStartLife = IMP_TRACKER_MMS_START_LIFE;
	pstPara->fMmsStartCoef = IMP_TRACKER_MMS_START_COEF;
	pstPara->fMmsEndCoef = IMP_TRACKER_MMS_END_COEF;
	pstPara->fMmsUpdateAlpha = IMP_TRACKER_MMS_UPDATE_ALPHA;
	pstPara->fMmsSearchSizeZoom = IMP_TRACKER_MMS_SEARCH_SIZE_ZOOM;


	pstPara->s32TrajectorySizeSmoothAnalysis = IMP_TRACKER_TRAJECTORY_SIZE_SMOOTH_ANALYSIS;


	pstPara->s32StableOcclusionTime = IMP_TRACKER_STABLE_OCCLUSION_TIME;
	pstPara->s32StableBorderWidth = IMP_TRACKER_STABLE_BORDER_WIDTH;
	pstPara->s32StableMinFrmCnt = IMP_TRACKER_STABLE_MIN_FRMCNT;
	pstPara->s32StableMinTime = IMP_TRACKER_STABLE_MIN_TIME;
	pstPara->s32StableMaxTime = IMP_TRACKER_STABLE_MAX_TIME;
	pstPara->s32StableValidRatio = IMP_TRACKER_STABLE_VALID_RATIO;
	pstPara->s32StableValidFrmCnt = IMP_TRACKER_STABLE_VALID_FRMCNT;
	pstPara->s32StableShapeRatio = IMP_TRACKER_STABLE_SHAPE_RATIO;
	pstPara->s32StableMinDistance = IMP_TRACKER_STABLE_MIN_DISTANCE;
	pstPara->s32StableMaxDistance = IMP_TRACKER_STABLE_MAX_DISTANCE;
	pstPara->s32StableTrendRatio = IMP_TRACKER_STABLE_TREND_RATIO;
	pstPara->s32StableTrendMotion = IMP_TRACKER_STABLE_TREND_MOTION;
	pstPara->s32StableDistRatio = IMP_TRACKER_STABLE_DIS_RATIO;
	pstPara->s32StableNon0Ratio = IMP_TRACKER_STABLE_NON0_RATIO;


	pstPara->s32StableMinFrmCntHighLevel = IMP_TRACKER_STABLE_MIN_FRMCNT_HIGH_LEVEL;
	pstPara->s32StableMinTimeHighLevel = IMP_TRACKER_STABLE_MIN_TIME_HIGH_LEVEL;
	pstPara->s32StableValidFrmCntHighLevel = IMP_TRACKER_STABLE_VALID_FRMCNT_HIGH_LEVEL;
	pstPara->s32StableMinDistHighLevel = IMP_TRACKER_STABLE_MIN_DISTANCE_HIGH_LEVEL;
	pstPara->s32StableDistRatioHighLevel = IMP_TRACKER_STABLE_DIS_RATIO_HIGH_LEVEL;
	pstPara->fMinMatchCoefHighLevel = IMP_TRACKER_MIN_MATCH_COEF_HIGH_LEVEL;


	pstPara->s32StableMinFrmCntMidLevel = IMP_TRACKER_STABLE_MIN_FRMCNT_MID_LEVEL;
	pstPara->s32StableMinTimeMidLevel = IMP_TRACKER_STABLE_MIN_TIME_MID_LEVEL;
	pstPara->s32StableValidFrmCntMidLevel = IMP_TRACKER_STABLE_VALID_FRMCNT_MID_LEVEL;
	pstPara->s32StableMinDistMidLevel = IMP_TRACKER_STABLE_MIN_DISTANCE_MID_LEVEL;
	pstPara->s32StableDistRatioMidLevel = IMP_TRACKER_STABLE_DIS_RATIO_MID_LEVEL;
	pstPara->fMinMatchCoefMidLevel = IMP_TRACKER_MIN_MATCH_COEF_MID_LEVEL;


	pstPara->s32StableMinFrmCntLowLevel = IMP_TRACKER_STABLE_MIN_FRMCNT_LOW_LEVEL;
	pstPara->s32StableMinTimeLowLevel = IMP_TRACKER_STABLE_MIN_TIME_LOW_LEVEL;
	pstPara->s32StableValidFrmCntLowLevel = IMP_TRACKER_STABLE_VALID_FRMCNT_LOW_LEVEL;
	pstPara->s32StableMinDistLowLevel = IMP_TRACKER_STABLE_MIN_DISTANCE_LOW_LEVEL;
	pstPara->s32StableDistRatioLowLevel = IMP_TRACKER_STABLE_DIS_RATIO_LOW_LEVEL;
	pstPara->fMinMatchCoefLowLevel = IMP_TRACKER_MIN_MATCH_COEF_LOW_LEVEL;


	pstPara->fAreaUpdateRate = IMP_TRACKER_AREA_UPDATE_RATE;
	pstPara->fSizeUpdateRate = IMP_TRACKER_SIZE_UPDATE_RATE;
	pstPara->fHistUpdateRate = IMP_TRACKER_HIST_UPDATE_RATE;


	pstPara->s32ChgStableOcclusionTime = IMP_TRACKER_CHG_STABLE_OCCLUSION_TIME;
	pstPara->s32ChgStableMinFrmCnt = IMP_TRACKER_CHG_STABLE_MIN_FRMCNT;
	pstPara->s32ChgStableMinTime = IMP_TRACKER_CHG_STABLE_MIN_TIME;

	pstPara->s32SampleLine = IMP_TRACKER_SAMPLE_LINE;
	pstPara->s32ZoomEnable = IMP_TRACKER_ZOOM_ENABLE;
	pstPara->s32PtzTrackingTime = IMP_TRACKER_PTZ_TRACKING_TIME;
	pstPara->s32PtzSleepTime = IMP_TRACKER_PTZ_SLEEP_TIME;
	pstPara->s32PtzStopTime = IMP_TRACKER_PTZ_STOP_TIME;
	pstPara->s32PtzZoomTimes = IMP_TRACKER_PTZ_ZOOM_TIMES;
	pstPara->fPtzZoomPeriodPerTime = IMP_TRACKER_PTZ_SPEED_FURTHEST;
	pstPara->fPtzSpeedFurther = IMP_TRACKER_PTZ_SPEED_FURTHER;
	pstPara->fPtzSpeedNear = IMP_TRACKER_PTZ_SPEED_NEAR;
	pstPara->fPtzSpeedNearest = IMP_TRACKER_PTZ_SPEED_NEAREST;
	pstPara->s32PtzZoomPeriodPerTime = IMP_TRACKER_PTZ_ZOOM_PERIOD_PERTIME;
	pstPara->s32PtzCenterSize = IMP_TRACKER_PTZ_CENTER_SIZE;
	pstPara->s32MaxTargetRectX = IMP_TRACKER_MAX_TARGET_RECT_X;
	pstPara->s32MaxTargetRectY = IMP_TRACKER_MAX_TARGET_RECT_Y;
	pstPara->s32TemplateCols = IMP_TRACKER_TEMPLATE_COLS;
	pstPara->s32TemplateRows = IMP_TRACKER_TEMPLATE_ROWS;
	pstPara->s32MaxTrackRegionWidth = IMP_TRACKER_MAX_TRACKING_REGION_WIDTH;
	pstPara->s32MaxTrackRegionHeight = IMP_TRACKER_MAX_TRACKING_REGION_HEIGHT;
	pstPara->s32MinTrackRegionWidth = IMP_TRACKER_MIN_TRACKING_REGION_WIDTH;
	pstPara->s32MinTrackRegionHeight = IMP_TRACKER_MIN_TRACKING_REGION_HEIGHT;
	pstPara->s32TemplateOffsetX = IMP_TRACKER_TEMPLATE_OFFSET_X;
	pstPara->s32TemplateOffsetY = IMP_TRACKER_TEMPLATE_OFFSET_Y;
	pstPara->s32StepX = IMP_TRACKER_STEP_X;
	pstPara->s32StepY = IMP_TRACKER_STEP_Y;
	pstPara->s32MagThreshold = IMP_TRACKER_MAG_THRESHOLD;
	pstPara->fTplCrlCoefThHigh = IMP_TRACKER_TPL_CRL_COEF_THD_HIGH;
	pstPara->fTplCrlCoefThLow = IMP_TRACKER_TPL_CRL_COEF_THD_LOW;
	pstPara->fTplCrlRatioThHigh = IMP_TRACKER_TPL_CRL_RATIO_THD_HIGH;
	pstPara->fTplCrlRatioThHigh = IMP_TRACKER_TPL_CRL_RATIO_THD_LOW;
	pstPara->s32FrmDura  = IMP_TRACKER_FRM_DURA;
}


IMP_VOID ipForceInnerParaToTrackerPara( INNER_PARA_S *pstInnerPara, IpTrackerPara *pstPara )
{
	GLOBAL_PARA_S *pstGlobalPara = &pstInnerPara->stGlobal;
	RULE_S *pstRule;
	RULE_ZONES_S *pstZones;
	RULE_ZONE_S *pstZone;
	USR_ADV_PARA_S *pstUsrAdvPara;
	IMP_U32 u32FuncUseMotion;
	IMP_U32 u32Mode;
	IMP_S32 i;


	pstPara->pstRule = pstInnerPara->pRule;
	pstRule = pstPara->pstRule;
	pstUsrAdvPara = (USR_ADV_PARA_S*)pstRule->u8AdvParaData;
	pstZones = &pstRule->stZones;

	pstPara->s32FrmDura = pstGlobalPara->s32TimeUnit;


	u32FuncUseMotion = IMP_FUNC_PERIMETER
		| IMP_FUNC_TRIPWIRE
		| IMP_FUNC_MTRIPWIRE
		| IMP_FUNC_LOITER;

	for( i = 0; i < IMP_MAX_NUM_RULE_ZONE; i++ )
	{
		pstZone = &pstZones->astZone[i];
		if( pstZone->u32Valid && pstZone->u32Enable )
			u32Mode |= pstZone->u32Mode;
	}
	pstPara->s32AnalysisMtrend = ( (u32Mode&u32FuncUseMotion) != 0 );

    if (pstUsrAdvPara->stParaUrp.s32AdvanceParaEnable)
    {
		if (pstUsrAdvPara->stParaUrp.s32TargetOutputSensitivityLevel == IMP_HIGH_LEVEL_SENSITIVITY)
		{
			pstPara->s32StableMinFrmCnt = pstPara->s32StableMinFrmCntHighLevel;
			pstPara->s32StableMinTime = pstPara->s32StableMinTimeHighLevel;
			pstPara->s32StableValidFrmCnt = pstPara->s32StableValidFrmCntHighLevel;
			pstPara->s32StableMinDistance = pstPara->s32StableMinDistHighLevel;
			pstPara->s32StableDistRatio = pstPara->s32StableDistRatioHighLevel;
			pstPara->fMinMatchCoefHigh = pstPara->fMinMatchCoefHighLevel;
		}
		else if (pstUsrAdvPara->stParaUrp.s32TargetOutputSensitivityLevel == IMP_MIDDLE_LEVEL_SENSITIVITY)
		{
			pstPara->s32StableMinFrmCnt = pstPara->s32StableMinFrmCntMidLevel;
			pstPara->s32StableMinTime = pstPara->s32StableMinTimeMidLevel;
			pstPara->s32StableValidFrmCnt = pstPara->s32StableValidFrmCntMidLevel;
			pstPara->s32StableMinDistance = pstPara->s32StableMinDistMidLevel;
			pstPara->s32StableDistRatio = pstPara->s32StableDistRatioMidLevel;
		}
		else if (pstUsrAdvPara->stParaUrp.s32TargetOutputSensitivityLevel == IMP_LOW_LEVEL_SENSITIVITY)
		{
			pstPara->s32StableMinFrmCnt = pstPara->s32StableMinFrmCntLowLevel;
			pstPara->s32StableMinTime = pstPara->s32StableMinTimeLowLevel;
			pstPara->s32StableValidFrmCnt = pstPara->s32StableValidFrmCntLowLevel;
			pstPara->s32StableMinDistance = pstPara->s32StableMinDistLowLevel;
			pstPara->s32StableDistRatio = pstPara->s32StableDistRatioLowLevel;
		}
    }
}
