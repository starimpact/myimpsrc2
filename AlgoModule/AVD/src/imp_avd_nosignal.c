
#include "imp_avd_nosignal.h"



void AVD_noSignalAnalysis( AVD_MODULE* avdModule )
{
	IMP_S32 ratio;
	IMP_S32 max = avdModule->histGrayImg.histAll.max;
	IMP_S32 volume = avdModule->histGrayImg.histAll.volume;
	IMP_S32 level = avdModule->Para.stNosignalPara.u32AlarmLevel;
	IMP_S32 percent = AVD_NOSIGNAL_H-(AVD_NOSIGNAL_H-AVD_NOSIGNAL_L)/4*(avdModule->Para.stNosignalPara.u32AlarmLevel-1);
	static IMP_S32 cnt=0;

	if (level == USER_LEVEL)
	{
		percent = AVD_NOSIGNAL_L;
	}

	if(avdModule->flag_change)
	{
		cnt = 0;
	}

	if( max * 100 > percent* volume  )			
		cnt++;
	else
		cnt = 0;

	if ( cnt >= AVD_NOSIGNAL_MIN_CNT )			
		ratio = 100;
	else
		ratio = 0;



	avdModule->signalValue = ratio;
	return;
}
