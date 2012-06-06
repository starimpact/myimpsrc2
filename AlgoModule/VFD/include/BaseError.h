/*****************************************************************************/
/** @file
 * 1D LUT变换。
 * @date        2003/03/10
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.
******************************************************************************/


#ifndef BASE_ERROR_H
#define BASE_ERROR_H


typedef enum {
    kBaseErr_Success            = 0,    /**< 成功 */
    kBaseErr_Pending            = 1,    /**< 处理保留项 */
    kBaseErr_NoSupportProcess   = 2,    /**< 未支持的处理 */
    kBaseErr_InvalidParam       = 3,    /**< 非法参数 */
    kBaseErr_ImgSizeError       = 10,   /**< 图像大小错误 */
    kBaseErr_ImgIOtypeError     = 11,   /**< 输入、输出图像类型不匹配 */
    kBaseErr_ImgDepthError      = 12,   /**< 错误的图像位数 */
    kBaseErr_ImgChannelError    = 13,   /**< 错误的图像通道数 */
    kBaseErr_ImgColorError      = 14,   /**< 错误的图像颜色格式表示 */
    kBaseErr_ImgOrderError      = 15,   /**< 错误的图像排列格式（点、线、面） */
    kBaseErr_ImgAlignError      = 16,   /**< 错误的图像对齐 */
    kBaseErr_ImgOrientError     = 17,   /**< 错误的图像开始方向 */
    kBaseErr_LUTError           = 30,   /**< 错误的LUT */
    kBaseErr_MagRateError       = 31,   /**< 扩缩率错误 */
    kBaseErr_MallocError        = 100,  /**< 内存分配错误 */
    kBaseErr_Arithmetic         = 101   /**< 计算错误   */
} BASE_ERROR_CODE;


#endif /* BASE_ERROR_H */

