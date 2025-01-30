
include Rules.make

BOARD_LIST_ALL =
BOARD_LIST_ALL += j721e_sim j721e_evm j7200_evm j721s2_evm j784s4_evm am62x_evm am62ax_evm am62px_evm j722s_evm
CORE_LIST_ALL =
CORE_LIST_ALL += mcu1_0 mcu1_1 mcu0_0
BUILD_PROFILE_LIST_ALL = release debug
BUILD_VARIANT_LIST_ALL = precompile linktime postbuild
FLS_PROTOCOL_LIST_ALL = ospi qspi

mcusw_LIB_LIST_CLEAN = $(addsuffix _clean, $(mcusw_LIB_LIST))
mcusw_APP_LIB_LIST_CLEAN = $(addsuffix _clean, $(mcusw_APP_LIB_LIST))
mcusw_EXAMPLE_LIST_CLEAN = $(addsuffix _clean, $(mcusw_EXAMPLE_LIST))
mcusw_test_EXAMPLE_LIST_CLEAN = $(addsuffix _clean, $(mcusw_test_EXAMPLE_LIST))
mcusw_DUP_EXAMPLE_LIST_CLEAN = $(addsuffix _clean, $(mcusw_DUP_EXAMPLE_LIST))
mcusw_PKG_LIST_ALL_CLEAN = $(addsuffix _clean, $(mcusw_PKG_LIST_ALL))
mcusw_PKG_LIST_PACKAGE = $(addsuffix _package, $(mcusw_PKG_LIST_ALL))

.PHONY : all testall mcusw_libs mcusw_libs_clean mcusw_app_libs mcusw_app_libs_clean examples examples_clean clean package $(mcusw_PKG_LIST_ALL) $(mcusw_DUP_EXAMPLE_LIST) depend depend_clean $(mcusw_pdk_LIB_LIST) sbl sbl_mmcsd_img sbl_ospi_img sbl_cust_img sbl_clean

depend: pdk

depend_clean: pdk_clean

all_libs: version depend mcusw_libs mcusw_app_libs

all: all_libs
	$(MAKE) examples

testall: all_libs
	$(MAKE) unit_test

version:
	$(ECHO) ------------------------------------------------------
	$(ECHO) \# MCUSW 09.02.00
	$(ECHO) ------------------------------------------------------

mcusw_libs: $(mcusw_LIB_LIST)

mcusw_libs_clean: $(mcusw_LIB_LIST_CLEAN)

mcusw_app_libs: $(mcusw_APP_LIB_LIST)

mcusw_app_libs_clean: $(mcusw_APP_LIB_LIST_CLEAN)

examples: depend $(mcusw_EXAMPLE_LIST)

unit_test: depend $(mcusw_test_EXAMPLE_LIST)

examples_clean: $(mcusw_EXAMPLE_LIST_CLEAN)

unit_test_clean: $(mcusw_test_EXAMPLE_LIST_CLEAN)

clean: mcusw_libs_clean mcusw_app_libs_clean examples_clean pdk_clean
	$(foreach dup_app, $(mcusw_DUP_EXAMPLE_LIST_CLEAN),\
	$(MAKE) $(dup_app);\
	)

allclean: destroot_clean all_libs_clean

destroot_clean:
ifneq ($(DEST_ROOT),)
	$(RM) -rf $(DEST_ROOT)
endif

all_libs_clean:
	$(RM) -rf $(MCUSW_INSTALL_PATH)/*/*/*/lib

package: $(mcusw_PKG_LIST_PACKAGE)

help:
	$(ECHO) ------------------------------------------------------
	$(ECHO) \# MCUSW make help
	$(ECHO) ------------------------------------------------------
	$(ECHO) make -s [OPTIONAL MAKE VARIABLES] Note: use gmake for windows
	$(ECHO)
	$(ECHO) "Supported targets:"
	$(ECHO) "------------------"
	$(ECHO) "all            : Builds all libraries and examples for the provided CORE and BOARD"
	$(ECHO) "allcores       : Builds all libraries and examples for all applicable CORES for a BOARD"
	$(ECHO) "clean          : Cleans all libraries and examples for the provided CORE and BOARD"
	$(ECHO) "allcores_clean : Cleans all libraries and examples for all applicable CORES for a BOARD"
	$(ECHO) "allclean       : Removes the binary directory using rm -rf"
	$(ECHO) "examples       : Builds all examples"
	$(ECHO) "mcusw_libs       : Builds all libraries"
	$(ECHO) "mcusw_app_libs   : Builds all application utility libaries"
	$(ECHO) "<Module>       : Builds a module. Possible values:"
	$(ECHO) "[$(mcusw_LIB_LIST)]"
	$(ECHO) ""
	$(ECHO) "<Module_App>   : Builds an application. Possible values:"
	$(ECHO) "[$(mcusw_EXAMPLE_LIST) $(mcusw_DUP_EXAMPLE_LIST)]"
	$(ECHO) ""
	$(ECHO) "Optional make variables:"
	$(ECHO) "------------------------"
	$(ECHO) "BOARD=[$(BOARD_LIST_ALL)]"
	$(ECHO) "    Default: $(BOARD)"
	$(ECHO) "CORE=[$(CORE_LIST_ALL)]"
	$(ECHO) "    Default: Depends on the platform"
	$(ECHO) "BUILD_PROFILE=[$(BUILD_PROFILE_LIST_ALL)]"
	$(ECHO) "    Default: $(BUILD_PROFILE)"
	$(ECHO) "BUILD_VARIANT=[$(BUILD_VARIANT_LIST_ALL)]"
	$(ECHO) "    Default: $(BUILD_VARIANT)"
	$(ECHO) "OS=[Windows_NT linux]"
	$(ECHO) "    Default: Windows_NT"
	$(ECHO) "FLS_PROTOCOL=[$(FLS_PROTOCOL_LIST_ALL)]"
	$(ECHO) "    Default: $(FLS_PROTOCOL)"

allcores:
	$(foreach current_core, $(CORE_LIST_ALL),\
	$(MAKE) all CORE=$(current_core) BOARD=$(BOARD) BUILD_PROFILE_$(current_core)=$(BUILD_PROFILE);\
	)

allcores_clean:
	$(foreach current_core, $(CORE_LIST_ALL),\
	$(MAKE) clean CORE=$(current_core) BOARD=$(BOARD) BUILD_PROFILE_$(current_core)=$(BUILD_PROFILE);\
	)

allboards:
	$(foreach current_board, $(BOARD_LIST_ALL),\
	$(MAKE) allcores BOARD=$(current_board) BUILD_PROFILE=$(BUILD_PROFILE);\
	)

allboards_clean:
	$(foreach current_board, $(BOARD_LIST_ALL),\
	$(MAKE) allcores_clean BOARD=$(current_board) BUILD_PROFILE=$(BUILD_PROFILE);\
	)

profiles:
	$(foreach current_profile, $(BUILD_PROFILE_LIST_ALL),\
	$(MAKE) allcores BOARD=$(BOARD) BUILD_PROFILE=$(current_profile);\
	)

profiles_clean:
	$(foreach current_profile, $(BUILD_PROFILE_LIST_ALL),\
	$(MAKE) allcores_clean BOARD=$(BOARD) BUILD_PROFILE=$(current_profile);\
	)

allall:
	$(foreach current_board, $(BOARD_LIST_ALL),\
	$(MAKE) profiles BOARD=$(current_board);\
	)

allall_clean:
	$(foreach current_board, $(BOARD_LIST_ALL),\
	$(MAKE) profiles_clean BOARD=$(current_board);\
	)
#=================================================================
	#These targets are for use with Klockwork only
kwadc:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(adc_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(adc_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(adc_PATH) $($adc_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) adc))
kwcan:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(can_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(can_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(can_PATH) $(can_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) can))
kwcdd_ipc:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(cdd_ipc_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(cdd_ipc_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(cdd_ipc_PATH) $(cdd_ipc_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) cdd_ipc))
kwdio:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(dio_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(dio_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(dio_PATH) $(dio_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) dio))
kweth:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(eth_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(eth_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(eth_PATH) $(eth_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) eth))
kwethtrcv:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(ethtrcv_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(ethtrcv_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(ethtrcv_PATH) $(ethtrcv_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) ethtrcv))
kwfls:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(fls_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(fls_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(fls_PATH) $(fls_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) fls))
kwgpt:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(gpt_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(gpt_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(gpt_PATH) $(gpt_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) gpt))
kwicu:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(icu_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(icu_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(icu_PATH) $(icu_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) icu))
kwpwm:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(pwm_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(pwm_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(pwm_PATH) $(pwm_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) pwm))
kwspi:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(spi_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(spi_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(spi_PATH) $(spi_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) spi))
kwwdg:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(spi_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(wdg_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(wdg_PATH) $(wdg_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) wdg))
kwmcu:
		$(if $(filter $(SOC), $(subst emptyreplacement,,$(mcu_SOCLIST))),\
			$(if $(filter $(CORE), $(subst emptyreplacement,,$(mcu_$(SOC)_CORELIST))),\
	            $(MAKE) -C $(mcu_PATH) $(mcu_MAKEFILE),$(ECHO) Nothing to be done for $(SOC) $(CORE) mcu))

#=================================================================
#MCUSWs libs and tests
$(mcusw_PKG_LIST_ALL) $(mcusw_DUP_EXAMPLE_LIST): depend
	$(if $(filter $(SOC), $(subst emptyreplacement,,$($@_SOCLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(if $(filter yes, $(subst emptyreplacement,,$($@_XDC_CONFIGURO))),\
	            $(MAKE) -C $($@_PATH) $($@_MAKEFILE) xdc_configuro,),),\
	$(if $(filter $(BOARD), $(subst emptyreplacement,,$($@_BOARDLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(if $(filter yes, $(subst emptyreplacement,,$($@_XDC_CONFIGURO))),\
	            $(MAKE) -C $($@_PATH) $($@_MAKEFILE) xdc_configuro,),),))
	$(if $(filter $(SOC), $(subst emptyreplacement,,$($@_SOCLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(MAKE) -C $($@_PATH) $($@_MAKEFILE),$(ECHO) Nothing to be Done for $(SOC) $(CORE) $@),\
	$(if $(filter $(BOARD), $(subst emptyreplacement,,$($@_BOARDLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(MAKE) -C $($@_PATH) $($@_MAKEFILE),$(ECHO) Nothing to be Done for $(BOARD) $(SOC) $(CORE) $@),$(ECHO) Nothing to be Done for $(SOC) $@  $($@_BOARDLIST)))
	$(if $(filter $(SOC), $(subst emptyreplacement,,$($@_SOCLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(if $(filter yes, $(subst emptyreplacement,,$($@_SBL_IMAGEGEN))),\
	            $(MAKE) -C $($@_PATH) $($@_MAKEFILE) sbl_imagegen,),),\
	$(if $(filter $(BOARD), $(subst emptyreplacement,,$($@_BOARDLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(if $(filter yes, $(subst emptyreplacement,,$($@_SBL_IMAGEGEN))),\
	            $(MAKE) -C $($@_PATH) $($@_MAKEFILE) sbl_imagegen,),),))
	$(if $(filter $(SOC), $(subst emptyreplacement,,$($@_SOCLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(if $(filter yes, $(subst emptyreplacement,,$($@_SBL_APPIMAGEGEN))),\
	            $(MAKE) -C $($@_PATH) $($@_MAKEFILE) sbl_appimagegen,),),\
	$(if $(filter $(BOARD), $(subst emptyreplacement,,$($@_BOARDLIST))),\
	    $(if $(filter $(CORE), $(subst emptyreplacement,,$($@_$(SOC)_CORELIST))),\
	        $(if $(filter yes, $(subst emptyreplacement,,$($@_SBL_APPIMAGEGEN))),\
	            $(MAKE) -C $($@_PATH) $($@_MAKEFILE) sbl_appimagegen,),),))

$(mcusw_PKG_LIST_ALL_CLEAN) $(mcusw_DUP_EXAMPLE_LIST_CLEAN):
	$(if $(filter $(SOC), $(subst emptyreplacement,,$($(subst _clean,,$@)_SOCLIST))),\
	        $(MAKE) -C $($(subst _clean,,$@)_PATH) $($(subst _clean,,$@)_MAKEFILE) clean,\
	$(if $(filter $(BOARD), $(subst emptyreplacement,,$($(subst _clean,,$@)_BOARDLIST))),\
	        $(MAKE) -C $($(subst _clean,,$@)_PATH) $($(subst _clean,,$@)_MAKEFILE) clean,))

$(mcusw_PKG_LIST_PACKAGE):
	$(if $(filter $(SOC), $(subst emptyreplacement,,$($(subst _package,,$@)_SOCLIST))),\
	        $(MAKE) -C $($(subst _package,,$@)_PATH) $($(subst _package,,$@)_MAKEFILE) package,\
	$(if $(filter $(BOARD), $(subst emptyreplacement,,$($(subst _package,,$@)_BOARDLIST))),\
	        $(MAKE) -C $($(subst _package,,$@)_PATH) $($(subst _package,,$@)_MAKEFILE) package,$(ECHO) Nothing to be Done for $(SOC) $(subst _package,,$@)))

#Below is used only for checking c++ build errors during development, not to be used for any other purpose
cplusplus_build:
	$(MAKE) all BUILD_PROFILE=debug CPLUSPLUS_BUILD=yes

# PDK Build Targets
PDK_BUILD_OPTIONS = MAKERULEDIR=$(PDK_INSTALL_PATH)/ti/build/makerules PDK_INSTALL_PATH=$(PDK_INSTALL_PATH) BUILD_OS_TYPE=$(BUILD_OS_TYPE) UTILS_INSTALL_DIR=$(utils_PATH)
PDK_BUILD_OPTIONS += TOOLCHAIN_PATH_R5=$(TOOLCHAIN_PATH_R5)
PDK_BUILD_OPTIONS += BOARD=$(BOARD) CORE=$(CORE) BUILD_PROFILE=$(BUILD_PROFILE)
PDK_BUILD_OPTIONS += XDC_INSTALL_PATH=$(XDC_INSTALL_PATH)
PDK_BUILD_OPTIONS += SDK_INSTALL_PATH=$(SDK_INSTALL_PATH)

mcusw_pdk_LIB_LIST_CLEAN = $(addsuffix _clean, $(mcusw_pdk_LIB_LIST))

pdk: $(mcusw_pdk_LIB_LIST)

pdk_clean: $(mcusw_pdk_LIB_LIST_CLEAN)

pdk_allclean:
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build allclean $(PDK_BUILD_OPTIONS)

$(mcusw_pdk_LIB_LIST) $(mcusw_pdk_LIB_LIST_CLEAN):
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build $@ $(PDK_BUILD_OPTIONS)

sbl: sbl_mmcsd_img sbl_ospi_img sbl_cust_img

sbl_mmcsd_img:
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build sbl_mmcsd_img $(PDK_BUILD_OPTIONS)

sbl_ospi_img:
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build sbl_ospi_img $(PDK_BUILD_OPTIONS)

sbl_cust_img:
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build sbl_cust_img $(PDK_BUILD_OPTIONS)

sbl_clean:
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build sbl_mmcsd_img_clean $(PDK_BUILD_OPTIONS)
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build sbl_ospi_img_clean $(PDK_BUILD_OPTIONS)
	$(MAKE) -C $(PDK_INSTALL_PATH)/ti/build sbl_cust_img_clean $(PDK_BUILD_OPTIONS)

# Nothing beyond this point
