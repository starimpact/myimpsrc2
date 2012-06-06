#include <unistd.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/fb.h>

#include "hi_tde_api.h"
#include "hi_tde_type.h"
#include "hi_tde_errcode.h"
#include "hifb.h"
#include "vo_open.h"

//#include "AlgoNICE_IF.h"
#include "imp_avd_thread.h"
//extern IP_RESULT g_IP_Result;
//#define IMAGE_WIDTH     640
//#define IMAGE_HEIGHT    352
//#define IMAGE_SIZE      (640*352*2)
#define IMAGE_WIDTH     1280
#define IMAGE_HEIGHT    1024
#define IMAGE_SIZE      (1280*1024*2)
#define IMAGE_NUM       14
#define IMAGE_PATH		"./res/%d.bits"




//#define VIR_SCREEN_WIDTH	640					/*virtual screen width*/
//#define VIR_SCREEN_HEIGHT	IMAGE_HEIGHT*2		/*virtual screen height*/
#define VIR_SCREEN_WIDTH	1280					/*virtual screen width*/
#define VIR_SCREEN_HEIGHT	IMAGE_HEIGHT*2		/*virtual screen height*/
static struct fb_bitfield g_r16 = {10, 5, 0};
static struct fb_bitfield g_g16 = {5, 5, 0};
static struct fb_bitfield g_b16 = {0, 5, 0};
static struct fb_bitfield g_a16 = {15, 1, 0};
#define PIXFMT  TDE2_COLOR_FMT_ARGB1555
static TDE2_SURFACE_S g_stScreen[2];
static TDE2_SURFACE_S* g_pstBackGround = NULL;
static TDE2_SURFACE_S g_stScreen_pic[2];
static TDE2_SURFACE_S g_stBackGround0;
static TDE2_SURFACE_S g_stBackGround1;
static TDE2_SURFACE_S g_stBackGround2;
static TDE2_SURFACE_S g_stBackGround3;
static TDE2_SURFACE_S g_stBackGround4;
static TDE2_SURFACE_S g_stBackGround5;
static TDE2_SURFACE_S g_stPingBuf;
static TDE2_SURFACE_S g_stPangBuf;
//#define printf(...)

static HI_S32 SetTrasValueBydevPath(IMP_S32 fd)
{

	HIFB_ALPHA_S stAlpha;
//	const char* file = pszDevPath;


	IMP_S32 ret;
    HIFB_COLORKEY_S CKey;

//	tellme_out("blair:u8Alpha0\n");
    if (ioctl(fd, FBIOGET_ALPHA_HIFB, &stAlpha) < 0)
    {
        fprintf (stderr, "Couldn't set alpha\n");
        ret = -1;
    }

	stAlpha.bAlphaEnable=HI_TRUE;
	stAlpha.bAlphaChannel=HI_FALSE;
	stAlpha.u8Alpha0 = 0xC0;
	//stAlpha.u8Alpha0 = 0x70;
    stAlpha.u8Alpha1 = 0xff;
    //stAlpha.u8GlobalAlpha = 0xff;
    ret = ioctl(fd, FBIOPUT_ALPHA_HIFB, &stAlpha);
    if (ret < 0)
    {
        fprintf (stderr, "Couldn't set alpha\n");
        ret = -1;
    }

	//getchar();
	ret = ioctl(fd, FBIOGET_COLORKEY_HIFB, &CKey);
        if (ret < 0)
        {
            fprintf (stderr, "Couldn't get colorkey\n");
            return -1;
        }
        CKey.bKeyEnable = HI_TRUE;
        CKey.u32Key = 0x00000000;
       // CKey.u32Key = 0x000000FF;
        if (ioctl(fd, FBIOPUT_COLORKEY_HIFB, &CKey) < 0)
        {
            fprintf (stderr, "Couldn't put colorkey\n");
            return -1;
        }

        return ret;
}

#define AVD_IMAGE_WIDTH     800
#define AVD_IMAGE_HEIGHT    600
#define AVD_IMAGE_SIZE      (800*600*2)
#define AVD_IMAGE_NUM       14
#define AVD_IMAGE_PATH		"./res/background.bits"

#define AVD_VIR_SCREEN_WIDTH	800					/*virtual screen width*/
#define AVD_VIR_SCREEN_HEIGHT	AVD_IMAGE_HEIGHT*2		/*virtual screen height*/

static TDE2_SURFACE_S g_AVDstScreen[2];
static TDE2_SURFACE_S* g_AVDpstBackGround = NULL;
static TDE2_SURFACE_S g_AVDstPingBuf;
static TDE2_SURFACE_S g_AVDstPangBuf;
static TDE2_SURFACE_S g_stBackGround;

static HI_S32 TDE_CreateSurfaceByFile(const HI_CHAR *pszFileName, TDE2_SURFACE_S *pstSurface, HI_U8 *pu8Virt)
{
    FILE *fp;
    HI_U32 colorfmt, w, h, stride;

    if((NULL == pszFileName) || (NULL == pstSurface))
    {
        printf("%s, LINE %d, NULL ptr!\n", __FUNCTION__, __LINE__);
        return -1;
    }

    fp = fopen(pszFileName, "rb");
    if(NULL == fp)
    {
        printf("error when open pszFileName %s, line:%d\n", pszFileName, __LINE__);
        return -1;
    }

    fread(&colorfmt, 1, 4, fp);
    fread(&w, 1, 4, fp);
    fread(&h, 1, 4, fp);
    fread(&stride, 1, 4, fp);

    pstSurface->enColorFmt = colorfmt;
    pstSurface->u32Width = w;
    pstSurface->u32Height = h;
    pstSurface->u32Stride = stride;
    pstSurface->u8Alpha0 = 0;
    pstSurface->u8Alpha1 = 0xff;
    pstSurface->bAlphaMax255 = HI_TRUE;

    fread(pu8Virt, 1, stride*h, fp);

    fclose(fp);

    return 0;
}
//extern AVD_RESULT_S g_AVD_Reslut;
void TDE_OUTPUT_AVD_RESULT(TDE_HANDLE s32Handle, TDE2_SURFACE_S* pstDst,AVD_RESULT_S *result)//,TDE2_RECT_S *pstDstRect)
{
    TDE2_RECT_S pstDstRect;
    HI_U32 u32FillData = 0x7c00;
    HI_U32 u32PEAData = 0x3e0;
    HI_U32 u32TWData = 0x3e0;
    HI_U32 u32BaseLine = 0x3e0;//0x1f;

    IMP_S32 zone = 0;
    IMP_RECT_S *rc;
    IMP_RECT_S rc1;
    IMP_FLOAT scale_x = (640.0/352);
    IMP_FLOAT scale_y = (512.0/288);
    IMP_S32 i = 0;
    IMP_S32 j = 0;
    IMP_S32 pixWidth = 4;
    IMP_S32 peaPixWidth = 8;
    IMP_S32 twPixWidth = 20;
    IMP_S32 s32Ret = 0;
    IMP_S16 cr;
    TDE2_FILLCOLOR_S pstFillColor;
    TDE2_OPT_S pstOpt;
    pstFillColor.enColorFmt = TDE2_COLOR_FMT_ARGB1555;
    pstFillColor.u32FillColor = 0x7c00;//red   green:0x3e0  yellow:0x7fe0

    //pstFillColor.u32FillColor = 0x7c00;

    memset(&pstOpt,0,sizeof(pstOpt));
    pstOpt.bDeflicker = HI_TRUE;
    rc1.s16X1 = 10+79+25;
    rc1.s16Y1 = 24+10+15;
    IMP_FLOAT scale = 2;
    for(i = 0; i < 8;i++)
    {
        pstDstRect.s32Xpos = rc1.s16X1;
        pstDstRect.s32Ypos =rc1.s16Y1 + i*21;
        pstDstRect.u32Height = 18;
        if(i == 0)
        {
            pstDstRect.u32Width =AVD_MIN(g_AVD_Reslut.s32SceneChg*scale,100*scale);
        }
        else if(i == 1)
        {
            pstDstRect.u32Width =AVD_MIN(g_AVD_Reslut.s32SignalValue*scale,100*scale);
        }
        else if(i == 2)
        {
            pstDstRect.u32Width = AVD_MIN(g_AVD_Reslut.s32ClarityValue*scale,100*scale);
        }
        else if(i == 3)
        {
            pstDstRect.u32Width = AVD_MIN(g_AVD_Reslut.s32BrightHValue*scale,100*scale);
        }
        else if(i == 4)
        {
            pstDstRect.u32Width = AVD_MIN(g_AVD_Reslut.s32BrightLValue*scale,100*scale);
        }
        else if(i == 5)
        {
            pstDstRect.u32Width = AVD_MIN(g_AVD_Reslut.s32ColorValue*scale,100*scale);
        }
        else if(i == 6)
        {
            pstDstRect.u32Width = AVD_MIN(g_AVD_Reslut.s32NoiseValue*scale,100*scale);
        }
        else if(i == 7)
        {
            pstDstRect.u32Width = AVD_MIN(g_AVD_Reslut.s32FreezValue*scale,100*scale);
        }
        else if(i == 8)
        {
            pstDstRect.u32Width = 0;
        }
        pstDstRect.u32Width = AVD_MAX(0,pstDstRect.u32Width);

        if(pstDstRect.u32Width > 30*scale && pstDstRect.u32Width < 60*scale)
        {
            pstFillColor.u32FillColor = 0x7fe0;
        }
        else if(pstDstRect.u32Width >= 60*scale)
        {
            pstFillColor.u32FillColor = 0x7c00;
        }
        else
        {
            pstFillColor.u32FillColor = 0x3e0;//0x4e73;
        }
        //printf("pstDstRect.u32Width:%d\n",pstDstRect.u32Width);
        HI_TDE2_SolidDraw( s32Handle,NULL,NULL,pstDst,&pstDstRect,&pstFillColor,&pstOpt);
    }
    pstFillColor.u32FillColor = 0x1f;
    pstDstRect.s32Xpos = rc1.s16X1-8;
    pstDstRect.s32Ypos =rc1.s16Y1;
    pstDstRect.u32Height = 170;
    pstDstRect.u32Width = 10;
    HI_TDE2_SolidDraw( s32Handle,NULL,NULL,pstDst,&pstDstRect,&pstFillColor,&pstOpt);
}
#define PIC_PEA_IMAGE_PATH6		"./res/pea6.bits"
IMP_S32 RunAVDHiFBTde(IMP_S32 layer)
{
    IMP_S32 fd;
    IMP_S32 i;
	IMP_S32 j;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    HIFB_CAPABILITY_S cap;
	HI_U32 u32FixScreenStride = 0;
    unsigned char *pShowScreen;
    unsigned char *pHideScreen;
	HIFB_ALPHA_S stAlpha;
    HIFB_POINT_S stPoIMP_S32 = {0, 0};
    FILE *fp;
    char file[12] = "/dev/fb0";
    char image_name[128];
	HI_CHAR *pDst = NULL;
    TDE2_OPT_S pstOpt;
    HI_U32 u32PhyAddr;
    HI_U32 u32Size;
    HI_U32 u32CurOnShow = 0;
    HI_U32 u32NextOnShow = 1;

    TDE2_RECT_S stSrcRect;
    TDE2_RECT_S stDstRect;
    HI_S32 s32Ret;
    /* 1. open tde device */
    HI_TDE2_Open();

    /* 1. open framebuffer device overlay 0 */
    //fd = open(file, O_RDWR, 0);
    /* 2. framebuffer operation */
    fd = open("/dev/fb0", O_RDWR);
    if(fd < 0)
    {
        printf("open %s failed!\n",file);
        return -1;
    }

    /* 2. set the screen original position */
    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoIMP_S32) < 0)
    {
        printf("set screen original show position failed!\n");
        close(fd);
        return -1;
    }
#if 1
	SetTrasValueBydevPath(fd);
#else
	/* 3.set alpha */
	stAlpha.bAlphaEnable = HI_FALSE;
	stAlpha.bAlphaChannel = HI_FALSE;
	stAlpha.u8Alpha0 = 0xff;
	stAlpha.u8Alpha1 = 0x8f;
    stAlpha.u8GlobalAlpha = 0x80;

	if (ioctl(fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
	{
	    printf("Set alpha failed!\n");
        close(fd);
        return -1;
	}

#endif
    /* 4. get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
   	    printf("Get variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 5. modify the variable screen info
          the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
          the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
          (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
          the pixel format: ARGB1555
    */

    var.xres_virtual = AVD_VIR_SCREEN_WIDTH;
	var.yres_virtual = AVD_VIR_SCREEN_HEIGHT;
	var.xres = AVD_IMAGE_WIDTH;
    var.yres = AVD_IMAGE_HEIGHT;

    var.transp= g_a16;
    var.red = g_r16;
    var.green = g_g16;
    var.blue = g_b16;
    var.bits_per_pixel = 16;
    var.activate = FB_ACTIVATE_FORCE;
    /* 6. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
   	    printf("Put variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 7. get the fix screen info */
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
   	    printf("Get fix screen info failed!\n");
        close(fd);
        return -1;
    }
	u32FixScreenStride = fix.line_length;	/*fix screen stride*/

    /* 8. map the physical video memory for user use */
    u32Size 	= fix.smem_len;
    u32PhyAddr  = fix.smem_start;
    pShowScreen = mmap(NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
   	    printf("mmap framebuffer failed!\n");
        close(fd);
        return -1;
    }

    pHideScreen = pShowScreen + u32FixScreenStride*AVD_IMAGE_HEIGHT;
    memset(pShowScreen, 0x0000, u32FixScreenStride*AVD_IMAGE_HEIGHT);

    /* 9. create surface */
    g_AVDstScreen[0].enColorFmt = PIXFMT;
    g_AVDstScreen[0].u32PhyAddr = u32PhyAddr;
    g_AVDstScreen[0].u32Width = AVD_VIR_SCREEN_WIDTH;
    g_AVDstScreen[0].u32Height = AVD_IMAGE_HEIGHT;
    g_AVDstScreen[0].u32Stride = fix.line_length;
    g_AVDstScreen[0].bAlphaMax255 = HI_TRUE;
    g_AVDstScreen[1] = g_AVDstScreen[0];
    g_AVDstScreen[1].u32PhyAddr = g_AVDstScreen[0].u32PhyAddr + g_AVDstScreen[0].u32Stride * g_AVDstScreen[0].u32Height;

    g_stBackGround = g_AVDstScreen[1];
    g_stBackGround.u32PhyAddr = g_AVDstScreen[1].u32PhyAddr + g_AVDstScreen[1].u32Stride * g_AVDstScreen[1].u32Height;
    //g_stBackGround = g_stScreen[0];

    g_AVDstPingBuf = g_AVDstScreen[0];
    g_AVDstPangBuf = g_AVDstScreen[1];

 //   printf("g_AVDstPingBuf:%d\n",g_AVDstPingBuf.u32Stride);
 //   printf("g_AVDstPangBuf:%d\n",g_AVDstPangBuf.u32Stride);
    //g_stBackGround.u32PhyAddr = g_stScreen[1].u32PhyAddr + g_stScreen[1].u32Stride * g_stScreen[1].u32Height;

        // var.yoffset = (i%2)?0:1024;
         //var.yoffset = 0;
        /*set frame buffer start position*/

    i = 0;
    while(1)
    {
        IMP_S32 target_num = 0;
        i++;
        u32CurOnShow = i%2;
        u32NextOnShow = !u32CurOnShow;
        if(u32CurOnShow == 1)
        {
            g_AVDpstBackGround = &g_AVDstPingBuf;
        }
        else
        {
            g_AVDpstBackGround = &g_AVDstPangBuf;
        }
        TDE_HANDLE s32Handle;
        TDE2_OPT_S stOpt = {0};
        HI_FLOAT eXMid, eYMid;
        HI_FLOAT eRadius;
        HI_U32 i;
        HI_FLOAT f;
        HI_U32 u32NextOnShow;


        HI_U32 u32FillData = 0x8010;

        {printf("TDE2_BeginJob!\n");
        /* 1. start job */
        s32Handle = HI_TDE2_BeginJob();
        if(HI_ERR_TDE_INVALID_HANDLE == s32Handle)
        {
        //    TDE_PRINT("start job failed!\n");
            printf("start job failed!\n");
            return ;
        }

        //memset(pShowScreen, 0x0000, u32FixScreenStride*IMAGE_HEIGHT);
        stDstRect.s32Xpos = 0;
        stDstRect.s32Ypos = 0;
        stDstRect.u32Width = AVD_VIR_SCREEN_WIDTH;
        stDstRect.u32Height = AVD_IMAGE_HEIGHT;
        s32Ret = HI_TDE2_QuickFill( s32Handle, g_AVDpstBackGround,&stDstRect, 0);



        printf("TDE2_BeginJob2!\n");
        stDstRect.s32Xpos = 0;
        stDstRect.s32Ypos = 0;
        stDstRect.u32Width = 120;
        stDstRect.u32Height = 120;
        stSrcRect.s32Xpos = 0;
        stSrcRect.s32Ypos = 0;
        stSrcRect.u32Width = g_AVDpstBackGround->u32Width;
        stSrcRect.u32Height = g_AVDpstBackGround->u32Height;


        //TDE_OUTPUT_PEA_RESULT(s32Handle, g_pstBackGround,&g_IP_Result);//,&stDstRect);
        printf("TDE_OUTPUT_AVD_RESULT start!\n");
        TDE_OUTPUT_AVD_RESULT(s32Handle, g_AVDpstBackGround,&g_AVD_Reslut);//,&stDstRect);
        printf("TDE_OUTPUT_AVD_RESULT end!\n");
        memset(&pstOpt,0,sizeof(pstOpt));
        pstOpt.bDeflicker = HI_TRUE;

        /* {
            TDE_CreateSurfaceByFile(PIC_PEA_IMAGE_PATH6, g_AVDpstBackGround, pShowScreen + ((HI_U32)g_AVDpstBackGround->u32PhyAddr - u32PhyAddr));
            stSrcRect.s32Xpos = 0;
            stSrcRect.s32Ypos = 0;
            stSrcRect.u32Width = 79;
            stSrcRect.u32Height = g_AVDpstBackGround->u32Height;
            stDstRect.s32Xpos = 2;
            stDstRect.s32Ypos = 35;
            stDstRect.u32Width = 79;
            stDstRect.u32Height = g_AVDpstBackGround->u32Height;
            s32Ret = HI_TDE2_QuickCopy(s32Handle, g_AVDpstBackGround, &stSrcRect, g_AVDpstBackGround, &stDstRect);
        }*/
        /*TDE_CreateSurfaceByFile(AVD_IMAGE_PATH, g_AVDpstBackGround, pShowScreen + ((HI_U32)g_AVDpstBackGround->u32PhyAddr - u32PhyAddr));
        stDstRect.s32Xpos = 0;
        stDstRect.s32Ypos = 0;
        stDstRect.u32Width = 720;
        stDstRect.u32Height = 576;
        s32Ret = HI_TDE2_QuickCopy(s32Handle, g_AVDpstBackGround, &stDstRect, g_AVDpstBackGround, &stDstRect);
        */
        /* 4. bitblt image to screen */
        //s32Ret = HI_TDE2_Bitblit(s32Handle, NULL, &stDstRect, g_AVDpstBackGround, &stSrcRect, g_AVDpstBackGround , &stDstRect, &pstOpt);
        if(s32Ret < 0)
        {
            printf("Line:%d,HI_TDE2_QuickFill failed,ret=0x%x!\n", __LINE__, s32Ret);
            return ;
        }
         printf("TDE2_BeginJob3!\n");

        /* 5. submit job */
        s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
        if(s32Ret < 0)
        {
          //  TDE_PRINT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret);
            HI_TDE2_CancelJob(s32Handle);
            return ;
        }
        printf("TDE2_EndJob5!\n");
        }
        var.yoffset = (u32CurOnShow==1) ? 0 : AVD_IMAGE_HEIGHT;
        if (ioctl(fd, FBIOPAN_DISPLAY, &var) < 0)
        {
             //TDE_PRINT("process frame buffer device error\n");
             printf("process frame buffer device error\n");
             //goto FB_PROCESS_ERROR1;
        }
        usleep(40000);
    }


    printf("Enter to quit!\n");
    getchar();

    /* 10.unmap the physical memory */
    munmap(pShowScreen, fix.smem_len);

    /* 11. close the framebuffer device */
    close(fd);

    HI_TDE2_Close();
    return 0;
}

IMP_S32 AVD_ProcessHiFbTde()
{
	HI_S32 s32Ret = 0;
    VOU_DEV_E VoDev = HD;
    IMP_S32 layer = 0;
    //return -1;
    sleep(10);
	/*2 run hifbTde*/
	printf("AVD_ProcessHiFbTde!\n");
	s32Ret = RunAVDHiFBTde(layer);
	if(s32Ret != HI_SUCCESS)
	{
		goto err;
	}

	err:
	DisableVoDev(HD);
	DisableVoDev(SD);
	MppSysExit();

	return 0;

}
#define PIC_IMAGE_WIDTH     800
#define PIC_IMAGE_HEIGHT    600
#define PIC_IMAGE_SIZE      (800*600*2)
#define PIC_IMAGE_NUM       14
#define PIC_IMAGE_PATH		"./res/%d.bits"
#define PIC_PEA_IMAGE_PATH1		"./res/pea1.bits"
#define PIC_PEA_IMAGE_PATH2		"./res/pea2.bits"
#define PIC_PEA_IMAGE_PATH3		"./res/pea3.bits"
#define PIC_PEA_IMAGE_PATH4		"./res/pea4.bits"
#define PIC_PEA_IMAGE_PATH5		"./res/pea5.bits"

//#define VIR_SCREEN_WIDTH	640					/*virtual screen width*/
//#define VIR_SCREEN_HEIGHT	IMAGE_HEIGHT*2		/*virtual screen height*/
#define PIC_VIR_SCREEN_WIDTH	800					/*virtual screen width*/
#define PIC_VIR_SCREEN_HEIGHT	PIC_IMAGE_HEIGHT*2		/*virtual screen height*/
#define NEW_PIC_IMAGE_WIDTH     79
#define NEW_PIC_IMAGE_HEIGHT    24
#define NEW_PIC_IMAGE_SIZE      (79*24*2)
#define NEW_PIC_IMAGE_NUM       14
#define NEW_PIC_IMAGE_PATH		"./res/%d.bits"
#define NEW_PIC_PEA_IMAGE_PATH1		"./res/pea1.bits"
#define NEW_PIC_PEA_IMAGE_PATH2		"./res/pea2.bits"
#define NEW_PIC_PEA_IMAGE_PATH3		"./res/pea3.bits"
#define NEW_PIC_PEA_IMAGE_PATH4		"./res/pea4.bits"

#define NEW_PIC_VIR_SCREEN_WIDTH	79					/*virtual screen width*/
#define NEW_PIC_VIR_SCREEN_HEIGHT	PIC_IMAGE_HEIGHT*2		/*virtual screen height*/
int AVD_RunHiFBTde_PIC(int layer)
{
    int fd;
    int i;
	int j;
    struct fb_fix_screeninfo fix;
    struct fb_var_screeninfo var;
    HIFB_CAPABILITY_S cap;
	HI_U32 u32FixScreenStride = 0;
    unsigned char *pShowScreen;
    unsigned char *pHideScreen;
	HIFB_ALPHA_S stAlpha;
    HIFB_POINT_S stPoint = {10, 10};
    FILE *fp;
    char file[12] = "/dev/fb1";
    char image_name[128];
	HI_CHAR *pDst = NULL;
    //int i = 0;
    HI_U32 u32PhyAddr;
    HI_U32 u32Size;
    HI_U32 u32CurOnShow = 0;
    HI_U32 u32NextOnShow = 1;
    /* 1. open tde device */
    HI_TDE2_Open();

    /* 1. open framebuffer device overlay 0 */
    //fd = open(file, O_RDWR, 0);
    /* 2. framebuffer operation */
    fd = open("/dev/fb1", O_RDWR);
    if(fd < 0)
    {
        printf("open %s failed!\n",file);
        return -1;
    }

    /* 2. set the screen original position */
    if (ioctl(fd, FBIOPUT_SCREEN_ORIGIN_HIFB, &stPoint) < 0)
    {
        printf("set screen original show position failed!\n");
        close(fd);
        return -1;
    }
#if 1
	SetTrasValueBydevPath(fd);
#else
	/* 3.set alpha */
	stAlpha.bAlphaEnable = HI_FALSE;
	stAlpha.bAlphaChannel = HI_FALSE;
	stAlpha.u8Alpha0 = 0xff;
	stAlpha.u8Alpha1 = 0x8f;
    stAlpha.u8GlobalAlpha = 0x80;

	if (ioctl(fd, FBIOPUT_ALPHA_HIFB,  &stAlpha) < 0)
	{
	    printf("Set alpha failed!\n");
        close(fd);
        return -1;
	}

#endif
    /* 4. get the variable screen info */
    if (ioctl(fd, FBIOGET_VSCREENINFO, &var) < 0)
    {
   	    printf("Get variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 5. modify the variable screen info
          the screen size: IMAGE_WIDTH*IMAGE_HEIGHT
          the virtual screen size: VIR_SCREEN_WIDTH*VIR_SCREEN_HEIGHT
          (which equals to VIR_SCREEN_WIDTH*(IMAGE_HEIGHT*2))
          the pixel format: ARGB1555
    */

    var.xres_virtual = PIC_VIR_SCREEN_WIDTH;
	var.yres_virtual = PIC_VIR_SCREEN_HEIGHT;
	var.xres = PIC_IMAGE_WIDTH;
    var.yres = PIC_IMAGE_HEIGHT;

    var.transp= g_a16;
    var.red = g_r16;
    var.green = g_g16;
    var.blue = g_b16;
    var.bits_per_pixel = 16;
    var.activate = FB_ACTIVATE_FORCE;
    /* 6. set the variable screeninfo */
    if (ioctl(fd, FBIOPUT_VSCREENINFO, &var) < 0)
    {
   	    printf("Put variable screen info failed!\n");
        close(fd);
        return -1;
    }

    /* 7. get the fix screen info */
    if (ioctl(fd, FBIOGET_FSCREENINFO, &fix) < 0)
    {
   	    printf("Get fix screen info failed!\n");
        close(fd);
        return -1;
    }
	u32FixScreenStride = fix.line_length;	/*fix screen stride*/

    /* 8. map the physical video memory for user use */
    u32Size 	= fix.smem_len;
    u32PhyAddr  = fix.smem_start;
    pShowScreen = mmap(NULL, fix.smem_len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(MAP_FAILED == pShowScreen)
    {
   	    printf("mmap framebuffer failed!\n");
        close(fd);
        return -1;
    }

    pHideScreen = pShowScreen + u32FixScreenStride*PIC_IMAGE_HEIGHT;
    memset(pShowScreen, 0x0000, u32FixScreenStride*PIC_IMAGE_HEIGHT);

    /* 9. create surface */
    g_stScreen_pic[0].enColorFmt = PIXFMT;
    g_stScreen_pic[0].u32PhyAddr = u32PhyAddr;
    g_stScreen_pic[0].u32Width = PIC_VIR_SCREEN_WIDTH;
    g_stScreen_pic[0].u32Height = PIC_IMAGE_HEIGHT;
    g_stScreen_pic[0].u32Stride = fix.line_length;
    g_stScreen_pic[0].bAlphaMax255 = HI_TRUE;

  /*  g_stBackGround0 = g_stScreen_pic[0];
    g_stBackGround0.enColorFmt = PIXFMT;
    g_stBackGround0.u32PhyAddr = u32PhyAddr;
    g_stBackGround0.u32Width = NEW_PIC_VIR_SCREEN_WIDTH;
    g_stBackGround0.u32Height = NEW_PIC_IMAGE_HEIGHT;
    g_stBackGround0.u32Stride = NEW_PIC_VIR_SCREEN_WIDTH;
    g_stBackGround0.bAlphaMax255 = HI_TRUE;
    g_stBackGround0.u32PhyAddr = g_stScreen_pic[0].u32PhyAddr + g_stScreen_pic[0].u32Stride * g_stScreen_pic[0].u32Height;
    g_stBackGround1 = g_stBackGround0;
    g_stBackGround1.u32PhyAddr = g_stBackGround0.u32PhyAddr + g_stBackGround0.u32Stride * g_stBackGround0.u32Height;

    g_stBackGround2 = g_stBackGround1;
    g_stBackGround2.u32PhyAddr = g_stBackGround1.u32PhyAddr + g_stBackGround1.u32Stride * g_stBackGround1.u32Height;

    g_stBackGround3 = g_stBackGround2;
    g_stBackGround3.u32PhyAddr = g_stBackGround2.u32PhyAddr + g_stBackGround2.u32Stride * g_stBackGround2.u32Height;
*/
    g_stBackGround4 = g_stScreen_pic[0];
    g_stBackGround4.enColorFmt = PIXFMT;
    g_stBackGround4.u32PhyAddr = g_stScreen_pic[0].u32PhyAddr + g_stScreen_pic[0].u32Stride * g_stScreen_pic[0].u32Height;;
    g_stBackGround4.u32Width = 71;
    g_stBackGround4.u32Height = 167;
    g_stBackGround4.u32Stride = 71;
    g_stBackGround4.bAlphaMax255 = HI_TRUE;
   // g_stBackGround4.u32PhyAddr = g_stBackGround3.u32PhyAddr + g_stBackGround3.u32Stride * g_stBackGround3.u32Height;

    //g_stBackGround1 = g_stBackGround0;
    //g_stBackGround1.u32PhyAddr = g_stBackGround0.u32PhyAddr + g_stBackGround0.u32Stride * g_stBackGround0.u32Height;
  //  g_stBackGround1 = g_stScreen_pic[0];
  //  g_stBackGround2 = g_stScreen_pic[0];
  //  g_stBackGround3 = g_stScreen_pic[0];
    //g_stBackGround.u32PhyAddr = g_stScreen_pic[0].u32PhyAddr + g_stScreen_pic[0].u32Stride * g_stScreen_pic[0].u32Height;

    i = 0;
    while(1)
    {

        TDE_HANDLE s32Handle;
        TDE2_OPT_S stOpt = {0};
        HI_FLOAT eXMid, eYMid;
        HI_FLOAT eRadius;
        //HI_U32 i;
        HI_FLOAT f;
        HI_U32 u32NextOnShow;
        TDE2_RECT_S stSrcRect;
        TDE2_RECT_S stDstRect;
        HI_S32 s32Ret;
        HI_U32 u32FillData = 0x8010;

        /* 1. start job */
        s32Handle = HI_TDE2_BeginJob();
        if(HI_ERR_TDE_INVALID_HANDLE == s32Handle)
        {
        //    TDE_PRINT("start job failed!\n");
            printf("start job failed!\n");
            return ;
        }

        //memset(pShowScreen, 0x0000, u32FixScreenStride*IMAGE_HEIGHT);
        /*stDstRect.s32Xpos = 0;
        stDstRect.s32Ypos = 0;
        stDstRect.u32Width = PIC_VIR_SCREEN_WIDTH;
        stDstRect.u32Height = PIC_IMAGE_HEIGHT;*/
        i++;
        //if(i == 3)
         //   i = 0;

   /*     stDstRect.s32Xpos = 0;
        stDstRect.s32Ypos = 0;
        stDstRect.u32Width = g_stBackGround0.u32Width;
        stDstRect.u32Height = g_stBackGround0.u32Height;

        stSrcRect.s32Xpos = 0;
        stSrcRect.s32Ypos = 0;
        stSrcRect.u32Width = g_stBackGround0.u32Width;
        stSrcRect.u32Height = g_stBackGround0.u32Height;
        //stSrcRect.u32Width = g_stBackGround0.u32Width;*/
        //stSrcRect.u32Height = g_stBackGround0.u32Height;
        printf("i = %d\n",i);
 /*       if(i%5==0)
        {
            TDE_CreateSurfaceByFile(PIC_PEA_IMAGE_PATH1, &g_stBackGround0, pShowScreen + ((HI_U32)g_stBackGround0.u32PhyAddr - u32PhyAddr));
            HI_TDE2_QuickCopy(s32Handle, &g_stBackGround0, &stSrcRect, &g_stScreen_pic[0], &stDstRect);
        }
        else if(i%5==1)
        {
            TDE_CreateSurfaceByFile(PIC_PEA_IMAGE_PATH4, &g_stBackGround3, pShowScreen + ((HI_U32)g_stBackGround3.u32PhyAddr - u32PhyAddr));
            stDstRect.s32Xpos = (IMAGE_WIDTH>>1)+2;
            stDstRect.s32Ypos = (IMAGE_HEIGHT>>1)+2;
            stDstRect.u32Width = g_stBackGround3.u32Width;
            stDstRect.u32Height = g_stBackGround3.u32Height;

            s32Ret = HI_TDE2_QuickCopy(s32Handle, &g_stBackGround3, &stSrcRect, &g_stScreen_pic[0], &stDstRect);
            if(s32Ret < 0)
            {
                printf("Line:%d,HI_TDE2_QuickCopy failed,ret=0x%x!\n", __LINE__, s32Ret);
                return ;
            }
        }
        else if(i%5==2)
        {
            printf("TDE_CreateSurfaceByFile\n");
            TDE_CreateSurfaceByFile(PIC_PEA_IMAGE_PATH2, &g_stBackGround1, pShowScreen + ((HI_U32)g_stBackGround1.u32PhyAddr - u32PhyAddr));
            stSrcRect.s32Xpos = 0;
            stSrcRect.s32Ypos = 0;
            stSrcRect.u32Width = g_stBackGround1.u32Width;
            stSrcRect.u32Height = g_stBackGround1.u32Height;

            stDstRect.s32Xpos =(IMAGE_WIDTH>>1)+2;
            stDstRect.s32Ypos = 0;
            stDstRect.u32Width = g_stBackGround1.u32Width;
            stDstRect.u32Height = g_stBackGround1.u32Height;
            s32Ret = HI_TDE2_QuickCopy(s32Handle, &g_stBackGround1, &stSrcRect, &g_stScreen_pic[0], &stDstRect);
            if(s32Ret < 0)
            {
                printf("Line:%d,HI_TDE2_QuickCopy failed,ret=0x%x!\n", __LINE__, s32Ret);
                return ;
            }
        }
        else
        if(i%5==3)
         {
            TDE_CreateSurfaceByFile(PIC_PEA_IMAGE_PATH3, &g_stBackGround2, pShowScreen + ((HI_U32)g_stBackGround2.u32PhyAddr - u32PhyAddr));
            stDstRect.s32Xpos =0;
            stDstRect.s32Ypos = (IMAGE_HEIGHT>>1)+2;
            stDstRect.u32Width = g_stBackGround2.u32Width;
            stDstRect.u32Height = g_stBackGround2.u32Height;
            s32Ret = HI_TDE2_QuickCopy(s32Handle, &g_stBackGround2, &stSrcRect, &g_stScreen_pic[0], &stDstRect);
        }*/

        {
            TDE_CreateSurfaceByFile(PIC_PEA_IMAGE_PATH6, &g_stBackGround4, pShowScreen + ((HI_U32)g_stBackGround4.u32PhyAddr - u32PhyAddr));
            stSrcRect.s32Xpos = 0;
            stSrcRect.s32Ypos = 0;
            stSrcRect.u32Width = 79;
            stSrcRect.u32Height = g_stBackGround4.u32Height;
            stDstRect.s32Xpos = 2+20;
            stDstRect.s32Ypos = 35+3;
            stDstRect.u32Width = 79;
            stDstRect.u32Height = g_stBackGround4.u32Height;
            s32Ret = HI_TDE2_QuickCopy(s32Handle, &g_stBackGround4, &stSrcRect, &g_stScreen_pic[0], &stDstRect);
        }
        //printf("g_stBackGround1.u32Width:%d,g_stBackGround1.u32Height:%d\n",g_stBackGround1.u32Width,g_stBackGround1.u32Height);
/*

*/


   /*
*/
        /* 5. submit job */
        s32Ret = HI_TDE2_EndJob(s32Handle, HI_FALSE, HI_TRUE, 10);
        if(s32Ret < 0)
        {
          //  TDE_PRINT("Line:%d,HI_TDE2_EndJob failed,ret=0x%x!\n", __LINE__, s32Ret);
            HI_TDE2_CancelJob(s32Handle);
            return ;
        }
        printf("TDE2_PIC_EndJob5!\n");

        var.yoffset = 0;
        if (ioctl(fd, FBIOPAN_DISPLAY, &var) < 0)
        {
             //TDE_PRINT("process frame buffer device error\n");
             printf("process frame buffer device error\n");
             //goto FB_PROCESS_ERROR1;
        }
        usleep(2000000);
    }
     /* 2. bitblt background to screen */
   // HI_TDE2_QuickCopy(s32Handle, &g_stBackGround, &stSrcRect, &g_stScreen_pic[0], &stSrcRect);


    /* 10.unmap the physical memory */
    munmap(pShowScreen, fix.smem_len);

    /* 11. close the framebuffer device */
    close(fd);

    HI_TDE2_Close();
    return 0;
}

int AVD_ProcessHiFbTde_PIC()
{
	HI_S32 s32Ret = 0;
    VOU_DEV_E VoDev = HD;
    int layer = 0;
    //return -1;
    sleep(10);
	/*2 run hifbTde*/
	s32Ret = AVD_RunHiFBTde_PIC(layer);
	if(s32Ret != HI_SUCCESS)
	{
		goto err;
	}

	err:
	DisableVoDev(HD);
	DisableVoDev(SD);
	MppSysExit();

	return 0;

}
