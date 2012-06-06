#include "imp_ga_canny_standard.h"
#include "math.h"

// 在把cvCanny函数进行以下修改。
// 在函数体中，当程序用两个sobel算子计算完水平和垂直两个方向的梯度强度过后加入以下代码
// // 自适应确定阈值
// if(low_thresh == -1 && high_thresh == -1)
// {
// 	AdaptiveFindThreshold(dx, dy, &low_thresh, &high_thresh);
// }
// 
// 这样，在调用cvCanny函数时，指定高低门限为－1，则cvCanny函数就自适应确定门限。
// 自适应求高低两个门限
// void ipAdaptiveFindThreshold(CvMat *dx, CvMat *dy, double *low, double *high)
// {
// 	CvSize size;
// 	IplImage *imge=0;
// 	int i,j;
// 	CvHistogram *hist;
// 	int hist_size = 255;
//     float range_0[]={0,256};
//     float* ranges[] = { range_0 };
// 	double  PercentOfPixelsNotEdges = 0.7;
// 	size = cvGetSize(dx);
// 	imge = cvCreateImage(size, IPL_DEPTH_32F, 1);
// 	// 计算边缘的强度, 并存于图像中
// 	float maxv = 0;
// 	for(i = 0; i < size.height; i++ )
// 	{
// 		const short* _dx = (short*)(dx->data.ptr + dx->step*i);
//         const short* _dy = (short*)(dy->data.ptr + dy->step*i);
// 		float* _image = (float *)(imge->imageData + imge->widthStep*i);
// 		for(j = 0; j < size.width; j++)
// 		{
// 			_image[j] = (float)(abs(_dx[j]) + abs(_dy[j]));
// 			maxv = maxv < _image[j] ? _image[j]: maxv;
// 		}
// 	}
// 	
// 	// 计算直方图
// 	range_0[1] = maxv;
// 	hist_size = (int)(hist_size > maxv ? maxv:hist_size);
// 	hist = cvCreateHist(1, &hist_size, CV_HIST_ARRAY, ranges, 1);
// 	cvCalcHist( &imge, hist, 0, NULL );
// 	int total = (int)(size.height * size.width * PercentOfPixelsNotEdges);
// 	float sum=0;
// 	int icount = hist->mat.dim[0].size;
// 	
// 	float *h = (float*)cvPtr1D( hist->bins, 0 );
// 	for(i = 0; i < icount; i++)
// 	{
// 		sum += h[i];
// 		if( sum > total )
// 			break; 
// 	}
// 	// 计算高低门限
// 	*high = (i+1) * maxv / hist_size ;
// 	*low = *high * 0.4;
// 	cvReleaseImage( &imge );
// 	cvReleaseHist(&hist);
// }


/*******************************************************************************
* PROGRAM: canny_edge
* PURPOSE: This program implements a "Canny" edge detector. The processing
* steps are as follows:
*
*   1) Convolve the image with a separable gaussian filter.
*   2) Take the dx and dy the first derivatives using [-1,0,1] and [1,0,-1]'.
*   3) Compute the magnitude: sqrt(dx*dx+dy*dy).
*   4) Perform non-maximal suppression.
*   5) Perform hysteresis.
*
* The user must input three parameters. These are as follows:
*
*   sigma = The standard deviation of the gaussian smoothing filter.
*   tlow  = Specifies the low value to use in hysteresis. This is a 
*           fraction (0-1) of the computed high threshold edge strength value.
*   thigh = Specifies the high value to use in hysteresis. This fraction (0-1)
*           specifies the percentage point in a histogram of the gradient of
*           the magnitude. Magnitude values of zero are not counted in the
*           histogram.
*
* NAME: Mike Heath
*       Computer Vision Laboratory
*       University of South Floeida
*       heath@csee.usf.edu
*
* DATE: 2/15/96
*
* Modified: 5/17/96 - To write out a floating point RAW headerless file of
*                     the edge gradient "up the edge" where the angle is
*                     defined in radians counterclockwise from the x direction.
*
* COMPILE: Three 'C' source code files are needed to compile and run this edge
*          detection: "canny_edge.c", "hysteresis.c" and "pgm_io.c". They should
*          all be available in the same directory. To compile, type:
*                gcc -o canny_edge canny_edge.c hysteresis.c pgm_io.c -lm
*                (Note: You can also use optimization such as -O3)
* USAGE: canny_edge image_filename sigma tlow thigh
*        where tlow < thigh are both fractions between 0.0 and 1.0.
*        Example: canny_edge panda.pgm 2 0.5 0.7
*        The resulting edge image is "panda.pgm_s_2.00_l_0.50_h_0.70.pgm"
*******************************************************************************/


// main(int argc, char *argv[])
// {
//    char *infilename = NULL;  /* Name of the input image */
//    char *dirfilename = NULL; /* Name of the output gradient direction image */
//    char outfilename[128];    /* Name of the output "edge" image */
//    char composedfname[128];  /* Name of the output "direction" image */
//    unsigned char *image;     /* The input image */
//    unsigned char *edge;      /* The output edge image */
//    int rows, cols;           /* The dimensions of the image. */
//    float sigma,              /* Standard deviation of the gaussian kernel. */
// 	 tlow,               /* Fraction of the high threshold in hysteresis. */
// 	 thigh;              /* High hysteresis threshold control. The actual
// 			        threshold is the (100 * thigh) percentage point
// 			        in the histogram of the magnitude of the
// 			        gradient image that passes non-maximal
// 			        suppression. */
// 
//    /****************************************************************************
//    * Get the command line arguments.
//    ****************************************************************************/
//    if(argc < 5){
//    fprintf(stderr,"\n<USAGE> %s image sigma tlow thigh [writedirim]\n",argv[0]);
//       fprintf(stderr,"\n      image:      An image to process. Must be in ");
//       fprintf(stderr,"PGM format.\n");
//       fprintf(stderr,"      sigma:      Standard deviation of the gaussian");
//       fprintf(stderr," blur kernel.\n");
//       fprintf(stderr,"      tlow:       Fraction (0.0-1.0) of the high ");
//       fprintf(stderr,"edge strength threshold.\n");
//       fprintf(stderr,"      thigh:      Fraction (0.0-1.0) of the distribution");
//       fprintf(stderr," of non-zero edge\n                  strengths for ");
//       fprintf(stderr,"hysteresis. The fraction is used to compute\n");
//       fprintf(stderr,"                  the high edge strength threshold.\n");
//       fprintf(stderr,"      writedirim: Optional argument to output ");
//       fprintf(stderr,"a floating point");
//       fprintf(stderr," direction image.\n\n");
//       exit(1);
//    }
// 
//    infilename = argv[1];
//    sigma = atof(argv[2]);
//    tlow = atof(argv[3]);
//    thigh = atof(argv[4]);
// 
//    if(argc == 6) dirfilename = infilename;
//    else dirfilename = NULL;
// 
//    /****************************************************************************
//    * Read in the image. This read function allocates memory for the image.
//    ****************************************************************************/
//    if (VERBOSE) printf("Reading the image %s.\n", infilename);
//    if(read_pgm_image(infilename, &image, &rows, &cols) == 0){
//       fprintf(stderr, "Error reading the input image, %s.\n", infilename);
//       exit(1);
//    }
// 
//    /****************************************************************************
//    * Perform the edge detection. All of the work takes place here.
//    ****************************************************************************/
//    if (VERBOSE) printf("Starting Canny edge detection.\n");
//    if(dirfilename != NULL){
//       sprintf(composedfname, "%s_s_%3.2f_l_%3.2f_h_%3.2f.fim", infilename,
//       sigma, tlow, thigh);
//       dirfilename = composedfname;
//    }
//    canny(image, rows, cols, sigma, tlow, thigh, &edge, dirfilename);
// 
//    /****************************************************************************
//    * Write out the edge image to a file.
//    ****************************************************************************/
//    sprintf(outfilename, "%s_s_%3.2f_l_%3.2f_h_%3.2f.pgm", infilename,
//       sigma, tlow, thigh);
//    if (VERBOSE) printf("Writing the edge iname in the file %s.\n", outfilename);
//    if(write_pgm_image(outfilename, edge, rows, cols, "", 255) == 0){
//       fprintf(stderr, "Error writing the edge image, %s.\n", outfilename);
//       exit(1);
//    }
// }

/**
* 四步法的Canny边缘提取   
* @param image 图像指针   
* @param rows 图像高度
* @param cols 图像宽度
* @param sigma 高斯核的方差（一般情况：0-3之间） 
* @param tlow 低阈值 
* @param thigh 高阈值
* @param edge 二值化后的边缘图像
* @param gradDir 梯度方向图像
*/
void ipCannyOriginal(unsigned char *image, int rows, int cols, float sigma,
         float tlow, float thigh, unsigned char *edge, float * gradDir)
{
   unsigned char *nms;        /* Points that are local maximal magnitude. */
   short int *smoothedim,     /* The image after gaussian smoothing.      */
             *delta_x,        /* The first devivative image, x-direction. */
             *delta_y,        /* The first derivative image, y-direction. */
             *magnitude;      /* The magnitude of the gadient image.      */
   float *dir_radians=NULL;   /* Gradient direction image.                */

   /****************************************************************************
   * Perform gaussian smoothing on the image using the input standard
   * deviation.
   ****************************************************************************/
   if (VERBOSE) printf("Smoothing the image using a gaussian kernel.\n");
   gaussian_smooth(image, rows, cols, sigma, &smoothedim);

   /****************************************************************************
   * Compute the first derivative in the x and y directions.
   ****************************************************************************/
   if (VERBOSE) printf("Computing the X and Y first derivatives.\n");
   derrivative_x_y(smoothedim, rows, cols, &delta_x, &delta_y);

   /****************************************************************************
   * This option to write out the direction of the edge gradient was added
   * to make the information available for computing an edge quality figure
   * of merit.
   ****************************************************************************/
   {
      /*************************************************************************
      * Compute the direction up the gradient, in radians that are
      * specified counteclockwise from the positive x-axis.
      *************************************************************************/
      radian_direction(delta_x, delta_y, rows, cols, &dir_radians, -1, -1);

      /*************************************************************************
      * Write the gradient direction image out to a file.
      *************************************************************************/
	  memcpy(gradDir, dir_radians, sizeof(float)*rows*cols);

      free(dir_radians);
   }

   /****************************************************************************
   * Compute the magnitude of the gradient.
   ****************************************************************************/
   if (VERBOSE) printf("Computing the magnitude of the gradient.\n");
   magnitude_x_y(delta_x, delta_y, rows, cols, &magnitude);

   /****************************************************************************
   * Perform non-maximal suppression.
   ****************************************************************************/
   if (VERBOSE) printf("Doing the non-maximal suppression.\n");
   if((nms = (unsigned char *) calloc(rows*cols,sizeof(unsigned char)))==NULL){
      fprintf(stderr, "Error allocating the nms image.\n");
      exit(1);
   }

   non_max_supp(magnitude, delta_x, delta_y, rows, cols, nms);

   /****************************************************************************
   * Use hysteresis to mark the edge pixels.
   ****************************************************************************/
   apply_hysteresis(magnitude, nms, rows, cols, tlow, thigh, edge);

   /****************************************************************************
   * Free all of the memory that we allocated except for the edge image that
   * is still being used to store out result.
   ****************************************************************************/
   free(smoothedim);
   free(delta_x);
   free(delta_y);
   free(magnitude);
   free(nms);
}

/*******************************************************************************
* Procedure: radian_direction
* Purpose: To compute a direction of the gradient image from component dx and
* dy images. Because not all derriviatives are computed in the same way, this
* code allows for dx or dy to have been calculated in different ways.
*
* FOR X:  xdirtag = -1  for  [-1 0  1]
*         xdirtag =  1  for  [ 1 0 -1]
*
* FOR Y:  ydirtag = -1  for  [-1 0  1]'
*         ydirtag =  1  for  [ 1 0 -1]'
*
* The resulting angle is in radians measured counterclockwise from the
* xdirection. The angle points "up the gradient".
*******************************************************************************/
void radian_direction(short int *delta_x, short int *delta_y, int rows,
    int cols, float **dir_radians, int xdirtag, int ydirtag)
{
   int r, c, pos;
   float *dirim=NULL;
   double dx, dy;

   /****************************************************************************
   * Allocate an image to store the direction of the gradient.
   ****************************************************************************/
   if((dirim = (float *) calloc(rows*cols, sizeof(float))) == NULL){
      fprintf(stderr, "Error allocating the gradient direction image.\n");
      exit(1);
   }
   *dir_radians = dirim;

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
         dx = (double)delta_x[pos];
         dy = (double)delta_y[pos];

         if(xdirtag == 1) dx = -dx;
         if(ydirtag == -1) dy = -dy;

         dirim[pos] = (float)angle_radians(dx, dy);
      }
   }
}

/*******************************************************************************
* FUNCTION: angle_radians
* PURPOSE: This procedure computes the angle of a vector with components x and
* y. It returns this angle in radians with the answer being in the range
* 0 <= angle <2*PI.
*******************************************************************************/
double angle_radians(double x, double y)
{
   double xu, yu, ang;

   xu = fabs(x);
   yu = fabs(y);

   if((xu == 0) && (yu == 0)) return(0);

   ang = atan(yu/xu);

   if(x >= 0){
      if(y >= 0) return(ang);
      else return(2*IMP_PI - ang);
   }
   else{
      if(y >= 0) return(IMP_PI - ang);
      else return(IMP_PI + ang);
   }
}

/*******************************************************************************
* PROCEDURE: magnitude_x_y
* PURPOSE: Compute the magnitude of the gradient. This is the square root of
* the sum of the squared derivative values.
*******************************************************************************/
void magnitude_x_y(short int *delta_x, short int *delta_y, int rows, int cols,
        short int **magnitude)
{
   int r, c, pos, sq1, sq2;

   /****************************************************************************
   * Allocate an image to store the magnitude of the gradient.
   ****************************************************************************/
   if((*magnitude = (short *) calloc(rows*cols, sizeof(short))) == NULL){
      fprintf(stderr, "Error allocating the magnitude image.\n");
      exit(1);
   }

   for(r=0,pos=0;r<rows;r++){
      for(c=0;c<cols;c++,pos++){
         sq1 = (int)delta_x[pos] * (int)delta_x[pos];
         sq2 = (int)delta_y[pos] * (int)delta_y[pos];
         (*magnitude)[pos] = (short)(0.5 + sqrt((float)sq1 + (float)sq2));
      }
   }

}

/*******************************************************************************
* PROCEDURE: derrivative_x_y
* PURPOSE: Compute the first derivative of the image in both the x any y
* directions. The differential filters that are used are:
*
*                                          -1
*         dx =  -1 0 +1     and       dy =  0
*                                          +1
*
*******************************************************************************/
void derrivative_x_y(short int *smoothedim, int rows, int cols,
        short int **delta_x, short int **delta_y)
{
   int r, c, pos;

   /****************************************************************************
   * Allocate images to store the derivatives.
   ****************************************************************************/
   if(((*delta_x) = (short *) calloc(rows*cols, sizeof(short))) == NULL){
      fprintf(stderr, "Error allocating the delta_x image.\n");
      exit(1);
   }
   if(((*delta_y) = (short *) calloc(rows*cols, sizeof(short))) == NULL){
      fprintf(stderr, "Error allocating the delta_x image.\n");
      exit(1);
   }

   /****************************************************************************
   * Compute the x-derivative. Adjust the derivative at the borders to avoid
   * losing pixels.
   ****************************************************************************/
   if (VERBOSE) printf("   Computing the X-direction derivative.\n");
   for(r=0;r<rows;r++){
      pos = r * cols;
      (*delta_x)[pos] = smoothedim[pos+1] - smoothedim[pos];
      pos++;
      for(c=1;c<(cols-1);c++,pos++){
         (*delta_x)[pos] = smoothedim[pos+1] - smoothedim[pos-1];
      }
      (*delta_x)[pos] = smoothedim[pos] - smoothedim[pos-1];
   }

   /****************************************************************************
   * Compute the y-derivative. Adjust the derivative at the borders to avoid
   * losing pixels.
   ****************************************************************************/
   if (VERBOSE) printf("   Computing the Y-direction derivative.\n");
   for(c=0;c<cols;c++){
      pos = c;
      (*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos];
      pos += cols;
      for(r=1;r<(rows-1);r++,pos+=cols){
         (*delta_y)[pos] = smoothedim[pos+cols] - smoothedim[pos-cols];
      }
      (*delta_y)[pos] = smoothedim[pos] - smoothedim[pos-cols];
   }
}

/*******************************************************************************
* PROCEDURE: gaussian_smooth
* PURPOSE: Blur an image with a gaussian filter.
*******************************************************************************/
void gaussian_smooth(unsigned char *image, int rows, int cols, float sigma,
        short int **smoothedim)
{
   int r, c, rr, cc,     /* Counter variables. */
      windowsize,        /* Dimension of the gaussian kernel. */
      center;            /* Half of the windowsize. */
   float *tempim,        /* Buffer for separable filter gaussian smoothing. */
         *kernel,        /* A one dimensional gaussian kernel. */
         dot,            /* Dot product summing variable. */
         sum;            /* Sum of the kernel weights variable. */

   /****************************************************************************
   * Create a 1-dimensional gaussian smoothing kernel.
   ****************************************************************************/
   if (VERBOSE) printf("   Computing the gaussian smoothing kernel.\n");
   make_gaussian_kernel(sigma, &kernel, &windowsize);
   center = windowsize / 2;

   /****************************************************************************
   * Allocate a temporary buffer image and the smoothed image.
   ****************************************************************************/
   if((tempim = (float *) calloc(rows*cols, sizeof(float))) == NULL){
      fprintf(stderr, "Error allocating the buffer image.\n");
      exit(1);
   }
   if(((*smoothedim) = (short int *) calloc(rows*cols,
         sizeof(short int))) == NULL){
      fprintf(stderr, "Error allocating the smoothed image.\n");
      exit(1);
   }

   /****************************************************************************
   * Blur in the x - direction.
   ****************************************************************************/
   if (VERBOSE) printf("   Bluring the image in the X-direction.\n");
   for(r=0;r<rows;r++){
      for(c=0;c<cols;c++){
         dot = 0.0;
         sum = 0.0;
         for(cc=(-center);cc<=center;cc++){
            if(((c+cc) >= 0) && ((c+cc) < cols)){
               dot += (float)image[r*cols+(c+cc)] * kernel[center+cc];
               sum += kernel[center+cc];
            }
         }
         tempim[r*cols+c] = dot/sum;
      }
   }

   /****************************************************************************
   * Blur in the y - direction.
   ****************************************************************************/
   if (VERBOSE) printf("   Bluring the image in the Y-direction.\n");
   for(c=0;c<cols;c++){
      for(r=0;r<rows;r++){
         sum = 0.0;
         dot = 0.0;
         for(rr=(-center);rr<=center;rr++){
            if(((r+rr) >= 0) && ((r+rr) < rows)){
               dot += tempim[(r+rr)*cols+c] * kernel[center+rr];
               sum += kernel[center+rr];
            }
         }
         (*smoothedim)[r*cols+c] = (short int)(dot*BOOSTBLURFACTOR/sum + 0.5);
      }
   }

   free(tempim);
   free(kernel);
}

/*******************************************************************************
* PROCEDURE: make_gaussian_kernel
* PURPOSE: Create a one dimensional gaussian kernel.
*******************************************************************************/
void make_gaussian_kernel(float sigma, float **kernel, int *windowsize)
{
   int i, center;
   float x, fx, sum=0.0;

   *windowsize =(int)( 1 + 2 * ceil(2.5 * sigma));
   center = (*windowsize) / 2;

   if (VERBOSE) printf("      The kernel has %d elements.\n", *windowsize);
   if((*kernel = (float *) calloc((*windowsize), sizeof(float))) == NULL){
      fprintf(stderr, "Error callocing the gaussian kernel array.\n");
      exit(1);
   }

   for(i=0;i<(*windowsize);i++){
      x = (float)(i - center);
      fx = (float)(pow(2.71828, -0.5*x*x/(sigma*sigma)) / (sigma * sqrt(6.2831853)));
      (*kernel)[i] = fx;
      sum += fx;
   }

   for(i=0;i<(*windowsize);i++) (*kernel)[i] /= sum;

   if (VERBOSE){
      printf("The filter coefficients are:\n");
      for(i=0;i<(*windowsize);i++)
         printf("kernel[%d] = %f\n", i, (*kernel)[i]);
   }
}

