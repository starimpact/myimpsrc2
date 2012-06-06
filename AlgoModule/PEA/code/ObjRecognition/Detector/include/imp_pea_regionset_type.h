
#ifndef _IMP_PEA_REGIONSET_TYPE_H_
#define _IMP_PEA_REGIONSET_TYPE_H_


typedef struct
{
	IMP_S32 s32ChangeOnly;
	IMP_S32 s32MotionIsLeft;
	IMP_S32 s32ThRgTmDifgRatio;
	IMP_S32 s32ThRgTmWidhRatio;
	IMP_S32 s32ThRgTsSizeMin;
	IMP_S32 s32ThRgTsSizeMax;
	IMP_S32 s32ThRgTsEdgeMin;
	IMP_S32 s32ThRgTsEdgeMax;
	IMP_S32 s32ThRgTsWidhRatio;
	IMP_S32 s32ThRgTsInBgSize;
	IMP_S32 s32ThRgTsInBgDiff;
	IMP_S32 s32ThRgTsStrengthRatioL;
	IMP_S32 s32ThRgTsShapeRatioL;
	IMP_S32 s32ThRgTsEdgeRatioL;
	IMP_S32 s32ThRgTsStrengthRatioR;
	IMP_S32 s32ThRgTsShapeRatioR;
	IMP_S32 s32ThRgTsEdgeRatioR;

} IpRGETypePara;


IMP_EXPORTS IMP_VOID ipLabelRegionSetTypeMotion( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_EXPORTS IMP_VOID ipLabelRegionSetTypeChange( IpRGETypePara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_EXPORTS IMP_VOID ipLabelRegionSetTypePost( IpRGETypePara *pstPara, IpTrackedTargetSet *tts );

#endif
