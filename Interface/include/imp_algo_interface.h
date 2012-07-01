#ifndef _IMP_ALGO_INTERFACE_H_
#define _IMP_ALGO_INTERFACE_H_

#include "imp_algo_type.h"
#include "imp_algo_urp_param.h"
#include "imp_avd_para.h"
#include "imp_vfd_para.h"
#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************
name:
do:
input:
	hModule:
output:
return:
history:
****************************************************/


STATUS_E IMP_PEA_HD_GetMemReq(IMP_HANDLE hModule, MEM_SET_S *pstMems);
STATUS_E IMP_PEA_HD_Create(IMP_HANDLE hModule, MEM_SET_S *pstMems);
STATUS_E IMP_PEA_HD_Release(IMP_HANDLE hModule);
STATUS_E IMP_PEA_HD_ConfigArmPeaParameter(IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara ,URP_PARA_S *pstURPPara);
STATUS_E IMP_PEA_HD_ProcessImage(IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage);
STATUS_E IMP_PEA_HD_GetResults(IMP_HANDLE hModule, RESULT_S *pstResult);


STATUS_E IMP_OSC_HD_GetMemReq(IMP_HANDLE hModule, MEM_SET_S *pstMems);
STATUS_E IMP_OSC_HD_Create(IMP_HANDLE hModule, MEM_SET_S *pstMems);
STATUS_E IMP_OSC_HD_Release(IMP_HANDLE hModule);
STATUS_E IMP_OSC_HD_ConfigArmPeaParameter(IMP_HANDLE hModule, EXTERNAL_PARA_S *pstExtnalPara ,URP_PARA_S *pstURPPara);
STATUS_E IMP_OSC_HD_ProcessImage(IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage);
STATUS_E IMP_OSC_HD_GetResults(IMP_HANDLE hModule, RESULT_S *pstResult);

/***************************************************
name:IMP_GetAlgoLibInfo
do:算法主过程
input:
	hModule:算法句柄
output:
	pstLibInfo:库信息
return:
	0:成功
	非0:失败
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_GetAlgoLibInfo( IMP_HANDLE hModule, LIB_INFO_S *pstLibInfo );


/***************************************************
name:IMP_GetMemReq
do:获取内存信息
input:
	hModule:算法句柄
output:
	pstMems:内存信息
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_GetMemReq( IMP_HANDLE hModule, MEM_SET_S *pstMems );


/***************************************************
name:IMP_Create
do:为算法分配内存并做初始化
input:
	hModule:算法句柄
	pstMems:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_Create( IMP_HANDLE hModule, MEM_SET_S *pstMems );


/***************************************************
name:IMP_Release
do:释放算法资源
input:
	hModule:算法句柄
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_Release( IMP_HANDLE hModule );


/***************************************************
name:IMP_ConfigArmPeaParameter
do:配置算法参数
input:
	hModule:算法句柄
	pstParaE:0
	pstParaURP:URP规则参数
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_ConfigArmPeaParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE ,URP_PARA_S *pstParaURP);


/***************************************************
name:IMP_ConfigParameter
do:配置规则参数
input:
	hModule:算法句柄
	pstParaE:规则参数
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_ConfigParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE );


/***************************************************
name:IMP_Start
do:启动算法
input:
	hModule:算法句柄
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_Start( IMP_HANDLE hModule );


/***************************************************
name:IMP_Stop
do:停止算法
input:
	hModule:算法句柄
output:
	pstResult:结果
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_Stop( IMP_HANDLE hModule, RESULT_S *pstResult );


/***************************************************
name:IMP_ProcessImage
do:算法主过程
input:
	hModule:算法句柄
	pstImage:YUV图像
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_ProcessImage( IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage );


/***************************************************
name:IMP_GetResults
do:
input:
	hModule:算法句柄
	pstResult:算法结果
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_GetResults( IMP_HANDLE hModule, RESULT_S *pstResult );


/***************************************************
name:IMP_OSC_Create
do:为OSC算法分配内存和初始化
input:
	hModule:算法句柄
	pstMems:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_OSC_Create( IMP_HANDLE hModule, MEM_SET_S *pstMems );


/***************************************************
name:IMP_OSC_Release
do:释放算法资源
input:
	hModule:算法句柄
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_OSC_Release( IMP_HANDLE hModule );


/***************************************************
name:IMP_OSC_ConfigArmPeaParameter
do:配置ＯＳＣ算法规则参数
input:
	hModule:算法句柄
	pstParaE:0
	pstParaURP:URP规则参数
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_OSC_ConfigArmPeaParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE ,URP_PARA_S *pstParaURP);


/***************************************************
name:IMP_OSC_ConfigParameter
do:配置ＯＳＣ算法规则参数
input:
	hModule:算法句柄
	pstParaE:规则参数
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_OSC_ConfigParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE );


/***************************************************
name:IMP_OSC_ProcessImage
do:OSC算法主过程
input:
	hModule:算法句柄
	pstImage:YUV图像
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_OSC_ProcessImage( IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage );


/***************************************************
name:IMP_OSC_GetResults
do:获取算法结果
input:
	hModule:算法句柄
	pstResult:算法结果
output:
return:
history:
****************************************************/
IMP_EXPORTS STATUS_E IMP_OSC_GetResults( IMP_HANDLE hModule, RESULT_S *pstResult );


/***************************************************
name:IMP_AVD_Create
do:创建AVD
input:
	s32Width:图像宽
	s32Height:图像高
output:
return:
	算法句柄
history:
****************************************************/
IMP_EXPORTS IMP_HANDLE IMP_AVD_Create(IMP_S32 s32Width, IMP_S32 s32Height);


/***************************************************
name:IMP_AVD_Release
do:释放AVD
input:
	hModule:算法句柄
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_AVD_Release(IMP_HANDLE hModule);


/***************************************************
name:IMP_AVD_Process
do:AVD主过程
input:
	hModule:算法句柄
	pstImage:YUV图像
	pstExtTriggers:外部触发器指针
output:
return:
history:
****************************************************/
IMP_BOOL IMP_AVD_Process(IMP_HANDLE hModule, YUV_IMAGE420_S *pstImage, EXT_TRIGGERS_S *pstExtTriggers );


/***************************************************
name:IMP_AVD_GetResult
do:获取AVD结果
input:
	hModule:算法句柄
output:
	pstAvdResult:AVD结果
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_AVD_GetResult(IMP_HANDLE hModule, AVD_RESULT_S *pstAvdResult);


/***************************************************
name:IMP_AVD_ConfigParameter
do:配置AVD参数
input:
	hModule:算法句柄
	pstAvdParaSrc:参数
output:
return:
history:
****************************************************/
IMP_BOOL IMP_AVD_ConfigParameter(IMP_HANDLE hModule, IMP_AVD_PARA_S *pstAvdParaSrc);


/***************************************************
name:IMP_VFD_Create
do:创建VFD
input:
	s32Width:图像宽
	s32Height:图像高
output:
return:
	算法句柄
history:
****************************************************/
IMP_EXPORTS IMP_HANDLE IMP_VFD_Create(IMP_S32 s32ImgW, IMP_S32 s32ImgH);


/***************************************************
name:IMP_VFD_Release
do:释放VFD
input:
	hModule:算法句柄
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_VFD_Release(IMP_HANDLE hModule);


/***************************************************
name:IMP_VFD_ConfigParameter
do:配置VFD参数
input:
	hModule:算法句柄
	pstVfdParaSrc:参数
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_BOOL IMP_VFD_ConfigParameter(IMP_HANDLE hModule, IMP_VFD_PARA_S *pstVfdParaSrc);


/***************************************************
name:IMP_VFD_Process
do:VFD主过程
input:
	hModule:算法句柄
	pstImage:GRAY图像
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_BOOL IMP_VFD_Process(IMP_HANDLE hModule, GRAY_IMAGE_S *pstImage);


/***************************************************
name:IMP_VFD_GetResult
do:获取VFD结果
input:
	hModule:算法句柄
output:
	pstVfdResult:VFD结果
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_VFD_GetResult(IMP_HANDLE hModule, VFD_RESULT_S *pstVfdResult);


/***************************************************
name:IMP_YUVImage422Create
do:创建YUV422图像
input:
	pstImg:图像指针
	s32W:图像宽
	s32H:图像高
	pMmgr:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Create( YUV_IMAGE422_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );


/***************************************************
name:IMP_YUVImage422Destroy
do:销毁YUV422图像
input:
	pstImg:图像指针
	pMmgr:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Destroy( YUV_IMAGE422_S *pstImg, IMP_VOID *pMmgr );


/***************************************************
name:IMP_YUVImage422Clear
do:清空YUV422图像内容
input:
	pstImg:图像指针
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Clear( YUV_IMAGE422_S *pstImg );


/***************************************************
name:IMP_YUVImage422Clone
do:复制YUV422图像内容
input:
	pstSrc:源图像
	pstDst:目标图像
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Clone( YUV_IMAGE422_S *pstSrc, YUV_IMAGE422_S *pstDst );


/***************************************************
name:IMP_YUVImage420Create
do:创建YUV420图像
input:
	pstImg:图像指针
	s32W:图像宽
	s32H:图像高
	pMmgr:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Create( YUV_IMAGE420_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );


/***************************************************
name:IMP_YUVImage420Destroy
do:销毁YUV420图像
input:
	pstImg:图像指针
	pMmgr:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Destroy( YUV_IMAGE420_S *pstImg, IMP_VOID *pMmgr );


/***************************************************
name:IMP_YUVImage420Clear
do:清空YUV420图像内容
input:
	pstImg:图像指针
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Clear( YUV_IMAGE420_S *pstImg );


/***************************************************
name:IMP_YUVImage420Clone
do:复制YUV420图像内容
input:
	pstSrc:源图像
	pstDst:目标图像
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Clone( YUV_IMAGE420_S *pstSrc, YUV_IMAGE422_S *pstDst );


/***************************************************
name:IMP_MemsAlloc
do:审请内存
input:
	pstMems:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_MemsAlloc( MEM_SET_S *pstMems );


/***************************************************
name:IMP_MemsFree
do:释放内存
input:
	pstMems:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_MemsFree( MEM_SET_S *pstMems );


/***************************************************
name:IMP_PARA_Alloc
do:分配外部配置参数缓冲区
input:
	s32VanaBufLen:vana缓冲区大小
	s32AdvBufNum:高级参数数量
	s32AdvBufLen:高级参数缓冲区大小
	pMmgr:内存信息
output:
return:
	外部参数指针
history:
****************************************************/
IMP_EXPORTS EXTERNAL_PARA_S* IMP_PARA_Alloc( IMP_S32 s32VanaBufLen,IMP_S32 s32AdvBufNum, IMP_S32 s32AdvBufLen, IMP_VOID *pMmgr );


/***************************************************
name:IMP_PARA_Free
do:释放外部配置参数缓冲区
input:
	pstPara:外部参数缓冲区指针
	s32AdvBufNum:高级参数数量
	pMmgr:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_PARA_Free( EXTERNAL_PARA_S *pstPara, IMP_S32 s32AdvBufNum, IMP_VOID *pMmgr );


/***************************************************
name:IMP_PARA_Clear
do:清空外部配置参数缓冲区
input:
	pstPara:外部参数缓冲区指针
	s32AdvBufNum:高级参数数量
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_PARA_Clear( EXTERNAL_PARA_S *pstPara, IMP_S32 s32AdvBufNum );


/***************************************************
name:IMP_PARA_Init
do:初始化图像处理规则参数，从外部文件读取数据
input:
	pstPara:外部参数
	pstScnFil:场景文件
	pstAdvFil:高级参数文件
	s32ImgW:图像宽度
	s32ImgH:图像高度
	pMmgr:内存信息
output:
return:
history:
****************************************************/
IMP_EXPORTS IMP_VOID IMP_PARA_Init( EXTERNAL_PARA_S *pstPara, STR_ARRAY_PARA_S *pstScnFil,
									  STR_ARRAY_PARA_S *pstAdvFil, IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMmgr );

/*@}*/


#ifdef __cplusplus
}
#endif

#endif /*_IMP_ALGO_INTERFACE_H_*/



/*@}*/
