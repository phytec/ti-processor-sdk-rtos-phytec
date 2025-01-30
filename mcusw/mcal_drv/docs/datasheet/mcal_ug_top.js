var mcal_ug_top =
[
    [ "Integrating MCAL Without Mcu and Port modules", "ug_no_mcu_port_top.html", [
      [ "Introduction", "ug_no_mcu_port_top.html#ug_no_muc_port_intro", null ],
      [ "Device Architecture", "ug_no_mcu_port_top.html#ug_no_muc_port_block", null ],
      [ "Device Architecture DMSC", "ug_no_mcu_port_top.html#ug_no_muc_port_arch_dmsc", null ],
      [ "AUTOSAR Mcu & Port Primary functionality", "ug_no_mcu_port_top.html#ug_no_muc_port_autosar_mcu_port", null ],
      [ "AUTOSAR Mapping Mcu Functionality", "ug_no_mcu_port_top.html#ug_no_muc_port_autosar_mcu_map", null ],
      [ "AUTOSAR Mapping Port Functionality", "ug_no_mcu_port_top.html#ug_no_muc_port_autosar_port_map", null ],
      [ "Document Revision History", "ug_no_mcu_port_top.html#ug_no_muc_port_rev_hist", null ]
    ] ],
    [ "Can User Guide", "ug_can_top.html", [
      [ "Introduction", "ug_can_top.html#ug_can_intro", null ],
      [ "CAN Driver Architecture/Design", "ug_can_top.html#ug_can_design", null ],
      [ "Functional Description", "ug_can_top.html#ug_can_functional_top", [
        [ "CAN Instance ID mapping and ISR mapping", "ug_can_top.html#ug_can_functional_id_mapping", null ],
        [ "Configuration", "ug_can_top.html#ug_can_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_can_top.html#ug_can_functional_cfg_v", [
            [ "CanWakeupFunctionalityAPI", "ug_can_top.html#ug_can_functional_cfg_v_wakeupfunctionality", null ],
            [ "CanWakeupSupport", "ug_can_top.html#ug_can_functional_cfg_v_wakeupsupport", null ],
            [ "CanIcomGeneral", "ug_can_top.html#ug_can_functional_cfg_v_icomgeneral", null ],
            [ "CanSupportTTCANRef", "ug_can_top.html#ug_can_functional_cfg_v_supportttcanref", null ],
            [ "CanTriggerTransmitEnable", "ug_can_top.html#ug_can_functional_cfg_v_triggertransmitenable", null ],
            [ "CanLPduReceiveCalloutFunction", "ug_can_top.html#ug_can_functional_cfg_v_lpdureceivecalloutfunction", null ],
            [ "CAN_CHANGE_BAUDRATE_API", "ug_can_top.html#ug_can_functional_cfg_v_canchangebaudrateapi", null ],
            [ "CanMainFunctionReadWriteApiPattern", "ug_can_top.html#ug_can_functional_i_canmainfunctionreadwriteapipattern", null ],
            [ "TimeoutInCanSetControllerMode", "ug_can_top.html#ug_can_functional_i_timeoutincansetcontrollermode", null ],
            [ "CanSetControllerModeWakeUp", "ug_can_top.html#ug_can_functional_i_cansetcontrollermodewakeup", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_can_top.html#ug_can_functional_i_cfg", null ],
          [ "Non Standard Service APIs", "ug_can_top.html#ug_can_functional_i_cfg_s_api_top", [
            [ "Can_RegisterReadback", "ug_can_top.html#ug_can_functional_i_cfg_s_api_rb", null ],
            [ "Can_TestLoopBackModeEnable", "ug_can_top.html#ug_can_functional_i_cfg_s_api_lpbck", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_can_top.html#ug_can_functional_cfg_int", null ],
        [ "Power-up", "ug_can_top.html#ug_can_functional_cfg_pwr", null ],
        [ "Build and Running the Application", "ug_can_top.html#ug_can_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_can_top.html#ug_can_functional_run_example", null ],
        [ "Memory Mapping", "ug_can_top.html#ug_can_functional_cfg_memmap", [
          [ "Cache", "ug_can_top.html#ug_can_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_can_top.html#ug_can_functional_dep_top", [
          [ "DET", "ug_can_top.html#ug_can_functional_dep_det", null ],
          [ "SchM", "ug_can_top.html#ug_can_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_can_top.html#ug_can_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_can_top.html#ug_can_error_top", [
        [ "Development Error Reporting", "ug_can_top.html#ug_can_error_dev", null ],
        [ "Error codes", "ug_can_top.html#ug_can_error_codes", null ],
        [ "Production Code Error Reporting", "ug_can_top.html#ug_can_error_prod", null ]
      ] ],
      [ "API Description", "ug_can_top.html#ug_can_api_top", null ],
      [ "Example Application", "ug_can_top.html#ug_can_eg_top", [
        [ "Example Log", "ug_can_top.html#ug_can_eg_log", [
          [ "DRA80x", "ug_can_top.html#ug_can_eg_log_dra80x", null ],
          [ "J721E", "ug_can_top.html#ug_can_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_can_top.html#ug_can_ref_top", null ],
      [ "Document Revision History", "ug_can_top.html#ug_can_rev_hist", null ]
    ] ],
    [ "Dio User Guide", "ug_dio_top.html", [
      [ "Introduction", "ug_dio_top.html#ug_dio_intro", null ],
      [ "Dio Driver Architecture/Design", "ug_dio_top.html#ug_dio_design", null ],
      [ "Functional Description", "ug_dio_top.html#ug_dio_functional_top", [
        [ "Pin Mapping DRA80x", "ug_dio_top.html#ug_dio_functional_pin_dra80x", null ],
        [ "Pin Mapping J721E", "ug_dio_top.html#ug_dio_functional_pin_j721e", null ],
        [ "Configuration", "ug_dio_top.html#ug_dio_functional_cfg", [
          [ "Variance from the specification", "ug_dio_top.html#ug_dio_functional_cfg_v", [
            [ "Direct Read Back", "ug_dio_top.html#ug_dio_functional_cfg_v_rb", null ],
            [ "Dio_ConfigType", "ug_dio_top.html#ug_dio_functional_cfg_v_pb", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_dio_top.html#ug_dio_functional_i_cfg", [
            [ "DioDeviceVariant", "ug_dio_top.html#ug_dio_functional_i_cfg_var", null ],
            [ "DIO_WRITE_PORT_EVENT_ID", "ug_dio_top.html#ug_dio_functional_i_cfg_var_demport", null ],
            [ "DIO_WRITE_CHANNEL_EVENT_ID", "ug_dio_top.html#ug_dio_functional_i_cfg_var_wr_ch", null ],
            [ "DioRegisterReadbackApi", "ug_dio_top.html#ug_dio_functional_i_cfg_s_api", null ]
          ] ],
          [ "Non Standard Service APIs", "ug_dio_top.html#ug_dio_functional_i_cfg_s_api_top", [
            [ "Dio_RegisterReadback", "ug_dio_top.html#ug_dio_functional_i_cfg_s_api_imp", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_dio_top.html#ug_dio_functional_cfg_int", null ],
        [ "Power-up", "ug_dio_top.html#ug_dio_functional_cfg_pwr", null ],
        [ "Build and Running the Application", "ug_dio_top.html#ug_dio_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_dio_top.html#ug_dio_functional_run_example", null ],
        [ "Memory Mapping", "ug_dio_top.html#ug_dio_functional_cfg_memmap", [
          [ "Cache", "ug_dio_top.html#ug_dio_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_dio_top.html#ug_dio_functional_dep_top", [
          [ "DET", "ug_dio_top.html#ug_dio_functional_dep_det", null ],
          [ "SchM", "ug_dio_top.html#ug_dio_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_dio_top.html#ug_dio_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_dio_top.html#ug_dio_error_top", [
        [ "Development Error Reporting", "ug_dio_top.html#ug_dio_error_dev", null ],
        [ "Error codes", "ug_dio_top.html#ug_dio_error_codes", null ],
        [ "Production Code Error Reporting", "ug_dio_top.html#ug_dio_error_prod", null ]
      ] ],
      [ "API Description", "ug_dio_top.html#ug_dio_api_top", null ],
      [ "Example Application", "ug_dio_top.html#ug_dio_eg_top", [
        [ "Example Log", "ug_dio_top.html#ug_dio_eg_log", [
          [ "DRA80x", "ug_dio_top.html#ug_dio_eg_log_dra80x", null ],
          [ "J721E", "ug_dio_top.html#ug_dio_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_dio_top.html#ug_dio_ref_top", null ],
      [ "Document Revision History", "ug_dio_top.html#ug_dio_rev_hist", null ]
    ] ],
    [ "Eth & EthTrcv User Guide", "ug_eth_top.html", [
      [ "Introduction", "ug_eth_top.html#ug_eth_intro", null ],
      [ "Eth Driver Architecture/Design", "ug_eth_top.html#ug_eth_design", null ],
      [ "Functional Description", "ug_eth_top.html#ug_eth_functional_top", [
        [ "Ethernet interrupt to ISR mapping", "ug_eth_top.html#ug_eth_functional_id_mapping", null ],
        [ "Configuration", "ug_eth_top.html#ug_eth_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_eth_top.html#ug_eth_functional_cfg_v", [
            [ "Ethernet Global Time APIs", "ug_eth_top.html#ug_eth_functional_cfg_v_globaltime", null ],
            [ "Ethernet Transceiver Wake-Up", "ug_eth_top.html#ug_eth_functional_cfg_v_wakeup", null ],
            [ "Ethernet Transceiver Manual/Auto-Negotiation Mode", "ug_eth_top.html#ug_eth_functional_cfg_v_autoneg", null ]
          ] ],
          [ "Ethernet Transceiver ECUC", "ug_eth_top.html#ug_eth_functional_i_cfg_ecuc", null ],
          [ "Non Standard Service APIs", "ug_eth_top.html#ug_eth_functional_non_std_api_top", null ]
        ] ],
        [ "Interrupt Configuration", "ug_eth_top.html#ug_eth_functional_cfg_int", null ],
        [ "Power-up", "ug_eth_top.html#ug_eth_functional_cfg_pwr", null ],
        [ "Build and Running the Application", "ug_eth_top.html#ug_eth_functional_cfg_build", [
          [ "Building the host-side example application", "ug_eth_top.html#ug_eth_functional_hostapp_build", null ],
          [ "Building the target-side example application in loopback mode", "ug_eth_top.html#ug_eth_functional_loopback_build", null ]
        ] ],
        [ "Steps to run example application", "ug_eth_top.html#ug_eth_functional_run_example", [
          [ "Running the loopback test", "ug_eth_top.html#ug_eth_function_run_loopback_example", null ],
          [ "Running the default (non-loopback) test", "ug_eth_top.html#ug_eth_function_run_default_example", null ]
        ] ],
        [ "Memory Mapping", "ug_eth_top.html#ug_eth_functional_cfg_memmap", [
          [ "Cache", "ug_eth_top.html#ug_eth_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_eth_top.html#ug_eth_functional_dep_top", [
          [ "DET", "ug_eth_top.html#ug_eth_functional_dep_det", null ],
          [ "SchM", "ug_eth_top.html#ug_eth_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_eth_top.html#ug_eth_functional_filestruct_top", [
          [ "Eth File Structure", "ug_eth_top.html#ug_eth_functional_eth_filestruct_top", null ],
          [ "EthTrcv File Structure", "ug_eth_top.html#ug_eth_functional_ethtrcv_filestruct_top", null ]
        ] ]
      ] ],
      [ "Error Handling", "ug_eth_top.html#ug_eth_error_top", [
        [ "Development Error Reporting", "ug_eth_top.html#ug_eth_error_dev", null ],
        [ "Error codes", "ug_eth_top.html#ug_eth_error_codes", [
          [ "Ethernet driver error codes", "ug_eth_top.html#ug_eth_drv_error_codes", null ],
          [ "Ethernet Transceiver driver error codes", "ug_eth_top.html#ug_ethtrcv_drv_error_codes", null ]
        ] ],
        [ "Production Code Error Reporting", "ug_eth_top.html#ug_eth_error_prod", null ]
      ] ],
      [ "API Description", "ug_eth_top.html#ug_eth_api_top", null ],
      [ "Example Application", "ug_eth_top.html#ug_eth_eg_top", [
        [ "Example Log", "ug_eth_top.html#ug_eth_eg_log", [
          [ "DRA80x", "ug_eth_top.html#ug_eth_eg_log_dra80x", null ],
          [ "J721E", "ug_eth_top.html#ug_eth_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_eth_top.html#ug_eth_ref_top", null ],
      [ "Document Revision History", "ug_eth_top.html#ug_eth_rev_hist", null ]
    ] ],
    [ "Gpt User Guide", "ug_gpt_top.html", [
      [ "Introduction", "ug_gpt_top.html#ug_gpt_intro", null ],
      [ "Gpt Driver Architecture/Design", "ug_gpt_top.html#ug_gpt_design", null ],
      [ "Functional Description", "ug_gpt_top.html#ug_gpt_functional_top", [
        [ "Clock Source to timers", "ug_gpt_top.html#ug_gpt_functional_clksrc", [
          [ "Computing the Gpt_ValueType Value for API Gpt_StartTimer ()", "ug_gpt_top.html#ug_gpt_functional_clkcompute", null ]
        ] ],
        [ "Timer Instance, Gpt Channel ID mapping and ISR mapping", "ug_gpt_top.html#ug_gpt_functional_id_mapping", null ],
        [ "Configuration", "ug_gpt_top.html#ug_gpt_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_gpt_top.html#ug_gpt_functional_cfg_v", [
            [ "PreDef Timers", "ug_gpt_top.html#ug_gpt_functional_cfg_v_predef", null ],
            [ "GptClockReference", "ug_gpt_top.html#ug_gpt_functional_cfg_v_clksrc", null ],
            [ "GptChannelTickFrequency", "ug_gpt_top.html#ug_gpt_functional_cfg_v_clkfreq", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_gpt_top.html#ug_gpt_functional_i_cfg", [
            [ "GptDeviceVariant", "ug_gpt_top.html#ug_gpt_functional_i_cfg_var", null ],
            [ "GptCategoryOfIsr", "ug_gpt_top.html#ug_gpt_functional_i_cfg_isr", null ],
            [ "GptDefaultOSCounterId", "ug_gpt_top.html#ug_gpt_functional_i_cfg_ctrid", null ],
            [ "GptRegisterReadbackApi", "ug_gpt_top.html#ug_gpt_functional_i_cfg_s_api", null ],
            [ "GptChannelPrescale", "ug_gpt_top.html#ug_gpt_functional_i_cfg_prescale", null ]
          ] ],
          [ "Non Standard Service APIs", "ug_gpt_top.html#ug_gpt_functional_i_cfg_s_api_top", [
            [ "Gpt_RegisterReadback", "ug_gpt_top.html#ug_gpt_functional_i_cfg_s_api_imp", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_gpt_top.html#ug_gpt_functional_cfg_int", null ],
        [ "Power-up", "ug_gpt_top.html#ug_gpt_functional_cfg_pwr", null ],
        [ "Build and Running the Example Application", "ug_gpt_top.html#ug_gpt_functional_cfg_build", null ],
        [ "Example Application", "ug_gpt_top.html#ug_gpt_eg_app", null ],
        [ "Steps to run example application", "ug_gpt_top.html#ug_gpt_functional_run_example", null ],
        [ "Memory Mapping", "ug_gpt_top.html#ug_gpt_functional_cfg_memmap", [
          [ "Cache", "ug_gpt_top.html#ug_gpt_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_gpt_top.html#ug_gpt_functional_dep_top", [
          [ "DET", "ug_gpt_top.html#ug_gpt_functional_dep_det", null ],
          [ "Module EcuM", "ug_gpt_top.html#ug_gpt_functional_dep_ecum", null ],
          [ "SchM", "ug_gpt_top.html#ug_gpt_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_gpt_top.html#ug_gpt_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_gpt_top.html#ug_gpt_error_top", [
        [ "Development Error Reporting", "ug_gpt_top.html#ug_gpt_error_dev", null ],
        [ "Error codes", "ug_gpt_top.html#ug_gpt_error_codes", null ],
        [ "Production Code Error Reporting", "ug_gpt_top.html#ug_gpt_error_prod", null ]
      ] ],
      [ "API Description", "ug_gpt_top.html#ug_gpt_api_top", null ],
      [ "Example Application", "ug_gpt_top.html#ug_gpt_eg_top", [
        [ "Example Log", "ug_gpt_top.html#ug_gpt_eg_log", [
          [ "DRA80x", "ug_gpt_top.html#ug_gpt_eg_log_dra80x", null ],
          [ "J721E", "ug_gpt_top.html#ug_gpt_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_gpt_top.html#ug_gpt_ref_top", null ],
      [ "Document Revision History", "ug_gpt_top.html#ug_gpt_rev_hist", null ]
    ] ],
    [ "Spi User Guide", "ug_spi_top.html", [
      [ "Introduction", "ug_spi_top.html#ug_spi_intro", null ],
      [ "Spi Driver Architecture/Design", "ug_spi_top.html#ug_spi_design", null ],
      [ "Functional Description", "ug_spi_top.html#ug_spi_functional_top", [
        [ "Spi Instance ID mapping and ISR mapping", "ug_spi_top.html#ug_spi_functional_id_mapping", null ],
        [ "Configuration", "ug_spi_top.html#ug_spi_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_spi_top.html#ug_spi_functional_cfg_v", [
            [ "SpiBaudrate", "ug_spi_top.html#ug_spi_functional_cfg_v_baudrate", null ],
            [ "SpiTimeClk2Cs", "ug_spi_top.html#ug_spi_functional_cfg_v_timeClk2Cs", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_spi_top.html#ug_spi_functional_i_cfg", null ],
          [ "Non Standard Service APIs", "ug_spi_top.html#ug_spi_functional_i_cfg_s_api_top", [
            [ "Spi_RegisterReadback", "ug_spi_top.html#ug_spi_functional_i_cfg_s_api_rb", null ],
            [ "Spi_SetLoopbackMode", "ug_spi_top.html#ug_spi_functional_i_cfg_s_api_lpbck", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_spi_top.html#ug_spi_functional_cfg_int", null ],
        [ "Power-up", "ug_spi_top.html#ug_spi_functional_cfg_pwr", null ],
        [ "Build and Running the Application", "ug_spi_top.html#ug_spi_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_spi_top.html#ug_spi_functional_run_example", null ],
        [ "Memory Mapping", "ug_spi_top.html#ug_spi_functional_cfg_memmap", [
          [ "Cache", "ug_spi_top.html#ug_spi_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_spi_top.html#ug_spi_functional_dep_top", [
          [ "DET", "ug_spi_top.html#ug_spi_functional_dep_det", null ],
          [ "SchM", "ug_spi_top.html#ug_spi_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_spi_top.html#ug_spi_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_spi_top.html#ug_spi_error_top", [
        [ "Development Error Reporting", "ug_spi_top.html#ug_spi_error_dev", null ],
        [ "Error codes", "ug_spi_top.html#ug_spi_error_codes", null ],
        [ "Production Code Error Reporting", "ug_spi_top.html#ug_spi_error_prod", null ]
      ] ],
      [ "API Description", "ug_spi_top.html#ug_spi_api_top", null ],
      [ "Example Application", "ug_spi_top.html#ug_spi_eg_top", [
        [ "Example Log", "ug_spi_top.html#ug_spi_eg_log", [
          [ "DRA80x", "ug_spi_top.html#ug_spi_eg_log_dra80x", null ],
          [ "J721E", "ug_spi_top.html#ug_spi_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_spi_top.html#ug_spi_ref_top", null ],
      [ "Document Revision History", "ug_spi_top.html#ug_spi_rev_hist", null ]
    ] ],
    [ "Wdg User Guide", "ug_wdg_top.html", [
      [ "Introduction", "ug_wdg_top.html#ug_wdg_intro", null ],
      [ "Wdg Driver Architecture/Design", "ug_wdg_top.html#ug_wdg_design", null ],
      [ "Functional Description", "ug_wdg_top.html#ug_wdg_functional_top", [
        [ "Clock Source to RTI", "ug_wdg_top.html#ug_wdg_functional_clksrc", null ],
        [ "RTI Instance", "ug_wdg_top.html#ug_wdg_instances_supported", null ],
        [ "Configuration", "ug_wdg_top.html#ug_wdg_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_wdg_top.html#ug_wdg_functional_cfg_v", [
            [ "Wdg_SetMode API", "ug_wdg_top.html#ug_wdg_functional_cfg_v_setmode", null ],
            [ "WdgOffMode", "ug_wdg_top.html#ug_wdg_functional_cfg_v_offmode", null ],
            [ "WdgServiceInterruptRoutine", "ug_wdg_top.html#ug_wdg_functional_cfg_v_service", null ],
            [ "WdgExternalDriver", "ug_wdg_top.html#ug_wdg_functional_cfg_v_external", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_wdg_top.html#ug_wdg_functional_i_cfg", [
            [ "WdgDeviceVariant", "ug_wdg_top.html#ug_wdg_functional_i_cfg_variant", null ],
            [ "WdgRtiFrequency", "ug_wdg_top.html#ug_wdg_functional_i_cfg_rtifrequency", null ],
            [ "WdgRegisterReadbackApi", "ug_wdg_top.html#ug_wdg_functional_i_cfg_regreadback", null ],
            [ "WdgInstanceId", "ug_wdg_top.html#ug_wdg_functional_i_cfg_s_wdginstanceid", null ],
            [ "WdgReaction", "ug_wdg_top.html#ug_wdg_functional_i_cfg_reaction", null ],
            [ "WdgWindowSize", "ug_wdg_top.html#ug_wdg_functional_i_cfg_windowsize", null ],
            [ "WdgTimeoutValue", "ug_wdg_top.html#ug_wdg_functional_i_cfg_timeout", null ]
          ] ],
          [ "Non Standard Service APIs", "ug_wdg_top.html#ug_wdg_functional_i_cfg_s_api_top", [
            [ "Wdg_trigger", "ug_wdg_top.html#ug_wdg_functional_i_cfg_s_api_trigger", null ],
            [ "Wdg_RegisterReadback", "ug_wdg_top.html#ug_wdg_functional_i_cfg_s_api_readback", null ]
          ] ]
        ] ],
        [ "Power-up", "ug_wdg_top.html#ug_wdg_functional_cfg_pwr", null ],
        [ "Build and Running the Example Application", "ug_wdg_top.html#ug_wdg_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_wdg_top.html#ug_wdg_functional_run_example", null ],
        [ "Memory Mapping", "ug_wdg_top.html#ug_wdg_functional_cfg_memmap", [
          [ "Cache", "ug_wdg_top.html#ug_wdg_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_wdg_top.html#ug_wdg_functional_dep_top", [
          [ "ESM", "ug_wdg_top.html#ug_wdg_functional_dep_esm", null ],
          [ "DET", "ug_wdg_top.html#ug_wdg_functional_dep_det", null ],
          [ "DEM", "ug_wdg_top.html#ug_wdg_functional_dep_dem", null ]
        ] ],
        [ "File Structure", "ug_wdg_top.html#ug_wdg_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_wdg_top.html#ug_wdg_error_top", [
        [ "Development Error Reporting", "ug_wdg_top.html#ug_wdg_error_dev", null ],
        [ "Error codes", "ug_wdg_top.html#ug_wdg_error_codes", null ],
        [ "Production Code Error Reporting", "ug_wdg_top.html#ug_wdg_error_dem", null ],
        [ "DEM Error codes", "ug_wdg_top.html#ug_wdg_dem_error_codes", null ]
      ] ],
      [ "API Description", "ug_wdg_top.html#ug_wdg_api_top", null ],
      [ "Example Application", "ug_wdg_top.html#ug_wdg_eg_top", [
        [ "Example Log", "ug_wdg_top.html#ug_wdg_eg_log", [
          [ "DRA80x", "ug_wdg_top.html#ug_wdg_eg_log_dra80x", null ],
          [ "J721E", "ug_wdg_top.html#ug_wdg_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_wdg_top.html#ug_wdg_ref_top", null ],
      [ "Document Revision History", "ug_wdg_top.html#ug_wdg_rev_hist", null ]
    ] ],
    [ "Cdd Ipc User Guide", "ug_cdd_ipc_top.html", [
      [ "Introduction", "ug_cdd_ipc_top.html#ug_cdd_ipc_intro", null ],
      [ "Cdd Driver Architecture/Design", "ug_cdd_ipc_top.html#ug_cdd_ipc_design", null ],
      [ "Functional Description", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_top", [
        [ "Communication Channel", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_ch", [
          [ "End Point", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_ch_ep", null ],
          [ "Buffer for each channel", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_ch_buf", null ]
        ] ],
        [ "Control End Point", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_ch_ctrl_ep", null ],
        [ "Interrupt to ISR mapping", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_id_mapping", null ],
        [ "Configuration", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg", null ],
        [ "Power-up", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg_pwr", null ],
        [ "Build and Running the Application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg_build", [
          [ "Building the remote core example application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_remoteapp_build", [
            [ "DRA80X", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_remoteapp_build_dra80x", null ],
            [ "J721E", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_remoteapp_build_j721e", null ]
          ] ],
          [ "Building the MCAL IPC profiling application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_profileapp_build", null ],
          [ "Steps to run example application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_run_example", [
            [ "Running the example application via CCS", "ug_cdd_ipc_top.html#ug_cdd_ipc_function_run_eg", null ]
          ] ]
        ] ],
        [ "Memory Mapping", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg_memmap", null ],
        [ "Dependencies on SW Modules", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_dep_top", [
          [ "DET", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_dep_det", null ],
          [ "SchM", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_filestruct_top", [
          [ "Cdd Ipc File Structure", "ug_cdd_ipc_top.html#ug_cdd_ipc_cdd_ipc_filestruct_top", null ]
        ] ]
      ] ],
      [ "Customizing Examples Application", "ug_cdd_ipc_top.html#ug_cdd_ipc_app_custom_top", [
        [ "Turn OFF Use Of Control End Point", "ug_cdd_ipc_top.html#ug_cdd_ipc_app_custom_no_ctrlep", null ]
      ] ],
      [ "Error Handling", "ug_cdd_ipc_top.html#ug_cdd_ipc_error_top", [
        [ "Development Error Reporting", "ug_cdd_ipc_top.html#ug_cdd_ipc_error_dev", null ],
        [ "Error codes", "ug_cdd_ipc_top.html#ug_cdd_ipc_error_codes", null ]
      ] ],
      [ "API Description", "ug_cdd_ipc_top.html#ug_cdd_ipc_api_top", null ],
      [ "Example Application", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_top", [
        [ "Flow Chart", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_top_flow_chart", null ],
        [ "Example Logs", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_logs", [
          [ "DRA80X", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_log_dra80x", null ],
          [ "J721E MCU 1 0", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_log_j721e_mcu10", null ],
          [ "J721E MCU 2 1", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_log_j721e_mcu21", null ]
        ] ]
      ] ],
      [ "References", "ug_cdd_ipc_top.html#ug_cdd_ipc_ref_top", null ],
      [ "Document Revision History", "ug_cdd_ipc_top.html#ug_cdd_ipc_rev_hist", null ]
    ] ],
    [ "Pwm User Guide", "ug_pwm_top.html", [
      [ "Introduction", "ug_pwm_top.html#ug_pwm_intro", null ],
      [ "Pwm Driver Architecture/Design", "ug_pwm_top.html#ug_pwm_design", null ],
      [ "Functional Description", "ug_pwm_top.html#ug_pwm_functional_top", [
        [ "Clock Source to timers", "ug_pwm_top.html#ug_pwm_functional_clksrc", null ],
        [ "Pwm Channel ID , Instance mapping and ISR mapping", "ug_pwm_top.html#ug_pwm_functional_id_mapping", null ],
        [ "Configuration", "ug_pwm_top.html#ug_pwm_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_pwm_top.html#ug_pwm_functional_cfg_v", [
            [ "APIs related to power state", "ug_pwm_top.html#ug_pwm_functional_cfg_v_powerstate", null ],
            [ "PwmMcuClockReference", "ug_pwm_top.html#ug_pwm_functional_cfg_v_clkref", null ],
            [ "PWM_FIXED_PERIOD_SHIFTED", "ug_pwm_top.html#ug_pwm_functional_cfg_v_fixedperiodshifted", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_pwm_top.html#ug_pwm_functional_i_cfg", [
            [ "PwmDeviceVariant", "ug_pwm_top.html#ug_pwm_functional_i_cfg_var", null ],
            [ "PwmTypeofInterruptFunction", "ug_pwm_top.html#ug_pwm_functional_i_cfg_isr", null ],
            [ "PwmDefaultOSCounterId", "ug_pwm_top.html#ug_pwm_functional_i_cfg_ctrid", null ],
            [ "PwmEnableRegisterReadbackApi", "ug_pwm_top.html#ug_pwm_functional_i_cfg_s_api", null ],
            [ "PwmClkPrescaler", "ug_pwm_top.html#ug_pwm_functional_i_cfg_prescale", null ],
            [ "PwmPolarity", "ug_pwm_top.html#ug_pwm_functional_i_cfg_polarity", null ],
            [ "PwmIdleState", "ug_pwm_top.html#ug_pwm_functional_i_cfg_Idlestate", null ]
          ] ],
          [ "Non Standard Service APIs", "ug_pwm_top.html#ug_pwm_functional_i_cfg_s_api_top", [
            [ "Pwm_RegisterReadback", "ug_pwm_top.html#ug_pwm_functional_i_cfg_s_api_imp", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_pwm_top.html#ug_pwm_functional_cfg_int", null ],
        [ "Power-up", "ug_pwm_top.html#ug_pwm_functional_cfg_pwr", null ],
        [ "Build and Running the Example Application", "ug_pwm_top.html#ug_pwm_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_pwm_top.html#ug_pwm_functional_run_example", null ],
        [ "Memory Mapping", "ug_pwm_top.html#ug_pwm_functional_cfg_memmap", [
          [ "Cache", "ug_pwm_top.html#ug_pwm_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_pwm_top.html#ug_pwm_functional_dep_top", [
          [ "DET", "ug_pwm_top.html#ug_pwm_functional_dep_det", null ],
          [ "SchM", "ug_pwm_top.html#ug_pwm_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_pwm_top.html#ug_pwm_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_pwm_top.html#ug_pwm_error_top", [
        [ "Development Error Reporting", "ug_pwm_top.html#ug_pwm_error_dev", null ],
        [ "Error codes", "ug_pwm_top.html#ug_pwm_error_codes", null ]
      ] ],
      [ "API Description", "ug_pwm_top.html#ug_pwm_api_top", null ],
      [ "Example Application", "ug_pwm_top.html#ug_pwm_eg_top", [
        [ "Example Log", "ug_pwm_top.html#ug_pwm_eg_log", [
          [ "DRA80X", "ug_pwm_top.html#ug_pwm_eg_log_dra80x", null ],
          [ "J721E", "ug_pwm_top.html#ug_pwm_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_pwm_top.html#ug_pwm_ref_top", null ],
      [ "Document Revision History", "ug_pwm_top.html#ug_pwm_rev_hist", null ]
    ] ],
    [ "Adc User Guide", "ug_adc_top.html", [
      [ "Introduction", "ug_adc_top.html#ug_adc_intro", null ],
      [ "Adc Driver Architecture/Design", "ug_adc_top.html#ug_adc_design", null ],
      [ "Functional Description", "ug_adc_top.html#ug_adc_functional_top", [
        [ "Clock Source to adc", "ug_adc_top.html#ug_adc_functional_clksrc", null ],
        [ "Adc Sample output", "ug_adc_top.html#ug_adc_functional_sampleout", null ],
        [ "Adc Instance, Adc Hw Unit ID mapping and ISR mapping", "ug_adc_top.html#ug_adc_functional_id_mapping", null ],
        [ "Configuration", "ug_adc_top.html#ug_adc_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_adc_top.html#ug_adc_functional_cfg_v", [
            [ "AdcClockReference", "ug_adc_top.html#ug_adc_functional_cfg_v_clksrc", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_adc_top.html#ug_adc_functional_i_cfg", null ],
          [ "Compile Time Configurations for Performance", "ug_adc_top.html#ug_adc_functional_profile_cfg", null ],
          [ "Non Standard Service APIs", "ug_adc_top.html#ug_adc_functional_i_cfg_s_api_top", [
            [ "Adc_RegisterReadback", "ug_adc_top.html#ug_adc_functional_i_cfg_s_api_imp", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_adc_top.html#ug_adc_functional_cfg_int", null ],
        [ "Power-up", "ug_adc_top.html#ug_adc_functional_cfg_pwr", null ],
        [ "Build and Running the Example Application", "ug_adc_top.html#ug_adc_functional_cfg_build", null ],
        [ "Example Application", "ug_adc_top.html#ug_adc_eg_app", null ],
        [ "Steps to run example application", "ug_adc_top.html#ug_adc_functional_run_example", null ],
        [ "Memory Mapping", "ug_adc_top.html#ug_adc_functional_cfg_memmap", [
          [ "Cache", "ug_adc_top.html#ug_adc_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_adc_top.html#ug_adc_functional_dep_top", [
          [ "DET", "ug_adc_top.html#ug_adc_functional_dep_det", null ],
          [ "SchM", "ug_adc_top.html#ug_adc_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_adc_top.html#ug_adc_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_adc_top.html#ug_adc_error_top", [
        [ "Development Error Reporting", "ug_adc_top.html#ug_adc_error_dev", null ],
        [ "Error codes", "ug_adc_top.html#ug_adc_error_codes", null ],
        [ "Production Code Error Reporting", "ug_adc_top.html#ug_adc_error_prod", null ]
      ] ],
      [ "API Description", "ug_adc_top.html#ug_adc_api_top", null ],
      [ "Example Application", "ug_adc_top.html#ug_adc_eg_top", [
        [ "Example Log", "ug_adc_top.html#ug_adc_eg_log", [
          [ "DRA80x", "ug_adc_top.html#ug_adc_eg_log_dra80x", null ],
          [ "J721E", "ug_adc_top.html#ug_adc_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_adc_top.html#ug_adc_ref_top", null ],
      [ "Document Revision History", "ug_adc_top.html#ug_adc_rev_hist", null ]
    ] ]
];