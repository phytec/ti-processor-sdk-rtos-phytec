
ifeq ($(SOC),$(filter $(SOC), am65xx j7200 j721e j784s4 j721s2 j722s am77x tpr12 awr294x am263x am64x am62x am62ax))
  PACKAGE_SRCS_COMMON += hw_include/arch/r5
  ifeq ($(ARCH),armv7r)
    SRCDIR += ./arch/r5/src
    INCDIR += ./arch/r5
    SRCS_ASM_COMMON += lld_arm_r5.asm
    SRCS_COMMON += interrupt.c interrupt_handlers.c interrupt_register.c lld_cache.c lld_r5.c
  endif

endif
