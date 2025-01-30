/*
*
* Copyright (c) 2024 Texas Instruments Incorporated
*
* All rights reserved not granted herein.
*
* Limited License.
*
* Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
* license under copyrights and patents it now or hereafter owns or controls to make,
* have made, use, import, offer to sell and sell ("Utilize") this software subject to the
* terms herein.  With respect to the foregoing patent license, such license is granted
* solely to the extent that any such patent is necessary to Utilize the software alone.
* The patent license shall not apply to any combinations which include this software,
* other than combinations with devices manufactured by or for TI ("TI Devices").
* No hardware patent is licensed hereunder.
*
* Redistributions must preserve existing copyright notices and reproduce this license
* (including the above copyright notice and the disclaimer and (if applicable) source
* code license limitations below) in the documentation and/or other materials provided
* with the distribution
*
* Redistribution and use in binary form, without modification, are permitted provided
* that the following conditions are met:
*
* *       No reverse engineering, decompilation, or disassembly of this software is
* permitted with respect to any software provided in binary form.
*
* *       any redistribution and use are licensed by TI for use only with TI Devices.
*
* *       Nothing shall obligate TI to provide you with source code for the software
* licensed and provided to you in object code.
*
* If software source code is provided to you, modification and redistribution of the
* source code are permitted provided that the following conditions are met:
*
* *       any redistribution and use of the source code, including any resulting derivative
* works, are licensed by TI for use only with TI Devices.
*
* *       any redistribution and use of any object code compiled from the source code
* and any resulting derivative works, are licensed by TI for use only with TI Devices.
*
* Neither the name of Texas Instruments Incorporated nor the names of its suppliers
*
* may be used to endorse or promote products derived from this software without
* specific prior written permission.
*
* DISCLAIMER.
*
* THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
* OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

/**
 *  \file     McspiApp.h
 *
 *  \brief    McSPI example header file
 *
 */

#ifndef MCSPI_APP_H_
#define MCSPI_APP_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
extern "C"
{
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define APP_NAME                        "SPI_APP"

/* Default loop count. */
#define SPI_APP_LOOP_COUNT              (1U)

/* Performance loop count. */
#define SPI_APP_PERF_LOOP_COUNT         (1000U)

/** \brief RX/TX buffer size for allocation in words */
#define RX_TX_BUF_SIZE_WORD             (10000U)

/** \brief Default RX/TX transfer length in words */
#define SPI_APP_RX_TX_LENGTH            (RX_TX_BUF_SIZE_WORD)

/** \brief Default clock divider - clock is 48MHz */
#define SPI_APP_DEFAULT_DIVIDER         (0U)

/**
 *  \brief Clock divider used for interruptible sequence testing. The SPI clock
 *  should be lower so that the second sequence gets scheduled. Otherwise
 *  the CPU will fully execute the first sequence in ISR as the McSPI data rate
 *  is faster than the code execution.
 */
#define SPI_APP_INTR_SEQ_DIVIDER        (47U)

/** \brief Default ASYNC/SYNC Transmit flag - TRUE: Async, FALSE: Sync */
#define SPI_APP_DEFAULT_ASYNC_SYNC      (TRUE)

/** \brief Default ASYNC mode - interrupt or polled */
#define SPI_APP_DEFAULT_ASYNC_MODE      (SPI_INTERRUPT_MODE)

/** \brief RX/TX transfer length used for ramp-up test */
#define SPI_APP_RX_TX_LENGTH_RAMP_UP    (0xFFFFU)
/** \brief RX/TX transfer length used for ramp-down test */
#define SPI_APP_RX_TX_LENGTH_RAMP_DOWN  (0xFFFEU)
/** \brief Ramp test loop count. */
#define SPI_APP_RAMP_LOOP_COUNT         (100U)
/** \brief Clock ramp test clock divider */
#define SPI_APP_RAMP_DIVIDER            (0xFFFFFFFFU)

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/**
 *  \brief SPI application test parameters.
 *  The test case execution happens based on values of this structure
 */
typedef struct
{
    char               *testDescStr;
    /**< Test description. */
    uint32              loopCount;
    /**< Number of iteration to run transmit. */
    uint32              length[SPI_MAX_CHANNELS];
    /**< Number of words to transmit per call per channel. */
    uint32              disableDataCheck;
    /**< Disable data check - used for performance. */
    uint32              isAsyncMode;
    /**< Whether to use async transmit or sync transmit. */
    Spi_AsyncModeType   asyncMode;
    /**< Whether to use polled or interrupt mode in async transmit. */
    Spi_ConfigType      spiConfig;
    /**< SPI configuration */
	Spi_ChannelConfigType_PC spiChannelConfig_PC[SPI_MAX_CHANNELS];
	/**< SPI Channel PC configuration */
	Spi_JobConfigType_PC spiJobConfig_PC[SPI_MAX_JOBS];
	/**< SPI Job PC configuration */
	Spi_SeqConfigType_PC spiSeqConfig_PC[SPI_MAX_SEQ];
	/**< SPI Sequence PC configuration */
    uint32              testId;
    /**< Test ID for print purpose. This is initialized in the code */
} SpiApp_TestParams;

/**
 *  \brief Test application data structure.
 */
typedef struct
{
    SpiApp_TestParams   testPrms;
    /**< Current test parameters. */
    uint32              enableAutoRun;
    /**< Auto-run enable flag. */
} SpiApp_Obj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

void SpiApp_McuMcspiJob0EndNotification(void);
void SpiApp_McuMcspiSeq0EndNotification(void);
void SpiApp_McuMcspi3Job0EndNotification(void);
void SpiApp_McuMcspi3Seq0EndNotification(void);
void SpiApp_McuMcspiJob1EndNotification(void);
void SpiApp_McuMcspiSeq1EndNotification(void);
void SpiApp_McspiJob0EndNotification(void);
void SpiApp_McspiSeq0EndNotification(void);
void SpiApp_McspiJob1EndNotification(void);
void SpiApp_McspiSeq1EndNotification(void);
void SpiApp_McspiJob2EndNotification(void);
void SpiApp_McspiSeq2EndNotification(void);

#define SPI_APP_NUM_TESTS               (sizeof(gSpiAppTestPrms) / \
                                         sizeof(gSpiAppTestPrms[0U]))

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef MCSPI_APP_H_ */
