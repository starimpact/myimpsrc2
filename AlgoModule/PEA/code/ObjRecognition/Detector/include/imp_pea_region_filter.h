/** 
* \defgroup 区域过滤
* 区域过滤
* @ingroup 背景建模
* @author 北京银瀑技术
* @version 1.0
* @data 2009-2010
*/
/*@{*/
#ifndef _IMP_PEA_REGION_FILTER_H_
#define _IMP_PEA_REGION_FILTER_H_

#include "imp_pea_common.h"

///<区域过滤参数
typedef struct  
{
	IMP_S32 s32RgeFilterEnable;             /**< 启用区域过滤 */
	IMP_S32 s32ThRgeFilterrMinEdge;         /**< 区域过滤最小梯度 */
	IMP_S32 s32ThRgeFilterMinWidth;         /**< 区域过滤最小宽度 */
	IMP_S32 s32ThRgeFilterMinHeight;        /**< 区域过滤最小高度 */
	IMP_S32 s32ThRgeFilterMinAreaPixel;     /**< 区域过滤区域最大像素数 */
} IpRGEFilterPara;

/**
* 区域集合过滤检测
* @param pstPara 阴影参数
* @param pstRgs 检测的目标集合
* @param pstShadow 阴影图像
*/
IMP_VOID ipRegionSetFilterDetect( IpRGEFilterPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs );

/**
* 区域过滤检测
* @param pstPara 区域过滤参数
* @param pstRgs 检测的区域集合
* @param s32RegionIndex 区域块序号
*/
static IMP_VOID ipRegionFilterDetect( IpRGEFilterPara *pstPara, PEA_DETECTED_REGIONSET_S *pstRgs, IMP_S32 s32RegionIndex );
#endif /*_IMP_PEA_SHADOW_MODLE_H_*/

/*@}*/
