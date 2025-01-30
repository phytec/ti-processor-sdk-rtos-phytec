
SRCDIR = . src/freertos src/nonos arch/core
INCDIR = . src/freertos src/nonos arch/core

SRCS_COMMON += SwiP_nonos.c DebugP_nonos.c RegisterIntr_nonos.c Core_utils.c TimerP_restrictedTimers.c
SRCS_COMMON += SemaphoreP_freertos.c TaskP_freertos.c MemoryP_freertos.c Utils_freertos.c
SRCS_COMMON += ClockP_freertos.c MailboxP_freertos.c MutexP_freertos.c QueueP_nonos.c StartuphooksP_freertos.c
SRCS_COMMON += HeapP_freertos.c HeapP_freertos_internal.c EventP_freertos.c LoadP_freertos.c

PACKAGE_SRCS_COMMON = makefile HwiP.h SwiP.h MuxIntcP.h osal.h osal_component.mk SemaphoreP.h MemoryP.h TaskP.h StartuphooksP.h \
                      TimerP.h RegisterIntr.h EventCombinerP.h CycleprofilerP.h CacheP.h DebugP.h MailboxP.h MutexP.h ClockP.h QueueP.h soc/osal_soc.h soc/$(SOC) \
                      src/nonos/Nonos_config.h src/nonos/SwiP_nonos.c src/nonos/CycleprofilerP_nonos.c \
                      src/nonos/RegisterIntr_nonos.c src/freertos/SemaphoreP_freertos.c src/freertos/TaskP_freertos.c src/freertos/Utils_freertos.c src/nonos/DebugP_nonos.c \
                      src/freertos/MemoryP_freertos.c src/freertos/ClockP_freertos.c src/freertos/MailboxP_freertos.c src/freertos/MutexP_freertos.c src/nonos/QueueP_nonos.c \
                      HeapP.h src/freertos/HeapP_freertos.c src/freertos/HeapP_freertos_internal.c src/freertos/HeapP_freertos_internal.h EventP.h src/freertos/EventP_freertos.c \
                      LoadP.h src/freertos/LoadP_freertos.c src/freertos/StartuphooksP_freertos.c TimerP_restrictedTimers.c \
                      src/src_common_freertos.mk \
                      arch/core/Core_utils.c docs/OSAL_SoftwareManifest.html docs/ReleaseNotes_OSAL.pdf \
                      build \
                      osalver.h

ifeq ($(SOC),$(filter $(SOC), j721e j721s2 j784s4))
  SRCDIR      += soc/$(SOC)
  SRCS_COMMON += TimerP_default.c CycleprofilerP_nonos.c
endif

ifeq ($(SOC),$(filter $(SOC), j7200))
  SRCDIR      += soc/$(SOC)
  SRCS_COMMON += TimerP_default_$(ISA).c CycleprofilerP_nonos.c
endif

ifeq ($(CORE),$(filter $(CORE), mcu1_0 mcu2_0 mcu2_1 mcu1_1 mcu3_0 mcu3_1 mcu4_0 mcu4_1))
  SRCDIR += arch/core/r5
  SRCS_COMMON += CacheP_nonos.c Arch_util.c
  SRCS_ASM_COMMON += TimestampProvider_asm.asm
  PACKAGE_SRCS_COMMON += arch/core/r5
endif

ifeq ($(CORE),$(filter $(CORE), c66x c66xdsp_1 c66xdsp_2 c674x))
  SRCDIR += arch/core/c6x
  SRCS_COMMON += CacheP_nonos.c Arch_util.c EventCombinerP_nonos.c
  PACKAGE_SRCS_COMMON += arch/core/c6x src/nonos/EventCombinerP_nonos.c
endif

ifeq ($(CORE),$(filter $(CORE), c7x_1 c7x_2 c7x_3 c7x_4))
  SRCS_COMMON += HwiP_freertos_c7x.c CacheP_freertos_c7x.c freertos_mmu.c
  PACKAGE_SRCS_COMMON += src/freertos/CacheP_freertos_c7x.c
  PACKAGE_SRCS_COMMON += src/freertos/HwiP_freertos_c7x.c
else
  SRCS_COMMON += HwiP_freertos.c
  PACKAGE_SRCS_COMMON += src/freertos/HwiP_freertos.c
endif
