#include <math.h>

#include "FFTLIB_ifft1d_i32fc_c32fc_o32f.h"

void ifft_c2r_tw_gen (FFTLIB_F32 *pW, int32_t numPoints)
{
   int32_t      i, j, k, t;
   const double PI = 3.141592654;

   // since we perform N-point FFT to compute FFT of 2N-point real sequence
   numPoints >>= 1;

   t = numPoints >> 2;
   for (j = 1, k = 0; j <= numPoints >> 2; j = j << 2) {
      for (i = 0; i<numPoints>> 2; i += j) {
         /* TODO: Big endian requires different format of Twiddle factors? */
         pW[k]             = (FFTLIB_F32) cos (2 * PI * i / numPoints);
         pW[k + 1]         = (FFTLIB_F32) -sin (2 * PI * i / numPoints);
         pW[k + 2 * t]     = (FFTLIB_F32) cos (4 * PI * i / numPoints);
         pW[k + 2 * t + 1] = (FFTLIB_F32) -sin (4 * PI * i / numPoints);
         pW[k + 4 * t]     = (FFTLIB_F32) cos (6 * PI * i / numPoints);
         pW[k + 4 * t + 1] = (FFTLIB_F32) -sin (6 * PI * i / numPoints);
         k += 2;
      }
      k += (t) *4;
      t = t >> 2;
   }
}

void ifft_c2r_split_factor_gen (FFTLIB_F32 *pSf, int32_t numPoints)
{

   int32_t i = 0;

   const double PI = 3.141592654;

   for (i = 0; i<numPoints>> 1; i++) {
      pSf[2 * i]     = (FFTLIB_F32) (0.5 * (1 - sin (2 * PI * i / numPoints)));
      pSf[2 * i + 1] = (FFTLIB_F32) (0.5 * (cos (2 * PI * i / numPoints)));

      pSf[2 * i + numPoints] =
          (FFTLIB_F32) (0.5 * (1 + sin (2 * PI * i / numPoints)));
      pSf[2 * i + numPoints + 1] =
          (FFTLIB_F32) (0.5 * (-cos (2 * PI * i / numPoints)));
   }
}
