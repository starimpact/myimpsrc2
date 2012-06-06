#include "imp_pea_tracker.h"



static FILE *pLogFile;
IMP_S32 ipOpenLogFile()
{
	IMP_S32 s32Ret = 0;
	pLogFile = fopen( "C:\\wh.txt", "w" );
	return s32Ret;
}

IMP_S32 ipCloseLogFile()
{
	IMP_S32 s32Ret = 0;
	fclose( pLogFile );
	pLogFile = NULL;
	return s32Ret;
}

IMP_S32 ipOutputTrackDataFrame(IpTargetTracker *pstTracker)
{
	IMP_S32  s32Ret = 0;
	IMP_S32 s32IndexN, i, s32Len;
	IpTargetPosition *pstPos;
	IpTrackedTargetSet *pstTargetSet = &pstTracker->pstResult->stTrackedTargetSet;
	PEA_DETECTED_REGIONSET_S *pstDRegionSet = &pstTracker->pstResult->stDRegionSet;
	IpTrackedTarget *pstTarget = pstTargetSet->astTargets;
	PEA_DETECTED_REGION_S *pstDRegion = pstDRegionSet->astDrg;
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	FILE *pLogFile = fopen( "c:\\imp_trk_frm_log.txt", "w" );

	for(s32IndexN = 0; s32IndexN < IMP_MAX_TGT_CNT; s32IndexN++)
	{
		PEA_DETECTED_REGION_S *pstDrg = NULL;
		IpTrackedTarget *pstTarget = &pstTarget[s32IndexN];
		if( pstTarget->stTargetImage.pstDRegions )
			pstDrg = pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex;
		pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
		s32Len = ipTargetTrajectoryGetLength(&(pstTarget->stTrajectory));
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), 0);
		i=0;
		if( ipTrackedTargetIsValid(&pstTarget[s32IndexN]) )
// 		if( ipTrackedTargetIsActiveMeasuredMtrend(&pstTarget[s32IndexN]) )
//  		if( TRACKED_TARGET_is_active_predicted_mtrend(&pstTarget[s32IndexN]) )
		{
			fprintf( pLogFile,
				"matchCoedf	=	%f\n"
				"s32ObjArea	=	%d\n"
				"s32Direction	=	%d\n"
				"s32Velocity	=	%d\n"
				"objectID	=	%d\n"
				"objStatus	=	%d\n"
				"minRect	=	(%d, %d, %d, %d)\n"
				"s32FirstTime	=	%d\n"
				"s32LastTime	=	%d\n"
				"pstDrg		=	%d\n",
				pstTargetData->fMatchCoefficient,
				pstTargetData->s32ObjArea,
				pstPos->s32Direction,
				pstPos->s32Velocity,
				pstTarget->u32TargetId&0x0FFFFFFF,
				pstTarget->stTargetInfo.u32Status,
				pstPos->stRg.s16X1, pstPos->stRg.s16Y1, pstPos->stRg.s16X2, pstPos->stRg.s16Y2,
				pstTargetData->s32FirstTime,
				pstTargetData->s32LastTime,
				(pstDrg ? pstDrg->u16Status : 0)
				);

			i = 0;
			fprintf(pLogFile, "drgtype:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->u32DrgType );
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "posx:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->stPt.s16X);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "posy:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->stPt.s16Y);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "w:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->stRg.s16X2-pstPos->stRg.s16X1);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "h:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->stRg.s16Y2-pstPos->stRg.s16Y1);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "area_p:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->u32AreaPixel);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "area_a:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->u32AreaActual);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "vel:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->s32Velocity);
				i++;
			}
			fprintf(pLogFile, "\n");

			i = 0;
			fprintf(pLogFile, "dir:\t");
			while(i<s32Len)
			{
				pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
				fprintf(pLogFile, "%d, ", pstPos->s32Direction);
				i++;
			}
			fprintf(pLogFile, "\n");

			fprintf(pLogFile, "\n");
		}
	}

	fclose( pLogFile );

	return s32Ret;
}

IMP_VOID ipPrintTarget( IpTargetTracker *pstTracker, IpTrackedTarget *pstTarget )
{
	IpTargetDataTrackerM *pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	PEA_DETECTED_REGION_S *pstDrg = NULL;
	IpDrgDataTrackerM *pstDrgData = NULL;
	IMP_S32 i, s32Len;
	IpTargetPosition *pstPos;
	FILE *pLogFile = fopen( "c:\\pstTarget.txt", "w" );

	if( pstTarget->stTargetImage.pstDRegions )
	{
		pstDrg = pstTarget->stTargetImage.pstDRegions->astDrg + pstTarget->stTargetImage.s32DrgIndex;
		pstDrgData = (IpDrgDataTrackerM*)pstDrg->stPrivateData.pDataTracker;
	}
	pstTargetData = (IpTargetDataTrackerM*)pstTarget->stPrivateData.pDataTracker;
	s32Len = ipTargetTrajectoryGetLength(&pstTarget->stTrajectory);
	pstPos = ipTargetTrajectoryGetPosition(&pstTarget->stTrajectory, 0);

	fprintf( pLogFile,
		"matchCoedf	=	%f\n"
		"s32ObjArea	=	%d\n"
		"s32Direction	=	%d\n"
		"s32Velocity	=	%d\n"
		"objectID	=	%d\n"
		"objStatus	=	%d\n"
		"minRect	=	(%d, %d, %d, %d)\n"
		"s32FirstTime	=	%d\n"
		"s32LastTime	=	%d\n"
		"maxdist	=	%d\n"
		"pstDrg		=	%d\n",
		pstTargetData->fMatchCoefficient,
		pstTargetData->s32ObjArea,
		pstPos->s32Direction,
		pstPos->s32Velocity,
		pstTarget->u32TargetId&0x0FFFFFFF,
		pstTarget->stTargetInfo.u32Status,
		pstPos->stRg.s16X1, pstPos->stRg.s16Y1, pstPos->stRg.s16X2, pstPos->stRg.s16Y2,
		pstTargetData->s32FirstTime,
		pstTargetData->s32LastTime,
		(pstDrg ? pstDrgData->s32MaxMatchDist : 0 ),
		(pstDrg ? pstDrg->u16Status : 0)
		);

	i = 0;
	fprintf(pLogFile, "drgtype:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->u32DrgType );
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "posx:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->stPt.s16X);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "posy:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->stPt.s16Y);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "w:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->stRg.s16X2-pstPos->stRg.s16X1);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "h:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->stRg.s16Y2-pstPos->stRg.s16Y1);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "area_p:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->u32AreaPixel);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "area_a:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->u32AreaActual);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "vel:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->s32Velocity);
		i++;
	}
	fprintf(pLogFile, "\n");

	i = 0;
	fprintf(pLogFile, "dir:\t");
	while(i<s32Len)
	{
		pstPos = ipTargetTrajectoryGetPosition(&(pstTarget->stTrajectory), -i);
		fprintf(pLogFile, "%d, ", pstPos->s32Direction);
		i++;
	}
	fprintf(pLogFile, "\n");

	fclose( pLogFile );
}
