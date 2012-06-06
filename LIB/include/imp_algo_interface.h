/**
* \defgroup 接口
* 接口
* @author 北京银瀑技术
* @version 2.1
* @data 2009-2012
*/
/*@{*/


/**
* \defgroup PEA接口
* PEA接口
* @ingroup 接口
* @author 北京银瀑技术
* @version 2.1
* @data 2009-2012
*/
/*@{*/
#ifndef _IMP_ALGO_INTERFACE_H_
#define _IMP_ALGO_INTERFACE_H_

#include "imp_algo_type.h"
#include "imp_algo_urp_param.h"
#ifdef __cplusplus
extern "C"
{
#endif

/**
* 获取版本号和功能码
* @param hModule 模块句柄
* @param pstLibInfo 返回库相关信息
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_GetAlgoLibInfo( IMP_HANDLE hModule, LIB_INFO_S *pstLibInfo );
/**
* 分配内存
* @param hModule 模块句柄
* @param pstMems 分配的内存
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_GetMemReq( IMP_HANDLE hModule, MEM_SET_S *pstMems );

/**
* 创建各个模块并初始化
* @param hModule 模块句柄
* @param pstMems 初始化的内存区域
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_Create( IMP_HANDLE hModule, MEM_SET_S *pstMems );
/**
* 终止各个模块并释放内存
* @param hModule 模块句柄
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_Release( IMP_HANDLE hModule );
/**
* 配置Arm PEA的参数
* @param hModule 模块句柄
* @param pstParaE 外部参数
* @param pstParaURP 用户高级参数
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_ConfigArmPeaParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE ,URP_PARA_S *pstParaURP);
/**
* 配置参数
* @param hModule 模块句柄
* @param pstParaE 外部参数
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_ConfigParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE );
/**
* 开始执行算法
* @param hModule 模块句柄
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_Start( IMP_HANDLE hModule );
/**
* 停止执行算法
* @param hModule 模块句柄
* @param pstResult 处理结果
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_Stop( IMP_HANDLE hModule, RESULT_S *pstResult );
/**
* 处理一幅图像
* @param hModule 模块句柄
* @param pstImage 图像指针
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_ProcessImage( IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage );
/**
* 获取处理结果
* @param hModule 模块句柄
* @param pstResult 图像指针
* @return STATUS_E 处理结果
*/
IMP_EXPORTS STATUS_E IMP_GetResults( IMP_HANDLE hModule, RESULT_S *pstResult );
/*@}*/


//OSC 接口
/**
* 创建各个模块并初始化
* @param hModule 模块句柄
* @param pstMems 初始化的内存区域
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_OSC_Create( IMP_HANDLE hModule, MEM_SET_S *pstMems );
/**
* 终止各个模块并释放内存
* @param hModule 模块句柄
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_OSC_Release( IMP_HANDLE hModule );
/**
* 配置Arm PEA的参数
* @param hModule 模块句柄
* @param pstParaE 外部参数
* @param pstParaURP 用户高级参数
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_OSC_ConfigArmPeaParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE ,URP_PARA_S *pstParaURP);
/**
* 配置参数
* @param hModule 模块句柄
* @param pstParaE 外部参数
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_OSC_ConfigParameter( IMP_HANDLE hModule, EXTERNAL_PARA_S *pstParaE );

/**
* 处理一幅图像
* @param hModule 模块句柄
* @param pstImage 图像指针
* @return STATUS_E 操作状态
*/
IMP_EXPORTS STATUS_E IMP_OSC_ProcessImage( IMP_HANDLE hModule, YUV_IMAGE422_S *pstImage );
/**
* 获取处理结果
* @param hModule 模块句柄
* @param pstResult 图像指针
* @return STATUS_E 处理结果
*/
IMP_EXPORTS STATUS_E IMP_OSC_GetResults( IMP_HANDLE hModule, RESULT_S *pstResult );




/**
* \defgroup YUV422图像管理
* YUV422图像管理
* @ingroup 接口
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2010
*/
/*@{*/
/**
* 创建YUV422图像
* @param pstImg YUV422图像指针
* @param s32W 图像宽度
* @param s32H 图像高度
* @param pMmgr 内存管理器指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Create( YUV_IMAGE422_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );
/**
* 销毁YUV422图像
* @param pstImg YUV422图像指针
* @param pMmgr 内存管理器指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Destroy( YUV_IMAGE422_S *pstImg, IMP_VOID *pMmgr );
/**
* 清空YUV422图像
* @param pstImg YUV422图像指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Clear( YUV_IMAGE422_S *pstImg );
/**
* 复制YUV422图像
* @param pstSrc 源YUV422图像指针
* @param pstDst 目的YUV422图像指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage422Clone( YUV_IMAGE422_S *pstSrc, YUV_IMAGE422_S *pstDst );


/**
* 创建YUV420图像
* @param pstImg YUV420图像指针
* @param s32W 图像宽度
* @param s32H 图像高度
* @param pMmgr 内存管理器指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Create( YUV_IMAGE420_S *pstImg, IMP_S32 s32W, IMP_S32 s32H, IMP_VOID *pMmgr );
/**
* 销毁YUV420图像
* @param pstImg YUV420图像指针
* @param pMmgr 内存管理器指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Destroy( YUV_IMAGE420_S *pstImg, IMP_VOID *pMmgr );
/**
* 清空YUV420图像
* @param pstImg YUV420图像指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Clear( YUV_IMAGE420_S *pstImg );
/**
* 复制YUV420图像
* @param pstSrc 源YUV420图像指针
* @param pstDst 目的YUV420图像指针
*/
IMP_EXPORTS IMP_VOID IMP_YUVImage420Clone( YUV_IMAGE420_S *pstSrc, YUV_IMAGE422_S *pstDst );
/*@}*/

/**
* \defgroup 内存分配与释放
* 内存分配与释放
* @ingroup 接口
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2010
*/
/*@{*/
/**
* 分配内存
* @param pstMems 内存指针
*/
IMP_EXPORTS IMP_VOID IMP_MemsAlloc( MEM_SET_S *pstMems );
/**
* 释放内存
* @param pstMems 内存指针
*/
IMP_EXPORTS IMP_VOID IMP_MemsFree( MEM_SET_S *pstMems );
/*@}*/


/**
* \defgroup 外部参数配置
* 外部参数配置
* @ingroup 接口
* @author 北京银瀑技术
* @version 2.0
* @data 2009-2010
*/
/*@{*/

/**
* 分配外部配置参数缓冲区
* @param s32VanaBufLen vana缓冲区大小
* @param s32AdvBufNum 高级参数数量
* @param s32AdvBufLen 高级参数缓冲区大小
* @param pMmgr 内存指针
* @return EXTERNAL_PARA_S 外部参数指针
*/
IMP_EXPORTS EXTERNAL_PARA_S* IMP_PARA_Alloc( IMP_S32 s32VanaBufLen,IMP_S32 s32AdvBufNum, IMP_S32 s32AdvBufLen, IMP_VOID *pMmgr );
/**
* 释放外部配置参数缓冲区
* @param pstPara 外部参数缓冲区指针
* @param s32AdvBufNum 高级参数数量
* @param s32AdvBufLen 高级参数缓冲区大小
* @param pMmgr 内存指针
*/
IMP_EXPORTS IMP_VOID IMP_PARA_Free( EXTERNAL_PARA_S *pstPara, IMP_S32 s32AdvBufNum, IMP_VOID *pMmgr );
/**
* 清空外部配置参数缓冲区
* @param pstPara 外部参数缓冲区指针
* @param s32AdvBufNum 高级参数数量
*/
IMP_EXPORTS IMP_VOID IMP_PARA_Clear( EXTERNAL_PARA_S *pstPara, IMP_S32 s32AdvBufNum );
/**
* 初始化图像处理规则参数，从外部文件读取数据
* @param pstPara 外部参数
* @param pstScnFil 场景文件
* @param pstAdvFil 高级参数文件
* @param s32ImgW 图像宽度
* @param s32ImgH 图像高度
* @param pMmgr 内存指针
*/
IMP_EXPORTS IMP_VOID IMP_PARA_Init( EXTERNAL_PARA_S *pstPara, STR_ARRAY_PARA_S *pstScnFil,
									  STR_ARRAY_PARA_S *pstAdvFil, IMP_S32 s32ImgW, IMP_S32 s32ImgH, IMP_VOID *pMmgr );

/*@}*/


#ifdef __cplusplus
}
#endif

#endif /*_IMP_ALGO_INTERFACE_H_*/



/*@}*/
