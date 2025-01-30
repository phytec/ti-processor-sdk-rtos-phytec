#*******************************************************************************
#                                                                              *
# Copyright (c) 2021 Texas Instruments Incorporated - http://www.ti.com/       *
#                        ALL RIGHTS RESERVED                                   *
#                                                                              *
#*******************************************************************************

# Filename: common.mk
#
# Common make file - This file has common rules and definitions that are common
#                    across platforms/cores/ISAs/SoCs
#
# This file needs to change when:
#     1. common rule/define has to be added or modified
#

#
# Include make paths and options for all supported targets/platforms
#

ifeq ($(MAKERULEDIR), )
# Makerule path not defined, define this and assume relative path from ROOTDIR
  MAKERULEDIR := $(ROOTDIR)/build/makerules
  export MAKERULEDIR
endif
include $(MAKERULEDIR)/build_config.mk
include $(MAKERULEDIR)/platform.mk
include $(MAKERULEDIR)/env.mk


.PHONY : all clean gendirs r5f clean_appimagerprc sbl_appimagerprc

all : $(CORE)

# Define directories that are going to be created as a part of build process
ifdef MODULE_NAME
  ifeq ($($(MODULE_NAME)_PLATFORM_DEPENDENCY),yes)
    ifeq ($($(MODULE_NAME)_CORE_DEPENDENCY),yes)
      DEPENDENCY_SUB_PATH = $(SOC)/$(CORE)
    else
      DEPENDENCY_SUB_PATH = $(SOC)/$(ISA)
    endif
  else
    ifeq ($($(MODULE_NAME)_CORE_DEPENDENCY),yes)
      DEPENDENCY_SUB_PATH = $(CORE)
    else
      DEPENDENCY_SUB_PATH = $(ISA)
    endif
  endif
endif

ifeq ($(DEST_ROOT),)
 ifdef MODULE_NAME
  OBJDIR = obj/$(DEPENDENCY_SUB_PATH)/$(PROFILE_$(CORE))
  LIBDIR = lib/$(DEPENDENCY_SUB_PATH)/$(PROFILE_$(CORE))
 else
  OBJDIR = obj/$(SOC)/$(CORE)/$(PROFILE_$(CORE))
  BINDIR = bin/$(SOC)
 endif
  PACKAGEDIR = package/$(DEPENDENCY_SUB_PATH)/$(PACKAGE_SELECT)
else
  ifdef MODULE_NAME
    OBJDIR = $(DEST_ROOT)/$($(MODULE_NAME)_RELPATH)/obj/$(DEPENDENCY_SUB_PATH)/$(PROFILE_$(CORE))
    LIBDIR = $(DEST_ROOT)/$($(MODULE_NAME)_RELPATH)/lib/$(DEPENDENCY_SUB_PATH)/$(PROFILE_$(CORE))
  else
     OBJDIR = $(DEST_ROOT)/$(APP_NAME)/obj/$(SOC)/$(CORE)/$(BUILD_OS_TYPE)/$(PROFILE_$(CORE))
     BINDIR = $(DEST_ROOT)/$(APP_NAME)/bin/$(SOC)
  endif
  PACKAGEDIR = $(DEST_ROOT/package/$(PACKAGE_SELECT)
endif
DEPDIR = $(OBJDIR)/.deps
DEPFILE = $(DEPDIR)/$(*F)

# Create directories
$(OBJDIR) :
	$(MKDIR) -p $(OBJDIR)

$(DEPDIR) :
	$(MKDIR) -p $(DEPDIR)

$(LIBDIR) :
	$(MKDIR) -p $(LIBDIR)

$(BINDIR) :
	$(MKDIR) -p $(BINDIR)

$(PACKAGEDIR) :
	$(MKDIR) -p $(PACKAGEDIR)

#
# Common variables
#

# Assemble list of source file names
SRCS = $(SRCS_COMMON) $(SRCS_$(CORE)) $(SRCS_$(SOCFAMILY))
SRCS_ASM = $(SRCS_ASM_COMMON) $(SRCS_ASM_$(CORE)) $(SRCS_ASM_$(SOCFAMILY))

ifneq ($(SOCFAMILY),$(SOC))
SRCS += $(SRCS_$(SOC))
SRCS_ASM += $(SRCS_ASM_$(SOC))
endif #ifneq ($(SOCFAMILY,$(SOC))

ifneq ($(SOC),$(SOCFAMILY))
SRCS += $(SRCS_$(SOC))
SRCS_ASM += $(SRCS_ASM_$(SOC))
endif #ifneq ($(SOC),$(SOCFAMILY))

PACKAGE_SRCS = $(PACKAGE_SRCS_COMMON)

# Define search paths
VPATH = $(SRCDIR)

# Following 'if...' block is for an application.
ifndef MODULE_NAME
  # Derive list of all packages from each of the components needed by the app
  PKG_LIST = $(foreach COMP,$(COMP_LIST_COMMON),$($(COMP)_PKG_LIST))
  PKG_LIST += $(foreach COMP,$(COMP_LIST_$(CORE)),$($(COMP)_PKG_LIST))
  # Derive list of all external libraries
  EXT_LIB_LIST = $(EXT_LIB_LIST_COMMON)

  # For each of the packages (or modules), get a list of source files that are
  # marked to be compiled in app stage of the build (or in the context in the app)
  SRCS_APPSTG_FILES = $(foreach PKG, $(PKG_LIST), $($(PKG)_APP_STAGE_FILES))
  # The app has to compile package cfg source files in its context. The name
  # of the file assumed is <MOD>_cfg.c under the top-level directory - i.e.
  # specified by <MOD>_PATH variable
  SRCS += $(SRCS_APPSTG_FILES)
  PKG_PATHS = $(foreach PKG,$(PKG_LIST),$($(PKG)_PATH))
  VPATH += $(PKG_PATHS)

  export PKG_LIST
endif

# Change the extension from C to $(OBJEXT) and also add path
OBJ_PATHS = $(patsubst %.c, $(OBJDIR)/%.$(OBJEXT), $(notdir $(SRCS)))

# Change the extension from ASM to $(OBJEXT) and also add path
OBJ_PATHS_ASM = $(patsubst %.asm, $(OBJDIR)/%.$(OBJEXT), $(SRCS_ASM))

PACKAGE_PATHS = $(patsubst %, $(PACKAGEDIR)/%, $(PACKAGE_SRCS))

# Assemble include paths here
INCLUDE_EXTERNAL = $(foreach INCL,$(INCLUDE_EXTERNAL_INTERFACES),$($(INCL)_INCLUDE))
INCLUDE_INTERNAL = $(foreach INCL,$(INCLUDE_INTERNAL_INTERFACES),$($(INCL)_INCLUDE))
INCLUDE_ALL = $(CODEGEN_INCLUDE) $(INCDIR) $(INCLUDE_EXTERNAL) $(INCLUDE_INTERNAL)

# Add prefix "-I" to each of the include paths in INCLUDE_ALL
INCLUDES = $(addprefix -I,$(INCLUDE_ALL))

# Create rule to "make" all packages
.PHONY : $(PKG_LIST)
$(PKG_LIST) :
	$(ECHO) \# Making $(SOC):$(CORE):$(PROFILE_$(CORE)):$@...
	$(MAKE) -C $($@_PATH)

# Get libraries for all the packages
define GET_COMP_DEPENDENCY_SUB_PATH
  ifeq ($$($(1)_PLATFORM_DEPENDENCY),yes)
    ifeq ($$($(1)_CORE_DEPENDENCY),yes)
      $(1)_DEPSUBPATH = $(SOC)/$(CORE)
    else
      $(1)_DEPSUBPATH = $(SOC)/$(ISA)
    endif
   else
    ifeq ($$($(1)_CORE_DEPENDENCY),yes)
      $(1)_DEPSUBPATH = $(CORE)
    else
      $(1)_DEPSUBPATH = $(ISA)
    endif
  endif
endef

$(foreach LIB,$(PKG_LIST),$(eval $(call GET_COMP_DEPENDENCY_SUB_PATH,$(LIB))))

ifeq ($(DEST_ROOT),)
  LIB_PATHS = $(foreach LIB,$(PKG_LIST),$($(LIB)_PATH)/lib/$($(LIB)_DEPSUBPATH)/$(PROFILE_$(CORE))/$(LIB).$(LIBEXT))
else
  LIB_PATHS = $(foreach LIB,$(PKG_LIST),$(DEST_ROOT)/$($(LIB)_RELPATH)/lib/$($(LIB)_DEPSUBPATH)/$(PROFILE_$(CORE))/$(LIB).$(LIBEXT))
endif
EXT_LIB_PATHS = $(EXT_LIB_LIST)

# Include make rules for ISA that is built in this iteration
#   eg: rules_m3.mk
# Added makerules for a8 on gcc.
# In case multiple Tool Chains are needed for other cores as well,
# respective EXT for makerules file can be defined here

# Derive GCC EXT makerules file

ifeq ($(TOOLCHAIN_$(ISA)),gcc)
  TOOLCHAINEXT = $(TOOLCHAIN_$(ISA))_
endif

include $(MAKERULEDIR)/rules_$(TOOLCHAINEXT)$(ISA).mk
package : $(PACKAGE_PATHS)

ifdef MODULE_NAME
# Rules for module; this iteration is for a module

# Clean Object and Library (archive) directories
clean :
	$(RM) -rf $(OBJDIR)/* $(DEPDIR)/* $(LIBDIR)/*

# Create dependencies list to ultimately create module archive library file
$(CORE) : $(OBJDIR) $(DEPDIR) $(LIBDIR) $(LIBDIR)/$(MODULE_NAME).$(LIBEXT)


else
# Rules for application; this iteration is for an app

# Clean Object, Binary and Configuro generated directories
clean :
	$(RM) -rf $(OBJDIR)/* $(DEPDIR)/* $(BINDIR)/*

# Create dependencies list to ultimately create application executable binary
$(CORE) : $(OBJDIR) $(BINDIR) $(DEPDIR) $(PKG_LIST) $(EXE_NAME)

endif

#
# SBL Image/App image Generation
#

ifneq ("$(wildcard $(PDK_SBL_AUTO_COMP_PATH)/tools)","")
SBL_TOOLS_PATH=$(PDK_SBL_AUTO_COMP_PATH)/tools
else
SBL_TOOLS_PATH=$(PDK_SBL_COMP_PATH)/tools
endif

ifneq ("$(wildcard $(SBL_TOOLS_PATH)/out2rprc/out2rprc.exe)","")
OUTRPRC_PATH=$(SBL_TOOLS_PATH)/out2rprc/out2rprc.exe
endif
ifneq ("$(wildcard $(SBL_TOOLS_PATH)/out2rprc/bin/out2rprc.exe)","")
OUTRPRC_PATH=$(SBL_TOOLS_PATH)/out2rprc/bin/out2rprc.exe
endif

ifneq ("$(wildcard $(SBL_TOOLS_PATH)/multicoreImageGen/bin/MulticoreImageGen$(EXE_EXT))","")
  SBL_IMAGE_GEN=$(SBL_TOOLS_PATH)/multicoreImageGen/bin/MulticoreImageGen$(EXE_EXT)
else
  SBL_IMAGE_GEN=$(SBL_TOOLS_PATH)/multicore_image_generator/v1/MulticoreImageGen$(EXE_EXT)
endif

# SBL related macro
SBL_CERT_KEY_HS=$(PDK_INSTALL_PATH)/ti/build/makerules/k3_dev_mpk.pem
SBL_CERT_KEY=$(PDK_INSTALL_PATH)/ti/build/makerules/rom_degenerateKey.pem

ifeq ($(OS),Windows_NT)
  SBL_CERT_GEN=powershell -executionpolicy unrestricted -command $(PDK_INSTALL_PATH)/ti/build/makerules/x509CertificateGen.ps1
else
  SBL_CERT_GEN=$(PDK_INSTALL_PATH)/ti/build/makerules/x509CertificateGen.sh
endif

#Output file names/paths
ifeq ($(LOCAL_APP_NAME),)
  SBL_IMAGE_NAME=$(APP_NAME)_$(CORE)_$(BUILD_OS_TYPE)_$(PROFILE_$(CORE))
else
  SBL_IMAGE_NAME=$(LOCAL_APP_NAME)_$(BUILD_OS_TYPE)_$(PROFILE_$(CORE))
endif

SBL_RPRC_PATH=$(BINDIR)/$(SBL_IMAGE_NAME).rprc
SBL_APPIMAGE_PATH=$(BINDIR)/$(SBL_IMAGE_NAME).appimage
SBL_APPIMAGE_PATH_BE=$(BINDIR)/$(SBL_IMAGE_NAME)_BE.appimage
SBL_APP_BINIMAGE_PATH=$(EXE_NAME).bin
SBL_APPIMAGE_PATH_SIGNED=$(BINDIR)/$(SBL_IMAGE_NAME).appimage.signed
SBL_APPIMAGE_PATH_SIGNED_BE=$(BINDIR)/$(SBL_IMAGE_NAME)_BE.appimage.signed
SBL_STDOUT_FILE=$(BINDIR)/temp_stdout.txt
SBL_STDOUT_FILE2=$(BINDIR)/temp_stderr.txt
ifeq ($(OS),Windows_NT)
  SBL_OUTRPRC=$(OUTRPRC_PATH)
  ifeq ($($(APP_NAME)_SBL_XIP_APPIMAGEGEN),yes)
  SBL_XIP_GEN=$(SBL_TOOLS_PATH)/xipGen/xipGen.exe
  endif
else
  SBL_OUTRPRC=mono $(OUTRPRC_PATH)
  ifeq ($($(APP_NAME)_SBL_XIP_APPIMAGEGEN),yes)
  SBL_XIP_GEN=$(SBL_TOOLS_PATH)/xipGen/xipGen.out
  endif
endif

ifeq ($($(APP_NAME)_APP_SMP_CONFIG),yes)
MULTI_CORE_APP_PARAMS += $(SBL_CORE_ID_$(CORE)_smp) $(SBL_RPRC_PATH)
else
MULTI_CORE_APP_PARAMS += $(SBL_CORE_ID_$(CORE)) $(SBL_RPRC_PATH)
endif

#SBL App image generation
sbl_appimagegen: $(SBL_APPIMAGE_PATH) $(SBL_APP_BINIMAGE_PATH)

$(SBL_APP_BINIMAGE_PATH): $(EXE_NAME)
  ifeq ($($(APP_NAME)_SBL_APP_BINIMAGEGEN),yes)
	$(SBL_OBJ_COPY) $(EXE_NAME) -O binary $($(APP_NAME)_SBL_APP_BIN_SECTIONS) $(SBL_APP_BINIMAGE_PATH)
	$(ECHO) \# App Hex image $(SBL_APP_BINIMAGE_PATH) created.
	$(ECHO) \#
  endif

clean_appimagerprc: $(EXE_NAME)
	$(RM) -f $@ $(SBL_APPIMAGE_PATH_BE) $(SBL_RPRC_PATH)

sbl_appimagerprc: clean_appimagerprc
  ifeq ($($(APP_NAME)_SBL_XIP_APPIMAGEGEN),yes)
	$(SBL_OUTRPRC) $(EXE_NAME) $(SBL_RPRC_TMP_PATH) >> $(SBL_STDOUT_FILE)
	$(CHMOD) a+x $(SBL_XIP_GEN)
  else
	$(SBL_OUTRPRC) $(EXE_NAME) $(SBL_RPRC_PATH) >> $(SBL_STDOUT_FILE)
  endif
	$(CHMOD) a+x $(SBL_IMAGE_GEN)

$(SBL_APPIMAGE_PATH): sbl_appimagerprc
  ifeq ($($(APP_NAME)_SBL_XIP_APPIMAGEGEN),yes)
	$(SBL_XIP_GEN) -i $(SBL_RPRC_TMP_PATH) -o $(SBL_RPRC_PATH) -x $(SBL_RPRC_XIP_PATH) --flash-start-addr $(FLASH_START_ADDR) -v > $(SBL_STDOUT_FILE)
	$(SBL_IMAGE_GEN) LE $(SBL_DEV_ID) $(SBL_APPIMAGE_PATH)    $(MULTI_CORE_APP_PARAMS) >> $(SBL_STDOUT_FILE)
	$(SBL_IMAGE_GEN) LE $(SBL_DEV_ID) $(SBL_APPIMAGE_XIP_PATH) $(MULTI_CORE_APP_PARAMS_XIP) >> $(SBL_STDOUT_FILE)
	$(SBL_IMAGE_GEN) BE $(SBL_DEV_ID) $(SBL_APPIMAGE_PATH_BE) $(MULTI_CORE_APP_PARAMS) >> $(SBL_STDOUT_FILE)
  else
	$(SBL_IMAGE_GEN) LE $(SBL_DEV_ID) $@                      $(MULTI_CORE_APP_PARAMS) >> $(SBL_STDOUT_FILE)
	$(SBL_IMAGE_GEN) BE $(SBL_DEV_ID) $(SBL_APPIMAGE_PATH_BE) $(MULTI_CORE_APP_PARAMS) >> $(SBL_STDOUT_FILE)
  endif
 ifeq ($(SOC),$(filter $(SOC), am65xx am64x j721e j7200 j721s2 j784s4)) 
   ifneq ($(OS),Windows_NT)
	$(CHMOD) a+x $(SBL_CERT_GEN)
   endif
	$(SBL_CERT_GEN) -b $@ -o $(SBL_APPIMAGE_PATH_SIGNED)    -c R5 -l $(SBL_RUN_ADDRESS) -k $(SBL_CERT_KEY_HS)
 endif
	$(ECHO) \# SBL App image $@ and $(SBL_APPIMAGE_PATH_BE) created.
	$(ECHO) \#


# Nothing beyond this point
