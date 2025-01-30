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

// include test infrastructure provided by FFTLIB
#include "FFTLIB_test.h"

#include <math.h>

#include "FFTLIB_ifft1d_i32fc_c32fc_o32fc.h"
#include "FFTLIB_ifft1d_i32fc_c32fc_o32fc_cn.h"
#include "FFTLIB_ifft1d_i32fc_c32fc_o32fc_idat.h"

__attribute__ ((section (".msmcData"), aligned (64)))
int8_t msmcBuffer[FFTLIB_L3_RESULTS_BUFFER_SIZE];
__attribute__ ((section (".ddrData"), aligned (64)))
int8_t ddrBuffer[2048 * 1024];

int16_t volatile volatileSum = 0;

#pragma DATA_SECTION(FFTLIB_ifft1d_i32fc_c32fc_o32fc_pBlock, ".pinit")
uint8_t FFTLIB_ifft1d_i32fc_c32fc_o32fc_pBlock
    [FFTLIB_FFT1D_I32FC_C32FC_O32FC_PBLOCK_SIZE];

void tw_gen (FFTLIB_F32 *pW, int32_t numPoints)
{
   int32_t      i, j, k, t;
   const double PI = 3.141592654;
   t               = numPoints >> 2;
   for (j = 1, k = 0; j <= numPoints >> 2; j = j << 2) {
      for (i = 0; i < numPoints >> 2; i += j) {
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

int FFTLIB_ifft1d_i32fc_c32fc_o32fc_d (uint32_t *pProfile,
                                       uint8_t   LevelOfFeedback)
{
   int32_t                                tpi;
   ifft1d_i32fc_c32fc_o32fc_testParams_t *prm;
   uint32_t                               testNum = 0; // test case id
   int32_t currentTestFail; // status of current test case
   int32_t fail = 0;        // fail flag

   ifft1d_i32fc_c32fc_o32fc_getTestParams (&prm, &test_cases);

   TI_profile_init ("FFTLIB_ifft1d_i32fc_c32fc_o32fc");

#if (!defined(RTL_TEST) && !defined(QT_TEST) && !defined(EVM_TEST))
   FILE *fpOutputCSV = fopen ("FFTLIB_ifft1d_i32fc_c32fc_o32fc.csv", "w+");
   fprintf (fpOutputCSV, "Test Pattern,"
                         "Num Channels,"
                         "Num Points\n");
#endif

   for (tpi = 0; tpi < test_cases; tpi++) {

      int32_t       status_nat_vs_opt = TI_TEST_KERNEL_PASS;
      int32_t       status_opt_vs_ref = TI_TEST_KERNEL_PASS;
      FFTLIB_STATUS status_opt        = FFTLIB_SUCCESS;
      FFTLIB_STATUS status_nat        = FFTLIB_SUCCESS;
      uint32_t      numPoints;
      FFTLIB_F32   *pX;
      FFTLIB_F32   *pY;
      FFTLIB_F32   *pXCn;
      FFTLIB_F32   *pYCn;
      FFTLIB_F32   *pW;

      testNum   = prm[tpi].testID;
      numPoints = prm[tpi].numPoints;

      pX = (FFTLIB_F32 *) TI_memalign (
          128,                                  /* pX is required to be*/
          numPoints * 2 * sizeof (FFTLIB_F32)); /* 16-byte aligned for
                                                 * streaming engine use
                                                 * in kernel           */
      pY =
          (FFTLIB_F32 *) TI_memalign (128, numPoints * 2 * sizeof (FFTLIB_F32));
      pW =
          (FFTLIB_F32 *) TI_memalign (128, numPoints * 2 * sizeof (FFTLIB_F32));
      pXCn = (FFTLIB_F32 *) msmcBuffer;
      pYCn = pXCn + numPoints * 2 * sizeof (FFTLIB_F32);
      /* pYCn = (FFTLIB_F32 *) malloc (numPoints * 2 * sizeof (FFTLIB_F32)); */

      if (pX && pY && pW && pYCn) {
         FFTLIB_bufParams1D_t bufParamsData;
         if ((prm[tpi].testPattern == RANDOM) ||
             (prm[tpi].testPattern == RANDOM_SIGNED)) {
            uint32_t i = 0;
            for (i = 0; i < numPoints * 2; i++) {
               pX[i] =
                   (float) rand () / (float) (RAND_MAX / (1 << 8)) - (1 << 7);
            }
         }
         else {
            TI_fillBuffer (prm[tpi].testPattern, 0, pX, prm[tpi].staticIn0,
                           numPoints * 2, 1, 1, sizeof (FFTLIB_F32),
                           testPatternString);
         }

         TI_fillBuffer (STATIC, 0, pXCn, pX, numPoints * 2, 1, 1,
                        sizeof (FFTLIB_F32), testPatternString);

         bufParamsData.dim_x     = numPoints * 2;
         bufParamsData.data_type = FFTLIB_FLOAT32;

         tw_gen (pW, numPoints);

         TI_profile_start (TI_PROFILE_KERNEL_INIT);
         status_opt = FFTLIB_ifft1d_i32fc_c32fc_o32fc_init (
             (FFTLIB_F32 *) pX, &bufParamsData, (FFTLIB_F32 *) pW,
             &bufParamsData, (FFTLIB_F32 *) pY, &bufParamsData,
             FFTLIB_ifft1d_i32fc_c32fc_o32fc_pBlock);
         TI_profile_stop ();

         TI_profile_start (TI_PROFILE_KERNEL_OPT);
         status_opt = FFTLIB_ifft1d_i32fc_c32fc_o32fc_kernel (
             (FFTLIB_F32 *) pX, &bufParamsData, (FFTLIB_F32 *) pW,
             &bufParamsData, (FFTLIB_F32 *) pY, &bufParamsData,
             FFTLIB_ifft1d_i32fc_c32fc_o32fc_pBlock);
         TI_profile_stop ();

         /* printf ("\n\n"); */

         /* uint32_t i = 0; */
         /* for (i = 0; i < numPoints * 2; i++) { */
         /*    printf ("%.2f, ", ((float *) pY)[i]); */
         /* } */

#if (defined(RTL_TEST) || defined(QT_TEST) || defined(EVM_TEST))

         TI_fillBuffer (STATIC, 0, pX, pXCn, numPoints * 2, 1, 1,
                        sizeof (FFTLIB_F32), testPatternString);

         /* run warm instruction cache test */
         TI_profile_start (TI_PROFILE_KERNEL_OPT_WARM);
         status_opt = FFTLIB_ifft1d_i32fc_c32fc_o32fc_kernel (
             (FFTLIB_F32 *) pX, &bufParamsData, (FFTLIB_F32 *) pW,
             &bufParamsData, (FFTLIB_F32 *) pY, &bufParamsData,
             FFTLIB_ifft1d_i32fc_c32fc_o32fc_pBlock);
         TI_profile_stop ();

         TI_fillBuffer (STATIC, 0, pX, pXCn, numPoints * 2, 1, 1,
                        sizeof (FFTLIB_F32), testPatternString);

         // get output to L1D
         /* uint32_t row    = 0; */
         int32_t col    = 0;
         int16_t outSum = 0;
         // treat output as bytes to be data type agnostic
         FFTLIB_F32 *pDst = (FFTLIB_F32 *) pY;
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
         TI_profile_start (TI_PROFILE_KERNEL_OPT_WARMWRB);
         status_opt = FFTLIB_ifft1d_i32fc_c32fc_o32fc_kernel (
             (FFTLIB_F32 *) pX, &bufParamsData, (FFTLIB_F32 *) pW,
             &bufParamsData, (FFTLIB_F32 *) pY, &bufParamsData,
             FFTLIB_ifft1d_i32fc_c32fc_o32fc_pBlock);
         TI_profile_stop ();
#endif

         TI_profile_start (TI_PROFILE_KERNEL_CN);
         status_nat = FFTLIB_ifft1d_i32fc_c32fc_o32fc_cn (
             (FFTLIB_F32 *) pXCn, &bufParamsData, (FFTLIB_F32 *) pW,
             &bufParamsData, (FFTLIB_F32 *) pYCn, &bufParamsData);
         TI_profile_stop ();

         status_nat_vs_opt = TI_compare_mem_2D_float (
             (void *) pYCn, (void *) pY, 0, (double) powf (10, -5),
             numPoints * 2, 1, numPoints * 2, sizeof (FFTLIB_F32));

         if (prm[tpi].staticOut != NULL) {
            status_opt_vs_ref = TI_compare_mem_2D_float (
                (void *) prm[tpi].staticOut, (void *) pY, 0,
                (double) powf (10, -3), numPoints * 2, 1, numPoints * 2,
                sizeof (FFTLIB_F32));
         }

         currentTestFail =
             ((status_nat_vs_opt == TI_TEST_KERNEL_FAIL) ||
              (status_opt_vs_ref == TI_TEST_KERNEL_FAIL) ||
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
         /* TI_profile_formula_add_test ( */
         /*     numPoints * numChannels, 1, NULL, fail, desc, 1); */
         TI_profile_add_test (testNum++, numPoints, archCycles, estCycles,
                              currentTestFail, desc);
      }
      else {
         sprintf (desc, "numPoints=%d", numPoints);
         TI_profile_skip_test (desc);
      }

      TI_align_free (pX);
      TI_align_free (pY);
      TI_align_free (pW);
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
      TI_memError ("FFTLIB_ifft1dBatched_i32fc_c32fc_o32fc");
      return 1;
   }
   else
#else
   printf ("_HOST_BUILD is defined.\n");
#endif //_HOST_BUILD
   {
      return FFTLIB_ifft1d_i32fc_c32fc_o32fc_d (&pProfile[0], 0);
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
/*  End of file:  FFTLIB_ifft1d_i32fc_c32fc_o32fc_d.c                        */
/* ======================================================================== */
