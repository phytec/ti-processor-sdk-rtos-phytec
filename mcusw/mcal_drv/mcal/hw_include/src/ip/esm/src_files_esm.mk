
ifeq ($(SOC),$(filter $(SOC), tda3xx dra78x tpr12 awr294x))
PACKAGE_SRCS_COMMON += lldr_esm.h lld_esm.h hw_include/src/ip/esm/src_files_esm.mk hw_include/src/ip/esm/V0
SRCDIR += hw_include/src/ip/esm/V0/priv
INCDIR += hw_include/src/ip/esm/V0
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
  INCDIR += hw_include/src/ip/esm/V0/V0_1
endif
SRCS_COMMON += esm.c
endif

ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e))
PACKAGE_SRCS_COMMON += lldr_esm.h lld_esm.h hw_include/src/ip/esm/src_files_esm.mk hw_include/src/ip/esm/V1
SRCDIR += hw_include/src/ip/esm/V1/priv
INCDIR += hw_include/src/ip/esm/V1/V1_0
SRCS_COMMON += lld_esm.c
endif

ifeq ($(SOC),$(filter $(SOC), am64x j721s2 am62ax am62x am62px j722s))
PACKAGE_SRCS_COMMON += lldr_esm.h lld_esm.h hw_include/src/ip/esm/src_files_esm.mk hw_include/src/ip/esm/V1
SRCDIR += hw_include/src/ip/esm/V1/priv
INCDIR += hw_include/src/ip/esm/V1/V1_1
SRCS_COMMON += lld_esm.c
endif
