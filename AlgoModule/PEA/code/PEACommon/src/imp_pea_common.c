
#include "imp_pea_common.h"

#if IMP_DBG_USE_OPENCV
#include "cv.h"
#include "highgui.h"


void ipShowGrayImage(int width, int height, char *pbyImage, char *pbyName)
{
	int r, c;
	IplImage *pimg = cvCreateImage(cvSize(width, height), 8, 1);
	for (r = 0; r < height; r++)
	{
		for (c = 0; c < width; c++)
		{
			pimg->imageData[r * pimg->widthStep + c] = pbyImage[r * width + c];
		}
	}
	cvShowImage(pbyName, pimg);
	cvReleaseImage(&pimg);
}

void ipShowBinImage(int width, int height, char *pbyImage, char *pbyName)
{
	int r, c;
	IplImage *pimg = cvCreateImage(cvSize(width, height), 8, 1);
	for (r = 0; r < height; r++)
	{
		for (c = 0; c < width; c++)
		{
			pimg->imageData[r * pimg->widthStep + c] = pbyImage[r * width + c] != 0 ? 255 : 0;
		}
	}
	cvShowImage(pbyName, pimg);
	cvReleaseImage(&pimg);
}

void ipShowBinImageBit(int width, int height, char *pbyImage, char ubyBit, char *pbyName)
{
	int r, c;
	IplImage *pimg = cvCreateImage(cvSize(width, height), 8, 1);
	for (r = 0; r < height; r++)
	{
		for (c = 0; c < width; c++)
		{
			pimg->imageData[r * pimg->widthStep + c] = (pbyImage[r * width + c] & ubyBit) == ubyBit ? 255 : 0;
		}
	}
	cvShowImage(pbyName, pimg);
	cvReleaseImage(&pimg);
}

void ipShowBinImageLargerVal(int width, int height, char *pbyImage, char ubyVal, char *pbyName)
{
	int r, c;
	IplImage *pimg = cvCreateImage(cvSize(width, height), 8, 1);
	for (r = 0; r < height; r++)
	{
		for (c = 0; c < width; c++)
		{
			pimg->imageData[r * pimg->widthStep + c] = ((unsigned char*)pbyImage)[r * width + c] >= ubyVal ? 255 : 0;
		}
	}
	cvShowImage(pbyName, pimg);
	cvReleaseImage(&pimg);
}
#endif


IMP_S32 ipGrayQuadResize(GRAY_IMAGE_S *pimgSrc, GRAY_IMAGE_S *pimgDst)
{
	IMP_S32 dwRI, dwCI;
	IMP_S32 dwBRI, dwBCI;
	IMP_S32 dwOft, dwOft2, dwMax;
	IMP_S32 dwWidth = pimgSrc->s32W, dwHeight = pimgSrc->s32H;
	IMP_U8 *pubyNow = pimgSrc->pu8Data, *pubyTmp = pimgDst->pu8Data;
	IMP_U8 *pubyR1 = 0, *pubyR2 = 0, *pubyR3 = 0;
	IMP_S32 dwQW = dwWidth / 2, dwQH = dwHeight / 2, dwV1, dwV2, dwV3;
	
	for (dwRI = 2; dwRI < dwHeight - 2; dwRI += 2)
	{
		pubyR1 = pubyNow + (dwRI - 1) * dwWidth;
		pubyR2 = pubyR1 + dwWidth;
		pubyR3 = pubyR2 + dwWidth;
		for (dwCI = 2; dwCI < dwWidth - 2; dwCI += 2)
		{
			dwV1 = pubyR1[dwCI - 1] + pubyR1[dwCI] * 2 + pubyR1[dwCI + 1];
			dwV2 = pubyR2[dwCI - 1] + pubyR2[dwCI] * 2 + pubyR2[dwCI + 1];
			dwV3 = pubyR3[dwCI - 1] + pubyR3[dwCI] * 2 + pubyR3[dwCI + 1];
			pubyTmp[dwRI / 2 * dwQW + dwCI / 2] = (IMP_U8)((dwV1 + dwV2 * 2 + dwV3) / 16);
		}
	}
	
	return 0;
}


//Noise Estimate by 3X3 area
IMP_S32 ipNoiseEstimateByBox(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgTmp, IMP_S32 *pdwNoiseValue)
{
	IMP_S32 dwRI, dwCI;
	IMP_S32 dwBRI, dwBCI;
	IMP_S32 dwOft, dwOft2, dwMax;
	IMP_U8 *pubyNow = pimgNow->pu8Data, *pubyTmp = pimgTmp->pu8Data;
	IMP_S32 dwWidth = pimgNow->s32W, dwHeight = pimgNow->s32H;
	IMP_S32 adwHist[256], dwNoiseNum;
	IMP_U8 ubyDiff, ubyCenter, ubyOth, ubyWW;
	IMP_U8 aubyWVal[100], ubyWValNum;
	float fMean, fVar, fNoise;
	IMP_S32 dwQW = dwWidth / 2, dwQH = dwHeight / 2;
	IMP_U8 *pubyEdge = 0;
	
//FILE *pf = fopen("/home/zm/mean_var", "w");
	ipGrayQuadResize(pimgNow, pimgTmp);

	pubyEdge = pubyTmp + dwQW * dwQH;
	
	ipSobel3_3(pubyTmp, dwQW, pubyEdge, dwQW, dwQW, dwQH);
//	ipShowGrayImage(dwQW, dwQH, pubyEdge, "pimgTmp->pu8Data");
	
	memset(adwHist, 0, 256 * 4);
	ubyWW = 2; //ubyWW <= 4
	
	for (dwRI = ubyWW; dwRI < dwQH- ubyWW; dwRI += ubyWW+1)
	{
		for (dwCI = ubyWW; dwCI < dwQW - ubyWW; dwCI += ubyWW+1)
		{
			ubyWValNum = 0;
			fMean = 0;
			dwOft = dwRI * dwQW + dwCI;
			if (pubyEdge[dwOft] > 80) continue;
			
		//	dwMax = 0;
			for (dwBRI = dwRI - ubyWW; dwBRI <= dwRI + ubyWW; dwBRI++)
			{
				for (dwBCI = dwCI -ubyWW; dwBCI <= dwCI + ubyWW; dwBCI++)
				{
					dwOft2 = dwBRI * dwQW + dwBCI;
					aubyWVal[ubyWValNum] = pubyTmp[dwOft2];
					fMean += pubyTmp[dwOft2];
				//	dwMax += pubyEdge[dwOft2];
					ubyWValNum++;
				}
			}
			fMean /= ubyWValNum;
		//	dwMax /= ubyWValNum;
		//	if (dwMax > 33) continue;
			fVar = 0;
			for (dwBRI = 0; dwBRI < ubyWValNum; dwBRI++)
			{
				fVar += (aubyWVal[dwBRI] - fMean) * (aubyWVal[dwBRI] - fMean);
			}
			fVar /= ubyWValNum;
			fVar = sqrt(fVar);
			
//			fprintf(pf, "%.2f, %.2f;", fMean, fVar);
			
			dwOft = (IMP_S32)(fVar + 0.5f);
			
			adwHist[dwOft]++;
		}
	}
	
//fclose(pf);
	
	dwOft = adwHist[1];
	dwMax = 1;
	for (ubyDiff = 1; ubyDiff < 200; ubyDiff++)
	{
		if (dwOft < adwHist[ubyDiff])
		{
			dwOft = adwHist[ubyDiff];
			dwMax = ubyDiff;
		}
	}
	
	fNoise = (adwHist[dwMax - 1] * (dwMax - 1)) + (adwHist[dwMax] * dwMax) + (adwHist[dwMax + 1] * (dwMax + 1));
	fNoise /= (adwHist[dwMax - 1] + adwHist[dwMax] + adwHist[dwMax + 1]);
	
#if 0
	for (ubyDiff = 0; ubyDiff < 20; ubyDiff++)
	{
		printf("%d, ", adwHist[ubyDiff]);
	}imp_pea_common.c:(.text+0x48)：对‘cvCreateImage’未定义的引用
imp_pea_common.c:(.text+0xa4)：对‘cvShowImage’未定义的引用
imp_pea_common.c:(.text+0xac)：对‘cvReleaseImage’未定义的引用
libArmPEA.a(imp_pea_common.o)：在函数‘ipShowBinImage’中：
imp_pea_common.c:(.text+0xec)：对‘cvCreateImage’未定义的引用
imp_pea_common.c:(.text+0x158)：对‘cvShowImage’未定义的引用
imp_pea_common.c:(.text+0x160)：对‘cvReleaseImage’未定义的引用
libArmPEA.a(imp_pea_common.o)：在函数‘ipShowBinImageBit’中：
imp_pea_common.c:(.text+0x1a0)：对‘cvCreateImage’未定义的引用
imp_pea_common.c:(.text+0x210)：对‘cvShowImage’未定义的引用
imp_pea_common.c:(.text+0x218)：对‘cvReleaseImage’未定义的引用
libArmPEA.a(imp_pea_common.o)：在函数‘ipShowBinImageLargerVal’中：
imp_pea_common.c:(.text+0x258)：对‘cvCreateImage’未定义的引用
imp_pea_common.c:(.text+0x2c4)：对‘cvShowImage’未定义的引用
imp_pea_common.c:(.text+0x2cc)：对‘cvReleaseImage’未定义的引用

#endif
	
//	dwOft2 = (IMP_S32)((dwOft2 * 1.414f / dwOft) + 0.5f);
	dwOft2 = fNoise * 3.5;
	if (dwOft2 > 20) dwOft2 = 20;
	if (dwOft2 < 6) dwOft2 = 6;
//	printf("Noise:%.2f %d\n", fNoise, dwMax);	
	
	*pdwNoiseValue = dwOft2;
	
	return 0;
}


//Noise Estimate by 5X5 area, better and quicker version
IMP_S32 ipNoiseEstimateByBox_25(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgTmp, IMP_S32 *pdwNoiseValue)
{
	IMP_S32 dwRI, dwCI;
	IMP_S32 dwBRI, dwBCI;
	IMP_S32 dwOft, dwOft2, dwMax;
	IMP_U8 *pubyNow = pimgNow->pu8Data, *pubyTmp = pimgTmp->pu8Data;
	IMP_S32 dwWidth = pimgNow->s32W, dwHeight = pimgNow->s32H;
	IMP_S32 adwHist[256], dwNoiseNum, adwOft[25];
	IMP_U8 ubyDiff, ubyCenter, ubyOth, ubyWW;
	IMP_U8 aubyWVal[25], ubyWValNum;
	IMP_S32 fMean, fVar;
	float fNoise;
	IMP_U8 *pubyEdge = 0;
	
//FILE *pf = fopen("/home/zm/mean_var", "w");

	pubyEdge = pubyTmp;
	ipSobel3_3(pubyNow, dwWidth, pubyEdge, dwWidth, dwWidth, dwHeight);
//	ipShowGrayImage(dwWidth, dwHeight, pubyEdge, "pimgTmp->pu8Data");
	
	memset(adwHist, 0, 256 * 4);
	ubyWW = 2; //ubyWW <= 4
	adwOft[0] = -dwWidth * 2 - 2;adwOft[1] = -dwWidth * 2 - 1;adwOft[2] = -dwWidth * 2;adwOft[3] = -dwWidth * 2 + 1;adwOft[4] = -dwWidth * 2 + 2;
	adwOft[5] = -dwWidth * 1 - 2;adwOft[6] = -dwWidth * 1 - 1;adwOft[7] = -dwWidth * 2;adwOft[8] = -dwWidth * 1 + 1;adwOft[9] = -dwWidth * 1 + 2;
	adwOft[10] = -2;adwOft[11] = -1;adwOft[12] = 0;adwOft[13] = 1;adwOft[14] = 2;
	adwOft[15] = dwWidth * 1 - 2;adwOft[16] = dwWidth * 1 - 1;adwOft[17] = dwWidth * 2;adwOft[18] = dwWidth * 1 + 1;adwOft[19] = dwWidth * 1 + 2;
	adwOft[20] = dwWidth * 2 - 2;adwOft[21] = dwWidth * 2 - 1;adwOft[22] = dwWidth * 2;adwOft[23] = dwWidth * 2 + 1;adwOft[24] = dwWidth * 2 + 2;
	
	for (dwRI = ubyWW; dwRI < dwHeight- ubyWW; dwRI += ubyWW+1)
	{
		for (dwCI = ubyWW; dwCI < dwWidth - ubyWW; dwCI += ubyWW+1)
		{
			ubyWValNum = 0;
			fMean = 0;
			dwOft = dwRI * dwWidth + dwCI;
			if (pubyEdge[dwOft] > 80) continue;
			
			aubyWVal[0] = pubyNow[dwOft + adwOft[0]];aubyWVal[1] = pubyNow[dwOft + adwOft[1]];aubyWVal[2] = pubyNow[dwOft + adwOft[2]];
			aubyWVal[3] = pubyNow[dwOft + adwOft[3]];aubyWVal[4] = pubyNow[dwOft + adwOft[4]];aubyWVal[5] = pubyNow[dwOft + adwOft[5]];
			aubyWVal[6] = pubyNow[dwOft + adwOft[6]];aubyWVal[7] = pubyNow[dwOft + adwOft[7]];aubyWVal[8] = pubyNow[dwOft + adwOft[8]];
			aubyWVal[9] = pubyNow[dwOft + adwOft[9]];aubyWVal[10] = pubyNow[dwOft + adwOft[10]];aubyWVal[11] = pubyNow[dwOft + adwOft[11]];
			aubyWVal[12] = pubyNow[dwOft + adwOft[12]];aubyWVal[13] = pubyNow[dwOft + adwOft[13]];aubyWVal[14] = pubyNow[dwOft + adwOft[14]];
			aubyWVal[15] = pubyNow[dwOft + adwOft[15]];aubyWVal[16] = pubyNow[dwOft + adwOft[16]];aubyWVal[17] = pubyNow[dwOft + adwOft[17]];
			aubyWVal[18] = pubyNow[dwOft + adwOft[18]];aubyWVal[19] = pubyNow[dwOft + adwOft[19]];aubyWVal[20] = pubyNow[dwOft + adwOft[20]];
			aubyWVal[21] = pubyNow[dwOft + adwOft[21]];aubyWVal[22] = pubyNow[dwOft + adwOft[22]];aubyWVal[23] = pubyNow[dwOft + adwOft[23]]; aubyWVal[24] = pubyNow[dwOft + adwOft[24]];
			
			fMean = aubyWVal[0] + aubyWVal[1] + aubyWVal[2] + aubyWVal[3] + aubyWVal[4];
			fMean += aubyWVal[5] + aubyWVal[6] + aubyWVal[7] + aubyWVal[8] + aubyWVal[9];
			fMean += aubyWVal[10] + aubyWVal[11] + aubyWVal[12] + aubyWVal[13] + aubyWVal[14];
			fMean += aubyWVal[15] + aubyWVal[16] + aubyWVal[17] + aubyWVal[18] + aubyWVal[19];
			fMean += aubyWVal[20] + aubyWVal[21] + aubyWVal[22] + aubyWVal[23] + aubyWVal[24];
			
			fMean /= 25;
			
			fVar = 0;
			for (dwBRI = 0; dwBRI < 25; dwBRI++)
			{
				fVar += abs(aubyWVal[dwBRI] - fMean) << 4;
			}
			fVar /= 25;
			fVar >>= 4;
			
//			fprintf(pf, "%.2f, %.2f;", fMean, fVar);
						
			adwHist[fVar]++;
		}
	}
	
//fclose(pf);
	
	dwOft = adwHist[1];
	dwMax = 1;
	for (ubyDiff = 1; ubyDiff < 200; ubyDiff++)
	{
		if (dwOft < adwHist[ubyDiff])
		{
			dwOft = adwHist[ubyDiff];
			dwMax = ubyDiff;
		}
	}
	
	fNoise = (adwHist[dwMax - 1] * (dwMax - 1)) + (adwHist[dwMax] * dwMax) + (adwHist[dwMax + 1] * (dwMax + 1));
	fNoise /= (adwHist[dwMax - 1] + adwHist[dwMax] + adwHist[dwMax + 1]);
	
#if 0
	for (ubyDiff = 0; ubyDiff < 20; ubyDiff++)
	{
		printf("%d, ", adwHist[ubyDiff]);
	}
#endif
	
//	dwOft2 = (IMP_S32)((dwOft2 * 1.414f / dwOft) + 0.5f);
	dwOft2 = fNoise * 4;
	if (dwOft2 > 18) dwOft2 = 18;
	if (dwOft2 < 8) dwOft2 = 8;
//	printf("Noise:%.2f %d\n", fNoise, dwMax);	
	
	*pdwNoiseValue = dwOft2;
	
#if 0
	printf("Subtract Threshold:%.3f, %d\n", fNoise, dwOft2);
#endif	
	
	return 0;
}


//Noise Estimate by neighbor images
IMP_S32 ipNoiseEstimateByNeighbor(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgPre, IMP_S32 *pdwNoiseValue)
{
	IMP_S32 dwRI, dwCI;
	IMP_S32 dwBRI, dwBCI;
	IMP_S32 dwOft, dwOft2, dwMax;
	IMP_U8 *pubyNow = pimgNow->pu8Data, *pubyPre = pimgPre->pu8Data;
	IMP_S32 dwWidth = pimgNow->s32W, dwHeight = pimgNow->s32H;
	IMP_S32 adwHist[256], dwNoiseNum;
	IMP_U8 ubyDiff, ubyCenter, ubyOth, ubyWW;
	IMP_U8 aubyWVal[100], ubyWValNum;
	float fMean, fVar, fNoise;
	IMP_S32 dwQW = dwWidth / 2, dwQH = dwHeight / 2;
	IMP_U8 *pubyEdge = 0;
	
	memset(adwHist, 0, 256 * 4);
	ubyWW = 2; //ubyWW <= 4

	for (dwRI = ubyWW; dwRI < dwQH- ubyWW; dwRI += ubyWW+1)
	{
		for (dwCI = ubyWW; dwCI < dwQW - ubyWW; dwCI += ubyWW+1)
		{
			dwOft = dwRI * dwWidth + dwCI;
			ubyDiff = abs(pubyPre[dwOft] - pubyNow[dwOft]);
			
			adwHist[ubyDiff]++;
		}
	}
	
//fclose(pf);
	
	dwOft = adwHist[1];
	dwMax = 1;
	for (ubyDiff = 1; ubyDiff < 200; ubyDiff++)
	{
		if (dwOft < adwHist[ubyDiff])
		{
			dwOft = adwHist[ubyDiff];
			dwMax = ubyDiff;
		}
	}
	
	fNoise = (adwHist[dwMax - 1] * (dwMax - 1)) + (adwHist[dwMax] * dwMax) + (adwHist[dwMax + 1] * (dwMax + 1));
	fNoise /= (adwHist[dwMax - 1] + adwHist[dwMax] + adwHist[dwMax + 1]);
	
#if 1
	for (ubyDiff = 0; ubyDiff < 20; ubyDiff++)
	{
		printf("%d, ", adwHist[ubyDiff]);
	}
#endif
	
//	dwOft2 = (IMP_S32)((dwOft2 * 1.414f / dwOft) + 0.5f);
	dwOft2 = fNoise * 3.5;
	if (dwOft2 > 20) dwOft2 = 20;
	if (dwOft2 < 6) dwOft2 = 6;
	printf("Noise:%.2f %d\n", fNoise, dwMax);	
	
	*pdwNoiseValue = dwOft2;

	return 0;
}



//check if the point inside the Polygon
int InsidePolygon(Point *polygon,int N,Point p)
{
  int counter = 0;
  int i;
  double xinters;
  Point p1,p2;
  p1 = polygon[0];

  for (i=1;i<=N;i++)
  {
	  p2 = polygon[i % N];
	  if (p.y > MIN(p1.y,p2.y))
	  {
		  if (p.y <= MAX(p1.y,p2.y))
		  {
			  if (p.x <= MAX(p1.x,p2.x))
			  {
				  if (p1.y != p2.y)
				  {
					  xinters = (p.y-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
					  if (p1.x == p2.x || p.x <= xinters)
						counter++;
				  }

			  }

		  }
	  }
	  p1 = p2;
  }
  if (counter % 2 == 0)
	  return(OUTSIDE);
  else
	  return(INSIDE);
}



//remove shadow based on texture similarity
IMP_S32 ipRemoveShadow(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgBkg, GRAY_IMAGE_S *pimgBin, IMP_S32 dwNoiseValue)
{
	IMP_S32 dwRI, dwCI;
	IMP_S32 dwBRI, dwBCI;
	IMP_S32 dwOft, dwOft2;
	IMP_U8 *pubyNow = pimgNow->pu8Data, *pubyBkg = pimgBkg->pu8Data, *pubyBin = pimgBin->pu8Data;
	IMP_S32 dwWidth = pimgNow->s32W, dwHeight = pimgNow->s32H;
	IMP_U8 aubyBkgData[9], aubyNowData[9], ubyDataNum = 0;
	IMP_S16 wRatio;
	IMP_U8 ubyV1, ubyV2;
	
	for (dwRI = 5; dwRI < dwHeight- 5; dwRI += 1)
	{
		for (dwCI = 5; dwCI < dwWidth - 5; dwCI += 1)
		{
			dwOft = dwRI * dwWidth + dwCI;
			if (!pubyBin[dwOft] 
		//	|| pubyBkg[dwOft] <= pubyNow[dwOft]
			) continue;
			ubyDataNum = 0;
			for (dwBRI = dwRI - 1; dwBRI <= dwRI + 1; dwBRI++)
			{
				for (dwBCI = dwCI - 1; dwBCI <= dwCI + 1; dwBCI++)
				{
					dwOft2 = dwBRI * dwWidth + dwBCI;
					aubyBkgData[ubyDataNum] = pubyBkg[dwOft2];
					aubyNowData[ubyDataNum] = pubyNow[dwOft2];
					ubyDataNum++;
				}
			}
			
			//compare the texture similarity
			wRatio = 0;
			ubyV1 = aubyBkgData[4];
			ubyV2 = aubyNowData[4];
			for (dwBRI = 0; dwBRI < 9; dwBRI++)
			{
				wRatio += abs(ubyV1 * 128 / aubyBkgData[dwBRI] - ubyV2 * 128 / aubyNowData[dwBRI]);
			}
			wRatio /= 9;
			if (wRatio < 4) pubyBin[dwOft] = 0;
		}
	}
	
	return 0;
}



//remove shadow based on NCC and standard deviation
IMP_S32 ipRemoveShadowByNCC(GRAY_IMAGE_S *pimgNow, GRAY_IMAGE_S *pimgBkg, GRAY_IMAGE_S *pimgBin, IMP_S32 dwNoiseValue)
{
	IMP_S32 dwRI, dwCI;
	IMP_S32 dwBRI, dwBCI;
	IMP_S32 dwOft, dwOft2;
	IMP_U8 *pubyNow = pimgNow->pu8Data, *pubyBkg = pimgBkg->pu8Data, *pubyBin = pimgBin->pu8Data;
	IMP_S32 dwWidth = pimgNow->s32W, dwHeight = pimgNow->s32H;
	IMP_U8 aubyBkgData[9], aubyNowData[9], ubyDataNum = 0;
	IMP_S16 wRatio;
	IMP_U8 ubyV1, ubyV2;
	float fNCC, fNCC1, fNCC2, fNCC3, fVar, fMean, afRatio[9];
	
	for (dwRI = 5; dwRI < dwHeight- 5; dwRI += 1)
	{
		for (dwCI = 5; dwCI < dwWidth - 5; dwCI += 1)
		{
			dwOft = dwRI * dwWidth + dwCI;
			if (!pubyBin[dwOft] 
		//	|| pubyBkg[dwOft] <= pubyNow[dwOft]
			) continue;
			ubyDataNum = 0;
			for (dwBRI = dwRI - 1; dwBRI <= dwRI + 1; dwBRI++)
			{
				for (dwBCI = dwCI - 1; dwBCI <= dwCI + 1; dwBCI++)
				{
					dwOft2 = dwBRI * dwWidth + dwBCI;
					aubyBkgData[ubyDataNum] = pubyBkg[dwOft2];
					aubyNowData[ubyDataNum] = pubyNow[dwOft2];
					ubyDataNum++;

				}
			}
			
			//compare the texture similarity
			wRatio = 0;
			fNCC1 = 0;
			fNCC2 = 0;
			fNCC3 = 0;
			fMean = 0;
			for (dwBRI = 0; dwBRI < 9; dwBRI++)
			{
				fNCC1 += aubyBkgData[dwBRI] * aubyNowData[dwBRI];
				fNCC2 += aubyBkgData[dwBRI] * aubyBkgData[dwBRI];
				fNCC3 += aubyNowData[dwBRI] * aubyNowData[dwBRI];
				afRatio[dwBRI] = aubyNowData[dwBRI] * 1.0f / aubyBkgData[dwBRI];
				fMean += afRatio[dwBRI];
			}
			fNCC = fNCC1 / (sqrt(fNCC2) * sqrt(fNCC3));
			fMean /= 9;
			fVar = 0;
			for (dwBRI = 0; dwBRI < 9; dwBRI++)
			{
				fVar += fabs(fMean - afRatio[dwBRI]);
			}
			fVar /= 9;
			
		//	if (fNCC > 0.80 && fVar < 0.01) pubyBin[dwOft] = 0;
			if (fNCC > 0.99 && fVar < 0.04 && fabs(fMean- 1) < 0.3) pubyBin[dwOft] = 0;
		//	if (fVar < 0.1) pubyBin[dwOft] = 0;
		}
	}
	
	return 0;
}


//////////////////////////////////////////////////////////////


IMP_EXPORTS IMP_S32 IMP_FDEPTH_CalcDRegionResolution( FDEPTH_S *pstFDepth, PEA_DETECTED_REGION_S *pstDrg, IMP_DOUBLE *pdResolution )
{
	IMP_S32 ret;
	IMP_S32 x, y;
	double res_x, res_y;

	*pdResolution = 0;
	x = ((pstDrg->stRect.s16X1+pstDrg->stRect.s16X2)>>1);
	// 	y = ((drg->lb_y+drg->ru_y)>>1);
	y = pstDrg->stRect.s16Y2;
	ret = IMP_FDEPTH_GetPointResolution( pstFDepth, x, y, &res_x, &res_y );
	if( ret!=IMP_CLBR_SUCCESS )
		return ret;
	*pdResolution = res_x;

	return ret;
}
IMP_EXPORTS IMP_S32 IMP_FDEPTH_calcDRegionArea( FDEPTH_S *pstFDepth, PEA_DETECTED_REGION_S *pstDrg )
{
	IMP_S32 ret;
	IMP_S32 x, y;
	double res_x, res_y;
	double resolution;

	pstDrg->s32AreaActual = 0;
	x = ((pstDrg->stRect.s16X1+pstDrg->stRect.s16X2)>>1);
	// 	y = ((drg->lb_y+drg->ru_y)>>1);
	y = pstDrg->stRect.s16Y2;
	ret = IMP_FDEPTH_GetPointResolution( pstFDepth, x, y, &res_x, &res_y );
	if( ret!=IMP_CLBR_SUCCESS )
		return ret;
	resolution = res_x;
	pstDrg->s32AreaActual = (IMP_S32)( pstDrg->s32AreaPixel * (resolution*resolution) );

	/*printf("height = %d, width = %d, s32AreaActual= %d \n", (IMP_S32)((pstDrg->stRect.s16Y2 - pstDrg->stRect.s16Y1 + 1)*resolution),
															(IMP_S32)((pstDrg->stRect.s16X2 - pstDrg->stRect.s16X1 + 1)*resolution),
															pstDrg->s32AreaActual );
															*/


	return ret;
}
//////////////////////////////////////////////////////////////////////////
/// PEA
//////////////////////////////////////////////////////////////////////////
static IMP_VOID ipSModuleSpecialDataClear( PEA_SPECIAL_DATA_S *pstSpecialData )
{
	if( pstSpecialData->pDataDetector )
		memset( pstSpecialData->pDataDetector, 0, pstSpecialData->u32DetectorSz );
	if( pstSpecialData->pDataTracker )
		memset( pstSpecialData->pDataTracker, 0, pstSpecialData->u32TrackerSz );
	if( pstSpecialData->pDataClassifier )
		memset( pstSpecialData->pDataClassifier, 0, pstSpecialData->u32ClassifierSz );
	if( pstSpecialData->pDataAnalyst )
		memset( pstSpecialData->pDataAnalyst, 0, pstSpecialData->u32AnalystSz );
}

IMP_VOID ipDetectedRegionClear( PEA_DETECTED_REGION_S *pstDrg  )
{
	memset( pstDrg, 0, sizeof(PEA_DETECTED_REGION_S)-sizeof(PEA_SPECIAL_DATA_S) );
	ipSModuleSpecialDataClear( &pstDrg->stPrivateData );
}


IMP_VOID ipDetectedRegionSetClear( PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 i;

	pstRgs->s32UsedTotal = 0;
	pstRgs->s32FreeFirst = 1;
	for( i=0; i<IMP_MAX_TGT_CNT; ++i )
		ipDetectedRegionClear( pstRgs->astDrg + i );
	ipSModuleSpecialDataClear( &pstRgs->stPrivateData );
}

IMP_S32 ipDetectedRegionSetAlloc( PEA_DETECTED_REGIONSET_S *pstRgs )
{
	IMP_S32 s32Index = 0, i;
	PEA_DETECTED_REGION_S *pstDrg;

	if( pstRgs->s32FreeFirst > 0 && pstRgs->s32FreeFirst < (IMP_MAX_TGT_CNT - 1) )
	{
		s32Index = pstRgs->s32FreeFirst;
		pstDrg = &pstRgs->astDrg[s32Index];
		pstRgs->s32UsedTotal++;
		IMP_DRG_SET_LABELING(pstDrg->u8Used);
		i = s32Index;
		while( i < (IMP_MAX_TGT_CNT - 1) && IMP_DRG_IS_USED(pstRgs->astDrg[i].u8Used) )
			i++;
		pstRgs->s32FreeFirst = i;
	}

	return s32Index;
}

IMP_VOID ipDetectedRegionSetFree( PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32Index )
{
	PEA_DETECTED_REGION_S *pstDrg = &pstRgs->astDrg[s32Index];
	IMP_DRG_SET_UNUSED(pstDrg->u8Used);
	pstRgs->s32UsedTotal--;
	if( s32Index<pstRgs->s32FreeFirst )
		pstRgs->s32FreeFirst = s32Index;
}

IMP_S32 ipDetectedRegionSetIsEmpty( PEA_DETECTED_REGIONSET_S *pstRgs )
{
	return (pstRgs->s32FreeFirst >= (IMP_MAX_TGT_CNT - 1));
}
IMP_VOID ipTrackedTargetClear( IpTrackedTarget *pstTarget )
{
	memset( pstTarget, 0, sizeof(IpTrackedTarget)-sizeof(PEA_SPECIAL_DATA_S) );
	ipSModuleSpecialDataClear( &pstTarget->stPrivateData );
}

IMP_VOID ipTrackedTargetSetClear( IpTrackedTargetSet *pstTargetSet )
{
	IMP_S32 i;

	pstTargetSet->s32UsedTotal = 0;
	for( i = 0; i<IMP_MAX_TGT_CNT; ++i )
		ipTrackedTargetClear( pstTargetSet->astTargets + i );
	ipSModuleSpecialDataClear( &pstTargetSet->stPrivateData );
}

IMP_S32 ipTrackedTargetSetAlloc( IpTrackedTargetSet *pstTargetSet )
{
	IMP_S32 s32Index = 0, i;
	IpTrackedTarget *pstTarget = &pstTargetSet->astTargets[0];

	for( i = 1; i < IMP_MAX_TGT_CNT; i++ )
	{
		pstTarget++;
		if( !pstTarget->s32Used )
		{
			pstTargetSet->s32UsedTotal++;
			pstTarget->s32Used = 1;

			pstTarget->stTargetInfo.s32HumanLikehood = 0;
			pstTarget->stTargetInfo.s32VehicleLikehood = 0;
			pstTarget->stTargetInfo.s32WaggleLikehood = 0;
			pstTarget->stTargetInfo.s32HeadNum = 0;
			pstTarget->stTargetInfo.s32HeadSingleLikehood = 0;

			pstTarget->stTargetInfo.stStaticTargetInfo.s32AbandumLikehood = 0;
			pstTarget->stTargetInfo.stStaticTargetInfo.s3ObjStolenLikehood = 0;
			pstTarget->stTargetInfo.stStaticTargetInfo.s32NoParkingLikehood = 0;

			s32Index = i;
			break;
		}
	}

	return s32Index;
}

IMP_VOID ipTrackedTargetSetFree( IpTrackedTargetSet *pstTargetSet, IMP_S32 s32Index )
{
	IpTrackedTarget *pstTarget = &pstTargetSet->astTargets[s32Index];
	pstTarget->s32Used = 0;
	ipTrackedTargetClear( pstTarget );
	pstTargetSet->s32UsedTotal--;
}



IMP_VOID IMP_PEA_StatusClear( PEA_STATUS_S *pstStatus )
{
	pstStatus->u32DetectorStatus =
	pstStatus->u32TrackerStatus =
	pstStatus->u32ClassifierStatus =
	pstStatus->u32AnalystStatus = IMP_MOUDLE_STATUS_UNKNOWN;

	pstStatus->u32WaterMarkStatus = IMP_WATERMARK_STAT_UNKNOWN;

	if( !pstStatus->u32NotFirstFrm )
	{
		pstStatus->u32SceneStatusPre = pstStatus->u32SceneStatus = IMP_SCENE_STATUS_WORKING;
		pstStatus->u32SignalStatusPre = pstStatus->u32SignalStatus = IMP_SIGNAL_STATUS_GOOD;
	}
#ifdef SUPPORT_PTZ
	pstStatus->u32PtzStatusPre = pstStatus->u32PtzStatus = IMP_PTZ_STATUS_STATIC;
#endif

	//	pstStatus->u32NotFirstFrm = 0;
	pstStatus->u32NeedReInit = 0;
	pstStatus->u32BgIsUpdated = IMP_TRUE;
}

IMP_VOID IMP_PEA_ResultInit( PEA_RESULT_S *pstResult, IMP_S32 s32ImgW, IMP_S32 s32ImgH, MEM_MGR_ARRAY_S *pstMemMgr )
{
	memset( pstResult, 0, sizeof(PEA_RESULT_S) );
	pstResult->s32Height = s32ImgH;
	pstResult->s32Width = s32ImgW;
	IMP_GrayImageCreate( &pstResult->stImgInGray, s32ImgW, s32ImgH, pstMemMgr );
	IMP_EVTMGR_Initialize( &pstResult->stEvtMgr, IMP_EVT_ID_MIN, IMP_EVT_ID_MAX );
}

IMP_VOID IMP_PEA_ResultTerminate( PEA_RESULT_S *pstResult, MEM_MGR_ARRAY_S *pstMemMgr )
{
	IMP_EVTMGR_Terminate( &pstResult->stEvtMgr );
	IMP_GrayImageDestroy( &pstResult->stImgInGray, pstMemMgr );
	memset( pstResult, 0, sizeof(PEA_RESULT_S) );
}

IMP_VOID IMP_PEA_ResultClear( PEA_RESULT_S *pstResult )
{
	pstResult->u32FrmTimeCur = pstResult->u32FrmTimePre = 0;
	IMP_PEA_StatusClear( &pstResult->stSysStatus );
	IMP_GrayImageClear( &pstResult->stImgInGray );
	ipDetectedRegionSetClear( &pstResult->stDRegionSet );
	ipTrackedTargetSetClear( &pstResult->stTrackedTargetSet );

#ifdef SUPPORT_CHANGE
	ipDetectedRegionSetClear( &pstResult->stDRegionSetChg );
	ipTrackedTargetSetClear( &pstResult->stTrackedTargetSetChg );
#endif
}


IMP_S32 ipCalcRegionFgNum( GRAY_IMAGE_S *pstImage, IMP_RECT_S *pstRect )
{
	IMP_S32 yy, xx, s32Offset, s32ImgW;
	IMP_S32 xs, xe, ys, ye;
	IMP_U8 *pu8Data;
	IMP_S32 s32FgNum;

	s32ImgW = pstImage->s32W;
	xs = pstRect->s16X1;
	xe = pstRect->s16X2;
	ys = pstRect->s16Y1;
	ye = pstRect->s16Y2;
	s32Offset = s32ImgW * ys;
	pu8Data = pstImage->pu8Data + s32Offset;
	s32FgNum = 0;

	for( yy = ys; yy <= ye; yy++ )
	{
#ifdef _TIDSP
	#pragma MUST_ITERATE(32,,4);
#endif
		for( xx = xs; xx <= xe; xx++ )
		{
			if( pu8Data[xx] )
				s32FgNum++;
		}
		pu8Data += s32ImgW;
	}

	return s32FgNum;
}

IMP_S32 ipCalFgRatio(GRAY_IMAGE_S *pstImageB)
{
	IMP_S32 s32ImgW = pstImageB->s32W;
	IMP_S32 s32ImgH = pstImageB->s32H;
	IMP_U8 *pu8ImgDat = pstImageB->pu8Data;
	IMP_S32 x, y;
	IMP_S32 s32FgNum = 0;
	IMP_S32 s32FgRatio = 0;
	for( y = 0; y < s32ImgH; y++ )
    {
        for( x = 0; x < s32ImgW; x++ )
		{
			if (pu8ImgDat[x])
			{
				s32FgNum++;
			}
		}
		pu8ImgDat += s32ImgW;
	}
	s32FgRatio = (IMP_S32)((s32FgNum * 100) / (IMP_FLOAT)(s32ImgW * s32ImgH) + 0.5);
	return s32FgRatio;
}
