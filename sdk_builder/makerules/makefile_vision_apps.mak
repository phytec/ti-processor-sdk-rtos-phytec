#
# Utility makefile to build vision_apps libaries and related components
#
# Edit this file to suit your specific build needs
#

ifeq ($(BUILD_MCU_BOARD_DEPENDENCIES),yes)
	MCU_SYSCFG_FILE=example.syscfg
else
	MCU_SYSCFG_FILE=example_no_board_deps.syscfg
endif

vision_apps:
ifeq ($(SOC), j722s)
ifeq ($(BUILD_CPU_MCU2_0),yes)
	@echo Generating SysConfig files for mcu2_0 vision_apps
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(MCU_PLUS_SDK_PATH)/.metadata/product.json --context main-r5fss0-0 --part Default --package AMW --output $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/mcu2_0/generated $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/mcu2_0/$(MCU_SYSCFG_FILE)
endif
ifeq ($(BUILD_CPU_MCU1_0),yes)
	@echo Generating SysConfig files for mcu1_0 vision_apps
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(MCU_PLUS_SDK_PATH)/.metadata/product.json --context wkup-r5fss0-0 --part Default --package AMW --output $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/mcu1_0/generated $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/mcu1_0/example.syscfg
endif
ifeq ($(BUILD_CPU_C7x_1),yes)
	@echo Generating SysConfig files for c7x_1 vision_apps
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(MCU_PLUS_SDK_PATH)/.metadata/product.json --context c75ss0-0 --part Default --package AMW --output $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/c7x_1/generated $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/c7x_1/example.syscfg
endif
ifeq ($(BUILD_CPU_C7x_2),yes)
	@echo Generating SysConfig files for c7x_2 vision_apps
	$(SYSCFG_NODE) $(SYSCFG_CLI_PATH)/dist/cli.js --product $(MCU_PLUS_SDK_PATH)/.metadata/product.json --context c75ss1-0 --part Default --package AMW --output $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/c7x_2/generated $(PSDK_PATH)/vision_apps/platform/$(SOC)/rtos/c7x_2/example.syscfg
endif
endif
	$(MAKE) -C $(VISION_APPS_PATH)

vision_apps_clean:
	$(MAKE) -C $(VISION_APPS_PATH) clean

vision_apps_scrub:
ifeq ($(SOC), j722s)
ifeq ($(BUILD_CPU_MCU1_0),yes)
	rm -rf $(VISION_APPS_PATH)/platform/$(SOC)/rtos/mcu1_0/generated
endif
ifeq ($(BUILD_CPU_MCU2_0),yes)
	rm -rf $(VISION_APPS_PATH)/platform/$(SOC)/rtos/mcu2_0/generated
endif
ifeq ($(BUILD_CPU_C7x_1),yes)
	rm -rf $(VISION_APPS_PATH)/platform/$(SOC)/rtos/c7x_1/generated
endif
ifeq ($(BUILD_CPU_C7x_2),yes)
	rm -rf $(VISION_APPS_PATH)/platform/$(SOC)/rtos/c7x_2/generated
endif
endif
	$(MAKE) -C $(VISION_APPS_PATH) scrub

vision_apps_docs:
	$(MAKE) -C $(VISION_APPS_PATH) doxy_docs

.PHONY: vision_apps vision_apps_clean vision_apps_scrub vision_apps_docs
