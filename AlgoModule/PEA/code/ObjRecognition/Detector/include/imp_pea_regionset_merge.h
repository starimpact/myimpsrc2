#ifndef _IMP_PEA_REGIONSET_MERGE_H_
#define _IMP_PEA_REGIONSET_MERGE_H_

#include "imp_pea_common.h"


typedef struct impPEA_RGE_MERGE_INNER_PARA_S
{
	IMP_S32 s32InnerLight;
} PEA_RGE_MERGE_INNER_PARA_S;


typedef struct impPEA_MERGE_NEIGHBOR_PARA_S
{
	IMP_S32 s32NeighborDiff;
	IMP_S32 s32ThNeighborRatio;
} PEA_MERGE_NEIGHBOR_PARA_S ;


typedef struct impPEA_RGE_MERGE_PARALLE_PARA_S
{
	IMP_S32 s32ThMergeParalleDist;
} PEA_RGE_MERGE_PARALLE_PARA_S;


typedef struct
{
	IMP_S32 s32MergeRegion;
	IMP_S32 s32MergeInner;
	IMP_S32 s32MergeNeighbor;
	IMP_S32 s32MergeParallel;

	PEA_RGE_MERGE_INNER_PARA_S stInnerPara;
	PEA_MERGE_NEIGHBOR_PARA_S stNeighborPara;
	PEA_RGE_MERGE_PARALLE_PARA_S stParallelPara;
} PEA_RGE_MERGE_PARA_S;


IMP_EXPORTS IMP_VOID IMP_PEA_RGE_MergeRegionSet( PEA_RGE_MERGE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_VOID IMP_PEA_RGE_MergeRegionSet_Static( PEA_RGE_MERGE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_EXPORTS IMP_VOID IMP_PEA_RGE_MergeRegionSetInner( PEA_RGE_MERGE_INNER_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_EXPORTS IMP_VOID IMP_PEA_RGE_MergeRegionSetNeighbor( PEA_MERGE_NEIGHBOR_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

IMP_EXPORTS IMP_VOID IMP_PEA_RGE_MergeRegionSetParallel( PEA_RGE_MERGE_PARALLE_PARA_S *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );


#endif
