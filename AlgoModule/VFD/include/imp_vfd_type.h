/**
* \defgroup VFD
* IMPOWER VFD检测算法
* @author 北京银瀑技术
* @version 1.0
* @data 2009-1010
*/
/*@{*/

#ifndef _IMP_VFD_TYPE_H
#define _IMP_VFD_TYPE_H

#ifdef __cplusplus
extern "C"
{
#endif

#include "imp_vfd_matrix.h"

/** smallest such that 1.0+DBL_EPSILON != 1.0 */
#define IMP_DBL_EPSILON     2.2204460492503131e-016 
/** min & max without jumps */
#define  IMP_IMIN(a, b)  ((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))

#define  IMP_IMAX(a, b)  ((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))

/** these settings affect the quality of detection: change with care */
#define IMP_ADJUST_FEATURES 1
#define IMP_ADJUST_WEIGHTS  0

#define IMP_HAAR_MAGIC_VAL    0x42500000
#define IMP_TYPE_NAME_HAAR    "haar-classifier"

#define IMP_IS_HAAR_CLASSIFIER( haar )                                                    \
    ((haar) != NULL &&                                                                   \
(((const IMP_HaarClassifierCascade*)(haar))->flags & IMP_MAGIC_MASK)==IMP_HAAR_MAGIC_VAL)

#define IMP_HAAR_FEATURE_MAX  3



typedef int sumtype;
typedef double sqsumtype;

/** 隐haar特征*/
typedef struct IMP_HidHaarFeature
{
	/** rect区域特征 */
    struct
    {
        sumtype *p0, *p1, *p2, *p3;
        float weight;
    }rect[IMP_HAAR_FEATURE_MAX];  
}
IMP_HidHaarFeature;

/** 隐haar树节点 */
typedef struct IMP_HidHaarTreeNode
{
    IMP_HidHaarFeature feature;   /**< 特征 */
    float threshold;              /**< 阈值 */
    int left;                     /**< 左节点 */
    int right;                    /**< 右节点 */
}IMP_HidHaarTreeNode;

/** 隐haar分类器 */
typedef struct IMP_HidHaarClassifier
{
    int count;                    /**< 数目 */
    IMP_HidHaarTreeNode* node;    /**< 节点 */
    float* alpha;                 /**< 系数 */
}IMP_HidHaarClassifier;

/** 隐haar stage分类器 */
typedef struct IMP_HidHaarStageClassifier
{
    int  count;                                /**< 数目 */
    float threshold;                           /**< 阈值 */         
    IMP_HidHaarClassifier* classifier;         /**< 隐分类器 */
    int two_rects;                             /**< rect */
    
    struct IMP_HidHaarStageClassifier* next;   /**< 下一个节点 */
    struct IMP_HidHaarStageClassifier* child;  /**< 子节点 */
    struct IMP_HidHaarStageClassifier* parent; /**< 父节点 */
}IMP_HidHaarStageClassifier;

/** 隐haar分类器数据 */
struct IMP_HidHaarClassifierCascade
{
    int  count;                                  /**< 数目 */
    int  is_stump_based;                         /**< 树墩标记 */
    int  has_tilted_features;                    /**< 有旋转积分图特征 */
    int  is_tree;                                /**< 树标记 */
    double inv_window_area;                      /**< 窗口区域 */
    IMP_Mat sum, sqsum, tilted;                  /**< 积分图、平方积分图、旋转积分图 */
    IMP_HidHaarStageClassifier* stage_classifier;/**< 分类器 */
    sqsumtype *pq0, *pq1, *pq2, *pq3;            /**< 平方积分图指针 */
    sumtype *p0, *p1, *p2, *p3;                  /**< 积分图指针 */
	
    void** ipp_stages;                           /**< stages指针 */
};


/** haar分类器特征 */
typedef struct IMP_HaarFeature
{
    int  tilted;                     /**< 旋转 */
	/** rect */
    struct
    {
        Imp_Rect r;
        float weight;
    } rect[IMP_HAAR_FEATURE_MAX];
}IMP_HaarFeature;

/** haar分类器 */
typedef struct IMP_HaarClassifier
{
    int count;                      /**< 数目 */
    IMP_HaarFeature* haar_feature;  /**< 特征 */  
    float* threshold;               /**< 阈值 */  
    int* left;                      /**< 左节点 */  
    int* right;                     /**< 右节点 */  
    float* alpha;                   /**< 系数 */  
}IMP_HaarClassifier;

/** stage haar分类器 */
typedef struct IMP_HaarStageClassifier
{
    int  count;                     /**< 数目 */
    float threshold;                /**< 阈值 */  
    IMP_HaarClassifier* classifier; /**< 分类器 */  
	
    int next;                       /**< 下一个节点数目 */
    int child;                      /**< 子节点数目 */
    int parent;                     /**< 父节点数目 */
}IMP_HaarStageClassifier;

typedef struct IMP_HidHaarClassifierCascade IMP_HidHaarClassifierCascade;

/** haar分类器数据 */
typedef struct IMP_HaarClassifierCascade
{
    int  flags;                                /**< 标记 */
    int  count;                                /**< 数目 */
    Imp_Size orig_window_size;                 /**< 原始窗尺寸 */
    Imp_Size real_window_size;                 /**< 实际窗尺寸 */
    double scale;                              /**< 比例系数 */
    IMP_HaarStageClassifier* stage_classifier; /**< stage分类 */
    IMP_HidHaarClassifierCascade* hid_cascade; /**< 隐分类器数据 */
}IMP_HaarClassifierCascade;

typedef struct IMP_AvgComp
{
    Imp_Rect rect; /**< bounding rectangle for the object (average rectangle of a group) */
    int neighbors; /**< number of neighbor rectangles in the group */
}IMP_AvgComp;

#ifdef __cplusplus
}
#endif

#endif /*_IMP_VFD_TYPE_H*/
/*@}*/