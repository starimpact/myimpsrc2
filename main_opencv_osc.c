#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "imp_algo_type.h"
#include "imp_algo_interface.h"
#include "imp_algo_urp_param.h"
#include "imp_pea_common.h"
#include "imp_pea_system.h"

#define DEBUG_OBJRECG
#define DEBUG_OBJRECG_DETECTOR
#define DEBUG_OBJRECG_TRACKER
// #define DEBUG_OBJRECG_CLASSIFIER
#define DEBUG_OBJRECG_ANALYST
#define SUPPORT_NICE


//#define TIME_TEST
#ifdef TIME_TEST
    #include <sys/time.h>
#endif

//#define cif
#define qcif

#ifdef qcif
#define Y_WIDTH 176
#define Y_HEIGHT 144
#define U_WIDTH 88
#define U_HEIGHT 72
#define V_WIDTH 88
#define V_HEIGHT 72

#define CIF_Y_WIDTH 352
#define CIF_Y_HEIGHT 288
#define CIF_U_WIDTH 176
#define CIF_U_HEIGHT 144
#define CIF_V_WIDTH 176
#define CIF_V_HEIGHT 144
#elif defined(cif)
#define Y_WIDTH 352
#define Y_HEIGHT 288
#define U_WIDTH 176
#define U_HEIGHT 144
#define V_WIDTH 176
#define V_HEIGHT 144
#endif


#define TBL_YUV(y,u,v) {y,u,v}
/** YUV图像索引表 0红,1绿,2蓝*/
static PIXEL_S cr_tbl[] =
{
	TBL_YUV(81,90,240),
	TBL_YUV(144,54,34),
	TBL_YUV(29,121,102),
	TBL_YUV(255,0,128),
	TBL_YUV(0,255,128),
	TBL_YUV(128,255,128),
	TBL_YUV(128,255,0),
	TBL_YUV(255,128,0),
	TBL_YUV(0,255,128),
	TBL_YUV(255,128,0),
	TBL_YUV(128,0,255),
	TBL_YUV(128,255,0),
	TBL_YUV(0,0,255)
};
#define IMP_IMAGE_WIDTH 704
#define IMP_IMAGE_HEIGHT 576

static IMP_U32 color_tbl[] =
{
	0x3e0,              /**< blue */
	0x7c00,             /**< red */
};
typedef enum impVIDEO_FORMAT_E
{
    IMP_QCIF,
    IMP_CIF,
    IMP_D1,
	IMP_HD1
}VIDEO_FORMAT_E;


typedef enum impVIDEO_SOURCE_E
{
    IMP_AVI,
    IMP_YUV
}VIDEO_SOURCE_E;


static void NICE_Example();
int ReadImage(unsigned char *pImage,char *cFileName,int nWidth,int nHeight,long offset);

unsigned char Y_space[Y_WIDTH*Y_HEIGHT];
unsigned char U_space[U_WIDTH*U_HEIGHT];
unsigned char V_space[V_WIDTH*V_HEIGHT];
URP_PARA_S stURPpara;
CvScalar colors[13];
int rec_count = 0;
/**
* return
*    -0:normal
*    !=0:end of file
*    -1:can not open the file
*/

int ReadImage(unsigned char *pImage,char *cFileName,int nWidth,int nHeight,long offset)
{
     int j,i,flag;
     unsigned char *pWork;
     FILE *fp=0;
     flag=0;

     if ( fp=fopen(cFileName,"rb" ) )   //open the yuv video
     {
        fseek(fp,offset,SEEK_SET);   //read from the offset
        pWork=pImage; //pointer
        for ( j=0;j<nHeight;j++,pWork+=nWidth )
        {
           for ( i=0;i<nWidth;i++ )
           {
                fread(pWork+i,1,1,fp); //read
                if(feof(fp))//end of the file
                {
                    flag=1;
                    break;
                }
           }
        }
        fclose(fp);
     }
     else
     {
         flag=-1;
	     printf("Failed to open file %s\n!",cFileName);
     }
     return flag;
}

static void draw_motion_trajectory_dregion( PEA_RESULT_S *rs, IplImage *img, PROCESS_TYPE_E enFlag )
{
    IMP_S32 i;
    IMP_RECT_S rc;


	PEA_DETECTED_REGIONSET_S *rgs;
	IMP_S8 ch[10];
	CvFont font1;

    CvScalar cr;
	CvScalar cr_m=CV_RGB( 0, 0, 255);
	CvScalar cr_sl=CV_RGB( 0, 255, 0 );
	CvScalar cr_sr=CV_RGB( 255, 0, 0 );
	CvScalar cr_suk=CV_RGB( 0,255,255 );
	CvScalar cr_light=CV_RGB( 255, 255, 0 );
	CvScalar cr_uk=CV_RGB( 255, 0, 255);

	cvInitFont( &font1, CV_FONT_HERSHEY_SIMPLEX, 1.0, 1.0, 0, 1, CV_AA );


	if( enFlag == IMP_PROCESS_CHG)
	{
		rgs = &rs->stDRegionSetChg;
	}
	if(enFlag == IMP_PROCESS_OSC)
	{
		rgs = &rs->stDRegionSetOsc;
	}
	else
	{
		rgs = &rs->stDRegionSet;
	}

	for( i=1; i<IMP_MAX_TGT_CNT-1; i++ )
	{
		if( rgs->astDrg[i].u8Used )
		{
			rc.s16X1 = rgs->astDrg[i].stRect.s16X1;
			rc.s16X2 = rgs->astDrg[i].stRect.s16X2;
			rc.s16Y1 = rgs->astDrg[i].stRect.s16Y1;
			rc.s16Y2 = rgs->astDrg[i].stRect.s16Y2;
			if( IMP_DRG_IS_MOTION(rgs->astDrg[i].u8Used) )
				cr = cr_m;
			else if( IMP_DRG_IS_STATIC_L(rgs->astDrg[i].u8Used) )
				cr = cr_sl;
			else if( IMP_DRG_IS_STATIC_R(rgs->astDrg[i].u8Used) )
				cr = cr_sr;
			else if( IMP_DRG_IS_LIGHT(rgs->astDrg[i].u8Used) )
				cr = cr_suk;
			else
				cr = cr_uk;
			if( IMP_DRG_IS_DIFF( rgs->astDrg[i].u8Used ) )
				cr = cr_light;

            cvRectangle( img, cvPoint(rc.s16X1,rc.s16Y1), cvPoint(rc.s16X2,rc.s16Y2),
				cr, 0, CV_AA, 0 );
			//             sprintf( ch, "%d", i );
			// 			cvPutText( img, ch, cvPoint(rc.s16X1,rc.s16Y1), &font1, cvScalar(255,0,0,0) );
		}
	}
}


static void draw_motion_trajectory_ttarget( PEA_RESULT_S *rs,IplImage *img, IMP_S32 flag )
{
	IMP_S32 i, j, k, n, cnt, num,flag1,line_thickness;
	CvScalar *pcrRect,*pcrLine;
	IpTargetPosition *pos0, *pos1;
	IMP_POINT_S *pt1, *pt2;
	IpTrackedTargetSet *tts;
	IpTrackedTarget *target;
	
	tts = &rs->stTrackedTargetSetOsc;
	target = tts->astTargets;
	cnt = tts->s32UsedTotal;
    line_thickness=1;
    flag1=0;
    
    int lnum = 0, rnum = 0;
    char abyText[100];
    
    CvFont font;
    
    cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX ,0.35f,0.35f,0,1,CV_AA);

	for( i=0, j=0; i<IMP_MAX_TGT_CNT; i++ )
	{
    //    if( target->stTargetInfo.u32TgtEvent & IMP_TGT_EVENT_PERIMETER )
    	target = &tts->astTargets[i];
    	if (target->s32Used)
        {

			n = 0;
			pos0 = ipTargetTrajectoryGetPosition( &target->stTrajectory, 0 );
			num = ipTargetTrajectoryGetLength( &target->stTrajectory );
			pt1 = &pos0->stPt;
			pcrLine = &(colors[target->u32TargetId%12]);
#if 0
			if (target->stTargetInfo.u32Type==IMP_TGT_TYPE_HUMAN)
			{
				pcrRect = &(colors[12]);
			}
			else if(target->stTargetInfo.u32Type==IMP_TGT_TYPE_VEHICLE)
			{
				pcrRect = &(colors[1]);
			}
			else if (target->stTargetInfo.u32Type==IMP_TGT_TYPE_UNKNOWN)
			{
				pcrRect = &(colors[0]);
			}
			else
#endif
			{
				pcrRect = &(colors[4]);//
			}
			
			if (pos0->u32AreaPixel > 100 && pos0->u32AreaPixel < 10000)
			{ //
			//	printf("id:%d %d\n", target->u32TargetId, target->u32TargetId == -1610612592);
				sprintf(abyText, "%d,%d,%d", target->u32TargetId, pos0->u32AreaPixel, num);
				cvPutText(img,abyText,cvPoint(pt1->s16X,pt1->s16Y),&font, CV_RGB(0, 255, 0));
				
		//		cvRectangle( img, cvPoint(pos0->stRg.s16X1,pos0->stRg.s16Y1), cvPoint(pos0->stRg.s16X2,pos0->stRg.s16Y2),*pcrRect, 0, CV_AA, 0 );
			}
			lnum = 0;
			rnum = 0;
			for( k=1; k<num; k++ )
			{
				pos1 = ipTargetTrajectoryGetPosition( &target->stTrajectory, -k );
				if (IMP_IS_RGN_RSTATIC_L(pos1->u32DrgType)) lnum++;
				if (IMP_IS_RGN_RSTATIC_R(pos1->u32DrgType)) rnum++;
			}
			
			if (
				(lnum > 0 || rnum > 0) &&
				target->u32TargetId == -1610612732
				)
			{
			//c	printf("id:%d, lnum:%d, rnum:%d, num:%d\n", target->u32TargetId, lnum, rnum, num);
				for( k=1; k < num; k++ )
				{
					pos1 = ipTargetTrajectoryGetPosition( &target->stTrajectory, -k );
					pt2 = &pos1->stPt;
					cvLine( img, cvPoint(pt1->s16X,pt1->s16Y), cvPoint(pt2->s16X,pt2->s16Y),
					*pcrLine, line_thickness, CV_AA, 0 );
					pt1 = pt2;
				}
           	 	cvRectangle(img, cvPoint(pos0->stRg.s16X1,pos0->stRg.s16Y1), cvPoint(pos0->stRg.s16X2,pos0->stRg.s16Y2),*pcrRect, 0, CV_AA, 0);
           	}
		}

		j += target->s32Used ? 1 : 0;
		if( j>=cnt ) break;

		target++;
	}
}

static void draw_motion_trajectory_ntarget( RESULT_S *rs, IplImage *img, IMP_S32 flag )
{
	int i, k, cnt, num,line_thickness;
	CvScalar *pcrRect,*pcrLine;//绘图像素颜色
	IMP_POINT_S pt1, pt2;
 	TGT_SET_S *tts;
	TGT_ITEM_S *target;

	//tts = &rs->target_set;
	//target = tts->targets;
	//cnt = tts->target_num;

    tts = &rs->stTargetSet;
	target = tts->astTargets;
	cnt = tts->s32TargetNum;

	line_thickness=1;
	//printf("cnt=%d\n",cnt);

	for( i=0; i<cnt; i++ )
	{
		//IP_PIXVAL *pcr = &(colors[target->id%12]);
		TGT_MOTION_ITEM_S *item_data = (TGT_MOTION_ITEM_S*)(target->au8Data);
		TGT_TRAJECT_S *traject = &item_data->stTraject;

		//printf("target->event =%d\n",target->event );
		if(target->u32Event !=0)
		{
		    pcrLine = &(colors[target->u32Id%12]);
		}
		else
		{
		    pcrLine = &(colors[0]);
		}

		num = traject->s32Length;
		//printf("num=%d\n",num);
		pt1 = traject->stPoints[0];
		for( k=1; k<num; k++ )
		{
			pt2 = traject->stPoints[k];
			//DrawLine( img, pt1, pt2, pcr );
			cvLine( img, cvPoint(pt1.s16X,pt1.s16Y), cvPoint(pt2.s16X,pt2.s16Y),
					*pcrLine, line_thickness, CV_AA, 0 );
			pt1 = pt2;
		}
 		//DrawRect( img, &target->rect, pcr );
 		IMP_RECT_S *rg=&target->stRect;
        cvRectangle( img, cvPoint(rg->s16X1,rg->s16Y1), cvPoint(rg->s16X2,rg->s16Y2),(colors[12]), 0, CV_AA, 0 );

		target++;
	}
}



//added by mzhang
void IMP_DrawOneRule(IplImage *img, CvScalar color, URP_POLYGON_REGION_S *pregion)
{
	int i;
	int s32BoundaryPtNum = 0;
	URP_IMP_POINT_S *pt1 = 0, *pt2 = 0;

	if (pregion->s32Valid)
	{
		s32BoundaryPtNum = pregion->s32PointNum;
		for (i = 0; i <  s32BoundaryPtNum -1 ;i++)
		{
			pt1 = &pregion->astPoint[i];

			pt2 = &pregion->astPoint[i+1];
	//	printf("%d, %d; %d, %d\n", pt1->s16X, pt1->s16Y, pt2->s16X, pt2->s16Y);
            cvLine(img, cvPoint(pt1->s16X, pt1->s16Y), cvPoint(pt2->s16X, pt2->s16Y), color, 1, 8, 0);
		}
		pt1 = &pregion->astPoint[0];
		pt2 = &pregion->astPoint[s32BoundaryPtNum-1];
		cvLine(img, cvPoint(pt1->s16X, pt1->s16Y), cvPoint(pt2->s16X, pt2->s16Y), color, 1, 8, 0);
	}
}




void ShowPeaRule(URP_PARA_S *pstURPpara,IplImage *img)
{
    int s32RuleIndex;
    int i;
    int s32BoundaryPtNum;

    for( s32RuleIndex=0; s32RuleIndex<8; s32RuleIndex++ )
	{
		if( 0
			||	pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Valid == 0
			||	pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Enable == 0
		   )
			continue;

		//added by mzhang
		if (1 && pstURPpara->stRuleSet.astRule[s32RuleIndex].u32Mode & IMP_FUNC_OSC)
		{
			URP_OSC_SPECL_REGIONS_S *psr = &pstURPpara->stRuleSet.astRule[s32RuleIndex].stPara.stOscRulePara.astSpclRgs[0];
			IMP_DrawOneRule(img, CV_RGB(255, 0, 0), &psr->stOscRg);
//printf("DRAW:stSpclRgA:%d %d\n", psr->stSpclRgA.s32Valid, psr->stSpclRgA.s32PointNum);
			IMP_DrawOneRule(img, CV_RGB(0, 255, 0), &psr->astSubRgA);
//printf("stSpclRgA:%d\n", psr->stSpclRgA.s32Valid);
			IMP_DrawOneRule(img, CV_RGB(0, 255, 0), &psr->astSubRgB);
			IMP_DrawOneRule(img, CV_RGB(0, 255, 0), &psr->astSubRgC);
        }

	}
}

void ShowPEAResult(RESULT_S *result,IplImage *img)
{
    int i,j,k,zone,num1,tmp,width ,height;
    CvPoint pt[20];
    EVT_DATA_PERIMETER_S *pdataPerimeter;
    EVT_DATA_TRIPWIRE_S *pdataTripwire;
    IMP_POINT_S pts, pte,pt_s,pt_e;
    int line_thickness;
	CvScalar *pcrRect,*pcrLine,*pcrLine1;//绘图像素颜色
	line_thickness=1;
	 pcrLine = &(colors[12]);
	 pcrLine1 = &(colors[3]);
	 pcrRect = &(colors[1]);


    //printf("Nice PEA result:\n");
    printf("event num:%d\n",result->stEventSet.s32EventNum);
    for(i=0;i<result->stEventSet.s32EventNum;i++)
    {
   //     if(result->stEventSet.astEvents[i].u32Status==IMP_EVT_STATUS_START)
        {

            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmOsc)
            {
                EVT_DATA_TARGET_S *pOSC = 0;
                zone=result->stEventSet.astEvents[i].u32Zone;
            //    printf("Target:zone:%d, id:%d AlarmOsc start \n", zone,result->stEventSet.astEvents[i].u32Id);
                pOSC=(EVT_DATA_TARGET_S*)(result->stEventSet.astEvents[i].au8Data);
            //    printf("pOSC->u32TId:%d\n", pOSC->u32TId);
			    //Send VCA Alarm Msg
		    //    printf("Hello OSC!!! [%d %d %d %d]\n", pOSC->stRect.s16X1, pOSC->stRect.s16Y1, pOSC->stRect.s16X2, pOSC->stRect.s16Y2);
		        cvRectangle(img, cvPoint(pOSC->stRect.s16X1, pOSC->stRect.s16Y1),
                        cvPoint(pOSC->stRect.s16X2, pOSC->stRect.s16Y2), CV_RGB(0, 255, 0), 3, 8, 0);
             //   cvWaitKey(0);
            }
			
			if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmOsc_R)
            {
                EVT_DATA_TARGET_S *pOSC = 0;
                zone=result->stEventSet.astEvents[i].u32Zone;
            //    printf("Target:%d AlarmOsc start \n",result->stEventSet.astEvents[i].u32Id);
                pOSC=(EVT_DATA_TARGET_S*)(result->stEventSet.astEvents[i].au8Data);
            //    printf("pOSC->u32TId:%d\n", pOSC->u32TId);
			    //Send VCA Alarm Msg
		    //    printf("Hello OSC_R!!! [%d %d %d %d]\n", pOSC->stRect.s16X1, pOSC->stRect.s16Y1, pOSC->stRect.s16X2, pOSC->stRect.s16Y2);
		        cvRectangle(img, cvPoint(pOSC->stRect.s16X1, pOSC->stRect.s16Y1),
                        cvPoint(pOSC->stRect.s16X2, pOSC->stRect.s16Y2), CV_RGB(255, 255, 0), 3, 8, 0);
             //   cvWaitKey(0);
            }
        }
    }
    //printf("rec_count=%d\n",rec_count);
}

/*

Region:  Name(1),PointNum(4),PolyValid(1)
Region: Point 1:(60,34)
Region: Point 2:(50,266)
Region: Point 3:(276,259)
Region: Point 4:(177,40)
SubA: PolyValid(1),PointNum(4)
SubA: Point 1:(117,195)
SubA: Point 2:(129,238)
SubA: Point 3:(205,232)
SubA: Point 4:(192,187)
SubB: PolyValid(1),PointNum(4)
SubB: Point 1:(81,93)
SubB: Point 2:(88,168)
SubB: Point 3:(205,166)
SubB: Point 4:(188,93)
SubC: PolyValid(0),PointNum(0)

*/


short gawRMVRect[8][2];
int gadwRMVPntNum = 0;


#define PAR_SRC_FILE 0
#define MAX_RULE_DATA_SIZE 64*1024
static void IMP_OSC_PARA_Config( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH )
{
short awPos1[4][4][2]={{215,102, 300,102, 295,159, 222,165}, {222,105, 262,106, 262,137, 223,138}, {272,108, 287,108, 287,117, 271,119}};
short awPank0203[4][4][2]={181,130, 331,125, 344,188, 184,210};
short awCar1[4][4][2]={157,195, 159,238, 245,232, 192,187};
short awCar2[4][4][2]={60,34, 50,266, 286,259, 177,40};
short awCar3[4][4][2]={81,93, 88,168, 205,166, 188,93};
short awCar4[4][6][2]={89,101, 114,100, 129,110, 139,140, 110,144, 85,120};
short awPos[4][4][2];

memcpy(awPos, awCar2, 4 * 2 * 2);
memcpy(awPos + 1, awCar1, 4 * 2 * 2);
memcpy(awPos + 2, awCar3, 4 * 2 * 2);

int dwI;
	URP_OSC_SPECL_REGIONS_S *psr = 0;
	memset(&stURPpara,0,sizeof(URP_PARA_S));
	{
		stURPpara.stConfigPara.s32ImgW = 352;
		stURPpara.stConfigPara.s32ImgH = 288;

		stURPpara.stAdvancePara.s32AdvanceParaEnable = 1;
		stURPpara.stAdvancePara.s32TargetOutputSensitivityLevel = IMP_HIGH_LEVEL_SENSITIVITY;
        stURPpara.stAdvancePara.s32TargetMinSize = -1;
        stURPpara.stAdvancePara.s32TargetMaxSize = -1;
        stURPpara.stAdvancePara.s32TargetMinWidth = -1;
        stURPpara.stAdvancePara.s32TargetMinHeight = -1;
        stURPpara.stAdvancePara.s32TargetMaxWidth = -1;
        stURPpara.stAdvancePara.s32TargetMaxHeight = -1;
//      stURPpara.stAdvancePara.f32TargetMinWHRatio = -1;
//      stURPpara.stAdvancePara.f32TargetMaxWHRatio = -1;
        stURPpara.stAdvancePara.s32TargetMinSpeed = -1;
        stURPpara.stAdvancePara.s32TargetMaxSpeed = -1;


#if 1
		stURPpara.stRuleSet.astRule[1].u32Enable = 1;
		stURPpara.stRuleSet.astRule[1].u32Valid = 1;
		stURPpara.stRuleSet.astRule[1].u32Mode = IMP_URP_FUNC_OSC_R; //设定为OSC算法

		stURPpara.stRuleSet.astRule[1].stPara.stOscRulePara.s32SceneType = IMP_URP_INDOOR;
		stURPpara.stRuleSet.astRule[1].stPara.stOscRulePara.s32CameraType = IMP_URP_COMMON_CAMERA;
		stURPpara.stRuleSet.astRule[1].stPara.stOscRulePara.stOscPara.s32TimeMin = 10;
		stURPpara.stRuleSet.astRule[1].stPara.stOscRulePara.stOscPara.s32SizeMin = 10;
		stURPpara.stRuleSet.astRule[1].stPara.stOscRulePara.stOscPara.s32SizeMax = 100000;
		
		psr = &stURPpara.stRuleSet.astRule[1].stPara.stOscRulePara.astSpclRgs[0];
		psr->s32Valid = 1;
//		{
	//		IMP_S8 *pbyBufName = "HelloOSC";
	//		memcpy(stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.astSpclRgs[0].as8Name, pbyBufName, sizeof(pbyBufName));
//		}
		
        //定义检测区域
		psr->stOscRg.s32Valid = 1;
		psr->stOscRg.s32PointNum = gadwRMVPntNum;
        IMP_POINT_S *pstPoint = psr->stOscRg.astPoint;
        for(dwI = 0; dwI < gadwRMVPntNum; dwI++)
        {
            psr->stOscRg.astPoint[dwI].s16X = gawRMVRect[dwI][0];
            psr->stOscRg.astPoint[dwI].s16Y = gawRMVRect[dwI][1];
        }
#endif

		stURPpara.stRuleSet.astRule[0].u32Enable = 1;
		stURPpara.stRuleSet.astRule[0].u32Valid = 1;
		stURPpara.stRuleSet.astRule[0].u32Mode = IMP_URP_FUNC_OSC; //设定为OSC算法

		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.s32SceneType = IMP_URP_INDOOR;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.s32CameraType = IMP_URP_COMMON_CAMERA;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.stOscPara.s32TimeMin = 20;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.stOscPara.s32SizeMin = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.stOscPara.s32SizeMax = 10000;
		
		psr = &stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.astSpclRgs[0];
		psr->s32Valid = 1;
//		{
	//		IMP_S8 *pbyBufName = "HelloOSC";
	//		memcpy(stURPpara.stRuleSet.astRule[0].stPara.stOscRulePara.astSpclRgs[0].as8Name, pbyBufName, sizeof(pbyBufName));
//		}
		
        //定义检测区域
		psr->stOscRg.s32Valid = 1;
		psr->stOscRg.s32PointNum = 4;
        for(dwI = 0; dwI < 4; dwI++)
        {
            psr->stOscRg.astPoint[dwI].s16X = awPos[0][dwI][0];
            psr->stOscRg.astPoint[dwI].s16Y = awPos[0][dwI][1];
        }
#if 0
		psr->stOscRg.s32PointNum = 6;
        for(dwI = 0; dwI < 6; dwI++)
        {
            psr->stOscRg.astPoint[dwI].s16X = awCar4[0][dwI][0];
            psr->stOscRg.astPoint[dwI].s16Y = awCar4[0][dwI][1];
        }
#endif
//        printf("%d,%d;\n", psr->stOscRg.astPoint[0].s16X, psr->stOscRg.astPoint[0].s16Y);
#if 0
        //定义无效子区域
		psr->astSubRgA.s32Valid = 0;
		psr->astSubRgA.s32PointNum = 4;
        for(dwI = 0; dwI < psr->astSubRgA.s32PointNum; dwI++)
        {
            psr->astSubRgA.astPoint[dwI].s16X = awPos[1][dwI][0];
            psr->astSubRgA.astPoint[dwI].s16Y = awPos[1][dwI][1];
        }
#endif

#if 0
        //定义无效子区域
		psr->astSubRgB.s32Valid = 0;
		psr->astSubRgB.s32PointNum = 4;
        for(dwI = 0; dwI < psr->astSubRgB.s32PointNum; dwI++)
        {
            psr->astSubRgB.astPoint[dwI].s16X = awPos[2][dwI][0];
            psr->astSubRgB.astPoint[dwI].s16Y = awPos[2][dwI][1];
        }
#endif
	}

    printf("IMP_ParaConfig\n");
	IMP_OSC_ConfigArmPeaParameter( hModule, NULL ,&stURPpara );
}


IplImage *gpstImgRMV = NULL, *gpstImgRMV2 = NULL;
IMP_S32 gs32RMV_SET_OVER = 0;

void on_mouse(int event, int x, int y, int flags, void* param)
{
	if (gs32RMV_SET_OVER)
	{
		return;
	}
	switch(event)
	{
	case CV_EVENT_LBUTTONDOWN:
		cvCvtColor(gpstImgRMV, gpstImgRMV2, CV_GRAY2BGR);	
		if (gadwRMVPntNum < 8)
		{
			gawRMVRect[gadwRMVPntNum][0] = x;
			gawRMVRect[gadwRMVPntNum][1] = y;
			gadwRMVPntNum++;
		}
		int dwPI;
		for (dwPI = 0; dwPI < gadwRMVPntNum - 1; dwPI++)
		{
			cvLine(gpstImgRMV2, cvPoint(gawRMVRect[dwPI][0], gawRMVRect[dwPI][1]), cvPoint(gawRMVRect[dwPI + 1][0], gawRMVRect[dwPI + 1][1]), CV_RGB(0, 255 , 0), 1, CV_AA, 0);
		}
		if (gadwRMVPntNum > 1)
		{
			cvLine(gpstImgRMV2, cvPoint(gawRMVRect[gadwRMVPntNum - 1][0], gawRMVRect[gadwRMVPntNum - 1][1]), cvPoint(gawRMVRect[0][0], gawRMVRect[0][1]), CV_RGB(100, 0 , 0), 1, CV_AA, 0);
		}
		cvShowImage("DrawRMV", gpstImgRMV2);
		break;
	case CV_EVENT_RBUTTONDOWN:
		printf("\n\n");
	}
}


void IMP_OpencvExample(IMP_S8 * cFileName,VIDEO_SOURCE_E enVideoSource, IMP_S32 s32FrmW, IMP_S32 s32FrmH, IMP_S32 s32SubSampleT, VIDEO_FORMAT_E enVideoFormat)
{
    CvCapture* pCapture = NULL;
    IplImage* pImgGray = NULL;
    IplImage* pFrImg = NULL;
    IplImage* pBkImg = NULL;
    IplImage* color_dst_blob = NULL;

    IplImage* color_dst_trajectory = NULL;
    IplImage *frame = 0;
    IplImage *imageSrc = 0;
	IplImage *imageDst = 0;

	IMP_U8 *Y_space;
	IMP_U8 *U_space;
	IMP_U8 *V_space;

    IMP_S32 nFrmNum = 0;
    IMP_S64 origin = 0;
    IMP_S32 flag;
    IMP_S8 key;

	IMP_S32 s32ImgW, s32ImgH;
	OBJ_S stObj;
	LIB_INFO_S stLibInfo;
	IMP_HANDLE hIMP = &stObj;
	MEM_SET_S stMems;
	YUV_IMAGE422_S stImage;
	RESULT_S stResult;
	IMP_S32 s32Width, s32Height;
    PEA_MODULE *pModule;
    PEA_RESULT_S *pResult;
	IMP_S32 cmd_cancel=1;
	IMP_S32 bRet;

#ifdef DEBUG_OBJRECG
	PEA_ModuleObjRecognition *pObjRecg;
	IpModuleBehaviorAnalysis *pEvtAnls ;
#ifdef DEBUG_OBJRECG_DETECTOR
	IMP_MODULE_HANDLE hDetector;
	PEA_TARGET_DETECTOR_S *pDetector;
#endif
#ifdef DEBUG_OBJRECG_TRACKER
	IMP_MODULE_HANDLE hTracker;
	IpTargetTracker *pTracker;
#endif
#ifdef DEBUG_OBJRECG_CLASSIFIER
	IMP_MODULE_HANDLE hClassifier;
	IpTargetClassifier *pClassifier ;
#endif
#ifdef DEBUG_OBJRECG_ANALYST
	IMP_MODULE_HANDLE hAnalyst ;
	BEHAVIOR_ANALYSIS_S* pAnalyst ;
#endif
#endif

	PEA_MODULE *pPEA ;
	IMP_MODULE_HANDLE hObjRecg;
	IMP_MODULE_HANDLE hEvtAnls;


    cvNamedWindow("videoBlob", 1);
    cvNamedWindow("videoTrajectory", 1);
    cvNamedWindow("foreground", 1);
    cvNamedWindow("background", 1);
    cvMoveWindow("videoBlob", 0, 0);
    cvMoveWindow("videoTrajectory", 360, 0);
    cvMoveWindow("background", 0, 288+60);
    cvMoveWindow("foreground", 360, 288+60);

	if (enVideoFormat == IMP_CIF)
	{
		s32ImgW = IMP_CIF_IMG_WIDTH;
		s32ImgH = IMP_CIF_PAL_IMG_HEIGHT;
	}
	else if (enVideoFormat == IMP_QCIF)
	{
		s32ImgW = IMP_QCIF_IMG_WIDTH;
		s32ImgH = IMP_QCIF_PAL_IMG_HEIGHT;
	}

    pImgGray = cvCreateImage(cvSize(s32ImgW, s32ImgH),  IPL_DEPTH_8U,1);
    pFrImg = cvCreateImage(cvSize(s32ImgW, s32ImgH),  IPL_DEPTH_8U,1);
    pBkImg = cvCreateImage(cvSize(s32ImgW, s32ImgH),  IPL_DEPTH_8U,1);
    color_dst_blob = cvCreateImage(cvSize(s32ImgW, s32ImgH), IPL_DEPTH_8U, 3 );
    color_dst_trajectory = cvCreateImage(cvSize(s32ImgW, s32ImgH), IPL_DEPTH_8U, 3 );
	
	imageDst = cvCreateImage(cvSize(s32ImgW, s32ImgH),  IPL_DEPTH_8U,1);
	
	gpstImgRMV = cvCreateImage(cvSize(s32ImgW, s32ImgH), IPL_DEPTH_8U, 1);
	gpstImgRMV2 = cvCreateImage(cvSize(s32ImgW, s32ImgH), IPL_DEPTH_8U, 3);
	
	Y_space = (IMP_U8 *)malloc(s32ImgW * s32ImgH);
	U_space = (IMP_U8 *)malloc(s32ImgW / 2  * s32ImgH /2);
	V_space = (IMP_U8 *)malloc(s32ImgW / 2  * s32ImgH /2);

	bRet=IMP_FALSE;

	s32Width = s32ImgW; s32Height = s32ImgH;
	IMP_YUVImage422Create( &stImage, s32Width, s32Height, NULL );
	IMP_GetAlgoLibInfo( hIMP, &stLibInfo );
	stMems.u32ImgW = s32Width;
	stMems.u32ImgH = s32Height;
	IMP_GetMemReq( hIMP, &stMems );
	IMP_MemsAlloc( &stMems );

    if(IMP_OSC_Create( hIMP, &stMems ) != IMP_STATUS_OK)
		exit(0);
	
    if(enVideoSource == IMP_AVI)
    {
        stImage.u32Time = 0;
		pCapture = cvCaptureFromFile(cFileName);

        if( pCapture == NULL )
        {
            printf("failed to open avi file!\n");
            pCapture = cvCaptureFromCAM(0);
            if( pCapture == NULL )
   		    {
               printf("failed to open Camera!\n");
         	   return 0;
            }
        }
    }
    else
    {
        origin += (s32ImgW * s32ImgH + (s32ImgW >> 1) * (s32ImgH >> 1) + (s32ImgW >> 1) * (s32ImgH >> 1)) * 1500;
        stImage.u32Time = 0;
    }
    
    IMP_OSC_PARA_Config(hIMP,s32Width,s32Height);
    gs32RMV_SET_OVER = 0;
    while(1)
    {
        nFrmNum++;
        printf("FrmNum:%d\n", nFrmNum);
        
        
        if(enVideoSource == IMP_AVI)
        {
            frame = cvQueryFrame( pCapture );   // acquire a frame
            
            if( !frame )
				break;
			
			if (nFrmNum < 10) continue;
      	    if(nFrmNum % s32SubSampleT != 0) continue;
            if (!imageSrc)
            {
                imageSrc = cvCreateImage( cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1 );
            }
            if (frame->nChannels!=1)
            {
                cvCvtColor( frame, imageSrc, CV_BGR2GRAY);
            }
            else
            {
                cvCopyImage(frame, imageSrc);
            }
			
			//抽样
			cvResize(imageSrc, imageDst, CV_INTER_LINEAR);	//缩放源图像到目标图像
        //    cvShowImage("Hello", imageDst);
          //  cvWaitKey(0);
			cvCopy(imageDst, pImgGray, NULL);
			cvCopy(imageDst, gpstImgRMV, NULL);
			memcpy(stImage.pu8Y,imageDst->imageData,s32ImgW * s32ImgH);
			
			if (0 && frame && !gadwRMVPntNum)
  			{
  				char byKey;
    			cvShowImage("DrawRMV", pImgGray);
    			cvSetMouseCallback("DrawRMV", on_mouse, 0);
		    	byKey = cvWaitKey(0);
				if (byKey == 'c')
		    	{
		    		exit(0);
		    	}
		    	else if (byKey == 's')
		    	{
		    		IMP_OSC_PARA_Config(hIMP, s32Width, s32Height);
		    		gs32RMV_SET_OVER = 1;
		    		printf("RMV Object is configured!\n");
		    	}
		    }
        }
        else
        {
			
        }
//printf("vfprint:0\n");
        // prepare stImage...
		IMP_OSC_ProcessImage( hIMP, &stImage );
//printf("vfprint:0_1\n");
		IMP_OSC_GetResults( hIMP, &stResult );
//printf("vfprint:0_2\n");

		pModule = (PEA_MODULE*)hIMP;
        pResult= pModule->pstResult;
        pPEA = (PEA_MODULE*)pModule;
        hObjRecg = pPEA->hObjRecg;
        hEvtAnls = pPEA->hEvtAnls;

		memcpy(pFrImg->imageData,pResult->stDRegionSet.pstImgFgRgn->pu8Data,s32ImgW*s32ImgH);
		memcpy(pBkImg->imageData,pResult->stDRegionSet.pstImgBgGray->pu8Data,s32ImgW*s32ImgH);
     	
		cvCvtColor( pImgGray, color_dst_blob, CV_GRAY2BGR );
		cvCvtColor( pImgGray, color_dst_trajectory, CV_GRAY2BGR );

	//	draw_motion_trajectory_dregion(pPEA->pstResult,color_dst_blob,IMP_PROCESS_OSC);
		draw_motion_trajectory_ttarget( pPEA->pstResult, color_dst_blob, 1 );
        //draw_classtype_trajectory_ttarget(pPEA->m_pResult, color_dst_trajectory, 1 );
    //  draw_motion_trajectory_ntarget(&stResult,color_dst_blob,2);

        //ShowTargetMsg(pModule,color_dst_trajectory);
#if 1
        ShowPEAResult(&stResult,color_dst_blob);

        ShowPeaRule(&stURPpara,color_dst_blob);
#endif
	    cvShowImage("videoBlob", color_dst_blob);
	    
        cvShowImage("videoTrajectory", color_dst_trajectory);
        cvShowImage("background", pBkImg);
		
		IMP_S32 dwPI;
		for (dwPI = 0; dwPI < s32ImgW*s32ImgH; dwPI++)
		{
			pFrImg->imageData[dwPI] = (pFrImg->imageData[dwPI] != 0) * 255;
		}
		
        cvShowImage("foreground", pFrImg);
     //   printf("HI\n");
     
     	if (nFrmNum == 1033)
     	{
        	key = cvWaitKey(0);
        }
        else
        {
        	key = cvWaitKey(10);
        }
		
        if (stResult.stEventSet.s32EventNum > 0)
        {
          //  key = cvWaitKey(10);
        }
		
        //time increase
        stImage.u32Time += s32SubSampleT;
        printf("stImage.u32Time:%d, s32SubSampleT:%d\n", stImage.u32Time, s32SubSampleT);

		if (key == 's')//单步
		{
			key = cvWaitKey(0);//等待20ms
		}
		else if (key == 'p')//暂停
		{
			key = cvWaitKey(0);//无限等待
		}
		else if( key == 'c' )
		{
			key = 0;
		    break;
		}
		else
		{
			printf("----------------------------\n");
		}
    }

	IMP_OSC_Release( hIMP );
	IMP_MemsFree( &stMems );
	IMP_YUVImage422Destroy( &stImage, NULL );
    cvDestroyWindow("videoBlob");
    cvDestroyWindow("videoTrajectory");
    cvDestroyWindow("background");
    cvDestroyWindow("foreground");
	cvReleaseImage(&pImgGray);
	cvReleaseImage(&color_dst_blob);
	cvReleaseImage(&color_dst_trajectory);
	cvReleaseImage(&pFrImg);
	cvReleaseImage(&pBkImg);
	cvReleaseImage(&imageSrc);
	cvReleaseImage(&imageDst);
	cvReleaseImage(&gpstImgRMV);
	cvReleaseImage(&gpstImgRMV2);
	cvReleaseCapture(&pCapture);
	
 	free(Y_space);
 	free(U_space);
 	free(V_space);

    return 0;
}

int main(int argc,char *argv[])
{
    IMP_S32 m_frame_width=0,m_frame_height=0,m_interFrame=0;
    VIDEO_FORMAT_E videoFormat;
    colors[0] = CV_RGB(255,0,0);
	colors[1] = CV_RGB(255,0,0);
	colors[2] = CV_RGB(0,255,0);
	colors[3] = CV_RGB(255,255,0);
	colors[4] = CV_RGB(255,0,128);
	colors[5] = CV_RGB(0,255,128);
	colors[6] = CV_RGB(128,255,128);
	colors[7] = CV_RGB(128,255,0);
	colors[8] = CV_RGB(255,128,0);
	colors[9] = CV_RGB(0,255,128);
	colors[10] = CV_RGB(255,128,0);
	colors[11] = CV_RGB(128,0,255);
	colors[12] = CV_RGB(128,255,0);
	colors[13] = CV_RGB(0,0,255);

	IMP_S8 *fileName = "/home/zm/video/OSC/OSC_1004_1.MTS";
	
#ifdef cif
	m_frame_width = 352;
	m_frame_height = 288;
	videoFormat = IMP_CIF;
#else
    m_frame_width = 176;
	m_frame_height = 144;
	videoFormat = IMP_QCIF;
#endif

	m_interFrame = 2;

	IMP_OpencvExample(fileName,IMP_AVI,m_frame_width,m_frame_height,m_interFrame,videoFormat);

    return 0;
}



