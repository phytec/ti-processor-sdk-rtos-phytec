ifeq ($(TARGET_CPU), $(filter $(TARGET_CPU), R5F))

include $(PRELUDE)
TARGET      := ti_imaging_sensordrv
TARGETTYPE  := library
CSOURCES    := $(call all-c-files)
CSOURCES    += imx390/iss_sensor_imx390.c
CSOURCES    += ar0233/iss_sensor_ar0233.c
CSOURCES    += ar0820/iss_sensor_ar0820.c
CSOURCES    += ub9xx_raw_test_pattern/iss_sensor_raw_testpat.c
CSOURCES    += ub9xx_yuv_test_pattern/iss_sensor_testpat.c
CSOURCES    += gw_ar0233_yuv/iss_sensor_gw_ar0233.c

ifeq ($(SOC), $(filter $(SOC), j721e j721s2 j784s4))
CSOURCES    += imx390_discovery/iss_sensor_imx390_discovery_narrow.c
CSOURCES    += imx390_discovery/iss_sensor_imx390_discovery_mid.c
CSOURCES    += imx390_discovery/iss_sensor_imx390_discovery_wide.c
CSOURCES    += imx390_discovery/iss_sensor_imx390_discovery_common.c
CSOURCES    += imx390_discovery/d3_eeprom.c
endif

IDIRS       += $(HOST_ROOT)/sensor_drv/include
IDIRS       += $(APP_UTILS_PATH)/utils/remote_service/include
IDIRS       += $(APP_UTILS_PATH)/utils/ipc/include

ifeq ($(SOC), $(filter $(SOC), j722s))
IDIRS       += $(MCU_PLUS_SDK_PATH)/source

else
IDIRS       += $(PDK_PATH)/packages
endif

ifeq ($(SOC), $(filter $(SOC), am62a))
SKIPBUILD=1
endif

include $(FINALE)

endif
