#include <stdio.h>
#include <cv.h>
#include <cxcore.h>
#include <highgui.h>

#include "imp_algo_type.h"
#include "imp_algo_interface.h"
#include "imp_algo_urp_param.h"
#include "imp_pea_common.h"
#include "imp_pea_system.h"
#include "imp_draw_osd.h"

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
	tts = &rs->stTrackedTargetSet;
	target = tts->astTargets;
	cnt = tts->s32UsedTotal;
    line_thickness=1;
    flag1=0;

	for( i=0, j=0; i<IMP_MAX_TGT_CNT; i++ )
	{
        if( target->stTargetInfo.u32TgtEvent & IMP_TGT_EVENT_PERIMETER )
        {

			n = 0;
			pos0 = ipTargetTrajectoryGetPosition( &target->stTrajectory, 0 );
			num = ipTargetTrajectoryGetLength( &target->stTrajectory );
			pt1 = &pos0->stPt;
			pcrLine = &(colors[target->u32TargetId%12]);

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
			{
				pcrRect = &(colors[4]);//
			}

			for( k=1; k<num; k++ )
			{
				pos1 = ipTargetTrajectoryGetPosition( &target->stTrajectory, -k );
				pt2 = &pos1->stPt;
				cvLine( img, cvPoint(pt1->s16X,pt1->s16Y), cvPoint(pt2->s16X,pt2->s16Y),
					*pcrLine, line_thickness, CV_AA, 0 );
				pt1 = pt2;
			}
            cvRectangle( img, cvPoint(pos0->stRg.s16X1,pos0->stRg.s16Y1), cvPoint(pos0->stRg.s16X2,pos0->stRg.s16Y2),*pcrRect, 0, CV_AA, 0 );

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


	/*pts.s16X=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[0].stLine.stStartPt.s16X;
     pts.s16Y=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[0].stLine.stStartPt.s16Y;
	 pte.s16X=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[0].stLine.stEndPt.s16X;
     pte.s16Y=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[0].stLine.stEndPt.s16Y;
                cvLine( img, cvPoint(pts.s16X,pts.s16Y), cvPoint(pte.s16X,pte.s16Y),
					*pcrLine1, line_thickness, CV_AA, 0 );

     pts.s16X=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[7].stLine.stStartPt.s16X;
     pts.s16Y=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[7].stLine.stStartPt.s16Y;
	 pte.s16X=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[7].stLine.stEndPt.s16X;
     pte.s16Y=stURPpara.stRuleSet.astRule[0].para.stTripwireRulePara.astLines[7].stLine.stEndPt.s16Y;
     cvLine( img, cvPoint(pts.s16X,pts.s16Y), cvPoint(pte.s16X,pte.s16Y),
					*pcrLine1, line_thickness, CV_AA, 0 );

     pts.s16X=stURPpara.stRuleSet.astRule[7].para.stTripwireRulePara.astLines[0].stLine.stStartPt.s16X;
     pts.s16Y=stURPpara.stRuleSet.astRule[7].para.stTripwireRulePara.astLines[0].stLine.stStartPt.s16Y;
	 pte.s16X=stURPpara.stRuleSet.astRule[7].para.stTripwireRulePara.astLines[0].stLine.stEndPt.s16X;
     pte.s16Y=stURPpara.stRuleSet.astRule[7].para.stTripwireRulePara.astLines[0].stLine.stEndPt.s16Y;
     cvLine( img, cvPoint(pts.s16X,pts.s16Y), cvPoint(pte.s16X,pte.s16Y),
					*pcrLine1, line_thickness, CV_AA, 0 );

					*/

    //printf("Nice PEA result:\n");
    //printf("event num:%d\n",result->stEventSet.s32EventNum);
    for(i=0;i<result->stEventSet.s32EventNum;i++)
    {
        if(result->stEventSet.astEvents[i].u32Status==IMP_EVT_STATUS_START)
        {
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
               // printf("Target:%d perimeter start \n",result->stEventSet.astEvents[i].u32Id);

                pdataPerimeter=(EVT_DATA_PERIMETER_S*)(result->stEventSet.astEvents[i].au8Data);

			    //Send VCA Alarm Msg
			    for (k = 0; k < result->stTargetSet.s32TargetNum; k++)
			    {
				    if (pdataPerimeter->u32TId == result->stTargetSet.astTargets[k].u32Id)
				    {
                      width = result->stTargetSet.astTargets[k].stRect.s16X2 - result->stTargetSet.astTargets[k].stRect.s16X1;
                      height = result->stTargetSet.astTargets[k].stRect.s16Y2 - result->stTargetSet.astTargets[k].stRect.s16Y1;

                      printf("width = %d, height = %d, area = width * height = %d\n", width ,height , width * height);
                       if(result->stTargetSet.astTargets[k].u32Type == IMP_TGT_TYPE_HUMAN)
				       {
                            printf("zone: %d, event: %s, state: 0, tid: %u, type: %s\n",
				 		                    zone, "perimeter", pdataPerimeter->u32TId,
				 		                   "HUMAN");
                        }
				       else if(result->stTargetSet.astTargets[k].u32Type == IMP_TGT_TYPE_VEHICLE)
				       {
                            printf("zone: %d, event: %s, state: 0, tid: %u, type: %s\n",
				 		                    zone, "perimeter", pdataPerimeter->u32TId,
				 		                   "VEHICLE");
				       }
				       else
				       {
				           printf("zone: %d, event: %s, state: 0, tid: %u, type: %s\n",
				 		                    zone, "perimeter", pdataPerimeter->u32TId,
							                   "UNKNOWN");
				       }

						break;
				    }
			    }


                /*num1=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pt_num;
                for(i=0;i<num1;i++)
                {
                    pt[i].s16X=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pts[i].s16X;
                    pt[i].s16Y=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pts[i].s16Y;
                }

                cvFillConvexPoly(img,pt,num1,*pcrRect);
                pdataPerimeter=(NICE_EVT_DATA_PERIMETER*)(result->stEventSet.astEvents[i].au8Data);
              */
                //printf("Target %d perimeter\n",result->stEventSet.astEvents[i].u32Id);
            }
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
                pdataTripwire=(EVT_DATA_TRIPWIRE_S*)(result->stEventSet.astEvents[i].au8Data);
               // printf("Target:%d tripwire start, zone = %d , u32TId = 0x%x\n",result->stEventSet.astEvents[i].u32Id, zone,pdataTripwire->u32TId);
                pts=pdataTripwire->stRule.stLine.stPs;
                pte=pdataTripwire->stRule.stLine.stPe;

			    for (k = 0; k < result->stTargetSet.s32TargetNum; k++)
			    {
				   if (pdataTripwire->u32TId == result->stTargetSet.astTargets[k].u32Id)
				   {
				       if(result->stTargetSet.astTargets[k].u32Type == IMP_TGT_TYPE_HUMAN)
				       {
						printf("rule: %d, event: %s, state: 1, tid: %u, type: %s\n",
							                   zone, "tripwire", pdataTripwire->u32TId,
							                   "HUMAN");
				       }
				       else if(result->stTargetSet.astTargets[k].u32Type == IMP_TGT_TYPE_VEHICLE)
				       {
                            printf("rule: %d, event: %s, state: 1, tid: %u, type: %s\n",
							                   zone, "tripwire", pdataTripwire->u32TId,
							                   "VEHICLE");
				       }
				       else
				       {
				           printf("rule: %d, event: %s, state: 1, tid: %u, type: %s\n",
							                   zone, "tripwire", pdataTripwire->u32TId,
							                   "UNKNOWN");
				       }

						break;
				   }
			    }


               //printf("start: i=%d,pts=(x=%d,y=%d),pte=(x=%d,y=%d)\n",i,pts.s16X,pts.s16Y,pte.s16X,pte.s16Y);
                line_thickness=10;
                cvLine( img, cvPoint(pts.s16X,pts.s16Y), cvPoint(pte.s16X,pte.s16Y),
					*pcrLine, line_thickness, CV_AA, 0 );

                tmp = 0;

                for (j = 0; j < IMP_URP_MAX_TRIPWIRE_CNT; j++)
				{
					if (stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].s32Valid &&
                        pts.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16X &&
						pts.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16Y &&
						pte.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16X &&
						pte.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16Y)
					{
                          rec_count++;
                          //printf("#########################rec_count++ = %d########################\n",rec_count);
                          tmp++;
					}
                }
                //printf("--------------------------start line = %d!----------------------------------\n",tmp);

            }

        }
        if(result->stEventSet.astEvents[i].u32Status == IMP_EVT_STATUS_PROCEDURE)
        {
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {

                zone=result->stEventSet.astEvents[i].u32Zone;
               // printf("Target:%d perimeter procedure \n",result->stEventSet.astEvents[i].u32Id);

                /*num1=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pt_num;
                for(i=0;i<num1;i++)
                {
                    pt[i].s16X=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pts[i].s16X;
                    pt[i].s16Y=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pts[i].s16Y;
                }

                cvFillConvexPoly(img,pt,num1,*pcrRect);
                pdataPerimeter=(NICE_EVT_DATA_PERIMETER*)(result->stEventSet.astEvents[i].au8Data);
               */
                //printf("Target %d perimeter\n",result->stEventSet.astEvents[i].u32Id);
            }
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                zone=result->stEventSet.astEvents[i].u32Zone;
               //printf("Target:%d tripwire procedure, zone = %d \n",result->stEventSet.astEvents[i].u32Id, zone);


                pdataTripwire=(EVT_DATA_TRIPWIRE_S*)(result->stEventSet.astEvents[i].au8Data);

                pts=pdataTripwire->stRule.stLine.stPs;
                pte=pdataTripwire->stRule.stLine.stPe;
               /* if(result->stEventSet.astEvents[i].u32Id == 39 && 0)
                {
                    printf("PROCEDURE##event id: %d == 39\n",result->stEventSet.astEvents[i].u32Id);

                    printf("Target:%d tripwire start, zone = %d , u32TId = 0x%x\n",result->stEventSet.astEvents[i].u32Id, zone,pdataTripwire->u32TId);

                    printf("PROCEDURE: i=%d,pts=(x=%d,y=%d),pte=(x=%d,y=%d)\n",i,pts.s16X,pts.s16Y,pte.s16X,pte.s16Y);
                }*/

                line_thickness=10;
                cvLine( img, cvPoint(pts.s16X,pts.s16Y), cvPoint(pte.s16X,pte.s16Y),
					*pcrLine, line_thickness, CV_AA, 0 );

            }

        }
        else if(result->stEventSet.astEvents[i].u32Status==IMP_EVT_STATUS_END)
        {

            pcrRect = &(colors[2]);
             if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmPerimeter)
            {
                //printf("Target:%d perimeter end \n",result->stEventSet.astEvents[i].u32Id);

                 //zone = result->stEventSet.astEvents[i].u32Zone;
				// NICE_EVT_DATA_PERIMETER *pData = (NICE_EVT_DATA_PERIMETER *)result->stEventSet.astEvents[i].au8Data;


                /*zone=result->stEventSet.astEvents[i].u32Zone;

                num1=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pt_num;
                for(i=0;i<num1;i++)
                {
                    pt[i].s16X=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pts[i].s16X;
                    pt[i].s16Y=stURPpara.stRuleSet.astRule[zone].stPara.perimeter_rule_para.perimeter_limit_para.perimeter_boundary.boundary_pts[i].s16Y;
                }

                cvFillConvexPoly(img,pt,num1,*pcrRect);
                pdataPerimeter=(NICE_EVT_DATA_PERIMETER*)(result->stEventSet.astEvents[i].au8Data);
                */
                //printf("Target %d perimeter\n",result->stEventSet.astEvents[i].u32Id);
            }
            if(result->stEventSet.astEvents[i].u32Type==IMP_EVT_TYPE_AlarmTripwire)
            {
                pdataTripwire=(EVT_DATA_TRIPWIRE_S*)(result->stEventSet.astEvents[i].au8Data);
                zone=result->stEventSet.astEvents[i].u32Zone;

                //printf("Target:%d tripwire end, zone = %d , u32TId = 0x%x \n",result->stEventSet.astEvents[i].u32Id, zone, pdataTripwire->u32TId);
                pts=pdataTripwire->stRule.stLine.stPs;
                pte=pdataTripwire->stRule.stLine.stPe;
                //printf("end: i=%d,pts=(x=%d,y=%d),pte=(x=%d,y=%d)\n",i,pts.s16X,pts.s16Y,pte.s16X,pte.s16Y);

                tmp = 0;
                for (j = 0; j < IMP_URP_MAX_TRIPWIRE_CNT; j++)
				{
					if (stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].s32Valid &&
                        pts.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16X &&
						pts.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stStartPt.s16Y &&
						pte.s16X == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16X &&
						pte.s16Y == stURPpara.stRuleSet.astRule[zone].stPara.stTripwireRulePara.astLines[j].stLine.stEndPt.s16Y)
					{

                        rec_count--;
                       //  printf("#########################rec_count-- = %d########################\n",rec_count);
                         tmp++;
					}
                }
                //printf("--------------------------end line = %d!----------------------------------\n",tmp);
                //if(j == URP_MAX_TRIPWIRE_CNT )
                    //printf("--------------------------Error here: End!----------------------------------\n");
            }

        }
    }
    //printf("rec_count=%d\n",rec_count);
}

#define PAR_SRC_FILE 0
#define MAX_RULE_DATA_SIZE 64*1024
static void IMP_PARA_Config( IMP_MODULE_HANDLE hModule, IMP_S32 s32ImgW, IMP_S32 s32ImgH )
{
//	EXTERNAL_PARA_S *pstPara = IMP_PARA_Alloc( IMP_PARA_RULE_BUFLEN,
//		IMP_PARA_ADVBUF_BUFCNT, IMP_PARA_ADVBUF_BUFLEN, NULL );

	memset(&stURPpara,0,sizeof(URP_PARA_S));
	{
		stURPpara.stConfigPara.s32ImgW = 352;
		stURPpara.stConfigPara.s32ImgH = 288;
		stURPpara.stRuleSet.astRule[0].u32Enable = 1;
		stURPpara.stRuleSet.astRule[0].u32Valid = 1;
		stURPpara.stRuleSet.astRule[0].u32Mode |= IMP_FUNC_PERIMETER;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32Mode = IMP_URP_PMODE_INTRUSION;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeHuman = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.s32TypeVehicle = 1;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32DirectionLimit = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32ForbiddenDirection = 180;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32MinDist = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.s32MinTime = 0;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.s32BoundaryPtNum = 4;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16X = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[0].s16Y = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16X = 10;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[1].s16Y = 270;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16X = 340;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[2].s16Y = 270;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16X = 340;
		stURPpara.stRuleSet.astRule[0].stPara.stPerimeterRulePara.stLimitPara.stBoundary.astBoundaryPts[3].s16Y = 10;

		stURPpara.stRuleSet.astRule[1].u32Enable = 1;
		stURPpara.stRuleSet.astRule[1].u32Valid = 1;
		stURPpara.stRuleSet.astRule[1].u32Mode |= IMP_FUNC_TRIPWIRE;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeLimit = 0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeHuman =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.s32TypeVehicle =1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.stLimitPara.s32MinDist=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.stLimitPara.s32MinTime=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32ForbiddenDirection=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32IsDoubleDirection=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].s32Valid=1;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16X=232;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stStartPt.s16Y=0;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16X=232;
		stURPpara.stRuleSet.astRule[1].stPara.stTripwireRulePara.astLines[0].stLine.stEndPt.s16Y=270;

        stURPpara.stEnvironment.u32Enable = 0;
		stURPpara.stEnvironment.s32SceneType = IMP_URP_INDOOR;

		stURPpara.stFdepth.u32Enable = 1;
		stURPpara.stFdepth.stMeasure.stFdzLines.u32NumUsed = 3;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[0].stRefLine.stPs.s16X = 244;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[0].stRefLine.stPs.s16Y = 206;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[0].stRefLine.stPe.s16X = 244;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[0].stRefLine.stPe.s16Y = 237;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[0].s32RefLen = 170;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[1].stRefLine.stPs.s16X = 70;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[1].stRefLine.stPs.s16Y = 153;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[1].stRefLine.stPe.s16X = 70;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[1].stRefLine.stPe.s16Y = 173;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[1].s32RefLen = 170;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[2].stRefLine.stPs.s16X = 82;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[2].stRefLine.stPs.s16Y = 205;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[2].stRefLine.stPe.s16X = 83;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[2].stRefLine.stPe.s16Y = 239;
		stURPpara.stFdepth.stMeasure.stFdzLines.stLines[2].s32RefLen = 180;


	}
//	IMP_PARA_Init( pstPara, NULL, NULL, s32ImgW, s32ImgH, NULL );
	IMP_ConfigArmPeaParameter( hModule, NULL ,&stURPpara );
//	IMP_PARA_Free( pstPara, IMP_PARA_ADVBUF_BUFCNT, NULL );
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
    cvMoveWindow("background", 0, 288);
    cvMoveWindow("foreground", 360, 288);

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

    if(IMP_Create( hIMP, &stMems ) != IMP_STATUS_OK)
		exit(0);

	IMP_PARA_Config(hIMP,s32Width,s32Height);

	IMP_Start( hIMP );

    if(enVideoSource == IMP_AVI)
    {
        stImage.u32Time = 0;
		pCapture = cvCaptureFromFile(cFileName);

        if( pCapture == NULL )
        {
            printf("failed to open avi file!/n");
            return 0;
        }
    }
    else
    {
        origin += (s32ImgW * s32ImgH + (s32ImgW >> 1) * (s32ImgH >> 1) + (s32ImgW >> 1) * (s32ImgH >> 1)) * 1500;
        stImage.u32Time = 0;
    }

    while(1)
    {
        nFrmNum++;
        if(enVideoSource == IMP_AVI)
        {
            frame = cvQueryFrame( pCapture );   // acquire a frame
            if( !frame )
				break;

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
            cvShowImage("Hello", imageDst);
            cvWaitKey(0);
			cvCopy(imageDst, pImgGray, NULL);
			memcpy(stImage.pu8Y,imageDst->imageData,s32ImgW * s32ImgH);
        }
        else
        {
			//read YUV video,hall_monitor_cif.y4m noise.yuv sun_shadow.yuv
			flag=ReadImage(Y_space,cFileName,s32ImgW,s32ImgH,origin);
			if(flag!=0)
				break;

			// flag=ReadImage(U_space,"sun_shadow.yuv",(s32ImgW >> 1),(s32ImgH >> 1),s32ImgW*s32ImgH);
			//if(flag!=0)
			//    break;
			//flag=ReadImage(V_space,"sun_shadow.yuv",(s32ImgW >> 1),(s32ImgH >> 1),s32ImgW*s32ImgH+(s32ImgW >> 1)*(s32ImgH >> 1));
			//if(flag!=0)
			//   break;

			//offset
			origin+=(s32ImgW*s32ImgH+(s32ImgW >> 1)*(s32ImgH >> 1)+(s32ImgW >> 1)*(s32ImgH >> 1))*s32SubSampleT;
			memcpy(pImgGray->imageData,Y_space,s32ImgW*s32ImgH);
			memcpy(stImage.pu8Y,Y_space,s32ImgW*s32ImgH);
			//memcpy(stImage.dataU,U_space,(s32ImgW >> 1)*(s32ImgH >> 1));
			//memcpy(stImage.dataV,V_space,(s32ImgW >> 1)*(s32ImgH >> 1));
        }

        // prepare stImage...
		IMP_ProcessImage( hIMP, &stImage );
		IMP_GetResults( hIMP, &stResult );
		pModule = (PEA_MODULE*)hIMP;
        pResult= pModule->pstResult;
        pPEA = (PEA_MODULE*)pModule;
        hObjRecg = pPEA->hObjRecg;
        hEvtAnls = pPEA->hEvtAnls;
#ifdef DEBUG_OBJRECG
		pObjRecg = (PEA_ModuleObjRecognition*)hObjRecg;
		pEvtAnls = (IpModuleBehaviorAnalysis*)hEvtAnls;
#ifdef DEBUG_OBJRECG_DETECTOR
		hDetector = pObjRecg->hDetector;
		pDetector = (PEA_TARGET_DETECTOR_S*)(*(IMP_U32*)hDetector);
#endif
#ifdef DEBUG_OBJRECG_TRACKER
		hTracker = pObjRecg->hTracker;
		pTracker = (IpTargetTracker*)(*(IMP_U32*)hTracker);
#endif
#ifdef DEBUG_OBJRECG_CLASSIFIER
		hClassifier = pObjRecg->hClassifier;
		pClassifier = (IpTargetClassifier*)(*(IMP_U32*)hClassifier);
#endif
#ifdef DEBUG_OBJRECG_ANALYST
		hAnalyst = pEvtAnls->hAnalyst;
		pAnalyst = (BEHAVIOR_ANALYSIS_S*) (*(IMP_U32*) hAnalyst);
#endif
#endif

#ifdef USE_LI_FAST_BGMODEL
		memcpy(pFrImg->imageData,pDetector->stLiFastModel.stImgFg.pu8Data,s32ImgW*s32ImgH);
		//memcpy(pBkImg->imageData,pDetector->liFastModel.prev_backimage.data,s32ImgW*s32ImgH);
		memcpy(pBkImg->imageData,pDetector->stImgInGrayDiff.pu8Data,s32ImgW*s32ImgH);

#endif
#ifdef USE_CODEBOOK_MODELING
		memcpy(pFrImg->imageData,pDetector->stCodebookBgModel.stImgFg.pu8Data,s32ImgW*s32ImgH);
		memcpy(pBkImg->imageData,pDetector->stCodebookBgModel.stImgBg.pu8Data,s32ImgW*s32ImgH);
#endif
        cvCvtColor( pImgGray, color_dst_blob, CV_GRAY2BGR );
        cvCvtColor( pImgGray, color_dst_trajectory, CV_GRAY2BGR );
        draw_motion_trajectory_dregion(pPEA->pstResult,color_dst_blob,IMP_PROCESS_CHG);
        //draw_motion_trajectory_ttarget( pPEA->m_pResult, color_dst_blob, 1 );
        //draw_classtype_trajectory_ttarget(pPEA->m_pResult, color_dst_trajectory, 1 );
        draw_motion_trajectory_ntarget(&stResult,color_dst_blob,2);
        //ShowTargetMsg(pModule,color_dst_trajectory);
        ShowPEAResult(&stResult,color_dst_blob);

	    cvShowImage("videoBlob", color_dst_blob);
        cvShowImage("videoTrajectory", color_dst_trajectory);
        cvShowImage("background", pBkImg);
        cvShowImage("foreground", pFrImg);
        //time increase
        stImage.u32Time += s32SubSampleT;
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
			key = cvWaitKey(33);
		}
    }

	IMP_Stop( hIMP, &stResult );
	IMP_Release( hIMP );
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
	cvReleaseCapture(&pCapture);

 	free(Y_space);
 	free(U_space);
 	free(V_space);

    return 0;
}
int main()
{
    IMP_S32 m_frame_width=0,m_frame_height=0,m_interFrame=0;
    VIDEO_FORMAT_E videoFormat;
    printf("%d,%d,%d\n", m_frame_width, m_frame_height, m_interFrame);
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

#ifdef cif
    IMP_S8 *fileName = "../../normal.avi";
    //IMP_S8 *fileName = "../../all-1-cif.avi";
	//IMP_S8 *fileName = "../../normal.yuv";
	m_frame_width = 352;
	m_frame_height = 288;
	videoFormat = IMP_CIF;
#else
    IMP_S8 *fileName = "/home/zm/video/PEA/00005.avi";
    m_frame_width = 176;
	m_frame_height = 144;
	videoFormat = IMP_QCIF;
#endif

	m_interFrame = 3;

	IMP_OpencvExample(fileName,IMP_AVI,m_frame_width,m_frame_height,m_interFrame,videoFormat);
    return 0;
}
