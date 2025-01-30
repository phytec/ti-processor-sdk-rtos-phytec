
#ifndef APP_FC_CFG_H_
#define APP_FC_CFG_H_

App_MscTestCfg gAppMscTestCfg[] =
{
    #include <ti/drv/vhwa/examples/include/vhwa_msc_test_cfg.h>
};

Vhwa_M2mFcGraphPathInfo gPathInfo[] =
{
    /* Start Path DDR->VISS->MSC->DDR */
    /* 0 */
    {
        6,
        {
            {VHWA_FC_PORT_DDR, VHWA_FC_PORT_VISS_IN_0},
            {VHWA_FC_PORT_VISS_OUT_Y8, VHWA_FC_PORT_DDR},
            {VHWA_FC_PORT_VISS_OUT_UV8, VHWA_FC_PORT_DDR},
            {VHWA_FC_PORT_VISS_OUT_Y8, VHWA_FC_PORT_MSC0_IN_Y},
            {VHWA_FC_PORT_MSC0_OUT_0, VHWA_FC_PORT_DDR},
            {VHWA_FC_PORT_MSC0_OUT_1, VHWA_FC_PORT_DDR}
        }
    },
    /* 1 */
    /* VISS->MSC0, VISS->MSC1 */
    {
        7,
        {
            {VHWA_FC_PORT_DDR, VHWA_FC_PORT_VISS_IN_0},
            {VHWA_FC_PORT_VISS_OUT_Y8, VHWA_FC_PORT_MSC0_IN_Y},
            {VHWA_FC_PORT_MSC0_OUT_0, VHWA_FC_PORT_DDR},
            {VHWA_FC_PORT_MSC0_OUT_1, VHWA_FC_PORT_DDR},
            {VHWA_FC_PORT_VISS_OUT_UV8, VHWA_FC_PORT_MSC1_IN_Y},
            {VHWA_FC_PORT_MSC1_OUT_2, VHWA_FC_PORT_DDR},
            {VHWA_FC_PORT_MSC1_OUT_3, VHWA_FC_PORT_DDR}
        }
    },
};


AppFc_TestParams gAppFcTestPrms[] =
{
    /* DDR->VISS->MSC->DDR start */
    {
        .testName = "TC_001",                   /* Test Name */
        .numHandles = 1,                        /* Num Handles */
        .repeatCnt = 3,                         /* Repeate Count */
        .isPerformanceTest = UFALSE,             /* Is Performance */
        .isVissEnabled = UTRUE,                  /* isVissEnabled */
        .isMsc0Enabled = UTRUE,                  /* Is Msc0Enabled */
        .isMsc1Enabled = UFALSE,                 /* isMsc1Enabled */
        .testCfg =                              /* Test Config */
        {
            {
                &gPathInfo[0],
                &gAppVissTestConfig[11],
                &gAppMscTestCfg[20],
                NULL
            }, {0}, {0}, {0}
        },
        .isEnableTest = UTRUE,                   /* isEnableTest */
        .vissIsSwitchGlbceCtx = UFALSE,          /* vissIsSwitchGlbceCtx */
        .vissChCfgOnEachIter = UFALSE,           /* vissChCfgOnEachIter  */
    },
    /* DDR->VISS->MSC0/MSC1->DDR start */
    {
        .testName = "TC_002",                   /* Test Name */
        .numHandles = 1,                        /* Num Handles */
        .repeatCnt = 3,                         /* Repeate Count */
        .isPerformanceTest = UFALSE,             /* Is Performance */
        .isVissEnabled = UTRUE,                  /* isVissEnabled */
        .isMsc0Enabled = UTRUE,                  /* Is Msc0Enabled */
        .isMsc1Enabled = UTRUE,                  /* isMsc1Enabled */
        .testCfg =                              /* Test Config */
        {
            {
            &gPathInfo[1],
            &gAppVissTestConfig[11],
            &gAppMscTestCfg[20], 
            &gAppMscTestCfg[21]
            }, {0}, {0}, {0}
        },
        .isEnableTest = UTRUE,            /* isEnableTest */
        .vissIsSwitchGlbceCtx = UFALSE,   /* vissIsSwitchGlbceCtx */
        .vissChCfgOnEachIter = UFALSE,    /* vissChCfgOnEachIter  */
    },
};

#endif

