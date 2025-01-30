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
 *  \file     os_wrapper.c
 *
 *  \brief    This file contains the wrapper code for freertos
 */
 #include "Std_Types.h"
#include <string.h>
#include "ti/osal/LoadP.h"
#include "os_wrapper.h"
/* ========================================================================== */
/*                            Funcion Declaration                             */
/* ========================================================================== */

#ifdef __cplusplus
extern "C" {
#endif

extern void _freertosresetvectors (void);
extern void OS_init(void);

#ifdef __cplusplus
}
#endif

/* ========================================================================== */
/*                            Funcion Definition                              */
/* ========================================================================== */

void OsUtils_init(void)
{
 	/* Relocate FreeRTOS Reset Vectors from BTCM*/
    memcpy((void *)0x0, (void *)_freertosresetvectors, 0x40);

    OS_init();
}

void* OsUtils_createTask(const char* name,
	uint8 priority,
	uint32 stackSize, uint8* stackMem,
	void (*func)(void *args, void* args1))
{
	TaskP_Handle hdl = NULL_PTR;
	TaskP_Params taskParams;
	TaskP_Params_init(&taskParams);
    taskParams.name      = name;
    taskParams.priority  = priority;
    taskParams.stack     = stackMem;
    taskParams.stacksize = stackSize;

    hdl = TaskP_create(func, &taskParams);
    return hdl;
}

void OsUtils_resetCpuload(void)
{
    LoadP_reset();
    LoadP_update();
}

uint32 OsUtils_getCpuload(void* handle)
{
    LoadP_Stats stats;
    LoadP_getTaskLoad((TaskP_Handle)handle, &stats);
    return stats.percentLoad;
}

uint32 OsUtils_getCpuloadall(void)
{
    return LoadP_getCPULoad();
}

void vApplicationIdleHook( void )
{
    /* nothing */
}