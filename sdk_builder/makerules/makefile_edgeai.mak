#
# Utility makefile to build edgeai components
#
# 1. edgeai-apps-utils
# 2. edgeai-tiovx-kernels
# 3. edgeai-tiovx-modules
# 4. edgeai-gst-plugins
# 5. edgeai-tiovx-apps
#

include $(PSDK_PATH)/sdk_builder/tools_path.mak

EDGEAI_UTILS_PATH   ?= $(PSDK_PATH)/edgeai/edgeai-apps-utils/
EDGEAI_KERNELS_PATH   ?= $(PSDK_PATH)/edgeai/edgeai-tiovx-kernels/
EDGEAI_TIOVX_APPS_PATH   ?= $(PSDK_PATH)/edgeai/edgeai-tiovx-apps/
YAML_CPP_PATH   ?= $(PSDK_PATH)/edgeai/yaml-cpp/
EDGEAI_INSTALL_PATH   ?= $(TARGET_FS)
EDGEAI_QNX_STAGING ?= $(PSDK_PATH)/edgeai/QNX

edgeai_check_paths:
	@if [ ! -d $(EDGEAI_UTILS_PATH) ]; then echo 'ERROR: $(EDGEAI_UTILS_PATH) not found !!!'; exit 1; fi
	@if [ ! -d $(EDGEAI_KERNELS_PATH) ]; then echo 'ERROR: $(EDGEAI_KERNELS_PATH) not found !!!'; exit 1; fi
	@if [ ! -d $(EDGEAI_TIOVX_APPS_PATH) ]; then echo 'ERROR: $(EDGEAI_TIOVX_APPS_PATH) not found !!!'; exit 1; fi

edgeai:
	@echo "Building EdgeAI Components"
ifeq ($(BUILD_QNX_MPU), yes)
	mkdir -p $(EDGEAI_QNX_STAGING)/usr/lib
endif
	$(MAKE) edgeai_check_paths
	$(MAKE) edgeai_utils
	$(MAKE) edgeai_kernels
	$(MAKE) edgeai_tiovx_apps

edgeai_utils:
ifeq ($(BUILD_LINUX_MPU), yes)
	@echo "------------------------------------"
	@echo "Building EdgeAI Apps Utils for Linux"
	@echo "------------------------------------"
	cd $(EDGEAI_UTILS_PATH); \
	mkdir -p build; \
	cd build; \
	CROSS_COMPILER_PATH=$(GCC_LINUX_ARM_ROOT) \
	CROSS_COMPILER_PREFIX=aarch64-none-linux-gnu- \
	TARGET_FS=$(LINUX_FS_PATH) \
	cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/cross_compile_aarch64.cmake ..; \
	$(MAKE) install DESTDIR=$(LINUX_FS_PATH)
endif
ifeq ($(BUILD_QNX_MPU), yes)
	@echo "----------------------------------"
	@echo "Building EdgeAI Apps Utils for QNX"
	@echo "----------------------------------"
	cd $(EDGEAI_UTILS_PATH); \
	mkdir -p QNX/build; \
	cd QNX/build; \
	CROSS_COMPILER_PATH=$(QNX_HOST)/usr/ \
	CROSS_COMPILER_PREFIX=$(QNX_CROSS_COMPILER_TOOL) \
	TARGET_FS=$(QNX_TARGET) \
	PSDK_INCLUDE_PATH=$(LINUX_FS_PATH)/usr/include/ \
	cmake -DCMAKE_MAKE_PROGRAM=make \
	-DCMAKE_TOOLCHAIN_FILE=../../cmake/cross_compile_aarch64.cmake ../../; \
	$(MAKE) install DESTDIR=$(EDGEAI_QNX_STAGING)
endif

edgeai_kernels:
ifeq ($(BUILD_LINUX_MPU), yes)
	@echo "---------------------------------"
	@echo "Building EdgeAI Kernels for Linux"
	@echo "---------------------------------"
	cd $(EDGEAI_KERNELS_PATH); \
	mkdir build; \
	cd build; \
	CROSS_COMPILER_PATH=$(GCC_LINUX_ARM_ROOT) \
	CROSS_COMPILER_PREFIX=aarch64-none-linux-gnu- \
	TARGET_FS=$(LINUX_FS_PATH) \
	cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/cross_compile_aarch64.cmake ..; \
	$(MAKE) install DESTDIR=$(LINUX_FS_PATH)
endif
ifeq ($(BUILD_QNX_MPU), yes)
	@echo "----------------------------------"
	@echo "Building EdgeAI Kernels for QNX"
	@echo "----------------------------------"
	cd $(EDGEAI_KERNELS_PATH); \
	mkdir -p QNX/build; \
	cd QNX/build; \
	CROSS_COMPILER_PATH=$(QNX_HOST)/usr/ \
	CROSS_COMPILER_PREFIX=$(QNX_CROSS_COMPILER_TOOL) \
	TARGET_FS=$(QNX_TARGET) \
	TARGET_OS=QNX \
	PSDK_INCLUDE_PATH=$(LINUX_FS_PATH)/usr/include/ \
	VISION_APPS_LIBS_PATH=$(VISION_APPS_PATH)/out/$(TARGET_SOC)/$(MPU_CPU)/QNX/$(LINUX_APP_PROFILE)/ \
	EDGEAI_LIBS_PATH=$(EDGEAI_QNX_STAGING)/usr/lib \
	cmake -DCMAKE_MAKE_PROGRAM=make \
	-DCMAKE_LIBRARY_PATH=$(PSDK_QNX_PATH)/stage/aarch64le/ \
	-DCMAKE_TOOLCHAIN_FILE=../../cmake/cross_compile_aarch64.cmake ../../; \
	$(MAKE) install DESTDIR=$(EDGEAI_QNX_STAGING)
endif

edgeai_tiovx_apps: yaml_cpp
ifeq ($(BUILD_LINUX_MPU), yes)
	@echo "------------------------------------"
	@echo "Building EdgeAI TIOVX Apps for Linux"
	@echo "------------------------------------"
	cd $(EDGEAI_TIOVX_APPS_PATH); \
	mkdir build; \
	cd build; \
	CROSS_COMPILER_PATH=$(GCC_LINUX_ARM_ROOT) \
	CROSS_COMPILER_PREFIX=aarch64-none-linux-gnu- \
	TARGET_FS=$(LINUX_FS_PATH) \
	cmake -DCMAKE_TOOLCHAIN_FILE=../cmake/cross_compile_aarch64.cmake ..; \
	$(MAKE) install DESTDIR=$(LINUX_FS_PATH)
endif
ifeq ($(BUILD_QNX_MPU), yes)
	@echo "----------------------------------"
	@echo "Building EdgeAI TIOVX Apps for QNX"
	@echo "----------------------------------"
	cd $(EDGEAI_TIOVX_APPS_PATH); \
	mkdir -p QNX/build; \
	cd QNX/build; \
	CROSS_COMPILER_PATH=$(QNX_HOST)/usr/ \
	CROSS_COMPILER_PREFIX=$(QNX_CROSS_COMPILER_TOOL) \
	TARGET_FS=$(QNX_TARGET) \
	TARGET_OS=QNX \
	PSDK_INCLUDE_PATH=$(LINUX_FS_PATH)/usr/include/ \
	EDGEAI_INCLUDE_PATH=$(EDGEAI_QNX_STAGING)/usr/include/ \
	VISION_APPS_LIBS_PATH=$(VISION_APPS_PATH)/out/$(TARGET_SOC)/$(MPU_CPU)/QNX/$(LINUX_APP_PROFILE)/ \
	EDGEAI_LIBS_PATH=$(EDGEAI_QNX_STAGING)/usr/lib \
	cmake -DCMAKE_MAKE_PROGRAM=make \
	-DCMAKE_LIBRARY_PATH=$(PSDK_QNX_PATH)/stage/aarch64le/ \
	-DCMAKE_TOOLCHAIN_FILE=../../cmake/cross_compile_aarch64.cmake ../../; \
	$(MAKE) install DESTDIR=$(EDGEAI_QNX_STAGING)
endif

yaml_cpp:
ifeq ($(BUILD_QNX_MPU), yes)
	if [ ! -d $(YAML_CPP_PATH) ]; then cd $(PSDK_PATH)/edgeai/; git clone https://github.com/jbeder/yaml-cpp.git; fi
	@echo "----------------------------------"
	@echo "Building YAMP CPP for QNX"
	@echo "----------------------------------"
	cd $(YAML_CPP_PATH); \
	mkdir -p build; \
	cd build; \
	CROSS_COMPILER_PATH=$(QNX_HOST)/usr/ \
	CROSS_COMPILER_PREFIX=$(QNX_CROSS_COMPILER_TOOL) \
	TARGET_FS=$(QNX_TARGET) \
	TARGET_OS=QNX \
	cmake -DCMAKE_MAKE_PROGRAM=make \
	-DCMAKE_INSTALL_PREFIX=/usr/ \
	-DCMAKE_TOOLCHAIN_FILE=$(EDGEAI_TIOVX_APPS_PATH)/cmake/cross_compile_aarch64.cmake ../; \
	$(MAKE) install DESTDIR=$(EDGEAI_QNX_STAGING)
endif

edgeai_install:
	@echo "Install EdgeAI Kernels, Modules and Plugins to EDGEAI_INSTALL_PATH"
	cd $(EDGEAI_UTILS_PATH); \
	if [ -d "build" ]; then $(MAKE) install DESTDIR=$(EDGEAI_INSTALL_PATH) -C build; else echo edgeai-apps-utils has not been built yet, skipping install; fi;
	cd $(EDGEAI_KERNELS_PATH); \
	if [ -d "build" ]; then $(MAKE) install DESTDIR=$(EDGEAI_INSTALL_PATH) -C build; else echo edgeai-tiovx-kernels has not been built yet, skipping install; fi;
	cd $(EDGEAI_TIOVX_APPS_PATH); \
	if [ -d "build" ]; then $(MAKE) install DESTDIR=$(EDGEAI_INSTALL_PATH) -C build; else echo edgeai-tiovx-apps has not been built yet, skipping install; fi;
	sync

edgeai_scrub:
	@echo "EdgeAI Scrub"
	rm -rf $(PSDK_PATH)/edgeai/QNX $(EDGEAI_QNX_STAGING) $(YAML_CPP_PATH)
	cd $(EDGEAI_UTILS_PATH); \
	rm -rf build bin lib QNX
	cd $(EDGEAI_KERNELS_PATH); \
	rm -rf build bin lib QNX
	cd $(EDGEAI_TIOVX_APPS_PATH); \
	rm -rf build bin lib QNX
