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
 *  \file Adc_Utils.h
 *
 *  \brief ADC utility functions header file
 *  This file declares the functions required to create, add and remove nodes.
 *
 */

#ifndef ADC_UTILS_H_
#define ADC_UTILS_H_

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include "Std_Types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/**< \brief Typedef for Node structure. */
typedef struct Adc_UtilsNode_t Adc_UtilsNode;

/**
 *  struct Adc_UtilsNode_t
 *  \brief Self referential structure for double link list.
 */
struct Adc_UtilsNode_t
{
    Adc_UtilsNode        *next;
    /**< Pointer to the next node. */
    Adc_UtilsNode        *prev;
    /**< Pointer to the previous node. */
    void                 *data;
    /**< Node data pointer. */
    Adc_GroupPriorityType priority;
    /**< Priority of the node. Used for priority based linked list. */
};

/**
 *  struct Adc_UtilsLinkListObj
 *  \brief Structure to the link list object information.
 */
typedef struct
{
    Adc_UtilsNode *headNode;
    /**< Head node reference pointer */
    Adc_UtilsNode *tailNode;
    /**< Tail node reference pointer */
} Adc_UtilsLinkListObj;

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

/**
 *  \brief Init a link list object.
 *
 *  \param llobj            Link list object.
 */
void Adc_utilsInitLinkList(Adc_UtilsLinkListObj *llobj);

/**
 *  \brief De-init a link list object.
 *
 *  \param llobj            Link list object.
 */
void Adc_utilsDeInitLinkList(Adc_UtilsLinkListObj *llobj);

/**
 *  \brief Links a node to the linked list.
 *  The memory to the node object should be allocated by the caller. This
 *  is used for link list with priority.
 *
 *  \param llobj            Link list object.
 *  \param node             Node object pointer used for linking.
 *  \param data             Data pointer to add to node.
 *  \param priority         Priority of the node used for priority based
 *                          addition of nodes. Priority is in ascending order
 *                          of the value. So 0 is the lowest priority and is
 *                          added to the bottom of the node.
 *                          Nodes with the same priority are always added to
 *                          the bottom of the existing nodes with same
 *                          priority.
 *                          For non-priority based modes, this parameter
 *                          is ignored and could be set to 0.
 *  \param isPaused         Flag to indiate if this group is paused mid way
 *                          and is queued back. This is required to put
 *                          this group ahead of the groups with the same
 *                          priority as this group so that this group is given
 *                          precedence to restart ahead of other groups of
 *                          same priority
 */
void Adc_utilsLinkNodePri(Adc_UtilsLinkListObj *llobj,
                          Adc_UtilsNode        *node,
                          void                 *data,
                          Adc_GroupPriorityType priority,
                          uint32                isPaused);

/**
 *  \brief Unlinks the node from the list. Used for the priority link lists.
 *
 *  \param llobj            Link list object.
 *  \param node             Node pointer to be unlinked from the list.
 */
void Adc_utilsUnLinkNodePri(Adc_UtilsLinkListObj *llobj, Adc_UtilsNode *node);

/**
 *  \brief Initialize the node object with default value.
 *
 *  \param node             Node pointer to initialize.
 */
void Adc_utilsInitNodeObject(Adc_UtilsNode *node);

/**
 *  \brief                  Returns the reference to the head node. This does
 *                          not remove the node from the head.
 *
 *  \param llobj            Link list object.
 */
Adc_UtilsNode *Adc_utilsGetHeadNode(const Adc_UtilsLinkListObj *llobj);

/* ========================================================================== */
/*                       Static Function Definitions                          */
/* ========================================================================== */

/* None */

#ifdef __cplusplus
}
#endif

#endif  /* #ifndef ADC_UTILS_H_ */
