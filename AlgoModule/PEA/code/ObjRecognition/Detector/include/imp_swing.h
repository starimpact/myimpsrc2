
#ifndef _IMP_SWING_MODEL_H_
#define _IMP_SWING_MODEL_H_


#include "imp_pea_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

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
	/*/存储正向SAD特征/*/
	IMP_UCHAR * vSadmin;
	/*/存储反向SAD特征/*/
	IMP_UCHAR * srcGray;
	IMP_UCHAR * forImg;
	IMP_UCHAR * bckImg;
	IMP_UCHAR * inMask;
	/*/存储随机摆动掩膜图/*/
	IMP_UCHAR * swImg;
	/*/存储随机摆动统计掩膜图/*/
}IMP_Swing_S;

IMP_S32 IMP_BinaryImg(IMP_UCHAR *srcGray,IMP_UCHAR *BinMask,IMP_S32 val,IMP_S32 Lenght);
/*/图像BinMask是对srcGray的二值化结果，srcGray像素值大于val相应的BinMask位置取值255，否则取值0，Length表示图像的像素数目/*/

IMP_S32 IMP_GaussImg(IMP_UCHAR*srcGray,IMP_UCHAR*dst,IMP_S32 ImgH,IMP_S32 ImgW);
/*/图像dst是对srcGray高斯平滑后的结果，此处的高斯模板是3X3模板，ImgH和ImgW分别表示图像的高和宽/*/

IMP_S32 IMP_DResizeImg(IMP_UCHAR*srcGray,IMP_UCHAR*dst,IMP_S32 ImgH,IMP_S32 ImgW);
/*/图像dst是对srcGray下采样后的结果，在下采样之前需要对图像进行高斯平滑，ImgH和ImgW分别表示图像的高和宽/*/

IMP_S32 IMP_UResizeImg(IMP_UCHAR*srcGray,IMP_UCHAR*dst,IMP_S32 ImgH,IMP_S32 ImgW);
/*/图像dst是对srcGray上采样后的结果/*/

IMP_S32 IMP_DePix(IMP_UCHAR*srcGray,IMP_S32 ImgH,IMP_S32 ImgW,IMP_S32 val);
/*/滤除噪声点，如果srcGray像素点的八邻域的之和小于val,则将当前的像素点值赋为0/*/

IMP_S32 IMP_StatiPix(IMP_UCHAR*srcGray,IMP_UCHAR*statiImg,IMP_S32 ImgH,IMP_S32 ImgW);

IMP_S32 IMP_StatiPixZ(IMP_Swing_S*PsModel,IMP_UCHAR*srcGray);
/*/得到随机摆动的掩膜图/*/
IMP_S32 IMP_Proporte(IMP_UCHAR*backGray,IMP_UCHAR*srcGray,IMP_UCHAR*proMat1,IMP_UCHAR*proMat2,IMP_S32 Length);
/*/得到图像backGray和图像srcGray中像素值大小比较图像
proMat1存储srcGray大于backGray的结果
proMat2存储backGray大于srcGray的结果/*/

IMP_S32 IMP_GetSadImg(IMP_Swing_S*PsModel);
/*/得到SAD特征操作，正负SAD特征分别存放在IMP_Swing_S参数中的zSadmin和vSadmin中/*/

IMP_S32 IMP_GetMemSizeSwingModel( PEA_RESULT_S *pstResult);
/*/估算该算法所需要的全部内存/*/

IMP_MODULE_HANDLE IMP_CreateSwing(PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource);
/*/对具体的参数初始化和内存的申请/*/

IMP_S32 IMP_ProcessSwing(IMP_MODULE_HANDLE hModule);
/*/算法的处理模块/*/

IMP_S32 IMP_ReleaseSwing(IMP_MODULE_HANDLE hModule);
/*/释放算法中申请的内存/*/



#ifdef __cplusplus
}
#endif

#endif

