#ifndef _IMP_TRACKER_PARA_H_
#define _IMP_TRACKER_PARA_H_



#ifdef __cplusplus
extern "C"
{
#endif



typedef struct
{
	IMP_S32 s32RemoveCalbFailDRegion;
	IMP_S32 s32MergeDRegion;
	IMP_S32 s32MergeTarget;
	IMP_S32 s32TimeCalVelocity;
	IMP_S32 s32FrmCntCalVelocity;
	IMP_S32 s32TimeStaticVelocityMax;
	IMP_S32 s32TimeStaticVelocityMin;
	IMP_S32 s32CsCollisionSplit;
	IMP_S32 s32CsSplitDefaultFeature;
	IMP_S32 s32CsStableCollision;
	IMP_S32 s32CsMultiTargetGroup;
	IMP_S32 s32CsMultiTargetSplit;
	IMP_S32 s32CsRefinedTargetDisplay;
	IMP_S32 s32CsCorrectCollisionTrajectory;
	IMP_S32 s32TrajectUseBottom;
	IMP_S32 s32WetStableTgt;
	IMP_S32 s32StaticMatchDisMax;
	IMP_S32 s32DisCrlSize;
	IMP_S32 s32MaxDistanceMotion;
	IMP_S32 s32MaxDistanceStatic;
	IMP_FLOAT fDistWeight;
	IMP_FLOAT fHistWeight;
	IMP_FLOAT fAreaWeight;
	IMP_FLOAT fPredictTargetDistWeight;
	IMP_FLOAT fPredictTargetHistWeight;
	IMP_FLOAT fPredictTargetAreaWeight;
	IMP_S32 s32DirectionDistanceLimit;

	IMP_FLOAT fAreaCoefMin;
	IMP_FLOAT fHistCoefMin;

	IMP_FLOAT fMinMatchCoefLow;
	IMP_FLOAT fMinMatchCoefHigh;

	IMP_S32 s32ProcessDisappearTarget;
	IMP_FLOAT fMaxDirectionDiff;
	IMP_S32 s32UsedTrajectLength;

	IMP_S32 s32DoMultiHyp;
	IMP_FLOAT fMinMatchCoefMultiHypTarget;
	IMP_FLOAT fMultiHypRangeRatio;

	IMP_S32 s32TrajectorySizeSmoothAnalysis;

	IMP_S32 s32SmoothTrajectory;
	IMP_S32 s32SmoothTimeWnd;


	IMP_S32 s32MotionMeanshift;
	IMP_S32 s32MmsIterMax;
	IMP_FLOAT fMmsDeltaB;
	IMP_S32 s32MmsFgWeight;
	IMP_S32 s32MmsStartLife;
	IMP_FLOAT fMmsStartCoef;
	IMP_FLOAT fMmsEndCoef;
	IMP_FLOAT fMmsUpdateAlpha;
	IMP_FLOAT fMmsSearchSizeZoom;
	IMP_S32 s32AnalysisMtrend;
	IMP_S32 s32StableOcclusionTime;
	IMP_S32 s32StableBorderWidth;
	IMP_S32	s32StableMinFrmCnt;
	IMP_S32 s32StableMinTime;
	IMP_S32 s32StableMaxTime;
	IMP_S32	s32StableValidRatio;
	IMP_S32 s32StableValidFrmCnt;
	IMP_S32 s32StableShapeRatio;
	IMP_S32 s32StableMinDistance;
	IMP_S32 s32StableMaxDistance;
	IMP_S32	s32StableTrendRatio;
	IMP_S32 s32StableTrendMotion;
	IMP_S32 s32StableDistRatio;
	IMP_S32 s32StableNon0Ratio;
	IMP_S32 s32StableMinFrmCntHighLevel;
	IMP_S32 s32StableMinTimeHighLevel;
	IMP_S32 s32StableValidFrmCntHighLevel;
	IMP_S32	s32StableDistRatioHighLevel;
	IMP_S32 s32StableMinDistHighLevel;
	IMP_FLOAT fMinMatchCoefHighLevel;
	IMP_S32 s32StableMinFrmCntMidLevel;
	IMP_S32 s32StableMinTimeMidLevel;
	IMP_S32 s32StableValidFrmCntMidLevel;
	IMP_S32 s32StableDistRatioMidLevel;
	IMP_S32 s32StableMinDistMidLevel;
	IMP_FLOAT fMinMatchCoefMidLevel;
	IMP_S32 s32StableMinFrmCntLowLevel;
	IMP_S32 s32StableMinTimeLowLevel;
	IMP_S32 s32StableValidFrmCntLowLevel;
	IMP_S32	s32StableDistRatioLowLevel;
	IMP_S32 s32StableMinDistLowLevel;
	IMP_FLOAT fMinMatchCoefLowLevel;
	IMP_FLOAT fAreaUpdateRate;
	IMP_FLOAT fSizeUpdateRate;
	IMP_FLOAT fHistUpdateRate;
	IMP_S32 s32ZoomEnable;
	IMP_S32 s32PtzTrackingTime;
	IMP_S32 s32PtzSleepTime;
	IMP_S32 s32PtzStopTime;
	IMP_S32 s32PtzZoomTimes;
	IMP_FLOAT fPtzZoomPeriodPerTime;
	IMP_FLOAT fPtzSpeedFurther;
	IMP_FLOAT fPtzSpeedNear;
	IMP_FLOAT fPtzSpeedNearest;
    IMP_S32 s32PtzZoomPeriodPerTime;
	IMP_S32 s32PtzCenterSize;
	IMP_S32 s32MaxTargetRectX;
	IMP_S32 s32MaxTargetRectY;
	IMP_S32 s32SampleLine;
	IMP_S32 s32TemplateCols;
	IMP_S32 s32TemplateRows;
	IMP_S32 s32TemplateOffsetX;
	IMP_S32 s32TemplateOffsetY;
	IMP_S32 s32MaxTrackRegionWidth;
	IMP_S32 s32MaxTrackRegionHeight;
	IMP_S32 s32MinTrackRegionWidth;
	IMP_S32 s32MinTrackRegionHeight;
	IMP_S32 s32StepX;
	IMP_S32 s32StepY;
	IMP_S32 s32MagThreshold;
	IMP_FLOAT fTplCrlCoefThHigh;
	IMP_FLOAT fTplCrlCoefThLow;
	IMP_FLOAT fTplCrlRatioThHigh;
	IMP_FLOAT fTplCrlRatioThLow;
	IMP_S32 s32ChgStableOcclusionTime;
	IMP_S32	s32ChgStableMinFrmCnt;
	IMP_S32	s32ChgStableMinTime;

	IMP_S32	s32FrmDura;
	RULE_S *pstRule;

} IpTrackerPara;



#ifdef __cplusplus
}
#endif


#endif
