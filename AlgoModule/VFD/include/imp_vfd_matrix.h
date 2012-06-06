/** 
* \defgroup 矩阵
* 矩阵定义及操作
* @ingroup VFD
* @author 北京银瀑技术
* @version 1.0
* @data 2009-2010
*/
/*@{*/

#ifndef _IMP_VFD_MATRIX_H_
#define _IMP_VFD_MATRIX_H_


#ifdef __cplusplus
extern "C"
{
#endif
	
	
#include "imp_algo_type.h"
#include "imp_ga_math.h"

/** rect定义 */
typedef struct Imp_Rect
{
	int x;          /**< 左上角x坐标 */
	int y;          /**< 左上角y坐标 */
	int width;      /**< 宽度 */
	int height;     /**< 高度 */
}Imp_Rect;

/**
* rect构造
* @param x 左上角x坐标
* @param y 左上角y坐标
* @param width 宽度
* @param height 高度
* @return Imp_Rect rect区域
*/
Imp_Rect  IMP_Rect( int x, int y, int width, int height );

/** 点定义 */
typedef struct Imp_Point
{
	int x;       /**< x坐标 */
	int y;       /**< y坐标 */
}Imp_Point;

/**
* 点构造
* @param x x坐标
* @param y y坐标
* @return Imp_Point 点
*/
Imp_Point  IMP_Point( int x, int y );

/** 尺寸定义 */
typedef struct
{
	int width;     /**< 宽度 */
	int height;    /**< 高度 */
}Imp_Size;


/**
* size构造
* @param width 宽度
* @param height 高度
* @return Imp_Size 尺寸
*/
Imp_Size  IMP_Size( int width, int height );	

/** minimum (signed) int value */
#define IMP_INT_MIN     (-2147483647 - 1) 
/** maximum (signed) int value */
#define IMP_INT_MAX       2147483647    
	
	
/****************************************************************************************\
*                                  Matrix type (IMP_Mat)                                   *
\****************************************************************************************/
	
#define IMP_CN_MAX     64
#define IMP_CN_SHIFT   3
#define IMP_DEPTH_MAX  (1 << IMP_CN_SHIFT)
	
#define IMP_8U   0
#define IMP_8S   1
#define IMP_16U  2
#define IMP_16S  3
#define IMP_32S  4
#define IMP_32F  5
#define IMP_64F  6
#define IMP_USRTYPE1 7
	
#define IMP_MAKETYPE(depth,cn) ((depth) + (((cn)-1) << IMP_CN_SHIFT))
#define IMP_MAKE_TYPE IMP_MAKETYPE
	
#define IMP_8UC1 IMP_MAKETYPE(IMP_8U,1)
#define IMP_8UC2 IMP_MAKETYPE(IMP_8U,2)
#define IMP_8UC3 IMP_MAKETYPE(IMP_8U,3)
#define IMP_8UC4 IMP_MAKETYPE(IMP_8U,4)
#define IMP_8UC(n) IMP_MAKETYPE(IMP_8U,(n))
	
#define IMP_8SC1 IMP_MAKETYPE(IMP_8S,1)
#define IMP_8SC2 IMP_MAKETYPE(IMP_8S,2)
#define IMP_8SC3 IMP_MAKETYPE(IMP_8S,3)
#define IMP_8SC4 IMP_MAKETYPE(IMP_8S,4)
#define IMP_8SC(n) IMP_MAKETYPE(IMP_8S,(n))
	
#define IMP_16UC1 IMP_MAKETYPE(IMP_16U,1)
#define IMP_16UC2 IMP_MAKETYPE(IMP_16U,2)
#define IMP_16UC3 IMP_MAKETYPE(IMP_16U,3)
#define IMP_16UC4 IMP_MAKETYPE(IMP_16U,4)
#define IMP_16UC(n) IMP_MAKETYPE(IMP_16U,(n))
	
#define IMP_16SC1 IMP_MAKETYPE(IMP_16S,1)
#define IMP_16SC2 IMP_MAKETYPE(IMP_16S,2)
#define IMP_16SC3 IMP_MAKETYPE(IMP_16S,3)
#define IMP_16SC4 IMP_MAKETYPE(IMP_16S,4)
#define IMP_16SC(n) IMP_MAKETYPE(IMP_16S,(n))
	
#define IMP_32SC1 IMP_MAKETYPE(IMP_32S,1)
#define IMP_32SC2 IMP_MAKETYPE(IMP_32S,2)
#define IMP_32SC3 IMP_MAKETYPE(IMP_32S,3)
#define IMP_32SC4 IMP_MAKETYPE(IMP_32S,4)
#define IMP_32SC(n) IMP_MAKETYPE(IMP_32S,(n))
	
#define IMP_32FC1 IMP_MAKETYPE(IMP_32F,1)
#define IMP_32FC2 IMP_MAKETYPE(IMP_32F,2)
#define IMP_32FC3 IMP_MAKETYPE(IMP_32F,3)
#define IMP_32FC4 IMP_MAKETYPE(IMP_32F,4)
#define IMP_32FC(n) IMP_MAKETYPE(IMP_32F,(n))
	
#define IMP_64FC1 IMP_MAKETYPE(IMP_64F,1)
#define IMP_64FC2 IMP_MAKETYPE(IMP_64F,2)
#define IMP_64FC3 IMP_MAKETYPE(IMP_64F,3)
#define IMP_64FC4 IMP_MAKETYPE(IMP_64F,4)
#define IMP_64FC(n) IMP_MAKETYPE(IMP_64F,(n))
	
#define IMP_AUTO_STEP  0x7fffffff
#define IMP_WHOLE_ARR  IMP_Slice( 0, 0x3fffffff )
	
#define IMP_MAT_CN_MASK          ((IMP_CN_MAX - 1) << IMP_CN_SHIFT)
#define IMP_MAT_CN(flags)        ((((flags) & IMP_MAT_CN_MASK) >> IMP_CN_SHIFT) + 1)
#define IMP_MAT_DEPTH_MASK       (IMP_DEPTH_MAX - 1)
#define IMP_MAT_DEPTH(flags)     ((flags) & IMP_MAT_DEPTH_MASK)
#define IMP_MAT_TYPE_MASK        (IMP_DEPTH_MAX*IMP_CN_MAX - 1)
#define IMP_MAT_TYPE(flags)      ((flags) & IMP_MAT_TYPE_MASK)
#define IMP_MAT_CONT_FLAG_SHIFT  14
#define IMP_MAT_CONT_FLAG        (1 << IMP_MAT_CONT_FLAG_SHIFT)
#define IMP_IS_MAT_CONT(flags)   ((flags) & IMP_MAT_CONT_FLAG)
#define IMP_IS_CONT_MAT          IMP_IS_MAT_CONT
#define IMP_MAT_TEMP_FLAG_SHIFT  15
#define IMP_MAT_TEMP_FLAG        (1 << IMP_MAT_TEMP_FLAG_SHIFT)
#define IMP_IS_TEMP_MAT(flags)   ((flags) & IMP_MAT_TEMP_FLAG)
	
#define IMP_MAGIC_MASK       0xFFFF0000
#define IMP_MAT_MAGIC_VAL    0x42420000
#define IMP_MATND_MAGIC_VAL    0x42430000
	
#define IMP_IS_MAT_HDR(mat) \
    ((mat) != NULL && \
    (((const IMP_Mat*)(mat))->type & IMP_MAGIC_MASK) == IMP_MAT_MAGIC_VAL && \
    ((const IMP_Mat*)(mat))->cols > 0 && ((const IMP_Mat*)(mat))->rows > 0)
	

#define IMP_IS_MAT(mat) \
(IMP_IS_MAT_HDR(mat) && ((const IMP_Mat*)(mat))->data.ptr != NULL)

/** 0x3a50 = 11 10 10 01 01 00 00 ~ array of log2(sizeof(arr_type_elem)) */
#define IMP_ELEM_SIZE(type) \
(IMP_MAT_CN(type) << ((((sizeof(size_t)/4+1)*16384|0x3a50) >> IMP_MAT_DEPTH(type)*2) & 3))

/** size of each channel item,
0x124489 = 1000 0100 0100 0010 0010 0001 0001 ~ array of sizeof(arr_type_elem) */
#define IMP_ELEM_SIZE1(type) \
((((sizeof(size_t)<<28)|0x8442211) >> IMP_MAT_DEPTH(type)*4) & 15)

#define IMP_MAT_ELEM_PTR_FAST( mat, row, col, pix_size )  \
    (assert( (unsigned)(row) < (unsigned)(mat).rows &&   \
	(unsigned)(col) < (unsigned)(mat).cols ),   \
(mat).data.ptr + (size_t)(mat).step*(row) + (pix_size)*(col))

#define IMP_ARE_SIZES_EQ(mat1, mat2) \
((mat1)->rows == (mat2)->rows && (mat1)->cols == (mat2)->cols)


/** matrices are continuous by default */
#define  IMP_DEFAULT_MAT_ROW_ALIGN  1

/** the alignment of all the allocated buffers */
#define  IMP_MALLOC_ALIGN    32

/** maximal size of vector to run matrix operations on it inline (i.e. w/o ipp calls) */
#define  IMP_MAX_INLINE_MAT_OP_SIZE  10

/** default step, set in case of continuous data
to work around checks for valid step in some ipp functions */
#define  IMP_STUB_STEP     (1 << 30)


/** general-purpose saturation macros */ 
#define  IMP_CAST_8U(t)  (IMP_U8)(!((t) & ~255) ? (t) : (t) > 0 ? 255 : 0)
#define  IMP_CAST_8S(t)  (char)(!(((t)+128) & ~255) ? (t) : (t) > 0 ? 127 : -128)
#define  IMP_CAST_16U(t) (IMP_U16)(!((t) & ~65535) ? (t) : (t) > 0 ? 65535 : 0)
#define  IMP_CAST_16S(t) (short)(!(((t)+32768) & ~65535) ? (t) : (t) > 0 ? 32767 : -32768)
#define  IMP_CAST_32S(t) (int)(t)
#define  IMP_CAST_64S(t) (IMP_S64)(t)
#define  IMP_CAST_32F(t) (float)(t)
#define  IMP_CAST_64F(t) (double)(t)


#define IMP_MEMCPY_AUTO( dst, src, len )                                             \
{                                                                                   \
    unsigned int _imp_memcpy_i_, _imp_memcpy_len_ = (len);                                \
    char* _imp_memcpy_dst_ = (char*)(dst);                                          \
    const char* _imp_memcpy_src_ = (const char*)(src);                              \
    if( (_imp_memcpy_len_ & (sizeof(int)-1)) == 0 )                                 \
    {                                                                               \
	assert( ((size_t)_imp_memcpy_src_&(sizeof(int)-1)) == 0 &&                  \
	((size_t)_imp_memcpy_dst_&(sizeof(int)-1)) == 0 );                  \
	for( _imp_memcpy_i_ = 0; _imp_memcpy_i_ < _imp_memcpy_len_;                 \
	_imp_memcpy_i_+=sizeof(int) )                                           \
        {                                                                           \
		*(int*)(_imp_memcpy_dst_+_imp_memcpy_i_) =                              \
		*(const int*)(_imp_memcpy_src_+_imp_memcpy_i_);                         \
        }                                                                           \
    }                                                                               \
    else                                                                            \
    {                                                                               \
	for(_imp_memcpy_i_ = 0; _imp_memcpy_i_ < _imp_memcpy_len_; _imp_memcpy_i_++)\
	_imp_memcpy_dst_[_imp_memcpy_i_] = _imp_memcpy_src_[_imp_memcpy_i_];    \
    }                                                                               \
}


/** mat定义 */
typedef struct IMP_Mat
{
    int type;                       /**< 类型 */
    int step;                       /**< 步长 */
	
    int* refcount;                  /**< for internal use only */
    int hdr_refcount;               /**< for internal use only */
	
	/** 指针 */
    union
    {
        unsigned char* ptr;         /**< char类型 */
        short* s;                   /**< short类型 */
        int* i;                     /**< int类型 */
        float* fl;                  /**< float类型 */
        double* db;                 /**< double类型 */
    } data;
	
#ifdef __cplusplus
	/** 高度 */
    union
    {
        int rows;                   /**< 高度 */           
        int height;                 /**< 高度 */
    };
	/** 宽度 */
    union
    {
        int cols;                   /**< 宽度 */
        int width;                  /**< 宽度 */
    };
#else
    int rows;                       /**< 高度 */
    int cols;                       /**< 宽度 */
#endif
}IMP_Mat;

/** Scalar定义 */
typedef struct IMP_Scalar
{
    double val[4];                  /** Scalar数据 */
}IMP_Scalar;

/**
* scalar
* @param val0123 scalar数据
* @return scalar
*/
IMP_Scalar  IMP_ScalarAll( double val0123 );
/**
* 矩阵构造
* @param rows 高度
* @param cols 宽度
* @param type 类型
* @param data 数据
* @return IMP_Mat 矩阵
*/
IMP_EXPORTS IMP_Mat IMP_Mat_Construct( int rows, int cols, int type, void* data);
/** 指针对齐 */
IMP_EXPORTS void* IMP_AlignPtr( const void* ptr, int align );
/** 指针对齐 */
IMP_EXPORTS int IMP_Align( int size, int align );
/**
* 创建矩阵
* @param rows 高度
* @param cols 宽度
* @param type 类型
* @return IMP_Mat 矩阵
*/
IMP_EXPORTS IMP_Mat* IMP_CreateMat( int rows, int cols, int type );
/**
* 释放矩阵
* @return array 矩阵
*/
IMP_EXPORTS void IMP_ReleaseMat( IMP_Mat** array );
/** dst(idx) = value */
IMP_EXPORTS void IMP_Set( void* arr, IMP_Scalar value, const void* maskarr );
/** 取整 */
IMP_EXPORTS int IMP_Round(double f);

#ifdef __cplusplus
}
#endif


#endif/*_IMP_VFD_MATRIX_H_*/

/*@}*/