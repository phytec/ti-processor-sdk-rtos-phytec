ifeq ($(SOC),$(filter $(SOC), j721e j721s2 j784s4))
  PACKAGE_SRCS_COMMON += arch/c7x
  ifeq ($(ARCH),c71)
    SRCDIR += ./arch/c7x/src
    INCDIR += ./arch/c7x
    SRCS_ASM_COMMON += Cache_asm.asm Clobber_asm.asm Exception_asm.asm Hwi_asm.asm
    SRCS_ASM_COMMON += Hwi_asm_switch.asm Hwi_disp_always.asm Mmu_asm.asm
    SRCS_COMMON += boot.c c7x_module_config.c Cache.c Exception.c Hwi.c IntrinsicsSupport.c Mmu.c
    SRCS_COMMON += Mmu_table.c Startup.c
    CFLAGS_LOCAL_COMMON += -DHwi_bootToNonSecure__D=true
    CFLAGS_LOCAL_COMMON += -DException_vectors__D
  endif
endif