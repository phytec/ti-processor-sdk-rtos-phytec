SRCS_COMMON += dss_soc.c dss_soc_graph.c

ifeq ($(SOC),$(filter $(SOC), j721e j721s2 j784s4))
SRCS_COMMON += dss_soc_fw.c
endif