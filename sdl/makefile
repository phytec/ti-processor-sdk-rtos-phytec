
include Rules.make

SDL_TESTS = $(sdl_PATH)/test
SDL_EXAMPLES = $(sdl_PATH)/examples

SOCFAMILY_LIST_DEFAULT = j721e

sdl_LIB_LIST_CLEAN = $(addsuffix _clean, $(sdl_LIB_LIST))
sdl_pdk_LIB_LIST_CLEAN = $(addsuffix _clean, $(sdl_pdk_LIB_LIST))
sdl_TEST_LIST_CLEAN = $(addsuffix _clean, $(sdl_TEST_LIST))
sdl_EXAMPLE_LIST_CLEAN = $(addsuffix _clean, $(sdl_EXAMPLE_LIST))
sdl_PKG_LIST_ALL_CLEAN = $(addsuffix _clean, $(sdl_PKG_LIST_ALL))

.PHONY : sdl_libs sdl_libs_clean test test_clean examples examples_clean all clean $(sdl_PKG_LIST_ALL) $(sdl_pdk_LIB_LIST)

ifeq ($(SOC), $(filter $(SOC), j721e))
#PDK Build Options - used for building dependencies for test apps
PDK_BUILD_OPTIONS = MAKERULEDIR=$(PDK_INSTALL_PATH)/ti/build/makerules PDK_INSTALL_PATH=$(PDK_INSTALL_PATH)
PDK_BUILD_OPTIONS += TOOLCHAIN_PATH_R5=$(TOOLCHAIN_PATH_R5)
PDK_BUILD_OPTIONS += SOC=$(SOC) BOARD=j721e_evm CORE=mcu1_0 BUILD_PROFILE=$(PROFILE)
sdl_DEPEND_LIST = pdk
sdl_DEPEND_LIST_CLEAN = pdk_clean
endif

ifeq ($(SOC), $(filter $(SOC), j7200))
#PDK Build Options - used for building dependencies for test apps
PDK_BUILD_OPTIONS = MAKERULEDIR=$(PDK_INSTALL_PATH)/ti/build/makerules PDK_INSTALL_PATH=$(PDK_INSTALL_PATH)
PDK_BUILD_OPTIONS += TOOLCHAIN_PATH_R5=$(TOOLCHAIN_PATH_R5)
PDK_BUILD_OPTIONS += SOC=$(SOC) BOARD=j7200_evm CORE=mcu1_0 BUILD_PROFILE=$(PROFILE)
sdl_DEPEND_LIST = pdk
sdl_DEPEND_LIST_CLEAN = pdk_clean
endif


ifeq ($(SOC), $(filter $(SOC), j721s2))
#PDK Build Options - used for building dependencies for test apps
PDK_BUILD_OPTIONS = MAKERULEDIR=$(PDK_INSTALL_PATH)/ti/build/makerules PDK_INSTALL_PATH=$(PDK_INSTALL_PATH)
PDK_BUILD_OPTIONS += TOOLCHAIN_PATH_R5=$(TOOLCHAIN_PATH_R5)
PDK_BUILD_OPTIONS += SOC=$(SOC) BOARD=phyboard_izar CORE=mcu1_0 BUILD_PROFILE=$(PROFILE)
sdl_DEPEND_LIST = pdk
sdl_DEPEND_LIST_CLEAN = pdk_clean
endif

ifeq ($(SOC), $(filter $(SOC), j784s4))
#PDK Build Options - used for building dependencies for test apps
PDK_BUILD_OPTIONS = MAKERULEDIR=$(PDK_INSTALL_PATH)/ti/build/makerules PDK_INSTALL_PATH=$(PDK_INSTALL_PATH)
PDK_BUILD_OPTIONS += TOOLCHAIN_PATH_R5=$(TOOLCHAIN_PATH_R5)
PDK_BUILD_OPTIONS += SOC=$(SOC) BOARD=j784s4_evm CORE=mcu1_0 BUILD_PROFILE=$(PROFILE)
sdl_DEPEND_LIST = pdk
sdl_DEPEND_LIST_CLEAN = pdk_clean
endif

depend: $(sdl_DEPEND_LIST)

depend_clean: $(sdl_DEPEND_LIST_CLEAN)

version:
	$(ECHO) ------------------------------------------------------
	$(ECHO) \# SDL 01.03.00.00
	$(ECHO) ------------------------------------------------------

pdk: $(sdl_pdk_LIB_LIST)

pdk_clean: $(sdl_pdk_LIB_LIST_CLEAN)

sdl_libs:  $(sdl_LIB_LIST)

sdl_libs_clean: $(sdl_LIB_LIST_CLEAN)

$(sdl_pdk_LIB_LIST) $(sdl_pdk_LIB_LIST_CLEAN):
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build $@ $(PDK_BUILD_OPTIONS)

test: depend $(sdl_TEST_LIST)

test_clean: $(sdl_TEST_LIST_CLEAN)

examples: depend $(sdl_EXAMPLE_LIST)

examples_clean: $(sdl_EXAMPLE_LIST_CLEAN)

all: version depend sdl_libs test examples

clean: sdl_libs_clean test_clean examples_clean pdk_clean

allclean:
	$(RM) -rf $(DEST_ROOT)

help:
	$(ECHO) ------------------------------------------------------
	$(ECHO) \# SDL make help
	$(ECHO) ------------------------------------------------------
	$(ECHO) make -s [OPTIONAL MAKE VARIABLES]
	$(ECHO)
	$(ECHO) "Supported targets:"
	$(ECHO) "------------------"
	$(ECHO) "all          : Builds all libraries and examples"
	$(ECHO) "clean        : Cleans all libraries and examples"
	$(ECHO) "allclean     : Removes the binary directory using rm -rf"
	$(ECHO) "examples     : Builds all safety examples"
	$(ECHO) "sdl_libs     : Builds all libraries"
	$(ECHO) "test         : Builds all tests"
	$(ECHO) "<Module>     : Builds a module: $(sdl_LIB_LIST)"
	$(ECHO) "<Module_Test>: Builds a test: $(sdl_TEST_LIST)"
	$(ECHO) "<Module_App> : Builds a app: $(sdl_EXAMPLE_LIST)"
	$(ECHO) ""
	$(ECHO) "Optional make variables:"
	$(ECHO) "------------------------"
	$(ECHO) "SOC=[j721e j7200 j721s2 j784s4]"
	$(ECHO) "    Default: j721e"
	$(ECHO) "CORE=[r5f]"
	$(ECHO) "    Default: r5f"
	$(ECHO) "PROFILE=[debug / release]"
	$(ECHO) "    Default: debug"
	$(ECHO) "BUILD_OS_TYPE=[baremetal / freertos / safertos]"
	$(ECHO) "    Default: baremetal"
	$(ECHO) "OS=[Linux / Windows]"
	$(ECHO) "    Default: Linux"

platforms:
	$(MAKE) all SOC=j721e

platformsclean:
	$(MAKE) clean SOC=j721e

profiles:
	$(MAKE) all PROFILE_$(CORE)=debug
	$(MAKE) all PROFILE_$(CORE)=release

profilesclean:
	$(MAKE) clean PROFILE_$(CORE)=debug
	$(MAKE) clean PROFILE_$(CORE)=release

allall:
	$(MAKE) platforms PROFILE_$(CORE)=debug
	$(MAKE) platforms PROFILE_$(CORE)=release


#=================================================================
#SDLs libs and tests
$(sdl_PKG_LIST_TESTS): depend
	$(ECHO) BUILD_OS_TYPE is $(BUILD_OS_TYPE)
	$(if $(filter $(SOCFAMILY), $(subst emptyreplacement,,$($@_SOCLIST))),\
		$(if $(filter $(BUILD_OS_TYPE), $(subst emptyreplacement,,$($@_OSLIST))),\
			$(MAKE) -C $($@_PATH) $($@_MAKEFILE),$(ECHO) Nothing to be done for $(SOCFAMILY) $(BUILD_OS_TYPE) $@),\
			$(ECHO) Nothing to be done for $(SOCFAMILY) $@)
	$(if $(filter $(SOCFAMILY), $(subst emptyreplacement,,$($@_SOCLIST))),\
		$(if $(filter $(BUILD_OS_TYPE), $(subst emptyreplacement,,$($@_OSLIST))),\
			$(if $(filter yes, $(subst emptyreplacement,,$($@_SBL_APPIMAGEGEN))),\
                        	$(MAKE) -C $($@_PATH) $($@_MAKEFILE) sbl_appimagegen,),),)

$(sdl_PKG_LIST_LIBS):
	$(if $(filter $(SOCFAMILY), $(subst emptyreplacement,,$($@_SOCLIST))),$(MAKE) -C $($@_PATH) $($@_MAKEFILE),$(ECHO) Nothing to be done for $(SOCFAMILY) $@)

$(sdl_PKG_LIST_ALL_CLEAN):
	$(MAKE) -C $($(subst _clean,,$@)_PATH) $($(subst _clean,,$@)_MAKEFILE) clean

# Nothing beyond this point

