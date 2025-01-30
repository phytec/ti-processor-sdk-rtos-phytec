    {
        /* 0, 2MP NV12 12u to 12u unpacked LSB aligned, with
             block size aligned,
           Enable Back Mapping and BiCubic Interpolation  */
        1920, 1080, 1920*2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 3, 32, 30,  /* Back Mapping enable, pixpad, block size */
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC, /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 1, 2MP NV12 12u to 12u unpacked MSB aligned, with
             block size unaligned,
           Enable Back Mapping and BiCubic Interpolation  */
        1920, 1080, 1920*2, FVID2_DF_YUV420SP_UV,
            FVID2_CCSF_BITS12_UNPACKED16_MSB_ALIGNED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1944*2, FVID2_DF_YUV420SP_UV,
                    FVID2_CCSF_BITS12_UNPACKED16_MSB_ALIGNED},
        },
        UTRUE, 3, 36, 34,  /* Back Mapping enable, pixpad, block size */
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC, /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x86f9e085, 0x99956308, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 2, 2MP NV12 12p to 12p, with block size aligned,
           Enable Back Mapping and BiCubic Interpolation  */
        1920, 1080, (1920*3)/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, (1920*3)/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,  /* Back Mapping enable, pixpad, block size */
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC, /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UTRUE, {0x86f9e085, 0x99956308, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 3, 2MP NV12 8p to 8p, with block size aligned */
        1920, 1080, 1920, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0xB9B9C7C5, 0xA8E23661, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0x96B21957D4B0B026, 0} /* MCRC */
    },
    {
        /* 4, 2MP YUV420 8p to YUV420 8p, with 9 regions */
        1920, 1080, 1920, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE,  UTRUE, UTRUE},
                {UTRUE,  UTRUE, UTRUE},
                {UTRUE,  UTRUE, UTRUE}
            },
            /* block width */
            {
               {40, 40, 20},
               {36, 20, 36},
               {40, 20, 24}
            },
            /* block height */
            {
               {40, 50, 46},
               {20, 20, 20},
               {24, 48, 48}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                800, 400, 720
            },
            /* Slice Height */
            {
                200, 400, 480
            }
        },
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 5, 2MP YUV420 12p to YUV420 12p, with 2x2 regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UFALSE},
                {UTRUE, UTRUE, UFALSE},
                {UFALSE, UFALSE, UFALSE}
            },
            /* block width */
            {
                {64, 32, 20},
                {16, 32, 20},
                {28, 28, 20}
            },
            /* block height */
            {
                {52, 52, 32},
                {32, 54, 32},
                {32, 32, 32}
            },
            /* pixel pad */
            {
                {1, 1, 1},
                {1, 1, 1},
                {1, 1, 1}
            },
            /* Slice Width */
            {
                960, 960, 0
            },
            /* Slice Height */
            {
                540, 540, 0
            }
        },
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 6, 1MP YUV420 8b to YUV420 8p , block non aligned 2x2 regions */
        1280, 720, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1344,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 720, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UFALSE},
                {UTRUE, UTRUE, UFALSE},
                {UFALSE, UFALSE, UFALSE}
            },
            /* block width */
            {
                {62, 58, 0},
                {48, 24, 0},
                { 0,  0, 0}
            },
            /* block height */
            {
                {42, 34, 0},
                {22, 26, 0},
                { 0,  0, 0}
            },
            /* pixel pad */
            {
                {3, 3, 0},
                {3, 3, 0},
                {0, 0, 0}
            },
            /* Slice Width */
            {
                520, 760, 0
            },
            /* Slice Height */
            {
                400, 320, 0
            }
        },
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 7, 2MP YUV420 12p to YUV420 12p, with 3x1 regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UTRUE},
                {UFALSE, UFALSE, UFALSE},
                {UFALSE, UFALSE, UFALSE}
            },
            /* block width */
            {
                {64, 32, 20},
                {16, 32, 20},
                {28, 28, 20}
            },
            /* block height */
            {
                {52, 52, 32},
                {32, 54, 32},
                {32, 32, 32}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                600, 600, 720
            },
            /* Slice Height */
            {
                1080, 0, 0
            }
        },
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 8, 2MP YUV420 12p to YUV420 12p, with 2x3 regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1952*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UFALSE},
                {UTRUE, UTRUE, UFALSE},
                {UTRUE, UTRUE, UFALSE}
            },
            /* block width */
            {
                {64, 32, 20},
                {16, 32, 20},
                {28, 28, 20}
            },
            /* block height */
            {
                {52, 52, 32},
                {32, 54, 32},
                {26, 32, 32}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                960, 960, 0
            },
            /* Slice Height */
            {
                400, 400, 280
            }
        },
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 9, 2MP YUV420 12p to YUV420 12p, with 3x2 regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, /*1936*3/2*/ 2896,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UTRUE},
                {UTRUE, UTRUE, UTRUE},
                {UFALSE, UFALSE, UFALSE}
            },
            /* block width */
            {
                {64, 32, 20},
                {16, 32, 18},
                {28, 28, 20}
            },
            /* block height */
            {
                {52, 52, 32},
                {32, 54, 32},
                {0, 0, 0}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                600, 500, 820
            },
            /* Slice Height */
            {
                400, 680, 0
            }
        },
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 10, 2MP YUV420 12p to YUV420 12p, with 3,2,1 regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UTRUE},
                {UTRUE, UTRUE, UFALSE},
                {UTRUE, UFALSE, UFALSE}
            },
            /* block width */
            {
                {36, 30, 20},
                {20, 20,  0},
                {10,  0,  0}
            },
            /* block height */
            {
                {32, 56, 46},
                {40, 32,  0},
                {34,  0,  0}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                700, 600, 620
            },
            /* Slice Height */
            {
                300, 400, 380
            }
        }
    },
    {
        /* 11, 2MP YUV420 12p to YUV420 12p, with 1, 2, 3 regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UFALSE, UFALSE, UTRUE},
                {UFALSE,  UTRUE, UTRUE},
                { UTRUE,  UTRUE, UTRUE}
            },
            /* block width */
            {
               { 0,  0, 20},
               { 0, 40, 36},
               {40, 20, 24}
            },
            /* block height */
            {
               { 0,  0, 46},
               { 0, 50, 20},
               {24, 48, 48}
            },
            /* pixel pad */
            {
                {0, 0, 3},
                {0, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                800, 400, 720
            },
            /* Slice Height */
            {
                200, 400, 480
            }
        }
    },
    {
        /* 12, 2MP YUV420 12p to YUV420 12p, with 3x3 - center regions */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE,  UTRUE, UTRUE},
                {UTRUE, UFALSE, UTRUE},
                {UTRUE,  UTRUE, UTRUE}
            },
            /* block width */
            {
               {40, 40, 20},
               {36,  0, 36},
               {40, 20, 24}
            },
            /* block height */
            {
               {40, 50, 46},
               {20,  0, 20},
               {24, 48, 48}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 0, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                800, 400, 720
            },
            /* Slice Height */
            {
                200, 400, 480
            }
        }
    },
    {
        /* 13 1MP YUV420 8b to YUV420 8p , 3x3 regions
            each block size non aligned */
        1280, 1280, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1328,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 720, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UTRUE,
        {
            /* enable */
            {
                {UTRUE, UTRUE, UTRUE},
                {UTRUE, UTRUE, UTRUE},
                {UTRUE, UTRUE, UTRUE}
            },
            /* block width */
            {
                {62, 58, 56},  //3 3 3
                {48, 60, 58},  //3 3 3
                {42, 30, 58}   //3 3 3
            },
            /* block width */
            {
                {42, 38, 38},
                {38, 32, 32},
                {26, 28, 34}
            },
            /* pixel pad */
            {
                {3, 3, 3},
                {3, 3, 3},
                {3, 3, 3}
            },
            /* Slice Width */
            {
                400, 400, 480
            },
            /* Slice Height */
            {
                300, 300, 120
            }
        }
    },
    {
        /* 14, 1MP YUV420 8p to YUV420 8p , block non-aligned */
        1280, 720, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1288,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 56, 36,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR, /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 15, 1MP YUV422I 8bit input and YUV422 8bit output */
        1280, 720, 1280*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1300*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS8_PACKED}
        },
        UTRUE, 3, 52, 26,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 16, 1MP YUV422I 8bit input and YUV420 12p output */
        1280, 720, 1280*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1280*3/2, FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 20,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UTRUE, {0x2DAB31E3, 0x50609024, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 17, 1MP YUV422I 8bit input and YUV420 8p output */
        1280, 720, 1280*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1280, FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 20,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UTRUE, {0xD974999D, 0x2D18C6C5, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 18, 1MP YUV422I 8bit input and YUV420 12u output */
        1280, 720, 1280*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1280*2, FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 3, 32, 20,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 19, 2MP NV12 12p to 8p, with block size aligned */
        1920, 1080, (1920*3)/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 64, 54,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 20, 2MP NV12 8p to 12p with block size aligned */
        1920, 1080, 1920, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV,
                FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 64, 54,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 21, 1MP NV12 8p to 12p and 8p, with block size not aligned */
        1280, 720, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UTRUE},
        {
            {1280, 720, 1312*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
            {1280, 720, 1312,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 52, 56,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 22, 2MP NV12 12p to 12p and 8p, with block size not aligned */
        1920, 1080, (1920*3)/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UTRUE},
        {
            {1920, 1080, 2896,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
            {1920, 1080, 1936,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 52, 56,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 23, 1MP YUV420 to Luma Only output for 12p, block aligned */
        1280, 720, 1280*3/2, FVID2_DF_LUMA_ONLY,
            FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1280*3/2,
                    FVID2_DF_LUMA_ONLY, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UTRUE, {0x1e76db20, 0x0, 0x0, 0x0},
        UFALSE
    },
    {
        /* 24, 1MP YUV420 to Luma Only output for 12p, block aligned */
        1280, 720, 1280*3/2, FVID2_DF_LUMA_ONLY, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UTRUE},
        {
            {1280, 720, 1280*3/2,
                    FVID2_DF_LUMA_ONLY, FVID2_CCSF_BITS12_PACKED},
            {1280, 720, 1280,
                    FVID2_DF_LUMA_ONLY, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0, 0x0, 0x0, 0x0},
        UFALSE
    },
    {
        /* 25, 1MP Chroma only, block aligned */
        1280, 360, 1280*3/2, FVID2_DF_CHROMA_ONLY, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 360, 1280*3/2,
                    FVID2_DF_CHROMA_ONLY, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 3, 32, 16,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 26, 1MP Chroma only, block aligned */
        1280, 360, 1280*3/2, FVID2_DF_CHROMA_ONLY, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 360, 1280*3/2,
                    FVID2_DF_CHROMA_ONLY, FVID2_CCSF_BITS12_PACKED},
            {1280, 360, 1280,
                    FVID2_DF_CHROMA_ONLY, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 12,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 27, HKMC Configuration */
        /* 1MP NV12 8p to 1280x256 8p, with block size aligned */
        1280, 720, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1200, 256, 1200,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 30, 30,
        VHWA_LDC_LUT_DS_FACTOR_16, 1200, 256, 320,
        (uint64_t)APP_LDC_LUT_ADDR_HKMC,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UTRUE, {0xcbba4ef6, 0x1909cd5c, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 28, 2MP NV12 8p to 8p, with block size aligned, pixel pad = 0 for
           pix_iblk_outofbound, P_IBLK_MEMOVF & M_IBLK_MEMOVF error */
        1920, 1080, 1920, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 0, 192, 192,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        {UFALSE, UFALSE},
    },
    {
        /* 29, 128x64 YUV420 12u to YUV420 12u, block aligned
           IFRAME_OUTB error */
        128, 64, 128*2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {128, 64, 128*2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 3, 16, 16,
        VHWA_LDC_LUT_DS_FACTOR_8, 1280, 1280, 704,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        {UFALSE, UFALSE}
    },
    {
        /* 30, 1MP YUV420 8b to YUV420 8p , block aligned
           P_IBLK_MEMOVF error */
        1280, 720, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1280,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 128, 128,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 720, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        {UFALSE, UFALSE}
    },
    {
        /* 31, 2MP NV12 8p to 8p, with block size aligned, pixel pad = 0 for
           pix_iblk_outofbound error */
        1920, 1080, 1920, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 0, 192, 192,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        {UFALSE, UFALSE},
    },
    {
        /* 32, 1MP YUV420 8p to YUV420 8p , block aligned,
               45 rotation using affine transform */
        1280, 720, 1280, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1280, 720, 1288,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 30,
        VHWA_LDC_LUT_DS_FACTOR_16, 1280, 800, 384,
        (uint64_t)APP_LDC_LUT_ADDR_1MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR, /* Luma Interpolation */
        UTRUE, {2144, -3100, 4484, 2197, 1344, 334, -404, -2655},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE,
    },
    {
        /* 33, 2MP NV12 12u to 12u unpacked LSB aligned, with
             block size aligned,
           Enable Back Mapping and BiCubic Interpolation,
           Dual Output, second output enabled with 12to8 Lut */
        1920, 1080, 1920*2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UTRUE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16},
            {1920, 1080, 1920,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 32, 30,  /* Back Mapping enable, pixpad, block size */
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC, /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UTRUE,
    },
#if defined (SOC_J721S2)  || defined (SOC_J784S4)
    {
        /* 34, 2MP NV12 12u to 12u, with block size aligned */
        1920, 1080, 1920*2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 35, 2MP YUV422SP 8b to 8b, with block size aligned */
        1920, 1080, 1920, FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 64, 54,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 36, 2MP YUV422SP 12u to 12u, with block size aligned */
        1920, 1080, 1920*2, FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 37, 2MP YUV422SP 12p to 12p, with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0} /* MCRC */
    },
    {
        /* 38, 2MP YUV422SP Y12 UV8, with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920,  FVID2_CCSF_BITS8_PACKED},
        {
            {1920, FVID2_CCSF_BITS8_PACKED},
        },

    },
    {
        /* 39, 2MP YUV420SP Y12 UV8, with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920,  FVID2_CCSF_BITS8_PACKED},
        {
            {1920, FVID2_CCSF_BITS8_PACKED},
        },
    },
    {
        /* 40, 2MP Y8 + Y8 , with block size aligned */
        1920, 1080, 1920, FVID2_DF_2PLANES, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_2PLANES, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 41, 2MP Y12 + Y12 , with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_2PLANES, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_2PLANES, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 42, 2MP YUV422SP 8b to YUV420SP 8b, with block size aligned */
        1920, 1080, 1920, FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 43, 2MP YUV422I 12u, with block size aligned */
        1920, 1080, 1920*2*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2*2,
                    FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 44, 2MP YUV422I 12p, with block size aligned */
        1920, 1080, 1920*3, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3,
                    FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 64,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 45, 2MP Y1+Y2Y3 8b to Y1+Y2+Y3, with block size aligned */
        1920, 1080, 1920, FVID2_DF_R_GBI, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920,
                    FVID2_DF_RGB, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 64, 54,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920*2,  FVID2_CCSF_BITS8_PACKED },
        {
            {1920, FVID2_CCSF_BITS8_PACKED, 1920},
        },
    },
    {
        /* 46, 2MP Y1+Y2Y3 12u to Y1+Y2+Y3, with block size aligned */
        1920, 1080, 1920*2, FVID2_DF_R_GBI, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_RGB, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 1, 32, 16,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920*2*2,  FVID2_CCSF_BITS12_UNPACKED16 },
        {
            {1920*2, FVID2_CCSF_BITS12_UNPACKED16, 1920*2},
        },
    },
    {
        /* 47, 2MP Y1+Y2Y3 12p to Y1+Y2+Y3, with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_R_GBI, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_RGB, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 32, 16,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920*2*3/2,  FVID2_CCSF_BITS12_PACKED },
        {
            {1920*3/2, FVID2_CCSF_BITS12_PACKED, 1920*3/2},
        },
    },
    {
        /* 48, 2MP Y1+Y2Y3 8b to Y1Y2+Y3, with block size aligned */
        1920, 1080, 1920, FVID2_DF_R_GBI, FVID2_CCSF_BITS8_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_RGI_B, FVID2_CCSF_BITS8_PACKED},
        },
        UTRUE, 3, 64, 54,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920*2,  FVID2_CCSF_BITS8_PACKED },
        {
            {1920, FVID2_CCSF_BITS8_PACKED, 1920},
        },
    },
    {
        /* 49, 2MP Y1+Y2Y3 12u to Y1Y2+Y3, with block size aligned */
        1920, 1080, 1920*2, FVID2_DF_R_GBI, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2*2,
                    FVID2_DF_RGI_B, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 1, 32, 16,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920*2*2,  FVID2_CCSF_BITS12_UNPACKED16 },
        {
            {1920*2, FVID2_CCSF_BITS12_UNPACKED16, 1920*2},
        },
    },
    {
        /* 50, 2MP Y1+Y2Y3 12p to Y1Y2+Y3, with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_R_GBI, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2*3/2,
                    FVID2_DF_RGI_B, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 32, 16,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BICUBIC,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
        { UTRUE, 1920*2*3/2,  FVID2_CCSF_BITS12_PACKED },
        {
            {1920*3/2, FVID2_CCSF_BITS12_PACKED, 1920*3/2},
        },
    },
    {
        /* 51, 2MP YUV422I 12u to YUV420, with block size aligned */
        1920, 1080, 1920*2*2, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_UNPACKED16,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*2,
                    FVID2_DF_YUV420SP_UV, FVID2_CCSF_BITS12_UNPACKED16},
        },
        UTRUE, 1, 64, 32,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 52, 2MP YUV422I to YUV422SP 12p, with block size aligned */
        1920, 1080, 1920*3, FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3/2,
                    FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 64,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
    {
        /* 53, 2MP YUV422SP to YUV422I 12p, with block size aligned */
        1920, 1080, 1920*3/2, FVID2_DF_YUV422SP_UV, FVID2_CCSF_BITS12_PACKED,
        {UTRUE, UFALSE},
        {
            {1920, 1080, 1920*3,
                    FVID2_DF_YUV422I_UYVY, FVID2_CCSF_BITS12_PACKED},
        },
        UTRUE, 1, 64, 64,
        VHWA_LDC_LUT_DS_FACTOR_4, 1920, 1080, 1984,
        (uint64_t)APP_LDC_LUT_ADDR_2MP,
        VHWA_LDC_LUMA_INTRP_BILINEAR,    /* Luma Interpolation */
        UFALSE, {4096, 0, 0, 0, 4096, 0, 0, 0},
        UFALSE, {0x0, 0x0, 0x0, 0x0},
        UFALSE, {0},
        UFALSE,{0, 0}, /* MCRC */
        UFALSE, /* Is Lut enable */
    },
#endif
