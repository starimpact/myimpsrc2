/*****************************************************************************/
/** @file 定义结构及常数。
 * 
 * @date	2008/07/22 
 * Copyright (C) 2004 WISEBIRD CO.,  All rights reserved.      
******************************************************************************/

#ifndef IFACE_FACEDETECT_H
#define IFACE_FACEDETECT_H

typedef struct {
	long rightEyeX;			/* 右眼的标准位置X */
							/* Standard position of right eye's X-axis */ 
	long rightEyeY;			/* 右眼的标准位置Y */
							/* Standard position of right eye's Y-axis */ 
	long leftEyeX;			/* 左眼的标准位置X */
							/* Standard position of left eye's X-axis */ 
	long leftEyeY;			/* 左眼的标准位置Y */
							/* Standard position of left eye's Y-axis */ 
	long centerX;			/* 标准脸中心位置X */
							/* Standard center position of face's X-axis */ 
	long centerY;			/* 标准脸中心位置Y */
							/* Standard center position of face's Y-axis */ 
    long faceSize;			/* 标准脸大小（矩形的一个边长） */
							/* Standard size of face(one side's length of rectangle) */
	short angleInPlane;		/* inplane的旋转角度 */
							/* angle of inplane */
	short angleOutPlane;	/* outplane的旋转角度 */
							/* angle of outplane */
	long faceScore;         /* 脸得分 */
							/* face score */
	long serialNumber;		/* 序列号 */
							/* serial number */
	long shapeScore;		/* 形状得分 */
							/* shape score */
} FACEINFO;

/* Inplane搜索模式 */
/* Inplane search mode */
typedef enum {
     Search_Inplane_All = -1,    /* 所有方向 */
								 /* All Direction */
     Search_Inplane_000 = 0,     /* 只有0度方向 */
								 /* 0℃ only */
     Search_Inplane_090 = 90,    /* 只有90度方向 */
								 /* 90℃ only */
     Search_Inplane_180 = 180,   /* 只有180度方向 */
								 /* 180℃ only */
     Search_Inplane_270 = 270    /* 只有270度方向 */
								 /* 9270℃ only */
} SearchInplaneMode;


/* Outplane搜索模式 */
/* Outplane search mode */
typedef enum {
     FRONT = 1,                 /* 正面 */
								/* Front */
     PROFILE = 2,               /* 侧面 */
								/* Profile */
     HALFPROFILE = 4,           /* 半侧面 */
								/* Half Profile */
     SEARCH_OUTPLANE_ALL = -1   /* 所有方向 */
								/* All Direction */
} SearchOutplaneMode;

/* LockRotation90模式 */
/* LockRotation90 mode */
typedef enum {
     LOCK_ROT90_ON = 1,         /* 锁定 */
								/* lock */
     LOCK_ROT90_OFF = 0         /* 不锁定 */
								/* no lock */
} LockRotation90Mode;


/* LockSearchLarge模式 */
/* LockSearchLarge mode */
typedef enum {
     LOCK_LARGE_ON = 1,         /* 锁定 */
								/* lock */
     LOCK_LARGE_OFF = 0         /* 不锁定 */
								/* no lock */
} LockSearchLarge;


/* Search模式 */
/* Search mode */
typedef enum {
     MODE_NORMAL = 0,       /* 通常模式 */
							/* Normal */
     MODE_TP = 1,           /* TP优先 */
							/* TP High Priority */
     MODE_SPEED = -1        /* 速度优先 */
							/* Speed Mode */
} SearchMode;

/** 错误码
 *  Error ID of library
 */
typedef enum {
	/** iFACE library common*/

    IFACE_ERR_SUCCESS_ID                    = 0,        /**< 处理成功   */
		                                                /**< Processing success */
    IFACE_ERR_WRONG_SEQUENCE                = 1,        /**< 调用顺序错误 */
                                                        /**< Calling sequence wrong */
    IFACE_ERR_INVALID_PARAM                 = 2,        /**< 非法参数 */
	                                                    /**< Injustice of argument */
    IFACE_ERR_NO_ENOUGH_MEMORY              = 3,        /**< 内存不足 */
                                                        /**< Memory shortage */
    IFACE_ERR_NO_FACE_FOUND                 = 4,		/**< 图像内没有发现人脸 */
	                                                    /**< The face was not able to be discovered in the image */
	IFACE_ERR_NO_FACE_CROP                  = 5,		/**< 不能切出图像内人脸 */
	                                                    /**< Face could not be cropped in the image */ 
    IFACE_ERR_FILE_OPEN                     = 6,        /**< 文件打开错误 */
	                                                    /**< File opening error */ 
    IFACE_ERR_FILE_READ                     = 7,        /**< 读文件错误 */
														/**< File reading error */
	IFACE_ERR_TOO_MANY_CANDIDATES           = 8			/**< 太多的候选人脸数 */
														/**< Too many candidates error */

} IFACE_ERROR_ID;

#define	SEARCH_INPLANE_ALL  Search_Inplane_All 
#define	SEARCH_INPLANE_000  Search_Inplane_000	
#define SEARCH_INPLANE_090  Search_Inplane_090
#define SEARCH_INPLANE_180  Search_Inplane_180
#define SEARCH_INPLANE_270  Search_Inplane_270
typedef unsigned char ubyte;

typedef void* FaceRecognitionHandle;
typedef unsigned long   IFACE_ERROR_CODE;

#define MAXINT	0x7fffff00
#define PI		3.1415926535f		/* 圆周率 */
									/* pi     */

//<<<--- 2009-12-28 修改为short
/*! Header of structure array */
//typedef struct {
//    long flag;
//} CandidateHeader;
//
///*! Structure array */
//typedef struct {
//    void* pTop;
//    long unitLength;
//    long maxNumber;
//    long number;
//} CandidatesList;

typedef struct {
	short flag;
} CandidateHeader;

/*! Structure array */
typedef struct {
	void* pTop;
	short unitLength;
	short maxNumber;
	short number;
} CandidatesList;
//--->>>

/* ==== 规格大小 ==== */
#define FACE_DETECT_UNIT    32
#define FEATURE_DETECT_UNIT 24

/* ==== 左眼/右眼/两眼的识别器 ==== */
#define EYE_RIGHT 0
#define EYE_LEFT  1

#ifndef __T_FACECANDIDATESNEW__
#define __T_FACECANDIDATESNEW__
/* 候选脸信息 */
//<<<--- 2009-12-28 修改为short
//typedef struct {
//	CandidateHeader header;
//    long centerHeight;            /* 搜索单位矩形的中心点Y坐标（FXP_UNIT_POS的倍值） */
//    long centerWidth;             /* 搜索单位矩形的中心点X坐标（FXP_UNIT_POS的倍值） */
//    long aCanonicalEyeHeight[2];  /* 规定的眼睛的Y坐标（FXP_UNIT_POS的倍值） */
//    long aCanonicalEyeWidth[2];   /* 规定的眼睛的X坐标（FXP_UNIT_POS的倍值） */
//    short angle;                  /* 检出时的角度 */
//    short angleOutPlane;
//	short direction;
//    short scale;                  /* 检出时使用的比例（FXP_UNIT_SCALE的倍值） */
//    long score;                  /* 检出时的得分 */
//    long scoreNeighbor;
//    long score2;
//    long score2Neighbor;
//} FaceCandidatesNew;

typedef struct {
	CandidateHeader header;
	short centerHeight;            /* 搜索单位矩形的中心点Y坐标（FXP_UNIT_POS的倍值） */
	short centerWidth;             /* 搜索单位矩形的中心点X坐标（FXP_UNIT_POS的倍值） */
	short aCanonicalEyeHeight[2];  /* 规定的眼睛的Y坐标（FXP_UNIT_POS的倍值） */
	short aCanonicalEyeWidth[2];   /* 规定的眼睛的X坐标（FXP_UNIT_POS的倍值） */
	short angle;                  /* 检出时的角度 */
	short angleOutPlane;
	short direction;
	short scale;                  /* 检出时使用的比例（FXP_UNIT_SCALE的倍值） */
	short score;                  /* 检出时的得分 */
	short scoreNeighbor;
	short score2;
	short score2Neighbor;
} FaceCandidatesNew;

//--->>>

#endif

#ifndef __T_IJINDEXNEW__
#define __T_IJINDEXNEW__
/* 脸搜索坐标信息 */

typedef struct {
	short x;
	short y;
	short score;
	short score2;
	short type;
} IJIndexNew;
#endif

#ifndef __T_IJINDEXNEWLIST__
#define __T_IJINDEXNEWLIST__
/* 脸搜索坐标列表 */

//<<<--- 2009-12-28 修改为short
//typedef struct {
//    IJIndexNew* list;
//    long length;
//    long lengthMax;
//} IJIndexNewList;

typedef struct {
	IJIndexNew* list;
	short length;
	long lengthMax;
} IJIndexNewList;
//--->>>
#endif

#ifndef __T_DETECTPARAM__
#define __T_DETECTPARAM__
typedef struct {
	short faceSize;			/* 上下左右方法的最小检出脸大小 */
	/* The least size of detected face for up-down-left-right four direction */
	short faceSize30;		/* 纵轴30度方向的最小检出脸大小 */
	/* The least size of detected face for vertical 30 grade direction */
	short inPlane;			/* 对象在平面内旋转 */
	/* Search for in plane rotation  */
	short outPlane;			/* 对象在平面的垂直方向旋转 */
	/* Search for out plane rotation */
	short lockRotate90;		/* 找到脸时上下方向的固定标记 */
	/* Direction fixed flag when a face is found */
	short lockLarge;		/* 找到脸时小尺寸脸是否搜索的标志 */
	/* Smaller face detect flag when a face is found. */
	short detectMode;		/* 脸检出优先方式 */
	/* Priority mode for face detect */
	short faceDetectThreshold;/* 脸检出适用的分值门限 */
	/* Threshold for face detect score */
	short faceCropThreshold;/* 切出脸的分值门限 */
	/* Threshold for face crop score */
} DetectParam;
#endif

#endif /* IFACE_FACEDETECT_H */
