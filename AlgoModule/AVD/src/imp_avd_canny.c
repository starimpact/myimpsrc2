
#include "imp_avd_canny.h"

#define NOEDGE			0
#define EDGE			255
#define POSSIBLE_EDGE	128
#define TH_GRADIENT		10
#define EDGE_MIN_ROWS	4
#define EDGE_MIN_COLS	4

#define  BIT8 (0x80)
#define _BIT8 (~0x80)

#define  BIT7 (0x40)
#define _BIT7 (~0x40)

#define  BIT6 (0x20)
#define _BIT6 (~0x20)

#define  BIT5 (0x10)
#define _BIT5 (~0x10)

#define  BIT4 (0x08)
#define _BIT4 (~0x08)

#define  BIT3 (0x04)
#define _BIT3 (~0x04)

#define  BIT2 (0x02)
#define _BIT2 (~0x02)

#define  BIT1 (0x01)
#define _BIT1 (~0x01)


static void round_parameters( IMP_S32 *pi_row, IMP_S32 *pi_col,
							 IMP_S32 *pn_rows, IMP_S32 *pn_cols,
							 IMP_S32 n_img_rows, IMP_S32 n_img_cols )
{
	IMP_S32 i_row = *pi_row;
	IMP_S32 i_col = *pi_col;
	IMP_S32 n_rows = *pn_rows;
	IMP_S32 n_cols = *pn_cols;

	if( n_rows&1 )
		n_rows += 1;
	if( n_cols&1 )
		n_cols += 1;

	if( n_rows < EDGE_MIN_ROWS )
		n_rows = EDGE_MIN_ROWS;
	if( i_row > n_img_rows-n_rows )
		i_row = n_img_rows-n_rows;

	if( n_cols < EDGE_MIN_COLS )
		n_cols = EDGE_MIN_COLS;
	if( i_col > n_img_cols-n_cols )
		i_col = n_img_cols-n_cols;

	i_row -= 2; i_col -= 2;
	n_rows += 4; n_cols += 4;

	if( i_row < 0 )
		i_row = 0;
	if( n_rows > n_img_rows-i_row )
	{
		n_rows = n_img_rows-i_row;
		if( n_rows&1 )
			n_rows -= 1;
	}

	if( i_col < 0 )
		i_col = 0;
	if( n_cols > n_img_cols-i_col )
	{
		n_cols = n_img_cols-i_col;
		if( n_cols&1 )
			n_cols -= 1;
	}

	*pi_row = i_row;
	*pi_col = i_col;
	*pn_rows = n_rows;
	*pn_cols = n_cols;
}

static void canny_calc_gradmag_i( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag,
								IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
								IMP_S32 n_img_cols, IMP_S32 n_img_rows )
{
	IMP_S32 icol, irow;
	IMP_U8 *gray;
	IMP_S8 *gradxy;
	IMP_U8 *mag;
	IMP_S32 val1, val2, val3;
	IMP_S32 g1, g2, g3, g4;
	IMP_U8 *gray_2;
	IMP_S8 *gradxy_2;
	IMP_U8 *mag_2;
	IMP_S32 val1_2, val2_2, val3_2;
	IMP_S32 g1_2, g2_2, g3_2, g4_2;
	IMP_S32 n_stride = n_img_cols;

	p_img_gray += n_img_cols*i_row+i_col;
	p_grad_xy += (n_img_cols*i_row+i_col)*2;
	p_img_mag += n_img_cols*i_row+i_col;

	for( irow=1; irow<n_rows-1; irow++ )
	{
		gray = p_img_gray + n_stride*irow+1;
 		gradxy = p_grad_xy + (n_stride*irow+1)*2;
		mag = p_img_mag + n_stride*irow+1;
		gray_2 = p_img_gray + n_stride*irow+2;
 		gradxy_2 = p_grad_xy + (n_stride*irow+2)*2;
		mag_2 = p_img_mag + n_stride*irow+2;

		for( icol=1; icol<n_cols-1; icol+=2 )
		{
			g1 = gray[-1];
			g2 = gray[1];
			g3 = gray[-n_stride];
			g4 = gray[n_stride];
			g1_2 = gray_2[-1];
			g2_2 = gray_2[1];
			g3_2 = gray_2[-n_stride];
			g4_2 = gray_2[n_stride];

			val1 = ( g1 - g2 ) >> 1;
			val2 = ( g3 - g4 ) >> 1;
			val1_2 = ( g1_2 - g2_2 ) >> 1;
			val2_2 = ( g3_2 - g4_2 ) >> 1;

			gradxy[0] = val1;
			gradxy[1] = val2;
			gradxy_2[0] = val1_2;
			gradxy_2[1] = val2_2;

			val3 = ( abs(val1) + abs(val2) );
			mag[0] = val3;
			val3_2 = ( abs(val1_2) + abs(val2_2) );
			mag_2[0] = val3_2;

			gray+=2;
			gradxy+=4;
			mag+=2;
			gray_2+=2;
			gradxy_2+=4;
			mag_2+=2;
		}
	}
}

static void canny_calc_gradient_i( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy,
								IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
								IMP_S32 n_img_cols, IMP_S32 n_img_rows )
{
	IMP_S32 icol, irow;
	IMP_U8 *gray;
	IMP_S8 *gradxy;
	IMP_S32 val1, val2;
	IMP_S32 g1, g2, g3, g4;
	IMP_U8 *gray_2;
	IMP_S8 *gradxy_2;
	IMP_S32 val1_2, val2_2;
	IMP_S32 g1_2, g2_2, g3_2, g4_2;
	IMP_S32 n_stride = n_img_cols;

	p_img_gray += n_img_cols*i_row+i_col;
	p_grad_xy += (n_img_cols*i_row+i_col)*2;

	for( irow=1; irow<n_rows-1; irow++ )
	{
		gray = p_img_gray + n_stride*irow+1;
 		gradxy = p_grad_xy + (n_stride*irow+1)*2;
		gray_2 = p_img_gray + n_stride*irow+2;
 		gradxy_2 = p_grad_xy + (n_stride*irow+2)*2;

		for( icol=1; icol<n_cols-1; icol+=2 )
		{
			g1 = gray[-1];
			g2 = gray[1];
			g3 = gray[-n_stride];
			g4 = gray[n_stride];
			g1_2 = gray_2[-1];
			g2_2 = gray_2[1];
			g3_2 = gray_2[-n_stride];
			g4_2 = gray_2[n_stride];

			val1 = ( g1 - g2 ) >> 1;
			val2 = ( g3 - g4 ) >> 1;
			val1_2 = ( g1_2 - g2_2 ) >> 1;
			val2_2 = ( g3_2 - g4_2 ) >> 1;

			gradxy[0] = val1;
			gradxy[1] = val2;
			gradxy_2[0] = val1_2;
			gradxy_2[1] = val2_2;

			gray+=2;
			gradxy+=4;
			gray_2+=2;
			gradxy_2+=4;
		}
	}
}

static void canny_calc_magnitude_i( IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag,
								IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
								IMP_S32 n_img_cols, IMP_S32 n_img_rows )
{
	IMP_S32 icol, irow;
	IMP_S8 *gradxy;
	IMP_U8 *mag;
	IMP_S32 val1, val2, val3;
	IMP_S8 *gradxy_2;
	IMP_U8 *mag_2;
	IMP_S32 val1_2, val2_2, val3_2;
	IMP_S32 n_stride = n_img_cols;

	p_grad_xy += (n_img_cols*i_row+i_col)*2;
	p_img_mag += n_img_cols*i_row+i_col;

	for( irow=1; irow<n_rows-1; irow++ )
	{
 		gradxy = p_grad_xy + (n_stride*irow+1)*2;
		mag = p_img_mag + n_stride*irow+1;
 		gradxy_2 = p_grad_xy + (n_stride*irow+2)*2;
		mag_2 = p_img_mag + n_stride*irow+2;

		for( icol=1; icol<n_cols-1; icol+=2 )
		{
			val1 = gradxy[0];
			val2 = gradxy[1];
			val1_2 = gradxy_2[0];
			val2_2 = gradxy_2[1];

			val3 = ( abs(val1) + abs(val2) );
			mag[0] = val3;
			val3_2 = ( abs(val1_2) + abs(val2_2) );
			mag_2[0] = val3_2;

			gradxy+=4;
			mag+=2;
			gradxy_2+=4;
			mag_2+=2;
		}
	}
}

static void canny_non_max_supp_i( IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag, IMP_U8 *p_img_edge,
								IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
								IMP_S32 n_img_cols, IMP_S32 n_img_rows, IMP_S32 tgrad )
{
    IMP_S32 row_count, col_count, count;
    IMP_U8 *p_mag_row;
    IMP_S8 *p_gxy_row;
    IMP_U8 *p_tmp_mag;
    IMP_U8 *p_result_row;
    IMP_S8 *p_gxy;
    IMP_U8 *p_tmp_result;
	IMP_S32 z11, z12, z21, z22;
    IMP_S32 m00, gx, gy;
    IMP_S32 nMag1,nMag2;
    IMP_S32 nXperp,nYperp;
	IMP_U32 gxa, gya;
	IMP_S32 signx, signy;
	IMP_S32 sy1, sx1, sy2, sx2;
    IMP_U8 *p_tmp_mag_2;
    IMP_S8 *p_gxy_2;
    IMP_U8 *p_tmp_result_2;
	IMP_S32 z11_2, z12_2, z21_2, z22_2;
    IMP_S32 m00_2, gx_2, gy_2;
    IMP_S32 nMag1_2,nMag2_2;
    IMP_S32 nXperp_2,nYperp_2;
	IMP_U32 gxa_2, gya_2;
	IMP_S32 signx_2, signy_2;
	IMP_S32 sy1_2, sx1_2, sy2_2, sx2_2;
	IMP_S32 m00_t;
	IMP_S32 n_stride = n_img_cols;
	IMP_U8 *p_result;

	p_grad_xy += (n_img_cols*i_row+i_col)*2;
	p_img_mag += n_img_cols*i_row+i_col;
	p_img_edge += n_img_cols*i_row+i_col;
	p_result = p_img_edge;

	// Zero the edges of the result image.
    for (count = 0, p_result_row = p_result, p_tmp_result = p_result + n_stride * (n_rows - 1);
        count < n_cols; p_tmp_result++, p_result_row++, count++)
	{
        p_result_row[0] = 0;
        p_result_row[n_stride] = 0;
		p_tmp_result[0] = 0;
		p_tmp_result[-n_stride] = 0;
	}

    for (count = 0, p_tmp_result = p_result, p_result_row = p_result + n_cols - 1;
        count < n_rows; count++, p_tmp_result += n_stride, p_result_row += n_stride)
	{
        p_tmp_result[0] = 0;
        p_tmp_result[1] = 0;
		p_result_row[-1] = 0;
		p_result_row[0] = 0;
	}

	// Suppress non-maximum points.
	for( row_count = 2, p_mag_row = p_img_mag + (n_stride*2+2), p_gxy_row = p_grad_xy + (n_stride*2+2)*2,
		p_result_row = p_result + (n_stride*2+2);
        row_count < n_rows - 2;
        row_count++, p_mag_row += n_stride, p_gxy_row += n_stride*2,
        p_result_row += n_stride )
	{
		for( col_count = 2,
			p_tmp_mag = p_mag_row, p_gxy = p_gxy_row, p_tmp_result = p_result_row,
			p_tmp_mag_2 = p_mag_row+1, p_gxy_2 = p_gxy_row+2, p_tmp_result_2 = p_result_row+1;
			col_count < n_cols - 2;
			col_count+=2,
			p_tmp_mag+=2, p_gxy+=4, p_tmp_result+=2,
			p_tmp_mag_2+=2, p_gxy_2+=4, p_tmp_result_2+=2
			)
		{
			gx = *p_gxy;	gy = *(p_gxy+1);
			gx_2 = *p_gxy_2;	gy_2 = *(p_gxy_2+1);

			m00 = *( p_tmp_mag - 0 - 0 );
			m00_2 = *( p_tmp_mag_2 - 0 - 0 );

			gxa = abs(gx);	gya = abs(gy);
			signx = gx>=0 ? 1 : -1;
			signy = gy>=0 ? 1 : -1;
			sx2 = 1*signx;
			sy2 = n_stride*signy;
			if( gxa>=gya )
			{
				nXperp = -gx*signx;
				nYperp = gy*signy;
				sx1 = 1*signx;
				sy1 = 0*signy;
			}
			else
			{
				nYperp = gx*signx;
				nXperp = -gy*signy;
				sx1 = 0*signx;
				sy1 = n_stride*signy;
			}

	        z11 = *( p_tmp_mag - sy1 - sx1 );
	        z12 = *( p_tmp_mag - sy2 - sx2 );
	        z21 = *( p_tmp_mag + sy1 + sx1 );
	        z22 = *( p_tmp_mag + sy2 + sx2 );

			gxa_2 = abs(gx_2);	gya_2 = abs(gy_2);
			signx_2 = gx_2>=0 ? 1 : -1;
			signy_2 = gy_2>=0 ? 1 : -1;
			sx2_2 = 1*signx_2;
			sy2_2 = n_stride*signy_2;
			if( gxa_2>=gya_2 )
			{
				nXperp_2 = -gx_2*signx_2;
				nYperp_2 = gy_2*signy_2;
				sx1_2 = 1*signx_2;
				sy1_2 = 0*signy_2;
			}
			else
			{
				nYperp_2 = gx_2*signx_2;
				nXperp_2 = -gy_2*signy_2;
				sx1_2 = 0*signx_2;
				sy1_2 = n_stride*signy_2;
			}

	        z11_2 = *( p_tmp_mag_2 - sy1_2 - sx1_2 );
	        z12_2 = *( p_tmp_mag_2 - sy2_2 - sx2_2 );
	        z21_2 = *( p_tmp_mag_2 + sy1_2 + sx1_2 );
	        z22_2 = *( p_tmp_mag_2 + sy2_2 + sx2_2 );

			m00_t = ( m00 & 0xFE );
			nMag1 = (m00 - z11) * nXperp + (z12 - z11) * nYperp;
			nMag2 = (m00 - z21) * nXperp + (z22 - z21) * nYperp;
			*p_tmp_result = ( m00<=tgrad || nMag1>=0 || nMag2>=0 ) ? NOEDGE : m00_t;

			m00_t = ( m00_2 & 0xFE );
			nMag1_2 = (m00_2 - z11_2) * nXperp_2 + (z12_2 - z11_2) * nYperp_2;
			nMag2_2 = (m00_2 - z21_2) * nXperp_2 + (z22_2 - z21_2) * nYperp_2;
			*p_tmp_result_2 = ( m00_2<=tgrad || nMag1_2>=0 || nMag2_2>=0 ) ? NOEDGE : m00_t;
        }
	}
}

void avd_canny_calc_gradmag( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows )
{
	round_parameters( &i_row, &i_col, &n_rows, &n_cols, n_img_rows, n_img_cols );
	canny_calc_gradmag_i( p_img_gray, p_grad_xy, p_img_mag, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows );
}

void avd_canny_calc_gradient( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows )
{
	round_parameters( &i_row, &i_col, &n_rows, &n_cols, n_img_rows, n_img_cols );
	canny_calc_gradient_i( p_img_gray, p_grad_xy, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows );
}

void avd_canny_calc_magnitude( IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows )
{
	round_parameters( &i_row, &i_col, &n_rows, &n_cols, n_img_rows, n_img_cols );
	canny_calc_magnitude_i( p_grad_xy, p_img_mag, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows );
}

void avd_canny_non_max_supp( IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag, IMP_U8 *p_img_edge,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows, IMP_S32 tgrad )
{
	round_parameters( &i_row, &i_col, &n_rows, &n_cols, n_img_rows, n_img_cols );
	canny_non_max_supp_i( p_grad_xy, p_img_mag, p_img_edge, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows, tgrad );
}


/*******************************************************************************
follow_edges
*******************************************************************************/
static void follow_edges( IMP_U8 *p_edge_map,
						 IMP_S16 *p_edge_mag,
						 IMP_S16 low_val, IMP_S32 n_cols )
{
   IMP_S16 *p_tmp_mag;
   IMP_U8 *p_tmp_map;
   IMP_S32 i;
   IMP_S32 x[8] = {1, 1, 0, -1, -1, -1, 0, 1},
		   y[8] = {0, 1, 1, 1, 0, -1, -1, -1};

   for (i = 0; i < 8; i++)
   {
      p_tmp_map = p_edge_map - y[i] * n_cols + x[i];
      p_tmp_mag = p_edge_mag - y[i] * n_cols + x[i];

      if ((*p_tmp_map == POSSIBLE_EDGE) && (*p_tmp_mag > low_val))
	  {
         *p_tmp_map = (IMP_U8)EDGE;
         follow_edges(p_tmp_map, p_tmp_mag, low_val, n_cols);
      }
   }
}

/*******************************************************************************
apply_hysteresis
*******************************************************************************/
static void apply_hysteresis( IMP_S16 *p_mag,
							 IMP_S32 n_cols, IMP_S32 n_rows,
							 IMP_S32 tlow, IMP_S32 thigh,
							 IMP_U8 *p_edge )
{
   IMP_S32 r, c, pos, low_thres, high_thres;


   /****************************************************************************
   * Initialize the edge map to possible edges everywhere the non-maximal
   * suppression suggested there could be an edge except for the border. At
   * the border we say there can not be an edge because it makes the
   * follow_edges algorithm more efficient to not worry about tracking an
   * edge off the side of the image.
   ****************************************************************************/
//   for (r = 0, pos = 0; r < n_rows; r++)
//   {
//	  for (c = 0; c < n_cols; c++, pos++)
//	  {
//		  if (p_nms[pos] == POSSIBLE_EDGE)
//			  p_edge[pos] = POSSIBLE_EDGE;
//	      else
//			  p_edge[pos] = NOEDGE;
//      }
//   }

   for (r = 0, pos = 0; r < n_rows; r++, pos += n_cols)
   {
      p_edge[pos] = NOEDGE;
      p_edge[pos + n_cols - 1] = NOEDGE;
   }

   pos = (n_rows - 1) * n_cols;
   for (c = 0; c < n_cols; c++, pos++)
   {
      p_edge[c] = NOEDGE;
      p_edge[pos] = NOEDGE;
   }

   /****************************************************************************
   * Compute the histogram of the magnitude image. Then use the histogram to
   * compute hysteresis thresholds.
   ****************************************************************************/
//   memset( hist, 0, 32768*sizeof(IMP_S32) );
//
//   for (r = 0, pos = 0; r < n_rows; r++)
//   {
//	  for (c = 0; c < n_cols; c++, pos++)
//	  {
//	     if (p_edge[pos] == POSSIBLE_EDGE)
//			++hist[p_mag[pos]];
//      }
//   }

   /****************************************************************************
   * Compute the number of pixels that passed the nonmaximal suppression.
   ****************************************************************************/
//   for (r = 1, n_edges = 0; r < 32768; r++)
//   {
//      if (hist[r] != 0)
//	  {
//		  max_mag = r;
//		  n_edges += hist[r];
//	  }
//   }
//
//   high_count = (IMP_S32)(n_edges * thigh + 0.5);

   /****************************************************************************
   * Compute the high threshold value as the (100 * thigh) percentage point
   * in the magnitude of the gradient histogram of all the pixels that passes
   * non-maximal suppression. Then calculate the low threshold as a fraction
   * of the computed high threshold value. John Canny said in his paper
   * "A Computational Approach to Edge Detection" that "The ratio of the
   * high to low threshold in the implementation is in the range two or three
   * to one." That means that in terms of this implementation, we should
   * choose tlow ~= 0.5 or 0.33333.
   ****************************************************************************/
//   r = 0;
//   n_edges = 0;
//   while( n_edges<=high_count && r<=max_mag )
//   {
//      n_edges += hist[++r];
//   }
//
//   high_thres = r;
//   low_thres = (IMP_S32)(high_thres * tlow + 0.5);

   /****************************************************************************
   * This loop looks for pixels above the high_thres to locate edges and
   * then calls follow_edges to continue the edge.
   ****************************************************************************/
   high_thres = thigh;
   low_thres = tlow;
   for (r = 0, pos = 0; r < n_rows; r++)
   {
	   for (c = 0; c < n_cols; c++, pos++)
	   {
		   if ((p_edge[pos] == POSSIBLE_EDGE) && (p_mag[pos] >= high_thres))
		   {
               p_edge[pos] = EDGE;
               follow_edges((p_edge + pos), (p_mag + pos), (IMP_S16)low_thres, n_cols);
		   }
       }
   }

   /****************************************************************************
   * Set all the remaining possible edges to non-edges.
   ****************************************************************************/
   for (r = 0, pos = 0; r < n_rows; r++)
      for (c = 0; c < n_cols; c++, pos++)
	  {
		  if (p_edge[pos] != EDGE)
			  p_edge[pos] = NOEDGE;
	  }
}
void avd_canny_process2( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy, IMP_U8 *p_img_mag, IMP_U8 *p_img_edge,
					IMP_S32 i_col, IMP_S32 i_row, IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows, IMP_S32 tgrad )
{
	round_parameters( &i_row, &i_col, &n_rows, &n_cols, n_img_rows, n_img_cols );
	if( p_img_gray )
		canny_calc_gradmag_i( p_img_gray, p_grad_xy, p_img_mag, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows );
	else
		canny_calc_magnitude_i( p_grad_xy, p_img_mag, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows );
	canny_non_max_supp_i( p_grad_xy, p_img_mag, p_img_edge, i_col, i_row, n_cols, n_rows, n_img_cols, n_img_rows, tgrad );
}


void avd_canny_process( IMP_U8 *p_img_gray, IMP_S8 *p_grad_xy,
					IMP_U8 *p_img_mag, IMP_U8 *p_img_edge,
					IMP_S32 i_col, IMP_S32 i_row,
					IMP_S32 n_cols, IMP_S32 n_rows,
					IMP_S32 n_img_cols, IMP_S32 n_img_rows,
					IMP_S32 tgrad )
{
    IMP_S32 row_count, col_count, count;
	IMP_U8 *p_gray_row;
	IMP_S8 *p_gxy_row;
	IMP_U8 *p_mag_row;
    IMP_U8 *p_result_row;
    IMP_U8 *p_gray;
    IMP_S8 *p_gxy;
    IMP_U8 *p_tmp_mag;
    IMP_U8 *p_tmp_result;
	IMP_S32 z11, z12, z21, z22;
    IMP_S32 m00, gx, gy;
    IMP_S32 nMag1,nMag2;
    IMP_S32 nXperp,nYperp;
	IMP_U32 gxa, gya;
	IMP_S32 signx, signy;
	IMP_S32 sy1, sx1, sy2, sx2;
	IMP_S32 m00_t;
	IMP_U8 *p_gray_2;
	IMP_S8 *p_gxy_2;
	IMP_U8 *p_tmp_mag_2;
    IMP_U8 *p_tmp_result_2;
	IMP_S32 z11_2, z12_2, z21_2, z22_2;
    IMP_S32 m00_2, gx_2, gy_2;
    IMP_S32 nMag1_2,nMag2_2;
    IMP_S32 nXperp_2,nYperp_2;
	IMP_U32 gxa_2, gya_2;
	IMP_S32 signx_2, signy_2;
	IMP_S32 sy1_2, sx1_2, sy2_2, sx2_2;
	IMP_S32 m00_t_2;
	IMP_S32 val1, val2, val3;
	IMP_S32 g1, g2, g3, g4;
	IMP_S32 val1_2, val2_2, val3_2;
	IMP_S32 g1_2, g2_2, g3_2, g4_2;
	IMP_S32 n_stride = n_img_cols;
	IMP_U8 *p_result;
	IMP_S32 offset;

	offset = n_img_cols*i_row+i_col;
	p_img_gray += offset;
	p_grad_xy += offset*2;
	p_img_mag += offset;
	p_img_edge += offset;
	p_result = p_img_edge;

	// Zero the edges of the result image
	offset = n_stride*(n_rows-1);
	p_result_row = p_result;
	p_tmp_result = p_result + offset;
	p_gxy_row = p_grad_xy;
	p_gxy = p_grad_xy + offset*2;
	p_mag_row = p_img_mag;
	p_tmp_mag = p_img_mag + offset;
	for( count=0; count<n_cols; count++ )
	{
        p_result_row[count] = 0;
        p_result_row[count+n_stride] = 0;
		p_tmp_result[count] = 0;
		p_tmp_result[count-n_stride] = 0;

        ((IMP_U16*)p_gxy_row)[count] = 0;
        ((IMP_U16*)p_gxy)[count] = 0;

        p_mag_row[count] = 0;
		p_tmp_mag[count] = 0;
	}
	offset = n_cols-1;
	p_result_row = p_result;
	p_tmp_result = p_result + offset;
	p_gxy_row = p_grad_xy;
	p_gxy = p_grad_xy + offset*2;
	p_mag_row = p_img_mag;
	p_tmp_mag = p_img_mag + offset;
    for( count= 0; count<n_rows; count++ )
	{
		IMP_S32 idx = count*n_stride;

		p_result_row[idx] = 0;
		p_result_row[idx+1] = 0;
        p_tmp_result[idx] = 0;
        p_tmp_result[idx-1] = 0;

        ((IMP_U16*)p_gxy_row)[idx] = 0;
        ((IMP_U16*)p_gxy)[idx] = 0;

        p_mag_row[idx] = 0;
		p_tmp_mag[idx] = 0;
	}

	// Suppress non-maximum points
	for( row_count=1; row_count<=2; row_count++ )
	{
		offset = (n_stride*row_count+1);
		p_gray = p_img_gray + offset;
		p_gxy = p_grad_xy + offset*2;
		p_tmp_mag = p_img_mag + offset;
		p_gray_2 = p_gray+1;
		p_gxy_2 = p_gxy+2;
		p_tmp_mag_2 = p_tmp_mag+1;

		for( col_count=1; col_count<n_cols-1; col_count+=2 )
		{
			g1 = p_gray[-1];
			g2 = p_gray[1];
			g3 = p_gray[-n_stride];
			g4 = p_gray[n_stride];
			g1_2 = p_gray_2[-1];
			g2_2 = p_gray_2[1];
			g3_2 = p_gray_2[-n_stride];
			g4_2 = p_gray_2[n_stride];

			val1 = ( g1 - g2 ) >> 1;
			val2 = ( g3 - g4 ) >> 1;
			val1_2 = ( g1_2 - g2_2 ) >> 1;
			val2_2 = ( g3_2 - g4_2 ) >> 1;

			p_gxy[0] = val1;
			p_gxy[1] = val2;
			p_gxy_2[0] = val1_2;
			p_gxy_2[1] = val2_2;

			val3 = ( abs(val1) + abs(val2) );
			p_tmp_mag[0] = val3;
			val3_2 = ( abs(val1_2) + abs(val2_2) );
			p_tmp_mag_2[0] = val3_2;

			p_gray += 2;
			p_gxy += 4;
			p_tmp_mag += 2;
			p_gray_2 += 2;
			p_gxy_2 += 4;
			p_tmp_mag_2 +=2;
		}
	}
	p_gray_row = p_img_gray + (n_stride*3+3);
	p_gxy_row = p_grad_xy + (n_stride*2+2)*2;
	p_mag_row = p_img_mag + (n_stride*2+2);
	p_result_row = p_result + (n_stride*2+2);
	for( row_count=2; row_count<n_rows-2; row_count++ )
	{
		offset = (n_stride*(row_count+1)+1);
		p_gray = p_img_gray + offset;
		p_gxy = p_grad_xy + offset*2;
		p_tmp_mag = p_img_mag + offset;
		p_gray_2 = p_gray+1;
		p_gxy_2 = p_gxy+2;
		p_tmp_mag_2 = p_tmp_mag+1;

		g1 = p_gray[-1];
		g2 = p_gray[1];
		g3 = p_gray[-n_stride];
		g4 = p_gray[n_stride];
		g1_2 = p_gray_2[-1];
		g2_2 = p_gray_2[1];
		g3_2 = p_gray_2[-n_stride];
		g4_2 = p_gray_2[n_stride];

		val1 = ( g1 - g2 ) >> 1;
		val2 = ( g3 - g4 ) >> 1;
		val1_2 = ( g1_2 - g2_2 ) >> 1;
		val2_2 = ( g3_2 - g4_2 ) >> 1;

		p_gxy[0] = val1;
		p_gxy[1] = val2;
		p_gxy_2[0] = val1_2;
		p_gxy_2[1] = val2_2;

		val3 = ( abs(val1) + abs(val2) );
		p_tmp_mag[0] = val3;
		val3_2 = ( abs(val1_2) + abs(val2_2) );
		p_tmp_mag_2[0] = val3_2;

		p_gray = p_gray_row;
		p_gxy = p_gxy_row;
		p_tmp_mag = p_mag_row;
		p_tmp_result = p_result_row;
		p_gray_2 = p_gray_row+1;
		p_gxy_2 = p_gxy_row+2;
		p_tmp_mag_2 = p_mag_row+1;
		p_tmp_result_2 = p_result_row+1;

		for( col_count=2; col_count<n_cols-2; col_count+=2 )
		{
			g1 = p_gray[-1];
			g2 = p_gray[1];
			g3 = p_gray[-n_stride];
			g4 = p_gray[n_stride];
			g1_2 = p_gray_2[-1];
			g2_2 = p_gray_2[1];
			g3_2 = p_gray_2[-n_stride];
			g4_2 = p_gray_2[n_stride];

			gx = *p_gxy;
			gy = *(p_gxy+1);
			m00 = *( p_tmp_mag - 0 - 0 );

			gx_2 = *p_gxy_2;
			gy_2 = *(p_gxy_2+1);
			m00_2 = *( p_tmp_mag_2 - 0 - 0 );

			val1 = ( g1 - g2 ) >> 1;
			val2 = ( g3 - g4 ) >> 1;
			val1_2 = ( g1_2 - g2_2 ) >> 1;
			val2_2 = ( g3_2 - g4_2 ) >> 1;

			offset = (1+n_stride)*2;
			p_gxy[0+offset] = val1;
			p_gxy[1+offset] = val2;
			p_gxy_2[0+offset] = val1_2;
			p_gxy_2[1+offset] = val2_2;

			val3 = ( abs(val1) + abs(val2) );
			p_tmp_mag[0+1+n_stride] = val3;
			val3_2 = ( abs(val1_2) + abs(val2_2) );
			p_tmp_mag_2[0+1+n_stride] = val3_2;

			gxa = abs(gx);	gya = abs(gy);
			signx = gx>=0 ? 1 : -1;
			signy = gy>=0 ? 1 : -1;
			sx2 = 1*signx;
			sy2 = n_stride*signy;
			if( gxa>=gya )
			{
				nXperp = -gx*signx;
				nYperp = gy*signy;
				sx1 = 1*signx;
				sy1 = 0*signy;
			}
			else
			{
				nYperp = gx*signx;
				nXperp = -gy*signy;
				sx1 = 0*signx;
				sy1 = n_stride*signy;
			}

	        z11 = *( p_tmp_mag - sy1 - sx1 );
	        z12 = *( p_tmp_mag - sy2 - sx2 );
	        z21 = *( p_tmp_mag + sy1 + sx1 );
	        z22 = *( p_tmp_mag + sy2 + sx2 );

			gxa_2 = abs(gx_2);	gya_2 = abs(gy_2);
			signx_2 = gx_2>=0 ? 1 : -1;
			signy_2 = gy_2>=0 ? 1 : -1;
			sx2_2 = 1*signx_2;
			sy2_2 = n_stride*signy_2;
			if( gxa_2>=gya_2 )
			{
				nXperp_2 = -gx_2*signx_2;
				nYperp_2 = gy_2*signy_2;
				sx1_2 = 1*signx_2;
				sy1_2 = 0*signy_2;
			}
			else
			{
				nYperp_2 = gx_2*signx_2;
				nXperp_2 = -gy_2*signy_2;
				sx1_2 = 0*signx_2;
				sy1_2 = n_stride*signy_2;
			}

	        z11_2 = *( p_tmp_mag_2 - sy1_2 - sx1_2 );
	        z12_2 = *( p_tmp_mag_2 - sy2_2 - sx2_2 );
	        z21_2 = *( p_tmp_mag_2 + sy1_2 + sx1_2 );
	        z22_2 = *( p_tmp_mag_2 + sy2_2 + sx2_2 );


//			ntmpMag1 = nMag1*m00;
//			ntmpMag2 = nMag2*m00;
//			if ((ntmpMag1 > 0) || (ntmpMag2 > 0))
//			{
//				*p_tmp_result = (UINT8) NOEDGE;
//			}
//			else
//			{
//				if (ntmpMag2 == 0)
//					*p_tmp_result = (UINT8) NOEDGE;
//				else
//					*p_tmp_result = (UINT8) POSSIBLE_EDGE;
//			}


			m00_t = ( m00 );
			nMag1 = (m00 - z11) * nXperp + (z12 - z11) * nYperp;
			nMag2 = (m00 - z21) * nXperp + (z22 - z21) * nYperp;
//			*p_tmp_result = ( m00<=tgrad || nMag1>=0 || nMag2>=0 ) ? NOEDGE : POSSIBLE_EDGE;
			*p_tmp_result = ( m00<=tgrad || nMag1>=0 || nMag2>=0 ) ? NOEDGE : m00_t;

			m00_t_2 = ( m00_2 );
			nMag1_2 = (m00_2 - z11_2) * nXperp_2 + (z12_2 - z11_2) * nYperp_2;
			nMag2_2 = (m00_2 - z21_2) * nXperp_2 + (z22_2 - z21_2) * nYperp_2;
//			*p_tmp_result_2 = ( m00_2<=tgrad || nMag1_2>=0 || nMag2_2>=0 ) ? NOEDGE : POSSIBLE_EDGE;
			*p_tmp_result_2 = ( m00_2<=tgrad || nMag1_2>=0 || nMag2_2>=0 ) ? NOEDGE : m00_t_2;

			p_gray += 2;
			p_gxy += 4;
			p_tmp_mag += 2;
			p_tmp_result += 2;
			p_gray_2 += 2;
			p_gxy_2 += 4;
			p_tmp_mag_2 +=2;
			p_tmp_result_2 += 2;
        }

		p_gray_row += n_stride;
		p_gxy_row += n_stride*2;
		p_mag_row += n_stride;
        p_result_row += n_stride;
	}
//	apply_hysteresis( (IMP_U16*)p_img_mag,352, 288,4, 8,p_img_edge );

}

