/**
 *  \file   monitor.c
 *
 *  \brief  CAN Profiling XIP FOTA application monitor file.
 *
 */
 /*
 *  Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
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
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "fota_monitor.h"
#include "fota_writer.h"

/* UART Header files */
#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>

#include <ti/osal/TaskP.h>

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

inline uint32_t MonitorApp_OspiRegFldExtract(volatile uint32_t *p,
                                                    uint32_t mask,
                                                    uint32_t shift) __attribute__((section(".fota_writer_app")));

inline void MonitorApp_OspiRegFldInsert(volatile uint32_t *p,
                                                uint32_t mask,
                                                uint32_t shift,
                                                uint32_t v) __attribute__((section(".fota_writer_app")));


/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/

extern TaskP_Handle gCanTask;
extern SemaphoreP_Handle WriterApp_WriteCompletionSemaphore;
extern SemaphoreP_Handle WriterApp_FlashRequestSemaphore;
extern SemaphoreP_Handle WriterApp_FlashAccessSemaphore;
extern SemaphoreP_Handle WriterApp_ChunkWriteCompleteSemaphore;
extern SemaphoreP_Handle MonitorApp_ShutdownRequestSemaphore;

inline uint32_t MonitorApp_OspiRegFldExtract(volatile uint32_t *p,
                                                    uint32_t mask,
                                                    uint32_t shift)
{
    uint32_t regVal = *p;
    regVal = (regVal & mask) >> shift;
    return (regVal);
}

inline void MonitorApp_OspiRegFldInsert(volatile uint32_t *p,
                                                uint32_t mask,
                                                uint32_t shift,
                                                uint32_t v)
{
    uint32_t regVal = *p;
    regVal = (regVal & ~(mask));
    regVal |= (v << shift) & mask;
    *p = regVal;
    return;
}

/** \brief TBD: Authenticate the update application image with OTFA and ECC */
void MonitorApp_AuthenticateImage()
{
    return;
}

/** \brief 
 * Setup environment for the writer task 
 * Erase the blocks to which update image would be written
 * Initialize the semaphores
 */
int32_t MonitorApp_InitWriter(uint32_t *updateImageOffset)
{
    SemaphoreP_Params semParams;
    uintptr_t       key;
    uint32_t        tskKey;
    int32_t         retVal = (int32_t)(0);
    uint32_t        blockNum;     /* flash block number */
    uint32_t        pageNum;      /* flash page number */
    uint32_t        jumpInstrOffset = BK_REGION_OFFSET;
    uint32_t        jumpInstrLocation = (OSPI_FLASH_DATA_BASE_ADDR+jumpInstrOffset);
    uint32_t        currentImageLocation;
    uint32_t        offset;
    uint32_t        i;
    volatile uint32_t delay;

    key = HwiP_disable();
    tskKey = TaskP_disable();

    currentImageLocation = CSL_REG32_RD(jumpInstrLocation);

    if( (currentImageLocation - OSPI_FLASH_DATA_BASE_ADDR) == P1_REGION_OFFSET)
    {
        *updateImageOffset = P2_REGION_OFFSET;
    }
    else if( (currentImageLocation - OSPI_FLASH_DATA_BASE_ADDR) == P2_REGION_OFFSET)
    {
        *updateImageOffset = P1_REGION_OFFSET;
    }
    else
    {
        *updateImageOffset   = P2_REGION_OFFSET;
    }

#if !defined(NOR_XSPI)
    /* Switch to INDAC mode */
    retVal += WriterApp_OspiSwitchMode(FALSE);
#endif

    /* Erase blocks, to which XIP sections have to be written */
    for(i = 0; i < UPDATE_APPIMAGE_XIP_SIZE; i+=OSPI_FLASH_BLOCK_SIZE)
    {
        offset = *updateImageOffset + i;
        retVal += WriterApp_OspiOffsetToBlkPage(offset, &blockNum, &pageNum);
        retVal += WriterApp_OspiErase(blockNum);
        /* TO DO: Find a better method to add the necessary delay here */
        retVal += WriterApp_WaitOspiIdle(OSPI_CHECK_IDLE_DELAY);
        for(delay = 0xFFFFFFF; delay>0U; delay--);
    }

    /* Erase blocks, to which non XIP sections have to be written */
    retVal += WriterApp_OspiOffsetToBlkPage(APPIMAGE_OFFSET, &blockNum, &pageNum);
    retVal += WriterApp_OspiErase(blockNum);
    /* TO DO: Find a better method to add the necessary delay here */
    retVal += WriterApp_WaitOspiIdle(OSPI_CHECK_IDLE_DELAY);
    for(delay = 0xFFFFFFF; delay>0U; delay--);

#if !defined(NOR_XSPI)
    /* Switch back to DAC mode */
    retVal += WriterApp_OspiSwitchMode(TRUE);
#endif

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    WriterApp_WriteCompletionSemaphore = SemaphoreP_create(0U, &semParams);

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    WriterApp_FlashRequestSemaphore = SemaphoreP_create(0U, &semParams);

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    WriterApp_FlashAccessSemaphore = SemaphoreP_create(0U, &semParams);

    SemaphoreP_Params_init(&semParams);
    semParams.mode = SemaphoreP_Mode_BINARY;
    WriterApp_ChunkWriteCompleteSemaphore = SemaphoreP_create(0U, &semParams);

    TaskP_restore(tskKey);
    HwiP_restore(key);

    return (retVal);
}

/** \brief Blocking function waiting for update completion */
uint32_t MonitorApp_IsUpdateComplete()
{
    uint32_t retVal = SemaphoreP_OK;

    /* Check for a signal from writer app that indicates update image write completion */
    retVal = SemaphoreP_pend(WriterApp_WriteCompletionSemaphore, SemaphoreP_WAIT_FOREVER);

    return (retVal);
}

/** \brief Requests and then waits for the shutdown of XIP application */
void MonitorApp_Shutdown()
{
    /* Request a shutdown of the CAN profiling app */
    SemaphoreP_post(MonitorApp_ShutdownRequestSemaphore);

    while(!TaskP_isTerminated(gCanTask))
    {
        /* Wait for the CAN Task to complete */
        TaskP_yield();
    }
    return;
}

/** \brief Updates the book keeping region with address of current image location */
int32_t MonitorApp_UpdateApplicationLocation(uint32_t updateImageOffset)
{
    int32_t retVal = (int32_t)(0);
    uint32_t jumpInstrOffset = BK_REGION_OFFSET;
    uint32_t updateImageLocation;
    uint32_t blockNum;     /* flash block number */
    uint32_t pageNum;      /* flash page number */
    uint8_t* buf;
    uintptr_t key;
    uint32_t tskKey;

    updateImageLocation = OSPI_FLASH_DATA_BASE_ADDR + updateImageOffset;
    buf = (uint8_t*)&updateImageLocation;

    key = HwiP_disable();
    tskKey = TaskP_disable();
#if !defined(NOR_XSPI)
    /* Switch to INDAC mode */
    retVal += WriterApp_OspiSwitchMode(FALSE);
#endif
    /* Convert offset into block number */
    retVal += WriterApp_OspiOffsetToBlkPage(jumpInstrOffset, &blockNum, &pageNum);
    /* Erase block, to which data has to be written */
    retVal += WriterApp_OspiErase(blockNum);
    /* TO DO: Find a better method to add the necessary delay here */
    retVal += WriterApp_WaitOspiIdle(OSPI_CHECK_IDLE_DELAY);
    volatile uint32_t delay;
    for(delay = 0xFFFFFFF; delay>0U; delay--);
#if defined(NOR_XSPI)
    /* Switch to INDAC mode */
    retVal += WriterApp_OspiSwitchMode(FALSE);
#endif
    /* Write buffer to flash */
    retVal += WriterApp_IndWrite(jumpInstrOffset, buf, 4U);
    /* Wait for the OSPI controller to be idle */
    retVal += WriterApp_WaitOspiIdle(OSPI_CHECK_IDLE_DELAY);
    /* Switch back to DAC mode */
    retVal += WriterApp_OspiSwitchMode(TRUE);
    TaskP_restore(tskKey);
    HwiP_restore(key);

#ifdef BUILD_P2
    volatile int loop = 1;
    while(1)
    {
        /* Demo complete - do not reset */
    }
#else
    retVal += Sciclient_pmDeviceReset(SCICLIENT_SERVICE_WAIT_FOREVER);
    return (retVal);
#endif
}
