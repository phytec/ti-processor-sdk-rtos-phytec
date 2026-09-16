#*******************************************************************************
#                                                                              *
# Copyright (c) 2021-2024 Texas Instruments Incorporated - http://www.ti.com/  *
#                        ALL RIGHTS RESERVED                                   *
#                                                                              *
#*******************************************************************************

# Filename: rules_r5f.mk
#
# Make rules for R5 - This file has all the common rules and defines required
#                     for Cortex-R5 ISA
#
# This file needs to change when:
#     1. Code generation tool chain changes
#     2. Internal switches (which are normally not touched) has to change
#     3. XDC specific switches change
#     4. a rule common for R5 ISA has to be added or modified

CGT_ISA = R5
CGT_EXT = r5f
CGT_PATH = $(TOOLCHAIN_PATH_R5)

# Set compiler/archiver/linker commands and include paths
CODEGEN_INCLUDE = $(TOOLCHAIN_PATH_$(CGT_ISA))/include

CC = $(TOOLCHAIN_PATH_$(CGT_ISA))/bin/tiarmclang
AR = $(TOOLCHAIN_PATH_$(CGT_ISA))/bin/tiarmar
LNK = $(TOOLCHAIN_PATH_$(CGT_ISA))/bin/tiarmclang
STRP = $(TOOLCHAIN_PATH_$(CGT_ISA))/bin/tiarmstrip
SIZE = $(TOOLCHAIN_PATH_$(CGT_ISA))/bin/tiarmsize

# Derive a part of RTS Library name based on ENDIAN: little/big
ifeq ($(ENDIAN),little)
  RTSLIB_ENDIAN = EL
else
  RTSLIB_ENDIAN = EB
endif

# Derive compiler switch and part of RTS Library name based on FORMAT: COFF/ELF
ifeq ($(FORMAT),ELF)
  CSWITCH_FORMAT = eabi
  RTSLIB_FORMAT = eabi
endif

# OPTIMIZATION can take two values i.e. CODE and PERFORMANCE
# This is parameter is only valid for release mode and not for debug mode
# CODE is recommended for optimizing code size 
# PERFORMANCE is recommended for optimizing performance, but it is likely to increase compiler generated code size
ifeq ($(OPTIMIZATION),PERFORMANCE)
  OPT_LEVEL = -O3
else
  OPT_LEVEL = -Oz
endif

LNKFLAGS_INTERNAL_COMMON +=

SUPRESS_WARNINGS_FLAG = -Wno-unused-command-line-argument -Wno-extra
#To remove Clang 2.1.3 warning on unused variables
#the warnings have proper deviation and are covered by SA/MISRA analysis
SUPRESS_WARNINGS_FLAG += -Wno-unused-but-set-variable

# Internal CFLAGS - normally doesn't change
CFLAGS_INTERNAL = -c -Wall $(SUPRESS_WARNINGS_FLAG) -$(RTSLIB_ENDIAN) -eo.$(OBJEXT) -ea.$(ASMEXT) -g -mfloat-abi=hard

# Vector Floating-Point co-processor instructions, vfpv3d16 is supported in R4/R5
CFLAGS_INTERNAL += -mfpu=vfpv3-d16 -mcpu=cortex-r5
ifeq ($(COMPILE_MODE), thumb)
  # Enabling thumb2 mode  
  CFLAGS_INTERNAL += -mthumb -march=thumbv7r
else
  # Enabling arm mode
  CFLAGS_INTERNAL += -marm -march=armv7-r
endif

ifeq ($(TREAT_WARNINGS_AS_ERROR), yes)
  CFLAGS_INTERNAL += -Werror
  LNKFLAGS_INTERNAL_COMMON += -Werror
endif

CFLAGS_DIROPTS = -c

ifneq ($(EXTERNAL_LNKCMD_FILE_LOCAL),)
EXTERNAL_LNKCMD_FILE = $(EXTERNAL_LNKCMD_FILE_LOCAL)
else
EXTERNAL_LNKCMD_FILE = $(CONFIG_BLD_LNK_r5f)
endif

# CFLAGS based on profile selected
ifeq ($(PROFILE_$(CORE)), debug)
 LNKFLAGS_INTERNAL_PROFILE =
 CFLAGS_INTERNAL += -D_DEBUG_=1 -O0
endif

ifeq ($(PROFILE_$(CORE)), release)
  LNKFLAGS_INTERNAL_PROFILE = $(LNKFLAGS_GLOBAL_$(CORE))
  CFLAGS_INTERNAL += $(OPT_LEVEL) -s
endif

# Following 'if...' block is for an application; to add a #define for each
#   component in the build. This is required to know - at compile time - which
#   components are on which core.
ifndef MODULE_NAME
  # Derive list of all packages from each of the components needed by the app
  PKG_LIST_R5_LOCAL = $(foreach COMP,$(COMP_LIST_$(CORE)),$($(COMP)_PKG_LIST))

  # Defines for the app and cfg source code to know which components/packages
  # are included in the build for the local CORE...
  CFLAGS_APP_DEFINES = $(foreach PKG,$(PKG_LIST_R5_LOCAL),-D_LOCAL_$(PKG)_)
  CFLAGS_APP_DEFINES += $(foreach PKG,$(PKG_LIST_R5_LOCAL),-D_BUILD_$(PKG)_)

  # Defines for the app and cfg source code to know which components/packages
  # are included in the build for the remote CORE...
  CFLAGS_APP_DEFINES += $(foreach PKG,$(PKG_LIST_R5_REMOTE),-D_REMOTE_$(PKG)_)
  CFLAGS_APP_DEFINES += $(foreach PKG,$(PKG_LIST_R5_REMOTE),-D_BUILD_$(PKG)_)

endif

# Assemble CFLAGS from all other CFLAGS definitions
_CFLAGS = $(CFLAGS_INTERNAL) $(CFLAGS_GLOBAL_$(CORE)) $(CFLAGS_LOCAL_COMMON) $(CFLAGS_LOCAL_$(CORE)) $(CFLAGS_LOCAL_$(SOC)) $(CFLAGS_LOCAL_$(SOCFAMILY)) $(CFLAGS_LOCAL_$(SOC)) $(CFLAGS_APP_DEFINES) $(CFLAGS_COMP_COMMON) $(CFLAGS_GLOBAL_$(SOC))

# Decide the compile mode
COMPILEMODE = -x c

# Object file creation
# The first $(CC) generates the dependency make files for each of the objects
# The second $(CC) compiles the source to generate object
$(OBJ_PATHS): $(OBJDIR)/%.$(OBJEXT): %.c
	$(ECHO) \# Compiling $(SOC):$(CORE):$(PROFILE_$(CORE)):$(APP_NAME)$(MODULE_NAME):$<
	$(CC) -MMD $(_CFLAGS) $(INCLUDES) -c $(COMPILEMODE) $<
	$(CC) $(_CFLAGS) $(INCLUDES) -c $(COMPILEMODE) $< -o $@

#TODO: Check ASMFLAGS if really required
ASMFLAGS = -me -g -mthumb

# Object file creation
$(OBJ_PATHS_ASM): $(OBJDIR)/%.$(OBJEXT): %.asm
	$(ECHO) \# $(OBJ_PATHS_ASM) Compiling $(SOC):$(CORE):$(PROFILE_$(CORE)):$(APP_NAME)$(MODULE_NAME): $<
	$(CC) -MMD $(_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -x ti-asm $<
	$(CC) $(_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -x ti-asm $< -o $@

$(PACKAGE_PATHS): $(PACKAGEDIR)/%: %
	$(ECHO) \# Copying $(PACKAGE_NAME)/$($(MODULE_NAME)_RELPATH)/$<
	$(MKDIR) -p $(DEST_ROOT)/package/$(PACKAGE_SELECT)/$(PACKAGE_NAME)/$($(MODULE_NAME)_RELPATH)
	$(CP) --parents -rf $< $(DEST_ROOT)/package/$(PACKAGE_SELECT)/$(PACKAGE_NAME)/$($(MODULE_NAME)_RELPATH)

# Archive flags - normally doesn't change
ARFLAGS = rc

# Archive/library file creation
$(LIBDIR)/$(MODULE_NAME).$(LIBEXT) : $(OBJ_PATHS_ASM) $(OBJ_PATHS)
	$(ECHO) \#
	$(ECHO) \# Archiving $(SOC):$(CORE):$(PROFILE_$(CORE)):$(MODULE_NAME)
	$(ECHO) \#
	$(AR) $(ARFLAGS) $@ $(OBJ_PATHS_ASM) $(OBJ_PATHS)

# Linker options and rules
LNKFLAGS_INTERNAL_COMMON += -Xlinker -q -Xlinker -u -Xlinker _c_int00 -Xlinker --display_error_number -Xlinker -c
# Supress warning for "entry-point symbol other than "_c_int00" specified"
LNKFLAGS_INTERNAL_COMMON += -Xlinker --diag_suppress=10063-D
# Supress warning for "no matching section"
LNKFLAGS_INTERNAL_COMMON += -Xlinker --diag_suppress=10068-D
LNKFLAGS_INTERNAL_COMMON += -Xlinker --reread_libs

ifeq ($(CGT_ISA), R5)
  LNKFLAGS_INTERNAL_COMMON += -mcpu=cortex-r5 -march=armv7-r
  #--diag_suppress=10063 supresses 'warning: entry point other than _c_int00 specified'
endif

# Assemble Linker flags from all other LNKFLAGS definitions
_LNKFLAGS = $(LNKFLAGS_PRELINK_FLAGS) $(LNKFLAGS_INTERNAL_COMMON) $(LNKFLAGS_INTERNAL_PROFILE) $(LNKFLAGS_GLOBAL_$(CORE)) $(LNKFLAGS_LOCAL_COMMON) $(LNKFLAGS_LOCAL_$(CORE))

# Path of the RTS library - normally doesn't change for a given tool-chain
#Let the linker choose the required library
RTSLIB_PATH = $(TOOLCHAIN_PATH_R5)/lib/libc.a

LNK_LIBS = $(addprefix -l,$(LIB_PATHS))
LNK_LIBS += $(addprefix -l,$(EXT_LIB_PATHS))

# Linker - to create executable file
ifeq ($(LOCAL_APP_NAME),)
 EXE_NAME = $(BINDIR)/$(APP_NAME)_$(CORE)_$(BUILD_OS_TYPE)_$(PROFILE_$(CORE)).$(EXEEXT)
else
 ifeq ($(PROFILE_$(CORE)),prod_release)
  EXE_NAME = $(BINDIR)/$(LOCAL_APP_NAME).$(EXEEXT)
 else
  EXE_NAME = $(BINDIR)/$(LOCAL_APP_NAME)_$(BUILD_OS_TYPE)_$(PROFILE_$(CORE)).$(EXEEXT)
 endif
endif

$(EXE_NAME) : $(OBJ_PATHS_ASM) $(OBJ_PATHS) $(LIB_PATHS) $(LNKCMD_FILE) $(EXTERNAL_LNKCMD_FILE)
	$(ECHO) \# Linking into $(EXE_NAME)...
	$(ECHO) \#
	$(LNK) $(_LNKFLAGS) $(OBJ_PATHS_ASM) $(OBJ_PATHS) -Xlinker $(LNKCMD_FILE) $(EXTERNAL_LNKCMD_FILE) $(APPEND_LNKCMD_FILE) -Xlinker --map_file=$@.map -Xlinker --output_file=$@ $(LNK_LIBS) $(RTSLIB_PATH)
	$(ECHO) \#
	$(ECHO) \# $@ created.
	$(ECHO) \#

# Include dependency make files that were generated by $(CC)
-include $(SRCS:%.c=$(DEPDIR)/%.P)

# Nothing beyond this point
