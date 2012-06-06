
#include "imp_avd_color.h"



#define R_CHANNAL	0
#define G_CHANNAL	1
#define B_CHANNAL	2

#define LOW_SEC		0
#define MID_SEC		1
#define HIGH_SEC	2

enum
{
	B_CAST=0,
	G_CAST,
	R_CAST,
	WHITE_BALANCE
};

void AVD_colorAnalysis( AVD_MODULE* avdModule )
{
	IMP_S32 sectionVolume[3][3];   // [通道][亮度]  通道：R/G/B；亮度：低/中/高
	IMP_S32 lowThrd,highThrd;
//	IpHistogram histMin;
	IMP_S32 ratio1;
//	IMP_S32 ratio2,ratio4;
	IMP_S32 ratio3;
	IMP_S32 i,sum;
	IMP_S32 j;
	IMP_S32 imgW = avdModule->YImg.s32W;
	IMP_S32 imgH = avdModule->YImg.s32H;
	IMP_S32 allVolume = avdModule->histGrayImg.histAll.volume;
	IMP_FLOAT alpha = 0.05f;
	IMP_FLOAT beta = 1.f;
	IMP_S32 minValue,maxValue,cnt;
//	IMP_S32 uMean,vMean;
	IMP_U8 *grayDat;
	IMP_U8 *rDat, *gDat, *bDat;
	IMP_U8 *castDat;
	IMP_U8 flag_color;

	AVD_HISTOGROUP * pHistUImg = &avdModule->histUImg;
	AVD_HISTOGROUP * pHistVImg = &avdModule->histVImg;

	IMP_U8 level = avdModule->Para.stColorPara.u32AlarmLevel;
	IMP_U8 mode = avdModule->IR_mode;



	sum = 0;
	memset(sectionVolume,0,sizeof(IMP_S32)*9);
	for (i=IP_HIST_LEVEL-1;i>=0;i--)
	{
		sum += avdModule->histGrayImg.histAll.bin[i];
		if (sum > alpha*allVolume)	// 寻找灰度值最亮的0.05的像素灰度级别
			break;
	}
//	lowThrd = 40>>IP_HIST_LEVEL_B;
	highThrd = i;
	avdModule->clrLine = i;
#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Line:%d\n",i);
#endif
	grayDat = avdModule->YImg.pu8Data;
	rDat = avdModule->RImg.pu8Data;
	gDat = avdModule->GImg.pu8Data;
	bDat = avdModule->BImg.pu8Data;

	memset(sectionVolume,0,sizeof(IMP_S32)*9);
	cnt=0;
	for (j=0;j<imgH;j++)
	{
		for (i=0;i<imgW;i++)
		{
			if( (grayDat[i]>>IP_HIST_LEVEL_B) >= highThrd )
			{
				sectionVolume[R_CHANNAL][HIGH_SEC] +=  rDat[i];
				sectionVolume[G_CHANNAL][HIGH_SEC] +=  gDat[i];
				sectionVolume[B_CHANNAL][HIGH_SEC] +=  bDat[i];
				cnt++;
//				grayDat[i]=255;
			}
			else
			{
//				grayDat[i]=0;
			}
		}
		grayDat+=imgW;
		rDat+=imgW;
		gDat+=imgW;
		bDat+=imgW;
	}

	minValue = sectionVolume[0][HIGH_SEC];
	maxValue = sectionVolume[0][HIGH_SEC];

	for (i=1;i<3;i++)
	{
		if( maxValue < sectionVolume[i][HIGH_SEC] )
			maxValue = sectionVolume[i][HIGH_SEC];
		if( minValue > sectionVolume[i][HIGH_SEC] )
			minValue = sectionVolume[i][HIGH_SEC];
	}

	ratio1 = (maxValue-minValue)/cnt;
// 	ratio1 = minValue*100.0f*2000/maxValue/cnt;
// 	ratio1 = 100 - ratio1;
	ratio1 = (IMP_S32)(beta*ratio1);



	sum = 0;
	memset(sectionVolume,0,sizeof(IMP_S32)*9);
	lowThrd = 2;	//16>>IP_HIST_LEVEL_B;
	highThrd = 29;	//240>>IP_HIST_LEVEL_B;


	for (i=0;i<IP_HIST_LEVEL;i++)
	{
		if (i<lowThrd)
		{
			sectionVolume[R_CHANNAL][LOW_SEC] += avdModule->histRImg.histAll.bin[i];
		}
		else if(i<highThrd)
		{
			sectionVolume[R_CHANNAL][MID_SEC] += avdModule->histRImg.histAll.bin[i];
		}
		else
		{
			sectionVolume[R_CHANNAL][HIGH_SEC] += avdModule->histRImg.histAll.bin[i];
		}
	}

	// 统计绿色低中高亮区域分布像素数
	for (i=0;i<IP_HIST_LEVEL;i++)
	{
		if (i<lowThrd)
		{
			sectionVolume[G_CHANNAL][LOW_SEC] += avdModule->histGImg.histAll.bin[i];
		}
		else if(i<highThrd)
		{
			sectionVolume[G_CHANNAL][MID_SEC] += avdModule->histGImg.histAll.bin[i];
		}
		else
		{
			sectionVolume[G_CHANNAL][HIGH_SEC] += avdModule->histGImg.histAll.bin[i];
		}
	}

	// 统计蓝色低中高亮区域分布像素数
	for (i=0;i<IP_HIST_LEVEL;i++)
	{
		if (i<lowThrd)
		{
			sectionVolume[B_CHANNAL][LOW_SEC] += avdModule->histBImg.histAll.bin[i];
		}
		else if(i<highThrd)
		{
			sectionVolume[B_CHANNAL][MID_SEC] += avdModule->histBImg.histAll.bin[i];
		}
		else
		{
			sectionVolume[B_CHANNAL][HIGH_SEC] += avdModule->histBImg.histAll.bin[i];
		}
	}

	//找出RGB三通道的高亮区域的最大值最小值
	minValue = sectionVolume[R_CHANNAL][HIGH_SEC];
	maxValue = sectionVolume[R_CHANNAL][HIGH_SEC];
	if( maxValue < sectionVolume[G_CHANNAL][HIGH_SEC] )
		maxValue = sectionVolume[G_CHANNAL][HIGH_SEC];
	if( minValue > sectionVolume[G_CHANNAL][HIGH_SEC] )
		minValue = sectionVolume[G_CHANNAL][HIGH_SEC];
	if( maxValue < sectionVolume[B_CHANNAL][HIGH_SEC] )
		maxValue = sectionVolume[B_CHANNAL][HIGH_SEC];
	if( minValue > sectionVolume[B_CHANNAL][HIGH_SEC] )
		minValue = sectionVolume[B_CHANNAL][HIGH_SEC];

	//计算比值
	if (maxValue)
	{
		ratio3 = minValue*100/maxValue;
		ratio3 = 100 - ratio3;
	}
	else
	{
		ratio3 = 0;
	}


#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Color1 = %d\n",ratio1);
	printf("Color3 = %d\n",ratio3);
#endif
	if(avdModule->IR_mode)
		avdModule->colorValue = 0;
	else
		avdModule->colorValue = ratio3;


	avdModule->colorValue = AVD_2ThTrans(	avdModule->Adv_Para.u8ColorThrd[mode][level][TH1],
											avdModule->Adv_Para.u8ColorThrd[mode][level][TH2],
											avdModule->colorValue);

#ifdef DEBUG_AVD_SHOW_PRINTF
	printf("Color'= %d\n",avdModule->colorValue);
	printf("avdModule->IR_mode = %d\n",avdModule->IR_mode);
#endif

	return;
}
