/** 
* \defgroup 队列
* 队列定义及操作
* @ingroup VFD
* @author 北京银瀑技术
* @version 1.0
* @data 2009-2010
*/
/*@{*/

#ifndef _IMP_VFD_XTYPE_H_
#define _IMP_VFD_XTYPE_H_

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>
#include "imp_vfd_matrix.h"

#ifdef __cplusplus
extern "C"
{
#endif

#if defined WIN32 || defined WIN64
#define IMP_CDECL __cdecl
#define IMP_STDCALL __stdcall
#else
#define IMP_CDECL
#define IMP_STDCALL
#endif
	
	
#ifndef IMP_API
#define IMP_API(type,declspec,name,args)                        \
    /** function pointer */                                      \
    typedef type (declspec* name##_t) args;                     \
    extern name##_t name##_p;                                   \
	type declspec name args;
#endif
	
#define IMP_API_EX(type,name,ipp_name,ipp_search_modules,args)  \
    IMP_API(type,IMP_STDCALL,name,args) 
	
	
#define IMP_DEPTH_1U     1
#define IMP_DEPTH_8U     8
#define IMP_DEPTH_16U   16
#define IMP_DEPTH_32F   32

#define IMP_PLUGINS1(lib1) ((lib1)&15)
#define IMP_PLUGINS2(lib1,lib2) (((lib1)&15)|(((lib2)&15)<<4))
#define IMP_PLUGINS3(lib1,lib2,lib3) (((lib1)&15)|(((lib2)&15)<<4)|(((lib2)&15)<<8))
	
#define IMP_PLUGIN_IPPCV     2 /** IPP: computer vision */


#define IMP_STORAGE_MAGIC_VAL    0x42890000

/** default storage block size */
#define  IMP_STORAGE_BLOCK_SIZE   ((1<<16) - 128)

/** default alignment for dynamic data strucutures, resided in storages. */
#define  IMP_STRUCT_ALIGN    ((int)sizeof(double))
/****************************************************************************************\
*                                   Dynamic Data structures                              *
\****************************************************************************************/

/******************************** Memory storage ****************************************/

typedef struct IMP_MemBlock
{
    struct IMP_MemBlock*  prev;
    struct IMP_MemBlock*  next;
}
IMP_MemBlock;

#define IMP_STORAGE_MAGIC_VAL    0x42890000

typedef struct IMP_MemStorage
{
    int signature;
    IMP_MemBlock* bottom;/**< first allocated block */
    IMP_MemBlock* top;   /**< current memory block - top of the stack */
    struct  IMP_MemStorage* parent; /**< borrows new blocks from */
    int block_size;  /**< block size */
    int free_space;  /**< free space in the current block */
}
IMP_MemStorage;

#define IMP_IS_STORAGE(storage)  \
    ((storage) != NULL &&       \
    (((IMP_MemStorage*)(storage))->signature & IMP_MAGIC_MASK) == IMP_STORAGE_MAGIC_VAL)


typedef struct IMP_MemStoragePos
{
    IMP_MemBlock* top;
    int free_space;
}
IMP_MemStoragePos;

typedef struct IMP_String
{
    int len;
    char* ptr;
}
IMP_String;

/************************************* Slice ******************************************/

typedef struct IMP_Slice
{
    int  start_index, end_index;
}
IMP_Slice;

IMP_Slice IMP_Slice_Construct( int start, int end );

#define IMP_WHOLE_SEQ_END_INDEX 0x3fffffff
#define IMP_WHOLE_SEQ  IMP_Slice_Construct(0, IMP_WHOLE_SEQ_END_INDEX)

/****************************************************************************************\
*                                    Sequence types                                      *
\****************************************************************************************/

#define IMP_SEQ_MAGIC_VAL             0x42990000

#define IMP_IS_SEQ(seq) \
    ((seq) != NULL && (((IMP_Seq*)(seq))->flags & IMP_MAGIC_MASK) == IMP_SEQ_MAGIC_VAL)

#define IMP_SET_MAGIC_VAL             0x42980000
#define IMP_IS_SET(set) \
    ((set) != NULL && (((IMP_Seq*)(set))->flags & IMP_MAGIC_MASK) == IMP_SET_MAGIC_VAL)

#define IMP_SEQ_ELTYPE_BITS           9
#define IMP_SEQ_ELTYPE_MASK           ((1 << IMP_SEQ_ELTYPE_BITS) - 1)

#define IMP_SEQ_ELTYPE_POINT          IMP_32SC2  /** (x,y) */
#define IMP_SEQ_ELTYPE_CODE           IMP_8UC1   /** freeman code: 0..7 */
#define IMP_SEQ_ELTYPE_GENERIC        0
#define IMP_SEQ_ELTYPE_PTR            IMP_USRTYPE1
#define IMP_SEQ_ELTYPE_PPOINT         IMP_SEQ_ELTYPE_PTR  /** &(x,y) */
#define IMP_SEQ_ELTYPE_INDEX          IMP_32SC1  /** #(x,y) */
#define IMP_SEQ_ELTYPE_GRAPH_EDGE     0  /** &next_o, &next_d, &vtx_o, &vtx_d */
#define IMP_SEQ_ELTYPE_GRAPH_VERTEX   0  /** first_edge, &(x,y) */
#define IMP_SEQ_ELTYPE_TRIAN_ATR      0  /** vertex of the binary tree   */
#define IMP_SEQ_ELTYPE_CONNECTED_COMP 0  /** connected component  */
#define IMP_SEQ_ELTYPE_POINT3D        IMP_32FC3  /** (x,y,z)  */

#define IMP_SEQ_KIND_BITS        3
#define IMP_SEQ_KIND_MASK        (((1 << IMP_SEQ_KIND_BITS) - 1)<<IMP_SEQ_ELTYPE_BITS)

/* types of sequences */
#define IMP_SEQ_KIND_GENERIC     (0 << IMP_SEQ_ELTYPE_BITS)
#define IMP_SEQ_KIND_CURVE       (1 << IMP_SEQ_ELTYPE_BITS)
#define IMP_SEQ_KIND_BIN_TREE    (2 << IMP_SEQ_ELTYPE_BITS)

/** types of sparse sequences (sets) */
#define IMP_SEQ_KIND_GRAPH       (3 << IMP_SEQ_ELTYPE_BITS)
#define IMP_SEQ_KIND_SUBDIV2D    (4 << IMP_SEQ_ELTYPE_BITS)

#define IMP_SEQ_FLAG_SHIFT       (IMP_SEQ_KIND_BITS + IMP_SEQ_ELTYPE_BITS)

/** flags for curves */
#define IMP_SEQ_FLAG_CLOSED     (1 << IMP_SEQ_FLAG_SHIFT)
#define IMP_SEQ_FLAG_SIMPLE     (2 << IMP_SEQ_FLAG_SHIFT)
#define IMP_SEQ_FLAG_CONVEX     (4 << IMP_SEQ_FLAG_SHIFT)
#define IMP_SEQ_FLAG_HOLE       (8 << IMP_SEQ_FLAG_SHIFT)

/** flags for graphs */
#define IMP_GRAPH_FLAG_ORIENTED (1 << IMP_SEQ_FLAG_SHIFT)

#define IMP_GRAPH               IMP_SEQ_KIND_GRAPH
#define IMP_ORIENTED_GRAPH      (IMP_SEQ_KIND_GRAPH|IMP_GRAPH_FLAG_ORIENTED)

/** point sets */
#define IMP_SEQ_POINT_SET       (IMP_SEQ_KIND_GENERIC| IMP_SEQ_ELTYPE_POINT)
#define IMP_SEQ_POINT3D_SET     (IMP_SEQ_KIND_GENERIC| IMP_SEQ_ELTYPE_POINT3D)
#define IMP_SEQ_POLYLINE        (IMP_SEQ_KIND_CURVE  | IMP_SEQ_ELTYPE_POINT)
#define IMP_SEQ_POLYGON         (IMP_SEQ_FLAG_CLOSED | IMP_SEQ_POLYLINE )
#define IMP_SEQ_CONTOUR         IMP_SEQ_POLYGON
#define IMP_SEQ_SIMPLE_POLYGON  (IMP_SEQ_FLAG_SIMPLE | IMP_SEQ_POLYGON  )

/** chain-coded curves */
#define IMP_SEQ_CHAIN           (IMP_SEQ_KIND_CURVE  | IMP_SEQ_ELTYPE_CODE)
#define IMP_SEQ_CHAIN_CONTOUR   (IMP_SEQ_FLAG_CLOSED | IMP_SEQ_CHAIN)

/* binary tree for the contour */
#define IMP_SEQ_POLYGON_TREE    (IMP_SEQ_KIND_BIN_TREE  | IMP_SEQ_ELTYPE_TRIAN_ATR)

/** sequence of the connected components */
#define IMP_SEQ_CONNECTED_COMP  (IMP_SEQ_KIND_GENERIC  | IMP_SEQ_ELTYPE_CONNECTED_COMP)

/** sequence of the integer numbers */
#define IMP_SEQ_INDEX           (IMP_SEQ_KIND_GENERIC  | IMP_SEQ_ELTYPE_INDEX)

#define IMP_SEQ_ELTYPE( seq )   ((seq)->flags & IMP_SEQ_ELTYPE_MASK)
#define IMP_SEQ_KIND( seq )     ((seq)->flags & IMP_SEQ_KIND_MASK )

/** flag checking */
#define IMP_IS_SEQ_INDEX( seq )      ((IMP_SEQ_ELTYPE(seq) == IMP_SEQ_ELTYPE_INDEX) && \
                                     (IMP_SEQ_KIND(seq) == IMP_SEQ_KIND_GENERIC))

#define IMP_IS_SEQ_CURVE( seq )      (IMP_SEQ_KIND(seq) == IMP_SEQ_KIND_CURVE)
#define IMP_IS_SEQ_CLOSED( seq )     (((seq)->flags & IMP_SEQ_FLAG_CLOSED) != 0)
#define IMP_IS_SEQ_CONVEX( seq )     (((seq)->flags & IMP_SEQ_FLAG_CONVEX) != 0)
#define IMP_IS_SEQ_HOLE( seq )       (((seq)->flags & IMP_SEQ_FLAG_HOLE) != 0)
#define IMP_IS_SEQ_SIMPLE( seq )     ((((seq)->flags & IMP_SEQ_FLAG_SIMPLE) != 0) || \
                                    IMP_IS_SEQ_CONVEX(seq))

/** type checking macros */
#define IMP_IS_SEQ_POINT_SET( seq ) \
    ((IMP_SEQ_ELTYPE(seq) == IMP_32SC2 || IMP_SEQ_ELTYPE(seq) == IMP_32FC2))

#define IMP_IS_SEQ_POINT_SUBSET( seq ) \
    (IMP_IS_SEQ_INDEX( seq ) || IMP_SEQ_ELTYPE(seq) == IMP_SEQ_ELTYPE_PPOINT)

#define IMP_IS_SEQ_POLYLINE( seq )   \
    (IMP_SEQ_KIND(seq) == IMP_SEQ_KIND_CURVE && IMP_IS_SEQ_POINT_SET(seq))

#define IMP_IS_SEQ_POLYGON( seq )   \
    (IMP_IS_SEQ_POLYLINE(seq) && IMP_IS_SEQ_CLOSED(seq))

#define IMP_IS_SEQ_CHAIN( seq )   \
    (IMP_SEQ_KIND(seq) == IMP_SEQ_KIND_CURVE && (seq)->elem_size == 1)

#define IMP_IS_SEQ_CONTOUR( seq )   \
    (IMP_IS_SEQ_CLOSED(seq) && (IMP_IS_SEQ_POLYLINE(seq) || IMP_IS_SEQ_CHAIN(seq)))

#define IMP_IS_SEQ_CHAIN_CONTOUR( seq ) \
    (IMP_IS_SEQ_CHAIN( seq ) && IMP_IS_SEQ_CLOSED( seq ))

#define IMP_IS_SEQ_POLYGON_TREE( seq ) \
    (IMP_SEQ_ELTYPE (seq) ==  IMP_SEQ_ELTYPE_TRIAN_ATR &&    \
    IMP_SEQ_KIND( seq ) ==  IMP_SEQ_KIND_BIN_TREE )

#define IMP_IS_GRAPH( seq )    \
    (IMP_IS_SET(seq) && IMP_SEQ_KIND((IMP_SET_S*)(seq)) == IMP_SEQ_KIND_GRAPH)

#define IMP_IS_GRAPH_ORIENTED( seq )   \
    (((seq)->flags & IMP_GRAPH_FLAG_ORIENTED) != 0)

#define IMP_IS_SUBDIV2D( seq )  \
    (IMP_IS_SET(seq) && IMP_SEQ_KIND((IMP_SET_S*)(seq)) == IMP_SEQ_KIND_SUBDIV2D)


/*********************************** Sequence *******************************************/

typedef struct IMP_SeqBlock
{
    struct IMP_SeqBlock*  prev; /**< previous sequence block */
    struct IMP_SeqBlock*  next; /**< next sequence block */
    int    start_index;       /**< index of the first element in the block +
                                 sequence->first->start_index */
    int    count;             /**< number of elements in the block */
    char*  data;              /**< pointer to the first element of the block */
}
IMP_SeqBlock;


#define	IMP_TREE_NODE_FIELDS(node_type)                          \
    int       flags;         /**< micsellaneous flags */          \
    int       header_size;   /**< size of sequence header */      \
    struct    node_type* h_prev; /**< previous sequence */        \
    struct    node_type* h_next; /**< next sequence */            \
    struct    node_type* v_prev; /**< 2nd previous sequence */    \
    struct    node_type* v_next  /**< 2nd next sequence */

/**
   Read/Write sequence.
   Elements can be dynamically inserted to or deleted from the sequence.
*/
#define IMP_SEQUENCE_FIELDS()                                            \
    IMP_TREE_NODE_FIELDS(IMP_Seq);                                         \
    int       total;          /**< total number of elements */            \
    int       elem_size;      /**< size of sequence element in bytes */   \
    char*     block_max;      /**< maximal bound of the last block */     \
    char*     ptr;            /**< current write pointer */               \
    int       delta_elems;    /**< how many elements allocated when the seq grows */  \
    IMP_MemStorage* storage;    /**< where the seq is stored */             \
    IMP_SeqBlock* free_blocks;  /**< free blocks list */                    \
    IMP_SeqBlock* first; /**< pointer to the first sequence block */

typedef struct IMP_Seq
{
    IMP_SEQUENCE_FIELDS()
}
IMP_Seq;


/****************************************************************************************/
/*                            Sequence writer & reader                                  */
/****************************************************************************************/

#define IMP_SEQ_WRITER_FIELDS()                                     \
    int          header_size;                                      \
    IMP_Seq*       seq;        /**< the sequence written */            \
    IMP_SeqBlock*  block;      /**< current block */                   \
    char*        ptr;        /**< pointer to free space */           \
    char*        block_min;  /**< pointer to the beginning of block*/\
char*        block_max;  /**< pointer to the end of block */

typedef struct IMP_SeqWriter
{
    IMP_SEQ_WRITER_FIELDS()
}
IMP_SeqWriter;


#define IMP_SEQ_READER_FIELDS()                                      \
    int          header_size;                                       \
    IMP_Seq*       seq;        /**< sequence, beign read */             \
    IMP_SeqBlock*  block;      /**< current block */                    \
    char*        ptr;        /**< pointer to element be read next */  \
    char*        block_min;  /**< pointer to the beginning of block */\
    char*        block_max;  /**< pointer to the end of block */      \
    int          delta_index;/**< = seq->first->start_index   */      \
char*        prev_elem;  /**< pointer to previous element */


typedef struct IMP_SeqReader
{
    IMP_SEQ_READER_FIELDS()
}IMP_SeqReader;


/** move reader position forward */
#define IMP_NEXT_SEQ_ELEM( elem_size, reader )                 \
{                                                             \
    if( ((reader).ptr += (elem_size)) >= (reader).block_max ) \
{                                                         \
	IMP_ChangeSeqBlock( &(reader), 1 );                     \
}                                                         \
}

/** move reader position backward */
#define IMP_PREV_SEQ_ELEM( elem_size, reader )                \
{                                                            \
    if( ((reader).ptr -= (elem_size)) < (reader).block_min ) \
{                                                        \
	IMP_ChangeSeqBlock( &(reader), -1 );                   \
}                                                        \
}

#define IMP_WRITE_SEQ_ELEM( elem, writer )             \
{                                                     \
    assert( (writer).seq->elem_size == sizeof(elem)); \
    if( (writer).ptr >= (writer).block_max )          \
{                                                 \
	IMP_CreateSeqBlock( &writer);                   \
}                                                 \
    assert( (writer).ptr <= (writer).block_max - sizeof(elem));\
    memcpy((writer).ptr, &(elem), sizeof(elem));      \
    (writer).ptr += sizeof(elem);                     \
}
/* a < b ? -1 : a > b ? 1 : 0 */
typedef int (IMP_CDECL* IMP_CmpFunc)(const void* a, const void* b, void* userdata );



/**
  Set.
  Order is not preserved. There can be gaps between sequence elements.
  After the element has been inserted it stays in the same place all the time.
  The MSB(most-significant or sign bit) of the first field (flags) is 0 iff the element exists.
*/
#define IMP_SET_ELEM_FIELDS(elem_type)   \
    int  flags;                         \
    struct elem_type* next_free;

typedef struct IMP_SetElem
{
    IMP_SET_ELEM_FIELDS(IMP_SetElem)
}
IMP_SetElem;

#define IMP_SET_FIELDS()      \
    IMP_SEQUENCE_FIELDS()     \
    IMP_SetElem* free_elems;   \
    int active_count;

typedef struct IMP_SET_S
{
    IMP_SET_FIELDS()
}IMP_SET_S;


#define IMP_SET_ELEM_IDX_MASK   ((1 << 26) - 1)
#define IMP_SET_ELEM_FREE_FLAG  (1 << (sizeof(int)*8-1))
/** Checks whether the element pointed by ptr belongs to a set or not */
#define IMP_IS_SET_ELEM( ptr )  (((IMP_SetElem*)(ptr))->flags >= 0)


#define IMP_MEMCPY_INT( dst, src, len )                                              \
{                                                                                   \
    unsigned int _imp_memcpy_i_, _imp_memcpy_len_ = (len);                                \
    int* _imp_memcpy_dst_ = (int*)(dst);                                            \
    const int* _imp_memcpy_src_ = (const int*)(src);                                \
    assert( ((size_t)_imp_memcpy_src_&(sizeof(int)-1)) == 0 &&                      \
	((unsigned int)_imp_memcpy_dst_&(sizeof(int)-1)) == 0 );                      \
	\
    for(_imp_memcpy_i_=0;_imp_memcpy_i_<_imp_memcpy_len_;_imp_memcpy_i_++)          \
	_imp_memcpy_dst_[_imp_memcpy_i_] = _imp_memcpy_src_[_imp_memcpy_i_];        \
}


/** Iteration through the sequence tree */
typedef struct IMP_TreeNodeIterator
{
    const void* node;
    int level;
    int max_level;
}IMP_TreeNodeIterator;


/** creates root memory storage */
IMP_MemStorage* IMP_CreateMemStorage( int block_size );
/** creates child memory storage */
IMP_MemStorage * IMP_CreateChildMemStorage( IMP_MemStorage * parent );

/** releases memory storage */
void IMP_ReleaseMemStorage( IMP_MemStorage** storage );

/** creates empty sequence */
 IMP_Seq* IMP_CreateSeq( int seq_flags, int header_size, int elem_size, IMP_MemStorage * storage );
/** pushes element to the sequence */
char* IMP_SeqPush( IMP_Seq *seq, void *element );

/** finds sequence element by its index */
char* IMP_GetSeqElem( const IMP_Seq *seq, int index );


/** the function splits the input sequence or set into one or more equivalence classes.
 is_equal(a,b,...) returns non-zero if the two sequence elements
 belong to the same class. the function returns sequence of integers -
 0-based class indexes for each element. */
int IMP_SeqPartition( const IMP_Seq* seq, IMP_MemStorage* storage, IMP_Seq** labels,
                IMP_CmpFunc is_equal, void* userdata );

#ifdef __cplusplus
}
#endif
#endif /*_IMP_VFD_XTYPE_H_*/

/*@}*/
