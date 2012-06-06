
#include "imp_avd_clarity.h"


void AVD_clarityAnalysis( AVD_MODULE* avdModule )
{
//	IMP_S32 ratio1,ratio3;
	IMP_S32 ratio2; //百分比
	IMP_S32 highVolume = 0;
	IMP_S32 para;
	IMP_S32 allVolume;
	IMP_S32 highThrd;
	IMP_S32 i;
//	AVD_HISTOGROUP* histImg = &avdModule->histGrayImgSmthEdge; //## 之后需要改成&avdModule->histEdgeImg;
	AVD_HISTOGROUP* histImg = &avdModule->histEdgeImg;
	IMP_S32* bin = histImg->histAll.bin;

	IMP_S32  widthCnt,heightCnt;
//	IMP_S32 imgW = avdModule->pRGBImg->width;
//	IMP_S32 imgH = avdModule->pRGBImg->height;
	IMP_U8 level = avdModule->Para.stClarityPara.u32AlarmLevel;
	IMP_U8 mode = avdModule->IR_mode;

	if (mode == IR_MODE)
	{
		para = 15;
		highThrd = AVD_CLARITY_EDGE_HT_IR;
	}
	else
	{
		para = 30;
		highThrd = AVD_CLARITY_EDGE_HT;
	}

	highThrd = max(highThrd,0);
	for (i=highThrd;i<IP_HIST_LEVEL;i++)
	{
		highVolume += bin[i];
	}
	allVolume = histImg->histAll.volume;


	widthCnt = 1;
	for (i=1;i<IP_HIST_LEVEL;i++)
	{
//		if(bin[i]<bin[i-1])
		if(bin[i] > 0)
			widthCnt++;
	}

	//统计高
	heightCnt = histImg->histAll.max;

	ratio2 = (IMP_S32)( ((float)heightCnt/(float)allVolume)/((float)widthCnt/(float)IP_HIST_LEVEL)  * para );

//	printf("widthCnt = %d\n",widthCnt);
//	printf("heightCnt = %d\n",heightCnt);


#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Clarity = %d\n",ratio2);
#endif
	avdModule->clarityValue = ratio2;
	avdModule->clarityValue = AVD_2ThTrans(	avdModule->Adv_Para.u8ClarityThrd[mode][level][TH1],
											avdModule->Adv_Para.u8ClarityThrd[mode][level][TH2],
											avdModule->clarityValue);
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Clarity'= %d\n",avdModule->clarityValue);
#endif
	return;
}
