
#ifndef _IMP_SWING_MODEL_H_
#define _IMP_SWING_MODEL_H_


#include "imp_pea_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

//算法配置参数
typedef struct impSwingPara_S
{
	IMP_S32 s32Tmp; 
}IMP_SwingPara_S;


typedef struct impswing_S
{
	GA_HARDWARE_RS_S  *pstHwResource;
	PEA_RESULT_S *pstResult; 

	IMP_UCHAR * p00;
	IMP_UCHAR * p01;
	IMP_UCHAR * p02;
	IMP_UCHAR * p10;
	IMP_UCHAR * p11;
	IMP_UCHAR * p12;
	IMP_UCHAR * p20;
	IMP_UCHAR * p21;
	IMP_UCHAR * p22;
	IMP_UCHAR * zSadmin;
	IMP_UCHAR * vSadmin;
	IMP_UCHAR * inMask;
	IMP_UCHAR * swImg;

}IMP_Swing_S;

IMP_S32 IMP_ShowImg(IMP_UCHAR*srcGray,IMP_S32 Height,IMP_S32 Width,IMP_UCHAR*widoName);

IMP_S32 IMP_BinaryImg(IMP_UCHAR *srcGray,IMP_UCHAR *BinMask,IMP_S32 val,IMP_S32 Lenght);
/*/得到srcGray的二值图,经大于val的赋值为255，其他的赋值为0，保存在BinMask中，Length是图像的“高X宽”/*/

IMP_S32 IMP_StatiPix(IMP_UCHAR*srcGray,IMP_UCHAR*statiImg,IMP_S32 ImgH,IMP_S32 ImgW);
/*/得到srcGray的累计图，/*/
/*/该操作之前需要对statiImg图像进行归零/*/
/*/srcGray图像中像素值不是0，则statiImg相应位置应该+1/*/
/*/srcGray图像中像素值为0，则statiImg相应位置应该-1/*/
/*/当数值大于255时，statiImg赋值为255/*/
/*/当数值小雨0，statiImg赋值为0/*/

IMP_S32 IMP_Proporte(IMP_UCHAR*backGray,IMP_UCHAR*srcGray,IMP_UCHAR*proMat1,IMP_UCHAR*proMat2,IMP_S32 Length);
/*/得到图像像素值的比值图，Length是图像数据内存的总长度/*/

IMP_S32 IMP_RectSum(IMP_UCHAR*srcGray,IMP_RECT_S widow,IMP_S32 Height,IMP_S32 Width);
/*/图像srcGray在矩形框widow中的像素总和/*/

IMP_S32 IMP_GetSadImg(IMP_Swing_S*PsModel);
/*/得到正反操作的SAD的图像，分别存放于输入指针对应的zSadMin和vSadMin中/*/

IMP_S32 IMP_GetMemSizeSwingModel( PEA_RESULT_S *pstResult);
/*/得到该算法所有需要内存总和,已经包含了内部申请的图像内存/*/

IMP_MODULE_HANDLE IMP_CreateSwing(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);
/*/算法内部需要的所有参数的建立和内存的申请的建立/*/

IMP_S32 IMP_ProcessSwing(IMP_MODULE_HANDLE hModule);
/*/算法的处理过程,算法的核心，封装在此/*/

IMP_S32 IMP_ReleaseSwing(IMP_MODULE_HANDLE hModule);
/*/释放算法的内存/*/



#ifdef __cplusplus
}
#endif

#endif



