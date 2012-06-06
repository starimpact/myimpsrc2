#include "imp_pea_tracker.h"



IMP_VOID ipCsGroupUpdateRegister(IpTargetTrackerMotion *pstTracker)
{

	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTardat;
	IMP_S32 flag_targetid_exist;

	IMP_S32 i, j, k, m, mc;


	{

		for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
		{
			for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
			{

				if (pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j]==0)
					continue;
				pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
				if(pstTarsetdat->stGroupSet.astGroups[i].s32ToBeDeleted[j]==IMP_TRUE)
				{
					pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]=NULL;
					pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j]=0;
					pstTarsetdat->stGroupSet.astGroups[i].s32ToBeDeleted[j]=IMP_FALSE;
					pstTardat->cs.pstGroup = NULL;
					pstTardat->cs.s32CollisionLength=0;
				}
			}
		}


		for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
		{

			for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
			{

				flag_targetid_exist=IMP_FALSE;
				if (pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j] != 0)
				{
					for( m=0, mc=0; m<IMP_MAX_TGT_CNT && mc<(pstTargetSet->s32UsedTotal); m++ )
					{
						if(!pstTarget[m].s32Used)
							continue;
						mc++;

						if (pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j] == pstTarget[m].u32TargetId)
						{
							flag_targetid_exist=IMP_TRUE;
							break;
						}
					}
				}


				if(!flag_targetid_exist)
				{
					if(pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal)
					{
						pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal--;
						for (k=j;k<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;k++)
						{
							pstTarsetdat->stGroupSet.astGroups[i].pastTargets[k]=pstTarsetdat->stGroupSet.astGroups[i].pastTargets[k+1];
							pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[k]=pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[k+1];
						}
						j--;
					}
				}

			}


			if (pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal==1)
			{
 				pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[0]->stPrivateData.pDataTracker;
				pstTardat->cs.pstGroup = NULL;
				pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal--;
			}


			if (pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal==0)
			{
				if(pstTarsetdat->stGroupSet.s32UsedTotal)
				{
					pstTarsetdat->stGroupSet.s32UsedTotal--;
					for (k=i;k<pstTarsetdat->stGroupSet.s32UsedTotal;k++)
					{
						pstTarsetdat->stGroupSet.astGroups[k]=pstTarsetdat->stGroupSet.astGroups[k+1];
						for( m=0, mc=0; m<IMP_MAX_TGT_CNT && mc<(pstTargetSet->s32UsedTotal); m++ )
						{
							if(!pstTarget[m].s32Used)
								continue;
							mc++;
							pstTardat = (IpTargetDataTrackerM*)pstTarget[m].stPrivateData.pDataTracker;
			 				if (&pstTarsetdat->stGroupSet.astGroups[k+1] == pstTardat->cs.pstGroup)
							{
								pstTardat->cs.pstGroup = &pstTarsetdat->stGroupSet.astGroups[k];
							}
						}
					}
					pstTarsetdat->stGroupSet.astGroups[k].s32UsedTotal=0;
					i--;
				}
			}
		}


		for (i=0;i<IMP_MAX_GROUP_CNT;i++)
		{
			if (pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal==0)
			{

				pstTarsetdat->stGroupSet.astGroups[i].s32GroupArea = 0;
				pstTarsetdat->stGroupSet.astGroups[i].s32MemberMaxArea = 0;
				pstTarsetdat->stGroupSet.astGroups[i].s32MemberMinArea = 0;
				pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaRegions = 0;
				pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaTargets = 0;

				for (j=0;j<IMP_MAX_TARGET_CNT_IN_GROUP;j++)
				{
					pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j] = 0;
					pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j] = 0;
					pstTarsetdat->stGroupSet.astGroups[i].s32ToBeDeleted[j] = IMP_FALSE;
				}
			}
		}

#ifdef DEBUG_TRACKER

		{
			IMP_S32 flag_member_valid;
			IMP_S32 flag_pgroup_valid;
			IMP_S32 jc;

			IMP_ASSERT(pstTarsetdat->stGroupSet.s32UsedTotal<=IMP_MAX_GROUP_CNT);
			for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
			{
				for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
				{
					IMP_ASSERT(pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal<=IMP_MAX_TARGET_CNT_IN_GROUP);
					flag_member_valid = IMP_FALSE;
					IMP_ASSERT(pstTarsetdat->stGroupSet.astGroups[i].s32ToBeDeleted[j]==IMP_FALSE);
					for( m=0, mc=0; m<IMP_MAX_TGT_CNT && mc<(pstTargetSet->s32UsedTotal); m++ )
					{
						pstTardat = (IpTargetDataTrackerM*)pstTarget[m].stPrivateData.pDataTracker;
						if(!pstTarget[m].s32Used)
							continue;
						mc++;

						if (pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j] == pstTarget[m].u32TargetId)//找到对应ID目标
						{
							IMP_ASSERT(pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]==&pstTarget[m]);//检测地址有效性
							IMP_ASSERT(&pstTarsetdat->stGroupSet.astGroups[i]==pstTardat->cs.pstGroup);//检测组有效性
							flag_member_valid = IMP_TRUE;
						}
					}
					IMP_ASSERT(flag_member_valid);
				}
			}


			for( j=0, jc=0; j<IMP_MAX_TGT_CNT && jc<(pstTargetSet->s32UsedTotal); j++ )
			{
				if(!pstTarget[j].s32Used)
					continue;
				jc++;
				pstTardat = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;
				if(pstTardat->cs.pstGroup)
				{
					flag_pgroup_valid = IMP_FALSE;
					for (k=0;k<pstTardat->cs.pstGroup->s32UsedTotal;k++)
					{
						if (pstTardat->cs.pstGroup->au32TargetIds[k] == pstTarget[j].u32TargetId)
						{
							IMP_ASSERT(pstTardat->cs.pstGroup->pastTargets[k]==&pstTarget[j]);
							flag_pgroup_valid = IMP_TRUE;
						}
					}
					IMP_ASSERT(flag_pgroup_valid);
				}
			}
		}
#endif


		{
			for (i=0;i<IMP_MAX_GROUP_CNT;i++)
			{
				for (j=0;j<IMP_MAX_TARGET_CNT_IN_GROUP;j++)
				{
					pstTarsetdat->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j]=0;
				}
			}
		}

	}
}


IMP_VOID ipCsGroupUpdateAreaInfo(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
//	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTardat;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
//	IpDrgDataTrackerM *drgdat;
	IMP_S32 tmpmax,tmpmin;
	IMP_S32 i,j,id;

    for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
	{
		pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[0]->stPrivateData.pDataTracker;
		tmpmax = pstTardat->cs.sampleTardat.s32ObjArea;
		tmpmin = pstTardat->cs.sampleTardat.s32ObjArea;
		pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaTargets=pstTardat->cs.sampleTardat.s32ObjArea;
		pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaRegions=0;


		pstTardat->cs.pstGroup->groupRect = pstTardat->stMinRect;

		if ( pstTardat->cs.pstGroup->pstRegion )
		{
			pstTardat->cs.pstGroup->groupRect.s16X1 = min(pstTardat->cs.pstGroup->groupRect.s16X1,pstTardat->cs.pstGroup->pstRegion->stRect.s16X1);//组检测
			pstTardat->cs.pstGroup->groupRect.s16X2 = max(pstTardat->cs.pstGroup->groupRect.s16X2,pstTardat->cs.pstGroup->pstRegion->stRect.s16X2);
			pstTardat->cs.pstGroup->groupRect.s16Y1 = min(pstTardat->cs.pstGroup->groupRect.s16Y1,pstTardat->cs.pstGroup->pstRegion->stRect.s16Y1);
			pstTardat->cs.pstGroup->groupRect.s16Y2 = max(pstTardat->cs.pstGroup->groupRect.s16Y2,pstTardat->cs.pstGroup->pstRegion->stRect.s16Y2);
		}


		for (j=1;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
		{
			pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;


			pstTardat->cs.pstGroup->groupRect.s16X1 = min(pstTardat->cs.pstGroup->groupRect.s16X1,pstTardat->stMinRect.s16X1);
			pstTardat->cs.pstGroup->groupRect.s16X2 = max(pstTardat->cs.pstGroup->groupRect.s16X2,pstTardat->stMinRect.s16X2);
			pstTardat->cs.pstGroup->groupRect.s16Y1 = min(pstTardat->cs.pstGroup->groupRect.s16Y1,pstTardat->stMinRect.s16Y1);
			pstTardat->cs.pstGroup->groupRect.s16Y2 = max(pstTardat->cs.pstGroup->groupRect.s16Y2,pstTardat->stMinRect.s16Y2);


			pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaTargets += pstTardat->cs.sampleTardat.s32ObjArea;
			if (pstTardat->cs.sampleTardat.s32ObjArea > tmpmax)
			{
				tmpmax = pstTardat->cs.sampleTardat.s32ObjArea;
			}
			else if (pstTardat->cs.sampleTardat.s32ObjArea < tmpmin)
			{
				tmpmin = pstTardat->cs.sampleTardat.s32ObjArea;
			}

		}

		pstTarsetdat->stGroupSet.astGroups[i].s32MemberMaxArea = tmpmax;
		pstTarsetdat->stGroupSet.astGroups[i].s32MemberMinArea = tmpmin;


		if (pstTarsetdat->stGroupSet.astGroups[i].s32ToBeDeleted[0])
		{
			for (j=0;j<IMP_MAX_TARGET_CNT_IN_GROUP && pstTarsetdat->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j];j++)
			{
				id = pstTarsetdat->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j];
				pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaRegions += pstDregion[id].s32AreaPixel;
			}
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
			info2 += sprintf( info2, "IdxD: ");
			for (j=0;j<IMP_MAX_TARGET_CNT_IN_GROUP;j++)
			{
				if (pstTarsetdat->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j])
				{
					info2 += sprintf( info2, "%03d_%03d ",pstTarsetdat->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j],
						pstDregion[pstTarsetdat->stGroupSet.astGroups[i].as32DrgIdInMultiSplit[j]].s32AreaPixel);
				}
			}
			info2 += sprintf( info2, "IdxT: ");
			for (j=0;j<IMP_MAX_TARGET_CNT_IN_GROUP;j++)
			{
				if (pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j])
				{
					pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
					info2 += sprintf( info2, "%03d_%03d ",pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j]-0x90000000, pstTardat->cs.sampleTardat.s32ObjArea);
				}
			}
			info2 += sprintf( info2, "\n");
#endif
		}
	}
}


IMP_VOID ipCsGroupUpdateRectInfo(IpTargetTrackerMotion *pstTracker)
{
	//////////////////////////////////////////////
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	//	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTardat;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IMP_S32 i,j,k;

    for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
	{
		for (k=0;k<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;k++)
		{
			pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[k]->stPrivateData.pDataTracker;
			if(pstTardat->cs.s32CurCollision)
			{
				break;
			}
		}


		pstTardat->cs.pstGroup->groupRect.s16X1 = pstTardat->stMinRect.s16X1;
		pstTardat->cs.pstGroup->groupRect.s16X2 = pstTardat->stMinRect.s16X2;
		pstTardat->cs.pstGroup->groupRect.s16Y1 = pstTardat->stMinRect.s16Y1;
		pstTardat->cs.pstGroup->groupRect.s16Y2 = pstTardat->stMinRect.s16Y2;

		if ( pstTardat->cs.pstGroup->pstRegion )
		{
			pstTardat->cs.pstGroup->groupRect.s16X1 = min(pstTardat->cs.pstGroup->groupRect.s16X1,pstTardat->cs.pstGroup->pstRegion->stRect.s16X1);//组检测
			pstTardat->cs.pstGroup->groupRect.s16X2 = max(pstTardat->cs.pstGroup->groupRect.s16X2,pstTardat->cs.pstGroup->pstRegion->stRect.s16X2);
			pstTardat->cs.pstGroup->groupRect.s16Y1 = min(pstTardat->cs.pstGroup->groupRect.s16Y1,pstTardat->cs.pstGroup->pstRegion->stRect.s16Y1);
			pstTardat->cs.pstGroup->groupRect.s16Y2 = max(pstTardat->cs.pstGroup->groupRect.s16Y2,pstTardat->cs.pstGroup->pstRegion->stRect.s16Y2);
		}


		for (j=k;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
		{
			if(!pstTardat->cs.s32CurCollision)
				continue;

			pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;


			pstTardat->cs.pstGroup->groupRect.s16X1 = min(pstTardat->cs.pstGroup->groupRect.s16X1,pstTardat->stMinRect.s16X1);
			pstTardat->cs.pstGroup->groupRect.s16X2 = max(pstTardat->cs.pstGroup->groupRect.s16X2,pstTardat->stMinRect.s16X2);
			pstTardat->cs.pstGroup->groupRect.s16Y1 = min(pstTardat->cs.pstGroup->groupRect.s16Y1,pstTardat->stMinRect.s16Y1);
			pstTardat->cs.pstGroup->groupRect.s16Y2 = max(pstTardat->cs.pstGroup->groupRect.s16Y2,pstTardat->stMinRect.s16Y2);

		}
	}
}

#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
IMP_VOID ipCsGroupInfoDisplay(IpTargetTrackerMotion *pstTracker)
{

	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IMP_S32* pgroup_used_total = &pstTarsetdat->stGroupSet.s32UsedTotal;
	IpDrgDataTrackerM *drgdat;
	IpTargetDataTrackerM *pstTardat;
	IMP_S32 i,j;


	{
		IMP_S32 collision_cnt=0;
		IMP_S32 split_cnt=0;

		for( i=1; i<IMP_MAX_TGT_CNT-1; i++ )
		{
			drgdat = (IpDrgDataTrackerM*)pstDregion[i].stPrivateData.pDataTracker;
			if(drgdat->s32Collision)
				collision_cnt++;
			if(drgdat->s32Split)
				split_cnt++;
		}


		info2 += sprintf( info2, "RGC:%01d RGS:%01d   \n",collision_cnt,split_cnt);
	}


	{


		info2 += sprintf( info2, "Groups:%01d    ",pstTarsetdat->stGroupSet.s32UsedTotal);

		info2 += sprintf( info2, "Members: ",pstTarsetdat->stGroupSet.s32UsedTotal);
		for (i=0;i<IMP_MAX_GROUP_CNT;i++)
		{

			info2 += sprintf( info2, "%01d->%02d    ",i,pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal);
		}

		info2 += sprintf( info2, "\n");


		for (i=0;i<IMP_MAX_GROUP_CNT;i++)
		{
				info2 += sprintf( info2, "%01d (%03d,%03d,%03d),S(%01d)%01d,[%03d,%03d] - ",
				i,
				pstTarsetdat->stGroupSet.astGroups[i].s32GroupArea,
				pstTarsetdat->stGroupSet.astGroups[i].s32MemberMaxArea,
				pstTarsetdat->stGroupSet.astGroups[i].s32MemberMinArea,
				pstTarsetdat->stGroupSet.astGroups[i].s32StablishCollision,
				pstTarsetdat->stGroupSet.astGroups[i].s32StableTime,
				pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaTargets,
				pstTarsetdat->stGroupSet.astGroups[i].s32SumAreaRegions);

			for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
			{
				pstTardat = (IpTargetDataTrackerM *)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;

				info2 += sprintf( info2, " %d(%X)%01d_%04d_%03d ",
					pstTarsetdat->stGroupSet.astGroups[i].au32TargetIds[j]-0x90000000,
					pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j],
					pstTarsetdat->stGroupSet.astGroups[i].s32ToBeDeleted[j],
					pstTardat->cs.s32CollisionLength,
					pstTardat->cs.sampleTardat.s32ObjArea);
			}
			info2 += sprintf( info2, "\n");
		}
		info2 += sprintf( info2, "\n");
	}
}
#endif


IMP_VOID ipCsGroupClearFlag (IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
//	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTardat;
	IpDrgDataTrackerM *drgdat;
	IMP_S32 i,j,jc;

	for( i=1; i<(IMP_MAX_TGT_CNT-1); i++ )
	{
		drgdat = (IpDrgDataTrackerM*)pstDregion[i].stPrivateData.pDataTracker;
		drgdat->s32Collision = IMP_FALSE;
		drgdat->s32Split = IMP_FALSE;
		drgdat->pstGroup = NULL;
	}


	for( j=0, jc=0; j<IMP_MAX_TGT_CNT && jc<(pstTargetSet->s32UsedTotal); j++ )
	{
		if(!pstTarget[j].s32Used)
			continue;
		jc++;
		pstTardat = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;
		pstTardat->cs.s32LastCollision = pstTardat->cs.s32CurCollision;
		pstTardat->cs.s32CurCollision = IMP_FALSE;
		pstTardat->cs.s32Split = IMP_FALSE;
	}
}

#define IMP_CS_C_OVERLAP_AREA_RATIO	/*pstTracker->pstPara->c_overlap_area_ratio	*/	0.4
#define IMP_CS_C_TM_AREA_RATIO			/*pstTracker->pstPara->c_tm_area_ratio 		*/	0.04
#define IMP_CS_C_TT_AREA_RATIO			/*pstTracker->pstPara->c_tt_area_ratio		*/	0.05
#define IMP_CS_S_OVERLAP_AREA_RATIO	/*pstTracker->pstPara->s_overlap_area_ratio */  0.3
#define IMP_CS_S_MT_AREA_RATIO			/*pstTracker->pstPara->s_mt_area_ratio		*/	0.1 
#define IMP_CS_S_MM_AREA_RATIO			/*pstTracker->pstPara->s_mm_area_ratio 		*/	0.05
#define IMP_CS_ALPHA_AREA_COEF_PLUS	/*pstTracker->pstPara->alpha_area_coef_plus	*/	1.3  //1.1
#define IMP_CS_ALPHA_AREA_COEF_MINUS	/*pstTracker->pstPara->alpha_area_coef_minus*/	0.55
#define IMP_CS_STABLE_COLLISION_MAX_TIME 5

IMP_VOID ipCsDetermineCollision(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IMP_S32* pgroup_used_total = &pstTarsetdat->stGroupSet.s32UsedTotal;
//	IMP_U32* ptarget_used_total_in_group;
	IpTargetDataTrackerM *pstTardat;
	IpTargetDataTrackerM *trans_tardat;
	IpDrgDataTrackerM *drgdat;
	IMP_S32 i,ic;
	IMP_S32 j1,j2,j1c;
	IMP_S32 k;
//	IMP_S32 d_width,d_height;
	IMP_S32 d_area;//for M
	IMP_S32 t_width, t_height,t_area;//for T1
	IMP_S32 lastt_width,lastt_height,lastt_area;//for T2
	IMP_FLOAT collisionAreaRate=0.0;
	IpTrackedTarget *ptempTarget,*plastTempTarget;
	IMP_S32 flag_use_tardat_group;
	IMP_S32 flag_collision;
	IMP_S32 target_index=0,last_target_index=0;
	IMP_S32 intersectArea;
	IMP_S32 dist;
	IpTargetPosition  *tmpPos;

	for( i=1, ic=0; i<(IMP_MAX_TGT_CNT-1) && ic<(pstDRegionSet->s32UsedTotal); i++ )
	{
		IpTargetDataTrackerM* last_tardat = NULL;
		if( !pstDregion[i].u8Used)
			continue;
		ic++;

		d_area = pstDregion[i].s32AreaPixel;
		drgdat = (IpDrgDataTrackerM*)pstDregion[i].stPrivateData.pDataTracker;
		drgdat->s32Collision = IMP_FALSE;

		for( j1=0,j1c=0; j1<IMP_MAX_TGT_CNT && j1c<(pstTargetSet->s32UsedTotal);  j1++ )
		{
			if(!pstTarget[j1].s32Used)
				continue;

			j1c++;

			if (!ipTrackedTargetIsActiveMeasuredMtrend(&pstTarget[j1]))
				continue;



			ptempTarget= &pstTarget[j1];
			pstTardat = (IpTargetDataTrackerM*)pstTarget[j1].stPrivateData.pDataTracker;



			t_width = pstTardat->stMinRect.s16X2 - pstTardat->stMinRect.s16X1 + 1;
			t_height = pstTardat->stMinRect.s16Y2 - pstTardat->stMinRect.s16Y1 + 1;
			t_area = pstTardat->cs.sampleTardat.s32ObjArea;

			dist = ipGetMinimumRegionDistance( &drgdat->stMinRect, &pstTardat->stMinRect, &intersectArea );


			if (
					dist==0
					&& (IMP_FLOAT)intersectArea/(t_width*t_height) > IMP_CS_C_OVERLAP_AREA_RATIO
					&& (IMP_FLOAT)t_area/d_area > IMP_CS_C_TM_AREA_RATIO
				)
			{
				target_index=j1;
			}
			else
				continue;

			for( j2=j1+1; j2<IMP_MAX_TGT_CNT; j2++ )
			{
				if(!pstTarget[j2].s32Used)
					continue;

				if (!ipTrackedTargetIsActiveMeasuredMtrend(&pstTarget[j2]))
					continue;



				plastTempTarget= &pstTarget[j2];
				last_tardat = (IpTargetDataTrackerM*)pstTarget[j2].stPrivateData.pDataTracker;


				lastt_width = last_tardat->stMinRect.s16X2 - last_tardat->stMinRect.s16X1 + 1;
				lastt_height = last_tardat->stMinRect.s16Y2 - last_tardat->stMinRect.s16Y1 + 1;
				lastt_area = last_tardat->cs.sampleTardat.s32ObjArea;


				dist = ipGetMinimumRegionDistance( &drgdat->stMinRect, &last_tardat->stMinRect, &intersectArea );
				if (
					dist==0
					&& (IMP_FLOAT)intersectArea/(lastt_width*lastt_height) > IMP_CS_C_OVERLAP_AREA_RATIO
					&& (IMP_FLOAT)lastt_area/d_area > IMP_CS_C_TM_AREA_RATIO
				   )
   				{
					last_target_index=j2;
				}
				else
					continue;

				{
					collisionAreaRate = (IMP_FLOAT)(min(t_area,lastt_area))/(IMP_FLOAT)(max(t_area,lastt_area));
					if (d_area>t_area *(1-IMP_CS_ALPHA_AREA_COEF_MINUS)
						&& d_area>lastt_area *(1-IMP_CS_ALPHA_AREA_COEF_MINUS)
						&& collisionAreaRate > IMP_CS_C_TT_AREA_RATIO
						&& d_area<(t_area+lastt_area)*(1+IMP_CS_ALPHA_AREA_COEF_PLUS)
						)
					{
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
						if (!pstTardat->cs.s32LastCollision)
						{
//							Beep(600,100);
						}
#endif


						flag_collision=IMP_FALSE;

						if (pstTardat->cs.pstGroup == NULL && last_tardat->cs.pstGroup == NULL && (*pgroup_used_total)<IMP_MAX_GROUP_CNT)
						{

							pstTardat->cs.pstGroup = &pstTarsetdat->stGroupSet.astGroups[*pgroup_used_total];
							last_tardat->cs.pstGroup = &pstTarsetdat->stGroupSet.astGroups[*pgroup_used_total];


							pstTardat->cs.pstGroup->s32UsedTotal = 0;
							pstTardat->cs.pstGroup->pastTargets[pstTardat->cs.pstGroup->s32UsedTotal] = ptempTarget;
							pstTardat->cs.pstGroup->au32TargetIds[pstTardat->cs.pstGroup->s32UsedTotal] = ptempTarget->u32TargetId;
							(pstTardat->cs.pstGroup->s32UsedTotal)++;

							pstTardat->cs.pstGroup->pastTargets[pstTardat->cs.pstGroup->s32UsedTotal] = plastTempTarget;
							pstTardat->cs.pstGroup->au32TargetIds[pstTardat->cs.pstGroup->s32UsedTotal] = plastTempTarget->u32TargetId;
							(pstTardat->cs.pstGroup->s32UsedTotal)++;
							(*pgroup_used_total)++;


							flag_collision=IMP_TRUE;
							IMP_ASSERT(pstTarsetdat->stGroupSet.s32UsedTotal<=IMP_MAX_GROUP_CNT);
							IMP_ASSERT(pstTardat->cs.pstGroup->s32UsedTotal<=IMP_MAX_TARGET_CNT_IN_GROUP);

						}//The end of "both are new members"


						else if ((pstTardat->cs.pstGroup != last_tardat->cs.pstGroup )&&(pstTardat->cs.pstGroup == NULL || last_tardat->cs.pstGroup==NULL)&&((*pgroup_used_total)<=IMP_MAX_GROUP_CNT))
						{
							if (pstTardat->cs.pstGroup==NULL && last_tardat->cs.pstGroup->s32UsedTotal<IMP_MAX_TARGET_CNT_IN_GROUP)
							{

								pstTardat->cs.pstGroup = last_tardat->cs.pstGroup;


								pstTardat->cs.pstGroup->pastTargets[pstTardat->cs.pstGroup->s32UsedTotal] = ptempTarget;
								pstTardat->cs.pstGroup->au32TargetIds[pstTardat->cs.pstGroup->s32UsedTotal] = ptempTarget->u32TargetId;
								pstTardat->cs.pstGroup->s32UsedTotal++;
								flag_collision=IMP_TRUE;
								IMP_ASSERT(pstTarsetdat->stGroupSet.s32UsedTotal<=IMP_MAX_GROUP_CNT);
								IMP_ASSERT(pstTardat->cs.pstGroup->s32UsedTotal<=IMP_MAX_TARGET_CNT_IN_GROUP);
							}
							else if (last_tardat->cs.pstGroup==NULL && pstTardat->cs.pstGroup->s32UsedTotal<IMP_MAX_TARGET_CNT_IN_GROUP)
							{

								last_tardat->cs.pstGroup = pstTardat->cs.pstGroup;

								pstTardat->cs.pstGroup->pastTargets[pstTardat->cs.pstGroup->s32UsedTotal] = plastTempTarget;
								pstTardat->cs.pstGroup->au32TargetIds[pstTardat->cs.pstGroup->s32UsedTotal] = plastTempTarget->u32TargetId;
								pstTardat->cs.pstGroup->s32UsedTotal++;
								flag_collision=IMP_TRUE;
								IMP_ASSERT(pstTarsetdat->stGroupSet.s32UsedTotal<=IMP_MAX_GROUP_CNT);
								IMP_ASSERT(pstTardat->cs.pstGroup->s32UsedTotal<=IMP_MAX_TARGET_CNT_IN_GROUP);
							}


						}//The end of "one is a new member"


						else if ((pstTardat->cs.pstGroup != last_tardat->cs.pstGroup )&&(pstTardat->cs.pstGroup != NULL && last_tardat->cs.pstGroup!=NULL) && (*pgroup_used_total)<=IMP_MAX_GROUP_CNT)
						{
							if( (pstTardat->cs.pstGroup->s32UsedTotal+last_tardat->cs.pstGroup->s32UsedTotal)<=IMP_MAX_TARGET_CNT_IN_GROUP )

							{


								IMP_S32 memberCNT;
								IpGroup* src_pgroup;
								for (k=0;k<pstTarsetdat->stGroupSet.s32UsedTotal;k++)
								{
									if(&pstTarsetdat->stGroupSet.astGroups[k] == pstTardat->cs.pstGroup)
									{
										flag_use_tardat_group = IMP_TRUE;
										break;
									}
									if(&pstTarsetdat->stGroupSet.astGroups[k] == last_tardat->cs.pstGroup)
									{
										flag_use_tardat_group = IMP_FALSE;
										break;
									}
								}


								if (flag_use_tardat_group)
								{

									memberCNT = last_tardat->cs.pstGroup->s32UsedTotal;
									src_pgroup = last_tardat->cs.pstGroup;

									last_tardat->cs.pstGroup->s32UsedTotal=0;


									for (k=0;k<memberCNT;k++)
									{

										pstTardat->cs.pstGroup->pastTargets[pstTardat->cs.pstGroup->s32UsedTotal]= src_pgroup->pastTargets[k];
										pstTardat->cs.pstGroup->au32TargetIds[pstTardat->cs.pstGroup->s32UsedTotal] = src_pgroup->pastTargets[k]->u32TargetId;


										trans_tardat = (IpTargetDataTrackerM*)pstTardat->cs.pstGroup->pastTargets[pstTardat->cs.pstGroup->s32UsedTotal]->stPrivateData.pDataTracker;
										trans_tardat->cs.pstGroup = pstTardat->cs.pstGroup;
										pstTardat->cs.pstGroup->s32UsedTotal++;
									}
									(*pgroup_used_total)--;
									flag_collision=IMP_TRUE;

								}
								else //pstTardat->cs.pstGroup MOVE TO last_tardat->cs.pstGroup
								{

									memberCNT=pstTardat->cs.pstGroup->s32UsedTotal;
									src_pgroup = pstTardat->cs.pstGroup;


									pstTardat->cs.pstGroup->s32UsedTotal=0;

									for (k=0;k<memberCNT;k++)
									{

										last_tardat->cs.pstGroup->pastTargets[last_tardat->cs.pstGroup->s32UsedTotal] = src_pgroup->pastTargets[k];
										last_tardat->cs.pstGroup->au32TargetIds[last_tardat->cs.pstGroup->s32UsedTotal] = src_pgroup->pastTargets[k]->u32TargetId;


										trans_tardat = (IpTargetDataTrackerM*)last_tardat->cs.pstGroup->pastTargets[last_tardat->cs.pstGroup->s32UsedTotal]->stPrivateData.pDataTracker;
							 			trans_tardat->cs.pstGroup = last_tardat->cs.pstGroup;
										last_tardat->cs.pstGroup->s32UsedTotal++;
									}
									(*pgroup_used_total)--;
									flag_collision=IMP_TRUE;
								}
							}
							else
							{
								flag_collision=IMP_TRUE;
							}

							IMP_ASSERT(pstTarsetdat->stGroupSet.s32UsedTotal<=IMP_MAX_GROUP_CNT);
							IMP_ASSERT(pstTardat->cs.pstGroup->s32UsedTotal<=IMP_MAX_TARGET_CNT_IN_GROUP);
						}//The end of "both are in different astGroups"


						else if (pstTardat->cs.pstGroup == last_tardat->cs.pstGroup && pstTardat->cs.pstGroup)
						{
							flag_collision=IMP_TRUE;
							IMP_ASSERT(pstTarsetdat->stGroupSet.s32UsedTotal<=IMP_MAX_GROUP_CNT);
							IMP_ASSERT(pstTardat->cs.pstGroup->s32UsedTotal<=IMP_MAX_TARGET_CNT_IN_GROUP);
						}

						if(flag_collision)
						{

							drgdat->s32Collision = IMP_TRUE;
							drgdat->s32MatchedWith = IMP_MATCH_GROUP;
							pstTardat->cs.s32CurCollision = last_tardat->cs.s32CurCollision = IMP_TRUE;
							pstTardat->s32MatchedWith = last_tardat->s32MatchedWith = i;

							if(pstTardat->cs.s32CurCollision==IMP_TRUE && pstTardat->cs.s32LastCollision==IMP_FALSE)
							{
								tmpPos = ipTargetTrajectoryGetPosition( &pstTarget[target_index].stTrajectory, -1 );
								pstTardat->cs.sampleTardat.stCentroid	= tmpPos->stPt;
								pstTardat->cs.sampleTardat.s32Direction	= tmpPos->s32Direction;
								pstTardat->cs.sampleTardat.stRect		= tmpPos->stRg;
							}

							if(last_tardat->cs.s32CurCollision==IMP_TRUE && last_tardat->cs.s32LastCollision==IMP_FALSE)
							{
 								tmpPos = ipTargetTrajectoryGetPosition( &pstTarget[last_target_index].stTrajectory, -1 );
								last_tardat->cs.sampleTardat.stCentroid  = tmpPos->stPt;
								last_tardat->cs.sampleTardat.s32Direction = tmpPos->s32Direction;
								last_tardat->cs.sampleTardat.stRect      = tmpPos->stRg;
							}

							pstTardat->cs.pstGroup->s32GroupArea = d_area;


							drgdat->pstGroup = pstTardat->cs.pstGroup;
							pstTardat->cs.pstGroup->pstRegion = &pstDregion[i];


							if(pstTracker->pstPara->s32CsStableCollision)
							{
								pstTardat->cs.pstGroup->s32StablishCollision = IMP_FALSE;
								pstTardat->cs.pstGroup->s32StableTime=IMP_CS_STABLE_COLLISION_MAX_TIME;
							}
						}

						IMP_ASSERT(pstTardat->cs.s32CollisionLength>=0);
						IMP_ASSERT(last_tardat->cs.s32CollisionLength>=0);

						///////////////////////////////////////////////////////////////
					}
				}
			}
		}
	}
}

IMP_VOID ipCsDetermineSplit (IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
//	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
//	IMP_S32* pgroup_used_total = &pstTarsetdat->stGroupSet.s32UsedTotal;
	IpTargetDataTrackerM *pstTardat;
	IpDrgDataTrackerM *drgdat;
	IMP_S32 i1,i2,i1c;
	IMP_S32 j,jc;
	IMP_S32 k;
	IMP_S32 d_width, d_height,d_area;//for M1
	IMP_S32 lastd_width,lastd_height,lastd_area;//for M2
//	IMP_S32 t_width, t_height,t_area;//for T
	IMP_FLOAT splitAreaRate = 0.0;
//	IMP_S32 target_index=0,last_target_index=0;
	IMP_S32 intersectArea;
	IMP_S32 dist;

	for( j=0, jc=0; j<IMP_MAX_TGT_CNT && jc<(pstTargetSet->s32UsedTotal); j++ )//split
	{
		IpDrgDataTrackerM* last_drgdat = NULL;

		if(!pstTarget[j].s32Used)
			continue;
		jc++;

		if (!ipTrackedTargetIsActiveMeasuredMtrend(&pstTarget[j]))
			continue;

		pstTardat = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;
//		t_area = pstTardat->cs.sampleTardat.s32ObjArea;

		if (!pstTardat->cs.s32LastCollision)
			continue;

		if (!pstTardat->cs.pstGroup)
			continue;

//		t_width = pstTardat->stMinRect.s16X2 - pstTardat->stMinRect.s16X1 + 1;
//		t_height = pstTardat->stMinRect.s16Y2 - pstTardat->stMinRect.s16Y1 + 1;

		for( i1=1, i1c=0; i1<(IMP_MAX_TGT_CNT-1) && i1c<(pstDRegionSet->s32UsedTotal); i1++ )
		{
			if( !pstDregion[i1].u8Used )
				continue;
			i1c++;

			d_width = pstDregion[i1].stRect.s16X2 - pstDregion[i1].stRect.s16X1 + 1;
			d_height = pstDregion[i1].stRect.s16Y2 - pstDregion[i1].stRect.s16Y1 + 1;
			drgdat = (IpDrgDataTrackerM*)pstDregion[i1].stPrivateData.pDataTracker;
			d_area = pstDregion[i1].s32AreaPixel;

			if (drgdat->pstGroup && drgdat->pstGroup!=pstTardat->cs.pstGroup )
				continue;


 			dist = ipGetMinimumRegionDistance( &drgdat->stMinRect, &pstTardat->stMinRect, &intersectArea );
			if (
				dist==0
				&& (IMP_FLOAT)intersectArea/(d_width*d_height)  > IMP_CS_S_OVERLAP_AREA_RATIO
	 			&& (IMP_FLOAT)d_area/pstTardat->cs.pstGroup->s32MemberMinArea > IMP_CS_S_MT_AREA_RATIO
			   )
			{

				for( i2=i1+1; i2<(IMP_MAX_TGT_CNT-1); i2++ )
				{
					if( !pstDregion[i2].u8Used )
						continue;

					lastd_width = pstDregion[i2].stRect.s16X2 - pstDregion[i2].stRect.s16X1 + 1;
					lastd_height = pstDregion[i2].stRect.s16Y2 - pstDregion[i2].stRect.s16Y1 + 1;
					last_drgdat = (IpDrgDataTrackerM*)pstDregion[i2].stPrivateData.pDataTracker;
					lastd_area = pstDregion[i2].s32AreaPixel;

					if (last_drgdat->pstGroup && last_drgdat->pstGroup!=pstTardat->cs.pstGroup )
						continue;



					dist = ipGetMinimumRegionDistance( &last_drgdat->stMinRect, &pstTardat->stMinRect, &intersectArea );
					if (
						dist==0
						&& (IMP_FLOAT)intersectArea/(lastd_width*lastd_height) > IMP_CS_S_OVERLAP_AREA_RATIO
	 					&& (IMP_FLOAT)lastd_area/pstTardat->cs.pstGroup->s32MemberMinArea > IMP_CS_S_MT_AREA_RATIO
					   )
					{

						splitAreaRate = (IMP_FLOAT)(min(d_area,lastd_area))/(IMP_FLOAT)(max(d_area,lastd_area));
						if(pstTardat->cs.pstGroup
							&& splitAreaRate > IMP_CS_S_MM_AREA_RATIO

							&& d_area > (1-IMP_CS_ALPHA_AREA_COEF_MINUS) * pstTardat->cs.pstGroup->s32MemberMinArea
	      					//&& d_area < (1+IMP_CS_ALPHA_AREA_COEF_PLUS) * pstTardat->cs.pstGroup->s32MemberMaxArea
							&& lastd_area > (1-IMP_CS_ALPHA_AREA_COEF_MINUS) * pstTardat->cs.pstGroup->s32MemberMinArea
							//&& lastd_area < (1+IMP_CS_ALPHA_AREA_COEF_PLUS) * pstTardat->cs.pstGroup->s32MemberMaxArea

							&& pstTardat->cs.pstGroup->s32GroupArea < (d_area+lastd_area) * (1+IMP_CS_ALPHA_AREA_COEF_PLUS)
							&& pstTardat->cs.pstGroup->s32GroupArea > d_area * (1-IMP_CS_ALPHA_AREA_COEF_MINUS)
							&& pstTardat->cs.pstGroup->s32GroupArea > lastd_area * (1-IMP_CS_ALPHA_AREA_COEF_MINUS)
						  )
						{
 							drgdat->s32Collision = last_drgdat->s32Collision = IMP_FALSE;
							drgdat->s32Split = last_drgdat->s32Split = IMP_TRUE;
							pstTardat->cs.s32CurCollision = IMP_FALSE;
							pstTardat->cs.s32Split = IMP_TRUE;
							pstTardat->s32MatchedWith = 0;
							last_drgdat->pstGroup = drgdat->pstGroup = pstTardat->cs.pstGroup;


							{
								IMP_S32 flag_drgId_exist=IMP_FALSE;
								for (k=0;pstTardat->cs.pstGroup->as32DrgIdInMultiSplit[k]&&k<IMP_MAX_TARGET_CNT_IN_GROUP;k++)
								{
									if (pstTardat->cs.pstGroup->as32DrgIdInMultiSplit[k]==i1)
										flag_drgId_exist=IMP_TRUE;
								}
								if (!flag_drgId_exist && k<IMP_MAX_TARGET_CNT_IN_GROUP)
								{
									pstTardat->cs.pstGroup->as32DrgIdInMultiSplit[k]=i1;
								}
								flag_drgId_exist=IMP_FALSE;
								for (k=0;pstTardat->cs.pstGroup->as32DrgIdInMultiSplit[k]&&k<IMP_MAX_TARGET_CNT_IN_GROUP;k++)
								{
									if (pstTardat->cs.pstGroup->as32DrgIdInMultiSplit[k]==i2)
										flag_drgId_exist=IMP_TRUE;
								}

								if (!flag_drgId_exist && k<IMP_MAX_TARGET_CNT_IN_GROUP)
								{
									pstTardat->cs.pstGroup->as32DrgIdInMultiSplit[k]=i2;
								}
							}

							///////////////////////////////////////////////////////////////
							{
								for (k=0;k<pstTardat->cs.pstGroup->s32UsedTotal;k++)
								{
									if (pstTarget[j].u32TargetId==pstTardat->cs.pstGroup->au32TargetIds[k])
									{
										pstTardat->cs.pstGroup->s32ToBeDeleted[k]=IMP_TRUE;
									}
								}
							}

							IMP_ASSERT(pstTardat->cs.s32CollisionLength>=0);

							///////////////////////////////////////////////////////////////
						}//end of
					}//end of if
				}//end of for
			}//end of if
		}//end of for
	}//end of for T
}

IMP_VOID ipCsUniformTgtStatusInSplitGroup(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
//	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTardat;
	IMP_S32 i,j;

	for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
	{
		IMP_S32 flag_split_in_group=IMP_FALSE;
		for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
		{
 			pstTardat=(IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
			if (pstTardat->cs.s32Split)
			{
				flag_split_in_group=IMP_TRUE;
			}
		}
		if(flag_split_in_group)
		{
 			for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
			{
				pstTardat=(IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;

				pstTardat->cs.s32CurCollision = IMP_FALSE;
				pstTardat->cs.s32Split = IMP_TRUE;
				pstTardat->s32MatchedWith = 0;
				pstTardat->cs.pstGroup->s32ToBeDeleted[j]=IMP_TRUE;
			}
		}
	}
}

IMP_VOID ipCsCollisionStableProcess(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
//	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
//	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
//	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IpTargetSetDataTrackerM* pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
//	IMP_S32* pgroup_used_total = &pstTarsetdat->stGroupSet.s32UsedTotal;
	IpTargetDataTrackerM *pstTardat;
	IMP_S32 i,j;

	if(pstTracker->pstPara->s32CsStableCollision)
	{
		for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
		{
			IMP_S32 need_stable_target_cnt=0;



			for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
			{
				pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
				if( !pstTardat->cs.s32CurCollision
					&&!pstTardat->cs.s32Split
					&&pstTardat->cs.pstGroup
					&&pstTardat->cs.pstGroup->s32StableTime > 0)
				{
					need_stable_target_cnt++;
				}
			}
			if (need_stable_target_cnt == pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal)
			{
				pstTardat->cs.pstGroup->s32StablishCollision = IMP_TRUE;
			}
			if (pstTardat->cs.pstGroup->s32StablishCollision)
			{

				if (!pstTardat->cs.pstGroup->s32StableTime)
				{

					for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
					{
						pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
						pstTardat->cs.pstGroup->s32ToBeDeleted[j]=IMP_TRUE;
					}
				}
				else
				{

					pstTardat->cs.pstGroup->pstRegion = NULL;


					for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
					{
						pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
						pstTardat->cs.s32CurCollision=IMP_TRUE;
						pstTardat->s32MatchedWith = 0;
					}

					pstTardat->cs.pstGroup->s32StableTime--;
					IMP_ASSERT( pstTardat->cs.pstGroup->s32StableTime>=0 );
				}
			}
		}
	}
	else
	{
		for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
		{

			for (j=0;j<pstTarsetdat->stGroupSet.astGroups[i].s32UsedTotal;j++)
			{
				pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
				if( !pstTardat->cs.s32CurCollision
					&&!pstTardat->cs.s32Split
					&&pstTardat->cs.pstGroup)
				{
					pstTardat->cs.pstGroup->s32ToBeDeleted[j]=IMP_TRUE;
//					PRINT_TARGET2( pstTracker, pstTardat->cs.pstGroup->pastTargets[j] );
				}
			}
		}
	}
}

extern IMP_FLOAT ipCalculateAreaCoefficientM( const IMP_S32 s32a, const IMP_S32 s32b );
extern IMP_FLOAT ipCalculateHistoCoefficient( const IMP_U32 *pu32CurObj, const IMP_U32 *pu32TrackObj, IMP_S32 s32GrayLevel );
IMP_S32 ipCsHistFeatureValidJudge(IpDrgDataTrackerM *pstDrgDat1,IpDrgDataTrackerM *pstDrgDat2,IpTargetDataTrackerM *pstTarData1,IpTargetDataTrackerM *pstTarData2);
IMP_FLOAT ipCalculateAreaCoefMinus(IMP_S32 s32AreaT,IMP_S32 s32AreaD,IpGroup* pstGroup);
IMP_VOID ipCalculateTrendCoefficient(IMP_POINT_S stPtT1,IMP_S32 s32IdxT1,
									 IMP_POINT_S stPtT2,IMP_S32 s32IdxT2,
									 IMP_POINT_S stPtM1,IMP_S32 s32IdxM1,
									 IMP_POINT_S stPtM2,IMP_S32 s32IdxM2,
									 IMP_S32 s32DirT1, IMP_S32 s32DirT2,
									  IMP_FLOAT afMatchCoeff[][IMP_MAX_TGT_CNT]);
IMP_VOID ipCalculateHistoCoefficientBalance(IpDrgDataTrackerM *pstDrgDat1,IMP_S32 s32IdxM1,
											IpDrgDataTrackerM *pstDrgDat2,IMP_S32 s32IdxM2,
											IpTargetDataTrackerM *pstTarData1,IMP_S32 s32IdxT1,
											IpTargetDataTrackerM *pstTarData2,IMP_S32 s32IdxT2,
											 IMP_FLOAT afMatchCoeff[][IMP_MAX_TGT_CNT]);

IMP_VOID ipCalculateSplitObjectMatchCoefficient(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTargetSetDataTrackerM *pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets,*tmptarget;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IpTargetDataTrackerM *pstTardat,*tmptardat;
	IpDrgDataTrackerM *drgdat,*tmpdrgdat;
	IMP_S32 i, j, ic, jc, k, g;
	IMP_FLOAT  histCoefficient, areaCoefficient;
//	IMP_FLOAT  totalCoefficient;
//	IMP_U32 is_static_region;
// 	IMP_S32 static_match_dis_max;
//	IMP_S32 width, height;
//	IMP_S32 flag = 0;
	IMP_FLOAT regionsAreaRate;
	IMP_FLOAT regionsHistRate;
	IMP_FLOAT targetsAreaRate;
	IMP_FLOAT targetsHistRate;
	IMP_FLOAT deltaAreaRate;
	IMP_FLOAT deltaHistRate;

	IMP_S32 target_index,tmptarget_index;
	IMP_S32 region_index,tmpregion_index;

	IMP_S32 alpha;


	IMP_S32 maxDrgId;
	IMP_FLOAT	maxAreaCoef;
	IMP_S32 cntMultiSplit;
	IMP_FLOAT	areaCoefAvg;
	IMP_FLOAT    areaCoefPlus;
	IMP_FLOAT    areaCoefMinus;
	IMP_S32 areaTAvg;
	IMP_S32 flag_use_sameAreaModel=IMP_FALSE;
	IMP_S32 flag_use_areaFeature=IMP_FALSE;
	IMP_S32 flag_use_histFeature=IMP_FALSE;
	IMP_S32 flag_use_trendFeature=IMP_FALSE;


#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD

	IMP_S32 flag_split=0;  //for display debug information
#endif

	pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;

	for (g=0;g<pstTarsetdat->stGroupSet.s32UsedTotal;g++)
	{


		if (pstTarsetdat->stGroupSet.astGroups[g].s32UsedTotal>2)
		{

			cntMultiSplit=0;
			maxDrgId=0;
			maxAreaCoef=0.0;
			flag_use_sameAreaModel=IMP_TRUE;

			areaTAvg=pstTarsetdat->stGroupSet.astGroups[g].s32SumAreaTargets/pstTarsetdat->stGroupSet.astGroups[g].s32UsedTotal;
			for (i=0;i<pstTarsetdat->stGroupSet.astGroups[g].s32UsedTotal;i++)
			{
				IMP_FLOAT deltaAreaRate;
				pstTardat = (IpTargetDataTrackerM*)pstTarsetdat->stGroupSet.astGroups[g].pastTargets[i]->stPrivateData.pDataTracker;
				deltaAreaRate = (IMP_FLOAT)(abs(pstTardat->cs.sampleTardat.s32ObjArea-areaTAvg))/areaTAvg;
				if (deltaAreaRate > 0.3)
				{
					flag_use_sameAreaModel=IMP_FALSE;
				}
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
				info2 += sprintf( info2, "DAR: %0.3f ",deltaAreaRate);
#endif
			}
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
			if (flag_use_sameAreaModel)
			{
				info2 += sprintf( info2, " sameAreaModel\n");//for display debug information
			}
			else
			{
				info2 += sprintf( info2, " diffAreaModel\n");//for display debug information
			}
#endif
		}
		pstTarsetdat->stGroupSet.astGroups[g].s32DrgIdforSingleTgtInMultiSplit=0;
		for( i=1, ic=0; i<(IMP_MAX_TGT_CNT-1) && ic<(pstDRegionSet->s32UsedTotal); i++ )
		{
			if( !pstDregion[i].u8Used )
				continue;
			ic++;
			drgdat = (IpDrgDataTrackerM*)pstDregion[i].stPrivateData.pDataTracker;

			if ( !drgdat->s32Split )
				continue;
			if (drgdat->pstGroup != &pstTarsetdat->stGroupSet.astGroups[g])
				continue;

			for( j=0, jc=0; j<IMP_MAX_TGT_CNT && jc<(pstTargetSet->s32UsedTotal); j++ )
			{
				if(!pstTarget[j].s32Used)
					continue;
				jc++;

				pstTardat = (IpTargetDataTrackerM*)pstTarget[j].stPrivateData.pDataTracker;

				if (!pstTardat->cs.s32Split)
					continue;
				if (pstTardat->cs.pstGroup != &pstTarsetdat->stGroupSet.astGroups[g])
					continue;
//				totalCoefficient = 0.0;

#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
				flag_split=IMP_TRUE;
#endif
				areaCoefficient  = ipCalculateAreaCoefficientM(pstDregion[i].s32AreaPixel,pstTardat->cs.sampleTardat.s32ObjArea);

#ifdef TRACKER_USE_RGB_INFO
				histCoefficient = ipCalculateHistoCoefficientRGB( drgdat->au32ObjectHistogram, pstTardat->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
			//	histCoefficient = CalculateHistoCoefficientMoments( drgdat, pstTardat);
#else
				histCoefficient = ipCalculateHistoCoefficient( drgdat->au32ObjectHistogram, pstTardat->au32ObjectHistogram, IMP_GRAY_LEVEL );
#endif

				{
					if(pstTardat->cs.pstGroup->s32UsedTotal==2)
					{

						{

							for(k=0;k<pstTardat->cs.pstGroup->s32UsedTotal;k++)
							{
								if(pstTarget[j].u32TargetId == pstTardat->cs.pstGroup->au32TargetIds[k])
								{
									tmptarget = pstTardat->cs.pstGroup->pastTargets[(k+1)%pstTardat->cs.pstGroup->s32UsedTotal];//获取下一个组员地址
									break;
								}
							}


							for( k=0; k<IMP_MAX_TGT_CNT; k++ )
							{
								if(tmptarget->u32TargetId == pstTarget[k].u32TargetId)
									break;
							}
							target_index=j;
							tmptarget_index=k;

							tmptardat =  (IpTargetDataTrackerM*)tmptarget->stPrivateData.pDataTracker;
							targetsAreaRate  = ipCalculateAreaCoefficientM(pstTardat->cs.sampleTardat.s32ObjArea,tmptardat->cs.sampleTardat.s32ObjArea);
#ifdef TRACKER_USE_RGB_INFO
							targetsHistRate  = ipCalculateHistoCoefficientRGB( pstTardat->au32ObjectHistogram, tmptardat->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
#else
							targetsHistRate  = ipCalculateHistoCoefficient( pstTardat->au32ObjectHistogram, tmptardat->au32ObjectHistogram, IMP_GRAY_LEVEL );
#endif

							for( k=1; k<(IMP_MAX_TGT_CNT-1); k++ )
							{
								if( !pstDregion[k].u8Used )
									continue;

								tmpdrgdat = (IpDrgDataTrackerM*)pstDregion[k].stPrivateData.pDataTracker;
								if (tmpdrgdat->s32Split)
								{
									if( k != i && tmpdrgdat->pstGroup==drgdat->pstGroup)
										break;
								}
							}
							region_index=i;
							tmpregion_index=k;
							regionsAreaRate = ipCalculateAreaCoefficientM(pstDregion[i].s32AreaPixel,pstDregion[k].s32AreaPixel);
#ifdef TRACKER_USE_RGB_INFO
							regionsHistRate = ipCalculateHistoCoefficientRGB(drgdat->au32ObjectHistogram,tmpdrgdat->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
#else
							regionsHistRate = ipCalculateHistoCoefficient(drgdat->au32ObjectHistogram,tmpdrgdat->au32ObjectHistogram, IMP_GRAY_LEVEL );
#endif
							//info2 += sprintf( info2, "(M%01d,M%01d)  RAR:%.3f  RHR:%.3f",i,k,regionsAreaRate,regionsHistRate);//for display debug information


							deltaAreaRate = (IMP_FLOAT)fabs(regionsAreaRate-targetsAreaRate);
							deltaHistRate = (IMP_FLOAT)fabs(regionsHistRate-targetsHistRate);

						}


						{
							if (targetsAreaRate < 0.5 && deltaAreaRate < 0.4)
								flag_use_areaFeature = IMP_TRUE;
							else
								flag_use_areaFeature = IMP_FALSE;
						}


						{
							if(ipCsHistFeatureValidJudge(drgdat,tmpdrgdat,pstTardat,tmptardat))
								flag_use_histFeature = IMP_TRUE;
							else
								flag_use_histFeature = IMP_FALSE;
						}


						{
							alpha = abs(pstTardat->cs.sampleTardat.s32Direction-tmptardat->cs.sampleTardat.s32Direction);
							if(alpha > 180)
								alpha = 360-alpha;


							if(

								(

									 (
										   (targetsAreaRate>0.85 && targetsHistRate>0.75 &&							deltaHistRate>0.2 )	 //面积、直方图相似 + 直方图变化剧烈
										 ||(targetsAreaRate>0.9  && targetsHistRate>0.8  && regionsHistRate>0.8						  )  //面积、直方图相似
										 ||(						targetsHistRate>0.9      										  )	 //直方图过于相似
										 ||(																		deltaHistRate>0.25)	 //直方图变化过于剧烈
									 )
								 &&  1
								 &&  alpha
								 &&  pstTardat->cs.s32CollisionLength < 50
								)
 							 )
							{
								flag_use_trendFeature = IMP_TRUE;
							}
							else
							{
								flag_use_trendFeature = IMP_FALSE;
							}
						}
					}
				}


				{
					if(pstTardat->cs.pstGroup->s32UsedTotal>2)
					{
#ifdef	TRACKER_USE_RGB_INFO
						histCoefficient = ipCalculateHistoCoefficientMoments( drgdat, pstTardat);
#else
						histCoefficient = ipCalculateHistoCoefficient(drgdat->au32ObjectHistogram, pstTardat->au32ObjectHistogram, IMP_GRAY_LEVEL );
#endif
						flag_use_histFeature = IMP_TRUE;


						if (flag_use_sameAreaModel)
						{
							cntMultiSplit++;
							areaCoefficient = (IMP_FLOAT)(1.0/pstDregion[i].s32AreaPixel);
							if (areaCoefficient>maxAreaCoef)
							{
								maxAreaCoef = areaCoefficient;
								maxDrgId = i;
							}

							if(cntMultiSplit==pstTardat->cs.pstGroup->s32UsedTotal*2)
							{
								pstTardat->cs.pstGroup->s32DrgIdforSingleTgtInMultiSplit=maxDrgId;
							}

						}


						else
						{
							cntMultiSplit++;

							areaCoefPlus=areaCoefficient;
							areaCoefMinus=ipCalculateAreaCoefMinus(pstTardat->cs.sampleTardat.s32ObjArea,pstDregion[i].s32AreaPixel,&pstTarsetdat->stGroupSet.astGroups[g]);			//反阵
							areaCoefAvg = (IMP_FLOAT)((areaCoefPlus+areaCoefMinus)*0.5);
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
							info2 += sprintf( info2, "SS(m:%02d,t:%02d)%.4f,%.4f,%.4f",i,j,areaCoefPlus,areaCoefMinus,areaCoefAvg);//for display debug information
#endif
							if (areaCoefAvg>maxAreaCoef)
							{
								maxAreaCoef = areaCoefAvg;
								maxDrgId = i;
							}

							if(cntMultiSplit==pstTardat->cs.pstGroup->s32UsedTotal*2)
							{
								pstTardat->cs.pstGroup->s32DrgIdforSingleTgtInMultiSplit=maxDrgId;
							}
						}
					}
				}


				{

					if( flag_use_areaFeature && pstTardat->cs.pstGroup->s32UsedTotal==2 )
					{
						pstTracker->afMatchCoeff[j][i] = areaCoefficient;
					}

					else if ( flag_use_histFeature )
					{
						pstTracker->afMatchCoeff[j][i] = histCoefficient;
					}

					else if ( flag_use_trendFeature && pstTardat->cs.pstGroup->s32UsedTotal==2)
					{
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
//						Beep(1000,100);
#endif

						ipCalculateTrendCoefficient(pstTardat->cs.sampleTardat.stCentroid,target_index,
												  tmptardat->cs.sampleTardat.stCentroid,tmptarget_index,
												  drgdat->stCurCentroid, region_index,
												  tmpdrgdat->stCurCentroid,tmpregion_index,
												  pstTardat->cs.sampleTardat.s32Direction,tmptardat->cs.sampleTardat.s32Direction,
												  pstTracker->afMatchCoeff);

					}

					else
					{
						if (pstTardat->cs.pstGroup->s32UsedTotal==2)
						{
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
//						Beep(2000,100);
#endif
							switch(pstTracker->pstPara->s32CsSplitDefaultFeature)
							{
								case 0:
								{
									pstTracker->afMatchCoeff[j][i] = histCoefficient;
									break;
								}
								case 1:
								{

									ipCalculateTrendCoefficient(pstTardat->cs.sampleTardat.stCentroid,target_index,
														  tmptardat->cs.sampleTardat.stCentroid,tmptarget_index,
														  drgdat->stCurCentroid, region_index,
														  tmpdrgdat->stCurCentroid,tmpregion_index,
														  pstTardat->cs.sampleTardat.s32Direction,tmptardat->cs.sampleTardat.s32Direction,
 														  pstTracker->afMatchCoeff);
									break;
								}
								case 2:
								{

									ipCalculateHistoCoefficientBalance(drgdat,region_index,
																	 tmpdrgdat,tmpregion_index,
																	 pstTardat,target_index,
																	 tmptardat,tmptarget_index,
																	 pstTracker->afMatchCoeff);
									break;
								}
								default:
								{
									pstTracker->afMatchCoeff[j][i] = histCoefficient;
									break;
								}
							}
						}
					}

#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
					info2 += sprintf( info2, "(m:%02d,t:%02d)_%02d %.4f   ",i,j,pstTarget[j].u32TargetId-0x90000000,pstTracker->afMatchCoeff[j][i]);//for display debug information
#endif
				}
			}
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
			if (flag_split)						//for display debug information
			{
				info2 += sprintf( info2, "\n");
				flag_split=IMP_FALSE;
			}
#endif
		}//the end of FOR i
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
//		info2 += sprintf( info2, "G:%01d  SingleTGTSplit(bestM:%02d)%.4f\n",g,maxDrgId,maxAreaCoef);//for display debug information
//
//		info2 += sprintf( info2, "     TargetAreaRate = %.3f  TargetHistRate =  %.3f  DeltaA:%0.3f DeltaH:%0.3f\n",targetsAreaRate,targetsHistRate,(regionsAreaRate-targetsAreaRate),(regionsHistRate-targetsHistRate));//for display debug information
//		info2 += sprintf( info2, "     RegionAreaRate = %.3f  RegionHistRate =  %.3f  \n",regionsAreaRate,regionsHistRate);//for display debug information
#endif
	}//the end of FOR g
}


IMP_VOID ipMatchRegionToSplitObject(IpTargetTrackerMotion *pstTracker)
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDregion = pstDRegionSet->astDrg;
	IpTargetDataTrackerM *pstTardat;
	IpDrgDataTrackerM *drgdat;
	IpTargetSetDataTrackerM *pstTarsetdat;
	IMP_S32 indexM, indexN, maxIndexN=0, tempIndexM,tempIndexN, cnt;
	IMP_FLOAT maxCoef;
	IMP_S32 loopTimes;
	IMP_U32 maxCoefMatrix[IMP_MAX_TGT_CNT][IMP_MAX_TGT_CNT];
	IMP_S32 flag_max_exist=IMP_FALSE;
	IMP_S32 flag_multiTarget_split=IMP_FALSE;
	IMP_S32 i;


	pstTarsetdat = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;


	for (i=0;i<pstTarsetdat->stGroupSet.s32UsedTotal;i++)
	{

		loopTimes=2;
Loop:

		flag_multiTarget_split = IMP_FALSE;
		for( indexM=0; indexM<IMP_MAX_TGT_CNT; indexM++ )
		{
			for ( indexN=0; indexN<IMP_MAX_TGT_CNT; indexN++ )
			{
				maxCoefMatrix[indexN][indexM]=0;
			}
		}


		{

			for( indexM=1, cnt=0; indexM<(IMP_MAX_TGT_CNT-1) && cnt<pstDRegionSet->s32UsedTotal; indexM++ )
			{
				if( !pstDregion[indexM].u8Used )
					continue;
				cnt++;

				drgdat = (IpDrgDataTrackerM*)pstDregion[indexM].stPrivateData.pDataTracker;

				if (! drgdat->s32Split )
					continue;
				if (drgdat->pstGroup != &pstTarsetdat->stGroupSet.astGroups[i])
					continue;

				maxCoef = 0;
				for ( indexN=0; indexN<IMP_MAX_TGT_CNT; indexN++ )
				{
					pstTardat = (IpTargetDataTrackerM*)pstTarget[indexN].stPrivateData.pDataTracker;
					if (! pstTardat->cs.s32Split )
						continue;
					if (pstTardat->cs.pstGroup != &pstTarsetdat->stGroupSet.astGroups[i]) //目标不属于该组，不计算
						continue;
					if ( pstTracker->afMatchCoeff[indexN][indexM] > maxCoef )
					{
						tempIndexM = indexM;
						tempIndexN = indexN;
						maxCoef = pstTracker->afMatchCoeff[indexN][indexM];
						flag_max_exist = IMP_TRUE;
					}
				}
				if(flag_max_exist)
				{
					maxCoefMatrix[tempIndexN][tempIndexM]++;
					flag_max_exist = IMP_FALSE;
				}
			}


			flag_max_exist = IMP_FALSE;
			for ( indexN=0; indexN<IMP_MAX_TGT_CNT; indexN++ )
			{
				pstTardat = (IpTargetDataTrackerM*)pstTarget[indexN].stPrivateData.pDataTracker;
				if (! pstTardat->cs.s32Split )
					continue;

				if (pstTardat->cs.pstGroup != &pstTarsetdat->stGroupSet.astGroups[i])
					continue;

				maxCoef = 0;
				for( indexM=1, cnt=0; indexM<(IMP_MAX_TGT_CNT-1) && cnt<pstDRegionSet->s32UsedTotal; indexM++ )
				{
					if( !pstDregion[indexM].u8Used )
						continue;
					cnt++;
					drgdat = (IpDrgDataTrackerM*)pstDregion[indexM].stPrivateData.pDataTracker;

					if (! drgdat->s32Split )
						continue;

					if (drgdat->pstGroup != &pstTarsetdat->stGroupSet.astGroups[i])
						continue;

					if ( pstTracker->afMatchCoeff[indexN][indexM] > maxCoef )
					{
						tempIndexM = indexM;
						tempIndexN = indexN;
						maxCoef = pstTracker->afMatchCoeff[indexN][indexM];
						flag_max_exist = IMP_TRUE;
					}
				}
				if(flag_max_exist)
				{
					maxCoefMatrix[tempIndexN][tempIndexM]++;
					flag_max_exist = IMP_FALSE;
				}
			}
		}


		maxCoef = 0;
		for( indexM=1, cnt=0; indexM<(IMP_MAX_TGT_CNT-1) && cnt<pstDRegionSet->s32UsedTotal; indexM++ )
		{
			if( !pstDregion[indexM].u8Used )
				continue;
			cnt++;

			drgdat = (IpDrgDataTrackerM*)pstDregion[indexM].stPrivateData.pDataTracker;
//			drgdat->s32MatchedWith = 0;

			if (! drgdat->s32Split )
				continue;

			if (drgdat->pstGroup != &pstTarsetdat->stGroupSet.astGroups[i])
				continue;

			for ( indexN=0; indexN<IMP_MAX_TGT_CNT; indexN++ )
			{
				pstTardat = (IpTargetDataTrackerM*)pstTarget[indexN].stPrivateData.pDataTracker;
				if (! pstTardat->cs.s32Split )
					continue;
				if (pstTardat->cs.pstGroup != &pstTarsetdat->stGroupSet.astGroups[i])
					continue;



				if (pstTardat->cs.pstGroup->s32UsedTotal>2 && pstTracker->pstPara->s32CsMultiTargetSplit)
				{

					if (drgdat->pstGroup != &pstTarsetdat->stGroupSet.astGroups[i])
						continue;

					if (indexM != pstTarsetdat->stGroupSet.astGroups[i].s32DrgIdforSingleTgtInMultiSplit)
						continue;
//					if ( maxCoefMatrix[indexN][indexM] != 2 )
//						continue;


					if (pstTracker->afMatchCoeff[indexN][indexM]>maxCoef)
					{
						maxCoef =  pstTracker->afMatchCoeff[indexN][indexM];
						tempIndexN = indexN;
						tempIndexM = indexM;
						flag_multiTarget_split = IMP_TRUE;
					}
				}


				else
				{
					if ( maxCoefMatrix[indexN][indexM] == 2 )

					{
						maxCoef = pstTracker->afMatchCoeff[indexN][indexM];
						maxIndexN = indexN;
						drgdat->s32MatchedWith = maxIndexN;

						pstTardat = (IpTargetDataTrackerM*)pstTarget[maxIndexN].stPrivateData.pDataTracker;
						pstTardat->s32MatchedWith = indexM;
						pstTardat->fMatchCoefficient = maxCoef;
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
						info2 += sprintf( info2, "(N:%02d,M:%02d)_C:%.3f_%02d\n",indexN,indexM,maxCoef,pstTarget[maxIndexN].u32TargetId-0x90000000);
#endif
						for ( tempIndexN=0; tempIndexN<IMP_MAX_TGT_CNT; tempIndexN++)
						{
							pstTracker->afMatchCoeff[tempIndexN][indexM]=0;
						}

						for( tempIndexM=1; tempIndexM<(IMP_MAX_TGT_CNT-1); tempIndexM++ )
						{
							pstTracker->afMatchCoeff[maxIndexN][tempIndexM]=0;
						}
						maxCoefMatrix[indexN][indexM] = 0;
						continue;

					}//The end of if ( maxCoefMatrix[indexN][indexM] == 2 )
				}//The end of else
			}
		}//the end of


		{

			if (flag_multiTarget_split)
			{

				///////////////////////////////////////////////////////////////
				{
					IMP_S32 k;
					IpGroup * pstGroup;

					drgdat = (IpDrgDataTrackerM*)pstDregion[tempIndexM].stPrivateData.pDataTracker;
					pstTardat = (IpTargetDataTrackerM*)pstTarget[tempIndexN].stPrivateData.pDataTracker;
					pstGroup = pstTardat->cs.pstGroup;


					for (k=0;k<pstGroup->s32UsedTotal;k++)
					{
						pstTardat = (IpTargetDataTrackerM*)pstGroup->pastTargets[k]->stPrivateData.pDataTracker;
						if (pstTarget[tempIndexN].u32TargetId==pstGroup->au32TargetIds[k])
						{

							pstTardat->cs.pstGroup->s32ToBeDeleted[k]=IMP_TRUE;


							drgdat->s32MatchedWith = tempIndexN;
							pstTardat->s32MatchedWith = tempIndexM;
							pstTardat->fMatchCoefficient = maxCoef;
							drgdat->s32Collision = IMP_FALSE;
							drgdat->s32Split = IMP_TRUE;
							pstTardat->cs.s32CurCollision = IMP_FALSE;
							pstTardat->cs.s32Split = IMP_TRUE;
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
							info2 += sprintf( info2, "multiSplit N-M (N:%02d,M:%02d) C:%.3f %02d\n",tempIndexN,tempIndexM,maxCoef,pstTarget[tempIndexN].u32TargetId-0x90000000);
#endif
						}
						else
						{

							pstTardat->cs.pstGroup->s32ToBeDeleted[k]=IMP_FALSE;

							pstTardat->cs.s32CurCollision = IMP_TRUE;
							pstTardat->cs.s32Split = IMP_FALSE;
						}

					}
					for(indexM=1, cnt=0; indexM<(IMP_MAX_TGT_CNT-1) && cnt<pstDRegionSet->s32UsedTotal; indexM++ )
					{
						if( !pstDregion[indexM].u8Used )
							continue;
						cnt++;
			 			drgdat = (IpDrgDataTrackerM*)pstDregion[indexM].stPrivateData.pDataTracker;

						if (! drgdat->s32Split )
							continue;
						if (drgdat->pstGroup != pstGroup)
							continue;

						if (drgdat->s32MatchedWith==IMP_MATCH_GROUP)
						{
							for (k=0;k<pstGroup->s32UsedTotal;k++)
							{
								pstTardat = (IpTargetDataTrackerM*)pstGroup->pastTargets[k]->stPrivateData.pDataTracker;
								if(pstTardat->cs.s32Split)
									continue;
								pstTardat->s32MatchedWith = indexM;
							}

							drgdat->s32Collision=IMP_TRUE;
							drgdat->s32Split=IMP_FALSE;
						}
#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD
						info2 += sprintf( info2, "multiSplit Region (M:%02d) C:%01d S:%01d\n",indexM,drgdat->s32Collision,drgdat->s32Split);
#endif
					}//the end of For

#ifdef DEBUG_IVP_OBJRECG_TRACKER_INFO2_OLD

					for (k=0;k<pstGroup->s32UsedTotal;k++)
					{
						pstTardat = (IpTargetDataTrackerM*)pstGroup->pastTargets[k]->stPrivateData.pDataTracker;
						for ( indexN=0; indexN<IMP_MAX_TGT_CNT; indexN++ )
						{
							if (pstGroup->au32TargetIds[k]==pstTarget[indexN].u32TargetId)
							{
								info2 += sprintf( info2, "multiSplit Target (N:%02d) C:%01d S:%01d %02d matchwith:%02d\n",indexN,pstTardat->cs.s32CurCollision,pstTardat->cs.s32Split,pstTarget[indexN].u32TargetId-0x90000000,pstTardat->s32MatchedWith);
							}
						}
					}//the end of for (k=0;k<pstGroup->s32UsedTotal;k++)
#endif
				}//the end of
				///////////////////////////////////////////////////////////////
			}

			else if( (--loopTimes) > 0 )
			{
				 goto Loop;
			}
		}//the end

	}//the end of
}



IMP_S32 ipCsHistFeatureValidJudge(IpDrgDataTrackerM *pstDrgDat1,IpDrgDataTrackerM *pstDrgDat2,IpTargetDataTrackerM *pstTarData1,IpTargetDataTrackerM *pstTarData2)
{
	IMP_FLOAT s32a[4];
	IMP_FLOAT fMax1, fMax2;
	IMP_S32 i;
	IMP_S32 s32Index1, s32Index2;

#ifdef TRACKER_USE_RGB_INFO
	s32a[0] = ipCalculateHistoCoefficientRGB(pstDrgDat1->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
	s32a[1] = ipCalculateHistoCoefficientRGB(pstDrgDat2->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
	s32a[2] = ipCalculateHistoCoefficientRGB(pstDrgDat1->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
	s32a[3] = ipCalculateHistoCoefficientRGB(pstDrgDat2->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
#else
	s32a[0] = ipCalculateHistoCoefficient(pstDrgDat1->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_GRAY_LEVEL );
	s32a[1] = ipCalculateHistoCoefficient(pstDrgDat2->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_GRAY_LEVEL );
	s32a[2] = ipCalculateHistoCoefficient(pstDrgDat1->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_GRAY_LEVEL );
	s32a[3] = ipCalculateHistoCoefficient(pstDrgDat2->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_GRAY_LEVEL );
#endif

	fMax1 = s32a[0];
	s32Index1 = 0;
	for (i = 1; i < 4; i++)
	{
	   if (s32a[i] > fMax1)
	   {
		   fMax1 = s32a[i];
		   s32Index1 = i;
	   }
	}

	s32a[s32Index1] = 0.0;
	fMax2 = s32a[0];
	s32Index2 = 0;
	for (i = 1; i < 4;i++)
	{
	   if (s32a[i] > fMax2)
	   {
		   fMax2 = s32a[i];
		   s32Index2 = i;
	   }
	}

	if(s32Index1 + s32Index2 == 3)
		return IMP_TRUE;
	else
		return IMP_FALSE;
}

IMP_VOID ipCalculateHistoCoefficientBalance(IpDrgDataTrackerM *pstDrgDat1,IMP_S32 s32IdxM1,
											 IpDrgDataTrackerM *pstDrgDat2,IMP_S32 s32IdxM2,
											 IpTargetDataTrackerM *pstTarData1,IMP_S32 s32IdxT1,
											 IpTargetDataTrackerM *pstTarData2,IMP_S32 s32IdxT2,
											 IMP_FLOAT afMatchCoeff[][IMP_MAX_TGT_CNT])
{
	IMP_FLOAT s32a[4];
	IMP_FLOAT fTmp1, fTmp2;

#ifdef TRACKER_USE_RGB_INFO
	s32a[0] = ipCalculateHistoCoefficientRGB(pstDrgDat1->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
	s32a[1] = ipCalculateHistoCoefficientRGB(pstDrgDat2->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
	s32a[2] = ipCalculateHistoCoefficientRGB(pstDrgDat1->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
	s32a[3] = ipCalculateHistoCoefficientRGB(pstDrgDat2->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_RGB_LEVEL );//rgb
#else
	s32a[0] = ipCalculateHistoCoefficient(pstDrgDat1->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_GRAY_LEVEL );
	s32a[1] = ipCalculateHistoCoefficient(pstDrgDat2->au32ObjectHistogram,pstTarData1->au32ObjectHistogram, IMP_GRAY_LEVEL );
	s32a[2] = ipCalculateHistoCoefficient(pstDrgDat1->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_GRAY_LEVEL );
	s32a[3] = ipCalculateHistoCoefficient(pstDrgDat2->au32ObjectHistogram,pstTarData2->au32ObjectHistogram, IMP_GRAY_LEVEL );
#endif

	fTmp1 = (s32a[0] + s32a[3]) / 2;
	fTmp2 = (s32a[1] + s32a[2]) / 2;
	if(fTmp1 > fTmp2)
	{
		afMatchCoeff[s32IdxT1][s32IdxM1] = fTmp1;
		afMatchCoeff[s32IdxT2][s32IdxM2] = fTmp2;
		afMatchCoeff[s32IdxT1][s32IdxM2] = 0.0;
		afMatchCoeff[s32IdxT2][s32IdxM1] = 0.0;
	}
	else
	{
		afMatchCoeff[s32IdxT1][s32IdxM2] = fTmp1;
		afMatchCoeff[s32IdxT2][s32IdxM1] = fTmp2;
		afMatchCoeff[s32IdxT1][s32IdxM1] = 0.0;
		afMatchCoeff[s32IdxT2][s32IdxM2] = 0.0;
	}

}


#ifdef TRACKER_USE_RGB_INFO
static IMP_FLOAT ipCalculateHistoCoefficientMoments( IpDrgDataTrackerM *pstDrgData,  IpTargetDataTrackerM *pstTargetData)
{
	IMP_FLOAT *E1 = pstDrgData->E;
	IMP_FLOAT *E2 = pstTargetData->E;
	IMP_FLOAT *sigma1 = pstDrgData->sigma;
	IMP_FLOAT *sigma2 = pstTargetData->sigma;
	IMP_FLOAT *skewness1 = pstDrgData->skewness;
	IMP_FLOAT *skewness2 = pstTargetData->skewness;
	IMP_FLOAT w[3][3], p[3][3], fHistoCoef=0;
	IMP_S32 i,j;

	w[0][0]=1;	w[0][1]=1;	w[0][2]=1;
	w[1][0]=1;	w[1][1]=1;	w[1][2]=1;
	w[2][0]=1;	w[2][1]=1;	w[2][2]=1;

	p[0][0]=(IMP_FLOAT)fabs(E1[0]-E2[0]);				p[0][1]=(IMP_FLOAT)fabs(E1[1]-E2[1]);				p[0][2]=(IMP_FLOAT)fabs(E1[2]-E2[2]);
	p[1][0]=(IMP_FLOAT)fabs(sigma1[0]-sigma2[0]);		p[1][1]=(IMP_FLOAT)fabs(sigma1[1]-sigma2[1]);		p[1][2]=(IMP_FLOAT)fabs(sigma1[2]-sigma2[2]);
	p[2][0]=(IMP_FLOAT)fabs(skewness1[0]-skewness2[0]);	p[2][1]=(IMP_FLOAT)fabs(skewness1[1]-skewness2[1]);	p[2][2]=(IMP_FLOAT)fabs(skewness1[2]-skewness2[2]);


	for (j = 0; j < 3;j++)
	{
		for (i = 0; i < 3; i++)
		{
			fHistoCoef+= ( w[i][j] * p[i][j]);
		}
	}

	fHistoCoef = 9 / fHistoCoef;

	return fHistoCoef;
}
#endif

IMP_FLOAT ipCalculateAreaCoefMinus(IMP_S32 s32AreaT,IMP_S32 s32AreaD,IpGroup* pstGroup)
{
	IMP_S32 s32AreaTMinus = pstGroup->s32SumAreaTargets-s32AreaT;
	IMP_S32 s32AreaDMinus = pstGroup->s32SumAreaRegions-s32AreaD;

	return ipCalculateAreaCoefficientM(s32AreaDMinus, s32AreaTMinus);

}


IMP_FLOAT ipCalculateAreaCoefficientM( const IMP_S32 s32a, const IMP_S32 s32b )
{
	IMP_FLOAT fRatioAB;
	fRatioAB = s32a > s32b ? (((IMP_FLOAT)s32b) / s32a) : (((IMP_FLOAT)s32a) / s32b);
	return fRatioAB;
}


IMP_S32 ipCalculateAngleofVector(IMP_POINT_S stStart,IMP_POINT_S stEnd)
{
	IMP_FLOAT dx, dy, fAngle;
	IMP_S32 s32Quadrant= -1;

	dx = (IMP_FLOAT)stEnd.s16X - (IMP_FLOAT)stStart.s16X;
	dy = (IMP_FLOAT)stEnd.s16Y - (IMP_FLOAT)stStart.s16Y;

	if ( dx >= 0 )
	{
		if ( dy >= 0 )
			s32Quadrant = 1;
		else
			s32Quadrant = 4;
	}
	else
	{
		if ( dy >= 0 )
			s32Quadrant = 2;
		else
			s32Quadrant = 3;
	}
	if(fabs(dx) > 0.000001)
		fAngle = (IMP_FLOAT)atan(dy / dx);
	else
		fAngle = 0;
	switch (s32Quadrant)
	{
		case 1:
			break;
		case 2:
		case 3:
			fAngle += (IMP_FLOAT)IMP_PI;
			break;
		case 4:
			fAngle += (IMP_FLOAT)IMP_PI * 2;
			break;
		default:
			break;
	}
	if ( fAngle < 0 )
		fAngle += (IMP_FLOAT)IMP_PI * 2;

	return( (IMP_S32)(fAngle * 180 / IMP_PI));
}



IMP_VOID ipCalculateTrendCoefficient(IMP_POINT_S stPtT1,IMP_S32 s32IdxT1,
									  IMP_POINT_S stPtT2,IMP_S32 s32IdxT2,
									  IMP_POINT_S stPtM1,IMP_S32 s32IdxM1,
									  IMP_POINT_S stPtM2,IMP_S32 s32IdxM2,
									  IMP_S32 s32DirT1, IMP_S32 s32DirT2,
									  IMP_FLOAT afMatchCoeff[][IMP_MAX_TGT_CNT])
{
	IMP_S32 s32Alpha;
	IMP_S32 s32Beta;
	IMP_S32 s32Gamma;
	IMP_S32 s32Delta;
	IMP_FLOAT d1, d2;


	if (!( afMatchCoeff[s32IdxT1][s32IdxM1] > 0.000001 || afMatchCoeff[s32IdxT1][s32IdxM2] > 0.000001  )) //如果尚无计算结果
	{

		s32Alpha = abs(ipCalculateAngleofVector(stPtT1,stPtM1)-s32DirT1);
		if(s32Alpha > 180)
			s32Alpha = 360 - s32Alpha;

		s32Beta = abs(ipCalculateAngleofVector(stPtT2,stPtM2) - s32DirT2);
		if(s32Beta > 180)
			s32Beta = 360 - s32Beta;

		s32Gamma = abs(ipCalculateAngleofVector(stPtT1,stPtM2) - s32DirT1);
		if(s32Gamma> 180)		//s32Gamma ∈[0°,180°]
			s32Gamma = 360 - s32Gamma;

		s32Delta = abs(ipCalculateAngleofVector(stPtT2,stPtM1) - s32DirT2);
		if(s32Delta > 180)		//s32Delta ∈[0°,180°]
			s32Delta = 360 - s32Delta;

		d1 = (IMP_FLOAT)abs(s32Alpha + s32Beta);
		d2 = (IMP_FLOAT)abs(s32Gamma + s32Delta);



		if (d1 < d2)
		{
			afMatchCoeff[s32IdxT1][s32IdxM1] = 1 - d1 / 360;
			afMatchCoeff[s32IdxT2][s32IdxM2] = 1 - d2 / 360;
			afMatchCoeff[s32IdxT1][s32IdxM2] = 0.0;
			afMatchCoeff[s32IdxT2][s32IdxM1] = 0.0;
		}
		else
		{
			afMatchCoeff[s32IdxT1][s32IdxM2] = 1 - d1 / 360;
			afMatchCoeff[s32IdxT2][s32IdxM1] = 1 - d2 / 360;
			afMatchCoeff[s32IdxT1][s32IdxM1] = 0.0;
			afMatchCoeff[s32IdxT2][s32IdxM2] = 0.0;
		}
	}

}

IMP_VOID ipCalcuCollisionLength( IpTargetTrackerMotion *pstTracker )
{
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTargetSetDataTrackerM *pstTargetSetData = (IpTargetSetDataTrackerM*)pstTargetSet->stPrivateData.pDataTracker;
	IpTargetDataTrackerM *pstTargetData;
	IMP_S32 i,j;


	for (i=0;i<pstTargetSetData->stGroupSet.s32UsedTotal;i++)
	{

		for (j=0;j<pstTargetSetData->stGroupSet.astGroups[i].s32UsedTotal;j++)
		{
			pstTargetData = (IpTargetDataTrackerM*)pstTargetSetData->stGroupSet.astGroups[i].pastTargets[j]->stPrivateData.pDataTracker;
			if( pstTargetData->cs.s32CurCollision && !pstTargetData->cs.s32Split)
			{
				if (!pstTargetData->cs.s32LastCollision)
				{
					pstTargetData->cs.s32CollisionLength=1;
				}
				else
				{
					pstTargetData->cs.s32CollisionLength++;
				}
			}
		}
	}
}
