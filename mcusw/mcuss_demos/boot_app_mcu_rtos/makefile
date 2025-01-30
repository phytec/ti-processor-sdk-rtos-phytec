# Default bootmode for Main domain cores is via MMCSD, unless specified at command line differently
BOOTMODE ?= ospi
# Default OSPI DMA to "no" (disabled).  Change to "yes", if building PDK SBL with -DSBL_USE_DMA=1
ENABLE_OSPI_DMA ?= no
# CAN Boot Function options are can_profiling, can_fast_response, or none
CANFUNC ?= can_fast_response
#CANFUNC ?= can_profiling
#CANFUNC ?= none
# Boot function options are enabled or disabled
BOOTFUNC = enabled
#BOOTFUNC ?= disabled
MCUONLYFUNC ?= disabled
#MCUONLYFUNC ?= enabled
# HLOS Boot options are linux, qnx, or none (rtos is loaded)
#HLOSBOOT ?= linux
#HLOSBOOT ?= qnx
HLOSBOOT ?= none
# Build for GP or HS
#DEVICETYPE ?= HS
DEVICETYPE ?= GP

# BIST options are enabled or disabled
#BISTFUNC ?= disabled
#Setting BISTFUNC to enabled is only supported on j721e
BISTFUNC ?= enabled

BUILD_OS_TYPE=freertos

$(info Building MCUSW CAN Boot App with BISTFUNC=$(BISTFUNC), BOOTFUNC=$(BOOTFUNC), HLOSBOOT=$(HLOSBOOT), BOOTMODE=$(BOOTMODE), CANFUNC=$(CANFUNC), DEVICETYPE=$(DEVICETYPE), BUILD_OS_TYPE=$(BUILD_OS_TYPE), ENABLE_OSPI_DMA=$(ENABLE_OSPI_DMA) and MCUONLYFUNC=$(MCUONLYFUNC))

include $(MCUSW_INSTALL_PATH)/build/Rules.make
include $(PDK_INSTALL_PATH)/ti/drv/spi/spi_component.mk
PDK_SBL_COMP_PATH = $(PDK_INSTALL_PATH)/ti/boot/sbl

APP_NAME = can_boot_app_mcu_rtos

SRCDIR = src $(mcal_PATH)
INCDIR = $(mcal_PATH)
SRCDIR += . $(autosarConfigSrc_PATH)/Can_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src $(autosarConfigSrc_PATH)/Dio_Demo_Cfg/output/generated/soc/$(SOC)/$(CORE)/src
SRCDIR += $(app_utils_PATH)/src
SRCDIR += soc/$(SOC)/
INCDIR += .

INCDIR += $(SDL_INSTALL_PATH)
INCDIR += $(SDL_INSTALL_PATH)/include
INCDIR += $(SDL_INSTALL_PATH)/src/sdl
INCDIR += $(SDL_INSTALL_PATH)/src/sdl/pbist
SRCDIR += $(SDL_INSTALL_PATH)/src/sdl/pbist
INCDIR += $(SDL_INSTALL_PATH)/src/sdl/lbist
SRCDIR += $(SDL_INSTALL_PATH)/src/sdl/lbist
SRCDIR += $(SDL_INSTALL_PATH)/src/sdl/pbist/soc/j721e
SRCDIR += $(SDL_INSTALL_PATH)/src/sdl/lbist/soc/j721e
INCDIR += $(SDL_INSTALL_PATH)/include/soc/j721e
INCDIR += $(SDL_INSTALL_PATH)/osal
SRCDIR += $(SDL_INSTALL_PATH)/osal/src
INCDIR += $(SDL_INSTALL_PATH)/test/osal
SRCDIR += $(SDL_INSTALL_PATH)/test/osal/src
INCDIR += $(SDL_INSTALL_PATH)/src/ip/pbist/V0
SRCDIR += $(SDL_INSTALL_PATH)/src/ip/pbist/V0
INCDIR += $(SDL_INSTALL_PATH)/src/ip/lbist/V0
SRCDIR += $(SDL_INSTALL_PATH)/src/ip/lbist/V0
INCDIR += $(SDL_INSTALL_PATH)/include
INCDIR += $(PDK_SBL_COMP_PATH)
INCDIR += $(PDK_SBL_COMP_PATH)/soc
INCDIR += $(PDK_SBL_COMP_PATH)/board/src
INCDIR += $(PDK_SBL_COMP_PATH)/src/rprc
INCDIR += $(PDK_SBL_COMP_PATH)/src/ospi
INCDIR += $(PDK_SBL_COMP_PATH)/src/mmcsd
INCDIR += $(PDK_SBL_COMP_PATH)/soc/k3

INCDIR += $(PDK_INSTALL_PATH)
INCDIR += $(PDK_INSTALL_PATH)/ti/board
ifeq ($(SOC),$(filter $(SOC), j7200))
INCDIR += $(PDK_INSTALL_PATH)/ti/board/src/j7200_evm/include
else ifeq ($(SOC),$(filter $(SOC), j721e))
INCDIR += $(PDK_INSTALL_PATH)/ti/board/src/j721e_evm/include
else ifeq ($(SOC),$(filter $(SOC), j721s2))
INCDIR += $(PDK_INSTALL_PATH)/ti/board/src/j721s2_evm/include
else ifeq ($(SOC),$(filter $(SOC), j784s4))
INCDIR += $(PDK_INSTALL_PATH)/ti/board/src/j784s4_evm/include
endif
INCDIR += $(PDK_INSTALL_PATH)/ti/board/src/flash/include
INCDIR += $(PDK_INSTALL_PATH)/ti/board/src/flash/nor

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
INCDIR += soc/$(SOC)/
endif

# List all the external components/interfaces, whose interface header files
# need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = pdk autosarBSW autosarConfig autosarCompiler

INCLUDE_INTERNAL_INTERFACES = can bsw_stubs_rtos app_utils dio demo_utils board

# List all the components required by the application
COMP_LIST_COMMON = can bsw_stubs_rtos app_utils dio demo_utils

# List all the external ibs required by the application
# Required for sciclient
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
  ifeq ($(CORE),mcu1_0)
  EXT_LIB_LIST_COMMON += $(sciclient_direct_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_direct_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(sciserver_tirtos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciserver_tirtos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(rm_pm_hal_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(rm_pm_hal_LIBNAME).$(LIBEXT)
  else
  EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
  endif
else
  EXT_LIB_LIST_COMMON += $(sciclient_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(sciclient_LIBNAME).$(LIBEXT)
endif
# Required for PM (for timer setup)
EXT_LIB_LIST_COMMON += $(pm_lib_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(pm_lib_LIBNAME).$(LIBEXT)
# Required for uart
EXT_LIB_LIST_COMMON += $(uart_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(uart_LIBNAME).$(LIBEXT)
#Board module
EXT_LIB_LIST_COMMON += $(board_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(board_LIBNAME).$(LIBEXT)
# Required by board
EXT_LIB_LIST_COMMON += $(i2c_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(i2c_LIBNAME).$(LIBEXT)

ifeq ($(BOOTMODE), mmcsd)
    ifeq ($(DEVICETYPE), GP)
        COMP_LIST_COMMON += mmcsd sbl_lib_mmcsd fatfs_indp udma
    endif
    ifeq ($(DEVICETYPE), HS)
        COMP_LIST_COMMON += mmcsd sbl_lib_mmcsd_hs fatfs_indp udma
    endif

    EXT_LIB_LIST_COMMON += $(udma_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE)/$(udma_LIBNAME).$(LIBEXT)

    # Required for sbl
    ifeq ($(DEVICETYPE), GP)
        EXT_LIB_LIST_COMMON += $(sbl_lib_mmcsd_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(sbl_lib_mmcsd_LIBNAME).$(LIBEXT)
    endif
    ifeq ($(DEVICETYPE), HS)
        EXT_LIB_LIST_COMMON += $(sbl_lib_mmcsd_hs_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(sbl_lib_mmcsd_hs_LIBNAME).$(LIBEXT)
    endif
    EXT_LIB_LIST_COMMON += $(fatfs_indp_LIBPATH)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(fatfs_indp_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(mmcsd_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(mmcsd_LIBNAME).$(LIBEXT)
endif
ifeq ($(BOOTMODE), ospi)
  ifeq ($(ENABLE_OSPI_DMA), yes)
    ifeq ($(DEVICETYPE), GP)
        COMP_LIST_COMMON += board sbl_lib_cust udma spi_dma
    endif
    ifeq ($(DEVICETYPE), HS)
        COMP_LIST_COMMON += board sbl_lib_cust_hs udma spi_dma
    endif

    EXT_LIB_LIST_COMMON += $(udma_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE)/$(udma_LIBNAME).$(LIBEXT)
    EXT_LIB_LIST_COMMON += $(spi_dma_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(spi_dma_LIBNAME).$(LIBEXT)
  else
    ifeq ($(DEVICETYPE), GP)
        COMP_LIST_COMMON += spi board sbl_lib_cust
    endif
    ifeq ($(DEVICETYPE), HS)
        COMP_LIST_COMMON += spi board sbl_lib_cust_hs
    endif

    EXT_LIB_LIST_COMMON += $(spi_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(spi_LIBNAME).$(LIBEXT)
  endif

    ifeq ($(DEVICETYPE), GP)
        EXT_LIB_LIST_COMMON += $(sbl_lib_cust_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(sbl_lib_cust_LIBNAME).$(LIBEXT)
    endif
    ifeq ($(DEVICETYPE), HS)
        EXT_LIB_LIST_COMMON += $(sbl_lib_cust_hs_LIBPATH)/$(BOARD)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(sbl_lib_cust_hs_LIBNAME).$(LIBEXT)
    endif
endif

SRCS_COMMON = main_rtos.c boot_core_defs.c

ifeq ($(BUILD_OS_TYPE), freertos)
  INCLUDE_EXTERNAL_INTERFACES += freertos
  SRCS_COMMON += overrides/$(SOC)/$(CORE)/r5_mpu_freertos.c
  EXT_LIB_LIST_COMMON += $(osal_freertos_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(osal_freertos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(freertos_LIBPATH)/$(SOC)/$(CORE)/$(BUILD_PROFILE_$(CORE))/$(freertos_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(csl_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_LIBNAME).$(LIBEXT)
  EXT_LIB_LIST_COMMON += $(csl_init_LIBPATH)/$(SOC)/$(ISA_EXT)/$(BUILD_PROFILE_$(CORE))/$(csl_init_LIBNAME).$(LIBEXT)
  EXTERNAL_LNKCMD_FILE_LOCAL = $(MCUSW_INSTALL_PATH)/mcuss_demos/boot_app_mcu_rtos/overrides/$(SOC)/$(CORE)/linker_r5_freertos.lds
endif

SRCS_COMMON += mcu_timer_multicore.c

# Enable copy of vectors
ifeq ($(ISA),$(filter $(ISA), r5f))
  SRCS_ASM_COMMON += utilsCopyVecs2ATcm.asm
endif

ifeq ($(CANFUNC), can_profiling)
    SRCS_COMMON += can_profile.c can_utils.c
else ifeq ($(CANFUNC), can_fast_response)
    SRCS_COMMON += can_resp.c can_utils.c
endif
ifeq ($(BOOTFUNC), enabled)
    SRCS_COMMON += boot.c
endif
ifeq ($(MCUONLYFUNC), enabled)
    SRCS_COMMON += mcu_only_app.c
endif
ifeq ($(SOC),$(filter $(SOC), j721e))
ifeq ($(BISTFUNC), enabled)
    SRCS_COMMON += bist.c lbist_utils.c pbist_utils.c power_seq.c
    # SOC-specific files
    SRCS_COMMON += lbist_defs.c pbist_defs.c bist_core_defs.c
ifeq ($(SOC),$(filter $(SOC), j721e j7200))
    SRCS_COMMON += armv8_power_utils.c
endif
endif
endif

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON += Can_Cfg.c Can_PBcfg.c Dio_Lcfg.c

# App utils file includes memory sections defined in this module
# which is used to check memory section corruption
SRCS_COMMON += app_utils_can.c

PACKAGE_SRCS_COMMON = .

CFLAGS_LOCAL_COMMON = $(MCUSW_CFLAGS)
LNKFLAGS_LOCAL_COMMON = $(MCUSW_LNKFLAGS)

CFLAGS_LOCAL_COMMON += -DSBL_LOG_LEVEL=2
CFLAGS_LOCAL_COMMON += -DMULTICORE_PROFILE_TIMER_ENABLED
ifeq ($(BOOTMODE), mmcsd)
    CFLAGS_LOCAL_COMMON += -DBOOT_MMCSD
endif
ifeq ($(BOOTMODE), ospi)
    CFLAGS_LOCAL_COMMON += -DBOOT_OSPI
  ifeq ($(ENABLE_OSPI_DMA), yes)
    CFLAGS_LOCAL_COMMON += -DENABLE_DMA
  endif
endif
ifeq ($(MCUSW_UART_ENABLE),TRUE)
    CFLAGS_LOCAL_COMMON += -DUART_ENABLED
  ifeq ($(BOOTMODE), mmcsd)
    # TODO: Currently, debug prints are enabled for all core images to load
    # properly in MMCSD boot mode.  Can disable this once there is a fix.
    CFLAGS_LOCAL_COMMON += -DUART_PRINT_DEBUG
  else
    #CFLAGS_LOCAL_COMMON += -DUART_PRINT_DEBUG
  endif
endif
ifeq ($(CANFUNC), can_profiling)
    CFLAGS_LOCAL_COMMON += -DCAN_PROFILE_TASK_ENABLED
else ifeq ($(CANFUNC), can_fast_response)
    CFLAGS_LOCAL_COMMON += -DCAN_RESP_TASK_ENABLED
endif
ifeq ($(BOOTFUNC), enabled)
    CFLAGS_LOCAL_COMMON += -DBOOT_TASK_ENABLED
endif
ifeq ($(MCUONLYFUNC), enabled)
    CFLAGS_LOCAL_COMMON += -DMCU_ONLY_TASK_ENABLED -DPMIC_OPS_ENABLED -DVTM_OPS_ENABLED
endif
ifeq ($(SOC),$(filter $(SOC), j721e))
ifeq ($(BISTFUNC), enabled)
    SRCS_COMMON += sdl_pbist.c sdl_osal.c sdl_ip_pbist.c sdl_soc_pbist.c osal_interface.c sdl_lbist.c sdl_ip_lbist.c sdl_soc_lbist.c
    CFLAGS_LOCAL_COMMON += -DBIST_TASK_ENABLED
endif
endif
ifeq ($(HLOSBOOT), linux)
    CFLAGS_LOCAL_COMMON += -DMPU1_HLOS_BOOT_ENABLED -DHLOS_BOOT_LINUX_OS
else ifeq ($(HLOSBOOT), qnx)
    CFLAGS_LOCAL_COMMON += -DMPU1_HLOS_BOOT_ENABLED -DHLOS_BOOT_QNX_OS
endif
# Core/SoC/platform specific source files and CFLAGS
# Example:
# SRCS_<core/SoC/platform-name> =
# CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(PDK_INSTALL_PATH)/ti/build/makerules
  export MAKERULEDIR
endif

include $(MAKERULEDIR)/common.mk
include $(MAKERULEDIR)/build_config.mk
