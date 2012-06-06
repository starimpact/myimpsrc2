/**
* \defgroup VFD
* IMPOWER VFD检测算法
* @author 北京银瀑技术
* @version 1.0
* @data 2009-1010
*/
/*@{*/

#ifndef _IMP_VFD_HAAR_H_
#define _IMP_VFD_HAAR_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include "imp_algo_type.h"
#include "imp_vfd_type.h"
#include "imp_vfd_xtype.h"
#include "imp_vfd_matrix.h"
#include "imp_vfd_para.h"

#define IMP_HAAR_MAGIC_VAL    0x42500000

/** 函数表 */
typedef struct impFUNC_TABLE_S
{
    void*   fn_2d[IMP_DEPTH_MAX]; /**< 函数表 */
}FUNC_TABLE_S;

/** compatible return codes */
typedef enum IMP_Status
{         
    IMP_BADMEMBLOCK_ERR          = -113,
	IMP_INPLACE_NOT_SUPPORTED_ERR= -112,
	IMP_UNMATCHED_ROI_ERR        = -111,
	IMP_NOTFOUND_ERR             = -110,
	IMP_BADCONVERGENCE_ERR       = -109,
	
	IMP_BADDEPTH_ERR             = -107,
	IMP_BADROI_ERR               = -106,
	IMP_BADHEADER_ERR            = -105,
	IMP_UNMATCHED_FORMATS_ERR    = -104,
	IMP_UNSUPPORTED_COI_ERR      = -103,
	IMP_UNSUPPORTED_CHANNELS_ERR = -102,
	IMP_UNSUPPORTED_DEPTH_ERR    = -101,
	IMP_UNSUPPORTED_FORMAT_ERR   = -100,
	
	IMP_BADARG_ERR      = -49,  
	IMP_NOTDEFINED_ERR  = -48,  
	
	IMP_BADCHANNELS_ERR = -47,  
	IMP_BADRANGE_ERR    = -44,  
	IMP_BADSTEP_ERR     = -29,  
		
	IMP_BADFLAG_ERR     =  -12,
	IMP_DIV_BY_ZERO_ERR =  -11, 
	IMP_BADCOEF_ERR     =  -10,
	
	IMP_BADFACTOR_ERR   =  -7,
	IMP_BADPOINT_ERR    =  -6,
	IMP_BADSCALE_ERR    =  -4,
	IMP_OUTOFMEM_ERR    =  -3,
	IMP_NULLPTR_ERR     =  -2,
	IMP_BADSIZE_ERR     =  -1,
	IMP_NO_ERR          =   0,
	IMP_OK              =   IMP_NO_ERR
}
IMP_Status;



IMP_API_EX( IMP_Status, icvIntegral_8u32s_C1R,
            "ippiIntegral_8u32s_C1R", IMP_PLUGINS1(IMP_PLUGIN_IPPCV),
            ( const unsigned char* pSrc, int srcStep, int* pDst, int dstStep,
              Imp_Size roiSize, int val ))

IMP_API_EX( IMP_Status, icvSqrIntegral_8u32s64f_C1R,
            "ippiSqrIntegral_8u32s64f_C1R", IMP_PLUGINS1(IMP_PLUGIN_IPPCV),
            ( const unsigned char* pSrc, int srcStep,
              int* pDst, int dstStep, double* pSqr, int sqrStep,
              Imp_Size roiSize, int val, double valSqr ))

IMP_API_EX( IMP_Status, icvRectStdDev_32s32f_C1R,
            "ippiRectStdDev_32s32f_C1R", IMP_PLUGINS1(IMP_PLUGIN_IPPCV),
            ( const int* pSrc, int srcStep,
              const double* pSqr, int sqrStep, float* pDst, int dstStep,
              Imp_Size roiSize, Imp_Rect rect ))

IMP_API_EX( IMP_Status, icvHaarClassifierInitAlloc_32f,
            "ippiHaarClassifierInitAlloc_32f", IMP_PLUGINS1(IMP_PLUGIN_IPPCV),
            ( void **pState, const Imp_Rect* pFeature, const float* pWeight,
              const float* pThreshold, const float* pVal1,
              const float* pVal2, const int* pNum, int length ))

IMP_API_EX( IMP_Status, icvHaarClassifierFree_32f,
            "ippiHaarClassifierFree_32f", IMP_PLUGINS1(IMP_PLUGIN_IPPCV),
            ( void *pState ))

IMP_API_EX( IMP_Status, icvApplyHaarClassifier_32s32f_C1R,
            "ippiApplyHaarClassifier_32s32f_C1R", IMP_PLUGINS1(IMP_PLUGIN_IPPCV),
            ( const int* pSrc, int srcStep, const float* pNorm,
              int normStep, unsigned char* pMask, int maskStep,
              Imp_Size roi, int *pPositive, float threshold,
              void *pState ))


/** canny过滤标记 */
#define IMP_HAAR_DO_CANNY_PRUNING 1
/** 图像缩放 */
#define IMP_HAAR_SCALE_IMAGE      2


/** 人脸检测模块结构 */
typedef struct impVFD_HAAR_MODULE
{
	GRAY_IMAGE_S temp;                  /**< 暂存灰度图 */
	IMP_U64 *integralImage;             /**< 积分图：(W+1)×(H+1), 单通道，64位，其中平方积分图像占用0-35位，积分图像占用36-59位 */
	IMP_S32 *sumcanny;                  /**< canny图像的积分图 */
	GRAY_IMAGE16_S stImgInEdgeGrad;		/**< 梯度图 */
	GRAY_IMAGE_S stImageEdgeMag;        /**< 幅度图 */
	
    IMP_Mat *sum;                       /**< 积分图 */
	IMP_Mat *sum2;                      /**< 积分图 */
	IMP_Mat *tilted;                    /**< 旋转积分图 */
	IMP_Mat *sqsum;                     /**< 平方积分图 */
	IMP_Mat *norm_img;                  /**< 正常尺寸的图像 */
	IMP_Mat *img_small;                 /**< 变换尺寸的图像（缩小） */
    IMP_Seq* seq;                       /**< 队列 */
    IMP_Seq* seq2;                      /**< 队列 */
    IMP_Seq* idx_seq;                   /**< 标号队列 */
    IMP_Seq* result_seq;                /**< 人脸结果队列 */
	IMP_S32 s32ImgW;                    /**< 图像宽度 */
	IMP_S32 s32ImgH;                    /**< 图像高度 */
	
	IMP_MemStorage* storage;            /**< 存储器 */
	IMP_VFD_PARA_S m_stVfdPara;         /**< 人脸检测参数 */
	VFD_RESULT_S m_stVfdResult;         /**< 人脸检测处理接过 */

	IMP_HaarClassifierCascade* Cascade; /**< 分类器 */

}VFD_HAAR_MODULE;

/**
* Haar人脸检测
* @param module 人脸检测模块句柄
* @param _img 被检图像 
* @param cascade harr 分类器级联的内部标识形式 
* @param storage 用来存储检测到的一序列候选目标矩形框的内存区域。 
* @param scale_factor 在前后两次相继的扫描中，搜索窗口的比例系数。例如1.1指将搜索窗口依次扩大10%。
* @param min_neighbors 构成检测目标的相邻矩形的最小个数(缺省－1)。如果组成检测目标的小矩形的个数和小于min_neighbors-1 都会被排除。如果min_neighbors 为 0, 则函数不做任何操作就返回所有的被检候选矩形框，这种设定值一般用在用户自定义对检测结果的组合程序上。 
* @param flags 操作方式。当前唯一可以定义的操作方式是 IMP_HAAR_DO_CANNY_PRUNING。如果被设定，函数利用Canny边缘检测器来排除一些边缘很少或者很多的图像区域，因为这样的区域一般不含被检目标。人脸检测中通过设定阈值使用了这种方法，并因此提高了检测速度。 
* @param min_size 检测窗口的最小尺寸。缺省的情况下被设为分类器训练时采用的样本尺寸(人脸检测中缺省大小是~20×20)。 
* @return 人脸定位的结果
*/
IMP_EXPORTS IMP_Seq* IMP_HaarDetectObjects( VFD_HAAR_MODULE *module, const GRAY_IMAGE_S* _img,
										   IMP_HaarClassifierCascade* cascade,
										   IMP_MemStorage* storage, double scale_factor,
					                       int min_neighbors, int flags, Imp_Size min_size );

/**
* 创建人脸分类器
* @param stage_count 分类器阶段数
* @return 分类器
*/
IMP_EXPORTS IMP_HaarClassifierCascade* IMP_CreateHaarClassifierCascade( int stage_count );
/**
* 释放人脸分类器
* @param cascade 分类器
*/
IMP_EXPORTS void IMP_ReleaseHaarClassifierCascade( IMP_HaarClassifierCascade** cascade );

/**
* 创建人脸分类器
* @param pPathName 分类器文本文件的名称
* @return 分类器
*/
IMP_EXPORTS IMP_HaarClassifierCascade* IMP_CreateHaarClassifier(char* pPathName);
/**
* 释放人脸分类器
* @param cascade 分类器
*/
IMP_EXPORTS void IMP_ReleaseHaarClassifier(IMP_HaarClassifierCascade** _cascade);

/**
* 读取人脸分类器
* @param pPathName 分类器文本文件的名称
* @param cascade 分类器
*/
IMP_EXPORTS void IMP_ReadHaarClassifierCascade(char* pPathName,IMP_HaarClassifierCascade* cascade);
/**
* 写入人脸分类器
* @param cascade 分类器
* @param pPathName 分类器文本文件的名称
*/
IMP_EXPORTS  void IMP_WriteHaarClassifierCascade(IMP_HaarClassifierCascade* cascade,char* pPathName);

/**
* 创建人脸分类器
* @param module 分类器模块句柄
* @param storage 存储器
* @param pPathName 分类器文本文件的名称
* @return 分类器
*/
IMP_EXPORTS IMP_HaarClassifierCascade* IMP_CreateHaar(VFD_HAAR_MODULE *module, IMP_MemStorage* storage, char* pPathName);
/**
* 释放人脸分类器
* @param module 分类器模块句柄
* @param cascade 分类器
*/
IMP_EXPORTS void IMP_ReleaseHaar(VFD_HAAR_MODULE *module,IMP_HaarClassifierCascade** _cascade);

#ifdef __cplusplus
}
#endif

#endif
/*@}*/