#ifndef APP_VISS_CFG_H_
#define APP_VISS_CFG_H_

Rfe_PwlConfig gPwlCfg[] =
{
    {
        4095,
        0,
        {-127, -127, -127, -127},
        {512, 512, 512, 512},
        UTRUE,
        512, 1408, 2176,
        2048, 16384, 65536,
        4, 16, 64, 512,
        0, 1048575
    },
    {
        4095,
        0,
        {-240, -240, -240, -240},
        {512, 512, 512, 512},
        UFALSE,
        512, 1024, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
    {
        65535,
        0,
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        UFALSE,
        512, 512, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 3 */
    {
        4095,
        0,
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        UFALSE,
        512, 1024, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
    /* 4 */
    {
        65535,
        0,
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        UFALSE,
        512, 1024, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
    /* 5 */
    {
        4095,
        0,
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        UFALSE,
        512, 1024, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
#endif
};

Rfe_WdrConfig gWdrCfg[] =
{
    /* 0 */
    {
        UFALSE, 0, 15, 4, 4,
        32768, 2048,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4094,
        0, 0, 0,
        0, 65535,
        0, 262143
    },
    /* 1 */
    {
        UFALSE, 0, 15, 8, 8,
        32768, 128,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        65504,
        0, 0, 0,
        0, 65535,
        0, 262143
    },
    /* 2 */
    {
        UFALSE, 0, 15, 3, 3,
        32768, 5851,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4956,
        0, 0, 0,
        0, 65535,
        0, 65535
    },
    /* 3 */
    {
        UTRUE, 0, 15, 6, 3,
        32768, 5461,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        19786,
        0, 0, 0,
        0, 65535,
        0, 1048575
    },
    /* 4 */
    {
        UTRUE, 0, 15, 3, 3,
        32768, 5851,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4956,
        0, 0, 0,
        0, 65535,
        0, 65535
    },
    /* 5 */
    {
        UTRUE, 0, 15, 6, 3,
        32768, 5461,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        19786,
        0, 0, 0,
        0, 65535,
        0, 1048575
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 6 */
    {
        UFALSE, 0, 15, 3, 3,
        32768, 5851,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4956,
        0, 0, 0,
        0, 65535,
        0, 65535
    },
    /* 7 */
    {
        UFALSE, 0, 15, 6, 3,
        32768, 5461,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        19786,
        0, 0, 0,
        0, 65535,
        0, 1048575
    }
#endif
};

#if defined (SOC_J721S2) || defined (SOC_J784S4)
uint32_t gPwlLut[][RFE_COMP_DECOMP_LUT_SIZE] =
{
    {
        #include "rawfe_pwl_vs_lut.txt"
    },
    {
        #include "rawfe_gr_pwl_vs_lut.txt"
    },
    {
        #include "rawfe_pwl_vs_lut_lca.txt"
    },
};

uint32_t gLut16To16[] = {
    #include "cfa_lut_16to_16.txt"
};
#endif

uint32_t gCompLut20To16[] = {
    #include "comp_lut_20_to_16_2.txt"
};
Vhwa_LutConfig gLutCfg[] =
{
    /* 0 */
    {
        UFALSE, 16, 4095, NULL
    },
    /* 1 */
    {
        UTRUE, 18, 65535, gCompLut20To16
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 2 */
    {
        UFALSE, 18, 65535, NULL
    },
    /* 3 - PWL Lut */
    {
        UTRUE, 12, 65535, gPwlLut[0]
    },
    /* 4 - PWL Lut */
    {
        UTRUE, 16, 65535, gPwlLut[1]
    },
    /* 5 */
    {
        UFALSE, 12, 65535, NULL
    },
    /* 6 - PWL Lut */
    {
        UTRUE, 16, 65535, gPwlLut[2]
    },
    /* 7 - CFA 16_to_16Lut */
    {
        UTRUE, 16, 65535, gLut16To16
    },
    /* 7 - CFA 16_to_16Lut */
    {
        UFALSE, 16, 65535, gLut16To16
    },
#endif
};

Rfe_DpcOtfConfig gDpcOtfCfg[] =
{
    {
        UTRUE,
        {200, 200, 300, 500, 800, 1600, 3200, 6400},
        {0, 50, 50, 37, 50, 50, 50, 50}
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 1 */
    {
        UFALSE,
        {200, 200, 300, 500, 800, 1600, 3200, 6400},
        {0, 50, 50, 37, 50, 50, 50, 50}
    },
    /* 2 */
    {
        UTRUE,
        {200, 200, 250, 350, 550, 950, 1750, 4000},
        {0, 25, 25, 37, 25, 25, 25, 0}
    }
#endif
};

uint32_t gLscLut_2[] =
{
    #include "lsc_lut_2.txt"
};

Rfe_LscConfig gLscCfg[] =
{
    {
        UFALSE,
        RFE_LSC_GAIN_FMT_U8Q8,
        RFE_LSC_DS_FACTOR_32,
        RFE_LSC_DS_FACTOR_16,
        NULL, 0
    },
    {
        UTRUE,
        RFE_LSC_GAIN_FMT_U8Q8_1,
        RFE_LSC_DS_FACTOR_64,
        RFE_LSC_DS_FACTOR_64,
        gLscLut_2, 2232
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    {
        UFALSE,
        RFE_LSC_GAIN_FMT_U8Q8_1,
        RFE_LSC_DS_FACTOR_64,
        RFE_LSC_DS_FACTOR_64,
        NULL, 0
    },
#endif
};

Rfe_GainOfstConfig  gWbCfg[] =
{
    /* WB CFG */
    {
        {512, 512, 512, 512},
        {0, 0, 0, 0}
    },
};
#if defined (SOC_J721S2) || defined (SOC_J784S4)
int32_t gCacLut[] =
{
    #include "cac_lut.txt"
};

Cac_Config          gCacCfg[] =
{
    /* 0 */
    {
        9, 36,
        {55, 37},
        gCacLut
    },
};
#endif

Fcp_CfaConfig       gCfaCfg[] =
{
    /* 0 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_colorb.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        }
    },
    /* 1 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_3m.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0, 1, 2, 3},
            {8, 9, 10, 11}
        },
        {
            {4, 5, 6, 7},
            {12, 13, 14, 15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        }
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 2 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_rccc_16.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE1,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012, FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012, FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012},
        {
            {175, 95, 95, 175},
            {175, 95, 95, 175}
        },
        {
            {175, 95, 95, 175},
            {175, 95, 95, 175}
        },
        {
            {15,15,15,15},
            {15,7,3,1}
        },
        {
            {8, 8, 8, 8},
            {8, 7, 7, 6}
        },
        {
            {1000, 2000, 4000, 8000, 12000, 24000, 32000},
            {700, 800, 900, 1200, 1400, 1500, 1600}
        },
        UTRUE, 16,
        UFALSE,
        {
            {256, 0, 0, 0, 0},
            {0, 256, 0, 0, 0},
            {0, 0,  256, 0, -16777216},
            {0, 0,  0, 256, 16777215}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 3 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_rccc_16.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE1,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012, FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012, FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012},
        {
            {175, 95, 95, 175},
            {175, 95, 95, 175}
        },
        {
            {175, 95, 95, 175},
            {175, 95, 95, 175}
        },
        {
            {15,15,15,15},
            {15,7,3,1}
        },
        {
            {8, 8, 8, 8},
            {8, 7, 7, 6}
        },
        {
            {1000, 2000, 4000, 8000, 12000, 24000, 32000},
            {700, 800, 900, 1200, 1400, 1500, 1600}
        },
        UTRUE, 16,
        UTRUE,
        {
            {256, 0, 0, 0, 0},
            {0, 256, 0, 0, 0},
            {0, 0,  256, 0, -16777216},
            {0, 0,  0, 256, 16777215}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 4 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_ar0233.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0,1,2,3},
            {8,9,10,11}
        },
        {
            {4,5,6,7},
            {12,13,14,15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        },
        UTRUE, 20,
        UTRUE,
        {
            {377, -80, -41, 0, 0},
            {-90, 316, 30, 0, 0},
            {-13, -198,  467, 0, 0},
            {0, 0,  0, 256, 0}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 5 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_ar0233.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0,1,2,3},
            {8,9,10,11}
        },
        {
            {4,5,6,7},
            {12,13,14,15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        },
        UTRUE, 24,
        UTRUE,
        {
            {392, -92, -44, 0, 0},
            {-80, 321, 15, 0, 0},
            {-6, -166,  428, 0, 0},
            {0, 0,  0, 256, 0}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 6 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_ar0233.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0,1,2,3},
            {8,9,10,11}
        },
        {
            {4,5,6,7},
            {12,13,14,15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        },
        UTRUE, 16,
        UFALSE,
        {
            {256, 0, 0, 0, 0},
            {0, 256, 0, 0, 0},
            {0, 0,  256, 0, 0},
            {0, 0,  0, 256, 0}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 7 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_rccc_16.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE1,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012, FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012, FCP_CFA_CORE_BLEND_MODE_ADAPT_INPUT012},
        {
            {175, 95, 95, 175},
            {175, 95, 95, 175}
        },
        {
            {175, 95, 95, 175},
            {175, 95, 95, 175}
        },
        {
            {15,15,15,15},
            {15,7,3,1}
        },
        {
            {8, 8, 8, 8},
            {8, 7, 7, 6}
        },
        {
            {1000, 2000, 4000, 8000, 12000, 24000, 32000},
            {700, 800, 900, 1200, 1400, 1500, 1600}
        },
        UTRUE, 16,
        UFALSE,
        {
            {256, 0, 0, 0, 0},
            {0, 256, 0, 0, 0},
            {0, 0,  256, 0, 0},
            {0, 0,  0, 256, 0}
        },
        {
            {UTRUE, 128, 16},
            {UTRUE, 256, 32},
            {UTRUE, 512, 48},
            {UTRUE, 1024, 64}
        }
    },
    /* 8 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lut_ar0233.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0,1,2,3},
            {8,9,10,11}
        },
        {
            {4,5,6,7},
            {12,13,14,15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        },
        UTRUE, 16,
        UTRUE,
        {
            {256, 0, 0, 0, 0},
            {0, 256, 0, 0, 0},
            {0, 0,  256, 0, -16777216},
            {0, 0,  0, 256, 16777215}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 9 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lca_lut_.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0,1,2,3},
            {8,9,10,11}
        },
        {
            {4,5,6,7},
            {12,13,14,15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        },
        UTRUE, 16,
        UTRUE,
        {
            {256, 0, 0, 0, 0},
            {0, 256, 0, 0, 0},
            {0, 0,  256, 0, 0},
            {0, 0,  0, 256, 0}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
    /* 10 */
    {
        {UFALSE, UFALSE, UFALSE, UFALSE},
        {
            #include "cfa_lca_lut_.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0,1,2,3},
            {8,9,10,11}
        },
        {
            {4,5,6,7},
            {12,13,14,15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        },
        UTRUE, 20,
        UTRUE,
        {
            {146, 0, 0, 0, 0},
            {-146, 307, -93, 0, 1984},
            {0, 0,  93, 0, 0},
            {0, 0,  0, 256, 0}
        },
        {
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0},
            {UTRUE, 256, 0}
        }
    },
#endif
};

Fcp_CcmConfig   gCcmCfg[] =
{
    /* 0 */
    {
        {
            {367, -111, 0, 0},
            {-53,  372, -63, 0},
            {-8,  -164, 428, 0}
        },
        {
            0, 0, 0
        }
    },
    /* 1 */
    {
        {
            {453, -166, -31, 0},
            {-149,  420, -15, 0},
            {17,  -327, 566, 0}
        },
        {
            0, 0, 0
        }
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 2 */
    {
        {
            {338,  -49, -33, 0},
            {-73,  379, -50, 0},
            {0,  -153, 409, 0}
        },
        {
            0,0,0
        }
    },
    /* 3 */
    {
        {
            {256,  0, 0, 0},
            {0, 256, 0, 0},
            {0, 0, 256, 0}
        },
        {
            0,0,0
        }
    },
    /* 4 */
    {
        {
            {392,  -92, -44, 0},
            {-80, 321, 15, 0},
            {-6, -166, 428, 0}
        },
        {
            0,0,0
        }
    },
    /* 5 */
    {
        {
            {322,  -34, -32, 0},
            {-78,  370, -36, 0},
            {-5,  -140, 400, 0}
        },
        {
            0,0,0
        }
    },
#endif
};

uint32_t gGammaTable[][FCP_GAMMA_LUT_SIZE] =
{
    {
        #include "saturation_lut.txt"
    },
    {
        #include "saturation_lut_3M.txt"
    },
    {
        #include "saturation_lut_lca.txt"
    },
};

Fcp_GammaConfig gGammaCfg[] =
{
    /* 0 */
    {
        UTRUE,
        10,
        gGammaTable[0],
        gGammaTable[0],
        gGammaTable[0]
    },
    /* 1 */
    {
        UTRUE,
        10,
        gGammaTable[1],
        gGammaTable[1],
        gGammaTable[1]
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 2 */
    {
        UFALSE,
        12,
        gGammaTable[0],
        gGammaTable[0],
        gGammaTable[0]
    },
    /* 3 */
    {
        UTRUE,
        12,
        gGammaTable[2],
        gGammaTable[2],
        gGammaTable[2]
    },
#endif
};

Fcp_Rgb2HsvConfig   gRgb2HsvCfg[] =
{
    {
        FCP_RGB2HSV_INPUT_CONTRAST_OUTPUT,
        FCP_RGB2HSV_H1_INPUT_RED_COLOR,
        FCP_RGB2HSV_H1_INPUT_RED_COLOR,
        {64, 64, 128}, 0,
        0,
        {0, 0, 0},
        {0, 0, 0},
        0,
        FCP_SAT_MODE_SUM_RGB_MINUS_MIN_RGB,
        FCP_SAT_DIV_4096_MINUS_GREY
    },
    /* 1 */
    {
        FCP_RGB2HSV_INPUT_CONTRAST_OUTPUT,
        FCP_RGB2HSV_H1_INPUT_RED_COLOR,
        FCP_RGB2HSV_H2_INPUT_BLUE_COLOR,
        {64, 64, 128}, 0,
        0,
        {0, 0, 0},
        {0, 0, 0},
        0,
        FCP_SAT_MODE_SUM_RGB_MINUS_MIN_RGB,
        FCP_SAT_DIV_4096_MINUS_GREY
    },
};

Fcp_Rgb2YuvConfig   gRgb2YuvCfg[] =
{
    {
        {
            {77, 150, 29},
            {-44, -84, 128},
            {128, -108, -20}
        },
        {
            0, 128, 128
        }
    },
    /* RGB2YUV */
    {
        {
            {77, 150, 29},
            {-43, -85, 128},
            {128, -107, -21}
        },
        {
            0, 128, 128
        }
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* RGB2YUV */
    {
        {
            {77, 150, 29},
            {-43, -85, 128},
            {128, -107, -21}
        },
        {
            0, 512, 512
        }
    },
    /* 3 */
    {
        {
            {256, 0, 0},
            {0, -353, 353},
            {353, -353, 0}
        },
        {
            0, 512, 512
        }
    },
#endif
};

Fcp_YuvSatLutConfig     gRgbLutCfg[] =
{
    {
        10,
        UFALSE, NULL,
        UFALSE, NULL,
        UFALSE, NULL
    },
    {
        12,
        UFALSE, NULL,
        UFALSE, NULL,
        UFALSE, NULL
    },
};

#if defined (SOC_J721S2) || defined (SOC_J784S4)
uint32_t gCfaComLut[][FCP_COMPANDING_LUT_SIZE] =
{
    {
        #include "cfa_lut_compd_20b.txt"
    },
    {
        #include "cfa_lut_compd_24b.txt"
    },
    {
        #include "cfa_lut_compd_16b.txt"
    },
    {
        #include "cfa_lut_compd_lca.txt"
    },
    /* 4*/
    {
        #include "cfa_lut_lca_mv_compd0.txt"
    },
    {
        #include "cfa_lut_lca_mv_compd1.txt"
    },

};

uint32_t gCfaDecomLut[][FCP_COMPANDING_LUT_SIZE] =
{
    {
        #include "cfa_lut_dcmpd_20b.txt"
    },
    {
        #include "cfa_lut_dcmpd_24b.txt"
    },
    {
        #include "cfa_lut_dcmpd_lca.txt"
    },
    /* 3 */
    {
        #include "cfa_lut_lca_mv_dcmpd.txt"
    },
};

Fcp_comDecomLutConfig gCfaComDecomLut[] =
{
    /* Companding 20b */
    /* 0 */
    {
        UTRUE,
        {
            gCfaComLut[0],
            gCfaComLut[0],
            gCfaComLut[0],
            gCfaComLut[0]
        }
    },
    /* Decompanding 20b */
    /* 1 */
    {
        UTRUE,
        {
            gCfaDecomLut[0],
            gCfaDecomLut[0],
            gCfaDecomLut[0],
            gCfaDecomLut[0]
        }
    },
    /* Companding 24b */
    /* 2 */
    {
        UTRUE,
        {
            gCfaComLut[1],
            gCfaComLut[1],
            gCfaComLut[1],
            gCfaComLut[1]
        }
    },
    /* Decompanding 24b */
    /* 3 */
    {
        UTRUE,
        {
            gCfaDecomLut[1],
            gCfaDecomLut[1],
            gCfaDecomLut[1],
            gCfaDecomLut[1]
        }
    },
    /* Companding 16b */
    /* 4 */
    {
        UTRUE,
        {
            gCfaComLut[2],
            gCfaComLut[2],
            gCfaComLut[2],
            gCfaComLut[2]
        }
    },
    /* 5 Disable */
    {
        UFALSE,
        {
            NULL,
            NULL,
            NULL,
            NULL
        }
    },
    /* Companding 16b LCA */
    /* 6 */
    {
        UTRUE,
        {
            gCfaComLut[3],
            gCfaComLut[3],
            gCfaComLut[3],
            gCfaComLut[3]
        }
    },
    /* Decompanding 16b LCA */
    /* 7 */
    {
        UTRUE,
        {
            gCfaDecomLut[2],
            gCfaDecomLut[2],
            gCfaDecomLut[2],
            gCfaDecomLut[2]
        }
    },
    /* Companding LCA MV */
    /* 8 */
    {
        UTRUE,
        {
            gCfaComLut[4],
            gCfaComLut[5],
            gCfaComLut[4],
            gCfaComLut[4]
        }
    },
    /* Decompanding 16b LCA */
    /* 9 */
    {
        UTRUE,
        {
            gCfaDecomLut[3],
            gCfaDecomLut[3],
            gCfaDecomLut[3],
            gCfaDecomLut[3]
        }
    },
};

uint16_t gNsf4HistLut [NSF4_HISTOGRAM_LUT_SIZE] =
{
    #include "nsf4_hist_lut.txt"
};
#endif

uint32_t gH3aLut[] =
{
    #include "h3a_16_to_10_lut.txt"
};

uint32_t gH3aLutrccb[] =
{
    #include "lut_16_10_h3a_rccb.txt"
};

Glbce_PerceptConfig fwdPrcptCfg[] =
{
    {
        UTRUE,
        {
            #include "glbce_fwd_percept_lut.txt"
        }
    }
};
Glbce_PerceptConfig revPrcptCfg[] =
{
    {
        UTRUE,
        {
            #include "glbce_rev_percept_lut.txt"
        }
    }
};
Glbce_Config  glbceCfg[] =
{
    /* 0 */
    {
        255,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0x6,
        0x6,
        GLBCE_NO_DITHER,
        72,
        62,
        {
            #include "glbce_asymmetry_lut.txt"
        }
    },
    /* 1 */
    {
        250,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        64,
        {
            #include "glbce_asymmetry_lut_150.txt"
        }
    },
    /* 2 */
    {
        250,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        64,
        {
            #include "glbce_asymmetry_lut_130.txt"
        }
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 3 */
    {
        0,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        64,
        {
            #include "glbce_asymmetry_lut.txt"
        }
    },
    /* 4 */
    {
        0,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        62,
        {
            #include "glbce_asymmetry_lut_20.txt"
        }
    },
    /* 5 */
    {
        255,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        62,
        {
            #include "glbce_asymmetry_lut_20.txt"
        }
    },
#endif
};

Nsf4v_Config nsf4Cfg[] =
{
    /* 0 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {660, 512, 512, 762},
        32,
        {64, 32, 16},
        {
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            },
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            },
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            },
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            }
        }
    },
    /* 1 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10u,
        {540, 512, 512, 1143},
        2,
        {64, 32, 16},
        {
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            },
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            },
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            },
            {
                {0   ,    16,    128},
                {64  ,    20,    192},
                {256 ,    38,    100},
                {1024,    76,     52},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0},
                {4096,     0,      0}
            }
        }
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 2 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0xFu,
        {512, 512, 512, 512},
        32,
        {64, 32, 16},
        {
            {
                {0,     0,    0},
                {64,    0,    0},
                {256,   0,    0},
                {1024,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0}
            },
            {
                {0,     0,    0},
                {64,    0,    0},
                {256,   0,    0},
                {1024,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0}
            },
            {
                {0,     0,    0},
                {64,    0,    0},
                {256,   0,    0},
                {1024,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0}
            },
            {
                {0,     0,    0},
                {64,    0,    0},
                {256,   0,    0},
                {1024,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0},
                {4096,  0,    0}
            },
        }
    },
    /* 3 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {512, 523, 703, 512},
        32,
        {64, 32, 16},
        {
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
            {
                {0    , 20 , 16 },
                {3550 , 48 , 374},
                {4032 , 136, 104},
                {4900 , 180, 14 },
                {23000, 300, 819},
                {24000, 700, 12 },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  }
            },
            {
                {0    , 33  , 16  },
                {3550 , 61  , 506 },
                {4032 , 180 , 94  },
                {4900 , 220 , 20  },
                {23000, 400 , 1126},
                {65535, 950 , 22  },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   }
            },
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
        }
    },
    /* 4 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {512, 548, 649, 512},
        32,
        {64, 32, 16},
        {
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
            {
                {0    , 20 , 16 },
                {3550 , 48 , 374},
                {4032 , 136, 104},
                {4900 , 180, 14 },
                {23000, 300, 819},
                {24000, 700, 12 },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  }
            },
            {
                {0    , 33  , 16  },
                {3550 , 61  , 506 },
                {4032 , 180 , 94  },
                {4900 , 220 , 20  },
                {23000, 400 , 1126},
                {65535, 950 , 22  },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   }
            },
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
        }
    },
    /* 5 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {516, 512, 707, 516},
        32,
        {64, 32, 16},
        {
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
            {
                {0    , 20 , 16 },
                {3550 , 48 , 374},
                {4032 , 136, 104},
                {4900 , 180, 14 },
                {23000, 300, 819},
                {24000, 700, 12 },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  }
            },
            {
                {0    , 33  , 16  },
                {3550 , 61  , 506 },
                {4032 , 180 , 94  },
                {4900 , 220 , 20  },
                {23000, 400 , 1126},
                {65535, 950 , 22  },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   }
            },
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
        }
    },
    /* 6 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {512, 547, 658, 512},
        32,
        {64, 32, 16},
        {
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
            {
                {0    , 20 , 16 },
                {3550 , 48 , 374},
                {4032 , 136, 104},
                {4900 , 180, 14 },
                {23000, 300, 819},
                {24000, 700, 12 },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
            },
            {
                {0    , 33  , 16  },
                {3550 , 61  , 506 },
                {4032 , 180 , 94  },
                {4900 , 220 , 20  },
                {23000, 400 , 1126},
                {65535, 950 , 22  },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   }
            },
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
        }
    },
    /* 7 - Histogram without LUT */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {512, 523, 703, 512},
        32,
        {64, 32, 16},
        {
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
            {
                {0    , 20 , 16 },
                {3550 , 48 , 374},
                {4032 , 136, 104},
                {4900 , 180, 14 },
                {23000, 300, 819},
                {24000, 700, 12 },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  }
            },
            {
                {0    , 33  , 16  },
                {3550 , 61  , 506 },
                {4032 , 180 , 94  },
                {4900 , 220 , 20  },
                {23000, 400 , 1126},
                {65535, 950 , 22  },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   }
            },
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
        },
        {
            UFALSE   /* DWB */
        },
        {
            UTRUE,   /* Histogram */
            16, 4,
            {
                {
                    UTRUE,
                    {0, 2},
                    {799, 127},
                },
                {
                    UTRUE,
                    {10, 128},
                    {959, 255},
                },
                {
                    UTRUE,
                    {50, 300},
                    {1899, 599},
                },
                {
                    UTRUE,
                    {1000, 600},
                    {1349, 799},
                },
                {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}
            },
            {
                UFALSE, gNsf4HistLut
            }
        }

    },
    /* 8 - Histogram With LUT */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {512, 523, 703, 512},
        32,
        {64, 32, 16},
        {
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
            {
                {0    , 20 , 16 },
                {3550 , 48 , 374},
                {4032 , 136, 104},
                {4900 , 180, 14 },
                {23000, 300, 819},
                {24000, 700, 12 },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  },
                {65535, 950, 0  }
            },
            {
                {0    , 33  , 16  },
                {3550 , 61  , 506 },
                {4032 , 180 , 94  },
                {4900 , 220 , 20  },
                {23000, 400 , 1126},
                {65535, 950 , 22  },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   },
                {65535, 1400, 0   }
            },
            {
                {0    , 14 , 13 },
                {3550 , 37 , 353},
                {4032 , 120, 47 },
                {4900 , 140, 12 },
                {23000, 250, 512},
                {24000, 500, 5  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  },
                {65535, 600, 0  }
            },
        },
        {
            UFALSE   /* DWB */
        },
        {
            UTRUE,   /* Histogram */
            16, 2,
            {
                {
                    UTRUE,
                    {0, 2},
                    {799, 127},
                },
                {
                    UTRUE,
                    {10, 128},
                    {959, 255},
                },
                {
                    UTRUE,
                    {50, 300},
                    {1899, 599},
                },
                {
                    UTRUE,
                    {1000, 600},
                    {1349, 799},
                },
                {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}
            },
            {
                UTRUE, gNsf4HistLut
            }
        }

    },
    /* 9 */
    {
        {
            UFALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {512, 512, 512, 512},
        32,
        {64, 32, 16},
        {
            {
                {0    , 18 , 0  },
                {350  , 18 , 19 },
                {1000 , 24 , 27 },
                {3000 , 50 , 20 },
                {6000 , 80 , 2  },
                {65529, 130, 0  },
                {65530, 130, 0  },
                {65531, 130, 0  },
                {65532, 130, 0  },
                {65533, 130, 0  },
                {65534, 130, 0  },
                {65535, 130, 0  }
            },
            {
                {0    , 20 , 0  },
                {350  , 20 , 47 },
                {1000 , 35 , 36 },
                {3000 , 70 , 20 },
                {6000 , 100, 2  },
                {65529, 150, 0  },
                {65530, 150, 0  },
                {65531, 150, 0  },
                {65532, 150, 0  },
                {65533, 150, 0  },
                {65534, 150, 0  },
                {65535, 150, 0  }
            },
            {
                {0    , 24 , 0  },
                {350  , 24 , 50 },
                {1000 , 40 , 51 },
                {3000 , 90 , 41 },
                {6000 , 150, 2  },
                {65529, 200, 0  },
                {65530, 200, 0  },
                {65531, 200, 0  },
                {65532, 200, 0  },
                {65533, 200, 0  },
                {65534, 200, 0  },
                {65535, 200, 0  }
            },
            {
                {0    , 18 , 0  },
                {350  , 18 , 19 },
                {1000 , 24 , 27 },
                {3000 , 50 , 20 },
                {6000 , 80 , 2  },
                {65529, 130, 0  },
                {65530, 130, 0  },
                {65531, 130, 0  },
                {65532, 130, 0  },
                {65533, 130, 0  },
                {65534, 130, 0  },
                {65535, 130, 0  }
            },
        },
        /* DWB */
        {
            UTRUE,
            {
                {
                   {0    ,  256,    0},
                   {8000 ,  256,    0},
                   {12000,  256,    0},
                   {19000,  256,    0},
                   {21000,  256,    0},
                   {25000,  256,    0},
                   {40000,  256,    0},
                   {65535,  256,    0}
                },
                {
                    {0    ,  260,    0},
                    {8000 ,  260,    0},
                    {12000,  260,    0},
                    {19000,  260,    0},
                    {21000,  258,    0},
                    {25000,  258,    0},
                    {40000,  258,    0},
                    {65535,  258,    0}
                },
                {
                    {0    ,  258,    0},
                    {8000 ,  258,    0},
                    {12000,  258,    0},
                    {19000,  258,    0},
                    {21000,  260,    0},
                    {25000,  260,    0},
                    {40000,  260,    0},
                    {65535,  260,    0}
                },
                {
                    {0    ,  256,    0},
                    {8000 ,  256,    0},
                    {12000,  256,    0},
                    {19000,  256,    0},
                    {21000,  256,    0},
                    {25000,  256,    0},
                    {40000,  256,    0},
                    {65535,  256,    0}
                },
            },
            {
                {0,0,0,256,0,0},
                {0,0,256,0,0,0},
            }
        },
    },
#endif
};

Rfe_H3aInConfig gRfeH3aInCfg[] =
{
    {
        RFE_H3A_IN_SEL_LSC_OUT_FRAME,       /* Input Source */
        0                                   /* Shift */
    },
    {
        RFE_H3A_IN_SEL_LSC_OUT_FRAME,       /* Input Source */
        1                                   /* Shift */
    }
};

Vhwa_LutConfig gRfeH3aLutCfg[] =
{
    /* 1 */
    {
        1, 16, 0x3FF,
        gH3aLut
    },
    /* 2 */
    {
        1, 16, 0x3FF,
        gH3aLutrccb
    },
};

H3a_Config gH3aCfg[] =
{
    {
        H3A_MODULE_AEWB,
        // Position
        {
            0, 0,
        },
        // AEWB Config
        {
            UFALSE, // ALAW
            UFALSE, // MED Filter
            0x0,   // MEd Filter Threshold
            // Win Config
            {
                {
                    0, 30   // position
                },
                120, 32,  //size
                16, 32,  //Count
                8, 8,    // Incr
            },
            1078, 2,      // Black line start
            H3A_AEWB_OUTPUT_MODE_SUM_ONLY,   // mode
            0,          // sumshift
            1023        // sat limit
        },
        // AF Config
        {
        }
    },
    {
        H3A_MODULE_AF,
        // Position
        {
            0, 0,
        },
        // AEWB Config
        {
            UFALSE, // ALAW
            UFALSE, // MED Filter
            0x0,   // MEd Filter Threshold
            // Win Config
            {
                {
                    0, 30   // position
                },
                120, 32,  //size
                16, 32,  //Count
                8, 8,    // Incr
            },
            1078, 2,      // Black line start
            H3A_AEWB_OUTPUT_MODE_SUM_ONLY,   // mode
            0,          // sumshift
            1023       // sat limit
        },
        // AF Config
        {
            UFALSE, /* enableALowCompr */
            UTRUE, /*enableMedFilt */
            100,  /* midFiltThreshold*/
            H3A_AF_RGBPOS_GR_BG,  /* rgbPos */
            {
                /* vpsissH3aPaxelConfig_t*/
                {128u + 0, 16u + 1u}, /* Fvid2_PosConfig pos */
                (((1280u - 2u*128u)/3u) + 7u) & (~7u), /* width, rounded up to next multiple of 8*/
                (((720u - 2u*16u)/3u) + 7u) & (~7u), /* height, rounded up to next multiple of 8 */
                3u, /* horzCount */
                3u, /* vertCount*/
                8u, /* horzIncr */
                8u, /* vertIncr */
            },
            H3A_AF_FV_MODE_SUM, /* fvMode */
            H3A_AF_VF_VERT_HORZ, /* vfMode */
            { /* iirCfg1 */
                {8, -95, -49, 38, 76, 38, -111, -54, 17, -34, 17}, /* coeff */
                0 /* threshold */
            },
            { /* iirCfg2 */
                {11, -72, -50, 26, 51, 26, -92, -53, 19, -38, 19}, /* coeff */
                0
            },
            { /* firCfg1 */
                {3, -6, 0, 6, -3}, /* coeff */
                0 /* threshold */
            },
            { /* firCfg2 */
                {3, -6, 0, 6, -3}, /* coeff */
                0 /* threshold */
            },
            0 /* iirFiltStartPos */
        }
    },
    /* New Config */
    /* 2 */
    {
        H3A_MODULE_AEWB,
        // Position
        {
            0, 0,
        },
        // AEWB Config
        {
            UFALSE, // ALAW
            UFALSE, // MED Filter
            0x0,   // MEd Filter Threshold
            // Win Config
            {
                {
                    32, 2   // position
                },
                14, 20,  //size
                32, 16,  //Count
                4, 4,    // Incr
            },
            350, 6,      // Black line start
            H3A_AEWB_OUTPUT_MODE_SUM_ONLY,   // mode
            0,          // sumshift
            1000        // sat limit
        },
        // AF Config
        {
        }
    },
};

int16_t gYeeLut[] = {
    #include "yee_lut.txt"
};

int16_t gYeeLutLca[] = {
    #include "yee_lut_lca.txt"
};

Fcp_EeConfig gEeCfg[] =
{
    {
        UTRUE,
        UFALSE,  // align for Y12
        UFALSE,  // align for Y8
        0,      //Y1212
        UFALSE,   // bypass
        UTRUE,   // bypass
        UTRUE,   // bypass
        UTRUE,   // bypass

        0,      // left shift
        0,      // right shift

        4,      // yee shift
        {
            -1, -3, -5, -3, -2, 2, -5, 2, 48
        },
        0,      // y Thr
        0,      // merge sel
        1,      // Halo reduction
        0,
        0,
        0,
        0,
        0,
        gYeeLut
    },
    /* 1 */
    {
        UTRUE,
        UFALSE,  // align for Y12
        UFALSE,  // align for Y8
        0,      //Y1212
        UFALSE,   // bypass
        UTRUE,   // bypass
        UTRUE,   // bypass
        UTRUE,   // bypass

        0,      // left shift
        0,      // right shift

        4,      // yee shift
        {
            -2, -6, -10, -6, 0, 12, -10, 12, 48
        },
        320,      // y Thr
        0,      // merge sel
        1,      // Halo reduction
        96,
        0,
        3200,
        400,
        384,
        gYeeLutLca
    },
};

Fcp_HistConfig gHistCfg[] = {
    {
        UTRUE, FCP_HIST_IN_SEL_COLOR_RED,
        {0U, 0U, 1280U, 720U}
    }
};

AppViss_Cfg gVissCfg[] =
{
    /* 0, Used for Single Frame Input 720p StartColor B */
    {
        &gPwlCfg[0],        /* PWL VS */
        &gPwlCfg[0],        /* PWL S */
        &gPwlCfg[0],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[0],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[0],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */
        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[0],        /* CFA */

        &gCcmCfg[0],        /* CCM */

        &gGammaCfg[0],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[0],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[0],        /* NSF4 */
        &glbceCfg[0],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[1],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 1, Two Frame Merge */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[2],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[2],        /* WDR1 */
        &gWdrCfg[3],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[1],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 2, Three Frame Merge with AEWB */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[1],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[4],        /* WDR1 */
        &gWdrCfg[5],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[2],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[0],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 3, Three Frame Merge with AF */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[1],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[4],        /* WDR1 */
        &gWdrCfg[5],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[2],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[1],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 4, Two Frame Merge */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[2],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[2],        /* WDR1 */
        &gWdrCfg[3],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[1],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        &gEeCfg[0],         /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 5, Used for Single Frame Input 720p StartColor B, histogram enabled */
    {
        &gPwlCfg[0],        /* PWL VS */
        &gPwlCfg[0],        /* PWL S */
        &gPwlCfg[0],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[0],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[0],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[0],        /* CFA */

        &gCcmCfg[0],        /* CCM */

        &gGammaCfg[0],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[0],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[0],        /* NSF4 */
        &glbceCfg[0],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[1],        /* H3A */
        NULL,               /* EE */
        &gHistCfg[0],       /* Hist Cfg*/
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)
    /* 6, Used for Single Frame Input 1080p rccb, 16b CFA pipe, histogram enabled */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
    },
    /* 7, Used for Single Frame Input 1080p rccb, 16b CFA pipe, CCM enabled */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
    },
    /* 8, Used for Single Frame Input 1920x1280 rccb, 16b CFA pipe, 20b CCM enabled */
    {
        &gPwlCfg[5],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[3],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */   /* TODO - Check Config */

        &gCfaCfg[4],        /* CFA */

        &gCcmCfg[3],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[3],        /* NSF4 */
        &glbceCfg[4],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[0],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[1],               /* 16 to 24 LUT config */
    },
    /* 9, Used for Single Frame Input 1920x1280 rccb, 16b CFA pipe, 24b CCM enabled */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[4],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[2],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[5],        /* CFA */

        &gCcmCfg[3],        /* CCM */

        &gGammaCfg[2],      /* Gamma */         /* Check with changing to UTRUE */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[2],    /* RGB2YUV */

        &gRgbLutCfg[1],     /* RGB Lut */

        &nsf4Cfg[4],        /* NSF4 */
        &glbceCfg[4],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[2], /* 24 to 12 LUT config */
        &gCfaComDecomLut[3], /* 16 to 24 LUT config */
    },
    /* 10, Used for Single Frame Input 8MP rccb, 16b CFA pipe, 24b CCM enabled */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[4],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[2],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[5],        /* CFA */

        &gCcmCfg[3],        /* CCM */

        &gGammaCfg[2],      /* Gamma */         /* Check with changing to UTRUE */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[2],    /* RGB2YUV */

        &gRgbLutCfg[1],     /* RGB Lut */

        &nsf4Cfg[5],        /* NSF4 */
        &glbceCfg[4],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[2], /* 24 to 12 LUT config */
        &gCfaComDecomLut[3], /* 16 to 24 LUT config */
    },
    /* 11, Used for Single Frame Input 1920x1280 rccb, 16b CFA pipe, CCM disable, Comp enabled */
    {
        &gPwlCfg[5],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[3],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */   /* TODO - Check Config */

        &gCfaCfg[6],        /* CFA */

        &gCcmCfg[4],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[6],        /* NSF4 */
        &glbceCfg[5],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[4],               /* 24 to 12 Comp LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 Decom LUT config */
    },
    /* 12, Used for Single Frame Input 1920x1280 rccb, 16b CFA pipe, 20b CCM enabled, NSF4 Hist without LUT */
    {
        &gPwlCfg[5],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[3],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */   /* TODO - Check Config */

        &gCfaCfg[4],        /* CFA */

        &gCcmCfg[3],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[7],        /* NSF4 */
        &glbceCfg[4],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[0],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[1],               /* 16 to 24 LUT config */
    },
    /* 13, Used for Single Frame Input 1920x1280 rccb, 16b CFA pipe, 20b CCM enabled, NSF4 Hist with LUT */
    {
        &gPwlCfg[5],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[3],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */   /* TODO - Check Config */

        &gCfaCfg[4],        /* CFA */

        &gCcmCfg[3],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[8],        /* NSF4 */
        &glbceCfg[4],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[0],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[1],               /* 16 to 24 LUT config */
    },
    /* 14, Used for Single Frame Input 1080p rccb, 16b CFA pipe, FIR enabled */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[7],        /* CFA */

        &gCcmCfg[5],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
    },
    /* 15 (copy of 7), Used for Single Frame Input 1080p*/
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 16 (copy of 7), Used for Single Frame Input 1080p, EE on HV */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        &gEeCfg[0],         /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 17 (copy of 7), Used for Single Frame Input 1080p, EE on MV */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        &gEeCfg[0],         /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 18 (copy of 7), Used for Single Frame Input 1080p, EE on MV and HV */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[3],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        &gEeCfg[0],         /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        &gEeCfg[0],         /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 19 (copy of 11), Used for Single Frame Input 1920x1280 rccb, HV + MV */
    {
        &gPwlCfg[5],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[3],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[6],        /* CFA */

        &gCcmCfg[4],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[6],        /* NSF4 */
        &glbceCfg[5],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        NULL,               /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[4],               /* 24 to 12 Comp LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 Decom LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[8],        /* CFA */
        &gCfaComDecomLut[4],               /* 24 to 12 Comp LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 Decom LUT config */

        &gCcmCfg[4],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 20 (copy of 7), Used for Single Frame Input 1080p, GLBC bypass*/
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[0],       /* GLBCE */
        NULL,    /* Glbce_fwdPercept */
        NULL,    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 21 (copy of 7), Used for Single Frame Input 1080p, GLBC and NSF4 bypass*/
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[1],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        NULL,        /* NSF4 */
        NULL,       /* GLBCE */
        NULL,    /* Glbce_fwdPercept */
        NULL,    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */
        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 22, Single Frame Input 1920x1280 , CAC checkout */
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[6],        /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[5],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[2],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[7],        /* 16 to 12 LUT */

        &gCfaCfg[9],        /* CFA */
        &gCcmCfg[4],        /* CCM */

        &gGammaCfg[3],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[2],    /* RGB2YUV */

        &gRgbLutCfg[1],     /* RGB Lut */

        &nsf4Cfg[9],        /* NSF4 */                      ////
        &glbceCfg[5],       /* GLBCE */                 /* TODO check with Asym lut 5 */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        &gEeCfg[1],         /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[6], /* 24 to 12 LUT config */
        &gCfaComDecomLut[7], /* 16 to 24 LUT config */
        /* FCP2 Config - start */
        &gLutCfg[7],//&gLutCfg[8],        /* 16 to 12 LUT */

        &gCfaCfg[9],//&gCfaCfg[10],        /* CFA */
        &gCfaComDecomLut[6],//&gCfaComDecomLut[8], /* 24 to 12 LUT config */
        &gCfaComDecomLut[7],//&gCfaComDecomLut[9], /* 16 to 24 LUT config */

        &gCcmCfg[4],//&gCcmCfg[3],        /* CCM */

        &gGammaCfg[2],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[2],//&gRgb2YuvCfg[3],    /* RGB2YUV */

        &gRgbLutCfg[1],     /* RGB Lut */

        &gEeCfg[1],         /* EE */
        NULL,               /* Hist Cfg*/
        /* FCP2 Config - End */

        /* CAC config */
        &gCacCfg[0],
    },
    /* 23 (copy of 20), Used for Single Frame Input 1080p, GLBC bypass, CAC checkout*/
    {
        &gPwlCfg[4],        /* PWL VS */
        &gPwlCfg[3],        /* PWL S */
        &gPwlCfg[3],        /* PWL L */
        &gLutCfg[5],               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[2],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[2],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[1],   /* RFE H3A Config */
        &gRfeH3aLutCfg[1],  /* H3a LUT */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[3],        /* CFA */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[2],        /* NSF4 */
        &glbceCfg[0],       /* GLBCE */
        NULL,    /* Glbce_fwdPercept */
        NULL,    /* Glbce_revPercept */
        &gH3aCfg[2],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
        UFALSE,              /* isCrcAvail */
        {0},

        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */
        /* FCP2 Config */
        &gLutCfg[5],               /* 16 to 12 LUT */

        &gCfaCfg[2],        /* CFA */
        &gCfaComDecomLut[5],               /* 24 to 12 LUT config */
        &gCfaComDecomLut[5],               /* 16 to 24 LUT config */

        &gCcmCfg[2],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[1],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/

        /* CAC config */
        &gCacCfg[0],
    },
#endif
};

AppViss_TestConfig gAppVissTestConfig[] =
{
    /* 0, Used for Single Frame Input 720p StartColor B */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280*2, 1280*2, 1280*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[0]
    },
    /* 1, Two Frame Merge wth two YUV420 and saturation output */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[1]
    },
    /* 2, Three Frame Merge wth two YUV420, saturation and AEWB output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[2]
    },
    /* 3, Three Frame Merge wth two YUV420, saturation and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[3]
    },
    /* 4, Three Frame Merge wth YUV420, RGB and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RGB24_888_PLANAR, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[3]
    },
    /* 5, Three Frame Merge wth YUV420, HSV and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_GREY, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
            &gVissCfg[3]
    },
    /* 6, Two Frame Merge with two YUV420 and saturation output, EE enabled on Y12 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_ON_LUMA12,     /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[4]
    },
    /* 7, Two Frame Merge with two YUV420 and saturation output, EE enabled on Y8 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_ON_LUMA8,      /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
            &gVissCfg[4]
    },
    /* 8, Three Frame Merge with YUV420, YUV422(UYVY) and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV422I_UYVY, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
            &gVissCfg[3]
    },
    /* 9, Three Frame Merge with YUV420, YUV422(YUYV) and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV422I_YUYV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[3]
    },
    /* 10, Used for Single Frame Input 720p StartColor, with histogram output */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280*2, 1280*2, 1280*2}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UTRUE,
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[5]
    },
    /* 11, Used for Single Frame Input 720p StartColor B */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE                            /* Enable DPC */
        },
        /* VISS config */
        &gVissCfg[0]
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)    
    /* 12 , MV pipeline Chroma Only Output */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {           
                    /* Output Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 0, 0}, {UFALSE},
                        FVID2_DF_CHROMA_ONLY, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    UFALSE,
                    {
                    }
                },
                {
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
            0u                              /* Chroma mode */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 13 , HV-MV pipeline Output RawFE Input; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 14 , HV-MV pipeline Output NSF4 Input; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 15 , HV-MV pipeline Output CAC Input; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 16 , HV-MV pipeline Output GLBC Input; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 17 , HV-MV pipeline Output RawFE Input; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 18 , HV-MV pipeline Output NSF4 Input; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 19 , HV-MV pipeline Output CAC Input; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 20 , HV-MV pipeline Output GLBC Input; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 21 , HV-MV pipeline Output RawFE Input; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 22 , HV-MV pipeline Output NSF4 Input; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 23 , HV-MV pipeline Output CAC Input; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 24 , HV-MV pipeline Output GLBC Input; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 25 , HV-MV pipeline Output RawFE Input; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 26 , HV-MV pipeline Output NSF4 Input; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 27 , HV-MV pipeline Output CAC Input; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 28 , HV-MV pipeline Output GLBC Input; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 29 , HV-MV pipeline Output RawFE Input; Q-VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 30 , HV-MV pipeline Output NSF4 Input; Q-VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 31 , HV-MV pipeline Output CAC Input; Q-VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 32 , HV-MV pipeline Output GLBC Input; Q-VGA | Doesn't work*/
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 33 , HV-MV pipeline Output RawFE Input; 4K 4096x8192 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 5120, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 5120, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UFALSE,
                    /* Output Format */
                    // {
                    //     0, 4096, 4096, {4096, 4096, 4096}, {UFALSE},
                    //     FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                    //     FVID2_CCSF_BITS8_PACKED
                    // },
                    // VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UFALSE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 34 , HV-MV pipeline Output NSF4 Input; 4K 4096x8192 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 4096, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 4096, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 4096, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 35 , HV-MV pipeline Output CAC Input; 4K 4096x8192 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 4096, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 4096, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 4096, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 36 , HV-MV pipeline Output GLBC Input; 4K  4096x8192*/
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 5120, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 5120, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UFALSE,
                    /* Output Format */
                    // {
                    //     0, 4096, 5120, {4096, 4096, 4096}, {UFALSE},
                    //     FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                    //     FVID2_CCSF_BITS8_PACKED
                    // },
                    // VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UFALSE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 37 , HV-MV pipeline Output RawFE Input CAC disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 38 , HV-MV pipeline Output NSF4 Input CAC disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
  /* 39 , HV-MV pipeline Output GLBC Input CAC disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 40 , HV-MV pipeline Output RawFE Input CAC disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 41 , HV-MV pipeline Output NSF4 Input CAC disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
  /* 42 , HV-MV pipeline Output GLBC Input CAC disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 43 , HV-MV pipeline Output RawFE Input CAC disabled; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 44 , HV-MV pipeline Output NSF4 Input CAC disabled; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
  /* 45 , HV-MV pipeline Output GLBC Input CAC disabled; 4K */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 46 , HV-MV pipeline Output RawFE Input CAC disabled; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 47 , HV-MV pipeline Output NSF4 Input CAC disabled; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
  /* 48 , HV-MV pipeline Output GLBC Input CAC disabled; VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 49 , HV-MV pipeline Output RawFE Input CAC disabled; Q-VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 50 , HV-MV pipeline Output NSF4 Input CAC disabled; Q-VGA */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV

                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },

    /* 51 , HV-MV pipeline Output RawFE Input CAC,GLBCE,NSF4 disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 52 , HV-MV pipeline Output NSF4 Input CAC,GLBCE disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 53 , HV-MV pipeline Output GLBC Input CAC,NSF4 disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 54 , HV-MV pipeline Output CAC Input CAC,NSF4 disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 55 , HV-MV pipeline Output NSF4 Input CAC,GLBC disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 56 , HV-MV pipeline Output GLBC Input NSF4 disabled; 1080p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },


    /* 57 , HV-MV pipeline Output RawFE Input CAC,GLBCE,NSF4 disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 58 , HV-MV pipeline Output NSF4 Input CAC,GLBCE disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 59 , HV-MV pipeline Output GLBC Input CAC,NSF4 disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 60 , HV-MV pipeline Output CAC Input CAC,NSF4 disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 61 , HV-MV pipeline Output NSF4 Input CAC,GLBC disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 62 , HV-MV pipeline Output GLBC Input NSF4 disabled; 720p */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },


    /* 63 , HV-MV pipeline Output RawFE Input CAC,GLBCE,NSF4 disabled; 4k */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 64 , HV-MV pipeline Output NSF4 Input CAC,GLBCE disabled; 4k */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 65 , HV-MV pipeline Output GLBC Input CAC,NSF4 disabled; 4k */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 66 , HV-MV pipeline Output CAC Input CAC,NSF4 disabled; 4k */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 67 , HV-MV pipeline Output NSF4 Input CAC,GLBC disabled; 4k */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 68 , HV-MV pipeline Output GLBC Input NSF4 disabled; 4k */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 4096, 2048, {4096*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 4096, 2048, {4096, 4096, 4096}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },



    /* 69 , HV-MV pipeline Output RawFE Input CAC,GLBCE,NSF4 disabled; vga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 70 , HV-MV pipeline Output NSF4 Input CAC,GLBCE disabled; vga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 71 , HV-MV pipeline Output GLBC Input CAC,NSF4 disabled; vga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 72 , HV-MV pipeline Output CAC Input CAC,NSF4 disabled; vga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 73 , HV-MV pipeline Output NSF4 Input CAC,GLBC disabled; vga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 74 , HV-MV pipeline Output GLBC Input NSF4 disabled; vga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 640, 480, {640*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 640, 480, {640, 640, 640}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UTRUE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_GLBC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },



    /* 75 , HV-MV pipeline Output RawFE Input CAC,GLBCE,NSF4 disabled; qvga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_RFE,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
    /* 76 , HV-MV pipeline Output NSF4 Input CAC,GLBCE disabled; qvga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UFALSE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[20]
    },
  /* 77 , HV-MV pipeline Output CAC Input CAC,NSF4 disabled; qvga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UFALSE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_CAC,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
    /* 78 , HV-MV pipeline Output NSF4 Input CAC,GLBC disabled; qvga */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 320, 240, {320*2, 0, 0}, {UFALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {   
                    /* Output 0 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_HV
                },
                {
                    /* Output 1 Enabled */
                    UFALSE,
                    /* Output Format */
                    {
                    },
                },
                {
                    /* Output 2 Enabled */
                    UTRUE,
                    /* Output Format */
                    {
                        0, 320, 240, {320, 320, 320}, {UFALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    },
                    VHWA_VISS_PIPE_MV
                },
                {
                    /* Output 3 Enabled */
                    UFALSE,
                    {
                    }
                },
                {
                    /* Output 4 Enabled */
                    UFALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    UFALSE,
                    {
                    }
                },
            },
            UFALSE,                           /* enable GLBCE */
            UTRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            UTRUE,                           /* Enable DPC */
            UTRUE,                           /* Enable CAC */
            UTRUE,                           /* Enable MV pipe*/
            VHWA_VISS_MV_PIPE_INPUT_NSF4,    /* MV input */
        },
        /* VISS config */
        &gVissCfg[23]
    },
#endif
};

AppViss_TestParams gAppVissTestPrms[] =
{
    {
        "TC_001",                   /* Test Name */
        1,                          /* Num Handles */
        100,                        /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[0]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_002",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[1]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_003",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[2]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_004",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[3]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_005",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[4]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_006",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[5]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_007",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[6]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_008",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[7]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_009",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[8]},     /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_010",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[9]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_011",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[10]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_012",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[11]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
#if defined (SOC_J721S2) || defined (SOC_J784S4)    
    {
        "TC_013",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[12]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_014",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[13]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled due to improper CAC golden reference */
        {
        "TC_015",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[14]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_016",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[15]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
        {
        "TC_017",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[16]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_018",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[17]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
        {
        "TC_019",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[18]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_020",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[19]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_021",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[20]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_022",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[21]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_023",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[22]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_024",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[23]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_025",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[24]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_026",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[25]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_027",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[26]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_028",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[27]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_029",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[28]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_030",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[29]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_031",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[30]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_032",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[31]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_033",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[32]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_034",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[33]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_035",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[34]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_036",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[35]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_037",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[36]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled before this due to improper CAC golden reference   */
        {
        "TC_038",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[37]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
        {
        "TC_039",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[38]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
        {
        "TC_040",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[39]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
        {
        "TC_041",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[40]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_042",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[41]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_043",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[42]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_044",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[43]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_045",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[44]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_046",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[45]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_047",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[46]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
        {
        "TC_048",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[47]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_049",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[48]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_050",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[49]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_051",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[50]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },

    {
        "TC_052",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[51]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_053",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[52]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_054",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[53]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_055",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[54]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_056",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[55]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_057",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[56]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },

        {
        "TC_058",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[57]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_059",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[58]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_060",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[59]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled due to improper CAC golden reference */
    {
        "TC_061",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[60]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_062",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[61]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_063",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[62]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled before this due to improper CAC golden reference   */

    {
        "TC_064",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[63]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_065",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[64]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_066",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[65]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled due to improper CAC golden reference */
    {
        "TC_067",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[66]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_068",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[67]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_069",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[68]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled before this due to improper CAC golden reference   */

        {
        "TC_070",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[69]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_071",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[70]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_072",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[71]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled due to improper CAC golden reference */
    {
        "TC_073",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[72]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_074",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[73]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_075",                   /* Test Name */
        1,                          /* Num Handles */
        100,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[74]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled before this due to improper CAC golden reference   */

        {
        "TC_076",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[75]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_077",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[76]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled due to improper CAC golden reference */
    {
        "TC_078",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[77]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
    {
        "TC_079",                   /* Test Name */
        1,                          /* Num Handles */
        1,                          /* Repeate Count */
        UTRUE,                       /* Is Performance */
        {&gAppVissTestConfig[78]},   /* Test Config */
        UTRUE,
        UFALSE,
        UFALSE
    },
/* Test cases disabled before this due to improper CAC golden reference   */
#endif
};



#endif

