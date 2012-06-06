
#ifndef _IMP_GA_CARRAY_H_
#define _IMP_GA_CARRAY_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif



#define IMP_SET_RECT(stRc,s16Xs,s16Ys,s16Xe,s16Ye) {stRc.s16X1 = s16Xs; stRc.s16Y1 = s16Ys; stRc.s16X2 = s16Xe; stRc.s16Y2 = s16Ye;}


#ifdef IMP_ARM_PEA
#define IMP_CIARRAY_ELEM_NUM	128
#else
#define IMP_CIARRAY_ELEM_NUM	128
#endif


typedef struct impGA_CYCLE_INTEGER_ARRAY_S
{
	IMP_S32 as32Elems[IMP_CIARRAY_ELEM_NUM];
	IMP_U32 u32Number;
} GA_CYCLE_INTEGER_ARRAY_S;


IMP_INLINE IMP_VOID IMP_GA_CArrayClear( GA_CYCLE_INTEGER_ARRAY_S *pstCArray )
{
	memset( pstCArray, 0, sizeof(GA_CYCLE_INTEGER_ARRAY_S) );
}

IMP_INLINE IMP_VOID IMP_GA_CArrayPushElem( GA_CYCLE_INTEGER_ARRAY_S *pstCArray, IMP_S32 s32Elem )
{
	pstCArray->u32Number++;
	pstCArray->as32Elems[(IMP_U32)(pstCArray->u32Number - 1) & (IMP_CIARRAY_ELEM_NUM - 1)] = s32Elem;
}

IMP_INLINE IMP_S32 IMP_GA_CArrayGetElem( GA_CYCLE_INTEGER_ARRAY_S *pstCArray, IMP_S32 s32Index )
{
	return pstCArray->as32Elems[(IMP_U32)(pstCArray->u32Number - 1 + s32Index) & (IMP_CIARRAY_ELEM_NUM - 1)];
}

IMP_INLINE IMP_U32 IMP_GA_CArrayGetLength( GA_CYCLE_INTEGER_ARRAY_S *pstCArray )
{
	return pstCArray->u32Number <= IMP_CIARRAY_ELEM_NUM ? pstCArray->u32Number : IMP_CIARRAY_ELEM_NUM;
}

#ifdef __cplusplus
}
#endif

#endif
