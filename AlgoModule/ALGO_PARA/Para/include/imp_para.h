#ifndef _IMP_PARA_H_
#define _IMP_PARA_H_


#include "imp_algo_type.h"
#include "imp_ga_common.h"
#include "imp_algo_urp_param.h"


#ifdef __cplusplus
IMP_EXPORTS "C"
{
#endif

#define SUPPORT_TRIPWIRE
#define SUPPORT_PERIMETER



#define IMP_RULE_enacode 16
#define IMP_RULE_name "¹æÔò"
#define IMP_RULE_USRADV_IVPSYS 0
#define IMP_RULE_USE_USRADVPARA 0
#define IMP_RULE_OUTPUT_PREDICTED 0
#define IMP_RULE_OUTPUT_EVENT 0
#define IMP_RULE_PROC_INTERVAL 80

typedef enum impRULE_ZONE_VALID_E
{
	IMP_RULE_ZONE0_VALID = 1,
	IMP_RULE_ZONE1_VALID = 0,
	IMP_RULE_ZONE2_VALID = 0,
	IMP_RULE_ZONE3_VALID = 0,
	IMP_RULE_ZONE4_VALID = 0,
	IMP_RULE_ZONE5_VALID = 0,
	IMP_RULE_ZONE6_VALID = 0,
	IMP_RULE_ZONE7_VALID = 0
}RULE_ZONE_VALID_E;

#define IMP_RULE_ZONE0_VALID 1

#define IMP_RULE_name1 "¹æÔò1"
#define IMP_RULE_enable 1
#define IMP_RULE_level 3
#define IMP_RULE_ANALYST_TYPE 0
#define IMP_RULE_ANALYST_ABANDUM 1
#define IMP_RULE_ANALYST_OBJSTOLEN 1
#define IMP_RULE_ANALYST_NOPARKING 1
#define IMP_RULE_ANALYST_PERIMETER 1
#define IMP_RULE_ANALYST_TRIPWIRE 0
#define IMP_RULE_ANALYST_MTRIPWIRE 0
#define IMP_RULE_ANALYST_ABNORMALVEL 0
#define IMP_RULE_ANALYST_LOITER 0
#define IMP_RULE_ANALYST_PARAMETER 0
#define IMP_RULE_SCENE_TYPE 0
#define IMP_RULE_CAM_TYPE 0
#define IMP_RULE_WORK_MODE 0
#define IMP_RULE_ABANDUM 0
#define IMP_RULE_ABANDUM_SIZE_MIN 100
#define IMP_RULE_ABANDUM_SIZE_MAX 10000
#define IMP_RULE_ABANDUM_TIME_MIN 5000
#define IMP_RULE_OBJSTOLEN 0
#define IMP_RULE_OBJSTOLEN_SIZE_MIN 100
#define IMP_RULE_OBJSTOLEN_SIZE_MAX 10000
#define IMP_RULE_OBJSTOLEN_TIME_MIN 5000
#define IMP_RULE_NOPARKING 0
#define IMP_RULE_NOPARKING_TIME_MIN 5000
#define IMP_RULE_PERIMETER 0
#define IMP_RULE_PERIMETER_TYPE_CONSTRAIN 1
#define IMP_RULE_PERIMETER_TYPE_HUMAN 1
#define IMP_RULE_PERIMETER_TYPE_VEHICLE 1
#define IMP_RULE_PERIMETER_MODE 0
#define IMP_RULE_PERIMETER_DIST_MIN 0
#define IMP_RULE_PERIMETER_TIME_MIN 0
#define IMP_RULE_PERIMETER_DIRECTION_LIMIT 0
#define IMP_RULE_PERIMETER_FORBIDDEN_DIRECTION 0
#define IMP_RULE_TRIPWIRE 0
#define IMP_RULE_TRIPWIRE_TYPE_CONSTRAIN 1
#define IMP_RULE_TRIPWIRE_TYPE_HUMAN 1
#define IMP_RULE_TRIPWIRE_TYPE_VEHICLE 1
#define IMP_RULE_TRIPWIRE_VALID 1
#define IMP_RULE_TRIPWIRE_BIDIRECTION 0
#define IMP_RULE_TRIPWIRE_ABNMLANGLE 225
#define IMP_RULE_TRIPWIRE_LINE_PTS_X 105
#define IMP_RULE_TRIPWIRE_LINE_PTS_Y 232
#define IMP_RULE_TRIPWIRE_LINE_PTE_X 232
#define IMP_RULE_TRIPWIRE_LINE_PTE_Y 103
#define IMP_RULE_MTRIPWIRE 0
#define IMP_RULE_MTRIPWIRE_TYPE_CONSTRAIN 1
#define IMP_RULE_MTRIPWIRE_TYPE_HUMAN 1
#define IMP_RULE_MTRIPWIRE_TYPE_VEHICLE 1
#define IMP_RULE_MTRIPWIRE_VALID 1
#define IMP_RULE_MTRIPWIRE_TIME_MIN 1000
#define IMP_RULE_MTRIPWIRE_TIME_MAX 5000
#define IMP_RULE_MTRIPWIRE_ABNMLANGLE1 225
#define IMP_RULE_MTRIPWIRE_ABNMLANGLE2 225
#define IMP_RULE_MTRIPWIRE_LINE1_PTS_X 105
#define IMP_RULE_MTRIPWIRE_LINE1_PTS_Y 262
#define IMP_RULE_MTRIPWIRE_LINE1_PTE_X 232
#define IMP_RULE_MTRIPWIRE_LINE1_PTE_Y 133
#define IMP_RULE_MTRIPWIRE_LINE2_PTS_X 105
#define IMP_RULE_MTRIPWIRE_LINE2_PTS_Y 232
#define IMP_RULE_MTRIPWIRE_LINE2_PTE_X 232
#define IMP_RULE_MTRIPWIRE_LINE2_PTE_Y 103
#define IMP_RULE_ABNORMALVEL 0
#define IMP_RULE_ABNMLVEL_TYPE_CONSTRAIN 1
#define IMP_RULE_ABNMLVEL_TYPE_HUMAN 1
#define IMP_RULE_ABNMLVEL_TYPE_VEHICLE 1
#define IMP_RULE_ABNMLVEL_INTERVAL_FORBIDDEN 0
#define IMP_RULE_ABNMLVEL_INTERVAL_MIN 500
#define IMP_RULE_ABNMLVEL_INTERVAL_MAX -1
#define IMP_RULE_LOITER 0
#define IMP_RULE_LOITER_TYPE_CONSTRAIN 1
#define IMP_RULE_LOITER_TIME_MIN 10000
#define IMP_RULE_FDEPTH_ENABLE 1
#define IMP_RULE_FDEPTH_LINE_NUM 3
#define IMP_RULE_FDEPTH_LINE1_PTS_X 142
#define IMP_RULE_FDEPTH_LINE1_PTS_Y 150
#define IMP_RULE_FDEPTH_LINE1_PTE_X 142
#define IMP_RULE_FDEPTH_LINE1_PTE_Y 173
#define IMP_RULE_FDEPTH_LINE1_REF_LEN 170
#define IMP_RULE_FDEPTH_LINE2_PTS_X 36
#define IMP_RULE_FDEPTH_LINE2_PTS_Y 203
#define IMP_RULE_FDEPTH_LINE2_PTE_X 36
#define IMP_RULE_FDEPTH_LINE2_PTE_Y 241
#define IMP_RULE_FDEPTH_LINE2_REF_LEN 170
#define IMP_RULE_FDEPTH_LINE3_PTS_X 154
#define IMP_RULE_FDEPTH_LINE3_PTS_Y 138
#define IMP_RULE_FDEPTH_LINE3_PTE_X 154
#define IMP_RULE_FDEPTH_LINE3_PTE_Y 155
#define IMP_RULE_FDEPTH_LINE3_REF_LEN 170


#define IMP_MAX_NUM_FDEPTH_LINE 3
#define IMP_MAX_NUM_RULE_ZONE	8

typedef struct impFDZ_LINE_S
{
	LINE_S stRefLine;
	IMP_S32 s32RefLen;
} FDZ_LINE_S;


typedef struct impFDZ_LINES_S
{
	IMP_U32    u32NumUsed;
	FDZ_LINE_S stLines[IMP_MAX_NUM_FDEPTH_LINE];
} FDZ_LINES_S;


typedef struct impFDEPTH_MEASURE_S
{
	FDZ_LINES_S stFdzLines;
} FDEPTH_MEASURE_S;


typedef struct impFDEPTH_COEF_S
{
	IMP_FLOAT fCX;
	IMP_FLOAT fCY;
	IMP_FLOAT fCF;
} FDEPTH_COEF_S;


typedef struct impFDEPTH_PARAMS_S
{
	FDEPTH_COEF_S stCoef;
} FDEPTH_PARAMS_S;


typedef struct impFDEPTH_S
{
	IMP_U32 u32Enable;
	FDEPTH_MEASURE_S stMeasure;
	FDEPTH_PARAMS_S stParams;
	GRAY_IMAGE_S stImgResolution;
} FDEPTH_S;


typedef struct impCLBR_IALG_S
{
	IMP_S32 (*IMP_CalcFDepth)( FDEPTH_S *pstFDepth );
	IMP_S32 (*IMP_CalcPointResolution)( FDEPTH_S *pstFDepth,  IMP_S32 s32X, IMP_S32 s32Y, IMP_U8 *ps32RX, IMP_U8 *ps32RY );
	IMP_S32 (*IMP_GetLineHeight)( FDEPTH_S *pstFDepth,  LINE_S *pstLine, IMP_S32 *ps32Length );
} CLBR_IALG_S;


typedef enum impCLBR_STATUS_E
{
	IMP_CLBR_FAILED = 0,
	IMP_CLBR_SUCCESS = 1
}CLBR_STATUS_E;


#define IMP_FDEPTH_MIN_RESOLUTION	1

#define IMP_FDEPTH_MAX_RESOLUTION	254

#define IMP_FDEPTH_MAX_RESOLUTION2	500


IMP_EXPORTS IMP_S32 IMP_FDEPTH_GetPointResolution( FDEPTH_S *pstFDepth, IMP_S32 s32X, IMP_S32 s32Y, IMP_DOUBLE *pdResX, IMP_DOUBLE *pdResY );
//IMP_EXPORTS IMP_S32 IMP_FDEPTH_CalcDRegionResolution( FDEPTH_S *pstFDepth, PEA_DETECTED_REGION_S *pstDrg, IMP_DOUBLE *pdResolution );
IMP_EXPORTS IMP_S32 IMP_FDEPTH_Calculate( FDEPTH_S *pstFDepth, IMP_U32 u32Flag );
IMP_EXPORTS IMP_S32 IMP_FDEPTH_GetLineHeight( FDEPTH_S *pstFDepth, LINE_S *pstLine , IMP_S32 *pLength );
IMP_EXPORTS IMP_S32 IMP_FDEPTH_GetRectDimension( FDEPTH_S *pstFDepth, IMP_POINT_S *pstPos, IMP_DOUBLE dArea,IMP_DOUBLE dRatio, IMP_S32 *ps32Width, IMP_S32 *ps32Height );

#define IMP_MAX_NUM_RULE_ZONE	8


typedef enum impPARA_RULE_E
{
	IMP_PARA_RULE_SET = 0,
	IMP_PARA_RULE_SET_ZONE,
	IMP_PARA_RULE_MAX
}PARA_RULE_E;


typedef enum impPARA_TYPE_E
{
	IMP_PARA_TYPE_GLOBAL  = 0x01,
	IMP_PARA_TYPE_RULE    = 0x02,
	IMP_PARA_TYPE_ADVANCE = 0x04
} PARA_TYPE_E;

IMP_S32 IMP_GetMemSizeRuleAdvPara(IMP_S32 s32BufNum, IMP_S32 s32BufLen);

IMP_EXPORTS IMP_VOID IMP_RULE_AdvParaAlloc( ADVANCE_PARA_S *pstPara, IMP_S32 s32BufNum, IMP_S32 s32BufLen, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_VOID IMP_RULE_AdvParaFree( ADVANCE_PARA_S *pstPara, IMP_S32 s32BufNum, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_VOID IMP_RULE_AdvParaClear( ADVANCE_PARA_S *pstPara, IMP_S32 s32Num );
IMP_EXPORTS IMP_VOID IMP_RULE_AdvParaClone( ADVANCE_PARA_S *stSrc, ADVANCE_PARA_S *stDst, IMP_S32 s32BufNum, IMP_S32 s32BufLen );


#define IMP_RULE_VERSION  IMP_MAKE_VERSION( 'R', 'I', 0x01, 0 )


//场景类型
typedef enum impSCENE_TYPE_E
{
	IMP_INDOOR = 0,
	IMP_OUTDOOR
} SCENE_TYPE_E;

//相机类型
typedef enum impCAMERA_TYPE_E
{
	IMP_COMMON_CAMERA = 0,
	IMP_PINHOLE_CAMERA
} CAMERA_TYPE_E;



#define IMP_MAX_POINT_NUM 4
#define IMP_MAX_BOUNDARY_POINT_NUM 8

typedef struct impPOLYGON_REGION_S
{
	IMP_S32 s32Valid;
	IMP_S32 s32PointNum;
	IMP_POINT_S astPoint[IMP_MAX_BOUNDARY_POINT_NUM];
}POLYGON_REGION_S;

//OSC Rule --begin
#define IMP_MAX_OSC_NUM 4
typedef struct impOSC_OBJ_LMT_SPECL_REGIONS_S
{
	IMP_S32 s32Valid;
	IMP_S8 as8Name[IMP_NAME_BUF_SIZE];
	POLYGON_REGION_S stOscRg;
	POLYGON_REGION_S stSpclRgA;
	POLYGON_REGION_S stSpclRgB;
	POLYGON_REGION_S stSpclRgC;
} OSC_OBJ_LMT_SPECL_REGIONS_S;


typedef struct
{
	IMP_S32 s32TimeMin;
	IMP_S32 s32SizeMin;
	IMP_S32 s32SizeMax;
} OSC_LMT_PARA_S;


typedef struct impRULE_PARA_OSC_S
{
	IMP_S32 s32SceneType;
	IMP_S32 s32CameraType;
	OSC_OBJ_LMT_SPECL_REGIONS_S astSpclRgs[IMP_MAX_OSC_NUM];
	OSC_LMT_PARA_S stOscPara;
} RULE_PARA_OSC_S;
//OSC Rule --end

typedef struct impRULE_PARA_ABANDUM_S
{
	IMP_S32 s32TimeMin;
	IMP_S32 s32SizeMin;
	IMP_S32 s32SizeMax;
} RULE_PARA_ABANDUM_S;


typedef struct impRULE_PARA_OBJ_STOLEN_S
{
	IMP_S32 s32TimeMin;
	IMP_S32 s32SizeMin;
	IMP_S32 s32SizeMax;
} RULE_PARA_OBJ_STOLEN_S;


typedef struct impRULE_PARA_NO_PARKING_S
{
	IMP_S32 s32TimeMin;
} RULE_PARA_NO_PARKING_S;


typedef enum impPERIMETER_MODE_E
{
	IMP_PMODE_INTRUSION = 0,
	IMP_PMODE_ENTER,
	IMP_PMODE_EXIT
} PERIMETER_MODE_E;


#define MAX_PERILMT_BOUNDARY_POINT_NUM 8


typedef struct impPERIMETE_LMT_BOUNDARY_S
{
	IMP_S32 s32BdPtNum;
	IMP_POINT_S astBdPts[MAX_PERILMT_BOUNDARY_POINT_NUM];
}PERIMETE_LMT_BOUNDARY_S;


typedef struct impPERILMT_LMT_PARA_S
{
	IMP_S32 s32DistMin;
	IMP_S32 s32TimeMin;
	IMP_S32	s32DirectionLmt;
	IMP_S32 s32ForbiddenDirection;
	PERIMETE_LMT_BOUNDARY_S stPeriBound;
} PERILMT_LMT_PARA_S;


typedef struct impRULE_PARA_PERIMETER_S
{
	IMP_S32 s32TypeConstrain;
	IMP_S32 s32TypeHuman;
	IMP_S32 s32TypeVehicle;
	IMP_S32 s32Mode;
	PERILMT_LMT_PARA_S stPara;
} RULE_PARA_PERIMETER_S;


#define IMP_MAX_TRIPWIRE_CNT 8


typedef struct impTRIPWIRE_S
{
	IMP_S32 s32Valid;
	IMP_S32 s32Bidirection;
	IMP_S32 s32AbnmlAngle;
	LINE_S	stLine;
} TRIPWIRE_S;


typedef struct impTRIPWIRE_LMT_PARA_S
{
	IMP_S32 s32DistMin;
	IMP_S32 s32TimeMin;
} TRIPWIRE_LMT_PARA_S;

typedef struct impRULE_PARA_TRIPWIRE_S
{
	IMP_S32 s32TypeConstrain;
	IMP_S32 s32TypeHuman;
	IMP_S32 s32TypeVehicle;
	TRIPWIRE_S astLines[IMP_MAX_TRIPWIRE_CNT];
	TRIPWIRE_LMT_PARA_S stLmtPara;
} RULE_PARA_TRIPWIRE_S;


#define IMP_MAX_MTRIPWIRE_CNT 4


typedef struct impMTRIPWIRE_S
{
	IMP_S32 s32Valid;
	IMP_S32 s32TimeMin;
	IMP_S32 s32TimeMax;
	IMP_S32 as32AbnmlAngle[2];
	LINE_S	astLine[2];
} MTRIPWIRE_S;


typedef struct impMTRIPWIRE_LMT_PARA_S
{
	IMP_S32 s32DistMin;
	IMP_S32 s32TimeMin;
} MTRIPWIRE_LMT_PARA_S;


typedef struct impRULE_PARA_MTRIPWIRE_S
{
	IMP_S32 s32TypeConstrain;
	IMP_S32 s32TypeHuman;
	IMP_S32 s32TypeVehicle;
	MTRIPWIRE_S astLines[IMP_MAX_MTRIPWIRE_CNT];
	MTRIPWIRE_LMT_PARA_S stLmtPara;
} RULE_PARA_MTRIPWIRE_S;


typedef struct impFORBIDDEN_INTERVAL_S
{
	IMP_U32 u32Forbidden;
	IMP_S32 s32Min;
	IMP_S32 s32Max;
} FORBIDDEN_INTERVAL_S;


typedef struct impRULE_PARA_ABNMLVEL_S
{
	IMP_S32 s32TypeConstrain;
	IMP_S32 s32TypeHuman;
	IMP_S32 s32TypeVehicle;
	FORBIDDEN_INTERVAL_S stInterval;
} RULE_PARA_ABNMLVEL_S;


typedef struct impRULE_PARA_LOITER_S
{
	IMP_S32 s32TypeConstrain;
	IMP_S32 s32TimeMin;
} RULE_PARA_LOITER_S;


typedef struct impRULE_PARA_S
{
	RULE_PARA_OSC_S stOsc;
	RULE_PARA_ABANDUM_S	    stAbandum;
	RULE_PARA_OBJ_STOLEN_S  stObjStolen;
	RULE_PARA_NO_PARKING_S  stNoParking;
	RULE_PARA_PERIMETER_S	stPerimeter;
	RULE_PARA_TRIPWIRE_S	stTripwire;
	RULE_PARA_MTRIPWIRE_S	stMTripwire;
	RULE_PARA_ABNMLVEL_S    stAbnmlVel;
	RULE_PARA_LOITER_S	    stLoiter;
} RULE_PARA_S;


typedef struct impRULE_ZONE_S
{
	IMP_S8 as8Name[IMP_NAME_BUF_SIZE];
	IMP_U32 u32Valid;
	IMP_U32 u32Enable;
	IMP_U32 u32Level;
	IMP_U32 u32Mode;
	RULE_PARA_S stPara;
} RULE_ZONE_S;


typedef struct
{
	RULE_ZONE_S astZone[IMP_MAX_NUM_RULE_ZONE];
	GRAY_IMAGE_S stImage;
} RULE_ZONES_S;

typedef struct impIES_PARA_S
{
	IMP_S32 s32DoIstab;
} IES_PARA_S;

typedef struct impSCENE_PARA_S
{
	IMP_S32 s32SceneLmt;
	IMP_S32 s32SceneMode;
} SCENE_PARA_S;

typedef struct impTARGET_PARA_S
{
	IMP_S32 s32TgtLmt;
	IMP_S32 s32Sensitivity;
	IMP_S32 s32SPixSizeMin;
	IMP_S32 s32SPixSizeMax;
	IMP_S32 s32PixVeloMin;
	IMP_S32 s32PixVeloMax;
	IMP_S32 s32TgtRealInfoLmt;
	IMP_S32 s32RealSizeMin;
	IMP_S32 s32RealSizeMax;
	IMP_S32 s32RealVeloMin;
	IMP_S32 s32RealVeloMax;
	IMP_S32 s32MaxStaticTime;
} TARGET_PARA_S;

typedef struct impENVIRONMENT_TGT_PARA_S
{
	IES_PARA_S    stIesPara;
	SCENE_PARA_S  stScenePara;
	TARGET_PARA_S stTgtPara;
} ENVIRONMENT_TGT_PARA_S;

#define IMP_ADV_DATA_LEN		8*1024
#define IMP_CALLBACK_DATA_LEN	100*1024


typedef struct impRULE_S
{
	IMP_U32 u32Enacode;
	IMP_S8 as8Name[IMP_NAME_BUF_SIZE];
	RULE_ZONES_S stZones;
	FDEPTH_S stFDepth;
	ENVIRONMENT_TGT_PARA_S stEvrmentTgtPara;
	IMP_U8 u8AdvParaData[IMP_ADV_DATA_LEN];

} RULE_S;

IMP_S32 IMP_GetMemSizeRule(IMP_S32 s32ImgW, IMP_S32 s32ImgH);

IMP_EXPORTS RULE_S* IMP_RULE_Alloc( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_VOID IMP_RULE_Free( RULE_S *pstVana, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_VOID IMP_RULE_Clone( RULE_S *pstSrc, RULE_S *pstDst );
IMP_EXPORTS IMP_VOID IMP_RULE_Clear( RULE_S *pstVana );
IMP_EXPORTS IMP_S32 IMP_RULE_Calculate( RULE_S *pstVana, IMP_U32 u32Flag );
IMP_EXPORTS IMP_VOID IMP_ZONE_Alloc( RULE_ZONE_S *pstZone, IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_VOID IMP_ZONE_Free( RULE_ZONE_S *pstZone, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_VOID IMP_ZONE_Clear( RULE_ZONE_S *pstZone );
IMP_EXPORTS IMP_VOID IMP_ZONE_Clone( RULE_ZONE_S *pstSrc, RULE_ZONE_S *pstDst );


typedef struct impUSR_ADV_PARA_SYS_S
{
	IMP_S32 s32OutputPredicted;
	IMP_S32 s32OutputEvents;
	IMP_S32 s32ProcInterval;
} USR_ADV_PARA_SYS_S;

typedef struct impUSR_ADV_PARA_BG_MODEL_S
{
	IMP_S32 s32ThMin;
	IMP_S32 s32ThMax;
	IMP_S32 s32ThUdpm;
	IMP_S32 s32ThNoiseRatio;


	IMP_S32 s32ThUp2dwRatio;


	IMP_U8 u8UpdateSpeedMax;
    IMP_U8 u8LearnRate;


	IMP_S32 s32TAdd;
}USR_ADV_PARA_BG_MODEL_S;

typedef struct impUSR_ADV_VIDEO_ABNORMAL_PARA_S
{
	IMP_S32 s32DetectVideoException;
	IMP_S32 s32FrameCountTh;
	IMP_S32 s32ThDrgMaxSizeH;
	IMP_S32 s32ThDrgMaxSizeW;
	IMP_U8  u8VideoLostLumiTh;
	IMP_U8  u8OcclusionTh;
	IMP_U8  u8FireceLightTh;

	IMP_U16 u16VideoLostOneBlockPixelNumTh;
    IMP_U16 u16VideoLostBlockNumTh;
	IMP_U16 u16OcclusionOneBlockPixelNumTh;
	IMP_U16 u16OcclusionBlockNumTh;
	IMP_U16 u16FireceLightOneBlockPixelNumTh;
	IMP_U16 u16FireceLightBlockNumTh;
	IMP_U16 u16LightMutationOneBlockPixelNumTh;
	IMP_U16 u16LightMutationBlockNumTh;
	IMP_U16 u16FieldScrambleOneBlockPixelNumTh;
	IMP_U16 u16FieldScrambleBlockNumTh;

	IMP_U8 u8AbnormalJudgeTh;

} USR_ADV_VIDEO_ABNORMAL_PARA_S;

typedef struct impUSR_ADV_PARA_SIMPLE_BG_MODEL
{
	IMP_S32 s32SbThetaThld;
	IMP_S32 s32GradThetaThld;
	IMP_S32 s32FgRatioThld;
	IMP_S32 s32FrmFgRatioThld;
	IMP_S32 s32FgRatioHisLenThld;
	IMP_S32 s32HisLenRatioThld;
	IMP_S32 s32SbInitTime;
	IMP_S32 s32PersonEnterFgRationHisLen;
	IMP_S32 s32PersonEnterRatioThreth;
	IMP_S32 s32PersonEnterContinueFrm;
	IMP_S32 s32personLeaveTimeSBgModelUpdate;
}USR_ADV_PARA_SIMPLE_BG_MODEL;

typedef struct impUSR_ADV_PARA_DETECTOR_S
{
	IMP_S32 s32DoSmooth;
	IMP_S32 s32SupFgLight;
	IMP_S32 s32SupFgPenumbra;
	IMP_S32 s32DetectShadow;
    USR_ADV_PARA_SIMPLE_BG_MODEL stSBgModelPara;
	USR_ADV_PARA_BG_MODEL_S  stBgModelPara;
	USR_ADV_VIDEO_ABNORMAL_PARA_S stVideoAbnormalPara;
} USR_ADV_PARA_DETECTOR_S;

typedef struct impUSR_ADV_PARA_TRACKER_S
{
	IMP_S32 s32TrajectUseBottom;
	IMP_S32 s32MaxDistanceMotion;
	IMP_S32 s32MaxDistanceStatic;
	IMP_DOUBLE dAreaCoefMin;
	IMP_DOUBLE dHistCoefMin;
	IMP_DOUBLE dMinMatchCoefLow;
	IMP_DOUBLE dMinMatchCoefHigh;
	IMP_S32 s32ProcessDisappearTarget;
	IMP_S32 s32CollisionSplit;
	IMP_S32 s32StableCollision;
	IMP_S32 s32MultiTargetSplit;
	IMP_S32 s32RefinedTargetDisplay;
	IMP_S32 s32StableMaxTime;
} USR_ADV_PARA_TRACKER_S;

typedef struct impUSR_ADV_PARA_CLASSIFIER_S
{
	IMP_S32 s32UseBorderConstrain;
	IMP_S32 s32ClassifierAnimal;
	IMP_S8  s8SvmFuncType;
} USR_ADV_PARA_CLASSIFIER_S;

//used for osc
typedef struct impUSR_ADV_OSC_PARA_ANALYST_S
{
	IMP_S32 s32LeaveLmt;
	IMP_S32 s32UseBorderConstrain;
	IMP_S32 s32BorderWidth;
	IMP_S32 s32DoSpecialRegionProcess;
	IMP_S32 s32SpclMinTime;
	IMP_S32 s32TrajectLen;
	IMP_S32 s32TrajectRatio;
}USR_ADV_OSC_PARA_ANALYST_S;

typedef struct impUSR_ADV_PERIMETER_PARA_ANALYST_S
{
	IMP_S32 s32UseMTrend;
	IMP_S32 s32PerimeterJudgeMethod;
	IMP_S32 s32PerimeterAreaRatio;
}USR_ADV_PERIMETER_PARA_ANALYST_S;

typedef struct impUSR_ADV_TRIPWIRE_PARA_ANALYST_S
{
	IMP_S32 s32TripwireUseMTrend;
	IMP_S32 s32TripwireJudgeMethod;
	IMP_S32 s32TripwireAreaRatio;
}USR_ADV_TRIPWIRE_PARA_ANALYST_S;

typedef struct impUSR_ADV_MTRIPWIRE_PARA_ANALYST_S
{
	IMP_S32 s32MTripwireUseMTrend;
	IMP_S32 s32MTripwireJudgeMethod;
	IMP_S32 s32MTripwireAreaRatio;
}USR_ADV_MTRIPWIRE_PARA_ANALYST_S;

typedef struct impUSR_ADV_AVNMAL_VEL_PARA_ANALYST_S
{
	IMP_S32 s32AbnmlvelUseMTrend;
}USR_ADV_AVNMAL_VEL_PARA_ANALYST_S;

typedef struct impUSR_ADV_LOITER_PARA_ANALYST_S
{
	IMP_S32 s32LoiterUseMTrend;
}USR_ADV_LOITER_PARA_ANALYST_S;

typedef struct impUSR_ADV_PARA_ANALYST_S
{
	USR_ADV_OSC_PARA_ANALYST_S stOscPara;
	USR_ADV_PERIMETER_PARA_ANALYST_S stPerimeterPara;
	USR_ADV_TRIPWIRE_PARA_ANALYST_S stTripwirePara;
	USR_ADV_MTRIPWIRE_PARA_ANALYST_S stMTripwirePara;
	USR_ADV_AVNMAL_VEL_PARA_ANALYST_S stAbnmlVelPara;
	USR_ADV_LOITER_PARA_ANALYST_S stLoiterPara;
} USR_ADV_PARA_ANALYST_S;

typedef struct impUSR_ADV_PARA_S
{
	IMP_S32 s32UseUsrAdvPara;
	USR_ADV_PARA_SYS_S stParaSys;
	USR_ADV_PARA_DETECTOR_S stParaDetector;
	USR_ADV_PARA_TRACKER_S stParaTracker;
	USR_ADV_PARA_CLASSIFIER_S stParaClassifier;
	USR_ADV_PARA_ANALYST_S stParaAnalyst;
	URP_ADVANCE_PARA_S  stParaUrp;
} USR_ADV_PARA_S;


IMP_EXPORTS IMP_S32 IMP_WriteVZImage( GRAY_IMAGE_S *pstImage, IMP_U8 *pu8Data );
IMP_EXPORTS IMP_S32 IMP_ReadVZImage( IMP_U8 *pu8Data, GRAY_IMAGE_S *pstImage );

#define IMP_PARSE_PARFILE_START \
	i=0; \
	while( i<s32Length ) \
{ \
	str1 = ps8Data+i; \
	while( i<s32Length && ps8Data[i++] != '=' ); \
	len1 = ps8Data+i-str1-1; \
	str2 = ps8Data+i; \
	while( i<s32Length && ps8Data[i++] != '\n' ); \
	if(0) \
{ \
}

#define IMP_PARSE_PARFILE_END \
	}

#define PARSE_PARFILE_ITEM_INT( str, par ) \
	if( strncmp( str1, str, len1 ) == 0 ) \
{ \
	par = atoi( str2 ); \
}
#define IMP_PARSE_PARFILE_ITEM_REAL( str, par ) \
	if( strncmp( str1, str, len1 ) == 0 ) \
{ \
	par = (IMP_DOUBLE)atof( str2 ); \
}

#define IMP_PARSE_PARFILE_ITEM_LINE( str, line ) \
		if( strncmp( str1, str, len1 ) == 0 ) \
		{ \
			IMP_S32 xs, ys, xe, ye; \
			sscanf( str2, "((%d,%d),(%d,%d))", &xs, &ys, &xe, &ye ); \
			line.stPs.s16X = xs; line.stPs.s16Y = ys;	line.stPe.s16X = xe; line.stPe.s16Y = ye; \
		}

#define IMP_PARSE_PARFILE_ITEM_FINTVAL( str, intval ) \
		if( strncmp( str1, str, len1 ) == 0 ) \
		{ \
			IMP_S32 forbidden, minimum, maximum; \
			sscanf( str2, "(%d,%d,%d)", &forbidden, &minimum, &maximum ); \
			intval.u32Forbidden = forbidden; intval.s32Min = minimum; intval.s32Max = maximum; \
		}

#define IMP_PARSE_PARFILE_ITEM_RECT( str, rect ) \
		if( strncmp( str1, str, len1 ) == 0 ) \
		{ \
			IMP_S32 lb_x, lb_y, ru_x, ru_y; \
			sscanf( str2, "(%d,%d,%d,%d)", &lb_x, &lb_y, &ru_x, &ru_y ); \
			rect.s16X1 = lb_x; rect.s16Y1 = lb_y; rect.s16X2 = ru_x; rect.s16Y2 = ru_y; \
		}

#define IMP_PARSE_PARFILE_ITEM_POINT( str, point ) \
		if( strncmp( str1, str, len1 ) == 0 ) \
		{ \
			IMP_S32 x, y; \
			sscanf( str2, "(%d,%d)", &x, &y ); \
			point.s16X = x;point.s16Y = y; \
		}

#define IMP_PACKET_PARFILE_START \
	{ \
		str1 = ps8Data; \
		str2 = ps8Data;

#define IMP_PACKET_PARFILE_END \
		s32Len = str2 - str1; \
	}

#define IMP_PACKET_PARFILE_ITEM_INT( str, par ) \
		{ \
			str2 += sprintf( str2, "%s=%d\n", str, par ); \
		}
#define IMP_PACKET_PARFILE_ITEM_REAL( str, par ) \
		{ \
			str2 += sprintf( str2, "%s=%.2f\n", str, par ); \
		}

#define IMP_PACKET_PARFILE_ITEM_LINE( str, line ) \
		{ \
			IMP_S32 xs, ys, xe, ye; \
			xs = line.stPs.s16X; ys = line.stPs.s16Y;	xe = line.stPe.s16X; ye = line.stPe.s16Y; \
			str2 += sprintf( str2, "%s=((%d,%d),(%d,%d))\n", str, xs, ys, xe, ye ); \
		}

#define IMP_PACKET_PARFILE_ITEM_FINTVAL( str, intval ) \
		{ \
			IMP_S32 forbidden, minimum, maximum; \
			forbidden = intval.u32Forbidden; minimum = intval.s32Min; maximum = intval.s32Max; \
			str2 += sprintf( str2, "%s=(%d,%d,%d)\n", str, forbidden, minimum, maximum ); \
		}

#define IMP_PACKET_PARFILE_ITEM_RECT( str, rect ) \
		{ \
			str2 += sprintf( str2, "%s=(%d,%d,%d,%d)\n", str, rect.s16X1, rect.s16Y1, rect.s16X2, rect.s16Y2 ); \
		}

#define IMP_PACKET_PARFILE_ITEM_POINT( str, point ) \
		{ \
			str2 += sprintf( str2, "%s=(%d,%d)\n", str, point.s16X, point.s16Y ); \
		}

#define IMP_FIND_LINE_START(data) {data = strstr( data, "\n" ); data += 1;}

typedef IMP_S32 (*IMP_FUNC_PKGALGP)( IMP_VOID *pAlgp, IMP_U8 *pu8Data );
typedef IMP_S32 (*IMP_FUNC_UPKALGP)( IMP_U8 *pu8Data, IMP_VOID *pAlgp );
typedef IMP_S32 (*IMP_FUNC_PKGALGPV)( IMP_VOID *pAlgp, IMP_S8 *ps8ParDat, IMP_S32 *ps32ParLen );
typedef IMP_S32 (*IMP_FUNC_UPKALGPV)( IMP_S8 *ps8ParDat, IMP_S32 u32ParLen, IMP_VOID *pAlgp );
typedef IMP_S32 (*IMP_FUNC_PKGALGPZ)( IMP_VOID *pAlgp, IMP_S8 *ps8ParDat, IMP_S32 *ps32ParLen );
typedef IMP_S32 (*IMP_FUNC_UPKALGPZ)( IMP_S8 *Ps8ParDat, IMP_S32 s32ParLen, IMP_VOID *pAlgp );
typedef IMP_S32 (*IMP_FUNC_DFTALGPZ)( IMP_VOID *pAlgp );

IMP_EXPORTS IMP_S32 IMP_RULE_CheckSceneValid( DATA_BUF_S *pstBuf, IMP_U32 u32Version, IMP_U32 *pu32Enacode );
IMP_EXPORTS IMP_S32 IMP_RULE_PackAlgp( IMP_VOID *pAlgp, DATA_BUF_S *pstBuf, IMP_FUNC_PKGALGP func_pkg, IMP_U32 u32Version );
IMP_EXPORTS IMP_S32 IMP_RULE_UnpackAlgp( DATA_BUF_S *pBuf, IMP_VOID *pAlgp, IMP_FUNC_UPKALGP func_upk, IMP_U32 u32Version );
IMP_EXPORTS IMP_S32 IMP_RULE_PackAlgpBuffer( IMP_VOID *pAlgp, DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ, IMP_FUNC_PKGALGPV func_pkgv, IMP_FUNC_PKGALGPZ func_pkgz );
IMP_EXPORTS IMP_S32 IMP_RULE_UnpackAlgpBuffer( DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ, IMP_VOID *pAlgp, IMP_FUNC_UPKALGPV func_upkv, IMP_FUNC_UPKALGPZ func_upkz );
IMP_EXPORTS IMP_S32 IMP_RULE_PackAlgpFile( IMP_VOID *pAlgp, IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pMMgr, IMP_FUNC_PKGALGPV func_pkgv, IMP_FUNC_PKGALGPZ func_pkgz );
IMP_EXPORTS IMP_S32 IMP_RULE_UnpackAlgpFile( IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pAlgp, IMP_VOID *pMMgr, IMP_FUNC_UPKALGPV func_upkv, IMP_FUNC_UPKALGPZ func_upkz );
IMP_EXPORTS IMP_S32 IMP_RULE_DefaultAlgp( IMP_S8 *ps8StrV, IMP_VOID *pAlgp, IMP_VOID *pMMgr, IMP_FUNC_UPKALGPV func_upkv, IMP_FUNC_DFTALGPZ func_dftz );

typedef struct  impSCENE_FILES_S
{
	IMP_S8 *ps8StrVana;
	IMP_S8 *ps8FileNameVana;
	IMP_S8 *ps8FileNameZone;
	IMP_S8 *ps8FileNameBin;
	IMP_S8 s8IFlag;
	IMP_S8 s8OFlag;
} SCENE_FILES_S;

typedef struct impSCENE_PACK_OBJ_S
{
	IMP_S32 s32ImgW;
	IMP_S32	s32ImgH;
	IMP_VOID *pMMgr;
	SCENE_FILES_S stScnFiles;

	IMP_VOID* (*IMP_Alloc)( IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMMgr );
	IMP_VOID (*IMP_Free)( IMP_VOID *pRule, IMP_VOID *pMMgr );
	IMP_VOID (*IMP_Clone)( IMP_VOID *pSrc, IMP_VOID *pDst );
	IMP_VOID (*IMP_Clear)( IMP_VOID *pRule );
	IMP_S32 (*IMP_Calculate)( IMP_VOID *pRule, IMP_U32 u32Flag );

	IMP_S32 (*IMP_PackScene)( IMP_VOID *pRule, DATA_BUF_S *pstBuf );
	IMP_S32 (*IMP_UnpackScene)( DATA_BUF_S *pstBuf, IMP_VOID *pRule );
	IMP_S32 (*IMP_PackSceneBuffer)( IMP_VOID *pRule, DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ );
	IMP_S32 (*IMP_UnpackSceneBuffer)( DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ, IMP_VOID *pRule );
	IMP_S32 (*IMP_PackSceneFile)( IMP_VOID *pRule, IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pMMgr );
	IMP_S32 (*IMP_UnpackSceneFile)( IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pRule, IMP_VOID *pMMgr );
	IMP_S32 (*IMP_DefaultScene)( IMP_S8 *ps8StrV, IMP_VOID *pRule, IMP_VOID *pMMgr );
} SCENE_PACK_OBJ_S;

IMP_EXPORTS IMP_VOID IMP_RULE_MakeSceneFile( SCENE_PACK_OBJ_S *pstObj );
IMP_EXPORTS IMP_S32 IMP_RULE_CheckRuleValid( DATA_BUF_S *pstBuf, IMP_U32 *pu32Enacode );

IMP_EXPORTS IMP_S32 IMP_RULE_PackRule( RULE_S *pstRule, DATA_BUF_S *pstBuf );
IMP_EXPORTS IMP_S32 IMP_RULE_UnpackRule( DATA_BUF_S *pstBuf, RULE_S *pstRule );

IMP_EXPORTS IMP_S32 IMP_RULE_PackRuleBuffer( RULE_S *pstRule, DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ );
IMP_EXPORTS IMP_S32 IMP_RULE_UnpackRuleBuffer( DATA_BUF_S *pstBufV, DATA_BUF_S *pstBufZ, RULE_S *pstVana );

IMP_EXPORTS IMP_S32 IMP_RULE_PackRuleFile( RULE_S *pstRule, IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, IMP_VOID *pMMgr );
IMP_EXPORTS IMP_S32 IMP_RULE_UnpackRuleFile( IMP_S8 *ps8FilV, IMP_S8 *ps8FilZ, RULE_S *pstRule, IMP_VOID *pMMgr );

IMP_EXPORTS IMP_S32 IMP_RULE_DefaultRule( IMP_S8 *ps8StrV, RULE_S *pstVana, IMP_VOID *pMMgr );

IMP_EXPORTS IMP_S8 *ps8ParRule;

typedef enum impRULE_VPARAADVBUFNUM_E
{
	IMP_VPARAADVBUFNUM_PROCESSOR = 0,
	IMP_VPARAADVBUFNUM_DETECTOR,
	IMP_VPARAADVBUFNUM_TRACKER,
	IMP_VPARAADVBUFNUM_CLASSIFIER,
	IMP_VPARAADVBUFNUM_ANALYST,
	IMP_VPARAADVBUFNUM_PTZ,
	IMP_VPARAADVBUFNUM_MAX
}RULE_VPARAADVBUFNUM_E;

IMP_EXPORTS IMP_VOID IMP_RULE_InitParaAdvance( ADVANCE_PARA_S *pstAdvPara, STR_ARRAY_PARA_S *pstStrArray, IMP_VOID *pMemMgr );
IMP_EXPORTS IMP_VOID IMP_RULE_InitParaRule( RULE_PARA_DATA_S *pstAdvPara, STR_ARRAY_PARA_S *pstStrArray,
									IMP_S32 s32ImgWidth, IMP_S32 s32ImgHeight, IMP_VOID *pMemMgr );


#ifdef __cplusplus
}
#endif

#endif
