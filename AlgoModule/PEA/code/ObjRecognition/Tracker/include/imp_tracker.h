
#ifndef _IMP_TRACKER_H_
#define _IMP_TRACKER_H_


#include "imp_pea_common.h"


#ifdef __cplusplus
extern "C"
{
#endif

//算法配置参数
typedef struct impTrackerPara_S
{
	IMP_S32 s32S; //amount of subsampling, default value:2
}IMP_TrackerPara_S;


/***************************************************
name:IMP_GetMemSizeTracker
do:得到算法所需要的内存大小
input:
	pstResult:系统公共数据
output:算法需要的内存大小
history:
****************************************************/
IMP_S32 IMP_GetMemSizeTracker( PEA_RESULT_S *pstResult );


/***************************************************
name:IMP_CreateTracker
do:审请算法内存并始化算法参数
input:
	pstResult:系统公共数据
	pHwResource:系统硬件资源
output:
	0:失败
	非0:句柄
history:
****************************************************/
IMP_MODULE_HANDLE IMP_CreateTracker( PEA_RESULT_S *pstResult, GA_HARDWARE_RS_S *pstHwResource );


/***************************************************
name:IMP_ReleaseTracker
do:释放算法内存
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ReleaseTracker( IMP_MODULE_HANDLE hModule );


/***************************************************
name:IMP_ConfigTracker
do:配置算法参数
input:
	hModule:算法模块句柄
	pstPara:参数配置结构体
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ConfigTracker( IMP_MODULE_HANDLE hModule, IMP_TrackerPara_S *pstPara );


/***************************************************
name:IMP_ProcessTracker
do:算法主过程
input:
	hModule:算法模块句柄
output:
	0:成功
	非0:失败
history:
****************************************************/
IMP_S32 IMP_ProcessTracker( IMP_MODULE_HANDLE hModule );


#ifdef __cplusplus
}
#endif

#endif



