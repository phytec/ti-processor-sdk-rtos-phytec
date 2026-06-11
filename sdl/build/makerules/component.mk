#*******************************************************************************
#                                                                              *
# Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com/       *
#                        ALL RIGHTS RESERVED                                   *
#                                                                              *
#*******************************************************************************

# File: component.mk
#       This file is component include make file of SDL.
# List of variables set in this file and their purpose:
# <mod>_RELPATH        - This is the relative path of the module, typically from
#                        top-level directory of the package
# <mod>_PATH           - This is the absolute path of the module. It derives from
#                        absolute path of the top-level directory (set in env.mk)
#                        and relative path set above
# <mod>_INCLUDE        - This is the path that has interface header files of the
#                        module. This can be multiple directories (space separated)
# <mod>_PKG_LIST       - Names of the modules (and sub-modules) that are a part
#                        part of this module, including itself.
# <mod>_PLATFORM_DEPENDENCY - "yes": means the code for this module depends on
#                             platform and the compiled obj/lib has to be kept
#                             under <platform> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no platform dependent code and hence
#                             the obj/libs are not kept under <platform> dir.
# <mod>_CORE_DEPENDENCY     - "yes": means the code for this module depends on
#                             core and the compiled obj/lib has to be kept
#                             under <core> directory
#                             "no" or "" or if this variable is not defined: means
#                             this module has no core dependent code and hence
#                             the obj/libs are not kept under <core> dir.
# <mod>_APP_STAGE_FILES     - List of source files that belongs to the module
#                             <mod>, but that needs to be compiled at application
#                             build stage (in the context of the app). This is
#                             primarily for link time configurations or if the
#                             source file is dependent on options/defines that are
#                             application dependent. This can be left blank or
#                             not defined at all, in which case, it means there
#                             no source files in the module <mod> that are required
#                             to be compiled in the application build stage.
#
ifeq ($(component_make_include), )

ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
DEFAULT_SOCLIST = j721e j7200 j721s2 j784s4
endif

DEFAULT_OSLIST = baremetal

############################
# Device SDL package
# List of components included under SDL lib
# The components included here are built and will be part of SDL lib
# SDL HW files will be included in this order :
# 1. SOC specific HW file - $(sdl_PATH)/src/ip/$(SOC)/
# 2. Common HW file - $(sdl_PATH)/src/ip
############################

sdl_LIB_LIST = r5f_core sdl_ip sdl_api sdl_osal


############################
# sdl r5f core lib
############################
r5f_core_COMP_LIST = r5f_core
r5f_core_RELPATH = src/ip/r5
r5f_core_PATH = $(sdl_PATH)/$(r5f_core_RELPATH)
r5f_core_PLATFORM_DEPENDENCY = yes
export r5f_core_COMP_LIST
export r5f_core_PLATFORM_DEPENDENCY
r5f_core_PKG_LIST = r5f_core
r5f_core_INCLUDE = $(r5f_core_PATH)
r5f_core_SOCLIST = $(DEFAULT_SOCLIST)
export r5f_core_SOCLIST

############################
# sdl ip support lib
############################
sdl_ip_COMP_LIST = sdl_ip
sdl_ip_RELPATH = src/ip
sdl_ip_PATH = $(sdl_PATH)/$(sdl_ip_RELPATH)
sdl_ip_PLATFORM_DEPENDENCY = yes
export sdl_ip_COMP_LIST
export sdl_ip_PLATFORM_DEPENDENCY
sdl_ip_PKG_LIST = sdl_ip
sdl_ip_INCLUDE = $(sdl_ip_PATH)
sdl_ip_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_ip_SOCLIST

############################
# sdl lib
############################
sdl_api_COMP_LIST = sdl_api
sdl_api_RELPATH = src/sdl
sdl_api_PATH = $(sdl_PATH)/$(sdl_api_RELPATH)
sdl_api_PLATFORM_DEPENDENCY = yes
export sdl_api_COMP_LIST
export sdl_api_PLATFORM_DEPENDENCY
sdl_api_PKG_LIST = sdl_api
sdl_api_INCLUDE = $(sdl_api_PATH)
sdl_api_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_api_SOCLIST

############################
# sdl osal
############################
sdl_osal_COMP_LIST = sdl_osal
sdl_osal_RELPATH = osal
sdl_osal_PATH = $(sdl_PATH)/$(sdl_osal_RELPATH)
sdl_osal_PLATFORM_DEPENDENCY = yes
export sdl_osal_COMP_LIST
export sdl_osal_PLATFORM_DEPENDENCY
sdl_osal_PKG_LIST = sdl_osal
sdl_osal_INCLUDE = $(sdl_osal_PATH)
sdl_osal_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_osal_OSLIST = $(DEFAULT_OSLIST)
export sdl_osal_SOCLIST

############################
#device sdl test
# List of tests under SDL
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################

ifeq ($(SOC),$(filter $(SOC),  j784s4))

sdl_TEST_LIST += esm_test_app esm_func_test_app rti_test_app rti_func_test_app rat_test_app rat_func_test_app sdl_mcrc_autoModeTest_app mcrc_test_app sdl_mcrc_test_app dcc_test_app dcc_func_test_app mpu_test_app mpu_func_test_app vtm_test_app sdl_vtm_test_app pmu_test_app mtog_test_app pbist_test_app sdl_pbist_test_app ecc_func_app ecc_test_app r5_ecc_test_app lbist_test_app sdl_lbist_test_app msmc_test_app sdl_ccm_test_app ccm_test_app sdl_pok_test_app pok_test_app tog_test_app sdl_j7_clock_test_app exception_test_app vim_test_app sdl_ecc_test_app sdl_rat_test_app rom_checksum_test_app rom_checksum_func_test_app
endif

ifeq ($(SOC),$(filter $(SOC),  j721s2))
sdl_TEST_LIST += esm_test_app esm_func_test_app rti_test_app rti_func_test_app vtm_test_app sdl_vtm_test_app mpu_test_app mpu_func_test_app pmu_test_app mtog_test_app ccm_test_app sdl_ccm_test_app vim_test_app mcrc_test_app sdl_mcrc_test_app ecc_test_app ecc_func_app pbist_test_app sdl_pbist_test_app sdl_ecc_test_app rom_checksum_test_app rom_checksum_func_test_app
sdl_TEST_LIST += sdl_mcrc_autoModeTest_app dcc_test_app dcc_func_test_app sdl_j7_clock_test_app rat_test_app rat_func_test_app sdl_rat_test_app lbist_test_app sdl_lbist_test_app tog_test_app msmc_test_app exception_test_app pok_test_app r5_ecc_test_app sdl_pok_test_app
endif

ifeq ($(SOC),$(filter $(SOC),  j721e j7200))
sdl_TEST_LIST += r5f_startup pbist_test_app sdl_pbist_test_app lbist_test_app sdl_lbist_test_app tog_test_app pok_test_app ecc_test_app sdl_ecc_test_app rti_test_app rti_func_test_app esm_func_test_app esm_test_app vtm_test_app sdl_pok_test_app sdl_vtm_test_app dcc_test_app dcc_func_test_app ecc_func_app mcrc_test_app sdl_mcrc_test_app rom_checksum_test_app rom_checksum_func_test_app
sdl_TEST_LIST += exception_test_app ccm_test_app sdl_ccm_test_app rat_test_app pmu_test_app sdl_j7_clock_test_app vim_test_app msmc_test_app sdl_rat_test_app rat_func_test_app sdl_mcrc_autoModeTest_app sdl_utils_test_app mpu_test_app mpu_func_test_app r5_ecc_test_app

ifeq ($(SOC),$(filter $(SOC),  j7200))
sdl_TEST_LIST += mtog_test_app
endif
endif

############################
# sdl exception test
############################
exception_test_app_COMP_LIST = exception_test_app
exception_test_app_RELPATH = test/misc
exception_test_app_PATH = $(sdl_PATH)/$(exception_test_app_RELPATH)
exception_test_app_BOARD_DEPENDENCY = yes
exception_test_app_CORE_DEPENDENCY = yes
export exception_test_app_COMP_LIST
export exception_test_app_PLATFORM_DEPENDENCY
export exception_test_app_CORE_DEPENDENCY
exception_test_app_PKG_LIST = exception_test_app
exception_test_app_INCLUDE = $(exception_test_app_PATH)
exception_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export exception_test_app_OSLIST = $(DEFAULT_OSLIST)
export exception_test_app_SBL_APPIMAGEGEN = yes
export exception_test_app_SOCLIST

############################
# sdl rat test
############################
sdl_rat_test_app_COMP_LIST = sdl_rat_test_app
sdl_rat_test_app_RELPATH = test/rat
sdl_rat_test_app_PATH = $(sdl_PATH)/$(sdl_rat_test_app_RELPATH)
sdl_rat_test_app_BOARD_DEPENDENCY = yes
sdl_rat_test_app_CORE_DEPENDENCY = yes
sdl_rat_test_app_MAKEFILE = -f makefile_ratIntTest
export sdl_rat_test_app_MAKEFILE
export sdl_rat_test_app_COMP_LIST
export sdl_rat_test_app_PLATFORM_DEPENDENCY
export sdl_rat_test_app_CORE_DEPENDENCY
sdl_rat_test_app_PKG_LIST = sdl_rat_test_app
sdl_rat_test_app_INCLUDE = $(sdl_rat_test_app_PATH)
sdl_rat_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_rat_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_rat_test_app_SBL_APPIMAGEGEN = yes
export sdl_rat_test_app_SOCLIST

############################
# rom checksum test 
############################
rom_checksum_test_app_COMP_LIST = rom_checksum_test_app
rom_checksum_test_app_RELPATH = test/rom_checksum/unit_test
rom_checksum_test_app_PATH = $(sdl_PATH)/$(rom_checksum_test_app_RELPATH)
rom_checksum_test_app_BOARD_DEPENDENCY = yes
rom_checksum_test_app_CORE_DEPENDENCY = yes
export rom_checksum_test_app_COMP_LIST
export rom_checksum_test_app_PLATFORM_DEPENDENCY
export rom_checksum_test_app_CORE_DEPENDENCY
rom_checksum_test_app_PKG_LIST = rom_checksum_test_app
rom_checksum_test_app_INCLUDE = $(rom_checksum_test_app_PATH)
rom_checksum_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export rom_checksum_test_app_OSLIST = $(DEFAULT_OSLIST)
export rom_checksum_test_app_SBL_APPIMAGEGEN = yes
export rom_checksum_test_app_SOCLIST

############################
# sdl func rat test
############################
rat_func_test_app_COMP_LIST = rat_func_test_app
rat_func_test_app_RELPATH = test/rat
rat_func_test_app_PATH = $(sdl_PATH)/$(rat_func_test_app_RELPATH)
rat_func_test_app_BOARD_DEPENDENCY = yes
rat_func_test_app_CORE_DEPENDENCY = yes
export rat_func_test_app_COMP_LIST
export rat_func_test_app_PLATFORM_DEPENDENCY
export rat_func_test_app_CORE_DEPENDENCY
rat_func_test_app_PKG_LIST = rat_func_test_app
rat_func_test_app_INCLUDE = $(rat_func_test_app_PATH)
rat_func_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export rat_func_test_app_OSLIST = $(DEFAULT_OSLIST)
export rat_func_test_app_SBL_APPIMAGEGEN = yes
export rat_func_test_app_SOCLIST

############################
# sdl r5f startup lib
############################
r5f_startup_COMP_LIST = r5f_startup
r5f_startup_RELPATH = test/r5f_startup
r5f_startup_PATH = $(sdl_PATH)/$(r5f_startup_RELPATH)
r5f_startup_PLATFORM_DEPENDENCY = yes
export r5f_startup_COMP_LIST
export r5f_startup_PLATFORM_DEPENDENCY
r5f_startup_PKG_LIST = r5f_startup
r5f_startup_INCLUDE = $(r5f_startup_PATH)
r5f_startup_SOCLIST = $(DEFAULT_SOCLIST)
export r5f_startup_SOCLIST

############################
# sdl vtm test
############################
vtm_test_app_COMP_LIST = vtm_test_app
vtm_test_app_RELPATH = test/vtm
vtm_test_app_PATH = $(sdl_PATH)/$(vtm_test_app_RELPATH)
vtm_test_app_BOARD_DEPENDENCY = yes
vtm_test_app_CORE_DEPENDENCY = yes
export vtm_test_app_COMP_LIST
export vtm_test_app_PLATFORM_DEPENDENCY
export vtm_test_app_CORE_DEPENDENCY
vtm_test_app_PKG_LIST = vtm_test_app
vtm_test_app_INCLUDE = $(vtm_test_app_PATH)
vtm_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export vtm_test_app_OSLIST = $(DEFAULT_OSLIST)
export vtm_test_app_SBL_APPIMAGEGEN = yes
export vtm_test_app_SOCLIST

############################
# sdl pbist test
############################
pbist_test_app_COMP_LIST = pbist_test_app
pbist_test_app_RELPATH = test/pbist
pbist_test_app_PATH = $(sdl_PATH)/$(pbist_test_app_RELPATH)
pbist_test_app_BOARD_DEPENDENCY = yes
pbist_test_app_CORE_DEPENDENCY = yes
export pbist_test_app_COMP_LIST
export pbist_test_app_PLATFORM_DEPENDENCY
export pbist_test_app_CORE_DEPENDENCY
pbist_test_app_PKG_LIST = pbist_test_app
pbist_test_app_INCLUDE = $(pbist_test_app_PATH)
pbist_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export pbist_test_app_OSLIST = $(DEFAULT_OSLIST)
export pbist_test_app_SBL_APPIMAGEGEN = yes
export pbist_test_app_SOCLIST

############################
# sdl pbist test
############################
sdl_pbist_test_app_COMP_LIST = sdl_pbist_test_app
sdl_pbist_test_app_RELPATH = test/pbist
sdl_pbist_test_app_PATH = $(sdl_PATH)/$(sdl_pbist_test_app_RELPATH)
sdl_pbist_test_app_BOARD_DEPENDENCY = yes
sdl_pbist_test_app_CORE_DEPENDENCY = yes
sdl_pbist_test_app_MAKEFILE = -f makefile_sdl_api
export sdl_pbist_test_app_MAKEFILE
export sdl_pbist_test_app_COMP_LIST
export sdl_pbist_test_app_PLATFORM_DEPENDENCY
export sdl_pbist_test_app_CORE_DEPENDENCY
sdl_pbist_test_app_PKG_LIST = sdl_pbist_test_app
sdl_pbist_test_app_INCLUDE = $(sdl_pbist_test_app_PATH)
sdl_pbist_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_pbist_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_pbist_test_app_SBL_APPIMAGEGEN = yes
export sdl_pbist_test_app_SOCLIST

############################
# sdl ccm test
############################
ccm_test_app_COMP_LIST = ccm_test_app
ccm_test_app_RELPATH = test/ccm
ccm_test_app_PATH = $(sdl_PATH)/$(ccm_test_app_RELPATH)
ccm_test_app_BOARD_DEPENDENCY = yes
ccm_test_app_CORE_DEPENDENCY = yes
export ccm_test_app_COMP_LIST
export ccm_test_app_PLATFORM_DEPENDENCY
export ccm_test_app_CORE_DEPENDENCY
ccm_test_app_PKG_LIST = ccm_test_app
ccm_test_app_INCLUDE = $(ccm_test_app_PATH)
ccm_test_app_SOCLIST = $(DEFAULT_SOCLIST)
ccm_test_app_APP_SMP_CONFIG = yes
export ccm_test_app_OSLIST = $(DEFAULT_OSLIST)
export ccm_test_app_APP_SMP_CONFIG
export ccm_test_app_SBL_APPIMAGEGEN = yes
export ccm_test_app_SOCLIST

############################
# sdl ccm test
############################
sdl_ccm_test_app_COMP_LIST = sdl_ccm_test_app
sdl_ccm_test_app_RELPATH = test/ccm
sdl_ccm_test_app_PATH = $(sdl_PATH)/$(sdl_ccm_test_app_RELPATH)
sdl_ccm_test_app_BOARD_DEPENDENCY = yes
sdl_ccm_test_app_CORE_DEPENDENCY = yes
sdl_ccm_test_app_MAKEFILE = -f makefile_sdl_api
export sdl_ccm_test_app_MAKEFILE
export sdl_ccm_test_app_COMP_LIST
export sdl_ccm_test_app_PLATFORM_DEPENDENCY
export sdl_ccm_test_app_CORE_DEPENDENCY
sdl_ccm_test_app_PKG_LIST = sdl_ccm_test_app
sdl_ccm_test_app_INCLUDE = $(sdl_ccm_test_app_PATH)
sdl_ccm_test_app_SOCLIST = $(DEFAULT_SOCLIST)
sdl_ccm_test_app_APP_SMP_CONFIG = yes
export sdl_ccm_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_ccm_test_app_APP_SMP_CONFIG
export sdl_ccm_test_app_SBL_APPIMAGEGEN = yes
export sdl_ccm_test_app_SOCLIST

############################
# sdl rat test
############################
rat_test_app_COMP_LIST = rat_test_app
rat_test_app_RELPATH = test/rat/unit
rat_test_app_PATH = $(sdl_PATH)/$(rat_test_app_RELPATH)
rat_test_app_BOARD_DEPENDENCY = yes
rat_test_app_CORE_DEPENDENCY = yes
export rat_test_app_COMP_LIST
export rat_test_app_PLATFORM_DEPENDENCY
export rat_test_app_CORE_DEPENDENCY
rat_test_app_PKG_LIST = rat_test_app
rat_test_app_INCLUDE = $(rat_test_app_PATH)
rat_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export rat_test_app_OSLIST = $(DEFAULT_OSLIST)
export rat_test_app_SBL_APPIMAGEGEN = yes
export rat_test_app_SOCLIST

############################
# sdl ESM clock loss test app
############################
sdl_j7_clock_test_app_COMP_LIST = sdl_j7_clock_test_app
sdl_j7_clock_test_app_RELPATH = test/j7_clock
sdl_j7_clock_test_app_PATH = $(sdl_PATH)/$(sdl_j7_clock_test_app_RELPATH)
sdl_j7_clock_test_app_BOARD_DEPENDENCY = yes
sdl_j7_clock_test_app_CORE_DEPENDENCY = yes
export sdl_j7_clock_test_app_COMP_LIST
export sdl_j7_clock_test_app_PLATFORM_DEPENDENCY
export sdl_j7_clock_test_app_CORE_DEPENDENCY
sdl_j7_clock_test_app_PKG_LIST = sdl_j7_clock_test_app
sdl_j7_clock_test_app_INCLUDE = $(sdl_j7_clock_test_app_PATH)
sdl_j7_clock_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_j7_clock_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_j7_clock_test_app_SBL_APPIMAGEGEN = yes
export sdl_j7_clock_test_app_SOCLIST

############################
# sdl mpu test
############################
mpu_test_app_COMP_LIST = mpu_test_app
mpu_test_app_RELPATH = test/mpu/unit_test
mpu_test_app_PATH = $(sdl_PATH)/$(mpu_test_app_RELPATH)
mpu_test_app_BOARD_DEPENDENCY = yes
mpu_test_app_CORE_DEPENDENCY = yes
export mpu_test_app_MAKEFILE
export mpu_test_app_COMP_LIST
export mpu_test_app_PLATFORM_DEPENDENCY
export mpu_test_app_CORE_DEPENDENCY
mpu_test_app_PKG_LIST = mpu_test_app
mpu_test_app_INCLUDE = $(mpu_test_app_PATH)
mpu_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export mpu_test_app_OSLIST = $(DEFAULT_OSLIST)
export mpu_test_app_SBL_APPIMAGEGEN = yes
export mpu_test_app_SOCLIST

############################
# sdl mpu functional test
############################
mpu_func_test_app_COMP_LIST = mpu_func_test_app
mpu_func_test_app_RELPATH = test/mpu/func_test
mpu_func_test_app_PATH = $(sdl_PATH)/$(mpu_func_test_app_RELPATH)
mpu_func_test_app_BOARD_DEPENDENCY = yes
mpu_func_test_app_CORE_DEPENDENCY = yes
export mpu_func_test_app_MAKEFILE
export mpu_func_test_app_COMP_LIST
export mpu_func_test_app_PLATFORM_DEPENDENCY
export mpu_func_test_app_CORE_DEPENDENCY
mpu_func_test_app_PKG_LIST = mpu_func_test_app
mpu_func_test_app_INCLUDE = $(mpu_func_test_app_PATH)
mpu_func_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export mpu_func_test_app_OSLIST = $(DEFAULT_OSLIST)
export mpu_func_test_app_SBL_APPIMAGEGEN = yes
export mpu_func_test_app_SOCLIST

############################
# sdl lbist test
############################
lbist_test_app_COMP_LIST = lbist_test_app
lbist_test_app_RELPATH = test/lbist
lbist_test_app_PATH = $(sdl_PATH)/$(lbist_test_app_RELPATH)
lbist_test_app_BOARD_DEPENDENCY = yes
lbist_test_app_CORE_DEPENDENCY = yes
export lbist_test_app_COMP_LIST
export lbist_test_app_PLATFORM_DEPENDENCY
export lbist_test_app_CORE_DEPENDENCY
lbist_test_app_PKG_LIST = lbist_test_app
lbist_test_app_INCLUDE = $(lbist_test_app_PATH)
lbist_test_app_SOCLIST = $(DEFAULT_SOCLIST)
lbist_test_app_OSLIST = baremetal freertos
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
lbist_test_app_OSLIST += safertos
endif
export lbist_test_app_OSLIST
export lbist_test_app_SBL_APPIMAGEGEN = yes
export lbist_test_app_SOCLIST

############################
# sdl lbist test
############################
sdl_lbist_test_app_COMP_LIST = sdl_lbist_test_app
sdl_lbist_test_app_RELPATH = test/lbist
sdl_lbist_test_app_PATH = $(sdl_PATH)/$(sdl_lbist_test_app_RELPATH)
sdl_lbist_test_app_CORE_DEPENDENCY = yes
sdl_lbist_test_app_MAKEFILE = -f makefile_sdl_api
export sdl_lbist_test_app_MAKEFILE
export sdl_lbist_test_app_COMP_LIST
export sdl_lbist_test_app_PLATFORM_DEPENDENCY
export sdl_lbist_test_app_CORE_DEPENDENCY
sdl_lbist_test_app_PKG_LIST = sdl_lbist_test_app
sdl_lbist_test_app_INCLUDE = $(sdl_lbist_test_app_PATH)
sdl_lbist_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_lbist_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_lbist_test_app_SBL_APPIMAGEGEN = yes
export sdl_lbist_test_app_SOCLIST

############################
# sdl tog test
############################
tog_test_app_COMP_LIST = tog_test_app
tog_test_app_RELPATH = test/tog
tog_test_app_PATH = $(sdl_PATH)/$(tog_test_app_RELPATH)
tog_test_app_BOARD_DEPENDENCY = yes
tog_test_app_CORE_DEPENDENCY = yes
export tog_test_app_COMP_LIST
export tog_test_app_PLATFORM_DEPENDENCY
export tog_test_app_CORE_DEPENDENCY
tog_test_app_PKG_LIST = tog_test_app
tog_test_app_INCLUDE = $(tog_test_app_PATH)
tog_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export tog_test_app_OSLIST = $(DEFAULT_OSLIST)
export tog_test_app_SBL_APPIMAGEGEN = yes
export tog_test_app_SOCLIST

############################
# sdl mtog test
############################
mtog_test_app_COMP_LIST = mtog_test_app
mtog_test_app_RELPATH = test/mtog
mtog_test_app_PATH = $(sdl_PATH)/$(mtog_test_app_RELPATH)
mtog_test_app_BOARD_DEPENDENCY = yes
mtog_test_app_CORE_DEPENDENCY = yes
export mtog_test_app_COMP_LIST
export mtog_test_app_PLATFORM_DEPENDENCY
export mtog_test_app_CORE_DEPENDENCY
mtog_test_app_PKG_LIST = mtog_test_app
mtog_test_app_INCLUDE = $(mtog_test_app_PATH)
mtog_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export mtog_test_app_OSLIST = $(DEFAULT_OSLIST)
export mtog_test_app_SBL_APPIMAGEGEN = yes
export mtog_test_app_SOCLIST

############################
# sdl pok test
############################
pok_test_app_COMP_LIST = pok_test_app
pok_test_app_RELPATH = test/pok
pok_test_app_PATH = $(sdl_PATH)/$(pok_test_app_RELPATH)
pok_test_app_BOARD_DEPENDENCY = yes
pok_test_app_CORE_DEPENDENCY = yes
export pok_test_app_COMP_LIST
export pok_test_app_PLATFORM_DEPENDENCY
export pok_test_app_CORE_DEPENDENCY
pok_test_app_PKG_LIST = pok_test_app
pok_test_app_INCLUDE = $(pok_test_app_PATH)
pok_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export pok_test_app_OSLIST = $(DEFAULT_OSLIST)
export pok_test_app_SBL_APPIMAGEGEN = yes
export pok_test_app_SOCLIST

############################
# sdl pok test
############################
sdl_pok_test_app_COMP_LIST = sdl_pok_test_app
sdl_pok_test_app_RELPATH = test/pok
sdl_pok_test_app_PATH = $(sdl_PATH)/$(sdl_pok_test_app_RELPATH)
sdl_pok_test_app_BOARD_DEPENDENCY = yes
sdl_pok_test_app_CORE_DEPENDENCY = yes
sdl_pok_test_app_MAKEFILE = -f makefile_pokFuncTest
export sdl_pok_test_app_MAKEFILE
export sdl_pok_test_app_COMP_LIST
export sdl_pok_test_app_PLATFORM_DEPENDENCY
export sdl_pok_test_app_CORE_DEPENDENCY
sdl_pok_test_app_PKG_LIST = sdl_pok_test_app
sdl_pok_test_app_INCLUDE = $(sdl_pok_test_app_PATH)
sdl_pok_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_pok_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_pok_test_app_SBL_APPIMAGEGEN = yes
export sdl_pok_test_app_SOCLIST

############################
# sdl ecc test
############################
ecc_test_app_COMP_LIST = ecc_test_app
ecc_test_app_RELPATH = test/ecc
ecc_test_app_PATH = $(sdl_PATH)/$(ecc_test_app_RELPATH)
ecc_test_app_BOARD_DEPENDENCY = yes
ecc_test_app_CORE_DEPENDENCY = yes
export ecc_test_app_COMP_LIST
export ecc_test_app_PLATFORM_DEPENDENCY
export ecc_test_app_CORE_DEPENDENCY
ecc_test_app_PKG_LIST = ecc_test_app
ecc_test_app_INCLUDE = $(ecc_test_app_PATH)
ecc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export ecc_test_app_OSLIST = $(DEFAULT_OSLIST)
export ecc_test_app_SBL_APPIMAGEGEN = yes
export ecc_test_app_SOCLIST

############################
# sdl ecc test
############################
sdl_ecc_test_app_COMP_LIST = sdl_ecc_test_app
sdl_ecc_test_app_RELPATH = test/ecc
sdl_ecc_test_app_PATH = $(sdl_PATH)/$(sdl_ecc_test_app_RELPATH)
sdl_ecc_test_app_BOARD_DEPENDENCY = yes
sdl_ecc_test_app_CORE_DEPENDENCY = yes
sdl_ecc_test_app_MAKEFILE = -f makefile_sdl_api
export sdl_ecc_test_app_MAKEFILE
export sdl_ecc_test_app_COMP_LIST
export sdl_ecc_test_app_PLATFORM_DEPENDENCY
export sdl_ecc_test_app_CORE_DEPENDENCY
sdl_ecc_test_app_PKG_LIST = sdl_ecc_test_app
sdl_ecc_test_app_INCLUDE = $(sdl_ecc_test_app_PATH)
sdl_ecc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_ecc_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_ecc_test_app_SBL_APPIMAGEGEN = yes
export sdl_ecc_test_app_SOCLIST

############################
# sdl r5 ecc test
############################
r5_ecc_test_app_COMP_LIST = r5_ecc_test_app
r5_ecc_test_app_RELPATH = test/ecc
r5_ecc_test_app_PATH = $(sdl_PATH)/$(r5_ecc_test_app_RELPATH)
r5_ecc_test_app_BOARD_DEPENDENCY = yes
r5_ecc_test_app_CORE_DEPENDENCY = yes
r5_ecc_test_app_MAKEFILE = -f makefile_r5
export r5_ecc_test_app_MAKEFILE
export r5_ecc_test_app_COMP_LIST
export r5_ecc_test_app_PLATFORM_DEPENDENCY
export r5_ecc_test_app_CORE_DEPENDENCY
r5_ecc_test_app_PKG_LIST = r5_ecc_test_app
r5_ecc_test_app_INCLUDE = $(r5_ecc_test_app_PATH)
r5_ecc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export r5_ecc_test_app_OSLIST = $(DEFAULT_OSLIST)
export r5_ecc_test_app_SBL_APPIMAGEGEN = yes
export r5_ecc_test_app_SOCLIST

############################
# sdl rti test
############################
rti_test_app_COMP_LIST = rti_test_app
rti_test_app_RELPATH = test/rti/unit
rti_test_app_PATH = $(sdl_PATH)/$(rti_test_app_RELPATH)
rti_test_app_BOARD_DEPENDENCY = yes
rti_test_app_CORE_DEPENDENCY = yes
export rti_test_app_COMP_LIST
export rti_test_app_PLATFORM_DEPENDENCY
export rti_test_app_CORE_DEPENDENCY
rti_test_app_PKG_LIST = rti_test_app
rti_test_app_INCLUDE = $(rti_test_app_PATH)
rti_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export rti_test_app_OSLIST = $(DEFAULT_OSLIST)
export rti_test_app_SBL_APPIMAGEGEN = yes
export rti_test_app_SOCLIST

############################
# sdl ecc func test
############################
sdl_ecc_test_app_COMP_LIST = ecc_func_app
ecc_func_app_RELPATH = test/ecc
ecc_func_app_PATH = $(sdl_PATH)/$(ecc_func_app_RELPATH)
ecc_func_app_BOARD_DEPENDENCY = yes
ecc_func_app_CORE_DEPENDENCY = yes
ecc_func_app_MAKEFILE = -f makefile_funcTest
export ecc_func_app_MAKEFILE
export ecc_func_app_COMP_LIST
export ecc_func_app_PLATFORM_DEPENDENCY
export ecc_func_app_CORE_DEPENDENCY
ecc_func_app_PKG_LIST = ecc_func_app
ecc_func_app_INCLUDE = $(ecc_func_app_PATH)
ecc_func_app_SOCLIST = $(DEFAULT_SOCLIST)
export ecc_func_app_OSLIST = $(DEFAULT_OSLIST)
export ecc_func_app_SBL_APPIMAGEGEN = yes
export ecc_func_app_SOCLIST

############################
# sdl rti Function test
############################
rti_func_test_app_COMP_LIST = rti_func_test_app
rti_func_test_app_RELPATH = test/rti/func_test
rti_func_test_app_PATH = $(sdl_PATH)/$(rti_func_test_app_RELPATH)
rti_func_test_app_BOARD_DEPENDENCY = yes
rti_func_test_app_CORE_DEPENDENCY = yes
export rti_func_test_app_COMP_LIST
export rti_func_test_app_PLATFORM_DEPENDENCY
export rti_func_test_app_CORE_DEPENDENCY
rti_func_test_app_PKG_LIST = rti_func_test_app
rti_func_test_app_INCLUDE = $(rti_func_test_app_PATH)
rti_func_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export rti_func_test_app_OSLIST = $(DEFAULT_OSLIST)
export rti_func_test_app_SBL_APPIMAGEGEN = yes
export rti_func_test_app_SOCLIST

############################
# esm test
############################
esm_func_test_app_COMP_LIST = esm_func_test_app
esm_func_test_app_RELPATH = test/esm
esm_func_test_app_PATH = $(sdl_PATH)/$(esm_func_test_app_RELPATH)
esm_func_test_app_BOARD_DEPENDENCY = yes
esm_func_test_app_CORE_DEPENDENCY = yes
esm_func_test_app_MAKEFILE = -f makefile_priv
export esm_func_test_app_MAKEFILE
export esm_func_test_app_COMP_LIST
export esm_func_test_app_PLATFORM_DEPENDENCY
export esm_func_test_app_CORE_DEPENDENCY
esm_func_test_app_PKG_LIST = esm_func_test_app
esm_func_test_app_INCLUDE = $(esm_func_test_app_PATH)
esm_func_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export esm_func_test_app_OSLIST = $(DEFAULT_OSLIST)
export esm_func_test_app_SBL_APPIMAGEGEN = yes
export esm_func_test_app_SOCLIST

############################
# esm test
############################
esm_test_app_COMP_LIST = esm_test_app
esm_test_app_RELPATH = test/esm
esm_test_app_PATH = $(sdl_PATH)/$(esm_test_app_RELPATH)
esm_test_app_BOARD_DEPENDENCY = yes
esm_test_app_CORE_DEPENDENCY = yes
esm_test_app_MAKEFILE = -f makefile
export esm_test_app_MAKEFILE
export esm_test_app_COMP_LIST
export esm_test_app_PLATFORM_DEPENDENCY
export esm_test_app_CORE_DEPENDENCY
esm_test_app_PKG_LIST = esm_test_app
esm_test_app_INCLUDE = $(esm_test_app_PATH)
esm_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export esm_test_app_OSLIST = $(DEFAULT_OSLIST)
export esm_test_app_SBL_APPIMAGEGEN = yes
export esm_test_app_SOCLIST

############################
# sdl vtm function test
############################
sdl_vtm_test_app_COMP_LIST = sdl_vtm_test_app
sdl_vtm_test_app_RELPATH = test/vtm/vtm_func
sdl_vtm_test_app_PATH = $(sdl_PATH)/$(sdl_vtm_test_app_RELPATH)
sdl_vtm_test_app_BOARD_DEPENDENCY = yes
sdl_vtm_test_app_CORE_DEPENDENCY = yes
export sdl_vtm_test_app_COMP_LIST
export sdl_vtm_test_app_PLATFORM_DEPENDENCY
export sdl_vtm_test_app_CORE_DEPENDENCY
sdl_vtm_test_app_PKG_LIST = sdl_vtm_test_app
sdl_vtm_test_app_INCLUDE = $(sdl_vtm_test_app_PATH)
sdl_vtm_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_vtm_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_vtm_test_app_SBL_APPIMAGEGEN = yes
export sdl_vtm_test_app_SOCLIST

############################
# sdl ASM utils test sdl_utils_test_app
############################
sdl_utils_test_app_COMP_LIST = sdl_utils_test_app
sdl_utils_test_app_RELPATH = test/utils/
sdl_utils_test_app_PATH = $(sdl_PATH)/$(sdl_utils_test_app_RELPATH)
sdl_utils_test_app_BOARD_DEPENDENCY = yes
sdl_utils_test_app_CORE_DEPENDENCY = yes
export sdl_utils_test_app_COMP_LIST
export sdl_utils_test_app_PLATFORM_DEPENDENCY
export sdl_utils_test_app_CORE_DEPENDENCY
sdl_utils_test_app_PKG_LIST = sdl_utils_test_app
sdl_utils_test_app_INCLUDE = $(sdl_utils_test_app_PATH)
sdl_utils_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_utils_test_app_SBL_APPIMAGEGEN = yes
export sdl_utils_test_app_SOCLIST

############################
# sdl dcc test
############################
dcc_test_app_COMP_LIST = dcc_test_app
dcc_test_app_RELPATH = test/dcc/unit
dcc_test_app_PATH = $(sdl_PATH)/$(dcc_test_app_RELPATH)
dcc_test_app_BOARD_DEPENDENCY = yes
dcc_test_app_CORE_DEPENDENCY = yes
export dcc_test_app_COMP_LIST
export dcc_test_app_PLATFORM_DEPENDENCY
export dcc_test_app_CORE_DEPENDENCY
dcc_test_app_PKG_LIST = dcc_test_app
dcc_test_app_INCLUDE = $(dcc_test_app_PATH)
dcc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_test_app_OSLIST = $(DEFAULT_OSLIST)
export dcc_test_app_SBL_APPIMAGEGEN = yes
export dcc_test_app_SOCLIST

############################
# sdl DCC Function test
############################
dcc_func_test_app_COMP_LIST = dcc_func_test_app
dcc_func_test_app_RELPATH = test/dcc/func_test
dcc_func_test_app_PATH = $(sdl_PATH)/$(dcc_func_test_app_RELPATH)
dcc_func_test_app_BOARD_DEPENDENCY = yes
dcc_func_test_app_CORE_DEPENDENCY = yes
export dcc_func_test_app_COMP_LIST
export dcc_func_test_app_PLATFORM_DEPENDENCY
export dcc_func_test_app_CORE_DEPENDENCY
dcc_func_test_app_PKG_LIST = dcc_func_test_app
dcc_func_test_app_INCLUDE = $(dcc_func_test_app_PATH)
dcc_func_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_func_test_app_OSLIST = $(DEFAULT_OSLIST)
export dcc_func_test_app_SBL_APPIMAGEGEN = yes
export dcc_func_test_app_SOCLIST

############################
# sdl mcrc test
############################
mcrc_test_app_COMP_LIST = mcrc_test_app
mcrc_test_app_RELPATH = test/mcrc
mcrc_test_app_PATH = $(sdl_PATH)/$(mcrc_test_app_RELPATH)
mcrc_test_app_BOARD_DEPENDENCY = yes
mcrc_test_app_CORE_DEPENDENCY = yes
export mcrc_test_app_COMP_LIST
export mcrc_test_app_PLATFORM_DEPENDENCY
export mcrc_test_app_CORE_DEPENDENCY
mcrc_test_app_PKG_LIST = mcrc_test_app
mcrc_test_app_INCLUDE = $(mcrc_test_app_PATH)
mcrc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export mcrc_test_app_OSLIST = $(DEFAULT_OSLIST)
export mcrc_test_app_SBL_APPIMAGEGEN = yes
export mcrc_test_app_SOCLIST

############################
# sdl mcrc test
############################
sdl_mcrc_test_app_COMP_LIST = sdl_mcrc_test_app
sdl_mcrc_test_app_RELPATH = test/mcrc
sdl_mcrc_test_app_PATH = $(sdl_PATH)/$(sdl_mcrc_test_app_RELPATH)
sdl_mcrc_test_app_BOARD_DEPENDENCY = yes
sdl_mcrc_test_app_CORE_DEPENDENCY = yes
sdl_mcrc_test_app_MAKEFILE = -f makefile_mcrcFuncTest
export sdl_mcrc_test_app_MAKEFILE
export sdl_mcrc_test_app_COMP_LIST
export sdl_mcrc_test_app_PLATFORM_DEPENDENCY
export sdl_mcrc_test_app_CORE_DEPENDENCY
sdl_mcrc_test_app_PKG_LIST = sdl_mcrc_test_app
sdl_mcrc_test_app_INCLUDE = $(sdl_mcrc_test_app_PATH)
sdl_mcrc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_mcrc_test_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_mcrc_test_app_SBL_APPIMAGEGEN = yes
export sdl_mcrc_test_app_SOCLIST

############################
# sdl pmu test
############################
pmu_test_app_COMP_LIST = pmu_test_app
pmu_test_app_RELPATH = test/pmu
pmu_test_app_PATH = $(sdl_PATH)/$(pmu_test_app_RELPATH)
pmu_test_app_BOARD_DEPENDENCY = yes
pmu_test_app_CORE_DEPENDENCY = yes
export pmu_test_app_COMP_LIST
export pmu_test_app_PLATFORM_DEPENDENCY
export pmu_test_app_CORE_DEPENDENCY
pmu_test_app_PKG_LIST = pmu_test_app
pmu_test_app_INCLUDE = $(pmu_test_app_PATH)
pmu_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export pmu_test_app_OSLIST = $(DEFAULT_OSLIST)
export pmu_test_app_SBL_APPIMAGEGEN = yes
export pmu_test_app_SOCLIST

############################
# sdl vim test
############################
vim_test_app_COMP_LIST = vim_test_app
vim_test_app_RELPATH = test/vim
vim_test_app_PATH = $(sdl_PATH)/$(vim_test_app_RELPATH)
vim_test_app_BOARD_DEPENDENCY = yes
vim_test_app_CORE_DEPENDENCY = yes
export vim_test_app_COMP_LIST
export vim_test_app_PLATFORM_DEPENDENCY
export vim_test_app_CORE_DEPENDENCY
vim_test_app_PKG_LIST = vim_test_app
vim_test_app_INCLUDE = $(vim_test_app_PATH)
vim_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export vim_test_app_OSLIST = $(DEFAULT_OSLIST)
export vim_test_app_SBL_APPIMAGEGEN = yes
export vim_test_app_SOCLIST

############################
# sdl msmc test
############################
msmc_test_app_COMP_LIST = msmc_test_app
msmc_test_app_RELPATH = test/msmc
msmc_test_app_PATH = $(sdl_PATH)/$(msmc_test_app_RELPATH)
msmc_test_app_BOARD_DEPENDENCY = yes
msmc_test_app_CORE_DEPENDENCY = yes
export msmc_test_app_COMP_LIST
export msmc_test_app_PLATFORM_DEPENDENCY
export msmc_test_app_CORE_DEPENDENCY
msmc_test_app_PKG_LIST = msmc_test_app
msmc_test_app_INCLUDE = $(msmc_test_app_PATH)
msmc_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export msmc_test_app_OSLIST = $(DEFAULT_OSLIST)
export msmc_test_app_SBL_APPIMAGEGEN = yes
export msmc_test_app_SOCLIST

############################
# sdl mcrc Auto mode test
############################
sdl_mcrc_autoModeTest_app_COMP_LIST = sdl_mcrc_autoModeTest_app
sdl_mcrc_autoModeTest_app_RELPATH = test/mcrc
sdl_mcrc_autoModeTest_app_PATH = $(sdl_PATH)/$(sdl_mcrc_autoModeTest_app_RELPATH)
sdl_mcrc_autoModeTest_app_BOARD_DEPENDENCY = yes
sdl_mcrc_autoModeTest_app_CORE_DEPENDENCY = yes
sdl_mcrc_autoModeTest_app_MAKEFILE = -f makefile_mcrcAutoModeTest
export sdl_mcrc_autoModeTest_app_MAKEFILE
export sdl_mcrc_autoModeTest_app_COMP_LIST
export sdl_mcrc_autoModeTest_app_PLATFORM_DEPENDENCY
export sdl_mcrc_autoModeTest_app_CORE_DEPENDENCY
sdl_mcrc_autoModeTest_app_PKG_LIST = sdl_mcrc_autoModeTest_app
sdl_mcrc_autoModeTest_app_INCLUDE = $(sdl_mcrc_autoModeTest_app_PATH)
sdl_mcrc_autoModeTest_app_SOCLIST = $(DEFAULT_SOCLIST)
export sdl_mcrc_autoModeTest_app_OSLIST = $(DEFAULT_OSLIST)
export sdl_mcrc_autoModeTest_app_SBL_APPIMAGEGEN = yes
export sdl_mcrc_autoModeTest_app_SOCLIST

############################
# sdl rom_checksum function test
############################
rom_checksum_func_test_app_COMP_LIST = rom_checksum_func_test_app
rom_checksum_func_test_app_RELPATH = test/rom_checksum/functional_test
rom_checksum_func_test_app_PATH = $(sdl_PATH)/$(rom_checksum_func_test_app_RELPATH)
rom_checksum_func_test_app_BOARD_DEPENDENCY = yes
rom_checksum_func_test_app_CORE_DEPENDENCY = yes
export rom_checksum_func_test_app_COMP_LIST
export rom_checksum_func_test_app_PLATFORM_DEPENDENCY
export rom_checksum_func_test_app_CORE_DEPENDENCY
rom_checksum_func_test_app_PKG_LIST = rom_checksum_func_test_app
rom_checksum_func_test_app_INCLUDE = $(rom_checksum_func_test_app_PATH)
rom_checksum_func_test_app_SOCLIST = $(DEFAULT_SOCLIST)
export rom_checksum_func_test_app_OSLIST = $(DEFAULT_OSLIST)
export rom_checksum_func_test_app_SBL_APPIMAGEGEN = yes
export rom_checksum_func_test_app_SOCLIST

############################
#device sdl examples
# List of examples under SDL
# All the tests mentioned in list are built when test target is called
# List below all examples for allowed values
############################
ifeq ($(SOC),$(filter $(SOC),  j784s4))

sdl_EXAMPLE_LIST +=  rti_app_uc1 rti_app_uc2 rti_app_uc3 rti_app_uc4 mcrc_app dcc_app_uc1 dcc_app_uc3 dcc_app_uc4_2 dcc_app_uc5 dcc_app_uc6 dcc_app_uc7 dcc_app_uc8 osal_app mtog_app pok_app cpu_app vtm_app tog_app esm_wkup_mcu_app bist_example_app esm_main_app ecc_app rom_checksum_example

endif

ifeq ($(SOC),$(filter $(SOC),  j721s2))
sdl_EXAMPLE_LIST += osal_app rti_app_uc1 rti_app_uc2 rti_app_uc3 rti_app_uc4 vtm_app dcc_app_uc1 dcc_app_uc3 dcc_app_uc4_2 dcc_app_uc5 dcc_app_uc6 dcc_app_uc7 dcc_app_uc8 mtog_app mcrc_app pok_app esm_main_app esm_wkup_mcu_app ecc_app cpu_app tog_app bist_example_app rom_checksum_example dm_firmware
endif

ifeq ($(SOC),$(filter $(SOC),  j721e j7200))
sdl_EXAMPLE_LIST += hello_world_app osal_app pok_app tog_app vtm_app esm_main_app esm_wkup_mcu_app rti_app_uc1 rti_app_uc2 rti_app_uc3 rti_app_uc4 dcc_app_uc1 dcc_app_uc3 dcc_app_uc4_2 dcc_app_uc5 dcc_app_uc6 dcc_app_uc7 dcc_app_uc8 ecc_app cpu_app mcrc_app bist_example_app rom_checksum_example
 
ifeq ($(SOC),$(filter $(SOC),  j7200))
sdl_EXAMPLE_LIST += mtog_app
endif
endif

hello_world_app_RELPATH = examples/hello_world_r5
hello_world_app_PATH= $(sdl_PATH)/$(hello_world_app_RELPATH)
hello_world_app_PLATFORM_DEPENDENCY = yes
hello_world_app_SOCLIST = $(DEFAULT_SOCLIST)
export hello_world_app_OSLIST = $(DEFAULT_OSLIST)
export hello_world_app_SOCLIST
export hello_world_app_PLATFORM_DEPENDENCY

mcrc_app_RELPATH = examples/mcrc
mcrc_app_PATH = $(sdl_PATH)/$(mcrc_app_RELPATH)
mcrc_app_PLATFORM_DEPENDENCY = yes
mcrc_app_SOCLIST = $(DEFAULT_SOCLIST)
export mcrc_app_OSLIST = $(DEFAULT_OSLIST)
export mcrc_app_SOCLIST
export mcrc_app_PLATFORM_DEPENDENCY
export mcrc_app_SBL_APPIMAGEGEN = yes

osal_app_RELPATH = examples/osal
osal_app_PATH= $(sdl_PATH)/$(osal_app_RELPATH)
osal_app_PLATFORM_DEPENDENCY = yes
osal_app_SOCLIST = $(DEFAULT_SOCLIST)
osal_app_OSLIST = baremetal freertos
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
osal_app_OSLIST += safertos
endif
export osal_app_OSLIST
export osal_app_SOCLIST
export osal_app_PLATFORM_DEPENDENCY
export osal_app_SBL_APPIMAGEGEN = yes

pok_app_RELPATH = examples/pok
pok_app_PATH= $(sdl_PATH)/$(pok_app_RELPATH)
pok_app_PLATFORM_DEPENDENCY = yes
pok_app_SOCLIST = $(DEFAULT_SOCLIST)
export pok_app_OSLIST = $(DEFAULT_OSLIST)
export pok_app_SOCLIST
export pok_app_PLATFORM_DEPENDENCY
export pok_app_SBL_APPIMAGEGEN = yes

tog_app_RELPATH = examples/tog
tog_app_PATH= $(sdl_PATH)/$(tog_app_RELPATH)
tog_app_PLATFORM_DEPENDENCY = yes
tog_app_SOCLIST = $(DEFAULT_SOCLIST)
export tog_app_OSLIST = $(DEFAULT_OSLIST)
export tog_app_SOCLIST
export tog_app_PLATFORM_DEPENDENCY
export tog_app_SBL_APPIMAGEGEN = yes

mtog_app_RELPATH = examples/mtog
mtog_app_PATH= $(sdl_PATH)/$(mtog_app_RELPATH)
mtog_app_PLATFORM_DEPENDENCY = yes
mtog_app_SOCLIST = $(DEFAULT_SOCLIST)
export mtog_app_OSLIST = $(DEFAULT_OSLIST)
export mtog_app_SOCLIST
export mtog_app_PLATFORM_DEPENDENCY
export mtog_app_SBL_APPIMAGEGEN = yes

vtm_app_RELPATH = examples/vtm
vtm_app_PATH= $(sdl_PATH)/$(vtm_app_RELPATH)
vtm_app_PLATFORM_DEPENDENCY = yes
vtm_app_SOCLIST = $(DEFAULT_SOCLIST)
export vtm_app_OSLIST = $(DEFAULT_OSLIST)
export vtm_app_SOCLIST
export vtm_app_PLATFORM_DEPENDENCY
export vtm_app_SBL_APPIMAGEGEN = yes

esm_main_app_RELPATH = examples/esm/main_esm0
esm_main_app_PATH= $(sdl_PATH)/$(esm_main_app_RELPATH)
esm_main_app_PLATFORM_DEPENDENCY = yes
esm_main_app_SOCLIST = $(DEFAULT_SOCLIST)
esm_main_app_OSLIST = baremetal freertos
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
esm_main_app_OSLIST += safertos
endif
export esm_main_app_OSLIST
export esm_main_app_SOCLIST
export esm_main_app_PLATFORM_DEPENDENCY
export esm_main_app_SBL_APPIMAGEGEN = yes

ecc_app_RELPATH = examples/ecc
ecc_app_PATH= $(sdl_PATH)/$(ecc_app_RELPATH)
ecc_app_PLATFORM_DEPENDENCY = yes
ecc_app_SOCLIST = $(DEFAULT_SOCLIST)
export ecc_app_OSLIST = $(DEFAULT_OSLIST)
export ecc_app_SOCLIST
export ecc_app_PLATFORM_DEPENDENCY
export ecc_app_SBL_APPIMAGEGEN = yes

esm_wkup_mcu_app_RELPATH = examples/esm/wkup_mcu_esm0
esm_wkup_mcu_app_PATH= $(sdl_PATH)/$(esm_wkup_mcu_app_RELPATH)
esm_wkup_mcu_app_PLATFORM_DEPENDENCY = yes
esm_wkup_mcu_app_SOCLIST = $(DEFAULT_SOCLIST)
export esm_wkup_mcu_app_OSLIST = $(DEFAULT_OSLIST)
export esm_wkup_mcu_app_SOCLIST
export esm_wkup_mcu_app_PLATFORM_DEPENDENCY
export esm_wkup_mcu_app_SBL_APPIMAGEGEN = yes

rti_app_uc1_RELPATH = examples/rti/UC1
rti_app_uc1_PATH= $(sdl_PATH)/$(rti_app_uc1_RELPATH)
rti_app_uc1_PLATFORM_DEPENDENCY = yes
rti_app_uc1_SOCLIST = $(DEFAULT_SOCLIST)
export rti_app_uc1_OSLIST = $(DEFAULT_OSLIST)
export rti_app_uc1_SOCLIST
export rti_app_uc1_PLATFORM_DEPENDENCY
export rti_app_uc1_SBL_APPIMAGEGEN = yes

rti_app_uc2_RELPATH = examples/rti/UC2
rti_app_uc2_PATH= $(sdl_PATH)/$(rti_app_uc2_RELPATH)
rti_app_uc2_PLATFORM_DEPENDENCY = yes
rti_app_uc2_SOCLIST = $(DEFAULT_SOCLIST)
export rti_app_uc2_OSLIST = $(DEFAULT_OSLIST)
export rti_app_uc2_SOCLIST
export rti_app_uc2_PLATFORM_DEPENDENCY
export rti_app_uc2_SBL_APPIMAGEGEN = yes

rti_app_uc3_RELPATH = examples/rti/UC3
rti_app_uc3_PATH= $(sdl_PATH)/$(rti_app_uc3_RELPATH)
rti_app_uc3_PLATFORM_DEPENDENCY = yes
rti_app_uc3_SOCLIST = $(DEFAULT_SOCLIST)
export rti_app_uc3_OSLIST = $(DEFAULT_OSLIST)
export rti_app_uc3_SOCLIST
export rti_app_uc3_PLATFORM_DEPENDENCY
export rti_app_uc3_SBL_APPIMAGEGEN = yes

rti_app_uc4_RELPATH = examples/rti/UC4
rti_app_uc4_PATH= $(sdl_PATH)/$(rti_app_uc4_RELPATH)
rti_app_uc4_PLATFORM_DEPENDENCY = yes
rti_app_uc4_SOCLIST = $(DEFAULT_SOCLIST)
export rti_app_uc4_OSLIST = $(DEFAULT_OSLIST)
export rti_app_uc4_SOCLIST
export rti_app_uc4_PLATFORM_DEPENDENCY
export rti_app_uc4_SBL_APPIMAGEGEN = yes

SDL_LIBS = $(sdl_LIB_LIST)
SDL_TEST_LIBS = $(sdl_TEST_LIST)
SDL_EXAMPLE_LIBS = $(sdl_EXAMPLE_LIST)
sdl_PKG_LIST_LIBS = $(SDL_LIBS)
sdl_PKG_LIST_TESTS = $(SDL_EXAMPLE_LIBS) $(SDL_TEST_LIBS)
sdl_PKG_LIST_ALL = $(sdl_PKG_LIST_LIBS) $(sdl_PKG_LIST_TESTS)

dcc_app_uc1_RELPATH = examples/dcc/UC1
dcc_app_uc1_PATH= $(sdl_PATH)/$(dcc_app_uc1_RELPATH)
dcc_app_uc1_PLATFORM_DEPENDENCY = yes
dcc_app_uc1_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc1_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc1_SOCLIST
export dcc_app_uc1_PLATFORM_DEPENDENCY
export dcc_app_uc1_SBL_APPIMAGEGEN = yes

dcc_app_uc2_RELPATH = examples/dcc/UC2
dcc_app_uc2_PATH= $(sdl_PATH)/$(dcc_app_uc2_RELPATH)
dcc_app_uc2_PLATFORM_DEPENDENCY = yes
dcc_app_uc2_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc2_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc2_SOCLIST
export dcc_app_uc2_PLATFORM_DEPENDENCY
export dcc_app_uc2_SBL_APPIMAGEGEN = yes

dcc_app_uc3_RELPATH = examples/dcc/UC3
dcc_app_uc3_PATH= $(sdl_PATH)/$(dcc_app_uc3_RELPATH)
dcc_app_uc3_PLATFORM_DEPENDENCY = yes
dcc_app_uc3_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc3_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc3_SOCLIST
export dcc_app_uc3_PLATFORM_DEPENDENCY
export dcc_app_uc3_SBL_APPIMAGEGEN = yes

dcc_app_uc4_2_RELPATH = examples/dcc/UC4_2
dcc_app_uc4_2_PATH= $(sdl_PATH)/$(dcc_app_uc4_2_RELPATH)
dcc_app_uc4_2_PLATFORM_DEPENDENCY = yes
dcc_app_uc4_2_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc4_2_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc4_2_SOCLIST
export dcc_app_uc4_2_PLATFORM_DEPENDENCY
export dcc_app_uc4_2_SBL_APPIMAGEGEN = yes

dcc_app_uc5_RELPATH = examples/dcc/UC5
dcc_app_uc5_PATH= $(sdl_PATH)/$(dcc_app_uc5_RELPATH)
dcc_app_uc5_PLATFORM_DEPENDENCY = yes
dcc_app_uc5_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc5_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc5_SOCLIST
export dcc_app_uc5_PLATFORM_DEPENDENCY
export dcc_app_uc5_SBL_APPIMAGEGEN = yes

dcc_app_uc6_RELPATH = examples/dcc/UC6
dcc_app_uc6_PATH= $(sdl_PATH)/$(dcc_app_uc6_RELPATH)
dcc_app_uc6_PLATFORM_DEPENDENCY = yes
dcc_app_uc6_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc6_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc6_SOCLIST
export dcc_app_uc6_PLATFORM_DEPENDENCY
export dcc_app_uc6_SBL_APPIMAGEGEN = yes

dcc_app_uc7_RELPATH = examples/dcc/UC7
dcc_app_uc7_PATH= $(sdl_PATH)/$(dcc_app_uc7_RELPATH)
dcc_app_uc7_PLATFORM_DEPENDENCY = yes
dcc_app_uc7_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc7_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc7_SOCLIST
export dcc_app_uc7_PLATFORM_DEPENDENCY
export dcc_app_uc7_SBL_APPIMAGEGEN = yes

dcc_app_uc8_RELPATH = examples/dcc/UC8
dcc_app_uc8_PATH= $(sdl_PATH)/$(dcc_app_uc8_RELPATH)
dcc_app_uc8_PLATFORM_DEPENDENCY = yes
dcc_app_uc8_SOCLIST = $(DEFAULT_SOCLIST)
export dcc_app_uc8_OSLIST = $(DEFAULT_OSLIST)
export dcc_app_uc8_SOCLIST
export dcc_app_uc8_PLATFORM_DEPENDENCY
export dcc_app_uc8_SBL_APPIMAGEGEN = yes

dm_firmware_RELPATH = examples/dm_firmware
dm_firmware_PATH = $(sdl_PATH)/$(dm_firmware_RELPATH)
dm_firmware_PLATFORM_DEPENDENCY = yes
dm_firmware_SOCLIST = $(DEFAULT_SOCLIST)
export dm_firmware_OSLIST = freertos
export dm_firmware_SOCLIST
export dm_firmware_PLATFORM_DEPENDENCY
export dm_firmware_APPIMAGEGEN = no

cpu_app_COMP_LIST = cpu_app
cpu_app_RELPATH =examples/cpu
cpu_app_PATH = $(sdl_PATH)/$(cpu_app_RELPATH)
cpu_app_BOARD_DEPENDENCY = yes
cpu_app_CORE_DEPENDENCY = yes
export cpu_app_COMP_LIST
export cpu_app_PLATFORM_DEPENDENCY
export cpu_app_CORE_DEPENDENCY
cpu_app_PKG_LIST = cpu_app
cpu_app_INCLUDE = $(cpu_app_PATH)
cpu_app_SOCLIST = $(DEFAULT_SOCLIST)
export cpu_app_OSLIST = $(DEFAULT_OSLIST)
cpu_app_APP_SMP_CONFIG = yes
export cpu_app_APP_SMP_CONFIG
export cpu_app_SBL_APPIMAGEGEN = yes
export cpu_app_SOCLIST

bist_example_app_RELPATH = examples/bist
bist_example_app_PATH= $(sdl_PATH)/$(bist_example_app_RELPATH)
bist_exmaple_PLATFORM_DEPENDENCY = yes
bist_example_app_SOCLIST = $(DEFAULT_SOCLIST)
export bist_example_app_OSLIST = $(DEFAULT_OSLIST)
export bist_example_app_SOCLIST
export bist_example_app_PLATFORM_DEPENDENCY
export bist_example_app_SBL_APPIMAGEGEN = yes

rom_checksum_example_RELPATH = examples/rom_checksum
rom_checksum_example_PATH= $(sdl_PATH)/$(rom_checksum_example_RELPATH)
rom_checksum_example_PLATFORM_DEPENDENCY = yes
rom_checksum_example_SOCLIST = $(DEFAULT_SOCLIST)
export rom_checksum_example_OSLIST = $(DEFAULT_OSLIST)
export rom_checksum_example_SOCLIST
export rom_checksum_example_PLATFORM_DEPENDENCY
export rom_checksum_example_SBL_APPIMAGEGEN = yes

#Jacinto uses PDK modules in the test applications
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))

pdk_INCLUDE = $(pdk_PATH)
export pdk_INCLUDE
-include $(PDK_INSTALL_PATH)/ti/build/comp_paths.mk

-include $(PDK_CSL_COMP_PATH)/csl_component.mk
ifneq ($(csl_LIB_LIST),)
  sdl_pdk_LIB_LIST += csl csl_init
endif

-include $(PDK_SCICLIENT_COMP_PATH)/sciclient_component.mk
ifeq ($(SOC),$(filter $(SOC), j721e j7200 j721s2 j784s4))
ifneq ($(sciclient_LIB_LIST),)
  sdl_pdk_LIB_LIST += sciclient_direct
  sdl_pdk_LIB_LIST += rm_pm_hal
endif
endif

-include $(PDK_I2C_COMP_PATH)/i2c_component.mk
ifneq ($(drvi2c_LIB_LIST),)
  sdl_pdk_LIB_LIST += $(i2c_LIB_LIST)
endif

#include ipc
-include $(PDK_IPC_COMP_PATH)/ipc_component.mk
ifneq ($(ipc_LIB_LIST),)
  sdl_pdk_LIB_LIST += $(ipc_LIB_LIST)
endif

#include osal
-include $(PDK_OSAL_COMP_PATH)/osal_component.mk
ifneq ($(osal_LIB_LIST),)
  sdl_pdk_LIB_LIST += osal_nonos
  sdl_pdk_LIB_LIST += osal_freertos
ifneq ($(wildcard $(SAFERTOS_KERNEL_INSTALL_PATH)),)
  sdl_pdk_LIB_LIST += osal_safertos
endif
endif

#include board
-include $(PDK_BOARD_COMP_PATH)/board_component.mk
ifneq ($(board_LIB_LIST),)
  sdl_pdk_LIB_LIST += board
endif

#include uart
-include $(PDK_UART_COMP_PATH)/uart_component.mk
ifneq ($(uart_LIB_LIST),)
  sdl_pdk_LIB_LIST += uart
endif


#include udma
-include $(PDK_UDMA_COMP_PATH)/udma_component.mk
ifneq ($(udma_LIB_LIST),)
  sdl_pdk_LIB_LIST += udma
endif

#include freertos
-include $(PDK_FREERTOS_COMP_PATH)/freertos_component.mk
ifneq ($(freertos_LIB_LIST),)
  sdl_pdk_LIB_LIST += $(freertos_LIB_LIST)
endif

#include safertos
-include $(PDK_SAFERTOS_COMP_PATH)/safertos_component.mk
ifneq ($(safertos_LIB_LIST),)
  sdl_pdk_LIB_LIST += $(safertos_LIB_LIST)
endif

endif

# Component specific CFLAGS
#CFLAGS_COMP_COMMON
SDL_CFLAGS =
SDL_LNKFLAGS =

ifeq ($(BUILD_OS_TYPE),freertos)
  SDL_CFLAGS += -DFREERTOS
endif

ifeq ($(BUILD_OS_TYPE),safertos)
  SDL_CFLAGS += -DSAFERTOS
endif

ifeq ($(CORE),$(filter $(CORE), r5f))
  SDL_CFLAGS += -DBUILD_R5F
  SDL_LNKFLAGS += --define=BUILD_R5F
endif

ifeq ($(PROFILE_$(CORE)),debug)
  SDL_CFLAGS += -DDEBUG_BUILD
# Enable asserts for debug build
  SDL_CFLAGS += -DASSERT_ENABLE
endif

ifeq ($(PROFILE_$(CORE)),release)
  SDL_CFLAGS += -DRELEASE_BUILD
# Enable asserts for release build
  SDL_CFLAGS += -DASSERT_ENABLE
endif

ifeq ($(SOC),$(filter $(SOC),j721e))
  CFLAGS_GLOBAL_j721e += -DSOC_J721E
  CFLAGS_GLOBAL_j721e += -Dmcu1_0
endif

ifeq ($(SOC),$(filter $(SOC),j7200))
  CFLAGS_GLOBAL_j7200 += -DSOC_J7200
  CFLAGS_GLOBAL_j7200 += -Dmcu1_0
endif

ifeq ($(SOC),$(filter $(SOC),j721s2))
  CFLAGS_GLOBAL_j721s2 += -DSOC_J721S2
  CFLAGS_GLOBAL_j721s2 += -Dmcu1_0
endif

ifeq ($(SOC),$(filter $(SOC),j784s4))
  CFLAGS_GLOBAL_j784s4 += -DSOC_J784S4
  CFLAGS_GLOBAL_j784s4 += -Dmcu1_0
endif

export SDL_LIBS
export SDL_TEST_LIBS
export SDL_EXAMPLE_LIBS
export SDL_CFLAGS
export SDL_LNKFLAGS

component_make_include := 1
endif

# Nothing beyond this point
