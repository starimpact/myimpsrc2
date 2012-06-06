
#ifndef _IMP_PEA_BVA_COMMON_H_
#define _IMP_PEA_BVA_COMMON_H_

#include "imp_ga_common.h"
#include "imp_pea_common.h"

#ifdef __cplusplus
extern "C"
{
#endif

/** °Ëµã¶à±ßÐÎ */
typedef struct
{
	IMP_POINT_S ul_point;
	IMP_POINT_S ur_point;
	IMP_POINT_S dl_point;
	IMP_POINT_S dr_point;
	IMP_POINT_S lu_point;
	IMP_POINT_S ld_point;
	IMP_POINT_S ru_point;
	IMP_POINT_S rd_point;
}IpEightPolygon;


#define IP_BYTE_CIARRAY_ELEM_NUM	256

typedef struct
{
	IMP_U8 au8Elems[IP_BYTE_CIARRAY_ELEM_NUM];
	IMP_U32 u32Num;
} IpCycleByteArray;

IMP_S32 ipZoneObjectEffective( IpTrackedTarget *pstTarget, GRAY_IMAGE_S *pstImage, IMP_S32 s32ZoneIndex,IMP_S32 s32UsingBottom,IMP_S32 s32DelicacyFlag,IMP_S32 s32AreaRatio );
IMP_S32 ipIsAcoordantAngleRange( IMP_S32 s32Angle, IMP_S32 s32Direction, IMP_S32 s32Tolerance );
IMP_S32 ipIsAccordantStatic( IpTrackedTarget *pstTarget );
IMP_S32 ipIsAccordantStaticTime( IpTrackedTarget *pstTarget );
IMP_S32 ipGetObjectDirectionByTrajectory( IpTrackedTarget *pstTarget,IMP_S32 *s32Direction );
IMP_S32 ipGetObjectDirection( IpTrackedTarget *pstTarget,IMP_S32 s32PosIndex,IMP_S32 *s32Direction );
IMP_S32 ipGetObjectActualArea( IpTrackedTarget *pstTarget );
IMP_S32 ipComputePolygonOfTarget(IpTrackedTarget *pstTarget,IpTargetPosition *pstPos,IpEightPolygon *pstPolygon);
IMP_S32 ipComputePointsInZone(IpTrackedTarget *pstTarget,IpTargetPosition *pstPos,GRAY_IMAGE_S *pstImage,IMP_S32 s32ZoneIndex);
// IMP_S32 ipGetRectInRegionPointNum(GRAY_IMAGE_S *pstImage,IpTargetPosition  *pstPos,IMP_S32 s32ZoneIndex);
IMP_S32 ipIsObjectInZone(IpCycleByteArray *pstCycleArray,IMP_S32 s32ZoneIndex,IMP_S32 s32PosIndex);
IMP_VOID ipSetBitValue(IMP_S32 s32BitIndex,IMP_U8 *pu8ByteValue);

IMP_INLINE IMP_VOID ipByteCArrayClear( IpCycleByteArray *pstCycleArray )
{
	memset( pstCycleArray, 0, sizeof(IpCycleByteArray) );
}
IMP_INLINE IMP_VOID ipByteCArrayPushElem( IpCycleByteArray *pstCycleArray, IMP_U8 u8Elem )
{
	pstCycleArray->u32Num++;
	pstCycleArray->au8Elems[(IMP_U32)(pstCycleArray->u32Num - 1) & (IP_BYTE_CIARRAY_ELEM_NUM - 1)] = u8Elem;
}
IMP_INLINE IMP_U8 ipByteCArrayGetElem( IpCycleByteArray *pstCycleArray, IMP_S32 s32Index )
{
	return pstCycleArray->au8Elems[(IMP_U32)(pstCycleArray->u32Num - 1 + s32Index) & (IP_BYTE_CIARRAY_ELEM_NUM - 1)];
}
IMP_INLINE IMP_U32 ipByteCArrayGetLength( IpCycleByteArray *pstCycleArray )
{
	return pstCycleArray->u32Num<=IP_BYTE_CIARRAY_ELEM_NUM ? pstCycleArray->u32Num : IP_BYTE_CIARRAY_ELEM_NUM;
}

#define IMP_IS_ACTIVE_ZONE(pstZone,s32FunCode) ( (pstZone)->u32Valid && (pstZone)->u32Enable && ((pstZone)->u32Mode&s32FunCode) )



#define IMP_MAX_NODE 256

#define IMP_NOT_APPEAR -1


typedef struct
{
	IMP_S32 s32Id;
	IMP_S32 s32Next;
	IMP_S32 s32Up;
}IpNode;


typedef struct
{
	IpNode astNode[IMP_MAX_NODE];
	IpNode *pstFirstUsedNode;
	IpNode *pstFirstFreeNode;
	IMP_S32 s32NumUsed;
	IMP_S32 s32NumFree;
	IMP_S32 s32NumTotal;
}IpStaticList;


typedef struct
{
	IMP_U32 u32TargetId;
	IMP_POINT_S stCentroid;
	IMP_RECT_S stRect;
	IMP_S32 s32TargetType;
	IMP_S32 aZoneFlag[IMP_MAX_NUM_RULE_ZONE];
	IMP_S32 s32DisappearZone;

	IMP_S32 s32Status;
	IMP_S32 s32DisappearTime;
	IMP_U32 u32EndId;
}IpRecordInfo;


#ifdef __cplusplus
}
#endif

#endif
