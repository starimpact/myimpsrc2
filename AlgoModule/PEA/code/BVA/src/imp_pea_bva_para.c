#include "imp_pea_bva_para.h"
#include "imp_pea_system.h"

IMP_VOID ipParseAnaystParaData(IpAnalystPara *pstPara )
{
	//perimeter parameter
	pstPara->stPerimeterPara.s32PerimeterUseMtrend = IMP_ANALYST_PERIMETER_USE_MTREND;
	pstPara->stPerimeterPara.s32PerimeterUseBottom = IMP_ANALYST_PERIMETER_USE_BOTTOM;
	pstPara->stPerimeterPara.s32PerimeterRatio = IMP_ANALYST_PERIMETER_RATIO;
	pstPara->stPerimeterPara.s32PerimeterDelicacy = IMP_ANALYST_PERIMETER_DELICACY;
	pstPara->stPerimeterPara.s32PerimeterAnalysisLength = IMP_ANALYST_PERIMETER_ANALYLEN;
	pstPara->stPerimeterPara.s32PerimeterValidRatio = IMP_ANALYST_PERIMETER_VALID_RATIO;

	pstPara->stPerimeterPara.s32PerimeterIntrusionRatio =IMP_ANALYST_PERIMETER_INTRUSION_RATIO;
	pstPara->stPerimeterPara.s32PerimeterEnterRatio = IMP_ANALYST_PERIMETER_ENTER_RATIO;
	pstPara->stPerimeterPara.s32PerimeterExitRatio = IMP_ANALYST_PERIMETER_EXIT_RATIO;
	pstPara->stPerimeterPara.s32PerimeterAppearRatio = IMP_ANALYST_PERIMETER_APPEAR_RATIO;
	pstPara->stPerimeterPara.s32PerimeterDisppearRatio = IMP_ANALYST_PERIMETER_DISAPPEAR_RATIO;
	pstPara->stPerimeterPara.s32PerimeterDisppearAlarmTime = IMP_ANALYST_PERIMETER_DISAPPEAR_ALARM_TIME;
	pstPara->stPerimeterPara.s32AngleTolerance = IMP_ANALYST_PERIMETER_TOLERANCE;

	//tripwire parameter
	pstPara->stTripwirePara.s32TripwireUseMtrend = IMP_ANALYST_TRIPWIRE_USE_MTREND;
	pstPara->stTripwirePara.s32UseBottom = IMP_ANALYST_TRIPWIRE_USE_BOTTOM;
	pstPara->stTripwirePara.s32UseObjectTrend = IMP_ANALYST_TRIPWIRE_USE_OBJECT_TREND;
	pstPara->stTripwirePara.s32AnalysisLength = IMP_ANALYST_TRIPWIRE_ANALYST_LEN;
	pstPara->stTripwirePara.s32ValidRatio = IMP_ANALYST_TRIPWIRE_VALID_RATIO;
	pstPara->stTripwirePara.s32ToleranceAngle = IMP_ANALYST_TRIPWIRE_TOLERANCE_ANGLE;
	pstPara->stTripwirePara.s32AwayDistance = IMP_ANALYST_TRIPWIRE_AWAY_DISTANCE;
	pstPara->stTripwirePara.s32SegLength = IMP_ANALYST_TRIPWIRE_SEG_LEN;

	//multiple tripwire parameter
	pstPara->stMTripwirePara.astSTripwire[0].s32TripwireUseMtrend = IMP_ANALYST_MTRIPWIRE_USE_MTREND1;
	pstPara->stMTripwirePara.astSTripwire[0].s32UseBottom = IMP_ANALYST_MTRIPWIRE_USE_BOTTOM1;
	pstPara->stMTripwirePara.astSTripwire[0].s32UseObjectTrend = IMP_ANALYST_MTRIPWIRE_USE_OBJECT_TREND1;
	pstPara->stMTripwirePara.astSTripwire[0].s32AnalysisLength = IMP_ANALYST_MTRIPWIRE_ANALYST_LEN1;
	pstPara->stMTripwirePara.astSTripwire[0].s32ValidRatio = IMP_ANALYST_MTRIPWIRE_VALID_RATIO1;
	pstPara->stMTripwirePara.astSTripwire[0].s32ToleranceAngle = IMP_ANALYST_MTRIPWIRE_TOLERANCE_ANGLE1;
	pstPara->stMTripwirePara.astSTripwire[0].s32AwayDistance = IMP_ANALYST_MTRIPWIRE_AWAY_DISTANCE1;
	pstPara->stMTripwirePara.astSTripwire[0].s32SegLength = IMP_ANALYST_MTRIPWIRE_SEG_LEN1;

	pstPara->stMTripwirePara.astSTripwire[1].s32TripwireUseMtrend = IMP_ANALYST_MTRIPWIRE_USE_MTREND2;
	pstPara->stMTripwirePara.astSTripwire[1].s32UseBottom = IMP_ANALYST_MTRIPWIRE_USE_BOTTOM2;
	pstPara->stMTripwirePara.astSTripwire[1].s32UseObjectTrend = IMP_ANALYST_MTRIPWIRE_USE_OBJECT_TREND2;
	pstPara->stMTripwirePara.astSTripwire[1].s32AnalysisLength = IMP_ANALYST_MTRIPWIRE_ANALYST_LEN2;
	pstPara->stMTripwirePara.astSTripwire[1].s32ValidRatio = IMP_ANALYST_MTRIPWIRE_VALID_RATIO2;
	pstPara->stMTripwirePara.astSTripwire[1].s32ToleranceAngle = IMP_ANALYST_MTRIPWIRE_TOLERANCE_ANGLE2;
	pstPara->stMTripwirePara.astSTripwire[1].s32AwayDistance = IMP_ANALYST_MTRIPWIRE_AWAY_DISTANCE2;
	pstPara->stMTripwirePara.astSTripwire[1].s32SegLength = IMP_ANALYST_MTRIPWIRE_SEG_LEN2;

	//OSC parameter
	pstPara->stOscPara.s32InitTime = 5000;
	pstPara->stOscPara.s32InitBgEdgeRatioTh = 50;
	pstPara->stOscPara.s32AddWeight = 1;
	pstPara->stOscPara.s32SubWeight = 3;
	pstPara->stOscPara.s32LeftAccumLevel = 8;
	pstPara->stOscPara.s32RemovedAccumLevel = 8;
	pstPara->stOscPara.s32MinArea = 15;
	pstPara->stOscPara.s32MinConnectivity = 2;
	pstPara->stOscPara.s32BgUpdateTime = IMP_ANALYST_BG_UPDATE_TIME;
	pstPara->stOscPara.s32LeaveLimit = IMP_ANALYST_LEAVE_LIMIT;
	pstPara->stOscPara.s32UseBorderConstrain = 1;
	pstPara->stOscPara.s32BorderWidth = 8;
	pstPara->stOscPara.s32DoSpecialRegionProcess = IMP_ANALYST_DO_SPCL_RG_PROCESS;
	pstPara->stOscPara.s32SpclMinTime = IMP_ANALYST_SPCL_MIN_TIME;
	pstPara->stOscPara.s32TrgnTrajectLen = 25;
	pstPara->stOscPara.s32TrgnTrajectRatio = 90;


	pstPara->s32InitEventLife = IMP_ANALYST_INIT_EVENT_LIFE;
	pstPara->s32OutputEvents = IMP_ANALYST_OUTPUT_EVENT;


	pstPara->s32FrmDura = IMP_ANALYST_FRM_DURA;
	pstPara->s32ObjectAreaRatio = IMP_ANALYST_OBJECT_AREA_RATIO;
}

