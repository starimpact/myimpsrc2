#ifndef _IMP_GA_MATH_H_
#define _IMP_GA_MATH_H_

#include "imp_algo_type.h"

#ifdef __cplusplus
extern "C"
{
#endif


#define IMP_PI   3.1415926535897932384626433832795

#define IMP_LOG2 0.69314718055994530941723212145818

#define IMP_SWAP(a,b,t) ((t) = (a), (a) = (b), (b) = (t))

#ifndef IMP_MIN
#define IMP_MIN(a,b)  ((a) > (b) ? (b) : (a))
#endif

#ifndef min
#define min IMP_MIN
#endif

#ifndef IMP_MAX
#define IMP_MAX(a,b)  ((a) < (b) ? (b) : (a))
#endif

#ifndef max
#define max IMP_MAX
#endif



#define  IMP_IMIN(a, b)  ((a) ^ (((a)^(b)) & (((a) < (b)) - 1)))

#define  IMP_IMAX(a, b)  ((a) ^ (((a)^(b)) & (((a) > (b)) - 1)))


#define SIGN(x) (( x > 0 ) - ( x < 0 ))

#define SIGN2(x) (( x >> 31 ) | ( (unsigned int)( -x ) >> 31 ))


IMP_U16 ipSqrtInt( IMP_U32 M );


IMP_VOID ipBubbleSort(IMP_S32 *a,IMP_S32 n);

IMP_VOID ipChoiseSort(IMP_S32 *a,IMP_S32 n);

IMP_VOID ipQuickSort(IMP_U8 *a,IMP_S32 i,IMP_S32 j);

IMP_VOID ipInsertSort(IMP_S32 *a,IMP_S32 n);







#define DO1(buf) crc = crc_table[((IMP_S32)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);


IMP_VOID ipGenerateCRC(IMP_U8 * crc_buffer, IMP_S32 len);

IMP_S32 ipCheckCRC(IMP_U8 * crc_buffer, IMP_S32 len);


#ifdef __cplusplus
}
#endif

#endif
