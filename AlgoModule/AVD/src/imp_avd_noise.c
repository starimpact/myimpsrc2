
#include "imp_avd_noise.h"



void AVD_noiseAnalysis( AVD_MODULE* avdModule )
{
	IMP_S32 sum;
	IMP_S32 i;
//	IMP_S32 valU, valV;
	AVD_HISTOGROUP * pHistGrayImg = &avdModule->histGrayImg;
//	AVD_HISTOGROUP * pHistUImgDiff = &avdModule->histUImgDiff;
//	AVD_HISTOGROUP * pHistVImgDiff = &avdModule->histVImgDiff;
//	AVD_HISTOGROUP * pHistGrayImgDiff = &avdModule->histGrayImgDiff;
//	AVD_HISTOGROUP * pHistGrayImgDiff2 = &avdModule->histGrayImgDiff2;
	AVD_HISTOGROUP * pHistYImgDiffNxt = &avdModule->histYImgDiffNxt;
	AVD_HISTOGROUP * pHistUImgDiffNxt = &avdModule->histUImgDiffNxt;
	AVD_HISTOGROUP * pHistVImgDiffNxt = &avdModule->histVImgDiffNxt;

#ifdef DEBUG_AVD_SHOW_FRM_DIFF
//	AVD_HISTOGROUP * pHistGrayImgDiff3 = &avdModule->histGrayImgDiff3;
#endif

	IMP_S32 w = avdModule->YImg.s32W;
	IMP_S32 h = avdModule->YImg.s32H;
	IMP_S32 uvW = avdModule->UImg.s32W;
	IMP_S32 uvH = avdModule->UImg.s32H;
	IMP_S32 ratio1, ratio2, ratio3;
	float para1,para2,para3;
	IMP_U8 level = avdModule->Para.stNoisePara.u32AlarmLevel;
	IMP_U8 mode = avdModule->IR_mode;

	para1 = 0.5f; 
	para2 = 0.5f; 
	para3 = 0.0f;


	sum=0;
	for (i=0;i<AVD_PEPPER_TH;i++)
	{
		sum += pHistGrayImg->histAll.bin[i];
	}

	for (i=AVD_SALT_TH;i<IP_HIST_LEVEL;i++)
	{
		sum += pHistGrayImg->histAll.bin[i];
	}
	ratio1 = sum * 100 /(w*h);


	sum=0;
	for (i=AVD_NOISE_TH;i<IP_HIST_LEVEL;i++)
	{
		sum += pHistUImgDiffNxt->histAll.bin[i];
		sum += pHistVImgDiffNxt->histAll.bin[i];
	}
	ratio2 = sum * 2*100 /(uvW*uvH);



	sum=0;
	for (i=AVD_NOISE_TH2;i<IP_HIST_LEVEL;i++)
	{
		sum += pHistYImgDiffNxt->histAll.bin[i];
	}
	ratio3 = sum * 100 * 3 /(w*h);


	//////////////////////////////////////////////////////////////////////////
#ifdef DEBUG_AVD_SHOW_FRM_DIFF
//	assert(sum == pHistGrayImgDiff3->histAll.bin[31]);
#endif
#ifdef DEBUG_AVD_SHOW_PRINTF
//	printf("Diff:%d,Diff2:%d\n",pHistGrayImgDiff->histAll.bin[IP_HIST_LEVEL-1],pHistGrayImgDiff2->histAll.bin[IP_HIST_LEVEL-1]);
	printf("Noise = %d\n",ratio2+ratio3);
#endif
	avdModule->noiseValue = ratio2+ratio3;//*para1 + ratio2*para2;

	avdModule->noiseValue = AVD_2ThTrans( avdModule->Adv_Para.u8NoiseThrd[mode][level][TH1],
										  avdModule->Adv_Para.u8NoiseThrd[mode][level][TH2],
										  avdModule->noiseValue);
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Noise'= %d\n",avdModule->noiseValue);
#endif
	return;
}
