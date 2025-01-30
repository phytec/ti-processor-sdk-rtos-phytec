#ifndef CSL_DSPCACHEC7_H_
#define CSL_DSPCACHEC7_H_

/**
 *  @file csl_cache.h
 *
 *  @brief
 *     Cache Management Instructions
 *  \par
 *   ================================================================================
 *
 *   @n   (C)  Copyright 2015 Texas Instruments Incorporated
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *      Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *      Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the
 *      distribution.
 *
 *      Neither the name of Texas Instruments Incorporated nor the names of
 *      its contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
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
 */



#ifdef __cplusplus
extern "C" {
#endif


    #include <stdint.h>


    /** ===========================================================================
     *
     * @defgroup CSL_DSPCACHEC7_API (C7x Cache Operations)
     * @ingroup CSL_DSPCACHEC7_API
     *
     * @section Introduction
     *
     * @subsection xxx Overview
     *
     * @subsection References
     *    -#
     * ============================================================================
     */





    /**
       @defgroup CSL_DSPCACHEC7_DATASTRUCT CPU Indepenent Cache Datastructs
       @ingroup CSL_DSPCACHEC7_API
    */
    /**
       @defgroup CSL_DSPCACHEC7_FUNCTION  CPU Indepenent Cache Functions
       @ingroup CSL_DSPCACHEC7_API
    */
    /**
       @defgroup CSL_DSPCACHEC7_ENUM  CPU Indepenent Cache Enumerated Data Types
       @ingroup CSL_DSPCACHEC7_API
    */

    //Function Prototypes


    /////////////////////////////////////////////////////////////////////////////
    //  Data Cache Enable and Disable Operations
    ////////////////////////////////////////////////////////////////////////////

    /** ==========================================================================
        @addtogroup CSL_DSPCACHEC7_FUNCTION
        @{
        *  ===========================================================================
        */



    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  DMC MAINTAINENCE OPERATIONS
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


    /**
     * @n@b CSL_c7xFreezeL1DCache
     *
     * @b Description
     * @n   Freezes the contents of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   CSL_c7xFreezeL1DCache();
     * @endverbatim
     */


    void CSL_c7xFreezeL1DCache();



    /**
     * @n@b CSL_c7xUnfreezeL1DCache
     *
     * @b Description
     * @n   Un-Freezes the contents of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   CSL_c7xUnfreezeL1DCache();
     * @endverbatim
     */


    void CSL_c7xUnfreezeL1DCache();


    /**
     * @n@b CSL_c7xStatusL1DCache
     *
     * @b Description
     * @n   Fetches the contents of the L1DCC register
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n uint64_t   value
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   value = CSL_c7xStatusL1DCache();
     * @endverbatim
     */


    uint64_t CSL_c7xStatusL1DCache();


    /**
     * @n@b CSL_c7xCleanL1DCache
     *
     * @b Description
     * @n   Cleans (forces writeback)  the contents of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   CSL_c7xCleanL1DCache();
     * @endverbatim
     */


    uint64_t CSL_c7xCleanL1DCache();



    /**
     * @n@b CSL_c7xCleaninvalidateL1DCache
     *
     * @b Description
     * @n   Cleans and Invalidates(forces writeback)  the contents of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *  CSL_c7xCleaninvalidateL1DCache();
     * @endverbatim
     */


    uint64_t CSL_c7xCleaninvalidateL1DCache();


    /**
     * @n@b CSL_c7xInvalidateL1DCache
     *
     * @b Description
     * @n  Invalidates the contents of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *  CSL_c7xInvalidateL1DCache();
     * @endverbatim
     */


    uint64_t CSL_c7xInvalidateL1DCache();



    /**
     * @n@b CSL_c7xSetL1DCacheSize
     *
     * @b Description
     * @n  Sets the size of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  uint64_t   size
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * uint64_t      l1dmod;
     *
     * if (cacheksize == 8) { l1dmod = 2}
     * else if (cacheksize == 16) { l1dmod = 3}
     * else if (cacheksize == 32) { l1dmod = 4}
     *
     *  CSL_c7xSetL1DCacheSize(l1dmod);
     * @endverbatim
     */


    uint64_t CSL_c7xSetL1DCacheSize(uint64_t l1dmod);


    /**
     * @n@b CSL_c7xGetL1DCacheSize
     *
     * @b Description
     * @n  Gets the size of the L1D Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n uint64_t   size
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * uint64_t      l1dmod;
     *
     *  l1dmod = CSL_c7xGetL1DCacheSize();
     * @endverbatim
     */


    uint64_t CSL_c7xGetL1DCacheSize();




    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  PMC MAINTAINENCE OPERATIONS
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

    /**
     * @n@b CSL_c7xInvalidateL1PCache
     *
     * @b Description
     * @n  Invalidates the L1P Cache Contents
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * CSL_c7xInvalidateL1PCache();
     * @endverbatim
     */


    void CSL_c7xInvalidateL1PCache();


    /**
     * @n@b CSL_c7xInvalidateL1PCacheUser
     *
     * @b Description
     * @n  Invalidates the L1P Cache Contents for User
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * CSL_c7xInvalidateL1PCacheUser();
     * @endverbatim
     */


    void CSL_c7xInvalidateL1PCacheUser();



    /**
     * @n@b CSL_c7xInvalidateL1PCacheSuper
     *
     * @b Description
     * @n  Invalidates the L1P Cache Contents for Supervisor
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * CSL_c7xInvalidateL1PCacheSuper();
     * @endverbatim
     */


    void CSL_c7xInvalidateL1PCacheSuper();



    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
    //  UMC MAINTAINENCE OPERATIONS
    //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



    /**
     * @n@b CSL_c7xSetL2CacheSize
     *
     * @b Description
     * @n  Sets the size of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  uint64_t l2mod
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * uint64_t        l2mod;
     *
     * if (cacheksize == 0) {l2mod = 0}
     * else if (cacheksize == 32) {l2mod = 1}
     * else if (cacheksize == 64) {l2mod = 2}
     * else if (cacheksize == 128) {l2mod = 3}
     * else if (cacheksize == 256) {l2mod = 4}
     * else if (cacheksize == 512) {l2mod = 5}
     * else if (cacheksize == 1024 {l2mod = 6}
     * else {l2mod = 7}
     *
     * CSL_c7xSetL2CahceSize(l2mod);
     * @endverbatim
     */


    void CSL_c7xSetL2CacheSize(uint64_t);




    /**
     * @n@b CSL_c7xGetL2CacheSize
     *
     * @b Description
     * @n  Gets the size of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n uint64_t l2mod
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * uint64_t        l2mod;
     *

     * l2mod = CSL_c7xGetL2CahceSize();
     * @endverbatim
     */


    uint64_t CSL_c7xGetL2CacheSize();



    /**
     * @n@b CSL_c7xFreezeL2Cache
     *
     * @b Description
     * @n   Freezes the contents of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   CSL_c7xFreezeL2Cache();
     * @endverbatim
     */


    void CSL_c7xFreezeL2Cache();



    /**
     * @n@b CSL_c7xUnfreezeL2Cache
     *
     * @b Description
     * @n   Un-Freezes the contents of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   CSL_c7xUnfreezeL2Cache();
     * @endverbatim
     */


    void CSL_c7xUnfreezeL2Cache();


    /**
     * @n@b CSL_c7xStatusL2CacheOper
     *
     * @b Description
     * @n   Get the status of the OPER field in L2CC
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n uint64_t    oper
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * uint64_t   oper;
     *
     *   oper = CSL_c7xStatusL2CacheOper();
     * @endverbatim
     */


    uint64_t CSL_c7xStatusL2CacheOper();




    /**
     * @n@b CSL_c7xStatusL2CachePoper
     *
     * @b Description
     * @n   Get the status of the POPER field in L2CC
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n uint64_t    poper
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     * uint64_t   poper;
     *
     *   oper = CSL_c7xStatusL2CachePoper();
     * @endverbatim
     */


    uint64_t CSL_c7xStatusL2CachePoper();





    /**
     * @n@b CSL_c7xCleanL2Cache
     *
     * @b Description
     * @n   Cleans (forces writeback)  the contents of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *   CSL_c7xCleanL2Cache();
     * @endverbatim
     */


    void CSL_c7xCleanL2Cache();



    /**
     * @n@b CSL_c7xCleaninvalidateL2Cache
     *
     * @b Description
     * @n   Cleans and Invalidates(forces writeback)  the contents of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *  CSL_c7xCleaninvalidateL2Cache();
     * @endverbatim
     */


    void CSL_c7xCleaninvalidateL2Cache();


    /**
     * @n@b CSL_c7xInvalidateL2Cache
     *
     * @b Description
     * @n  Invalidates the contents of the L2 Cache
     *
     *
     *
     * @b Arguments
     * @n  None
     *
     * <b> Return Value </b>
     * @n None
     *
     * <b> Pre Condition </b>
     * @n None
     *
     * <b> Post Condition </n>
     * @n None
     *
     * @b Reads
     * @n None
     *
     * @b Example
     * @verbatim
     *
     *
     *  CSL_c7xInvalidateL12ache();
     * @endverbatim
     */


    void CSL_c7xInvalidateL2Cache();







    /** @} */








#ifdef __cplusplus
}
#endif

#endif             // end of _CSL_CACHE_H_ definition
