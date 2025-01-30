#if (VPAC_TEST_INSTANCE == 0)
    #define VPAC_MSC_TEST_INST_ID_0     VHWA_M2M_VPAC_0_MSC_DRV_INST_ID_0
    #define VPAC_MSC_TEST_INST_ID_1     VHWA_M2M_VPAC_0_MSC_DRV_INST_ID_1
#endif
#if (VPAC_TEST_INSTANCE == 1)
    #define VPAC_MSC_TEST_INST_ID_0     VHWA_M2M_VPAC_1_MSC_DRV_INST_ID_0
    #define VPAC_MSC_TEST_INST_ID_1     VHWA_M2M_VPAC_1_MSC_DRV_INST_ID_1
#endif

#if defined(SOC_J721E) || defined(SOC_J721S2) || defined (SOC_J784S4)
    /* 0, Instance 0 TestCase 1-in 10-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UTRUE,
        {
            {
                0xEE68C3FE, 0xCF9397E3, 0xFB785F14, 0xDC853C6A, 0xEFA819AC,
                0xEEF60190, 0xC40DCBC3, 0x218198F8, 0x765A99F8, 0x7CE39B9E
            },
            {
                0x3A0BD532, 0x668AA2A2, 0x42FCF08E, 0x5CEDE765, 0x86D5DB01,
                0x3DA35EBA, 0xF30DE7AB, 0xE55BB80D, 0x39D37F4B, 0x280F7833
            }
        }
    },
    /* 1, Instance 1 TestCase 1-in 10-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UTRUE,
        {
            {
                0xEE68C3FE, 0xCF9397E3, 0xFB785F14, 0xDC853C6A, 0xEFA819AC,
                0xEEF60190, 0xC40DCBC3, 0x218198F8, 0x765A99F8, 0x7CE39B9E
            },
            {
                0x3A0BD532, 0x668AA2A2, 0x42FCF08E, 0x5CEDE765, 0x86D5DB01,
                0x3DA35EBA, 0xF30DE7AB, 0xE55BB80D, 0x39D37F4B, 0x280F7833
            }
        }
    },
    /* 2, THREAD 0 TestCase 1-in 10-out 1920x1080 12bit unpacked input and 12bit unpacked output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_UNPACKED16,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                960, 540, 960*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                1728, 972, 1728*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                1536, 864, 1536*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                1344, 756, 1344*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                1152, 648, 1152*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                768, 432, 768*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                576, 324, 576*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                1248, 702, 1248*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            {
                864, 486, 864*2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}}
    },
    /* 3, THREAD 0 TestCase 1-in 10-out 1920x1080 12bit packed input and 8bit output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1344, 756, 1344,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1152, 648, 1152,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                576, 324, 576,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1248, 702, 1248,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                864, 486, 864,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}}
    },
    /* 4, THREAD 0 TestCase 1-in 10-out 1920x1080 8bit packed input and 8bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1344, 756, 1344,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1152, 648, 1152,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                576, 324, 576,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1248, 702, 1248,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                864, 486, 864,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}}
    },
    /* 5, THREAD 0 TestCase 1-in 10-out 1920x1080 8bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}}
    },
    /* 6, THREAD 0 TestCase 1-in 5-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
        },
        UTRUE,
        {
            {0x8DC9CDFA, 0xD95ABB46, 0x143129FE, 0x8BC2D516, 0x41264E29},
            {0x3E1DDF0B, 0x3D2555D9, 0x408B3CC1, 0x4C9BF8F2, 0xE74B4326}
        }
    },
    /* 7, THREAD 1 TestCase 1-in 5-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {0}, {0}, {0}, {0}, {0},
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UTRUE,
        {
            {0, 0, 0, 0, 0,
             0x18435A84, 0x093D7805, 0xEEFCE657, 0x372ADD40, 0x83219C2A},
            {0, 0, 0, 0, 0,
             0xE3844B20, 0x91D3F804, 0x770502C8, 0x0F26FFEA, 0xA8E04955}
        }
    },
    /* 8, THREAD 0 TestCase 1-in 10-out 1280x720 12bit packed input and ROI 1024x640 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1280, 720, 1280*3/2,
            FVID2_DF_LUMA_ONLY,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                1024, 640, 1024*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                512, 320, 512*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                256, 160, 256*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                920, 576, 920*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                818, 512, 818*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                716, 448, 716*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                614, 384, 614*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                408, 256, 408*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                308, 192, 308*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 480, 768*3/2,
                1024, 640,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {80,40},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {80,40},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {10,10},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {20,20},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}}
    },
    /* 9, THREAD 0 TestCase 1-in 5-out intereaved 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_LUMA_ONLY,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
        },
        UTRUE,
        {
            {
                0x6B2879B5, 0, 0x489C28D5, 0, 0x654D2893,
                0, 0xEE126CD2, 0, 0x33EA88FF, 0
            },
            {
                0
            }
        }
    },
    /* 10, THREAD 1 TestCase 1-in 5-out interleaved 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1920, 1080/2, 1920*3/2,
            FVID2_DF_CHROMA_ONLY,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {0},
            {
                960, 540/2, 960*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                1536, 864/2, 1536*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                1152, 648/2, 1152*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                576, 324/2, 576*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {0},
            {
                864, 486/2, 864*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UTRUE,
        {
            {
                0, 0x2D236640, 0, 0x71DB6E7C, 0,
                0x87531734, 0, 0xC121A445, 0, 0x219DC4CD
            },
            {
                0
            }
        }
    },
    /* 11, THREAD 0 TestCase 1-in 1-out 1920x1080 8bit packed input and 8bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                1920, 1080, 1920,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
        },
        UTRUE,
        {
            {0x006FA1F6}, //{0x2AFDF6B8},
            {0x38602E58} //{0x4D4B96B1}
        },
        UFALSE, /* MCRC */
        {0xE678FF159EE2C866}   /* MCRC */
    },
    /* 12, THREAD 1 TestCase 1-in 1-out 1920x1080 8bit packed input and 8bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {0}, {0}, {0}, {0},
            {0}, {0}, {0}, {0}, {0},
            {
                1920, 1080, 1920,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED
            }
        },
        /* MSC Configuration for each instance */
        {
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            }
        },
        UTRUE,
        {
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0x006FA1F6}, //0x2AFDF6B8},
            {0, 0, 0, 0, 0, 0, 0, 0, 0, 0x38602E58}, //0x4D4B96B1}
        },
        UFALSE, /* MCRC */
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0xE678FF159EE2C866}   /* MCRC */
    },
    /* 13, THREAD 0 TestCase 1-in 10-out 1920x1080 12bit packed input and 12bit packed output Single Phase */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_SINGLE_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_SINGLE_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_1,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE},{UFALSE},{UFALSE},{UFALSE},{UFALSE},
            {UFALSE},{UFALSE},{UFALSE}
        },
        UFALSE,
        {{0},{0}},
        UFALSE, /* MCRC */
        {0x09526F94BB27382D, 0x7F50372CB943053F, 0, 0,
         0, 0, 0, 0, 0, 0}   /* MCRC */
    },
    /* 14, THREAD 0 TestCase 1-in 10-out 1920x1080 12bit packed input and 12bit packed output 64 Phase */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_64PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_64PHASE_COEFF_SET_2,
                MSC_MULTI_64PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}},
        UFALSE, /* MCRC */
        {0x7A42078E8215AD3D, 0xD8CC3853BD4A129B, 0x4C246255D00216C9,
         0x8A0E77F404DF687B, 0, 0, 0, 0, 0, 0}   /* MCRC */
    },
    /* 15, THREAD 0 TestCase 1-in 10-out 1920x1080 12bit packed input and 12bit packed output 4tap filter */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_4TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}},
        UFALSE, /* MCRC */
        {0x25286C09544A2D93, 0xFFC24F9C03195C62, 0x4B94FBEF0AAD152C, 0x9FC7F47762758E22,
         0, 0, 0, 0, 0, 0}   /* MCRC */
    },
    /* 16, THREAD 0 TestCase 1-in 10-out 1920x1080 12bit packed input and 12bit packed output 3-Tap */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_3TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {{0},{0}},
        UFALSE, /* MCRC */
        {0xBBF67D83807C8B0D, 0xFDADE56B4E2FD659, 0x5785B3AC1820656C, 0xA53FCE1156CB2DED,
         0, 0, 0, 0, 0, 0}   /* MCRC */
    },
    /* 17, THREAD 0 TestCase 1-in 1-out 1920x1080 12bit packed input and 12bit packed output Line Skip */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UTRUE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}
        },
        UFALSE,
        {{0},{0}},
        UFALSE, /* MCRC */
        {0, 0, 0, 0,
         0, 0, 0, 0, 0, 0}   /* MCRC */
    },
    /* 18, THREAD 0 TestCase 1-in 5-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_LUMA_ONLY,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1344, 756, 1344*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
        },
        UTRUE,
        {
            {0x8DC9CDFA, 0xD95ABB46, 0x143129FE, 0x8BC2D516, 0x41264E29},
            {0x3E1DDF0B, 0x3D2555D9, 0x408B3CC1, 0x4C9BF8F2, 0xE74B4326}
        }
    },
    /* 19, THREAD 1 TestCase 1-in 5-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_LUMA_ONLY,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {0}, {0}, {0}, {0}, {0},
            {
                1152, 648, 1152*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                576, 324, 576*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1248, 702, 1248*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                864, 486, 864*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UTRUE,
        {
            {0, 0, 0, 0, 0,
             0x18435A84, 0x093D7805, 0xEEFCE657, 0x372ADD40, 0x83219C2A},
            {0, 0, 0, 0, 0,
             0xE3844B20, 0x91D3F804, 0x770502C8, 0x0F26FFEA, 0xA8E04955}
        }
    },
    /* 20, THREAD 0, 1280x720, Luma only, 2 output*/
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1280, 720, 1280,
            FVID2_DF_LUMA_ONLY,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                640, 360, 640,
                1280, 720,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                320, 180, 320,
                1280, 720,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
        },
        UFALSE,
        {
            {0},
            {0}
        }
    },
    /* 21, THREAD 0, 1280x720, Chroma only, 2 output*/
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1280, 720/2, 1280,
            FVID2_DF_CHROMA_ONLY,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {0}, {0},
            {
                640, 360/2, 640,
                1280, 720/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                320, 180/2, 320,
                1280, 720/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {UFALSE}, {UFALSE},
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE}, {UFALSE},
        },
        UFALSE,
        {
            {0},
            {0}
        }
    },
#endif
/* Below config is currently disabled due to limitation at SDK */
#if 0
#if defined(SOC_J721S2) || defined (SOC_J784S4)
    /* 0 (J721S2), Instance 0 TestCase 1-in 5-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                480, 270/2, 480*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540/2, 960*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972/2, 1728*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864/2, 1536*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432/2, 768*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },

        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        }
    },
    /* 1 (J721S2), Instance 1 TestCase 1-in 5-out 1920x1080 12bit packed input and 12bit packed output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_1,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                480, 270/2, 480*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540/2, 960*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972/2, 1728*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864/2, 1536*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432/2, 768*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        }
    },
    /* 2 (J721S2), THREAD 0 TestCase 1-in 5-out 1920x1080 8bit input and 8bit output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                480, 270/2, 480,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540/2, 960,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972/2, 1728,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864/2, 1536,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432/2, 768,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {0,0}
    },
    /* 3 (J721S2), Instance 0 TestCase 1-in 5-out 1920x1080 Y12 UV8  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                480, 270/2, 480,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540/2, 960,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972/2, 1728,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864/2, 1536,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432/2, 768,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },

        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        },
        UFALSE,
        {
        },
        {
            UTRUE,
            1920,
            FVID2_CCSF_BITS8_PACKED,
        },
    },
    /* 4 (J721S2), Instance 0 TestCase 1-in 5-out 1920x1080 Y8 UV12  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                480, 270/2, 480*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540/2, 960*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972/2, 1728*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864/2, 1536*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432/2, 768*3/2,
                1920, 1080/2,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },

        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        },
        UFALSE,
        {
        },
        {
            UTRUE,
            1920*3/2,
            FVID2_CCSF_BITS12_PACKED,
        },
    },
    /* 5 (J721S2), Instance 0 TestCase 1-in 5-out 1920x1080 Y8 Y8  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_2PLANES,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_PLANE_1,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_PLANE_2,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_PLANE_1,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_PLANE_2,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_PLANE_1,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_PLANE_2,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_PLANE_1,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_PLANE_2,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_PLANE_1,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_PLANE_2,
                FVID2_CCSF_BITS8_PACKED
            },

        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        },
        UFALSE,
        {
        }
    },
    /* 6 (J721S2), Instance 0, 1920x1080 YUV422SP 12bit packed input/output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*3/2,
            FVID2_DF_YUV422SP_UV,
            FVID2_CCSF_BITS12_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                480, 270, 480*3/2,
                1920, 1080,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                960, 540, 960*3/2,
                1920, 1080,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1728, 972, 1728*3/2,
                1920, 1080,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                1536, 864, 1536*3/2,
                1920, 1080,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_LUMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },
            {
                768, 432, 768*3/2,
                1920, 1080,
                FVID2_DF_CHROMA_ONLY,
                FVID2_CCSF_BITS12_PACKED
            },

        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        }
    },
    /* 7, (J721S2) Instance 0 TestCase 1-in 4-out 1920x1080 YUV422I 8bit input/output  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920*2,
            FVID2_DF_YUV422I_YUYV,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480*2,
                1920, 1080,
                FVID2_DF_YUV422I_YUYV,
                FVID2_CCSF_BITS8_PACKED
            },
            { 0 },
            {
                960, 540, 960*2,
                1920, 1080,
                FVID2_DF_YUV422I_YUYV,
                FVID2_CCSF_BITS8_PACKED
            },
            { 0 },
            {
                1728, 972, 1728*2,
                1920, 1080,
                FVID2_DF_YUV422I_YUYV,
                FVID2_CCSF_BITS8_PACKED
            },
            { 0 },
            {
                1536, 864, 1536*2,
                1920, 1080,
                FVID2_DF_YUV422I_YUYV,
                FVID2_CCSF_BITS8_PACKED
            },
            { 0 },
            { 0 },
            { 0 }
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UFALSE
            },
            {
                UFALSE
            },
            {
                UFALSE
            },
        },
        UFALSE,
        {
        }
    },
    /* 8 (J721S2), Instance 0 TestCase 1-in 5-out 1920x1080 P1 +P2P3 Interleaved  */
    {
        UFALSE /*loopBack*/,
        VPAC_MSC_TEST_INST_ID_0,
        /* Input Frame Parameter */
        {
            1920, 1080, 1920,
            FVID2_DF_R_GBI,
            FVID2_CCSF_BITS8_PACKED,
            UFALSE, MSC_TAP_SEL_5TAPS
        },
        /* output Frame Parameter */
        {
            {
                480, 270, 480,
                1920, 1080,
                FVID2_DF_R,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                480*2, 270, 480*2,
                1920*2, 1080,
                FVID2_DF_GBI,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960, 540, 960,
                1920, 1080,
                FVID2_DF_R,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                960*2, 540, 960*2,
                1920*2, 1080,
                FVID2_DF_GBI,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728, 972, 1728,
                1920, 1080,
                FVID2_DF_R,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1728*2, 972, 1728*2,
                1920*2, 1080,
                FVID2_DF_GBI,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536, 864, 1536,
                1920, 1080,
                FVID2_DF_R,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                1536*2, 864, 1536*2,
                1920*2, 1080,
                FVID2_DF_GBI,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768, 432, 768,
                1920, 1080,
                FVID2_DF_R,
                FVID2_CCSF_BITS8_PACKED
            },
            {
                768*2, 432, 768*2,
                1920*2, 1080,
                FVID2_DF_GBI,
                FVID2_CCSF_BITS8_PACKED
            },
        },
        /* MSC Configuration for each instance */
        {
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_2,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_1,
                MSC_MULTI_32PHASE_COEFF_SET_0,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
            {
                UTRUE, {0,0},
                0, 0,
                MSC_FILTER_MODE_MULTI_PHASE,
                MSC_PHASE_MODE_32PHASE,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_SINGLE_PHASE_SP_COEFF_0,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_MULTI_32PHASE_COEFF_SET_3,
                MSC_COEFF_SHIFT_8,
                UFALSE, UFALSE
            },
        },
        UFALSE,
        {
        },
        UFALSE,
        {
        },
        {
            UTRUE,
            1920*2,
            FVID2_CCSF_BITS8_PACKED,
        },
    },
#endif
#endif
