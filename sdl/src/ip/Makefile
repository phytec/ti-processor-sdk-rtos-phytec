MODULE_NAME = sdl_ip

SRCDIR = .
INCDIR = .

INCDIR += $(sdl_PATH) $(sdl_PATH)/src/sdl
INCDIR += ../.. $(sdl_PATH)/include $(sdl_PATH)/include/soc/$(SOC)

INCLUDE_INTERNAL_INTERFACES = sdl_ip

ifeq ($(SOC),$(filter $(SOC), j721e j7200))
PACKAGE_SRCS_COMMON += sdlr_pbist.h pbist/V0
SRCDIR += pbist/V0
INCDIR += pbist/V0

PACKAGE_SRCS_COMMON += sdlr_lbist.h lbist/V0
SRCDIR += lbist/V0
INCDIR += lbist/V0

PACKAGE_SRCS_COMMON += tog
SRCDIR += tog/V0
INCDIR += tog/V0

PACKAGE_SRCS_COMMON += rti/V0
SRCDIR += rti/V0
INCDIR += rti/V0

PACKAGE_SRCS_COMMON += esm
SRCDIR += esm/V0
INCDIR += esm/V0 esm/V0_0

PACKAGE_SRCS_COMMON += vtm/V0
SRCDIR += vtm/V0
INCDIR += vtm/V0 $(sdl_PATH)/include/soc/$(SOC)

PACKAGE_SRCS_COMMON += mcrc
SRCDIR += mcrc
INCDIR += mcrc

PACKAGE_SRCS_COMMON += rat/V0
SRCDIR += rat/V0
INCDIR += rat/V0

PACKAGE_SRCS_COMMON += sdlr_ecc.h ecc/V1
SRCDIR += ecc/V1
INCDIR += ecc/V1

PACKAGE_SRCS_COMMON += pok
SRCDIR += pok pok/soc/$(SOC)
INCDIR += pok pok/soc/$(SOC)

PACKAGE_SRCS_COMMON += rom_checksum
SRCDIR += rom_checksum
INCDIR += rom_checksum

endif

ifeq ($(SOC),$(filter $(SOC), j7200 j721s2 j784s4))

SRCDIR += pok/V1
INCDIR += pok/V1

endif

ifeq ($(SOC),$(filter $(SOC), j721s2))

PACKAGE_SRCS_COMMON += mcrc
SRCDIR += mcrc
INCDIR += mcrc

PACKAGE_SRCS_COMMON += esm
SRCDIR += esm/V0
INCDIR += esm/V0 esm/V0_0

PACKAGE_SRCS_COMMON += vtm/V0
SRCDIR += vtm/V0
INCDIR += vtm/V0 $(sdl_PATH)/include/soc/$(SOC)

PACKAGE_SRCS_COMMON += rti/V0
SRCDIR += rti/V0
INCDIR += rti/V0

PACKAGE_SRCS_COMMON += tog
SRCDIR += tog/V0
INCDIR += tog/V0

PACKAGE_SRCS_COMMON += rat/V0
SRCDIR += rat/V0
INCDIR += rat/V0

PACKAGE_SRCS_COMMON += sdlr_ecc.h ecc/V1
SRCDIR += ecc/V1
INCDIR += ecc/V1

PACKAGE_SRCS_COMMON += pok
SRCDIR += pok pok/soc/$(SOC)
INCDIR += pok pok/soc/$(SOC)

PACKAGE_SRCS_COMMON += sdlr_lbist.h lbist/V0
SRCDIR += lbist/V0
INCDIR += lbist/V0

PACKAGE_SRCS_COMMON += sdlr_pbist.h pbist/V0
SRCDIR += pbist/V0
INCDIR += pbist/V0

PACKAGE_SRCS_COMMON += rom_checksum
SRCDIR += rom_checksum
INCDIR += rom_checksum

endif

ifeq ($(SOC),$(filter $(SOC), j784s4))

PACKAGE_SRCS_COMMON += esm
SRCDIR += esm/V0
INCDIR += esm/V0 esm/V0_0
PACKAGE_SRCS_COMMON += rti/V0
SRCDIR += rti/V0
INCDIR += rti/V0

PACKAGE_SRCS_COMMON += tog
SRCDIR += tog/V0
INCDIR += tog/V0 

PACKAGE_SRCS_COMMON += sdlr_pbist.h pbist/V0
SRCDIR += pbist/V0
INCDIR += pbist/V0

PACKAGE_SRCS_COMMON += sdlr_lbist.h lbist/V0
SRCDIR += lbist/V0
INCDIR += lbist/V0

PACKAGE_SRCS_COMMON += rat/V0
SRCDIR += rat/V0
INCDIR += rat/V0

PACKAGE_SRCS_COMMON += mcrc
SRCDIR += mcrc
INCDIR += mcrc

PACKAGE_SRCS_COMMON += pok
SRCDIR += pok pok/soc/$(SOC)
INCDIR += pok pok/soc/$(SOC)

PACKAGE_SRCS_COMMON += sdlr_ecc.h ecc/V1
SRCDIR += ecc/V1
INCDIR += ecc/V1

PACKAGE_SRCS_COMMON += vtm/V0
SRCDIR += vtm/V0
INCDIR += vtm/V0 $(sdl_PATH)/include/soc/$(SOC)


PACKAGE_SRCS_COMMON += rom_checksum
SRCDIR += rom_checksum
INCDIR += rom_checksum

endif

# List all the external components/interfaces, whose interface header files
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = 
ifeq ($(SOC),$(filter $(SOC), j784s4))
SRCS_COMMON += sdl_ip_esm.c
PACKAGE_SRCS_COMMON += sdl_ip_esm.h

SRCS_COMMON += sdl_ip_rti.c
PACKAGE_SRCS_COMMON +=  sdl_ip_rti.h

SRCS_COMMON += sdl_ip_mcrc.c
PACKAGE_SRCS_COMMON += sdl_ip_mcrc.h

SRCS_COMMON += sdl_ip_rat.c
PACKAGE_SRCS_COMMON +=  sdl_ip_rat.h

SRCS_COMMON += sdl_ip_ecc.c
PACKAGE_SRCS_COMMON += sdl_ip_ecc.h

SRCS_COMMON += sdl_ip_vtm.c sdl_vtm_pvt_sensor.c
PACKAGE_SRCS_COMMON += sdl_ip_vtm.h

SRCS_COMMON += sdl_ip_mtog.c
PACKAGE_SRCS_COMMON += sdl_ip_mtog.h

SRCS_COMMON += sdl_ip_pbist.c
PACKAGE_SRCS_COMMON += sdl_ip_pbist.h

SRCS_COMMON += sdl_ip_lbist.c
PACKAGE_SRCS_COMMON += sdl_ip_lbist.h

SRCS_COMMON += sdl_ip_pok_defs.c

SRCS_COMMON += sdl_ip_pok.c
PACKAGE_SRCS_COMMON += sdl_ip_pok.h sdl_pok_def.h

SRCS_COMMON += sdl_ip_tog.c
PACKAGE_SRCS_COMMON += sdl_ip_tog.h

SRCS_COMMON += sdl_ip_rom_checksum.c
PACKAGE_SRCS_COMMON += sdl_ip_rom_checksum.h

endif

ifeq ($(SOC),$(filter $(SOC), j721s2))

SRCS_COMMON += sdl_ip_mcrc.c
PACKAGE_SRCS_COMMON += sdl_ip_mcrc.h

SRCS_COMMON += sdl_ip_esm.c
PACKAGE_SRCS_COMMON += sdl_ip_esm.h

SRCS_COMMON += sdl_ip_vtm.c sdl_vtm_pvt_sensor.c
PACKAGE_SRCS_COMMON += sdl_ip_vtm.h

SRCS_COMMON += sdl_ip_rti.c
PACKAGE_SRCS_COMMON +=  sdl_ip_rti.h

SRCS_COMMON += sdl_ip_rat.c
PACKAGE_SRCS_COMMON +=  sdl_ip_rat.h

SRCS_COMMON += sdl_ip_pbist.c
PACKAGE_SRCS_COMMON += sdl_ip_pbist.h

SRCS_COMMON += sdl_ip_mtog.c
PACKAGE_SRCS_COMMON += sdl_ip_mtog.h

SRCS_COMMON += sdl_ip_ecc.c
PACKAGE_SRCS_COMMON += sdl_ip_ecc.h

SRCS_COMMON += sdl_ip_tog.c
PACKAGE_SRCS_COMMON += sdl_ip_tog.h

SRCS_COMMON += sdl_ip_pok_defs.c

SRCS_COMMON += sdl_ip_pok.c
PACKAGE_SRCS_COMMON += sdl_ip_pok.h sdl_pok_def.h

SRCS_COMMON += sdl_ip_lbist.c
PACKAGE_SRCS_COMMON += sdl_ip_lbist.h

SRCS_COMMON += sdl_ip_rom_checksum.c
PACKAGE_SRCS_COMMON += sdl_ip_rom_checksum.h

endif

ifeq ($(SOC),$(filter $(SOC), j721e j7200))

SRCS_COMMON += sdl_ip_esm.c
PACKAGE_SRCS_COMMON += sdl_ip_esm.h

SRCS_COMMON += sdl_ip_pbist.c
PACKAGE_SRCS_COMMON += sdl_ip_pbist.h

SRCS_COMMON += sdl_ip_lbist.c
PACKAGE_SRCS_COMMON += sdl_ip_lbist.h

SRCS_COMMON += sdl_ip_tog.c
PACKAGE_SRCS_COMMON += sdl_ip_tog.h

SRCS_COMMON += sdl_ip_mcrc.c
PACKAGE_SRCS_COMMON += sdl_ip_mcrc.h

SRCS_COMMON += sdl_ip_ecc.c
PACKAGE_SRCS_COMMON += sdl_ip_ecc.h

SRCS_COMMON += sdl_ip_rti.c
PACKAGE_SRCS_COMMON +=  sdl_ip_rti.h

SRCS_COMMON += sdl_ip_rat.c
PACKAGE_SRCS_COMMON +=  sdl_ip_rat.h

SRCS_COMMON += sdl_ip_vtm.c sdl_vtm_pvt_sensor.c
PACKAGE_SRCS_COMMON += sdl_ip_vtm.h

SRCS_COMMON += sdl_ip_pok.c
PACKAGE_SRCS_COMMON += sdl_ip_pok.h sdl_pok_def.h

SRCS_COMMON += sdl_ip_rom_checksum.c
PACKAGE_SRCS_COMMON += sdl_ip_rom_checksum.h
endif

ifeq ($(SOC),$(filter $(SOC), j7200))

SRCS_COMMON += sdl_ip_pok_defs.c

SRCS_COMMON += sdl_ip_mtog.c
PACKAGE_SRCS_COMMON += sdl_ip_mtog.h

endif

MODULE_SRC_BASE_PATH = $(sdl_PATH)/src/ip

# Include common make files
ifeq ($(MAKERULEDIR), )
#Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/common.mk
