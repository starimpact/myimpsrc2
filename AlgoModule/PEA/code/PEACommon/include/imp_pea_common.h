#ifndef _IMP_PEA_COMMON_H_
#define _IMP_PEA_COMMON_H_



#include "imp_para.h"
#include "imp_algo_type.h"
#include "imp_ga_evt_manager.h"



#ifdef __cplusplus
extern "C"
{
#endif

#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

#define INSIDE 0
#define OUTSIDE 1

#define IMP_DBG_USE_OPENCV 1
#if IMP_DBG_USE_OPENCV

#include "cv.h"
#include "highgui.h"

//function for debug start
void ipShowGrayImage(int width, int height, char *pbyImage, char *pbyName);
void ipShowBinImage(int width, int height, char *pbyImage, char *pbyName);
void ipShowBinImageBit(int width, int height, char *pbyImage, char ubyBit, char *pbyName);
void ipShowBinImageLargerVal(int width, int height, char *pbyImage, char ubyVal, char *pbyName);
void ipShowBinImageLargerVal_L7(int width, int height, char *pbyImage, char ubyVal, char *pbyName);
//function for debug end
#endif


typedef struct
{
   double x,y;
} Point;

//Noise Estimate by 5X5 area, better and quicker version
IMP_S32 ipNoiseEstimateByBox_25(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgTmp, IMP_S32 *pdwNoiseValue);

//Noise Estimate by neighbor frames
IMP_S32 ipNoiseEstimateByBox(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgTmp, IMP_S32 *pdwNoiseValue);

//Noise Estimate by neighbor images
IMP_S32 ipNoiseEstimateByNeighbor(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgPre, IMP_S32 *pdwNoiseValue);

//remove shadow based on texture similarity
IMP_S32 ipRemoveShadow(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgBkg, GRAY_IMAGE_S *pimgBin, IMP_S32 dwNoiseValue);

//remove shadow based on NCC and standard deviation
IMP_S32 ipRemoveShadowByNCC(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgBkg, GRAY_IMAGE_S *pimgBin, IMP_S32 dwNoiseValue);

//check if the point inside the Polygon
int InsidePolygon(Point *polygon,int N,Point p);

//check if the point inside the Polygon
int impInsidePolygon(IMP_POINT_S *polygon, int N, IMP_POINT_S p);


typedef enum impFUNC_E
{
	IMP_FUNC_UNKNOWN			= 0x00000000
}FUNC_E;

typedef enum impDRGN_STATUS_E
{
	IMP_DRGN_STATUS_RGN_MOTION	= 0x0001,
	IMP_DRGN_STATUS_RGN_STATIC_L = 0x0002,
	IMP_DRGN_STATUS_RGN_STATIC_R = 0x0003,
	IMP_DRGN_STATUS_RGN_LIGHT	=0x0004,
	IMP_DRGN_STATUS_TGT_MOTION	= 0x0100,
	IMP_DRGN_STATUS_TGT_STATIC	= 0x0200
} DRGN_STATUS_E;

#define IMP_SET_RGN_RMOTION(x)			( x = ( ( x & 0xF0 ) | IMP_DRGN_STATUS_RGN_MOTION ) )
#define IMP_IS_RGN_RMOTION(x)			( ( x & 0x0F ) == IMP_DRGN_STATUS_RGN_MOTION )
#define IMP_SET_RGN_RSTATIC_L(x)		    ( x = ( ( x & 0xF0 ) | IMP_DRGN_STATUS_RGN_STATIC_L ) )
#define IMP_IS_RGN_RSTATIC_L(x)			( ( x & 0x0F ) == IMP_DRGN_STATUS_RGN_STATIC_L )
#define IMP_SET_RGN_RSTATIC_R(x)		    ( x = ( ( x & 0xF0 ) | IMP_DRGN_STATUS_RGN_STATIC_R ) )
#define IMP_IS_RGN_RSTATIC_R(x)			( ( x & 0x0F ) == IMP_DRGN_STATUS_RGN_STATIC_R )
#define IMP_SET_RGN_RLIGHT(x)			( x = ( ( x & 0xF0 ) | IMP_DRGN_STATUS_RGN_LIGHT ) )
#define IMP_IS_RGN_RLIGHT(x)			    ( ( x & 0x0F ) == IMP_DRGN_STATUS_RGN_LIGHT )
#define IMP_SET_RGN_TMOTION(x)			( x |= IMP_DRGN_STATUS_TGT_MOTION )
#define IMP_IS_RGN_TMOTION(x)			( x &  IMP_DRGN_STATUS_TGT_MOTION )
#define IMP_SET_RGN_TSTATIC(x)			( x |= IMP_DRGN_STATUS_TGT_STATIC )
#define IMP_IS_RGN_TSTATIC(x)			( x &  IMP_DRGN_STATUS_TGT_STATIC )

#define IMP_DRG_SET_UNUSED(x)			( x = 0x00 )
#define IMP_DRG_IS_USED(x)				( x )
#define IMP_DRG_SET_LABELING(x)			( x = ( ( x & 0xFC ) | 0x01 ) )
#define IMP_DRG_IS_LABELING(x)			( ( x & 0x03 ) == 0x01 )
#define IMP_DRG_SET_LABELED(x)			( x = ( ( x & 0xFC ) | 0x02 ) )
#define IMP_DRG_IS_LABELED(x)			( ( x & 0x03 ) == 0x02 )
#define IMP_DRG_SET_COMPLETE(x)			( x = ( ( x & 0xFC ) | 0x03 ) )
#define IMP_DRG_IS_COMPLETE(x)			( ( x & 0x03 ) == 0x03 )
#define IMP_DRG_SET_SHDRMVD(x)			( x |= 0x04 )
#define IMP_DRG_IS_SHDRMVD(x)			( x | 0xFF )
#define IMP_DRG_SET_MOTION(x)			( x = ( ( x & 0x8F ) | 0x10 ) )
#define IMP_DRG_IS_MOTION(x)			    ( ( x & 0x70 ) == 0x10 )
#define IMP_DRG_SET_STATIC_L(x)			( x = ( ( x & 0x8F ) | 0x20 ) )
#define IMP_DRG_IS_STATIC_L(x)			( ( x & 0x70 ) == 0x20 )
#define IMP_DRG_SET_STATIC_R(x)			( x = ( ( x & 0x8F ) | 0x30 ) )
#define IMP_DRG_IS_STATIC_R(x)			( ( x & 0x70 ) == 0x30 )
#define IMP_DRG_SET_DIFF(x)				( x = ( ( x & 0x8F ) | 0x40 ) )
#define IMP_DRG_IS_DIFF(x)				( ( x & 0x70 ) == 0x40 )
#define IMP_DRG_SET_LIGHT(x)			    ( x = ( ( x & 0x8F ) | 0x50 ) )
#define IMP_DRG_IS_LIGHT(x)				( ( x & 0x70 ) == 0x50 )


typedef struct impPEA_SPECIAL_DATA_S
{
	IMP_VOID *pDataDetector;
	IMP_U32 u32DetectorSz;
	IMP_VOID *pDataTracker;
	IMP_U32 u32TrackerSz;
	IMP_VOID *pDataClassifier;
	IMP_U32 u32ClassifierSz;
	IMP_VOID *pDataAnalyst;
	IMP_U32 u32AnalystSz;
} PEA_SPECIAL_DATA_S;


typedef struct impPEA_DETECTED_REGION_S
{
	IMP_U8 u8Used;
	IMP_U8 u8Sign;
	IMP_U16 u16Status;
	IMP_RECT_S	stRect;
	IMP_S32 s32AreaPixel;
	IMP_S32 s32AreaActual;
	PEA_SPECIAL_DATA_S stPrivateData;
} PEA_DETECTED_REGION_S;


typedef struct impPEA_DETECTED_REGIONSET_S
{
	IMP_S32 s32UsedTotal;
	IMP_S32 s32FreeFirst;
	PEA_DETECTED_REGION_S astDrg[IMP_MAX_TGT_CNT];
	PEA_SPECIAL_DATA_S stPrivateData;
	GRAY_IMAGE_S *pstImgInGray;
	GRAY_IMAGE_S *pstImgInEdge;
	GRAY_IMAGE_S *pstImgInEdgeMag;
	GRAY_IMAGE_S *pstImgInEdgeMagPre;
	GRAY_IMAGE16_S *pstImgInEdgeGrad;
	GRAY_IMAGE_S *pstImgBgGray;
	GRAY_IMAGE_S *pstImgStBgGray;
	GRAY_IMAGE_S *pstImgBgDiff;
	GRAY_IMAGE_S *pstImgBgEdge;
	GRAY_IMAGE_S *pstImgBgEdgeMag;
	GRAY_IMAGE16_S *pstImgBgEdgeGrad;
	GRAY_IMAGE_S *pstImgSbgGray;
	GRAY_IMAGE_S *pstImgBgInDiffMag;
	GRAY_IMAGE_S *pstImgFgOrg;
	GRAY_IMAGE_S *pstImgFgRgn;
	GRAY_IMAGE16_S *pstImgFgLife;

	GRAY_IMAGE_S *pstImgContour;
	GRAY_IMAGE_S *pstImgFrmDiff;

	GRAY_IMAGE_S *pstImgFgMorph;
	GRAY_IMAGE_S *pstImgMediate;
} PEA_DETECTED_REGIONSET_S;



#define IMP_ITERATE_RGS_START \
	for( i=1; i<IMP_MAX_TGT_CNT-1; i++ ) \
{ \
	pstDrg = &pstRgs->astDrg[i]; \
	if( IMP_DRG_IS_USED(pstDrg->u8Used) ) \
{

#define IMP_ITERATE_RGS_END \
} \
} \
	for( i=1, s32Cnt=0; i<IMP_MAX_TGT_CNT-1; i++ ) \
	s32Cnt += pstRgs->astDrg[i].u8Used ? 1 : 0; \
	IMP_ASSERT( s32Cnt==pstRgs->s32UsedTotal );


typedef enum
{
	IMP_TPOS_STATUS_UNUSED		= 0x0000,
	IMP_TPOS_STATUS_MEASURED	= 0x0001,
	IMP_TPOS_STATUS_PREDICTED	= 0x0002,
	IMP_TPOS_STATUS_COLLISION	= 0x0100,
	IMP_TPOS_STATUS_ABNORMAL	= 0x0200,
	IMP_TPOS_STATUS_MEANSHIFT	= 0x1000,
} IpTPosStatus;

#define IMP_SET_TPOS_MEASURED(x)	( x = ( ( x & 0xF0 ) | IMP_TPOS_STATUS_MEASURED ) )
#define IMP_IS_TPOS_MEASURED(x)		( ( x & 0x0F ) == IMP_TPOS_STATUS_MEASURED )
#define IMP_SET_TPOS_PREDICTED(x)	( x = ( ( x & 0xF0 ) | IMP_TPOS_STATUS_PREDICTED ) )
#define IMP_IS_TPOS_PREDICTED(x)	( ( x & 0x0F ) == IMP_TPOS_STATUS_PREDICTED )
#define IMP_SET_TPOS_COLLISION(x)	( x = ( ( x & 0xF0FF ) | IMP_TPOS_STATUS_COLLISION ) )
#define IMP_IS_TPOS_COLLISION(x)	( ( x & 0x0F00 ) == IMP_TPOS_STATUS_COLLISION )
#define IMP_SET_TPOS_ABNORMAL(x)	( x = ( ( x & 0xF0FF ) | IMP_TPOS_STATUS_ABNORMAL ) )
#define IMP_IS_TPOS_ABNORMAL(x)		( ( x & 0x0F00 ) == IMP_TPOS_STATUS_ABNORMAL )
#define IMP_SET_TPOS_KEYTRAJECT(x)	( x = ( ( x & 0x0F ) | 0x10 ) )
#define IMP_IS_TPOS_KEYTRAJECT(x)	( ( x & 0xF0 ) == 0x10 )
#define IMP_SET_TPOS_MEANSHIFT(x)	( x |= IMP_TPOS_STATUS_MEANSHIFT )
#define IMP_IS_TPOS_MEANSHIFT(x)	( x &  IMP_TPOS_STATUS_MEANSHIFT )

typedef enum impTTGT_STATUS_E
{
	IMP_TTGT_STATUS_DELETE		= 0x0000,
	IMP_TTGT_STATUS_INIT		= 0x0001,
	IMP_TTGT_STATUS_STABLE		= 0x0002,
	IMP_TTGT_STATUS_MEASURED	= 0x0010,
	IMP_TTGT_STATUS_PREDICTED	= 0x0020,
	IMP_TTGT_STATUS_MTREND		= 0x0100,
	IMP_TTGT_STATUS_STATIC		= 0x0200,
	IMP_TTGT_STATUS_MOTION		= 0x0400,
	IMP_TTGT_STATUS_MEANSHIFT	= 0x0800,
	IMP_TTGT_STATUS_COLLISION	= 0x1000,
	IMP_TTGT_STATUS_DISAPPEAR	= 0x2000,
	IMP_TTGT_STATUS_ABNORMAL	= 0x4000,
	IMP_TTGT_STATUS_INAIR		= 0x8000,
	IMP_TTGT_STATUS_PSTATOBJ	= 0x10000
} TTGT_STATUS_E;
#define IMP_GET_TGT_STATUS(x)		( x & 0x000F )
#define IMP_SET_TGT_DELETE(x)		( x = ( ( x & 0xFFF0 ) | IMP_TTGT_STATUS_DELETE ) )
#define IMP_IS_TGT_DELETE(x)		( ( x & 0x000F ) == IMP_TTGT_STATUS_DELETE )
#define IMP_SET_TGT_INIT(x)			( x = IMP_TTGT_STATUS_INIT )
#define IMP_IS_TGT_INIT(x)			( ( x & 0x000F ) == IMP_TTGT_STATUS_INIT )
#define IMP_SET_TGT_STABLE(x)		( x = ( ( x & 0xFFF0 ) | IMP_TTGT_STATUS_STABLE ) )
#define IMP_IS_TGT_STABLE(x)		( ( x & 0x000F ) == IMP_TTGT_STATUS_STABLE )
#define IMP_SET_TGT_MEASURED(x)		( x = ( ( x & 0xFF0F ) | IMP_TTGT_STATUS_MEASURED ) )
#define IMP_IS_TGT_MEASURED(x)		( ( x & 0x00F0 ) == IMP_TTGT_STATUS_MEASURED )
#define IMP_SET_TGT_PREDICTED(x)	( x = ( ( x & 0xFF0F ) | IMP_TTGT_STATUS_PREDICTED ) )
#define IMP_IS_TGT_PREDICTED(x)		( ( x & 0x00F0 ) == IMP_TTGT_STATUS_PREDICTED )
#define IMP_SET_TGT_MTREND(x)		( x |= IMP_TTGT_STATUS_MTREND )
#define IMP_IS_TGT_MTREND(x)		( x &  IMP_TTGT_STATUS_MTREND )




#define IMP_SET_TGT_STATIC(x)		( x |= IMP_TTGT_STATUS_STATIC )
#define IMP_IS_TGT_STATIC(x)		( x &  IMP_TTGT_STATUS_STATIC )
#define IMP_RESET_TGT_STATIC(x)		( x &= ~IMP_TTGT_STATUS_STATIC )
#define IMP_SET_TGT_MOTION(x)		( x |= IMP_TTGT_STATUS_MOTION )
#define IMP_IS_TGT_MOTION(x)		( x &  IMP_TTGT_STATUS_MOTION )
#define IMP_RESET_TGT_MOTION(x)		( x &= ~IMP_TTGT_STATUS_MOTION )
#define IMP_SET_TGT_MEANSHIFT(x)	( x |= IMP_TTGT_STATUS_MEANSHIFT )
#define IMP_IS_TGT_MEANSHIFT(x)		( x &  IMP_TTGT_STATUS_MEANSHIFT )
#define IMP_RESET_TGT_MEANSHIFT(x)	( x &= ~IMP_TTGT_STATUS_MEANSHIFT )
#define IMP_SET_TGT_COLLISION(x)	( x |= IMP_TTGT_STATUS_COLLISION )
#define IMP_IS_TGT_COLLISION(x)		( x &  IMP_TTGT_STATUS_COLLISION )
#define IMP_RESET_TGT_COLLISION(x)	( x &= ~IMP_TTGT_STATUS_COLLISION )
#define IMP_SET_TGT_DISAPPEAR(x)	( x |= IMP_TTGT_STATUS_DISAPPEAR )
#define IMP_IS_TGT_DISAPPEAR(x)		( x &  IMP_TTGT_STATUS_DISAPPEAR )
#define IMP_RESET_TGT_DISAPPEAR(x)	( x &= ~IMP_TTGT_STATUS_DISAPPEAR )
#define IMP_SET_TGT_ABNORMAL(x)		( x |= IMP_TTGT_STATUS_ABNORMAL )
#define IMP_IS_TGT_ABNORMAL(x)		( x &  IMP_TTGT_STATUS_ABNORMAL )
#define IMP_RESET_TGT_ABNORMAL(x)	( x &= ~IMP_TTGT_STATUS_ABNORMAL )
#define IMP_SET_TGT_INAIR(x)		( x |= IMP_TTGT_STATUS_INAIR )
#define IMP_IS_TGT_INAIR(x)			( x &  IMP_TTGT_STATUS_INAIR )
#define IMP_RESET_TGT_INAIR(x)		( x &= ~IMP_TTGT_STATUS_INAIR )
#define IMP_SET_TGT_PSTATOBJ(x)		( x |= IMP_TTGT_STATUS_PSTATOBJ )
#define IMP_IS_TGT_PSTATOBJ(x)		( x &  IMP_TTGT_STATUS_PSTATOBJ )
#define IMP_RESET_TGT_PSTATOBJ(x)	( x &= ~IMP_TTGT_STATUS_PSTATOBJ )


typedef struct IpClassFeature
{
	IMP_S32 label;
	IMP_DOUBLE _P;
	IMP_DOUBLE _D;
	IMP_DOUBLE _Axis;
	IMP_DOUBLE _P13;
	IMP_DOUBLE _P23;
	IMP_DOUBLE _I;
	IMP_DOUBLE _Delta;
	IMP_DOUBLE _Hu[7];
}IpClassFeature;


typedef struct
{
	IMP_S32 s32Used;
	IMP_U32 u32Time;
	IMP_U32 u32Type;
	IMP_U32 u32TgtEvent;
	IMP_POINT_S stPt;
	IMP_RECT_S stRg;
	IMP_U32 u32AreaPixel;
	IMP_U32 u32AreaActual;
	IMP_S32 s32Velocity;
	IMP_S32 s32Direction;
	IMP_S32 s32DiffEnergy;
	IMP_U32 u32DrgType;
} IpTargetPosition;


#define IMP_MAX_TRAJECTORY_LEN		256


typedef struct
{
	IpTargetPosition astPositions[IMP_MAX_TRAJECTORY_LEN];
	IMP_S32 s32Num;
} IpTargetTrajectory;

IMP_INLINE IMP_VOID ipTargetTrajectoryNewPosition( IpTargetTrajectory *pstTraject )
{
	pstTraject->s32Num++;
}

IMP_INLINE IpTargetPosition* ipTargetTrajectoryGetPosition( IpTargetTrajectory *pstTraject, IMP_S32 s32Index )
{
	return &pstTraject->astPositions[(IMP_U32)(pstTraject->s32Num-1+s32Index)&(IMP_MAX_TRAJECTORY_LEN-1)];
}

IMP_INLINE IMP_U32 ipTargetTrajectoryGetLength( IpTargetTrajectory *pstTraject )
{
	return pstTraject->s32Num<=IMP_MAX_TRAJECTORY_LEN ? pstTraject->s32Num : IMP_MAX_TRAJECTORY_LEN;
}

IMP_INLINE IMP_VOID ipTargetTrajectoryClear( IpTargetTrajectory *pstTraject )
{
	memset( pstTraject, 0, sizeof(IpTargetTrajectory) );
}


typedef struct
{
	IMP_S32 s32Num;
	IMP_POINT_S *pstPoints;
} IpTargetContour;


typedef struct
{
	PEA_DETECTED_REGIONSET_S *pstDRegions;
	IMP_S32 s32DrgIndex;
	IpTargetContour stContour;
} IpTargetImage;


typedef struct
{
	IMP_U32 u32StaticTimeStart;
	IMP_U32 u32EllipseAngle;
} IpTargetFeatureVal;


#define IMP_TFEAT_BUF_LEN	64


typedef struct
{
	IMP_U8 au8Data[IMP_TFEAT_BUF_LEN];
} IpTargetFeature;


typedef struct
{
	IMP_S32 s32AbandumLikehood;
	IMP_S32 s3ObjStolenLikehood;
	IMP_S32 s32NoParkingLikehood;
} IpStaticTargetInfo;


typedef struct
{
	IMP_U32 u32Status;
	IMP_U32 u32Type;
	IMP_S32 s32HeadSingleLikehood;
	IMP_S32 s32HeadNum;
	IMP_S32 s32HumanLikehood;
	IMP_S32 s32VehicleLikehood;
	IMP_S32 s32WaggleLikehood;
	IpStaticTargetInfo stStaticTargetInfo;
	IMP_U32 u32TgtEvent;
	IMP_U32 u32AlarmLevel;
	IMP_U32 au32Events[IMP_MAX_NUM_RULE_ZONE];
} IpTargetInfo;


typedef struct
{
	IMP_S32 s32Used;
	IMP_U32 u32TargetId;
	IpTargetInfo stTargetInfo;
	IpTargetImage stTargetImage;
	IpTargetFeature stTargetFeat;
	IpTargetTrajectory stTrajectory;
	PEA_SPECIAL_DATA_S stPrivateData;
} IpTrackedTarget;


IMP_INLINE IMP_S32 ipTrackedTargetIsValid( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsActive( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsMeasured( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_MEASURED(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsPredicted( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_PREDICTED(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsActiveMeasured( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_MEASURED(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsActivePredicted( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_PREDICTED(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsActiveMtrend( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsActiveMeasuredMtrend( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_MEASURED(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 ipTrackedTargetIsActivePredictedMtrend( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used && IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_PREDICTED(pstTarget->stTargetInfo.u32Status) && IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status) );
}

IMP_INLINE IMP_S32 TRACKED_TARGET_is_meanshifted( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used
		&& IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status)
		&& IMP_IS_TGT_MEASURED(pstTarget->stTargetInfo.u32Status)
		&& IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status)
		&& IMP_IS_TGT_MEANSHIFT(pstTarget->stTargetInfo.u32Status)
		//		&& !IMP_IS_TGT_MRF(pstTarget->stTargetInfo.u32Status)
		);
}

IMP_INLINE IMP_S32 TRACKED_TARGET_is_blobed( IpTrackedTarget *pstTarget )
{
	return ( pstTarget->s32Used
		&& IMP_IS_TGT_STABLE(pstTarget->stTargetInfo.u32Status)
		&& IMP_IS_TGT_MEASURED(pstTarget->stTargetInfo.u32Status)
		&& IMP_IS_TGT_MTREND(pstTarget->stTargetInfo.u32Status)
		&& !IMP_IS_TGT_MEANSHIFT(pstTarget->stTargetInfo.u32Status)
		//		&& !IS_TGT_MRF(pstTarget->stTargetInfo.u32Status)
		);
}

typedef struct
{
	IMP_S32 s32UsedTotal;
	IpTrackedTarget astTargets[IMP_MAX_TGT_CNT];
	IpClassFeature  astClassFeature[IMP_MAX_TGT_CNT];
	PEA_SPECIAL_DATA_S stPrivateData;
} IpTrackedTargetSet;


IMP_VOID ipTrackedTargetClear( IpTrackedTarget *pstTarget );

IMP_VOID ipTrackedTargetSetClear( IpTrackedTargetSet *pstTargetSet );

IMP_S32 ipTrackedTargetSetAlloc( IpTrackedTargetSet *pstTargetSet );

IMP_VOID ipTrackedTargetSetFree( IpTrackedTargetSet *pstTargetSet, IMP_S32 s32Index );


IMP_VOID ipDetectedRegionClear( PEA_DETECTED_REGION_S *pstDrg );

IMP_VOID ipDetectedRegionSetClear( PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_S32 ipDetectedRegionSetAlloc( PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_VOID ipDetectedRegionSetFree( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index );

IMP_S32 ipDetectedRegionSetIsEmpty( PEA_DETECTED_REGIONSET_S *pstRgs );


typedef enum impMODULE_STATUS_E
{
	IMP_MOUDLE_STATUS_UNKNOWN,
	IMP_MOUDLE_STATUS_SUCCESS,
	IMP_MOUDLE_STATUS_FAILED,
	IMP_MOUDLE_STATUS_MAX
} MODULE_STATUS_E;


typedef enum impSCENE_STATUS_E
{
	IMP_SCENE_STATUS_UNKNOWN,

	IMP_SCENE_STATUS_INITING,
	IMP_SCENE_STATUS_WORKING,
	IMP_SCENE_STATUS_CHANGED,

	IMP_SCENE_STATUS_MAX

} SCENE_STATUS_E;


typedef enum impSIGNAL_STATUS_E
{
	IMP_SIGNAL_STATUS_UNKNOWN,

	IMP_SIGNAL_STATUS_GOOD,
	IMP_SIGNAL_STATUS_BAD,
	IMP_SIGNAL_STATUS_LOSS,

	IMP_SIGNAL_STATUS_MAX
} SIGNAL_STATUS_E;


typedef enum impVIDEO_SIGNAL_STATUS_E
{
	IMP_VIDEO_SGNSTAT_NORMAL	      = 0x0001,
	IMP_VIDEO_SGNSTAT_FIERCE_LIGHT	  = 0x0002,
	IMP_VIDEO_SGNSTAT_WOBBLE	      = 0x0004,
	IMP_VIDEO_SGNSTAT_OCCLUSION	      = 0x0008,
	IMP_VIDEO_SGNSTAT_INTERFERENCE	  = 0x0010,
	IMP_VIDEO_SGNSTAT_FIELD_SCRAMBLE  = 0x0020,
	IMP_VIDEO_SGNSTAT_LIGHT_MUTATION  = 0x0040,
	IMP_VIDEO_SGNSTAT_LOST     	      = 0x0080
} VIDEO_SIGNAL_STATUS_E;



#ifdef SUPPORT_SOLDIER

typedef enum impSOLDIER_STATUS_E
{
	IMP_SOLDIER_STATUS_UNKNOWN,

	IMP_SOLDIER_STATUS_NORMAL,
	IMP_SOLDIER_STATUS_ABNORMAL,

	IMP_SOLDIER_STATUS_MAX
} SOLDIER_STATUS_E;
#endif


typedef enum impWATERMARK_STATUS_E
{
	IMP_WATERMARK_STAT_UNKNOWN = 0,
	IMP_WATERMARK_STAT_IMPOWER

} WATERMARK_STATUS_E;


typedef enum impDETECTOR_METHOD_E
{
	IMP_FAST_METHOD = 0,
	IMP_CODEBOOK_METHOD
} DETECTOR_METHOD_E;

typedef struct impPEA_STATUS_S
{
	IMP_U32 u32DetectorStatus;
	IMP_U32 u32TrackerStatus;
	IMP_U32 u32ClassifierStatus;
	IMP_U32 u32AnalystStatus;
	IMP_U32 u32WaterMarkStatus;

	IMP_U32 u32DetectorMethod;

	IMP_U32 u32SceneStatus;
	IMP_U32 u32SceneStatusPre;

	IMP_U32 u32SignalStatus;
	IMP_U32 u32SignalStatusPre;


	IMP_U32 u32NotFirstFrm;
	IMP_U32 u32NeedReInit;
	IMP_U32 u32BgIsUpdated;
	IMP_U32 u32FrmCounter;
	IMP_U32 u32ProcessChg;

} PEA_STATUS_S;

IMP_VOID IMP_PEA_StatusClear( PEA_STATUS_S *pstStatus );
#define IMP_STAT_BG_IS_UPDATED(pstStatus) ( pstStatus->u32BgIsUpdated )
#define IMP_STAT_BG_IS_UPDATED_CHG(pstStatus) ( pstStatus->u32BgIsUpdated && pstStatus->u32ProcessChg )

typedef RESULT_S IMP_OUTPUT;


//output information for Gray Gaussian module
typedef struct impOutputGrayGaussian_S
{
	GRAY_IMAGE_S *pstBkg;
	GRAY_IMAGE_S *pstFilter;
}IMP_OutputGrayGaussian_S;


//ViBe输出信息
typedef struct impOutputViBe_S
{
	GRAY_IMAGE_S stImgBg; //background image
	GRAY_IMAGE_S stImgFg; //binary image
}IMP_OutputViBe_S;


//light remove
typedef struct impOutputLightRemove_S
{
	IMP_UCHAR * pu8img;
}IMP_OutputLightRemove_S;


typedef struct impPEA_RESULT_S
{
	IMP_S32 s32Width;
	IMP_S32 s32Height;
	IMP_U32 u32FrmTimeCur;
	IMP_U32 u32FrmTimePre;
	IMP_S32 s32Noise; //noise standard deviation
	GRAY_IMAGE_S stImgInGray;
	IMP_S32 s32ModuleSwitch; //0:PEA 1:OSC

	PEA_STATUS_S stSysStatus;
	IMP_S32 s32VideoAbnrmlStatus;

	PEA_DETECTED_REGIONSET_S stDRegionSet;
	PEA_DETECTED_REGIONSET_S stDRegionSetChg;
	PEA_DETECTED_REGIONSET_S stDRegionSetOsc; //OSC Detected Region Set

	IpTrackedTargetSet stTrackedTargetSet; 
	IpTrackedTargetSet stTrackedTargetSetChg;
	IpTrackedTargetSet stTrackedTargetSetOsc; //OSC TrackedTarget Set

	IMP_OUTPUT stOutput;
	YUV_IMAGE422_S *pstImageYUV;
	EVT_MGR_S stEvtMgr;
	
	IMP_OutputGrayGaussian_S stOutPutGG; //
	IMP_OutputViBe_S stOutPutViBeModel;  //
	IMP_OutputLightRemove_S stOutPutLR;
	
} PEA_RESULT_S;


IMP_VOID IMP_PEA_ResultInit( PEA_RESULT_S *pstResult, IMP_S32 s32ImgWidth, IMP_S32 s32ImgHeight, MEM_MGR_ARRAY_S *pstMemMgr );
IMP_VOID IMP_PEA_ResultTerminate( PEA_RESULT_S *pstResult, MEM_MGR_ARRAY_S *pstMemMgr );
IMP_VOID IMP_PEA_ResultClear( PEA_RESULT_S *pstResult );


IMP_EXPORTS IMP_S32 ipCalcRegionFgNum( GRAY_IMAGE_S *pstImage, IMP_RECT_S *pstRect );
IMP_EXPORTS IMP_S32 ipCalFgRatio(GRAY_IMAGE_S *pstImageB);

IMP_EXPORTS IMP_S32 IMP_FDEPTH_CalcDRegionResolution( FDEPTH_S *pstFDepth, PEA_DETECTED_REGION_S *pstDrg, IMP_DOUBLE *pdResolution );
IMP_EXPORTS IMP_S32 IMP_FDEPTH_calcDRegionArea( FDEPTH_S *pstFDepth, PEA_DETECTED_REGION_S *pstDrg );
#ifdef __cplusplus
}
#endif

#endif

