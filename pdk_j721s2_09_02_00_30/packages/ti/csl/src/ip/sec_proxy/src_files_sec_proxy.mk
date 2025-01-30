
ifeq ($(SOC),$(filter $(SOC), am65xx j721e j721s2 j784s4 j7200 am64x am62x am62a am62px))
PACKAGE_SRCS_COMMON += cslr_sec_proxy.h csl_sec_proxy.h src/ip/sec_proxy/src_files_sec_proxy.mk src/ip/sec_proxy/V0
SRCDIR += src/ip/sec_proxy/V0/priv
SRCS_COMMON += csl_sec_proxy.c
INCDIR += src/ip/sec_proxy/V0
endif

ifeq ($(SOC),$(filter $(SOC), am65xx j721e))
PACKAGE_SRCS_COMMON += src/ip/sec_proxy/V0/V0_0
INCDIR += src/ip/sec_proxy/V0/V0_0
endif

ifeq ($(SOC),$(filter $(SOC), j7200 am64x am62x am62a am62px j721s2 j784s4))
PACKAGE_SRCS_COMMON += src/ip/sec_proxy/V0/V0_1
INCDIR += src/ip/sec_proxy/V0/V0_1
endif
