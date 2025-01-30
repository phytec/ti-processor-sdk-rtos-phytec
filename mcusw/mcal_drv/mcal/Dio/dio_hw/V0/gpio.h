/**
 *  \file   gpio.h
 *
 *  \brief  File provides functional level API prototypes for GPIO interface.
 *
 */

/*
 * Copyright (C) 2024 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef  GPIO_V0_H_
#define  GPIO_V0_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <Dio/dio_hw/V0/lldr_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif
/* ========================================================================== */
/*                             Macros & Typedefs                              */
/* ========================================================================== */

/** \brief GPIO pin is at logic low.*/
#define GPIO_PIN_LOW                     (0x0U)
/** \brief GPIO pin is at logic high.*/
#define GPIO_PIN_HIGH                    (0x1U)

/** \brief Macro to configure GPIO pin as output */
#define GPIO_OE_OUTPUTEN_N_ENABLED       (0U)

/** \brief Macro to configure GPIO pin as input */
#define GPIO_OE_OUTPUTEN_N_DISABLED      (1U)

/** \brief Mask for all pins of single gpio port */
#define GPIO_PIN_MASK_ALL   0xFF

/**
 *  \anchor gpioIntrMask_t
 *  \name GPIO Interrupt mask
 *  @{
 */
/** \brief Enumerates the possible GPIO interrupt generation events. */
typedef uint32 gpioIntrMask_t;
#define GPIO_INTR_MASK_NO_EDGE              (0x01U)
/**< No interrupt request on either rising or
       falling edges on the pin. */
#define GPIO_INTR_MASK_RISE_EDGE            (0x02U)
/**< Interrupt request on occurrence of a rising edge
       on the input GPIO pin. */
#define GPIO_INTR_MASK_FALL_EDGE            (0x04U)
/**< Interrupt request on occurrence of a falling edge
       on the input GPIO pin. */
#define GPIO_INTR_MASK_BOTH_EDGE            (0x08U)
/**< Interrupt request on occurrence of both a rising
       and a falling edge on the pin. */
/* @} */

/**
 *  \anchor gpioDirection_t
 *  \name GPIO Direction
 *  @{
 */
/** \brief Enumerates the direction of GPIO pin. */
typedef uint32 gpioDirection_t;
#define GPIO_DIRECTION_INPUT                (GPIO_OE_OUTPUTEN_N_DISABLED)
/**< Input pin. */
#define GPIO_DIRECTION_OUTPUT               (GPIO_OE_OUTPUTEN_N_ENABLED)
/**< Output pin. */
/* @} */

/* ========================================================================== */
/*                         Structures and Enums                               */
/* ========================================================================== */

/** \brief Structure holding the GPIO context. */
typedef struct gpioContext
{
    uint32 ctrl;
    /**< GPIO control register value. */
    uint32 dir;
    /**< GPIO direction control register value. */
    uint32 data;
    /**< GPIO pin value register. */
}gpioContext_t;

/* ========================================================================== */
/*                         Global Variables Declarations                      */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef GPIO_V0_H_ */
