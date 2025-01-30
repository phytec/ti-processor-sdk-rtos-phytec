#
# Copyright (c) 2013-2016, Texas Instruments Incorporated
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# *  Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#
# *  Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# *  Neither the name of Texas Instruments Incorporated nor the names of
#    its contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
# THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# Filename: rules_ti_cgt_arm.mk
#
# Make rules for TI ARM CGT - This file has all the common rules and defines
# required for Cortex-M4/R5F ISA
#
# This file needs to change when:
#     1. Code generation tool chain changes (currently it uses TI ARM CGT)
#     2. Internal switches (which are normally not touched) has to change
#     3. XDC specific switches change
#     4. a rule common for M4/R5F ISA has to be added or modified

# Set compiler/archiver/linker commands and include paths
ifeq ($(CGT_ISA),$(filter $(CGT_ISA), M4F))
CGT_ISA_PATH_PRFX = M4
else
CGT_ISA_PATH_PRFX = $(CGT_ISA)
endif

CODEGEN_INCLUDE = $(TOOLCHAIN_PATH_$(CGT_ISA_PATH_PRFX))/include
CC = $(TOOLCHAIN_PATH_$(CGT_ISA_PATH_PRFX))/bin/tiarmclang
AR = $(TOOLCHAIN_PATH_$(CGT_ISA_PATH_PRFX))/bin/tiarmar
LNK = $(TOOLCHAIN_PATH_$(CGT_ISA_PATH_PRFX))/bin/tiarmclang
STRP = $(TOOLCHAIN_PATH_$(CGT_ISA_PATH_PRFX))/bin/tiarmstrip
SIZE = $(TOOLCHAIN_PATH_$(CGT_ISA_PATH_PRFX))/bin/tiarmsize

# Derive a part of RTS Library name based on ENDIAN: little/big
ifeq ($(ENDIAN),little)
  RTSLIB_ENDIAN = EL

else
  RTSLIB_ENDIAN = EB
endif

# Derive compiler switch and part of RTS Library name based on FORMAT: COFF/ELF
ifeq ($(FORMAT),COFF)
  CSWITCH_FORMAT = ti_arm9_abi
  RTSLIB_FORMAT = tiarm9
endif
ifeq ($(FORMAT),ELF)
  CSWITCH_FORMAT = eabi
  RTSLIB_FORMAT = eabi
endif

# OPTIMIZATION can take two values i.e. CODE and PERFORMANCE
# This is parameter is only valid for release mode and not for debug mode
# CODE is recommended for optimizing code size 
# PERFORMANCE is recommended for optimizing performance, but it is likely to increase compiler generated code size
ifeq ($(OPTIMIZATION),PERFORMANCE)
  OPT_LEVEL = -O3 -mthumb
else
  OPT_LEVEL = -Oz
endif

ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
CFLAGS_LOCAL_COMMON += -mcpu=cortex-r5 -mfloat-abi=hard -mfpu=vfpv3-d16 -mthumb -Wall -g -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function
LNKFLAGS_LOCAL_COMMON += -Wl,--diag_suppress=10083 -Wl,--ram_model -Wl,--reread_libs
endif

LNKFLAGS_INTERNAL_COMMON +=

SUPRESS_WARNINGS_FLAG = -Wno-extra -Wno-exceptions -ferror-limit=100 -Wno-unused-command-line-argument -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function -Wno-extern-initializer -Wno-excess-initializers -Wno-bitfield-constant-conversion -Wno-address-of-packed-member

ifeq ($(CPLUSPLUS_BUILD), yes)
  SUPRESS_WARNINGS_FLAG += -Wno-c99-designator -Wno-extern-c-compat -Wno-c++11-narrowing -Wno-reorder-init-list -Wno-deprecated-register -Wno-writable-strings -Wno-enum-compare -Wno-reserved-user-defined-literal -Wno-unused-const-variable -Wno-missing-braces
endif

# Internal CFLAGS - normally doesn't change
ifeq ($(CGT_ISA),$(filter $(CGT_ISA), M4 R5 M3))
  CFLAGS_INTERNAL = -c -Wall -Werror $(SUPRESS_WARNINGS_FLAG) -$(RTSLIB_ENDIAN) -eo.$(OBJEXT) -ea.$(ASMEXT) -g -mfloat-abi=hard
  ifeq ($(CGT_ISA),$(filter $(CGT_ISA), R5))
    CFLAGS_INTERNAL += -mfpu=vfpv3-d16 -mcpu=cortex-r5
    ifeq ($(COMPILE_MODE), thumb)
      # Enabling thumb2 mode
      CFLAGS_INTERNAL += -mthumb -march=thumbv7r
     else
      # Enabling arm mode
       CFLAGS_INTERNAL += -marm -march=armv7-r
    endif
  else
    CFLAGS_INTERNAL += -mfpu=vfplib
  endif
else ifeq ($(CGT_ISA), Arm9)
  CFLAGS_INTERNAL = -c -Wall -Werror $(SUPRESS_WARNINGS_FLAG) --endian=$(ENDIAN) -mv5e --float_support=vfplib -eo.$(OBJEXT) -ea.$(ASMEXT) -g
endif

# Reset the CFLAGS_INTERNAL flag for M4F
ifeq ($(CGT_ISA), M4F)
  CFLAGS_INTERNAL = -mcpu=cortex-m4 -c -Wall -Werror $(SUPRESS_WARNINGS_FLAG) --endian=$(ENDIAN) -mv7M4 -eo.$(OBJEXT) -ea.$(ASMEXT) -g
  CFLAGS_INTERNAL += -mfpu=fpv4-sp-d16
endif

ifeq ($(TREAT_WARNINGS_AS_ERROR), yes)
  CFLAGS_INTERNAL += -Werror
  LNKFLAGS_INTERNAL_COMMON += -Werror
endif
CFLAGS_DIROPTS = -c

ifeq ($(CGT_ISA),$(filter $(CGT_ISA),R5))

ifeq ($(XDC_DISABLE_THUMB_MODE),yes)
 XDC_TARGET_NAME=$(CGT_ISA)F
else
 XDC_TARGET_NAME=$(CGT_ISA)Ft
endif

 ifneq ($(EXTERNAL_LNKCMD_FILE_LOCAL),)
 EXTERNAL_LNKCMD_FILE = $(EXTERNAL_LNKCMD_FILE_LOCAL)
 else
 EXTERNAL_LNKCMD_FILE = $(CONFIG_BLD_LNK_r5f)
 endif
else
 XDC_TARGET_NAME=$(CGT_ISA)
endif

# Reset the XDC_TARGET_NAME for M4F
ifeq ($(CGT_ISA),$(filter $(CGT_ISA),M4F))
 XDC_TARGET_NAME=M4

 ifneq ($(EXTERNAL_LNKCMD_FILE_LOCAL),)
 EXTERNAL_LNKCMD_FILE = $(EXTERNAL_LNKCMD_FILE_LOCAL)
 else
 EXTERNAL_LNKCMD_FILE = $(CONFIG_BLD_LNK_m4f)
 endif
endif

XDC_HFILE_NAME = $(basename $(notdir $(XDC_CFG_FILE_$(CORE))))

ifneq ($(PEXT_BIOS),)
XDC_HFILE_EXT = $(PEXT_BIOS)
else
XDC_HFILE_EXT = p$(FORMAT_EXT)$(CGT_EXT)
endif

# CFLAGS based on profile selected
ifeq ($(BUILD_PROFILE_$(CORE)), debug)
 LNKFLAGS_INTERNAL_BUILD_PROFILE =
 CFLAGS_XDCINTERNAL = -Dxdc_target_name__=$(XDC_TARGET_NAME) -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_debug
 CFLAGS_INTERNAL += -D_DEBUG_=1 -O0
 ifndef MODULE_NAME
  CFLAGS_XDCINTERNAL += -Dxdc_cfg__header__='$(CONFIGURO_DIR)/package/cfg/$(XDC_HFILE_NAME)_$(XDC_HFILE_EXT).h'
 endif

endif
ifeq ($(BUILD_PROFILE_$(CORE)), release)
 ifeq ($(CGT_ISA),$(filter $(CGT_ISA), M4 R5 M3))
   LNKFLAGS_INTERNAL_BUILD_PROFILE = $(LNKFLAGS_GLOBAL_$(CORE))
   ifeq ($(CGT_ISA),$(filter $(CGT_ISA), R5))
     CFLAGS_INTERNAL += $(OPT_LEVEL) -s
     ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s))
       CFLAGS_INTERNAL += $(OPT_LEVEL) -Os
     endif
   else
     CFLAGS_INTERNAL += $(OPT_LEVEL) -op0 -os --optimize_with_debug --inline_recursion_limit=20
   endif
   CFLAGS_XDCINTERNAL = -Dxdc_target_name__=$(XDC_TARGET_NAME) -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release
   ifndef MODULE_NAME
     CFLAGS_XDCINTERNAL += -Dxdc_cfg__header__='$(CONFIGURO_DIR)/package/cfg/$(XDC_HFILE_NAME)_$(XDC_HFILE_EXT).h'
   endif
 endif
 ifeq ($(CGT_ISA), Arm9)
        LNKFLAGS_INTERNAL_BUILD_PROFILE = --diag_warning=225 --diag_suppress=23000 $(LNKFLAGS_GLOBAL_$(CORE))
        CFLAGS_INTERNAL += -oe $(OPT_LEVEL) -op0 -os --optimize_with_debug --inline_recursion_limit=20
	CFLAGS_XDCINTERNAL = -Dxdc_target_name__=$(XDC_TARGET_NAME) -Dxdc_target_types__=ti/targets/arm/elf/std.h -Dxdc_bld__profile_release
	ifndef MODULE_NAME
	  CFLAGS_XDCINTERNAL += -Dxdc_cfg__header__='$(CONFIGURO_DIR)/package/cfg/$(XDC_HFILE_NAME)_$(XDC_HFILE_EXT).h'
	endif
 endif
 ifeq ($(CGT_ISA),$(filter $(CGT_ISA), M4F))
	# LNKFLAGS_INTERNAL_BUILD_PROFILE = --opt='--float_support=FPv4SPD16 --endian=$(ENDIAN) -mv7M4 -Wno-gnu-variable-sized-type-not-at-end -Wno-unused-function $(CFLAGS_GLOBAL_$(CORE)) -oe -g -op2 $(OPT_LEVEL) -os --optimize_with_debug --inline_recursion_limit=20 --diag_suppress=23000' --strict_compatibility=on
     LNKFLAGS_INTERNAL_BUILD_PROFILE = --diag_warning=225 --diag_suppress=23000 $(LNKFLAGS_GLOBAL_$(CORE))
	 CFLAGS_INTERNAL += -oe $(OPT_LEVEL) -op0 -os --optimize_with_debug --inline_recursion_limit=20
 endif
endif

# Assemble CFLAGS from all other CFLAGS definitions
_CFLAGS = $(CFLAGS_GLOBAL_$(CORE)) $(CFLAGS_INTERNAL) $(CFLAGS_LOCAL_COMMON) $(CFLAGS_LOCAL_$(CORE)) $(CFLAGS_COMP_COMMON)
ifeq ($($(MODULE_NAME)_BOARD_DEPENDENCY),yes)
  _CFLAGS += $(CFLAGS_LOCAL_$(BOARD)) $(CFLAGS_GLOBAL_$(BOARD))
else
  ifeq ($($(APP_NAME)_BOARD_DEPENDENCY),yes)
    _CFLAGS += $(CFLAGS_LOCAL_$(BOARD)) $(CFLAGS_GLOBAL_$(BOARD))
  else
    _CFLAGS += $(CFLAGS_LOCAL_$(SOC)) $(CFLAGS_GLOBAL_$(SOC))
  endif
endif
ifneq ($(XDC_CFG_FILE_$(CORE)),)
  _CFLAGS += $(CFLAGS_XDCINTERNAL)
else
  ifneq ($(findstring xdc, $(INCLUDE_EXTERNAL_INTERFACES)),)
      _CFLAGS += $(CFLAGS_XDCINTERNAL)
  endif
endif

# For TPR12, fore enum type to int to be compatible with DSP
ifeq ($(SOC),$(filter $(SOC), tpr12 awr294x))
  _CFLAGS += --enum_type=int
endif

# Decide the compile mode
COMPILEMODE = -x c
ifeq ($(CPLUSPLUS_BUILD), yes)
  COMPILEMODE = -x c++
endif

# Object file creation
# The first $(CC) generates the dependency make files for each of the objects
# The second $(CC) compiles the source to generate object
$(OBJ_PATHS): $(OBJDIR)/%.$(OBJEXT): %.c $(GEN_FILE) | $(OBJDIR) $(DEPDIR)
	$(ECHO) \# Compiling $(PRINT_MESSAGE):$<
	$(MKDIR) -p $(dir $@)
	$(CC) -MMD $(_CFLAGS) $(INCLUDES) -c $(COMPILEMODE) $< -o $@ -MF $(DEPFILE).d
	$(CC) $(_CFLAGS) $(INCLUDES) -c $(COMPILEMODE) $< -o $@


ASMFLAGS = -me -g -mthumb --diag_warning=225

# Object file creation
$(OBJ_PATHS_ASM): $(OBJDIR)/%.$(OBJEXT): %.asm $(GEN_FILE) | $(OBJDIR) $(DEPDIR)
	$(ECHO) \# Compiling $(PRINT_MESSAGE): $<
	$(CC) -MMD $(_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -x ti-asm $< -o $@
	$(CC) $(_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -x ti-asm $< -o $@

# Object file creation
$(OBJ_PATHS_S): $(OBJDIR)/%.$(OBJEXT): %.S $(GEN_FILE) | $(OBJDIR) $(DEPDIR)
	$(ECHO) \# Compiling $(PRINT_MESSAGE): $<
	$(CC) -MMD -c $(_CFLAGS) $< -o $@ -MF $(DEPFILE).d
	$(CC) -c $(_CFLAGS) $< -o $@

$(PACKAGE_PATHS): $(PACKAGEDIR)/%: %
	$(ECHO) \# Copying to $(PACKAGE_RELPATH)/$($(APP_NAME)$(MODULE_NAME)_RELPATH)/$<
	$(MKDIR) -p $(PACKAGE_ROOT)/$($(APP_NAME)$(MODULE_NAME)_RELPATH)
	$(CP) --parents -rf $< $(PACKAGE_ROOT)/$($(APP_NAME)$(MODULE_NAME)_RELPATH)

# Archive flags - normally doesn't change
ARFLAGS = rc

# Archive/library file creation
$(LIBDIR)/$(LIBNAME).$(LIBEXT) : $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS) $(GEN_FILE) | $(LIBDIR)
	$(ECHO) \#
	$(ECHO) \# Archiving $(PRINT_MESSAGE) into $@ ...
	$(ECHO) \#
	$(AR) $(ARFLAGS) $@ $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS)

$(LIBDIR)/$(LIBNAME).$(LIBEXT)_size: $(LIBDIR)/$(LIBNAME).$(LIBEXT)
	$(ECHO) \#
	$(ECHO) \# Computing sectti size $(PRINT_MESSAGE) info into $@.txt ...
	$(ECHO) \#
	$(SIZE) -x $(subst _size,,$@) > $@temp
	$(SECTTI) $@temp > $@.txt
	$(RM)   $@temp

# Linker options and rules
# These Extra Flags have been added reffering the MCU+SDK example
ifeq ($(SOC),$(filter $(SOC), am62x am62ax am62px j722s)) 
LNKFLAGS_INTERNAL_COMMON += -Wl,--diag_suppress=10063 -Wl,--ram_model -Wl,--reread_libs -Wl
endif
LNKFLAGS_INTERNAL_COMMON += -Xlinker -q -Xlinker -u -Xlinker _c_int00 -Xlinker --display_error_number -Xlinker --use_memcpy=fast -Xlinker --use_memset=fast
# Supress warning for "entry-point symbol other than "_c_int00" specified"
LNKFLAGS_INTERNAL_COMMON += -Xlinker --diag_suppress=10063-D 
# Supress warning for "no matching section"
LNKFLAGS_INTERNAL_COMMON += -Xlinker --diag_suppress=10068-D
# Supress warning for " LOAD placement ignored for "":  object is uninitialized"
LNKFLAGS_INTERNAL_COMMON += -Xlinker --diag_suppress=10083-D


ifeq ($(BOARD),$(filter $(BOARD), qtJ7))
  LNKFLAGS_INTERNAL_COMMON += -Xlinker -cr -Xlinker --ram_model
else
  LNKFLAGS_INTERNAL_COMMON += -Xlinker -c
endif

ifeq ($(CGT_ISA), R5)
  LNKFLAGS_INTERNAL_COMMON += -mcpu=cortex-r5 -march=armv7-r
  #--diag_suppress=10063 supresses 'warning: entry point other than _c_int00 specified'
else
ifeq ($(CGT_ISA), M4F)
  LNKFLAGS_INTERNAL_COMMON += --mcpu=cortex-m4
else
ifeq ($(CGT_ISA), Arm9)
  LNKFLAGS_INTERNAL_COMMON +=
else
  LNKFLAGS_INTERNAL_COMMON += --silicon_version=7$(CGT_ISA)
endif
endif
endif

# Assemble Linker flags from all other LNKFLAGS definitions
_LNKFLAGS = $(LNKFLAGS_INTERNAL_COMMON) $(LNKFLAGS_INTERNAL_BUILD_PROFILE) $(LNKFLAGS_GLOBAL_$(CORE)) $(LNKFLAGS_LOCAL_COMMON) $(LNKFLAGS_LOCAL_$(CORE))

# Path of the RTS library - normally doesn't change for a given tool-chain
#Let the linker choose the required library
RTSLIB_PATH = $(CGT_PATH)/lib/libc.a
LNK_LIBS = $(addprefix -l,$(LIB_PATHS))
LNK_LIBS += $(EXT_LIB_LIST)

# Linker - to create executable file
ifeq ($(LOCAL_APP_NAME),)
 EXE_NAME = $(BINDIR)/$(APP_NAME)_$(CORE)_$(BUILD_PROFILE_$(CORE)).$(EXEEXT)
 EXE_STRIP_NAME = $(BINDIR)/$(APP_NAME)_$(CORE)_$(BUILD_PROFILE_$(CORE))_strip.$(EXEEXT)
else
 EXE_NAME = $(BINDIR)/$(LOCAL_APP_NAME)_$(BUILD_PROFILE_$(CORE)).$(EXEEXT)
 EXE_STRIP_NAME = $(BINDIR)/$(LOCAL_APP_NAME)_$(BUILD_PROFILE_$(CORE))_strip.$(EXEEXT)
endif

NUM_PROCS = 1

ifeq ($(OS),Windows_NT)
  NUM_PROCS = $(NUMBER_OF_PROCESSORS)
else
  NUM_PROCS = $(shell grep -c ^processor /proc/cpuinfo)
endif

# Commenting this out as these are already available with compiler
#ifneq ($(findstring mcu,$(CORE)),)
#RTSLIB_NAME = rtsv7R4_T_le_v3D16_eabi.lib
#BUILD_LIB_ONCE = $(CGT_PATH)/lib/$(RTSLIB_NAME)
#$(BUILD_LIB_ONCE):
#	$(ECHO) \# $@ not found, building  $@ ...
#	$(CGT_PATH)/lib/mklib --pattern=$(RTSLIB_NAME) --parallel=$(NUM_PROCS) --compiler_bin_dir=$(CGT_PATH)/bin
#endif

ifneq ($(XDC_CFG_FILE_$(CORE)),)
$(EXE_NAME) : $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS) $(LIB_PATHS) $(LNKCMD_FILE) $(OBJDIR)/$(CFG_COBJ_XDC) $(BUILD_LIB_ONCE)
	$(CP) $(CONFIGURO_DIR)/package/cfg/*.rov.xs $(BINDIR)
else
$(EXE_NAME) : $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS) $(LIB_PATHS) $(LNKCMD_FILE) $(BUILD_LIB_ONCE)
ifeq ($(BUILD_OS_TYPE), freertos)
	$(CP) $(pdk_PATH)/ti/kernel/freertos/rov/syscfg_c.rov.xs $(BINDIR)
endif
endif
	$(ECHO) \# Linking into $(EXE_NAME)...
	$(ECHO) \#
ifneq ($(XDC_CFG_FILE_$(CORE)),)
	$(CP) $(OBJDIR)/$(CFG_COBJ_XDC) $(CONFIGURO_DIR)/package/cfg
  ifeq ($(BUILD_PROFILE_$(CORE)),whole_program_debug)
	$(LNK) $(_LNKFLAGS) $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS) $(OBJDIR)/$(CFG_COBJ_XDC) $(LNKCMD_FILE) $(EXTERNAL_LNKCMD_FILE) $(APPEND_LNKCMD_FILE) -o $@ -Xlinker -map_file=$@.map $(LNK_LIBS) 
  else
	$(LNK) $(_LNKFLAGS) $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS) $(LNKCMD_FILE) $(EXTERNAL_LNKCMD_FILE) $(APPEND_LNKCMD_FILE) -o $@ -m $@.map $(LNK_LIBS) 
  endif
else
	$(LNK) $(_LNKFLAGS) $(OBJ_PATHS_ASM) $(OBJ_PATHS_S) $(OBJ_PATHS) -Xlinker $(LNKCMD_FILE) $(EXTERNAL_LNKCMD_FILE) $(APPEND_LNKCMD_FILE) -Xlinker --map_file=$@.map -Xlinker --output_file=$@ $(LNK_LIBS) 
endif
	$(ECHO) \#
	$(ECHO) \# $@ created.
	$(ECHO) \#
ifeq ($(BUILD_PROFILE_$(CORE)), release)
	$(ECHO) \# Generating stripped image into $(EXE_STRIP_NAME)...
	$(ECHO) \#
	$(RM)  $(EXE_STRIP_NAME)
	$(STRP) -p $(EXE_NAME) -o $(EXE_STRIP_NAME)
endif
# XDC specific - assemble XDC-Configuro command
ifeq ($(BUILD_PROFILE_$(CORE)),prod_release)
  CONFIGURO_BUILD_PROFILE = release
else
  CONFIGURO_BUILD_PROFILE = $(BUILD_PROFILE_$(CORE))
endif

_XDC_GREP_STRING = \"$(XDC_GREP_STRING)\"
EGREP_CMD = $(EGREP) -ivw $(XDC_GREP_STRING) $(XDCLNKCMD_FILE)

ifeq ($(OS),Windows_NT)
  EVERYONE = $(word 1,$(shell whoami -groups | findstr "S-1-1-0"))
endif

# Invoke configuro for the rest of the components
#  NOTE: 1. String handling is having issues with various make versions when the
#           cammand is directly tried to be given below. Hence, as a work-around,
#           the command is re-directed to a file (shell or batch file) and then
#           executed
#        2. The linker.cmd file generated, includes the libraries generated by
#           XDC. An egrep to search for these and omit in the .cmd file is added
#           after configuro is done
xdc_configuro : $(CFG_C_XDC)
	$(MAKE) $(LNKCMD_FILE)

ifeq ($(DEST_ROOT),)
  XDC_BUILD_ROOT = .
else
  XDC_BUILD_ROOT = $(DEST_ROOT)
endif

ifneq ($(OBJEXT_BIOS),)
# if BIOS' obj extensions are different, xdc generated files's objfiles need to have different extensions as well
CFG_C_XDC_CFLAGS = $(subst -eo.$(OBJEXT),-eo.$(OBJEXT_BIOS),$(_CFLAGS))
else
CFG_C_XDC_CFLAGS = $(_CFLAGS)
endif

$(CFG_C_XDC) $(LNKCMD_FILE) : $(XDC_CFG_FILE_NAME) $(XDC_CFG_UPDATE)
	$(ECHO) \# Invoking configuro...
	$(MKDIR) -p $(XDC_BUILD_ROOT)
	$(xdc_PATH)/xs xdc.tools.configuro --generationOnly -o $(CONFIGURO_DIR) -t $(TARGET_XDC) -p $(PLATFORM_XDC) \
               -r $(CONFIGURO_BUILD_PROFILE) -b $(CONFIG_BLD_XDC_$(ISA)) --ol $(LNKCMD_FILE) $(XDC_CFG_FILE_NAME)
	$(ECHO) \# Configuro done!

ifneq ($(XDC_CFG_FILE_$(CORE)),)
  ifndef MODULE_NAME
$(OBJDIR)/$(CFG_COBJ_XDC) : $(CFG_C_XDC)
	$(ECHO) \# Compiling generated $(CFG_COBJ_XDC)
	$(CC) -M $(CFG_C_XDC_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -x c $(CFG_C_XDC)
	$(CC) $(CFG_C_XDC_CFLAGS) $(INCLUDES) $(CFLAGS_DIROPTS) -x c $(CFG_C_XDC)
  endif
endif

# Include dependency make files that were generated by $(CC)
-include $(SRCS:%.c=$(DEPDIR)/%.d)

# Nothing beyond this point
