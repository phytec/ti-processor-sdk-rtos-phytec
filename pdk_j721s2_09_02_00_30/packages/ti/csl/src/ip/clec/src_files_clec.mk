
ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e j721s2))
PACKAGE_SRCS_COMMON += cslr_clec.h csl_clec.h src/ip/clec/src_files_clec.mk src/ip/clec/V0 arch/c7x/cslr_C7X_CPU.h
SRCDIR += src/ip/clec/V0/priv
INCDIR += src/ip/clec/V0
SRCS_COMMON += csl_clec.c
endif

ifeq ($(SOC),$(filter $(SOC), j784s4))
PACKAGE_SRCS_COMMON += cslr_clec.h csl_clec.h src/ip/clec/src_files_clec.mk src/ip/clec/V1 arch/c7x/cslr_C7X_CPU.h
SRCDIR += src/ip/clec/V1/priv
INCDIR += src/ip/clec/V1
SRCS_COMMON += csl_clec.c
endif

ifeq ($(SOC),$(filter $(SOC), am62a))
PACKAGE_SRCS_COMMON += cslr_clec.h csl_clec.h src/ip/clec/src_files_clec.mk src/ip/clec/V2
SRCDIR += src/ip/clec/V2/priv
INCDIR += src/ip/clec/V2
SRCS_COMMON += csl_clec.c
endif
