/*******************************************************************************
**+--------------------------------------------------------------------------+**
**|                            ****                                          |**
**|                            ****                                          |**
**|                            ******o***                                    |**
**|                      ********_///_****                                   |**
**|                      ***** /_//_/ ****                                   |**
**|                       ** ** (__/ ****                                    |**
**|                           *********                                      |**
**|                            ****                                          |**
**|                            ***                                           |**
**|                                                                          |**
**|         Copyright (c) 2017 Texas Instruments Incorporated                |**
**|                        ALL RIGHTS RESERVED                               |**
**|                                                                          |**
**| Permission to use, copy, modify, or distribute this software,            |**
**| whether in part or in whole, for any purpose is forbidden without        |**
**| a signed licensing agreement and NDA from Texas Instruments              |**
**| Incorporated (TI).                                                       |**
**|                                                                          |**
**| TI makes no representation or warranties with respect to the             |**
**| performance of this computer program, and specifically disclaims         |**
**| any responsibility for any damages, special or consequential,            |**
**| connected with the use of this program.                                  |**
**|                                                                          |**
**+--------------------------------------------------------------------------+**
*******************************************************************************/

#include <fftlib.h>
#include <math.h>

// include test infrastructure provided by FFTLIB
#include "FFTLIB_test.h"

#include "FFTLIB_fft1d_i16sc_c16sc_o16sc.h"
#include "FFTLIB_fft1d_i16sc_c16sc_o16sc_cn.h"
#include "FFTLIB_fft1d_i16sc_c16sc_o16sc_idat.h"

__attribute__ ((section (".msmcData"), aligned (64)))
int8_t msmcBuffer[FFTLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__ ((section (".ddrData"), aligned (64)))
int8_t ddrBuffer[2048 * 1024];

#pragma DATA_SECTION(FFTLIB_fft1d_i16sc_c16sc_o16sc_pBlock, ".pinit")
uint8_t FFTLIB_fft1d_i16sc_c16sc_o16sc_pBlock
    [FFTLIB_FFT1D_I16SC_C16SC_O16SC_PBLOCK_SIZE];

int16_t volatile volatileSum = 0;

static int16_t float2short (FFTLIB_D64 x)
{
   x = floor (0.5 + x); // Explicit rounding to integer //
   if (x >= 32767.0)
      return 32767;
   if (x <= -32768.0)
      return -32768;
   return (int16_t) x;
}

void tw_gen (int16_t *pW, uint32_t numPoints)
{
   int32_t          i, j, k, t;
   const FFTLIB_D64 PI   = 3.141592654;
   FFTLIB_D64 twF2sScale = 32767.5; /* Scale twiddle factors (max abs value of
                                     * 1) to use full capacity of int16_t */

   t = numPoints >> 2;
   for (j = 1, k = 0; j <= numPoints >> 2; j = j << 2) {
      for (i = 0; i < numPoints >> 2; i += j) {
         /* TODO: Big endian requires different format of Twiddle factors? */
         pW[k]     = float2short (twF2sScale * cos (2 * PI * i / numPoints));
         pW[k + 1] = float2short (twF2sScale * (-sin (2 * PI * i / numPoints)));
         pW[k + 2 * t] =
             float2short (twF2sScale * cos (4 * PI * i / numPoints));
         pW[k + 2 * t + 1] =
             float2short (twF2sScale * (-sin (4 * PI * i / numPoints)));
         pW[k + 4 * t] =
             float2short (twF2sScale * cos (6 * PI * i / numPoints));
         pW[k + 4 * t + 1] =
             float2short (twF2sScale * (-sin (6 * PI * i / numPoints)));
         k += 2;
      }
      k += (t) *4;
      t = t >> 2;
   }
}

int FFTLIB_fft1d_i16sc_c16sc_o16sc_d (uint32_t *pProfile,
                                      uint8_t   LevelOfFeedback)
{
   int32_t                               tpi;
   fft1d_i16sc_c16sc_o16sc_testParams_t *prm;

   uint32_t testNum = 0;     // test case id
   int32_t  currentTestFail; // status of current test case
   int32_t  fail = 0;        // fail flag

   fft1d_i16sc_c16sc_o16sc_getTestParams (&prm, &test_cases);

   TI_profile_init ("FFTLIB_fft1d_i16sc_c16sc_o16sc");

#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   FILE *fpOutputCSV = fopen ("FFTLIB_fft1d_i16sc_c16sc_o16sc.csv", "w+");
   fprintf (fpOutputCSV, "Test ID,"
                         "Num Points\n");
#endif

   for (tpi = 0; tpi < test_cases; tpi++) {

      int32_t       status_nat_vs_opt = TI_TEST_KERNEL_PASS;
      int32_t       status_nat_vs_ref = TI_TEST_KERNEL_PASS;
      FFTLIB_STATUS status_opt        = FFTLIB_SUCCESS;
      FFTLIB_STATUS status_nat        = FFTLIB_SUCCESS;
      uint32_t      numPoints;
      uint32_t      numShifts;

      int16_t  *pX;
      int16_t  *pY;
      int16_t  *pXCn;
      int16_t  *pYCn;
      int16_t  *pW;
      uint32_t *pShift;

      testNum   = prm[tpi].testID;
      numPoints = prm[tpi].numPoints;
      numShifts = prm[tpi].numShifts;

      pX = (int16_t *) TI_memalign (
          64,                                /* pX is required to be*/
          numPoints * 2 * sizeof (int16_t)); /* 16-byte aligned for
                                              * streaming engine use
                                              * in kernel           */
      pY     = (int16_t *) TI_memalign (64, numPoints * 2 * sizeof (int16_t));
      pW     = (int16_t *) TI_memalign (64, numPoints * 2 * sizeof (int16_t));
      pShift = (uint32_t *) TI_memalign (64, numShifts * sizeof (uint32_t));
      /* pXCn   = (int16_t *) malloc (numPoints * 2 * sizeof (int16_t)); */
      /* pYCn   = (int16_t *) malloc (numPoints * 2 * sizeof (int16_t)); */
      pXCn = (int16_t *) msmcBuffer;

      pYCn = pXCn + numPoints * 2;

      if (pX && pY && pW && pXCn && pYCn && pShift) {
         int32_t              fail;
         FFTLIB_bufParams1D_t bufParamsData;
         FFTLIB_bufParams1D_t bufParamsShift;

         TI_fillBuffer (prm[tpi].testPattern, 0, pX, prm[tpi].staticIn0,
                        numPoints * 2, 1, 1, sizeof (int16_t),
                        testPatternString);
         bufParamsData.dim_x     = numPoints * 2;
         bufParamsData.data_type = FFTLIB_INT16;

         /* Save data before calling kernel function because kernel
          * writes into input buffer                                */
         TI_fillBuffer (STATIC, 0, pXCn, pX, numPoints * 2, 1, 1,
                        sizeof (int16_t), testPatternString);

         bufParamsShift.dim_x     = numShifts;
         bufParamsShift.data_type = FFTLIB_UINT32;
         TI_fillBuffer (STATIC, 0, /* Shift vector is always STATIC */
                        pShift, prm[tpi].shiftVector, numShifts, 1, 1,
                        sizeof (uint32_t), testPatternString);

         tw_gen (pW, numPoints);

         TI_profile_start (TI_PROFILE_KERNEL_INIT);
         status_opt = FFTLIB_fft1d_i16sc_c16sc_o16sc_init (
             (int16_t *) pX, &bufParamsData, (int16_t *) pW, &bufParamsData,
             (int16_t *) pY, &bufParamsData, (uint32_t *) pShift,
             &bufParamsShift, FFTLIB_fft1d_i16sc_c16sc_o16sc_pBlock);
         TI_profile_stop ();

         TI_profile_start (TI_PROFILE_KERNEL_OPT);
         status_opt = FFTLIB_fft1d_i16sc_c16sc_o16sc_kernel (
             (int16_t *) pX, &bufParamsData, (int16_t *) pW, &bufParamsData,
             (int16_t *) pY, &bufParamsData, (uint32_t *) pShift,
             &bufParamsShift, FFTLIB_fft1d_i16sc_c16sc_o16sc_pBlock);
         TI_profile_stop ();

#if (defined(RTL_TEST) || defined(QT_TEST) || defined(EVM_TEST))
         /* Save data before calling kernel function because kernel
          * writes into input buffer                                */
         TI_fillBuffer (STATIC, 0, pX, pXCn, numPoints * 2, 1, 1,
                        sizeof (int16_t), testPatternString);

         TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
         status_opt = FFTLIB_fft1d_i16sc_c16sc_o16sc_kernel (
             (int16_t *) pX, &bufParamsData, (int16_t *) pW, &bufParamsData,
             (int16_t *) pY, &bufParamsData, (uint32_t *) pShift,
             &bufParamsShift, FFTLIB_fft1d_i16sc_c16sc_o16sc_pBlock);
         TI_profile_stop ();
         // get output to L1D
         /* uint32_t row    = 0; */
         int32_t col    = 0;
         int16_t outSum = 0;
         // treat output as bytes to be data type agnostic
         int16_t *pDst = (int16_t *) pY;
         /* for (row = 0; row < numChannels; row++) { */
         for (col = 0; col < numPoints; col++) {
            outSum += *pDst;
            pDst++;
         }
         /* } */

         /* dummy store of outSum to insure that the compiler does
not remove
          * it. */
         volatileSum = outSum;

         /* Re-copy data before calling kernel function because kernel
          * writes into input buffer                                */
         TI_fillBuffer (STATIC, 0, pX, pXCn, numPoints * 2, 1, 1,
                        sizeof (int16_t), testPatternString);

         TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
         status_opt = FFTLIB_fft1d_i16sc_c16sc_o16sc_kernel (
             (int16_t *) pX, &bufParamsData, (int16_t *) pW, &bufParamsData,
             (int16_t *) pY, &bufParamsData, (uint32_t *) pShift,
             &bufParamsShift, FFTLIB_fft1d_i16sc_c16sc_o16sc_pBlock);
         TI_profile_stop ();
#endif
         TI_profile_start (TI_PROFILE_KERNEL_CN);
         status_nat = FFTLIB_fft1d_i16sc_c16sc_o16sc_cn (
             (int16_t *) pXCn, &bufParamsData, (int16_t *) pW, &bufParamsData,
             (int16_t *) pYCn, &bufParamsData, (uint32_t *) pShift,
             &bufParamsShift);
         TI_profile_stop ();

         status_nat_vs_opt = TI_compare_mem ((void *) pY, (void *) pYCn,
                                             numPoints * 2 * sizeof (int16_t));
         if (prm[tpi].staticOut != NULL) {
            status_nat_vs_ref =
                TI_compare_mem ((void *) prm[tpi].staticOut, (void *) pYCn,
                                numPoints * 2 * sizeof (int16_t));
         }

         currentTestFail =
             ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
              (status_nat_vs_ref == TI_TEST_KERNEL_FAIL) ||
              (status_opt != FFTLIB_SUCCESS) || (status_nat != FFTLIB_SUCCESS))
                 ? 1
                 : 0;

         fail = ((fail == 1) || (currentTestFail == 1)) ? 1 : 0;

         pProfile[3 * tpi] =
             (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT);
         pProfile[3 * tpi + 1] =
             (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARM);
         pProfile[3 * tpi + 2] =
             (int32_t) TI_profile_get_cycles (TI_PROFILE_KERNEL_OPT_WARMWRB);
         /* est_test = 1; */
         sprintf (desc, "%s generated input | numPoints=%d", testPatternString,
                  numPoints);

#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
         fprintf (fpOutputCSV, "%d, %d\n", testNum, numPoints);
#endif

         uint64_t archCycles = 0, estCycles = 0;
         TI_profile_add_test (testNum++, numPoints, archCycles, estCycles,
                              currentTestFail, desc);
      }
      else {
         sprintf (desc, "numPoints=%d", numPoints);
         TI_profile_skip_test (desc);
      }

      /* Free buffers for each test vector */
      TI_align_free (pX);
      TI_align_free (pY);
      TI_align_free (pW);
      /* free (pXCn); */
      /* free (pYCn); */
   }

#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   /* Close results CSV */
   fclose (fpOutputCSV);
#endif //#if ( !defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   return fail;
}

int test_main (uint32_t *pProfile)
{
#if !defined(_HOST_BUILD)
   if (TI_cache_init ()) {
      TI_memError ("FFTLIB_fft1dBatched_i16sc_c16sc_o16sc");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return FFTLIB_fft1d_i16sc_c16sc_o16sc_d (&pProfile[0], 0);
   }
}

/* Main call for individual test projects */
#if !defined(__ONESHOTTEST) && !defined(RTL_TEST)
int main ()
{
   int fail = 1;

   uint32_t profile[256 * 3];

   FFTLIB_TEST_init ();

   fail = test_main (&profile[0]);

#if !defined(NO_PRINTF)
   if (fail == 0)
      printf ("Test Pass!\n");
   else
      printf ("Test Fail!\n");

   int i;
   for (i = 0; i < test_cases; i++) {
      printf ("Test %4d: Cold Cycles = %8d, Warm Cycles = %8d, Warm Cycles WRB "
              "= %8d\n",
              i, profile[3 * i], profile[3 * i + 1], profile[3 * i + 2]);
   }
#endif

   return fail;
}
#endif

/* ======================================================================== */
/*  End of file:  FFTLIB_fft1d_i16sc_c16sc_o16sc_d.c                        */
/* ======================================================================== */
