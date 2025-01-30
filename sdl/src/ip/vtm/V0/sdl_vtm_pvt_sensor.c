/**
 * @file  sdl_vtm_pvt_sensor.c
 *
 * @brief
 *  C implementation of the workaround computed temperature array.
 *
 *  Contains the look up table and control command and status query
 *  function definitions
 *
 * @details
 * The VTM Temperature Monitors (TEMPSENSORs) are trimmed during production
 * with resulting values stored in software readable registers.
 *
 * Software should use these register values when translating the
 * Temperature Monitor output codes to temperature values.
 *
 * A bug in the VTM-IP has zero-out the trim bits of the
 * Temperature Monitor IP (PVT). The end result is an increased error
 * in the temperature reading, which is estimated up to +/-20c.
 * The temperature monitoring feature is not usable with such
 * large error.
 *
 * This HW issues is workaround by soft trim by passing the desired trim
 * via the GP eFUSEs.
 *
 * - Use as golden reference the PVT J721e-PVT-Code values, produced by the
 *   J721e-PVTPolynomial for -40c, 30c and 125c and with it, de-compress the
 *   e-fuse AMTV values for a given sensor for the same 3 temperature points.
 * - Now using the AMTV 10-bit values vs the golden reference J7es-PVT-Code
 *   values
 *   create 2 error lines using linear interpolation of the errors.
 * - First using AMTV(-40c) and AMTV(30c) create the error line for that
 *   segment vs J721e-PVT-Code. We call that error line L_err_a1.
 * - Now to find out all the interpolation points In the look-up table for this
 *   segment we simply add L_err_a1 function values to
 *   J721e-PVT-Code values in this segment.
 * - Followed by creating the error line for the segment between AMTV(30c) and
 *   AMTV(125c) . We call that error line L_err_a2.
 * - Now to find out all the interpolation points In the look-up table for this segment we
 *   simply add L_err_a2 function values to J721e-PVT-Code values in this segment.
 * - Finally we extrapolate for the segment between 125c and 150c using the same
 *   method. Simply add L_err_a2 function values to J721e-PVT-Code values in this
 *   segment.
 *
 *  \par
 *  ============================================================================
 *  @n   (C) Copyright 2021, Texas Instruments, Inc.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS int32_tERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
#include <string.h>
#include <stdbool.h>
#include <src/ip/vtm/V0/sdl_ip_vtm.h>
#include <include/soc.h>
#include <src/ip/vtm/V0/sdl_vtm_pvt_sensor.h>
#include <src/ip/vtm/V0/sdl_pvt_sensor_lut.h>
#include <sdl_types.h>
#include <stdint.h>

extern int32_t gNumTempSensors;
extern int32_t gNumCoreVoltageDomains;

/* Global variables */
int32_t              gSDL_pvt_poly_work_around[SDL_VTM_NUM_OF_SENSOR_WA_COMP][SDL_VTM_NUM_OF_ADC_CODES];
int32_t              gSDL_vtm_pvt_error[SDL_VTM_NUM_EFUSE_REGS];

/* Static globals */
#if defined (SOC_J721E)
/* ADC codes for standard temperatures */
static int32_t minus40cref = 5;
static int32_t plus30cref = 253;
static int32_t plus125cref = 730;
static int32_t plus150cref = 940;
#endif

/* lut_computation done */
bool gSDL_vtm_lut_done[SDL_VTM_NUM_OF_SENSOR_WA_COMP];
int32_t gSDL_lut_computation_init_done = SDL_VTM_VALUES_ARE_UNINITIALIZED;

/* Internal functions */
static void SDL_vtmPrepLookupTable(void);

#if defined (SOC_J721E)
static int32_t  SDL_vtm2sComplement(uint32_t value, uint32_t mask);
static void     SDL_vtmEfuseOffsetAndMasks(uint32_t id, int32_t *pErr);


static void SDL_vtmGetErr(int32_t   *p_minus40cerr, \
                          int32_t   *p_plus30cerr,  \
                          int32_t   *p_plus125cerr, \
                          int32_t   *p_plus150cerr, \
                          uint32_t   id);

static int32_t SDL_vtm2sComplement(uint32_t value, uint32_t mask)
{
    uint32_t tmp;
    tmp = ~(value);
    tmp &= mask;
    tmp ++;

    /* Return negative value */
    return (0 - (int32_t)tmp);
}

static void SDL_vtmEfuseOffsetAndMasks(uint32_t id, int32_t *pErr)
{
    uint32_t    *p_efuse0;
    uint32_t    v0, mask0;
    uint32_t    *p_efuse1;
    uint32_t    v1, mask1;
    uint32_t    *p_efuse2;
    uint32_t    v2, mask2;


    p_efuse0 = (uint32_t *)(uintptr_t)(SDL_VTM_EFUSE_BASE_ADDR + 0x0u);
    p_efuse1 = (uint32_t *)(uintptr_t)(SDL_VTM_EFUSE_BASE_ADDR + 0x4u);
    p_efuse2 = (uint32_t *)(uintptr_t)(SDL_VTM_EFUSE_BASE_ADDR + 0x8u);

    /* Populate efuse reg offsets & Bit masks for -40C, 30C, 125C */
    switch (id) {
    case (uint32_t)SDL_VTM_INSTANCE_TS_0:
        v0 = SDL_REG32_FEXT(p_efuse0, VTM_EFUSE0_S0_N40);
        v1 = SDL_REG32_FEXT(p_efuse2, VTM_EFUSE2_S0_P30);
        v2 = SDL_REG32_FEXT(p_efuse1, VTM_EFUSE1_S0_P125);

        mask0 = (SDL_VTM_EFUSE0_S0_N40_MASK >> SDL_VTM_EFUSE0_S0_N40_SHIFT);
        mask1 = (SDL_VTM_EFUSE2_S0_P30_MASK >> SDL_VTM_EFUSE2_S0_P30_SHIFT);
        mask2 = (SDL_VTM_EFUSE1_S0_P125_MASK >> SDL_VTM_EFUSE1_S0_P125_SHIFT);

        /* Store the 2's complement value in 32 bit representation */
        if ((v0 & SDL_VTM_EFUSE0_S0_N40_SIGN_BIT_MASK) == SDL_VTM_EFUSE0_S0_N40_SIGN_BIT_MASK)
        {
            pErr[0] = SDL_vtm2sComplement(v0, mask0);
        }
        else
        {
            pErr[0] = (int32_t)v0;
        }

        if ((v1 & SDL_VTM_EFUSE2_S0_P30_SIGN_BIT_MASK) == SDL_VTM_EFUSE2_S0_P30_SIGN_BIT_MASK)
        {
            pErr[1] = SDL_vtm2sComplement(v1, mask1);
        }
        else
        {
            pErr[1] = (int32_t)v1;
        }
        if ((v2 & SDL_VTM_EFUSE1_S0_P125_SIGN_BIT_MASK) == SDL_VTM_EFUSE1_S0_P125_SIGN_BIT_MASK)
        {
            pErr[2] = SDL_vtm2sComplement(v2, mask2);
        }
        else
        {
            pErr[2] = (int32_t)v2;
        }

        break;
    default:
        break;
    }

}

static void SDL_vtmGetErr(int32_t   *p_minus40cerr, \
                          int32_t   *p_plus30cerr,  \
                          int32_t   *p_plus125cerr, \
                          int32_t   *p_plus150cerr, \
                          uint32_t   id)
{
    /* construct the error values */
    SDL_vtmEfuseOffsetAndMasks(id, &gSDL_vtm_pvt_error[0]);
    *p_minus40cerr = gSDL_vtm_pvt_error[0];
    *p_plus30cerr  = gSDL_vtm_pvt_error[1];
    *p_plus125cerr = gSDL_vtm_pvt_error[2];
    *p_plus150cerr = gSDL_vtm_pvt_error[3];

}

static bool i2128_sw_workaround_needed(void)
{
    uint32_t    *p_efuse0;
    uint32_t    v0;
    bool        retVal = (bool)true;

    p_efuse0 = (uint32_t *)(uintptr_t)(SDL_VTM_EFUSE_BASE_ADDR + 0x0u);

    v0 = SDL_REG32_RD_OFF(p_efuse0, 0);

    /* Workaround not needed if bit30/bit31 is set */
    if (0xc0000000U == (v0 & 0xc0000000U))
    {
        retVal = (bool)false;
    }
    else
    {
        retVal = (bool)true;
    }

    return retVal;
}
#endif

static void SDL_vtmPrepLookupTable(void)
{
#if defined (SOC_J721E)
    int32_t minus40cerr, plus30cerr, plus125cerr, plus150cerr;
    int32_t m1, m2, m3, c1, c2, c3;
    int32_t num, den, inc;
    int32_t err;
#endif
    uint32_t sens_id = 0u;
    int32_t  i;
    int32_t *derived_array = &gSDL_pvt_poly_work_around[0][0];

    if (gSDL_lut_computation_init_done == SDL_VTM_VALUES_ARE_UNINITIALIZED)
    {
        gSDL_vtm_lut_done[sens_id] = FALSE;
    }

#if defined (SOC_J721E)
    /* Work around NOT needed */
    /* If not computed earlier, computation needed only once */
    if (gSDL_vtm_lut_done[sens_id] == FALSE)
    {
        if ((bool)false == i2128_sw_workaround_needed())
        {
            for ( i = 0; i < SDL_VTM_NUM_OF_ADC_CODES; i++)
            {
                derived_array[i] = gSDL_pvt_poly_golden[i];
            }
	}
        else
        {
            /* Work around needed */
            /* If not computed earlier, computation needed only once */
            for (i = 0; i < SDL_VTM_NUM_OF_ADC_CODES; i++)
            {
                derived_array[i] = 0;
            }
            SDL_vtmGetErr(&minus40cerr, &plus30cerr, &plus125cerr, &plus150cerr, sens_id);

            /*
             * Calculate the slope with adc values read from the register
             * as the y-axis param and err in adc value as x-axis param
             * gSDL_pvt_poly[5] represnts -40C with adc value 6 in polynomial equation
             * This is region -40C to 30C
             */
            if (minus40cerr != plus30cerr) {
                num = plus30cref - minus40cref;
                den = plus30cerr - minus40cerr;
                m1 = num / den;
                c1 = plus30cref - (m1 * plus30cerr);
                for (i = 0; i <= plus30cref; i++) {
                    err = (i - c1) / m1;
                    if ((i + err) < 0)
                    {
                        continue;
                    }
                    derived_array[i] = gSDL_pvt_poly[i + err];
                }
            } else { /* Constant error take care of divide by zero */
                for (i = 0; i < plus30cref; i++) {
                    if ((i + plus30cerr) < 0)
                    {
                        continue;
                    }
                    derived_array[i] = gSDL_pvt_poly[i + plus30cerr];
                }
            }

            /*
             * Calculate the slope with adc values
             * as the y-axis param and err in adc value as x-axis param
             * gSDL_pvt_poly[5] represnts -40C with adc value 6 in polynomial equation
             * This is region 30C to 125C
             */
            if (plus125cerr != plus30cerr) {
                num = plus125cref - plus30cref;
                den = plus125cerr - plus30cerr;
                m2 = num / den;
                c2 = plus125cref - (m2 * plus125cerr);

                for (i = plus30cref; i < plus125cref; i++) {
                    err = (i - c2) / m2;
                    derived_array[i] = gSDL_pvt_poly[i + err];
                }
            } else { /* Constant error take care of divide by zero */

                for (i = plus30cref; i < plus125cref; i++) {
                    if ((i + plus30cerr) < 0)
                    {
                        continue;
                    }
                    derived_array[i] = gSDL_pvt_poly[i + plus30cerr];
                }
            }

            /*
             * Calculate the slope with adc values
             * as the y-axis param and err in adc value as x-axis param
             * gSDL_pvt_poly[5] represnts -40C with adc value 6 in polynomial equation
             * This is region 125C to 150C
             */
            if (plus150cerr != plus125cerr) {
                num = plus150cref - plus125cref;
                den = plus150cerr - plus125cerr;
                m3 = num / den;
                c3 = plus125cref - (m3 * plus125cerr);

                for (i = plus125cref; i < SDL_VTM_NUM_OF_ADC_CODES; i++) {
                    err = (i - c3) / m3;
                    if ((i + err) > (SDL_VTM_NUM_OF_ADC_CODES-1))
                    {
                        continue;
                    }
                    derived_array[i] = gSDL_pvt_poly[i +err];
                }
            } else { /* Constant error take care of divide by zero */

                for (i = plus125cref; i < SDL_VTM_NUM_OF_ADC_CODES; i++) {
                    if ((i + plus125cerr) < 0)
                    {
                        continue;
                    }
                    derived_array[i] = gSDL_pvt_poly[i + plus125cerr];
                }
            }

            i = 0;
            /* Get to the first valid temperature */
            while (derived_array[i] == 0)
            {
                if ( i < (SDL_VTM_NUM_OF_ADC_CODES-2))
                {
                    i++;
                }
                else
                {
                    break;
                }
            }

            /* Get to the last zero index and back fill the temperature just for sake of continuity */
            if (i > 0)
            {
                /* 300 milli celsius steps */
                while(i > 0)
                {
                    derived_array[i] = derived_array[i + 1] - 300;
                    i--;
                }
                /* case 0 */
                derived_array[0] = derived_array[1] - 300;
            }

            /* Fill the last trailing 0s which are unfilled with increments of 100 milli celsius till (SDL_VTM_NUM_OF_ADC_CODES-1u) code */
            i = (SDL_VTM_NUM_OF_ADC_CODES-1);
            while (derived_array[i] == 0)
            {
                i--;
            }
            i++;
            inc = (int32_t)1;
            while (i < SDL_VTM_NUM_OF_ADC_CODES) {
                derived_array[i] = derived_array[i - 1] + (inc * 100);
                inc++;
                i++;
            }
        }
    }
#else
    if (gSDL_vtm_lut_done[sens_id] == FALSE)
    {
        for ( i = 0; i < SDL_VTM_NUM_OF_ADC_CODES; i++)
        {
            derived_array[i] = gSDL_pvt_poly_golden[i];
        }
    }
#endif

    gSDL_lut_computation_init_done = SDL_VTM_VALUES_ARE_INITIALIZED;
    gSDL_vtm_lut_done[sens_id] = TRUE;
    return;
}

 /**
 * Design: PROC_SDL-1322,PROC_SDL-1323
 */	
int32_t SDL_VTM_tsConvADCToTemp (SDL_VTM_adc_code       adc_code,
                                 SDL_VTM_InstTs		instance,
                                 int32_t                *p_milli_degree_temp_val)
{
    int32_t retVal = SDL_PASS;
    const SDL_VTM_cfg1Regs               *p_cfg1;
    p_cfg1 = (SDL_VTM_cfg1Regs *) SDL_VTM_CFG1_BASE;

    /* Argument check for temperature sensor */
    if (gNumTempSensors == SDL_VTM_VALUES_ARE_UNINITIALIZED)
    {
        SDL_VTM_getSensorVDCount(p_cfg1);
    }

    if ((int32_t)instance <= gNumTempSensors)
    {
        SDL_vtmPrepLookupTable();
    }

    if ((adc_code < (SDL_VTM_adc_code)0) ||
        (adc_code > (SDL_VTM_adc_code)(SDL_VTM_NUM_OF_ADC_CODES-1)))
    {
        retVal = SDL_EBADARGS;
    }

    if ((p_milli_degree_temp_val != NULL_PTR) &&	\
        (retVal                  == SDL_PASS))
    {
        /* for all temp sensors, use the sensor 0 table */
        *p_milli_degree_temp_val = gSDL_pvt_poly_work_around[0][adc_code];
    }
    else
    {
        retVal = SDL_EBADARGS;
    }

    return (retVal);
}

 /**
 * Design: PROC_SDL-1320,PROC_SDL-1321
 */		
int32_t SDL_VTM_tsConvTempToAdc (int32_t             milli_degree_temp_val,
								SDL_VTM_InstTs 		instance,
                                SDL_VTM_adc_code    *p_adc_code)

{
    int32_t             retVal;
    SDL_VTM_adc_code    low  = (SDL_VTM_adc_code)(0);
    SDL_VTM_adc_code    high = (SDL_VTM_adc_code)(SDL_VTM_NUM_OF_ADC_CODES-1);
    SDL_VTM_adc_code    mid;
	SDL_VTM_InstTs		ts_id;
    const SDL_VTM_cfg1Regs               *p_cfg1;
    p_cfg1 = (SDL_VTM_cfg1Regs *) SDL_VTM_CFG1_BASE;

    /* since pvt sensor 0 is used for all sensors, the input arg is not used */
    ts_id = SDL_VTM_INSTANCE_TS_0;

    if ((milli_degree_temp_val > SDL_VTM_TEMPERATURE_MILLI_DEGREE_C_MAX) ||
        (milli_degree_temp_val < SDL_VTM_TEMPERATURE_MILLI_DEGREE_C_MIN))
    {
        retVal = SDL_EBADARGS;
    }
    else
    {
        retVal = SDL_PASS;
    }

    /* Argument check for temperature sensor */
    if (gNumTempSensors == SDL_VTM_VALUES_ARE_UNINITIALIZED)
    {
        SDL_VTM_getSensorVDCount(p_cfg1); 
    }

    /* Check the temperature sensor ID out of range values */
    if ((int32_t)instance > gNumTempSensors)
    {
        retVal = SDL_EBADARGS;
    }

    if ( (p_adc_code     != NULL_PTR) &&	\
         (retVal         == SDL_PASS))
    {

        SDL_vtmPrepLookupTable();

        /* Binary search to find the adc code */
        while (low < (high - (SDL_VTM_adc_code)1)) {
            mid = (low + high) / 2;
            if (milli_degree_temp_val <= gSDL_pvt_poly_work_around[ts_id][mid])
            {
                high = mid;
                if (milli_degree_temp_val == gSDL_pvt_poly_work_around[ts_id][mid])
                {
                    break;
                }
            }
            else
            {
                low = mid;
            }
        }

        *p_adc_code =  mid;
    }
    else
    {
        retVal = SDL_EBADARGS;
    }

    return (retVal);
}

 /**
 * Design: PROC_SDL-1338,PROC_SDL-1339
 */
int32_t SDL_VTM_tsSetMaxTOutRgAlertThr( const SDL_VTM_cfg2Regs  *p_cfg2,
										SDL_VTM_InstTs 				instance,
                                       int32_t                 high_temp_in_milli_degree_celsius,
                                       int32_t                 low_temp_in_milli_degree_celsius)
{
    int32_t                 retVal = SDL_EBADARGS;
    volatile                int32_t i;
    SDL_VTM_adc_code        adc_code_h, adc_code_l;
    uint32_t                value;
    SDL_VTM_Ctrlcfg     ts_ctrl_cfg;

    if ((p_cfg2 != NULL_PTR))
    {
        retVal = SDL_VTM_tsConvTempToAdc(high_temp_in_milli_degree_celsius, SDL_VTM_INSTANCE_TS_0, &adc_code_h);
    }

    if (retVal == SDL_PASS)
    {
        retVal = SDL_VTM_tsConvTempToAdc(low_temp_in_milli_degree_celsius, SDL_VTM_INSTANCE_TS_0, &adc_code_l);
    }

    if (retVal == SDL_PASS)
    {
        /*
         * Program maximum temperature out of range thresholds
         * Step 1: set the thresholds to ~123C (sample value for high) and
         * 105C (sample value for low) WKUP_VTM_MISC_CTRL2
         * Step 2: WKUP_VTM_TMPSENS_CTRL_j set the MAXT_OUTRG_EN  bit This is already taken care as per of init
         * Step 3: WKUP_VTM_MISC_CTRL set the ANYMAXT_OUTRG_ALERT_EN  bit
         */

        /* Step 1 */
         ts_ctrl_cfg.valid_map = SDL_VTM_TS_CTRL_MAXT_OUTG_ALERT_VALID | \
                                 SDL_VTM_TS_CTRL_RESET_CTRL_VALID      | \
                                 SDL_VTM_TS_CTRL_SOC_VALID             | \
                                 SDL_VTM_TS_CTRL_MODE_VALID;
         value =  0u;
         SDL_REG32_FINS(&value, VTM_CFG2_MISC_CTRL2_MAXT_OUTRG_ALERT_THR0, adc_code_l);
         SDL_REG32_FINS(&value, VTM_CFG2_MISC_CTRL2_MAXT_OUTRG_ALERT_THR, adc_code_h);
         SDL_REG32_WR(&p_cfg2->MISC_CTRL2,value);

         /* Step 2 */
         ts_ctrl_cfg.valid_map = SDL_VTM_TS_CTRL_MAXT_OUTG_ALERT_VALID;
         ts_ctrl_cfg.maxt_outrg_alert_en = SDL_VTM_TS_CTRL_MAXT_OUTRG_GEN_ALERT;

         retVal = SDL_VTM_tsSetCtrl (p_cfg2,
                         instance,
                         &ts_ctrl_cfg);

         if (retVal == SDL_PASS)
         {
            /* have some delay before write */
            for (i = 0; i < SDL_VTM_REG_READ_DELAY;)
            {
                i = i + 1;
            }

            /* Step 3 */
            SDL_REG32_FINS(&p_cfg2->MISC_CTRL, \
                           VTM_CFG2_MISC_CTRL_ANY_MAXT_OUTRG_ALERT_EN, \
                           SDL_VTM_TSGLOBAL_ANY_MAXT_OUTRG_ALERT_ENABLE);
         }
    }
    else
    {
        retVal = SDL_EBADARGS;
    }

    return (retVal);

}

/* Nothing past this point */

