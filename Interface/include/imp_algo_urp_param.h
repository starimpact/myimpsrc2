#ifndef _IMP_ALGO_URP_PARAM_H_
#define _IMP_ALGO_URP_PARAM_H_


#ifdef __cplusplus
extern "C"
{
#endif


/** 场景类型 */
typedef enum impURP_SCENE_TYPE_E
{
	IMP_URP_INDOOR = 0,					/**< 室内场景 */
	IMP_URP_OUTDOOR						/**< 室外场景 */
} URP_SCENE_TYPE_E;

//相机类型
typedef enum impURP_CAMERA_TYPE_E
{
	IMP_URP_COMMON_CAMERA = 0,
	IMP_URP_PINHOLE_CAMERA
} URP_CAMERA_TYPE_E;

/** 周界检测模式 */
typedef enum impURP_PERIMETER_MODE_E
{
	IMP_URP_PMODE_INTRUSION = 0,/**< 入侵 */
	IMP_URP_PMODE_ENTER,		/**< 进入 */
	IMP_URP_PMODE_EXIT			/**< 离开 */
} URP_PERIMETER_MODE_E;

/** 最大边界点数 */
#define IMP_MAX_BOUNDARY_POINT_NUM 8

//Added by mzhang 2011-11-14 14:02:48
#define IMP_MAX_POINT_NUM 4
#define IMP_NAME_BUF_SIZE 16

/** 坐标点 */
typedef struct impURP_IMP_POINT_S
{
	IMP_S16 s16X;        /**< x */
	IMP_S16 s16Y;        /**< y */
}URP_IMP_POINT_S;

/** 线结构 */
typedef struct impURP_LINE_S
{
	URP_IMP_POINT_S stStartPt;	/**< 线段起点 */
	URP_IMP_POINT_S stEndPt;	/**< 线段终点 */
}URP_LINE_S;

//added by mzhang 2011-11-14 14:00:14
typedef struct impURP_POLYGON_REGION_S
{
	IMP_S32 s32Valid;
	IMP_S32 s32PointNum;
	URP_IMP_POINT_S astPoint[IMP_MAX_BOUNDARY_POINT_NUM];
} URP_POLYGON_REGION_S;

/** URP周界检测边界点 */
typedef struct impURP_PERIMETER_LIMIT_BOUNDARY_S
{
	IMP_S32       s32BoundaryPtNum;								    /**< 边界点数目 */
	URP_IMP_POINT_S   astBoundaryPts[IMP_MAX_BOUNDARY_POINT_NUM];	/**< 边界点集合 */
}URP_PERIMETER_LIMIT_BOUNDARY_S;

/** URP周界检测限制参数 */
typedef struct impURP_PERIMETER_LIMIT_PARA_S
{
	IMP_S32 s32MinDist;							    /**< 最小像素距离 */
	IMP_S32 s32MinTime;							    /**< 最短时间 */
	IMP_S32	s32DirectionLimit;						/**< 是否做方向限制 */
	IMP_S32 s32ForbiddenDirection;					/**< 禁止方向角度(单位: 角度) */
	URP_PERIMETER_LIMIT_BOUNDARY_S stBoundary;		/**< 周界边界区域 */
}URP_PERIMETER_LIMIT_PARA_S;

/** URP周界检测规则参数 */
typedef struct impURP_PERIMETER_RULE_PARA_S
{
	IMP_S32 s32TypeLimit;							/**< 目标类型限制 */
	IMP_S32 s32TypeHuman;							/**< 目标类别：人(限定类型时使用) */
	IMP_S32 s32TypeVehicle;						    /**< 目标类别：车 */
	IMP_S32 s32Mode;								/**< 周界模式 */
	URP_PERIMETER_LIMIT_PARA_S stLimitPara;		/**< 周界限制参数 */
} URP_PERIMETER_RULE_PARA_S;

//物品遗留-begin
#define IMP_MAX_OSC_NUM 4
typedef struct impURP_OSC_SPECL_REGIONS_S
{
	IMP_S32 s32Valid; //是否有效
	IMP_S8 as8Name[IMP_NAME_BUF_SIZE]; //区域名称
	URP_POLYGON_REGION_S stOscRg; //要检测区域定义
	URP_POLYGON_REGION_S astSubRgA; //检测区域中的无效子区域
	URP_POLYGON_REGION_S astSubRgB; //检测区域中的无效子区域
	URP_POLYGON_REGION_S astSubRgC; //检测区域中的无效子区域
} URP_OSC_SPECL_REGIONS_S;

typedef struct impURP_OSC_LMT_PARA_S
{
	IMP_S32 s32TimeMin; //时间约束
	IMP_S32 s32SizeMin; //最小尺寸
	IMP_S32 s32SizeMax; //最大尺寸
} URP_OSC_LMT_PARA_S;

typedef struct impURP_OSC_RULE_PARA_S
{
	IMP_S32 s32SceneType; //场景类型
	IMP_S32 s32CameraType; //相机类型
	URP_OSC_SPECL_REGIONS_S astSpclRgs[IMP_MAX_OSC_NUM]; //要处理的区域个数
	URP_OSC_LMT_PARA_S stOscPara; //一些约束
} URP_OSC_RULE_PARA_S;
//物品遗留-end

/** URP单绊线最多条数 */
#define IMP_URP_MAX_TRIPWIRE_CNT 8

/** URP单绊线结构 */
typedef struct impURP_TRIPWIRE_S
{
	IMP_S32     s32Valid;							/**< 是否有效 */
	IMP_S32     s32IsDoubleDirection;			    /**< 表示该绊线是否为双向绊线(0: 否, 1: 是) */
	IMP_S32     s32ForbiddenDirection;	            /**< 单绊线禁止方向角度(单位: 度) */
	URP_LINE_S	stLine;				          	    /**< 单绊线位置 */
}URP_TRIPWIRE_S;

/** URP单绊线限制参数 */
typedef struct impURP_TRIPWIRE_LIMIT_PARA_S
{
	IMP_S32 s32MinDist;			/**< 最小像素距离 */
	IMP_S32 s32MinTime;			/**< 最短时间 */
}URP_TRIPWIRE_LIMIT_PARA_S;

/** URP单绊线规则参数 */
typedef struct impURP_TRIPWIRE_RULE_PARA_S
{
	IMP_S32 s32TypeLimit;							    /**< 目标类型限制 */
	IMP_S32 s32TypeHuman;							    /**< 目标类别：人(限定类型时使用) */
	IMP_S32 s32TypeVehicle;							    /**< 目标类别：车 */
	URP_TRIPWIRE_S astLines[IMP_URP_MAX_TRIPWIRE_CNT];	/**< 单绊线 */
	URP_TRIPWIRE_LIMIT_PARA_S stLimitPara;			    /**< 单绊线限制参数 */
} URP_TRIPWIRE_RULE_PARA_S;

/** URP双警戒线最大数 */
#define IMP_URP_MAX_MTRIPWIRE_CNT 4

/** URP双警戒线结构定义 */
typedef struct impURP_MTRIPWIRE_S
{
	IMP_S32 s32Valid;					/**< 是否有效*/
	IMP_S32 s32TimeMin;					/**< 穿越两条线的最短时间间隔(unit: s)*/
	IMP_S32 s32TimeMax;					/**< 穿越两条线的最长时间间隔(unit: s)*/
	IMP_S32 as32IsDoubleDirection[2];
	IMP_S32 as32AbnmlAngle[2];			/**< 双警戒线禁止方向角度(unit: degree)*/
	URP_LINE_S	astLine[2];				/**< 双警戒线位置(unit: pixel)*/
} URP_MTRIPWIRE_S;

/** URP双警戒线限制参数定义 */
typedef struct impURP_MTRIPWIRE_LMT_PARA_S
{
	IMP_S32 s32DistMin;                /**< 最小距离 */
	IMP_S32 s32TimeMin;                /**< 最小时间 */
} URP_MTRIPWIRE_LMT_PARA_S;

/** URP双警戒线规则参数 */
typedef struct impURP_MTRIPWIRE_RULE_PARA_S
{
	IMP_S32 s32TypeLimit;						    /**< 目标类型限制 */
	IMP_S32 s32TypeHuman;							    /**< 目标类别：人(限定类型时使用) */
	IMP_S32 s32TypeVehicle;						        /**< 目标类别：车 */
	URP_MTRIPWIRE_S astLines[IMP_URP_MAX_MTRIPWIRE_CNT];	/**< 双警戒线 */
	URP_MTRIPWIRE_LMT_PARA_S stLimitPara;                   /**< 限制参数 */
} URP_MTRIPWIRE_RULE_PARA_S;

/** URP徘徊规则参数 */
typedef struct impURP_LOITER_RULE_PARA_S
{
	IMP_S32 s32TypeLimit;  	/**< 目标类型限制 */
	IMP_S32 s32TimeMin;			/**< 闲逛最短时间(unit: s) */
} URP_LOITER_RULE_PARA_S;

/** 物品遗弃 */
typedef struct impURP_RULE_PARA_ABANDUM_S
{
	IMP_S32 s32TimeMin;			/**< 最小报警时间 */
	IMP_S32 s32SizeMin;			/**< 最小像素尺寸 */
	IMP_S32 s32SizeMax;			/**< 最大像素尺寸 */
} URP_RULE_PARA_ABANDUM_S;

/** 物品被盗 */
typedef struct impURP_RULE_PARA_OBJ_STOLEN_S
{
	IMP_S32 s32TimeMin;			/**< 最小报警时间 */
	IMP_S32 s32SizeMin;			/**< 最小像素尺寸 */
	IMP_S32 s32SizeMax;			/**< 最大像素尺寸 */
} URP_RULE_PARA_OBJ_STOLEN_S;

/** 非法停车 */
typedef struct impURP_RULE_PARA_NO_PARKING_S
{
	IMP_S32 s32TimeMin;			/**< 最小报警时间 */
} URP_RULE_PARA_NO_PARKING_S;

/** URP静止目标检测规则参数 */
typedef struct impURP_STATIC_OBJ_DETECT_RULE_PARA_S
{
	URP_RULE_PARA_ABANDUM_S stObjAbandumRulePara;     /**< 物品遗留参数 */
	URP_RULE_PARA_OBJ_STOLEN_S stObjStolenRulePara;   /**< 物品被盗参数 */
	URP_RULE_PARA_NO_PARKING_S stNoParkingRulePara;   /**< 非法停车参数 */
} URP_STATIC_OBJ_DETECT_RULE_PARA_S;


/** 用户规则协议（URP）参数 */
typedef struct impURP_RULE_PARA_S
{
	URP_OSC_RULE_PARA_S     stOscRulePara;  //物品遗留检测
	URP_PERIMETER_RULE_PARA_S	stPerimeterRulePara;	          /**< 周界检测 */
	URP_TRIPWIRE_RULE_PARA_S	stTripwireRulePara;		          /**< 单绊线检测 */
	URP_MTRIPWIRE_RULE_PARA_S	stMTripwireRulePara;		      /**< 多绊线检测 */
	URP_LOITER_RULE_PARA_S      stLoiterRulePara;                 /**< 徘徊检测 */
	URP_STATIC_OBJ_DETECT_RULE_PARA_S stStaticObjDetectRulePara;  /**< 静物检测 */
}URP_RULE_PARA_S;

/** 最大规则数目 */
#define IMP_URP_MAX_NUM_RULE_NUM 8

/** 最大规则名字长度 */
#define IMP_URP_NAME_BUF_SIZE 16

/** URP功能码 */
typedef enum impURP_FUNC_PERIMETER_E
{
	IMP_URP_FUNC_ABANDUM        = 0x00000002,	/**< 遗弃物检测 */
	IMP_URP_FUNC_OBJSTOLEN      = 0x00000004,	/**< 被盗物检测 */
	IMP_URP_FUNC_NOPARKING      = 0x00000008,	/**< 非法停车检测 */
	IMP_URP_FUNC_PERIMETER		= 0x00000010,	/**< 周界检测 */
	IMP_URP_FUNC_TRIPWIRE		= 0x00000020,	/**< 单绊线检测 */
	IMP_URP_FUNC_MTRIPWIRE      = 0x00000040,	/**< 多绊线 */
	IMP_URP_FUNC_ABNMLVEL       = 0x00000080,	/**< 非正常速度 */
	IMP_URP_FUNC_LOITER         = 0x00000100,	/**< 徘徊 */
	IMP_URP_FUNC_OSC            = 0X00000200,   //object status change
	IMP_URP_FUNC_OSC_R          = 0X00000400    //object status change
}URP_FUNC_PERIMETER_E;

/** URP规则结构定义 */
typedef struct impURP_RULE_S
{
	IMP_S8  aName[IMP_URP_NAME_BUF_SIZE];		/**< 规则名称 */
	IMP_U32 u32Valid;				 	        /**< 是否有效 */
	IMP_U32 u32Enable;			  	            /**< 是否使能 */
	IMP_U32 u32Level;					        /**< 警戒级别 */
	IMP_U32 u32Mode;					        /**< 规则模式（启用的功能对应功能码按位相或） */
	URP_RULE_PARA_S stPara;				        /**< 规则参数 */
}URP_RULE_S;

/** URP规则集合 */
typedef struct impURP_RULE_SET_S
{
	URP_RULE_S astRule[IMP_URP_MAX_NUM_RULE_NUM];			/**< 规则集合 */
}URP_RULE_SET_S;

/** D1图像宽度 */
#define IMP_D1_IMG_WIDTH 704
/** CIF图像宽度 */
#define IMP_CIF_IMG_WIDTH 352
/** QCIF图像宽度 */
#define IMP_QCIF_IMG_WIDTH 176
/** PAL D1图像高度 */
#define IMP_D1_PAL_IMG_HEIGHT 576
/** PAL CIF图像高度 */
#define IMP_CIF_PAL_IMG_HEIGHT 288
/** PAL QCIF图像高度 */
#define IMP_QCIF_PAL_IMG_HEIGHT 144
/** NTSC D1图像高度 */
#define IMP_D1_NTSC_IMG_HEIGHT 480
/** NTSC CIF图像高度 */
#define IMP_CIF_NTSC_IMG_HEIGHT 240
/** NTSC QCIF图像高度 */
#define IMP_QCIF_NTSC_IMG_HEIGHT 120

typedef enum impVIDEO_STANDARD_E
{
    IMP_NTSC,				/**< NTSC制式 */
    IMP_PAL				    /**< PAL制式 */
}VIDEO_STANDARD_E;
/** URP配置参数 */
typedef struct impURP_CONFIG_PARA_S
{
	IMP_S32 s32Version;									/**< 版本号 */
	IMP_S32 s32ImgW;									/**< 所配置视频源的宽度 */
	IMP_S32 s32ImgH;									/**< 所配置视频源的高度 */
	VIDEO_STANDARD_E eVideoStandard;					/**< 所配置视频源的制式 */
}URP_CONFIG_PARA_S;


/** URP目标输出灵敏度级别 */
typedef enum impSENSITIVITY_LEVEL_E
{
	IMP_HIGH_LEVEL_SENSITIVITY,         /**< 高灵敏度 */
	IMP_MIDDLE_LEVEL_SENSITIVITY,		/**< 中灵敏度 */
	IMP_LOW_LEVEL_SENSITIVITY			/**< 低灵敏度 */
}SENSITIVITY_LEVEL_E;

/** URP高级参数 */
typedef struct impURP_ADVANCE_PARA_S
{
	IMP_S32   s32AdvanceParaEnable;				/**< 是否启用高级参数(0:否 1:是) */
	IMP_S32   s32TargetMinSize;					/**< 目标最小像素尺寸(-1为不做限制) */
	IMP_S32   s32TargetMaxSize;					/**< 目标最大像素尺寸(-1为不做限制) */
	IMP_S32   s32TargetMinWidth;				/**< 目标最小像素宽度(-1为不做限制) */
	IMP_S32   s32TargetMaxWidth;				/**< 目标最大像素宽度(-1为不做限制) */
	IMP_S32   s32TargetMinHeight;				/**< 目标最小像素高度(-1为不做限制) */
	IMP_S32   s32TargetMaxHeight;				/**< 目标最大像素高度(-1为不做限制) */
	IMP_S32   s32TargetMinSpeed;				/**< 目标最小像素速度(-1为不做限制) */
	IMP_S32   s32TargetMaxSpeed;				/**< 目标最大像素速度(-1为不做限制) */
	IMP_S32   s32TargetOutputSensitivityLevel;	/**< 目标输出灵敏度级别(高、中、低) */
}URP_ADVANCE_PARA_S;



#define IMP_URP_MAX_NUM_FDEPTH_LINE 3
/** 景深线结构 */
typedef struct impURP_FDZ_LINE_S
{
	LINE_S stRefLine;			/**< 垂直线段坐标(unit: pixel) */
	IMP_S32 s32RefLen;			/**< 线段长度(unit: cm) */
} URP_FDZ_LINE_S;

/** 景深线结构 */
typedef struct impURP_FDZ_LINES_S
{
	IMP_U32    u32NumUsed;			               /**< 样本数目 大于等于2 */
	URP_FDZ_LINE_S stLines[IMP_URP_MAX_NUM_FDEPTH_LINE];   /**< 样本数目 */
} URP_FDZ_LINES_S;

/** 景深测量结构 */
typedef struct impURP_FDEPTH_MEASURE_S
{
	URP_FDZ_LINES_S stFdzLines;		/**< 标定数据 */
} URP_FDEPTH_MEASURE_S;
/** 标定参数 */
typedef struct impURP_FDEPTH_S
{
	IMP_U32 u32Enable;					/**< 启用标识*/
	URP_FDEPTH_MEASURE_S stMeasure;			/**< 标定数据*/
}URP_FDEPTH_S;


/** 环境参数 */
typedef struct impURP_ENVIRONMENT_S
{
	IMP_U32 u32Enable;					/**< 启用标识*/
	IMP_S32 s32SceneType;				/**< 场景类型 */
} URP_ENVIRONMENT_S;

/** URP参数 */
typedef struct impURP_PARA_S
{
	URP_CONFIG_PARA_S stConfigPara;				/**< 配置参数 */
	URP_RULE_SET_S    stRuleSet;				/**< 规则参数 */
	URP_FDEPTH_S	stFdepth;					/**< 标定参数 */
	URP_ENVIRONMENT_S stEnvironment;            /**< 环境参数 */
	URP_ADVANCE_PARA_S  stAdvancePara;			/**< 高级参数 */
}URP_PARA_S;

#ifdef __cplusplus
}
#endif


#endif
/*@}*/

/*@}*/
