
#ifndef _IMP_VIBE_BG_MODEL_H_
#define _IMP_VIBE_BG_MODEL_H_


#include "imp_pea_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

//算法配置参数
typedef struct impViBePara_S
{
	IMP_S32 s32BkgUR; //背景更新速度1~120
}IMP_ViBePara_S;



PEA_RESULT_S *pstResult;


/***************************************************
name:IMP_GetMemSizeViBe
do:得到算法所需要的内存大小
input:
	pstResult:系统公共数据
output:算法需要的内存大小
history:
****************************************************/
IMP_S32 IMP_GetMemSizeViBe(IMP_S32 s32Width, IMP_S32 s32Height);


/***************************************************
name:IMP_CreateViBe
do:审请算法内存并始化算法参数
input:
	pstResult:系统公共数据
	pHwResource:系统硬件资源
output:
	0:失败
	非0:句柄
history:
****************************************************/
IMP_MODULE_HANDLE IMP_CreateViBe( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );


/***************************************************
name:IMP_ReleaseViBe
do:释放算法内存
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ReleaseViBe( IMP_MODULE_HANDLE hModule );


/***************************************************
name:IMP_ConfigViBe
do:配置算法参数
input:
	hModule:算法模块句柄
	pstPara:参数配置结构体
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ConfigViBeModel( IMP_MODULE_HANDLE hModule, IMP_ViBePara_S *pstPara );


/***************************************************
name:IMP_ProcessViBe
do:算法主过程
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ProcessViBe( IMP_MODULE_HANDLE hModule );


#ifdef __cplusplus
}
#endif

#endif



