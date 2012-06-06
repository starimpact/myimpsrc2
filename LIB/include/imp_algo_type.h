/**
* \defgroup Ω”ø⁄
* Ω”ø⁄
* @author ±±æ©“¯∆Ÿºº ı
* @version 2.1
* @data 2009-2012
*/
/*@{*/

/**
* \defgroup Ω·ππ¿‡–Õ∂®“Â
* À„∑® ˝æ›¿‡–Õ∂®“Â
* @author ±±æ©“¯∆Ÿºº ı
* @version 2.1
* @data 2009-2012
*/
/*@{*/
#ifndef _IMP_ALGO_TYPE_H_
#define _IMP_ALGO_TYPE_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>

#define PRINT_DBG 0

#ifdef __cplusplus
extern "C"
{
#endif

typedef unsigned char IMP_U8;
typedef unsigned char IMP_UCHAR;
typedef unsigned short IMP_U16;
typedef unsigned int IMP_U32;
typedef char IMP_S8;
typedef short IMP_S16;
typedef int IMP_S32;
#ifndef _M_IX86
	typedef unsigned long long IMP_U64;
	typedef long long IMP_S64;
#else
	typedef __int64 IMP_U64;
	typedef __int64 IMP_S64;
#endif
typedef char IMP_CHAR;
typedef char* IMP_PCHAR;
typedef float IMP_FLOAT;
typedef double IMP_DOUBLE;
typedef void IMP_VOID;
typedef unsigned long IMP_SIZE_T;
typedef unsigned long IMP_LENGTH_T;

typedef void *IMP_MODULE_HANDLE;

typedef enum
{
	IMP_FALSE = 0,
	IMP_TRUE = 1,
} IMP_BOOL;

#ifndef NULL
#define NULL 0L
#endif
#define IMP_NULL 0L
#define IMP_NULL_PTR 0L
#define IMP_SUCCESS 0
#define IMP_FAILURE (-1)

/** IMP_EXPORTS */
#if defined(IMP_API_EXPORTS)
#define IMP_EXPORTS __declspec(dllexport)
#elif defined(IMP_API_IMPORTS)
#define IMP_EXPORTS __declspec(dllimport)
#else
#define IMP_EXPORTS extern
#endif


#define IMP_ASSERT(x) ( assert(x) )
#define RESTRICT

/** IMP_INLINE */
#ifndef IMP_INLINE
#if defined __cplusplus
#define IMP_INLINE inline
#elif (defined WIN32 || defined WIN64) && !defined __GNUC__
#define IMP_INLINE __inline
#else
#define IMP_INLINE static
#endif
#endif



/** À„∑®ƒ£øÈ */
typedef enum impALGO_MODULE_E
{
	IMP_NONE_AGLO_MODULE= 0x00000000,		/**< À„∑®ƒ£øÈ */
	IMP_PEA_AGLO_MODULE = 0x00000001,		/**< PEAÀ„∑®ƒ£øÈ */
	IMP_AVD_AGLO_MODULE = 0x00000002,		/**< AVDÀ„∑®ƒ£øÈ */
	IMP_VFD_AGLO_MODULE = 0x00000004,		/**< VFDÀ„∑®ƒ£øÈ */
	IMP_AAI_AGLO_MODULE = 0x00000008		/**< AAIÀ„∑®ƒ£øÈ */
}ALGO_MODULE_E;

/** À„∑®ƒ£øÈ */
typedef enum impALGO_STATUS_E
{
	IMP_AGLO_STATUS_ARM 	= 0,	/**< À„∑®≤º∑¿◊¥Ã¨ */
	IMP_AGLO_STATUS_DISARM 	= 1		/**< À„∑®≥∑∑¿◊¥Ã¨ */
}ALGO_STATUS_E;

/** ø‚œ‡πÿ–≈œ¢ */
typedef struct impLIB_INFO_S
{
	IMP_U32 pu32Version;         /**< ∞Ê±æ∫≈ */
	IMP_U32 pu32FuncAuthorized;  /**<  ⁄»®π¶ƒ‹ */
} LIB_INFO_S;

/** µ„Ω·ππ∂®“Â */
typedef struct impIMP_POINT_S
{
	IMP_S16 s16X;/**< x */
	IMP_S16 s16Y;/**< y */
}IMP_POINT_S;

/** µ„Ω·ππ∂®“Â */
typedef struct impIMP_POINT32S_S
{
	IMP_S32 s32X;/**< x */
	IMP_S32 s32Y;/**< y */
}IMP_POINT32S_S;

/** µ„Ω·ππ∂®“Â */
typedef struct impIMP_POINT32F_S
{
	IMP_FLOAT f32X;/**< x */
	IMP_FLOAT f32Y;/**< y */
}IMP_POINT32F_S;

/** 3Œ¨µ„Ω·ππ∂®“Â */
typedef struct impPOINT3D_S
{
	IMP_S32 s32X;/**< x */
	IMP_S32 s32Y;/**< y */
	IMP_S32 s32Z;/**< z */
} POINT3D_S;

/** œﬂ∂ŒΩ·ππ∂®“Â */
typedef struct impLINE_S
{
	IMP_POINT_S stPs; /**< ∆ ºµ„ */
	IMP_POINT_S stPe; /**< Ω· ¯µ„ */
} LINE_S;

/** æÿ–ŒΩ·ππ∂®“Â */
typedef struct impIMP_RECT_S
{
	IMP_S16 s16X1;  /**< ◊Û…œΩ«x◊¯±Í */
	IMP_S16 s16Y1;  /**< ◊Û…œΩ«y◊¯±Í */
	IMP_S16 s16X2;  /**< ”“œ¬Ω«x◊¯±Í */
	IMP_S16 s16Y2;  /**< ”“œ¬Ω«y◊¯±Í */
} IMP_RECT_S;

/** ◊¥Ã¨∂®“Â */
typedef enum impSTATUS_E
{
	IMP_STATUS_CHECK_LICENSE_TIMEOUT =-3, /**< —È÷§license ß∞‹ */
	IMP_STATUS_CHECK_LICENSE_FAILED =-2,  /**< —È÷§license ß∞‹ */
	IMP_STATUS_READ_MAC_FAILED = -1,      /**< ∂¡»°MAC ß∞‹ */
	IMP_STATUS_OK = 1,                    /**< ≥…π¶ */
	IMP_STATUS_SKIP,                      /**< Ã¯π˝ */
	IMP_STATUS_FALSE,                     /**< ≤ª÷ß≥÷ */
	IMP_STATUS_UNSUPPORT
} STATUS_E;


/** æ‰±˙∂®“Â */
typedef void *IMP_HANDLE;

/** ƒ£øÈ ˝æ›≥§∂»∂®“Â */
#define IMP_MODULE_DATLEN	256

/** IMPƒø±Í∂®“Â */
typedef struct impOBJ_S
{
	IMP_U8 au8Buffer[IMP_MODULE_DATLEN];  /**< BUFFER∂®“Â */
} OBJ_S;

/** ƒ⁄¥Êπ‹¿Ì∂®“Â*/
#define IMP_MEM_ITEM_CNT	4

/** ƒ⁄¥Ê¿‡–Õ∂®“Â*/
typedef enum impMEM_TYPE_E
{
	IMP_MEM_TYPE_IRAM = 0, /**< ƒ⁄≤øRam */
	IMP_MEM_TYPE_ERAM	   /**< Õ‚≤øRam */
} MEM_TYPE_E;

/** ƒ⁄¥Êµ•‘™Ω·ππ∂®“Â */
typedef struct impMEM_ITEM_S
{
	IMP_U32 u32Size;		/**< ¥Û–° */
	IMP_U32 u32Type;		/**< ¿‡–Õ */
	IMP_VOID *pMem;			/**<  ˝æ›÷∏’Î */
} MEM_ITEM_S;

/** ƒ⁄¥ÊΩ·ππ∂®“Â*/
typedef struct impMEM_SET_S
{
	IMP_U32 u32ImgW;                      /**< øÌ∂» */
	IMP_U32 u32ImgH;                      /**< ∏ﬂ∂» */
	IMP_U32 u32MemNum;                    /**<  ˝¡ø */
	MEM_ITEM_S stMems[IMP_MEM_ITEM_CNT];  /**<  ˝◊È */
} MEM_SET_S;

/**  ˝æ›ª∫¥ÊΩ·ππ∂®“Â */
typedef struct impDATA_BUF_S
{
	IMP_S32 s32BufLen;       /**< ª∫¥Ê≥§∂» */
	IMP_S32 s32DatLen;       /**<  ˝æ›≥§∂» */
	IMP_U8 *pu8Buf;          /**< ÷∏’Î */
} DATA_BUF_S;

/** »´æ÷≤Œ ˝ */
typedef struct impGLOBAL_PARA_S
{
	IMP_S32 s32TimeUnit;	/**<  ÕºœÒ ±º‰µ•Œª */
	IMP_S32 s32FuncCode;	/**<  π¶ƒ‹¬Î */
} GLOBAL_PARA_S;

/** πÊ‘Ú≤Œ ˝ */
#define IMP_PARA_RULE_BUFLEN		(1024 * 64)

/** πÊ‘Ú≤Œ ˝ ˝æ› */
typedef struct impRULE_PARA_DATA_S
{
	DATA_BUF_S stVanaData;    /**<   ˝æ› */
} RULE_PARA_DATA_S;

/** ∏ﬂº∂≤Œ ˝count */
#define IMP_PARA_ADVBUF_BUFCNT	16
/** ∏ﬂº∂≤Œ ˝length */
#define IMP_PARA_ADVBUF_BUFLEN	(1024 * 8)

/** ∏ﬂº∂≤Œ ˝ ˝æ› */
typedef struct impADVANCE_PARA_S
{
	DATA_BUF_S astAdvDats[IMP_PARA_ADVBUF_BUFCNT];  /** ∏ﬂº∂≤Œ ˝ ˝æ› ˝◊È */
} ADVANCE_PARA_S;

/** Õ‚≤ø≤Œ ˝Ω·ππ∂®“Â */
typedef struct impEXTERNAL_PARA_S
{
	IMP_U32         u32Type;            /**<  ¿‡–Õ */
	GLOBAL_PARA_S	stGlobal;			/**<  »´æ÷≤Œ ˝ */
	RULE_PARA_DATA_S    stRule;		    /**<  ≥°æ∞ ˝æ› */
	ADVANCE_PARA_S	stAdvance;			/**< ∏ﬂº∂≤Œ ˝£®Ωˆø™∑¢»À‘±ø……Ë÷√£© */
} EXTERNAL_PARA_S;

/** ƒ⁄≤ø≤Œ ˝Ω·ππ∂®“Â */
typedef struct impINNER_PARA_S
{
	IMP_VOID		*pRule;				/**<  ≥°æ∞≤Œ ˝ */
	GLOBAL_PARA_S	stGlobal;			/**<  »´æ÷≤Œ ˝ */
	ADVANCE_PARA_S	stAdvance;			/**<  ∏ﬂº∂≤Œ ˝£®Ωˆø™∑¢»À‘±ø……Ë÷√£©*/
} INNER_PARA_S;

/** ≤Œ ˝◊÷∑˚¥Æ‘™Àÿ◊Ó¥Û∏ˆ ˝ */
#define IMP_PARA_STRARRAY_ELEMCNT	16

/** ≤Œ ˝◊÷∑˚¥Æ ˝æ› */
typedef struct impSTR_ARRAY_PARA_S
{
	IMP_S8 *pStrings[IMP_PARA_STRARRAY_ELEMCNT];  /**<  ≤Œ ˝◊÷∑˚¥Æ ˝◊È */
}STR_ARRAY_PARA_S;

/** πÊ‘Ú¿‡–Õ∂®“Â */
typedef enum impRULE_TYPE_E
{
	IMP_RULE_SET_TYPE = 0,  /**<  πÊ‘ÚºØ¿‡–Õ∂®“Â */
} RULE_TYPE_E;

/** πÊ‘ÚºÏ≤È∂®“Â */
typedef struct impRULE_CHECK_S
{
	IMP_U32     u32Type;     /**<  ¿‡–Õ */
	DATA_BUF_S	stScnDat;    /**<  ≥°æ∞ ˝æ› */
	IMP_U32	    u32EnaCode;  /**<   πƒ‹¬Î */
} RULE_CHECK_S;

/** RGBœÒÀÿΩ·ππ∂®“Â */
typedef struct impPIXEL_S
{
	IMP_U8 u8B;    /**< blueÕ®µ¿ */
	IMP_U8 u8G;    /**< greenÕ®µ¿ */
	IMP_U8 u8R;    /**< redÕ®µ¿ */
} PIXEL_S;

/** HSVœÒÀÿΩ·ππ∂®“Â*/
typedef struct impHSV_PIXEL_S
{
	IMP_FLOAT f32H;    /**< blueÕ®µ¿ */
	IMP_FLOAT f32S;    /**< greenÕ®µ¿ */
	IMP_FLOAT f32V;    /**< redÕ®µ¿ */
} HSV_PIXEL_S;

/** HSLœÒÀÿΩ·ππ∂®“Â */
typedef struct impHSL_PIXEL_S
{
	IMP_FLOAT f32H;    /**< hueÕ®µ¿ */
	IMP_FLOAT f32S;    /**< saturationÕ®µ¿ */
	IMP_FLOAT f32L;    /**< luminanceÕ®µ¿ */
} HSL_PIXEL_S;

/** RGBÕºœÒΩ·ππ∂®“Â */
typedef struct impRGB_IMAGE_S
{
	IMP_S32 s32W;      /**< ÕºœÒøÌ∂» */
	IMP_S32 s32H;      /**< ÕºœÒ∏ﬂ∂» */
	IMP_U8 *pu8Data;    /**< ÕºœÒ ˝æ›÷∏’Î */
	IMP_U32 u32Time;   /**<  ±º‰ */
} RGB_IMAGE_S;


/** HSVÕºœÒΩ·ππ∂®“Â */
typedef struct impHSV_IMAGE_S
{
	IMP_S32   s32W;           /**< ÕºœÒøÌ∂» */
	IMP_S32   s32H;           /**< ÕºœÒ∏ﬂ∂» */
	IMP_FLOAT *pf32Data;      /**< ÕºœÒ ˝æ›÷∏’Î */
	IMP_U32   u32Time;        /**<  ±º‰ */
} HSV_IMAGE_S;

/** ª“∂»ÕºœÒΩ·ππ∂®“Â */
typedef struct impGRAY_IMAGE_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂» */
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂» */
	IMP_U8 *pu8Data;        /**< ÕºœÒ ˝æ›÷∏’Î */
} GRAY_IMAGE_S;

/** 16Œªª“∂»ÕºœÒΩ·ππ∂®“Â */
typedef struct impGRAY_IMAGE16_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂» */
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂» */
	IMP_S16 *ps16Data;      /**< ÕºœÒ ˝æ›÷∏’Î */
} GRAY_IMAGE16_S;

/** 32Œªª“∂»ÕºœÒΩ·ππ∂®“Â */
typedef struct impGRAY_IMAGE32_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂» */
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂» */
	IMP_S32 *ps32Data;      /**< ÕºœÒ ˝æ›÷∏’Î */
} GRAY_IMAGE32_S;

/** TMPLÕºœÒΩ·ππ∂®“Â */
typedef struct impTMPL_IMAGE_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂» */
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂» */
	IMP_S32 s32BufLen;      /**< ÕºœÒƒ⁄¥Ê≥§∂» */
	IMP_U8 *pu8Data;        /**< ÕºœÒøÌ∂»ÕºœÒ ˝æ›÷∏’Î */
} TMPL_IMAGE_S;

/** YUV422ÕºœÒ ˝æ›¿‡–Õ */
typedef struct impYUV_IMAGE422_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂»*/
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂»*/
	IMP_U8 *pu8Y;           /**< ÕºœÒY∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U8 *pu8U;           /**< ÕºœÒU∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U8 *pu8V;           /**< ÕºœÒV∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U32 u32Time;        /**<  ±º‰*/
	IMP_U32 u32Flag;        /**< ±Í÷æ*/
} YUV_IMAGE422_S;

/** YUV420ÕºœÒ ˝æ›¿‡–Õ */
typedef struct impYUV_IMAGE420_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂»*/
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂»*/
	IMP_U8 *pu8Y;           /**< ÕºœÒY∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U8 *pu8U;           /**< ÕºœÒU∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U8 *pu8V;           /**< ÕºœÒV∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U32 u32Time;        /**<  ±º‰*/
	IMP_U32 u32Flag;        /**< ±Í÷æ*/
} YUV_IMAGE420_S;

//YUV FORMAT TYPE
typedef enum impYUV_FORMAT_E
{
	YUV_FORMAT_IMP_422 = 0,
	YUV_FORMAT_IMP_420,
	YUV_FORMAT_HIS_SP422,
	YUV_FORMAT_HIS_SP420
} YUV_FORMAT_E;

//UNIVERSAL YUV IMAGE
typedef struct impYUV_IMAGE_S
{
	IMP_S32 s32W;           /**< ÕºœÒøÌ∂»*/
	IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂»*/
	IMP_U8 *pu8Y;           /**< ÕºœÒY∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U8 *pu8U;           /**< ÕºœÒU∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U8 *pu8V;           /**< ÕºœÒV∑÷¡ø ˝æ›÷∏’Î*/
	IMP_U32 u32Time;        /**<  ±º‰*/
	IMP_U32 u32Flag;        /**< ±Í÷æ*/
	YUV_FORMAT_E enFormat;  /**< ∏Ò Ω*/
} YUV_IMAGE_S;

typedef enum impIMAGE_FORMAT_E

{
         IMAGE_FORMAT_IMP_YUV422 = 0,
         IMAGE_FORMAT_IMP_YUV420
 } IMAGE_FORMAT_E;



typedef struct impIMAGE3_S

{

         IMP_S32 s32W;           /**< ÕºœÒøÌ∂»*/
         IMP_S32 s32H;           /**< ÕºœÒ∏ﬂ∂»*/
         IMP_U8 *pu8D1;           /**< ÕºœÒ∑÷¡ø1  ˝æ›÷∏’Î*/
         IMP_U8 *pu8D2;           /**< ÕºœÒ∑÷¡ø2  ˝æ›÷∏’Î*/
         IMP_U8 *pu8D3;           /**< ÕºœÒ∑÷¡ø3  ˝æ›÷∏’Î*/
         IMP_U32 u32Time;        /**<  ±º‰*/
         IMP_U32 u32Flag;        /**< ±Í÷æ*/
         IMAGE_FORMAT_E enFormat;        /**< ∏Ò Ω*/
} IMAGE3_S;



/**  ¬º˛¿‡–Õ */
typedef enum impEVT_TYPE_TOP_E
{
	IMP_EVT_TYPE_UNKNOWN        = 0x00000000,		/**< Œ¥÷™¿‡–Õ */
	IMP_EVT_TYPE_Armed          = 0x00002000,		/**< æØΩ‰∆Ù∂Ø */
	IMP_EVT_TYPE_Disarmed       = 0x00002001,		/**< æØΩ‰Ω‚≥˝ */
	IMP_EVT_TYPE_AlarmCleared   = 0x00002002,		/**< æØ±®«Â≥˝ */
	IMP_EVT_TYPE_SignalLoss     = 0x00003000		/**<  ”∆µ–≈∫≈∂™ ß */
}EVT_TYPE_TOP_E;

/**  ¬º˛◊¥Ã¨ */
typedef enum impEVT_STATUS_E
{
	IMP_EVT_STATUS_NOSTA         = 0,				/**< Œﬁ◊¥Ã¨ */
	IMP_EVT_STATUS_START         = 1,				/**< ø™ º */
	IMP_EVT_STATUS_END           = 2,				/**< Ω· ¯ */
	IMP_EVT_STATUS_PROCEDURE     = 3				/**< π˝≥Ã÷– */
} EVT_STATUS_E;

/**  ”∆µ∑÷Œˆπ¶ƒ‹¬Î */
typedef enum impFUNC_ANALYSIS_E
{
	IMP_FUNC_ABANDUM        = 0x00000002,	/**< “≈∆˙ŒÔºÏ≤‚ */
	IMP_FUNC_OBJSTOLEN      = 0x00000004,	/**< ±ªµ¡ŒÔºÏ≤‚ */
	IMP_FUNC_NOPARKING      = 0x00000008,	/**< ∑«∑®Õ£≥µºÏ≤‚ */
	IMP_FUNC_PERIMETER      = 0x00000010,	/**< ÷‹ΩÁ±£ª§ */
	IMP_FUNC_TRIPWIRE       = 0x00000020,	/**< ∞Ìœﬂ */
	IMP_FUNC_MTRIPWIRE      = 0x00000040,	/**< ∂‡∞Ìœﬂ */
	IMP_FUNC_ABNMLVEL       = 0x00000080,	/**< ∑«’˝≥£ÀŸ∂» */
	IMP_FUNC_LOITER         = 0x00000100,	/**< ≈«ª≤ */
	IMP_FUNC_OSC            = 0X00000200,   //ŒÔ∆∑“≈¡Ù
}FUNC_ANALYSIS_E;

/** ƒø±ÍID◊Ó–°÷µ */
#define IMP_TGT_ID_MIN		0x00000000
/** ƒø±ÍID◊Ó¥Û÷µ */
#define IMP_TGT_ID_MAX		0x1FFFFFFF

/** ƒø±Í¿‡–Õ */
typedef enum impTGT_TYPE_E
{
	IMP_TGT_TYPE_HUMAN          = 0x00000001,	/**< »À */
	IMP_TGT_TYPE_UNKNOWN        = 0x00000002,	/**< Œ¥÷™ */
	IMP_TGT_TYPE_VEHICLE        = 0x00000003,	/**< ≥µ */
	IMP_TGT_TYPE_ANIMAL         = 0x00000004	/**< ∂ØŒÔ */
}TGT_TYPE_E;

/** ƒø±Í ¬º˛ */
typedef enum impTGT_EVENT_E
{
	IMP_TGT_EVENT_UNKNOWN       = 0x00000000,	/**< Œ¥÷™ ¬º˛*/
	IMP_TGT_EVENT_ABANDUM       = 0x00000002,	/**< “≈∆˙ŒÔºÏ≤‚ */
	IMP_TGT_EVENT_OBJSTOLEN     = 0x00000004,	/**< ±ªµ¡ŒÔºÏ≤‚ */
	IMP_TGT_EVENT_NOPARKING     = 0x00000008,	/**< ∑«∑®Õ£≥µºÏ≤‚ */
	IMP_TGT_EVENT_PERIMETER     = 0x00000010,	/**< ÷‹ΩÁ±£ª§ */
	IMP_TGT_EVENT_TRIPWIRE      = 0x00000020,	/**< ∞Ìœﬂ */
	IMP_TGT_EVENT_MTRIPWIRE     = 0x00000040,	/**< ∂‡∞Ìœﬂ */
	IMP_TGT_EVENT_ABNORMALVEL   = 0x00000080,	/**< ∑«’˝≥£ÀŸ∂» */
	IMP_TGT_EVENT_LOITER        = 0x00000100,	/**< ≈«ª≤ */
	IMP_TGT_EVENT_OSC		    = 0x00000200,
	IMP_TGT_EVENT_OSC_R		    = 0x00000400,	/**< OSCøøøøø */
}TGT_EVENT_E;

/** ƒø±Í◊¥Ã¨ */
typedef enum impTGT_STATUS_E
{
	IMP_TGT_STATUS_UNKNOWN		= 0x00000000,	/**< Œ¥÷™◊¥Ã¨ */
	IMP_TGT_STATUS_MEASURED		= 0x00000010,	/**< ƒø±ÍŒª÷√”…≤‚¡øµ√µΩ */
	IMP_TGT_STATUS_PREDICTED	= 0x00000020,	/**< ƒø±ÍŒª÷√”…‘§≤‚µ√µΩ */
	IMP_TGT_STATUS_MOTION		= 0x00000100,	/**< ƒø±Íæﬂ”–‘À∂Ø Ù–‘ */
	IMP_TGT_STATUS_STATIC		= 0x00000200	/**< ƒø±Íµ±«∞ «æ≤÷πµƒ */
}TGT_STATUS_E;

/** ƒø±ÍπÏº£≥§∂» */
#define IMP_MAX_TRAJECT_LEN	40

/** ƒø±ÍπÏº£Ω·ππ∂®“Â */
typedef struct impTGT_TRAJECT_S
{
	IMP_S32 s32Length;			          	    /**< πÏº£≥§∂» */
	IMP_POINT_S	stPoints[IMP_MAX_TRAJECT_LEN];	/**< πÏº£µ„ ˝◊È */
} TGT_TRAJECT_S;

/** ƒø±Í ˝æ› */
typedef struct impTGT_MOTION_ITEM_S
{
	IMP_S32	  	  s32Velocity;				/**< ÀŸ¬ £®pix/s£©*/
	IMP_S32		  s32Direction;				/**< ∑ΩœÚ£®0~359∂»£©*/
	TGT_TRAJECT_S stTraject;				/**< πÏº£*/
} TGT_MOTION_ITEM_S;

/** ƒø±Í ˝æ›ª∫≥Â≥§∂» */
#define IMP_BUFLEN_TGT_ITEM	256

/** ƒø±Í ˝æ› */
#define IMP_BUFLEN_TGT_DATA	(IMP_BUFLEN_TGT_ITEM - (sizeof(IMP_U32) * 4 + sizeof(IMP_POINT_S) + sizeof(IMP_RECT_S)))

/** ƒø±Í ˝æ›Ω·ππ */
typedef struct impTGT_ITEM_S
{
	IMP_U32	u32Id;							/**< ID */
	IMP_U32	u32Status;						/**< ◊¥Ã¨ */
	IMP_U32	u32Type;						/**< ¿‡–Õ */
	IMP_U32	u32Event;						/**<  ¬º˛ */
	IMP_POINT_S	stPoint;					/**< Œª÷√ */
	IMP_RECT_S	stRect;						/**< «¯”Ú */
	IMP_U8	au8Data[IMP_BUFLEN_TGT_DATA];   /**<  ˝æ› */
} TGT_ITEM_S;

/** ƒø±Íµƒ◊Ó¥Û∏ˆ ˝ */
#define IMP_MAX_TGT_CNT			64

/** ƒø±ÍºØ∫œ */
typedef struct impTGT_SET_S
{
	IMP_S32	    s32TargetNum;					 /**< ƒø±Í ˝ƒø */
	TGT_ITEM_S	astTargets[IMP_MAX_TGT_CNT];     /**< ƒø±Í ˝æ› */
} TGT_SET_S;



/**  ¬º˛◊Ó¥Û∏ˆ ˝ */
#define IMP_MAX_EVT_CNT	128

/**  ¬º˛œÓ∏ˆ ˝ */
#define IMP_BUFLEN_EVT_ITEM	64

/**  ¬º˛≥§∂» */
#define IMP_BUFLEN_EVT_DATA	(IMP_BUFLEN_EVT_ITEM - sizeof(IMP_U32) * 5)

/**  ¬º˛¿‡–Õ∂®“Â */
typedef enum impEVT_TYPE_E
{
	IMP_EVT_TYPE_SignalBad          = 0x00000001,		/**<  ”∆µ–≈∫≈“Ï≥£ºÏ≤‚ */
	IMP_EVT_TYPE_AlarmAbabdum       = 0x00000102,		/**< “≈∆˙ŒÔºÏ≤‚ */
	IMP_EVT_TYPE_AlarmObjStolen     = 0x00000103,		/**< ±ªµ¡ŒÔºÏ≤‚ */
	IMP_EVT_TYPE_AlarmNoParking     = 0x00000104,		/**< ∑«∑®Õ£≥µºÏ≤‚ */
	IMP_EVT_TYPE_AlarmPerimeter     = 0x00000105,		/**< æØΩ‰«¯”ÚºÏ≤‚ */
	IMP_EVT_TYPE_AlarmTripwire      = 0x00000106,		/**< µ•æØΩ‰œﬂºÏ≤‚ */
	IMP_EVT_TYPE_AlarmMTripwire     = 0x00000107,		/**< À´æØΩ‰œﬂºÏ≤‚ */
	IMP_EVT_TYPE_AlarmAbnmlvel      = 0x00000108,		/**< ∑«’˝≥£ÀŸ∂»ºÏ≤‚	*/
	IMP_EVT_TYPE_AlarmLoiter        = 0x00000109,		/**< ≈«ª≤ºÏ≤‚ */
	IMP_EVT_TYPE_AlarmOsc		    = 0x0000010a,		/**< OSC */
	IMP_EVT_TYPE_AlarmOsc_R		    = 0x0000010b,		/**< OSCøøøøø */

	IMP_EVT_TYPE_AVD_AlarmBase		= 0x00000200,		/**< AVDÀ„∑®∏ÊæØ */
	IMP_EVT_TYPE_AlarmSceneChg    	= 0x00000201,		/**< ≥°æ∞±‰ªª */
	IMP_EVT_TYPE_AlarmNoSignal      = 0x00000202,		/**< –≈∫≈»± ß */
	IMP_EVT_TYPE_AlarmBrightAbnml   = 0x00000203,		/**< ¡¡∂»“Ï≥£ */
	IMP_EVT_TYPE_AlarmClarityAbnml  = 0x00000204,		/**<  ”∆µƒ£∫˝ */
	IMP_EVT_TYPE_AlarmColorAbnml    = 0x00000205,		/**<  ”∆µ∆´…´ */
	IMP_EVT_TYPE_AlarmNoise      	= 0x00000206,		/**< ‘Î…˘∏…»≈ */
	IMP_EVT_TYPE_AlarmPtzLoseCrl    = 0x00000207,		/**< PTZ ßøÿ  */
	IMP_EVT_TYPE_AlarmFreeze      	= 0x00000208,		/**< ª≠√Ê∂≥Ω· */
	IMP_EVT_TYPE_AlarmInterfere		= 0x00000209,		/**< »ÀŒ™∏…»≈ */

	IMP_EVT_TYPE_VFD_AlarmBase		= 0x00000300,		/**< VFDÀ„∑®∏ÊæØ */
	IMP_EVT_TYPE_AlarmFaceCapture   = 0x00000301,		/**< »À¡≥◊•≈ƒ */
	IMP_EVT_TYPE_AlarmFaceCamouflage= 0x00000302,		/**< »À¡≥Œ±◊∞ */
	IMP_EVT_TYPE_AlarmPasswordPeep  = 0x00000303,		/**< √‹¬ÎÕµø˙ */
}EVT_TYPE_E;

/**  ”∆µ“Ï≥£¿‡–Õ */
enum
{
	VETYPE_CameraMoved = 0,					/**< œ‡ª˙“∆∂ØªÚ’⁄µ≤ */
	VETYPE_SignalBad						/**<  ”∆µ÷ ¡ø≤Ó */
};

/** “Ï≥£¿‡–ÕΩ·ππ */
typedef struct impEVT_DATA_EXCEPTION_S
{
	IMP_U32	u32Type;						/**< “Ï≥£¿‡–Õ */
} EVT_DATA_EXCEPTION_S;


/** “≈∆˙ŒÔ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_ABANDUM_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂ */
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√ */
} EVT_DATA_ABANDUM_S;

/** ±ªµ¡ŒÔºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_OBJSTOLEN_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂ */
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√ */
} EVT_DATA_OBJSTOLEN_S;

/** ƒø±ÍºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_TARGET_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂ */
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√ */
} EVT_DATA_TARGET_S;

/** ÷‹ΩÁºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_PERIMETER_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂ */
	IMP_RECT_S  stRect;					/**<  ¬º˛Œª÷√ */
	struct
	{									/**< πÊ‘Ú */
		IMP_U32	u32Mode;				/**< æØΩ‰«¯ƒ£ Ω */
	} stRule;
} EVT_DATA_PERIMETER_S;

/** ∞ÌœﬂºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_TRIPWIRE_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂ */
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√ */
	IMP_U8  u8LineIndex;                /**< ∞Ìœﬂ∫≈ */
	struct
	{
		IMP_S32		s32Bidirection;		/**< ±Ì æ∏√∞Ìœﬂ «∑ÒŒ™À´œÚ∞Ìœﬂ(0: ∑Ò, 1:  «) */
		IMP_S32		s32AbnmlAngle;		/**< ∞ÌœﬂΩ˚÷π∑ΩœÚΩ«∂»(unit: degree) */
		LINE_S		stLine;				/**< ∞ÌœﬂŒª÷√(unit: pixel) */
	} stRule;							/**< πÊ‘Ú */
} EVT_DATA_TRIPWIRE_S;

/** ∂‡∞ÌœﬂºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_MTRIPWIRE_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂ */
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√ */
	IMP_U8  u8LineIndex;                /**< ∞Ìœﬂ∫≈ */
	struct
	{
		IMP_S32		as32AbnmlAngle[2];	/**< ∞ÌœﬂΩ˚÷π∑ΩœÚΩ«∂»(unit: degree) */
		LINE_S		astLine[2];			/**< ∞ÌœﬂŒª÷√(unit: pixel) */
	} stRule;							/**< πÊ‘Ú */
} EVT_DATA_MTRIPWIRE_S;

/** “Ï≥£ÀŸ∂»ºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_ABNMLVEL_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂*/
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√*/
} EVT_DATA_ABNMLVEL_S;

/** ≈«ª≤ÀŸ∂»ºÏ≤‚ ¬º˛ ˝æ›Ω·ππ */
typedef struct impEVT_DATA_LOITER_S
{
	IMP_U32	u32TId;						/**< ƒø±Í±Í ∂*/
	IMP_RECT_S	stRect;					/**<  ¬º˛Œª÷√*/
} EVT_DATA_LOITER_S;

/**  ¬º˛œÓ */
typedef struct impEVT_ITEM_S
{
	IMP_U32	u32Type;						/**<  ¬º˛¿‡–Õ */
	IMP_U32	u32Id;							/**<  ¬º˛±Í ∂ */
	IMP_U32	u32Level;						/**<  ¬º˛æØ±®º∂± */
	IMP_U32	u32Status;						/**<  ¬º˛◊¥Ã¨ */
	IMP_U32	u32Zone;						/**<  ¬º˛∑¢…˙«¯”Ú */
	IMP_U8	au8Data[IMP_BUFLEN_EVT_DATA];   /**<  ¬º˛ ˝æ› */
} EVT_ITEM_S;

/**  ¬º˛ºØ */
typedef struct impEVT_SET_S
{
	IMP_S32	    s32EventNum;					/**<  ¬º˛ ˝ƒø */
	EVT_ITEM_S	astEvents[IMP_MAX_EVT_CNT];		/**<  ¬º˛ ˝æ› */
} EVT_SET_S;

/** PEA ¥¶¿ÌΩ·π˚ */
typedef struct impRESULT_S
{
	TGT_SET_S	stTargetSet;		/**< ƒø±Í */
	EVT_SET_S	stEventSet;		    /**<  ¬º˛ */
} RESULT_S;

/** AVD∑÷Œˆ ˝æ› */
typedef struct impAVD_STATCL_DATA_S
{
  IMP_S32 s32SceneChg;            /**< ≥°æ∞∏ƒ±‰ */
  IMP_S32 s32SignalValue;	        /**< –≈∫≈÷µ */
	IMP_S32 s32ClarityValue;        /**< ƒ£∫˝∂»÷µ */
	IMP_S32 s32BrightHValue;        /**< ¡¡∂»∏ﬂ÷µ */
	IMP_S32 s32BrightLValue;        /**< ¡¡∂»µÕ÷µ */
	IMP_S32 s32ColorValue;	        /**< —’…´÷µ */
	IMP_S32 s32NoiseValue;	        /**< ‘Î…˘÷µ */
	IMP_S32 s32PtzValue;            /**< PTZ ßøÿ */
	IMP_S32 s32FreezValue;          /**< ª≠√Ê∂≥Ω· */
	IMP_S32 s32InterfereValue;      /**< »ÀŒ™∏…»≈ */
}AVD_ANALYSIS_DATA_S;

/** AVD ¥¶¿ÌΩ·π˚ */
typedef struct impAVD_RESULT_S
{
  AVD_ANALYSIS_DATA_S stAnalysisData;	/**< AVD∑÷Œˆ ˝æ› */
	EVT_SET_S	stEventSet;			/**< AVD±®æØ ˝æ› */
}AVD_RESULT_S;



#ifdef __cplusplus
}
#endif

#endif /*_IMP_ALGO_TYPE_H_*/

/*@}*/
/*@}*/
