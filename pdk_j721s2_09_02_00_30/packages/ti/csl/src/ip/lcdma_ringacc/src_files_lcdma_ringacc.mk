
ifeq ($(SOC),$(filter $(SOC), am64x am62x j721s2 j784s4 am62a am62px))
PACKAGE_SRCS_COMMON += cslr_lcdma_ringacc.h csl_lcdma_ringacc.h src/ip/lcdma_ringacc/V0 src/ip/lcdma_ringacc/src_files_lcdma_ringacc.mk
SRCDIR += src/ip/lcdma_ringacc/V0/priv
INCDIR += src/ip/lcdma_ringacc/V0
SRCS_COMMON += csl_lcdma_ringacc.c
endif
