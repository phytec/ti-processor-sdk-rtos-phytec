;******************************************************************************
;* BOOT  v16.9.4                                                              *
;*                                                                            *
;* Copyright (c) 1996-2019 Texas Instruments Incorporated                     *
;* http://www.ti.com/                                                         *
;*                                                                            *
;*  Redistribution and  use in source  and binary forms, with  or without     *
;*  modification,  are permitted provided  that the  following conditions     *
;*  are met:                                                                  *
;*                                                                            *
;*     Redistributions  of source  code must  retain the  above copyright     *
;*     notice, this list of conditions and the following disclaimer.          *
;*                                                                            *
;*     Redistributions in binary form  must reproduce the above copyright     *
;*     notice, this  list of conditions  and the following  disclaimer in     *
;*     the  documentation  and/or   other  materials  provided  with  the     *
;*     distribution.                                                          *
;*                                                                            *
;*     Neither the  name of Texas Instruments Incorporated  nor the names     *
;*     of its  contributors may  be used to  endorse or  promote products     *
;*     derived  from   this  software  without   specific  prior  written     *
;*     permission.                                                            *
;*                                                                            *
;*  THIS SOFTWARE  IS PROVIDED BY THE COPYRIGHT  HOLDERS AND CONTRIBUTORS     *
;*  "AS IS"  AND ANY  EXPRESS OR IMPLIED  WARRANTIES, INCLUDING,  BUT NOT     *
;*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR     *
;*  A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT     *
;*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,     *
;*  SPECIAL,  EXEMPLARY,  OR CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT  NOT     *
;*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     *
;*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY     *
;*  THEORY OF  LIABILITY, WHETHER IN CONTRACT, STRICT  LIABILITY, OR TORT     *
;*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE     *
;*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.      *
;*                                                                            *
;******************************************************************************

;****************************************************************************
;* resetvecs.asm
;*
;* THIS IS THE INITAL BOOT ROUTINE FOR TMS470 C++ PROGRAMS.
;* IT MUST BE LINKED AND LOADED WITH ALL C++ PROGRAMS.
;*
;*
;*
;****************************************************************************
   .if  __TI_ARM_V7M__ | __TI_ARM_V6M0__
	.thumbfunc _resetvectors
   .else
	.armfunc _resetvectors
   .endif

    .global	SDL_undefInstructionExptnHandler
    .global	SDL_swIntrExptnHandler
    .global	SDL_prefetchAbortExptnHandler
    .global	SDL_dataAbortExptnHandler
    .global	rsvdExptnHandler
    .global	SDL_irqExptnHandler
    .global	SDL_fiqExptnHandler
    .ref    _c_int00
;****************************************************************************
; Setup reserved Handler
;****************************************************************************
    .global	_sdlRsvdHandler
_sdlRsvdHandler:
    
     b   _sdlRsvdHandler
    
;****************************************************************************
; Setup Reset Vectors always in ARM mode
;****************************************************************************
    .arm
	.global	_resetvectors
    .sect   ".rstvectors"
_resetvectors:
    
        LDR pc, c_int00_addr        ; Reset
        LDR pc, undefInst_addr      ; Undefined Instruction
        LDR pc, swi_addr            ; Software interrupt
        LDR pc, preAbort_addr       ; Abort (prefetch)
        LDR pc, dataAbort_addr      ; Abort (data)
        LDR pc, rsvd_addr            ; rsvd
        LDR pc, irq_addr             ; IRQ
        LDR pc, fiq_addr             ; FIQ
    


c_int00_addr .long _c_int00
undefInst_addr .long SDL_undefInstructionExptnHandler
swi_addr .long SDL_swIntrExptnHandler
preAbort_addr .long SDL_prefetchAbortExptnHandler
dataAbort_addr .long SDL_dataAbortExptnHandler
rsvd_addr .long _sdlRsvdHandler
irq_addr .long SDL_irqExptnHandler
fiq_addr .long SDL_fiqExptnHandler
	.end
