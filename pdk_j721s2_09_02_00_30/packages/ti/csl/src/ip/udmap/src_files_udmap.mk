
ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e j721s2 j784s4 am62a am62px))
PACKAGE_SRCS_COMMON += cslr_udmap.h csl_udmap.h src/ip/udmap/src_files_udmap.mk src/ip/udmap/V0
SRCDIR += src/ip/udmap/V0/priv
INCDIR += src/ip/udmap/V0
SRCS_COMMON += csl_udmap.c
endif
