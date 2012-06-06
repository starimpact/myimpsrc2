#include "imp_pea_detector.h"

#ifndef _IMP_PEA_BGM_STABILIZER_H_
#define _IMP_PEA_BGM_STABILIZER_H_


typedef struct impPEA_STABILIZE_PARA_S
{
	IMP_S32 s32DoStabilize;
	IMP_RECT_S astRegions[4];
	IMP_S32 s32SearchScope;
	IMP_S32 s32MatchNumTh;
	IMP_S32 s32MatchDistTh;
	IMP_S32 s32DiffRatioMatchHighTh;
	IMP_S32 s32DiffRatioMatchLowTh;
	IMP_S32 s32DiffMaxMatchHighTh;
	IMP_S32 s32DiffMinMatchLowTh;
	IMP_S32 s32ShakingUseFgEdge;
	IMP_S32 s32ShakingEdgeLikehoodTh;
	IMP_S32 s32ShakingHistoryLenTh;
	IMP_S32 s32ShakingValidLenTh;
	IMP_S32 s32ConstMvTimeTh;
} PEA_STABILIZE_PARA_S;


typedef struct impPEA_STABILIZE_MATCH_DATA_S
{
	IMP_RECT_S stRegion;
	IMP_POINT_S stMv;
	IMP_S32 s32Diff;
	IMP_S32 s32BgDiff;
	IMP_S32 s32BgEdge;
	IMP_S32 s32DiffMatchHighTh;
	IMP_S32 s32DiffMatchLowTh;
	IMP_S32 s32Matched;
	IMP_S32 s32Moved;
	IMP_S32 s32SuccessFlag;
} PEA_STABILIZE_MATCH_DATA_S;


#define IMP_PEA_MATCH_REGION_NUM	4


typedef struct impPEA_BGMODEL_STABILIZER_S
{
	GA_HARDWARE_RS_S *pstHwRes;
	PEA_RESULT_S *pstPeaResult;
	PEA_BG_MODEL_DATA_S *pstBgModelData;
	PEA_STABILIZE_PARA_S stStabPara;
	IMP_S32 s32ValidRegionCnt;
	IMP_S32 s32SuccessFlag;
	IMP_POINT_S stMvCurrent;
	IMP_POINT_S stMvPre;
	IMP_S32 s32MatchNum;
	IMP_S32 s32MatchDataUpdated;
	PEA_STABILIZE_MATCH_DATA_S astMatchData[IMP_PEA_MATCH_REGION_NUM];
	IMP_S32 s32ShakingFlag;
	IMP_S32 s32ShakingCurFlag;
	GA_CYCLE_INTEGER_ARRAY_S stShakingHistory;
	IMP_U32 u32FrmTimeCur;
	IMP_U32 u32CntMvStartTime;
} PEA_BGMODEL_STABILIZER_S;

IMP_VOID ipCreateBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule, PEA_BG_MODEL_DATA_S *pstBgModelData, PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );
IMP_VOID ipReleaseBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule );
IMP_VOID ipConfigBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule, PEA_STABILIZE_PARA_S *pstPara );
IMP_S32 ipProcessBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule );
IMP_VOID ipPostProcessBgModelStabilizer( PEA_BGMODEL_STABILIZER_S *pModule );

#endif
