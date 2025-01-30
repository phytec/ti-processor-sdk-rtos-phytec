ifeq ($(TARGET_PLATFORM), J7_SDK)
#Disable test bench build for MCU+SDK
ifneq ($(RTOS_SDK),mcu_plus_sdk)
include $(PRELUDE)

TARGET      := j7-c71_0-fw
override BIN_EXT :=
TARGETTYPE  := exe
CSOURCES    := $(call all-c-files)

# DSP build needs extenal getopt() support
CSOURCES    += ../../../../common/getopt/getopt.c
IDIRS += $($(_MODULE)_SDIR)/../../../../common/getopt

# DSP build needs XDC support
ifeq ($(RTOS),SYSBIOS)
	XDC_BLD_FILE = $($(_MODULE)_SDIR)/config_c71.bld
	XDC_IDIRS    = $($(_MODULE)_SDIR)
	XDC_CFG_FILE = $($(_MODULE)_SDIR)/c7x_1.cfg
	XDC_PLATFORM = "ti.platforms.tms320C7x:J7ES"
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/linker.cmd
else
	LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/c7x_linker_freertos.cmd
endif

# DSP build needs linker command files for memory maps
LINKER_CMD_FILES +=  $($(_MODULE)_SDIR)/linker_mem_map.cmd

# DSP build needs CGT, BIOS, and XDC include files
IDIRS += $(CGT7X_ROOT)/include
ifeq ($(RTOS),SYSBIOS)
	IDIRS += $(BIOS_PATH)/packages
	IDIRS += $(XDCTOOLS_PATH)/packages
endif

LDIRS += $(CGT7X_ROOT)/lib
# library search dirs are always platform specific
ifeq ($(RTOS_SDK),mcu_plus_sdk)
  ifeq ($(TARGET_SOC),$(filter $(TARGET_SOC), AM62A am62a))
    ADDITIONAL_STATIC_LIBS += dmautils.am62ax.c75x.ti-c7x-hostemu.$(TARGET_BUILD).lib
  else ifeq ($(TARGET_SOC),$(filter $(TARGET_SOC), J722S j722s))
    ADDITIONAL_STATIC_LIBS += dmautils.j722s.c75ssx-0.ti-c7x-hostemu.$(TARGET_BUILD).lib
  endif
  LDIRS += $(DMA_UTILS_PATH)/lib
else
  ifeq ($(TARGET_SOC),$(filter $(TARGET_SOC), am62a AM62A))
    ADDITIONAL_STATIC_LIBS += mailbox.ae71  
    LDIRS += $(PDK_PATH)/ti/csl/lib/$(SOC)/c75x/$(TARGET_BUILD)
    LDIRS += $(PDK_PATH)/ti/osal/lib/freertos/$(SOC)/c75x/$(TARGET_BUILD)
    LDIRS += $(PDK_PATH)/ti/drv/mailbox/lib/$(SOC)/c7x_1/$(TARGET_BUILD)
  else
    ADDITIONAL_STATIC_LIBS += udma.ae71
    ADDITIONAL_STATIC_LIBS += sciclient.ae71
    ADDITIONAL_STATIC_LIBS += ti.osal.ae71
	
    LDIRS += $(PDK_PATH)/ti/osal/lib/freertos/$(SOC)/c7x/$(TARGET_BUILD)
    LDIRS += $(PDK_PATH)/ti/drv/sciclient/lib/$(SOC)/c7x_1/$(TARGET_BUILD)	
  endif
  ADDITIONAL_STATIC_LIBS += dmautils.ae71
  ADDITIONAL_STATIC_LIBS += ti.csl.ae71
  LDIRS += $(PDK_PATH)/ti/drv/udma/lib/$(SOC)/c7x_1/$(TARGET_BUILD)
  LDIRS += $(PDK_PATH)/ti/csl/lib/$(SOC)/c7x/$(TARGET_BUILD)

  ADDITIONAL_STATIC_LIBS += ipc.ae71  
  
  LDIRS += $(PDK_PATH)/ti/kernel/lib/$(SOC)/c7x_$(C7X_CORE_ID)/$(TARGET_BUILD)
  LDIRS += $(PDK_PATH)/ti/drv/ipc/lib/$(SOC)/c7x_$(C7X_CORE_ID)/$(TARGET_BUILD)
  ADDITIONAL_STATIC_LIBS += $(DMA_LIBS)
  ADDITIONAL_STATIC_LIBS += ti.kernel.freertos.ae71
endif

ifeq ($(RTOS),SYSBIOS)
	LDIRS += $(PDK_PATH)/ti/osal/lib/tirtos/$(SOC)/c7x/$(TARGET_BUILD)
else
	LDIRS += $(PDK_PATH)/ti/kernel/lib/$(SOC)/c7x_$(C7X_CORE_ID)/$(TARGET_BUILD)

	ifeq ($(TARGET_C7X_VERSION), C7504)
		LDIRS += $(PDK_PATH)/ti/osal/lib/freertos/$(SOC)/c75x/$(TARGET_BUILD)
	else
		ifeq ($(RTOS),FREERTOS)
			LDIRS += $(PDK_PATH)/ti/osal/lib/freertos/$(SOC)/c7x/$(TARGET_BUILD)
		endif
		ifeq ($(RTOS),SAFERTOS)
			LDIRS += $(PDK_PATH)/ti/osal/lib/safertos/$(SOC)/c7x/$(TARGET_BUILD)
		endif
	endif
endif

LDIRS += $(PDK_PATH)/ti/drv/sciclient/lib/$(SOC)/c7x_$(C7X_CORE_ID)/$(TARGET_BUILD)
LDIRS += $(PDK_PATH)/ti/drv/udma/lib/$(SOC)/c7x_$(C7X_CORE_ID)/$(TARGET_BUILD)
LDIRS += $(MMALIB_PATH)/lib/$(TARGET_C7X_VERSION)/$(TARGET_BUILD)
# path to tidl_algo and tidl_priv_algo
ifeq ($(TIDL_BUILD_PATHS), LEGACY)
LDIRS += $($(_MODULE)_SDIR)/../../../lib/dsp/algo/$(TARGET_BUILD)
else
LDIRS += $($(_MODULE)_SDIR)/../../../lib/$(TARGET_SOC)/dsp/algo/$(TARGET_BUILD)
endif

# External libraries
ifeq ($(TARGET_SOC),$(filter $(TARGET_SOC), am62a AM62A))
DEFS += DMA_UTILS_STANDALONE
endif

ADDITIONAL_STATIC_LIBS += $(DMA_LIBS)

ADDITIONAL_STATIC_LIBS += common_$(TARGET_C7X_VERSION).lib
ADDITIONAL_STATIC_LIBS += mmalib_$(TARGET_C7X_VERSION).lib
ADDITIONAL_STATIC_LIBS += mmalib_cn_$(TARGET_C7X_VERSION).lib




# internal libraries
ADDITIONAL_STATIC_LIBS += tidl_algo.lib
ADDITIONAL_STATIC_LIBS += tidl_obj_algo.lib
ADDITIONAL_STATIC_LIBS += tidl_priv_algo.lib

# Custom Library
ADDITIONAL_STATIC_LIBS += tidl_custom.lib

ADDITIONAL_STATIC_LIBS += rts$(SI_VER)_le.lib

#
# Suppress this warning, 10063-D: entry-point symbol other than "_c_int00" specified
# c7x boots in secure mode and to switch to non-secure mode we need to start at a special entry point '_c_int00_secure'
# and later after switching to non-secure mode, sysbios jumps to usual entry point of _c_int00
# Hence we need to suppress this warning
CFLAGS+=--diag_suppress=10063

CFLAGS += --advice:performance=none

DEFS += DDR_BW_STATS
DEFS += BUILD_C7X

# get the common make flags from test/src/<plat>/../concerto_common.mak
include $($(_MODULE)_SDIR)/../concerto_common.mak

include $(FINALE)

endif #iifneq ($(RTOS_SDK),mcu_plus_sdk)
endif #ifeq ($(TARGET_PLATFORM), J7_SDK)