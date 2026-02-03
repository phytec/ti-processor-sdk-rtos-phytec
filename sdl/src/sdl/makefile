MODULE_NAME = sdl_api

SRCDIR = . pbist/ lbist/ tog/ pok/ ecc/ r5/ esm/ vtm/ dcc/ mcrc/ rat/ misc/ rti/ rom_checksum/
INCDIR = .

INCDIR += ../.. $(sdl_PATH)/include $(sdl_PATH)/include/soc/$(SOC)
INCDIR += $(sdl_PATH)/src/sdl/r5/$(SOC)

INCLUDE_INTERNAL_INTERFACES = sdl_api

ifeq ($(SOC),$(filter $(SOC), j721e j7200))
PACKAGE_SRCS_COMMON += pbist/soc/$(SOC)
SRCDIR += pbist/soc/$(SOC)
INCDIR += pbist/soc/$(SOC)

PACKAGE_SRCS_COMMON += lbist/soc/$(SOC)
SRCDIR += lbist/soc/$(SOC)
INCDIR += lbist/soc/$(SOC)

PACKAGE_SRCS_COMMON += tog/soc/$(SOC)
SRCDIR += tog/soc/$(SOC)
INCDIR += tog/soc/$(SOC)

PACKAGE_SRCS_COMMON += rti
SRCDIR += rti/soc/$(SOC)
INCDIR += rti/soc/$(SOC)

PACKAGE_SRCS_COMMON += dcc/soc/$(SOC)
SRCDIR += dcc/soc/$(SOC)
INCDIR += dcc/soc/$(SOC)

PACKAGE_SRCS_COMMON += mcrc
SRCDIR += mcrc
INCDIR += mcrc

PACKAGE_SRCS_COMMON += esm
SRCDIR += esm/soc/$(SOC)
INCDIR += esm/soc/$(SOC)

PACKAGE_SRCS_COMMON +=rat
SRCDIR += rat
INCDIR += rat

PACKAGE_SRCS_COMMON += vtm/soc/$(SOC)
SRCDIR += vtm/soc/$(SOC)
INCDIR += vtm/soc/$(SOC)

PACKAGE_SRCS_COMMON += ecc/soc/$(SOC)
INCDIR += ecc/soc/$(SOC)

PACKAGE_SRCS_COMMON += misc/soc/$(SOC)
SRCDIR += misc/soc/$(SOC)
INCDIR += misc/soc/$(SOC)

PACKAGE_SRCS_COMMON +=rom_checksum
SRCDIR += rom_checksum
INCDIR += rom_checksum

endif

ifeq ($(SOC),$(filter $(SOC), j721e))

PACKAGE_SRCS_COMMON +=pok
SRCDIR += pok pok/v0
INCDIR += pok pok/v0

endif

ifeq ($(SOC),$(filter $(SOC), j7200))

PACKAGE_SRCS_COMMON +=pok
SRCDIR += pok pok/v1
INCDIR += pok pok/v1

endif

ifeq ($(SOC),$(filter $(SOC), j721s2))

PACKAGE_SRCS_COMMON += esm
SRCDIR += esm/soc/$(SOC)
INCDIR += esm/soc/$(SOC)

PACKAGE_SRCS_COMMON += dcc/soc/$(SOC)
SRCDIR += dcc/soc/$(SOC)
INCDIR += dcc/soc/$(SOC)

PACKAGE_SRCS_COMMON += vtm/soc/$(SOC)
SRCDIR += vtm/soc/$(SOC)
INCDIR += vtm/soc/$(SOC)

PACKAGE_SRCS_COMMON += rti
SRCDIR += rti/soc/$(SOC)
INCDIR += rti/soc/$(SOC)

PACKAGE_SRCS_COMMON +=rat
SRCDIR += rat
INCDIR += rat

PACKAGE_SRCS_COMMON += tog/soc/$(SOC)
SRCDIR += tog/soc/$(SOC)
INCDIR += tog/soc/$(SOC)

PACKAGE_SRCS_COMMON += mcrc
SRCDIR += mcrc
INCDIR += mcrc

PACKAGE_SRCS_COMMON += ecc/soc/$(SOC)
INCDIR += ecc/soc/$(SOC)

PACKAGE_SRCS_COMMON +=pok
SRCDIR += pok pok/v1
INCDIR += pok pok/v1

PACKAGE_SRCS_COMMON += misc/soc/$(SOC)
SRCDIR += misc/soc/$(SOC)
INCDIR += misc/soc/$(SOC)

PACKAGE_SRCS_COMMON += lbist/soc/$(SOC)
SRCDIR += lbist/soc/$(SOC)
INCDIR += lbist/soc/$(SOC)

PACKAGE_SRCS_COMMON += pbist/soc/$(SOC)
SRCDIR += pbist/soc/$(SOC)
INCDIR += pbist/soc/$(SOC)

PACKAGE_SRCS_COMMON +=rom_checksum
SRCDIR += rom_checksum
INCDIR += rom_checksum

endif

ifeq ($(SOC),$(filter $(SOC), j784s4))

PACKAGE_SRCS_COMMON += esm
SRCDIR += esm/soc/$(SOC)
INCDIR += esm/soc/$(SOC)

PACKAGE_SRCS_COMMON += rti
SRCDIR += rti/soc/$(SOC)
INCDIR += rti/soc/$(SOC)

PACKAGE_SRCS_COMMON += mcrc
SRCDIR += mcrc
INCDIR += mcrc

PACKAGE_SRCS_COMMON +=rat
SRCDIR += rat
INCDIR += rat

PACKAGE_SRCS_COMMON += ecc/soc/$(SOC)
INCDIR += ecc/soc/$(SOC)

PACKAGE_SRCS_COMMON += dcc/soc/$(SOC)
SRCDIR += dcc/soc/$(SOC)
INCDIR += dcc/soc/$(SOC)

PACKAGE_SRCS_COMMON += vtm/soc/$(SOC)
SRCDIR += vtm/soc/$(SOC)
INCDIR += vtm/soc/$(SOC)

PACKAGE_SRCS_COMMON += tog/soc/$(SOC)
SRCDIR += tog/soc/$(SOC)
INCDIR += tog/soc/$(SOC)

PACKAGE_SRCS_COMMON += pbist/soc/$(SOC)
SRCDIR += pbist/soc/$(SOC)
INCDIR += pbist/soc/$(SOC)

PACKAGE_SRCS_COMMON += lbist/soc/$(SOC)
SRCDIR += lbist/soc/$(SOC)
INCDIR += lbist/soc/$(SOC)

PACKAGE_SRCS_COMMON +=pok
SRCDIR += pok pok/v1
INCDIR += pok pok/v1

PACKAGE_SRCS_COMMON +=rom_checksum
SRCDIR += rom_checksum
INCDIR += rom_checksum

PACKAGE_SRCS_COMMON += misc/soc/$(SOC)
SRCDIR += misc/soc/$(SOC)
INCDIR += misc/soc/$(SOC)
endif

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = sdl_osal

ifeq ($(SOC),$(filter $(SOC), j784s4))

SRCS_COMMON += sdl_esm.c sdl_esm_priv.c sdl_esm_core.c
PACKAGE_SRCS_COMMON += sdl_esm.h

SRCS_COMMON +=  sdl_rti.c
PACKAGE_SRCS_COMMON += sdl_rti.h

SRCS_COMMON +=  sdl_vtm.c
PACKAGE_SRCS_COMMON += sdl_vtm.h

SRCS_COMMON +=  sdl_pbist.c sdl_soc_pbist.c
PACKAGE_SRCS_COMMON += sdl_pbist.h sdl_pbist_priv.h

SRCS_COMMON +=  sdl_tog.c
PACKAGE_SRCS_COMMON += sdl_tog.h

SRCS_COMMON +=  sdl_rat.c
PACKAGE_SRCS_COMMON += sdl_rat.h

SRCS_COMMON +=  sdl_mcrc.c
PACKAGE_SRCS_COMMON += sdl_mcrc.h

SRCS_COMMON += sdl_ecc_r5.c sdl_exception.c sdl_mpu.c sdl_mpuCfg.c
SRCS_ASM_COMMON += sdl_r5_utils.asm

SRCS_COMMON += sdl_ecc.c

SRCS_ASM_COMMON += sdl_ecc_utils.asm
PACKAGE_SRCS_COMMON += sdl_ecc_core.h sdl_ecc_utils.h

SRCS_COMMON +=  sdl_dcc.c
PACKAGE_SRCS_COMMON += sdl_dcc.h

SRCS_COMMON +=  sdl_mtog.c
PACKAGE_SRCS_COMMON += sdl_mtog.h

SRCS_COMMON += sdl_lbist.c sdl_soc_lbist.c
PACKAGE_SRCS_COMMON += sdl_lbist.h sdl_lbist_priv.h

SRCS_COMMON += sdl_pok.c
PACKAGE_SRCS_COMMON += sdl_pok.c

SRCS_COMMON += sdl_ccm.c
PACKAGE_SRCS_COMMON += sdl_ccm.h

SRCS_COMMON += sdl_msmc.c
PACKAGE_SRCS_COMMON += sdl_msmc.h

SRCS_COMMON +=  sdl_rom_checksum.c
PACKAGE_SRCS_COMMON += sdl_rom_checksum.h
endif

ifeq ($(SOC),$(filter $(SOC), j721s2))

SRCS_COMMON += sdl_esm.c sdl_esm_priv.c sdl_esm_core.c
PACKAGE_SRCS_COMMON += sdl_esm.h

SRCS_COMMON +=  sdl_dcc.c
PACKAGE_SRCS_COMMON += sdl_dcc.h

SRCS_COMMON += sdl_ecc_r5.c sdl_exception.c sdl_mpu.c sdl_mpuCfg.c

SRCS_ASM_COMMON += sdl_r5_utils.asm

SRCS_COMMON +=  sdl_vtm.c
PACKAGE_SRCS_COMMON += sdl_vtm.h

SRCS_COMMON +=  sdl_rat.c
PACKAGE_SRCS_COMMON += sdl_rat.h

SRCS_COMMON +=  sdl_rti.c
PACKAGE_SRCS_COMMON += sdl_rti.h

SRCS_COMMON +=  sdl_mtog.c
PACKAGE_SRCS_COMMON += sdl_mtog.h

SRCS_COMMON +=  sdl_tog.c
PACKAGE_SRCS_COMMON += sdl_tog.h

SRCS_COMMON += sdl_ccm.c
PACKAGE_SRCS_COMMON += sdl_ccm.h

SRCS_COMMON +=  sdl_mcrc.c
PACKAGE_SRCS_COMMON += sdl_mcrc.h

SRCS_COMMON += sdl_ecc.c
SRCS_ASM_COMMON += sdl_ecc_utils.asm
PACKAGE_SRCS_COMMON += sdl_ecc_core.h sdl_ecc_utils.h

SRCS_COMMON += sdl_pok.c
PACKAGE_SRCS_COMMON += sdl_pok.c

SRCS_COMMON += sdl_lbist.c sdl_soc_lbist.c
PACKAGE_SRCS_COMMON += sdl_lbist.h sdl_lbist_priv.h

SRCS_COMMON += sdl_msmc.c
PACKAGE_SRCS_COMMON += sdl_msmc.h

SRCS_COMMON +=  sdl_pbist.c sdl_soc_pbist.c
PACKAGE_SRCS_COMMON += sdl_pbist.h sdl_pbist_priv.h

SRCS_COMMON +=  sdl_rom_checksum.c
PACKAGE_SRCS_COMMON += sdl_rom_checksum.h

endif

ifeq ($(SOC),$(filter $(SOC), j721e j7200))

SRCS_COMMON += sdl_ecc_r5.c sdl_exception.c sdl_mpu.c sdl_mpuCfg.c
SRCS_ASM_COMMON += sdl_r5_utils.asm

SRCS_COMMON += sdl_esm.c sdl_esm_priv.c sdl_esm_core.c
PACKAGE_SRCS_COMMON += sdl_esm.h

SRCS_COMMON += sdl_ecc.c
SRCS_ASM_COMMON += sdl_ecc_utils.asm
PACKAGE_SRCS_COMMON += sdl_ecc_core.h sdl_ecc_utils.h

SRCS_COMMON +=  sdl_pbist.c sdl_soc_pbist.c
PACKAGE_SRCS_COMMON += sdl_pbist.h sdl_pbist_priv.h

SRCS_COMMON += sdl_lbist.c sdl_soc_lbist.c
PACKAGE_SRCS_COMMON += sdl_lbist.h sdl_lbist_priv.h

SRCS_COMMON +=  sdl_tog.c
PACKAGE_SRCS_COMMON += sdl_tog.h

SRCS_COMMON +=  sdl_vtm.c
PACKAGE_SRCS_COMMON += sdl_vtm.h

SRCS_COMMON +=  sdl_rti.c
PACKAGE_SRCS_COMMON += sdl_rti.h

SRCS_COMMON +=  sdl_dcc.c
PACKAGE_SRCS_COMMON += sdl_dcc.h

SRCS_COMMON +=  sdl_rat.c
PACKAGE_SRCS_COMMON += sdl_rat.h

SRCS_COMMON +=  sdl_mcrc.c
PACKAGE_SRCS_COMMON += sdl_mcrc.h

SRCS_COMMON += sdl_pok.c
PACKAGE_SRCS_COMMON += sdl_pok.c

SRCS_COMMON += sdl_ccm.c
PACKAGE_SRCS_COMMON += sdl_ccm.h

SRCS_COMMON += sdl_msmc.c
PACKAGE_SRCS_COMMON += sdl_msmc.h

SRCS_COMMON +=  sdl_rom_checksum.c
PACKAGE_SRCS_COMMON += sdl_rom_checksum.h

endif

ifeq ($(SOC),$(filter $(SOC), j7200))

SRCS_COMMON +=  sdl_mtog.c
PACKAGE_SRCS_COMMON += sdl_mtog.h

endif

PACKAGE_SRCS_COMMON += hw_types.h sdl_types.h sdlr.h tistdtypes.h

MODULE_SRC_BASE_PATH = $(sdl_PATH)/src/sdl

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk
