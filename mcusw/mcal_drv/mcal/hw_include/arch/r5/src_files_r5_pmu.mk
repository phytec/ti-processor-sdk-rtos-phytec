
ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e j784s4 j721s2 j722s am77x tpr12 awr294x am263x am64x am62x am62ax j722s))
  PACKAGE_SRCS_COMMON += hw_include/arch/r5/src/csl_arm_r5_pmu.asm
  PACKAGE_SRCS_COMMON += hw_include/arch/r5/lld_arm_r5_pmu.h
  PACKAGE_SRCS_COMMON += hw_include/arch/r5/src_files_r5_pmu.mk
  ifeq ($(ARCH),armv7r)
    SRCDIR += ./arch/r5/src
    INCDIR += ./arch/r5
    SRCS_ASM_COMMON += csl_arm_r5_pmu.asm
  endif

endif
