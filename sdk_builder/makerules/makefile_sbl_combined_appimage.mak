#
# Utility makefile for SBL boot support with combined appimage tool
#
# Edit this file to suit your build needs
#

SBL_CORE=mcu1_0
BOARD=$(BUILD_PDK_BOARD)
COMBINED_APPIMAGE_ATF_OPTEE_PATH=$(VISION_APPS_PATH)/out/sbl_combined_bootfiles/atf_optee_dir
ATF_TARGET_BOARD=generic
ifeq ($(SOC),j784s4)
	ATF_TARGET_BOARD=j784s4
endif
SBL_REPO_PATH=$(PDK_PATH)/packages/ti/boot/sbl
COMBINED_APPIMAGE_TOOL_PATH=$(SBL_REPO_PATH)/tools/combined_appimage

##############
### Main
##############
sbl_combined_bootimage: sbl_sd atf_optee sbl_vision_apps sbl_qnx_combined_bootimage

##############
### Install
##############
sbl_combined_bootimage_install_sd: sbl_combined_bootimage
	cp $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/tiboot3.bin $(SBL_SD_FS_PATH)
	cp $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/tifs.bin $(SBL_SD_FS_PATH)
	cp $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/app $(SBL_SD_FS_PATH)
	sync

##############
### SBL
##############
sbl_sd:
	$(MAKE) -C $(PDK_PATH)/packages/ti/build sbl_mmcsd_img TOOLS_INSTALL_PATH=$(PSDK_TOOLS_PATH) DISABLE_RECURSE_DEPS=no BOARD=$(BOARD) CORE=$(SBL_CORE) -s
	mkdir -p $(VISION_APPS_PATH)/out/sbl_combined_bootfiles
	cp $(PDK_PATH)/packages/ti/boot/sbl/binary/$(BOARD)/mmcsd/bin/sbl_mmcsd_img_$(SBL_CORE)_release.tiimage $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/tiboot3.bin
	cp $(PDK_PATH)/packages/ti/drv/sciclient/soc/V1/tifs.bin $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/tifs.bin

##############
### atf_optee
##############
atf_optee: sbl_sd
ifeq ($(BUILD_QNX_MPU), yes)
	# For ATF, setting HANDLE_EA_EL3_FIRST_NS=0 for QNX so that the all runtime exception to be routed to current exception level (or in EL1 if the current exception level is EL0)
	$(MAKE) -C $(VISION_APPS_PATH)/../trusted-firmware-a -s -j32 CROSS_COMPILE=$(GCC_LINUX_ARM_ROOT)/bin/aarch64-none-linux-gnu- PLAT=k3 TARGET_BOARD=$(ATF_TARGET_BOARD) SPD=opteed  HANDLE_EA_EL3_FIRST_NS=0
endif
ifeq ($(BUILD_LINUX_MPU), yes)
	$(MAKE) -C $(VISION_APPS_PATH)/../trusted-firmware-a -s -j32 CROSS_COMPILE=$(GCC_LINUX_ARM_ROOT)/bin/aarch64-none-linux-gnu- PLAT=k3 TARGET_BOARD=$(ATF_TARGET_BOARD) SPD=opteed
endif
	$(MAKE) -C $(VISION_APPS_PATH)/../ti-optee-os -s -j32 CROSS_COMPILE_core=$(GCC_LINUX_ARM_ROOT)/bin/aarch64-none-linux-gnu- CROSS_COMPILE_ta_arm32=$(GCC_LINUX_ARM_ROOT)/bin/aarch64-none-linux-gnu- CROSS_COMPILE_ta_arm64=$(GCC_LINUX_ARM_ROOT)/bin/aarch64-none-linux-gnu- NOWERROR=1 CFG_TEE_TA_LOG_LEVEL=0 CFG_TEE_CORE_LOG_LEVEL=2 CFG_ARM64_core=y ta-targets=ta_arm64 PLATFORM=k3 PLATFORM_FLAVOR=j7
	mkdir -p $(COMBINED_APPIMAGE_ATF_OPTEE_PATH)
	cp $(VISION_APPS_PATH)/../trusted-firmware-a/build/k3/$(ATF_TARGET_BOARD)/release/bl31.bin $(COMBINED_APPIMAGE_ATF_OPTEE_PATH)/bl31.bin
	cp $(VISION_APPS_PATH)/../ti-optee-os/out/arm-plat-k3/core/tee-pager_v2.bin $(COMBINED_APPIMAGE_ATF_OPTEE_PATH)/bl32.bin


##############
### vision apps
##############
INPUT_IMG_PATH = $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/vision_apps

sbl_vision_apps: sbl_sd
	mkdir -p $(INPUT_IMG_PATH)
	# build DM image
ifeq ($(BUILD_CPU_MCU1_0),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu1_0.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu1_0.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu1_0.xer5f
else
	$(MAKE) -C $(PDK_PATH)/packages/ti/build   ipc_qnx_echo_testb_freertos TOOLS_INSTALL_PATH=$(PSDK_TOOLS_PATH) CORE=mcu1_0 BOARD=$(BOARD) SOC=$(SOC) BUILD_PROFILE=release -s
	cp $(PDK_PATH)/packages/ti/binary/ipc_qnx_echo_testb_freertos/bin/$(BOARD)/ipc_qnx_echo_testb_freertos_mcu1_0_release_strip.xer5f $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu1_0.xer5f
endif
ifeq ($(BUILD_CPU_MCU2_0),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu2_0.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu2_0.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu2_0.xer5f
endif
ifeq ($(BUILD_CPU_MCU2_1),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu2_1.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu2_1.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu2_1.xer5f
endif
ifeq ($(BUILD_CPU_MCU3_0),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu3_0.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu3_0.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu3_0.xer5f
endif
ifeq ($(BUILD_CPU_MCU3_1),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu3_1.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu3_1.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu3_1.xer5f
endif
ifeq ($(BUILD_CPU_MCU4_0),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu4_0.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu4_0.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu4_0.xer5f
endif
ifeq ($(BUILD_CPU_MCU4_1),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/R5F/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_mcu4_1.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu4_1.xer5f
	$(TIARMCGT_LLVM_ROOT)/bin/tiarmstrip -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu4_1.xer5f
endif
ifeq ($(BUILD_CPU_C6x_1),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/C66/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_c6x_1.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c6x_1.xe66
	$(CGT6X_ROOT)/bin/strip6x -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c6x_1.xe66
endif
ifeq ($(BUILD_CPU_C6x_2),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/C66/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_c6x_2.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c6x_2.xe66
	$(CGT6X_ROOT)/bin/strip6x -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c6x_2.xe66
endif
ifeq ($(BUILD_CPU_C7x_1),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/$(C7X_TARGET)/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_c7x_1.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_1.xe71
	$(CGT7X_ROOT)/bin/strip7x -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_1.xe71
endif
ifeq ($(BUILD_CPU_C7x_2),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/$(C7X_TARGET)/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_c7x_2.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_2.xe71
	$(CGT7X_ROOT)/bin/strip7x -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_2.xe71
endif
ifeq ($(BUILD_CPU_C7x_3),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/$(C7X_TARGET)/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_c7x_3.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_3.xe71
	$(CGT7X_ROOT)/bin/strip7x -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_3.xe71
endif
ifeq ($(BUILD_CPU_C7x_4),yes)
	cp $(VISION_APPS_PATH)/out/$(TARGET_SOC)/$(C7X_TARGET)/$(RTOS)/$(QNX_APP_PROFILE)/vx_app_rtos_qnx_c7x_4.out $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_4.xe71
	$(CGT7X_ROOT)/bin/strip7x -p $(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_4.xe71
endif

##############################
## Combined bootapp image
##############################
OUT_DIR=$(VISION_APPS_PATH)/out/sbl_combined_bootfiles/
ATF_IMG=mpu1_0,$(COMBINED_APPIMAGE_ATF_OPTEE_PATH)/bl31.bin,0x70000000,0x70000000
OPTEE_IMG=load_only,$(COMBINED_APPIMAGE_ATF_OPTEE_PATH)/bl32.bin,0x9e800000,0x9e800000
KERNEL_IMG=load_only,$(QNX_BOOT_PATH)/qnx-ifs,0x80080000,0x80080000
DTB_IMG=
SPL_IMG=
  IMG1=mcu1_0,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu1_0.xer5f
ifeq ($(BUILD_CPU_MCU2_0),yes)
  IMG2=mcu2_0,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu2_0.xer5f
else
  IMG2=
endif
ifeq ($(BUILD_CPU_MCU2_1),yes)
  IMG3=mcu2_1,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu2_1.xer5f
else
  IMG3=
endif
ifeq ($(BUILD_CPU_MCU3_0),yes)
  IMG4=mcu3_0,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu3_0.xer5f
else
  IMG4=
endif
ifeq ($(BUILD_CPU_MCU3_1),yes)
  IMG5=mcu3_1,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_mcu3_1.xer5f
else
  IMG5=
endif
ifeq ($(BUILD_CPU_C6x_1),yes)
  IMG6=c66xdsp_1,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_c6x_1.xe66
else
  IMG6=
endif
ifeq ($(BUILD_CPU_C6x_2),yes)
  IMG7=c66xdsp_2,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_c6x_2.xe66
else
  IMG7=
endif
ifeq ($(BUILD_CPU_C7x_1),yes)
  IMG8=c7x_1,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_1.xe71
else
  IMG8=
endif
ifeq ($(BUILD_CPU_C7x_2),yes)
  IMG9=c7x_2,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_2.xe71
else
  IMG9=
endif
ifeq ($(BUILD_CPU_C7x_3),yes)
  IMG10=c7x_3,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_3.xe71
else
  IMG10=
endif
ifeq ($(BUILD_CPU_C7x_4),yes)
  IMG11=c7x_4,$(INPUT_IMG_PATH)/vx_app_rtos_qnx_c7x_4.xe71
else
  IMG11=
endif

sbl_qnx_combined_bootimage: atf_optee sbl_vision_apps
	echo "---------------------------"
	echo "start"
	echo "---------------------------"
	$(MAKE) -C $(COMBINED_APPIMAGE_TOOL_PATH) PDK_INSTALL_PATH=$(PDK_PATH)/packages OUT_DIR=$(OUT_DIR) ATF_IMG=$(ATF_IMG) OPTEE_IMG=$(OPTEE_IMG) KERNEL_IMG=$(KERNEL_IMG) DTB_IMG=$(DTB_IMG) SPL_IMG=$(SPL_IMG) IMG1=$(IMG1) IMG2=$(IMG2) IMG3=$(IMG3) IMG4=$(IMG4) IMG5=$(IMG5) IMG6=$(IMG6) IMG7=$(IMG7) IMG8=$(IMG8)
	mv $(OUT_DIR)/combined.appimage $(OUT_DIR)/app
	echo "---------------------------"
	echo "done"
	echo "---------------------------"

##############
### clean up
##############
sbl_combined_bootimage_clean: sbl_combined_bootimage_scrub

sbl_combined_bootimage_scrub: atf_optee_scrub
	rm -rf $(PDK_PATH)/packages/ti/binary/sbl_*
	rm -rf $(PDK_PATH)/packages/ti/binary/ti/boot/
	rm -rf $(PDK_PATH)/packages/ti/boot/sbl/binary
	rm -rf $(MCUSW_PATH)/binary
	rm -rf $(VISION_APPS_PATH)/out/sbl_combined_bootfiles/

atf_optee_scrub:
	$(MAKE) -C $(VISION_APPS_PATH)/../trusted-firmware-a clean
	$(MAKE) -C $(VISION_APPS_PATH)/../ti-optee-os clean CFG_ARM64_core=y PLATFORM=k3 PLATFORM_FLAVOR=j7
	rm -rf $(VISION_APPS_PATH)/../trusted-firmware-a/build/k3
	rm -rf $(VISION_APPS_PATH)/../ti-optee-os/out/arm-plat-k3

