var mcal_ug_top =
[
    [ "Integrating MCAL Without Port modules", "ug_no_mcu_port_top.html", [
      [ "Introduction", "ug_no_mcu_port_top.html#ug_no_muc_port_intro", null ],
      [ "Device Architecture", "ug_no_mcu_port_top.html#ug_no_muc_port_block", null ],
      [ "Device Architecture DMSC", "ug_no_mcu_port_top.html#ug_no_muc_port_arch_dmsc", null ],
      [ "AUTOSAR Port Primary functionality", "ug_no_mcu_port_top.html#ug_no_muc_port_autosar_mcu_port", null ],
      [ "AUTOSAR Mapping Port Functionality", "ug_no_mcu_port_top.html#ug_no_muc_port_autosar_port_map", null ]
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
        [ "Sample Example Log", "ug_can_top.html#ug_can_eg_log", null ]
      ] ],
      [ "References", "ug_can_top.html#ug_can_ref_top", null ]
    ] ],
    [ "Dio User Guide", "ug_dio_top.html", [
      [ "Introduction", "ug_dio_top.html#ug_dio_intro", null ],
      [ "Dio Driver Architecture/Design", "ug_dio_top.html#ug_dio_design", null ],
      [ "Functional Description", "ug_dio_top.html#ug_dio_functional_top", [
        [ "Pin Mapping for SOCs", "ug_dio_top.html#ug_dio_functional_pin_SOCs", [
          [ "Pin Mapping J721E and J7200", "ug_dio_top.html#ug_dio_functional_pin_j721e_j7200", null ],
          [ "Pin Mapping AM62X/AM62AX/AM62PX/J722S", "ug_dio_top.html#ug_dio_functional_pin_SMPU", null ]
        ] ],
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
      [ "Safety Diagnostic Features", "ug_dio_top.html#ug_dio_safety_diagnostic_top", [
        [ "GPIO1 - Software test of function using I/O loopback", "ug_dio_top.html#ug_dio_loopback", null ],
        [ "GPIO3 - Periodic Software Readback of Static Configuration Registers / GPIO4 - Software Readback of Written Configuration", "ug_dio_top.html#ug_dio_readback", null ]
      ] ],
      [ "API Description", "ug_dio_top.html#ug_dio_api_top", null ],
      [ "Example Application", "ug_dio_top.html#ug_dio_eg_top", [
        [ "Example Log", "ug_dio_top.html#ug_dio_eg_log", null ]
      ] ],
      [ "References", "ug_dio_top.html#ug_dio_ref_top", null ]
    ] ],
    [ "Eth & EthTrcv User Guide", "ug_eth_top.html", [
      [ "Introduction", "ug_eth_top.html#ug_eth_intro", null ],
      [ "Supported SoCs", "ug_eth_top.html#ug_eth_supported_socs", null ],
      [ "Eth Driver Architecture/Design", "ug_eth_top.html#ug_eth_design", null ],
      [ "Functional Description", "ug_eth_top.html#ug_eth_functional_top", [
        [ "Ethernet interrupt to ISR mapping", "ug_eth_top.html#ug_eth_functional_id_mapping", null ],
        [ "Configuration", "ug_eth_top.html#ug_eth_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_eth_top.html#ug_eth_functional_cfg_v", [
            [ "Ethernet Switch Interface", "ug_eth_top.html#ug_eth_functional_cfg_ethswt", null ],
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
          [ "J721E/J7200", "ug_eth_top.html#ug_eth_eg_log_j721e", null ],
          [ "AM62x/AM62Ax/AM62Px", "ug_eth_top.html#ug_eth_eg_log_am62x", null ]
        ] ]
      ] ],
      [ "Known Limitations", "ug_eth_top.html#ug_eth_known_limitations", null ],
      [ "References", "ug_eth_top.html#ug_eth_ref_top", null ],
      [ "Document Revision History", "ug_eth_top.html#ug_eth_rev_hist", null ]
    ] ],
    [ "Eth Virtual Mac User Guide", "ug_ethvirtmac_top.html", [
      [ "Introduction", "ug_ethvirtmac_top.html#ug_ethvirtmac_intro", null ],
      [ "Supported SoCs", "ug_ethvirtmac_top.html#ug_ethvirtmac_supported_socs", null ],
      [ "Virtual MAC basics", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics", [
        [ "J721E and J7200 Integrated Ethernet Switch Overview", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics_cpswintro", null ],
        [ "J7 Multi core data path", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics_j7_multicore_data_path", null ],
        [ "Master Core and Virtual MAC concept", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics_master_core_virtual_mac", null ],
        [ "AUTOSAR software stack <-> Ethernet Firmware Interaction", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics_asrethfwinteraction", null ],
        [ "RPC msg exchange between  Eth Driver and Ethernet Firmware", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics_rpcmsgexchange", null ],
        [ "Remote Core Data Path setup illustration", "ug_ethvirtmac_top.html#ug_ethvirtmac_virtmacbasics_remotecoresetup", null ]
      ] ],
      [ "Functional Description", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_top", [
        [ "Ethernet interrupt to ISR mapping", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_id_mapping", null ],
        [ "Configuration", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_v", [
            [ "Ethernet APIs Not Supported", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_v_unsupportedapis", null ],
            [ "Ethernet Configuration variance", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_v_config", null ],
            [ "Ethernet Transceiver", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_v_ethtransceiver", null ]
          ] ],
          [ "Non Standard Service APIs", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_non_std_api_top", null ]
        ] ],
        [ "Interrupt Configuration", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_int", null ],
        [ "Power-up", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_pwr", null ],
        [ "Build and Running the Application", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_cfg_build", [
          [ "Building the host-side example application", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_hostapp_build", null ],
          [ "Building the target-side example application in loopback mode", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_loopback_build", null ]
        ] ],
        [ "Steps to run example application", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_run_example", [
          [ "Running the  test", "ug_ethvirtmac_top.html#ug_ethvirtmac_function_run_default_example", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_dep_top", [
          [ "DET", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_dep_det", null ],
          [ "SchM", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_dep_schm", null ],
          [ "CddIpc", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_dep_cddipc", null ]
        ] ],
        [ "File Structure", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_filestruct_top", [
          [ "Eth File Structure", "ug_ethvirtmac_top.html#ug_ethvirtmac_functional_eth_filestruct_top", null ]
        ] ]
      ] ],
      [ "Error Handling", "ug_ethvirtmac_top.html#ug_ethvirtmac_error_top", [
        [ "Development Error Reporting", "ug_ethvirtmac_top.html#ug_ethvirtmac_error_dev", null ],
        [ "Error codes", "ug_ethvirtmac_top.html#ug_ethvirtmac_error_codes", [
          [ "Ethernet driver error codes", "ug_ethvirtmac_top.html#ug_ethvirtmac_drv_error_codes", null ]
        ] ],
        [ "Production Code Error Reporting", "ug_ethvirtmac_top.html#ug_ethvirtmac_error_prod", null ]
      ] ],
      [ "API Description", "ug_ethvirtmac_top.html#ug_ethvirtmac_api_top", null ],
      [ "Example Application", "ug_ethvirtmac_top.html#ug_ethvirtmac_eg_top", [
        [ "Example Log", "ug_ethvirtmac_top.html#ug_ethvirtmac_eg_log", [
          [ "J721E/J7200", "ug_ethvirtmac_top.html#ug_ethvirtmac_eg_log_j721e", null ]
        ] ]
      ] ],
      [ "References", "ug_ethvirtmac_top.html#ug_ethvirtmac_ref_top", null ],
      [ "Document Revision History", "ug_ethvirtmac_top.html#ug_ethvirtmac_rev_hist", null ]
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
        [ "Example Log", "ug_gpt_top.html#ug_gpt_eg_log", null ]
      ] ],
      [ "References", "ug_gpt_top.html#ug_gpt_ref_top", null ]
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
          [ "Implementation Specific Configuration Constraint", "ug_spi_top.html#ug_spi_functional_i_cfg_cons", null ],
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
        [ "Example Log", "ug_spi_top.html#ug_spi_eg_log", null ]
      ] ],
      [ "References", "ug_spi_top.html#ug_spi_ref_top", null ]
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
      [ "Safety Diagnostic Features", "ug_wdg_top.html#ug_wdg_safety_diag_top", [
        [ "CLK5C - External Watchdog", "ug_wdg_top.html#ug_wdg_External_Watchdog", null ],
        [ "Software Readback of Static Configuration Registers", "ug_wdg_top.html#ug_wdg_Register_Readback", null ]
      ] ],
      [ "API Description", "ug_wdg_top.html#ug_wdg_api_top", null ],
      [ "Example Application", "ug_wdg_top.html#ug_wdg_eg_top", [
        [ "Example Log", "ug_wdg_top.html#ug_wdg_eg_log", null ]
      ] ],
      [ "References", "ug_wdg_top.html#ug_wdg_ref_top", null ]
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
        [ "Interrupt to ISR mapping for jacinto variants", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_id_mapping_jacinto", null ],
        [ "Interrupt to ISR mapping for SMPU variants", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_id_mapping_smpu", null ],
        [ "Configuration", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg", [
          [ "Non Standard Service APIs", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_i_cfg_s_api_top", [
            [ "Cdd_IpcRegisterReadBack", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_i_cfg_s_api_rb", null ],
            [ "Cdd_IpcGetMailboxStatus", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_i_cfg_s_api_mbs", null ]
          ] ]
        ] ],
        [ "Power-up", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg_pwr", null ],
        [ "Build and Running for CDD_IPC Application for jacinto variants", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_cfg_build", [
          [ "Building the remote core example application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_remoteapp_build_jacinto", null ],
          [ "Building the MCAL IPC profiling application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_profileapp_build_tirtos", null ],
          [ "Building the MCAL IPC profiling application for Linux communication", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_profileapp_build_linux", null ],
          [ "Steps to run example application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_run_example_jacinto", null ],
          [ "Build for M4 remote core example application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_remoteapp_build_smpu", [
            [ "AM62X", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_remoteapp_build_am62x", null ]
          ] ],
          [ "Steps to run CDD_IPC example application", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_run_example_am62x", [
            [ "Running the CDD_IPC example application via CCS", "ug_cdd_ipc_top.html#ug_cdd_ipc_function_run_eg_am62x", null ]
          ] ],
          [ "Steps to run example application for AM62AX", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_run_example_am62ax", [
            [ "Running the example application via CCS", "ug_cdd_ipc_top.html#ug_cdd_ipc_function_run_eg_am62ax", null ]
          ] ]
        ] ],
        [ "Steps to run example application for AM62PX", "ug_cdd_ipc_top.html#ug_cdd_ipc_functional_run_example_am62px", null ],
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
          [ "MCU 2 1", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_log_mcu2_1", null ],
          [ "AM62X CDD_IPC R5F to M4 Example Logs", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_logs_am62x_mcu0_0", null ],
          [ "AM62X CDD_IPC DMR5 to A53", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_log_am62x_DMR5_A53_mcu0_0", null ],
          [ "AM62AX MCU_R5 to A53 Example Logs", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_logs_am62ax_mcu0_0", null ],
          [ "AM62PX MCU_R5 to A53 Example Logs", "ug_cdd_ipc_top.html#ug_cdd_ipc_eg_logs_am62pxmcu0_0", null ]
        ] ]
      ] ],
      [ "References", "ug_cdd_ipc_top.html#ug_cdd_ipc_ref_top", null ]
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
            [ "PwmIdleState", "ug_pwm_top.html#ug_pwm_functional_i_cfg_Idlestate", null ],
            [ "PwmIndex", "ug_pwm_top.html#ug_pwm_functional_i_cfg_pwm_index", null ],
            [ "PwmHSClkPrescaler", "ug_pwm_top.html#ug_pwm_functional_i_cfg_hsprescale", null ],
            [ "PwmEnableHighRes", "ug_pwm_top.html#ug_pwm_functional_i_cfg_enableHR", null ],
            [ "PwmOutputChSelect", "ug_pwm_top.html#ug_pwm_functional_i_cfg_outputch", null ]
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
          [ "Cache", "ug_pwm_top.html#ug_pwm_functional_cfg_cache", null ],
          [ "Prescaler/Dividers for ePWM based implementation", "ug_pwm_top.html#ug_pwm_function_cfg_prescaler", null ]
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
          [ "J721E - PwmApp_Gpt", "ug_pwm_top.html#ug_pwm_eg_log_j721e_gpt", null ],
          [ "J7200 - PwmApp_Gpt", "ug_pwm_top.html#ug_pwm_eg_log_j7200_gpt", null ],
          [ "J721E - PwmApp_Epwm", "ug_pwm_top.html#ug_pwm_eg_log_j721e_epwm", null ],
          [ "J7200 - PwmApp_Epwm", "ug_pwm_top.html#ug_pwm_eg_log_j7200_epwm", null ]
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
            [ "Adc_RegisterReadback", "ug_adc_top.html#ug_adc_functional_i_cfg_s_api_imp", null ],
            [ "Adc_EnableInternalDiagnostic", "ug_adc_top.html#ug_adc_functional_i_cfg_s_api_sd1", null ],
            [ "Adc_DisableInternalDiagnostic", "ug_adc_top.html#ug_adc_functional_i_cfg_s_api_sd2", null ]
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
      [ "Safety Diagnostic Features", "ug_adc_top.html#ug_adc_safety_diagnostic_top", [
        [ "ADC1A - Converter Self-Test", "ug_adc_top.html#ug_adc_converter_self_test", null ],
        [ "ADC1B - Converter Calibration Test", "ug_adc_top.html#ug_adc_calibration_self_test", null ],
        [ "ADC4 - Software Readback of Written Configuration / ADC5 - Periodic Software Readback of Static Configuration Registers", "ug_adc_top.html#ug_adc_readback", null ]
      ] ],
      [ "API Description", "ug_adc_top.html#ug_adc_api_top", null ],
      [ "Example Application", "ug_adc_top.html#ug_adc_eg_top", [
        [ "Example Log", "ug_adc_top.html#ug_adc_eg_log", null ]
      ] ],
      [ "References", "ug_adc_top.html#ug_adc_ref_top", null ],
      [ "Document Revision History", "ug_adc_top.html#ug_adc_rev_hist", null ]
    ] ],
    [ "Fls User Guide", "ug_fls_top.html", [
      [ "Introduction", "ug_fls_top.html#ug_fls_intro", null ],
      [ "Fls Driver Architecture/Design", "ug_fls_top.html#ug_fls_design", null ],
      [ "Functional Description", "ug_fls_top.html#ug_fls_functional_top", [
        [ "Configuration", "ug_fls_top.html#ug_fls_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_fls_top.html#ug_fls_functional_cfg_v", [
            [ "FlsDacEnable", "ug_fls_top.html#ug_fls_functional_cfg_v_dacEnable", null ],
            [ "FlsXipEnable", "ug_fls_top.html#ug_fls_functional_cfg_v_xipEnable", null ],
            [ "FlsOspiClkSpeed", "ug_fls_top.html#ug_fls_functional_cfg_v_ospiClkSpeed", null ],
            [ "FlsDtrEnable", "ug_fls_top.html#ug_fls_functional_cfg_v_dtrEnable", null ],
            [ "FlsPhyEnable", "ug_fls_top.html#ug_fls_functional_cfg_v_phyEnable", null ],
            [ "Features that are not supported:", "ug_fls_top.html#ug_fls_functional_cfg_v_", null ]
          ] ],
          [ "Non Standard Service APIs", "ug_fls_top.html#ug_fls_functional_i_cfg_s_api_top", [
            [ "Ospi_RegisterReadback", "ug_fls_top.html#ug_fls_functional_i_cfg_s_api_rb", null ],
            [ "Fls_dataOverflowUnderflowIntrEnable", "ug_fls_top.html#ug_fls_functional_i_cfg_s_api_Ie", null ],
            [ "Fls_dataOverflowUnderflowIntrDisable", "ug_fls_top.html#ug_fls_functional_i_cfg_s_api_Id", null ],
            [ "Fls_dataOverflowUnderflowIntrGetStatus", "ug_fls_top.html#ug_fls_functional_i_cfg_s_api_Is", null ],
            [ "Fls_dataOverflowUnderflowIntrStatusClear", "ug_fls_top.html#ug_fls_functional_i_cfg_s_api_Isc", null ]
          ] ]
        ] ],
        [ "Constraints", "ug_fls_top.html#ug_fls_functional_cfg_constraints", null ],
        [ "Interrupt Configuration", "ug_fls_top.html#ug_fls_functional_cfg_int", null ],
        [ "Power-up", "ug_fls_top.html#ug_fls_functional_cfg_pwr", null ],
        [ "PHY Tune Module Algorithm", "ug_fls_top.html#ug_fls_functional_cfg_phy_tune", null ],
        [ "Driver Adaption for new flash protocol and new flash device", "ug_fls_top.html#ug_fls_functional_driver_adaption", null ],
        [ "QSPI device support on J721S2", "ug_fls_top.html#ug_fls_functional_j721s2qspi", null ],
        [ "Build and Running the Application", "ug_fls_top.html#ug_fls_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_fls_top.html#ug_fls_functional_run_example", null ],
        [ "Dependencies on SW Modules", "ug_fls_top.html#ug_fls_functional_dep_top", [
          [ "DET", "ug_fls_top.html#ug_fls_functional_dep_det", null ],
          [ "SchM", "ug_fls_top.html#ug_fls_functional_dep_schm", null ],
          [ "MemIf", "ug_fls_top.html#ug_fls_functional_dep_memif", null ],
          [ "Fee", "ug_fls_top.html#ug_fls_functional_dep_fee", null ]
        ] ],
        [ "Driver File Structure", "ug_fls_top.html#ug_fls_functional_driverstruct_top", null ],
        [ "Configuration Folder Structure", "ug_fls_top.html#ug_fls_functional_cfgstruct_top", null ]
      ] ],
      [ "Error Handling", "ug_fls_top.html#ug_fls_error_top", [
        [ "Development Error Reporting", "ug_fls_top.html#ug_fls_error_dev", null ],
        [ "Error codes", "ug_fls_top.html#ug_fls_error_codes", null ],
        [ "Transient Faults Reporting", "ug_fls_top.html#ug_fls_error_transient", null ],
        [ "Fault codes", "ug_fls_top.html#ug_fls_fault_codes", null ]
      ] ],
      [ "API Description", "ug_fls_top.html#ug_fls_api_top", null ],
      [ "Example Application", "ug_fls_top.html#ug_fls_eg_top", [
        [ "Example Log", "ug_fls_top.html#ug_fls_eg_log", [
          [ "fls_app_dac", "ug_fls_top.html#ug_fls_eg_log_fls_app_dac", null ],
          [ "fls_app_indac", "ug_fls_top.html#ug_fls_eg_log_fls_app_indac", null ],
          [ "fls_app_intr", "ug_fls_top.html#ug_fls_eg_log_fls_app_intr", null ],
          [ "fls_app_xip", "ug_fls_top.html#ug_fls_eg_log_fls_app_xip", null ]
        ] ]
      ] ],
      [ "References", "ug_fls_top.html#ug_fls_ref_top", null ]
    ] ],
    [ "Icu User Guide", "ug_icu_top.html", [
      [ "Introduction", "ug_icu_top.html#ug_icu_intro", null ],
      [ "Icu Driver Architecture/Design", "ug_icu_top.html#ug_icu_design", null ],
      [ "Functional Description", "ug_icu_top.html#ug_icu_functional_top", [
        [ "Clock Source to timers", "ug_icu_top.html#ug_icu_functional_clksrc", null ],
        [ "Icu Channel ID , Instance mapping and ISR mapping", "ug_icu_top.html#ug_icu_functional_id_mapping", null ],
        [ "Configuration", "ug_icu_top.html#ug_icu_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_icu_top.html#ug_icu_functional_cfg_v", [
            [ "APIs related to wakeup capability", "ug_icu_top.html#ug_icu_functional_cfg_v_wakeup", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_icu_top.html#ug_icu_functional_i_cfg", [
            [ "IcuFunctionalClock", "ug_icu_top.html#ug_icu_functional_i_cfg_clk", null ],
            [ "IcuDeviceVariant", "ug_icu_top.html#ug_icu_functional_i_cfg_var", null ],
            [ "IcuClkPrescaler", "ug_icu_top.html#ug_icu_functional_i_cfg_prescale", null ]
          ] ]
        ] ],
        [ "Interrupt Configuration", "ug_icu_top.html#ug_icu_functional_cfg_int", [
          [ "Non Standard Service APIs", "ug_icu_top.html#ug_icu_functional_i_cfg_s_api_top", [
            [ "Icu_RegisterReadback", "ug_icu_top.html#ug_icu_functional_i_cfg_s_api_rb", null ]
          ] ]
        ] ],
        [ "Power-up", "ug_icu_top.html#ug_icu_functional_cfg_pwr", null ],
        [ "Build and Running the Example Application", "ug_icu_top.html#ug_icu_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_icu_top.html#ug_icu_functional_run_example", null ],
        [ "Memory Mapping", "ug_icu_top.html#ug_icu_functional_cfg_memmap", [
          [ "Cache", "ug_icu_top.html#ug_icu_functional_cfg_cache", null ],
          [ "Prescaler/Dividers for ECAP based implementation", "ug_icu_top.html#ug_icu_function_cfg_prescaler", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_icu_top.html#ug_icu_functional_dep_top", [
          [ "DET", "ug_icu_top.html#ug_icu_functional_dep_det", null ],
          [ "SchM", "ug_icu_top.html#ug_icu_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_icu_top.html#ug_icu_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_icu_top.html#ug_icu_error_top", [
        [ "Development Error Reporting", "ug_icu_top.html#ug_icu_error_dev", null ],
        [ "Error codes", "ug_icu_top.html#ug_icu_error_codes", null ]
      ] ],
      [ "API Description", "ug_icu_top.html#ug_icu_api_top", null ],
      [ "Example Application", "ug_icu_top.html#ug_icu_eg_top", [
        [ "Example Log", "ug_icu_top.html#ug_icu_eg_log", null ]
      ] ],
      [ "References", "ug_icu_top.html#ug_icu_ref_top", null ]
    ] ],
    [ "Mcu User Guide", "ug_mcu_top.html", [
      [ "Introduction", "ug_mcu_top.html#ug_mcu_intro", null ],
      [ "Mcu Driver Architecture/Design", "ug_mcu_top.html#ug_mcu_design", null ],
      [ "Functional Description", "ug_mcu_top.html#ug_mcu_functional_top", [
        [ "DMSC", "ug_mcu_top.html#ug_mcu_functional_dmsc", null ],
        [ "SCI-CLIENT", "ug_mcu_top.html#ug_mcu_functional_sciclient", null ],
        [ "Clocks", "ug_mcu_top.html#ug_mcu_functional_clocks", [
          [ "Mapping of Clocks to CPU/Compute cores", "ug_mcu_top.html#ug_mcu_functional_clocks_clockmapping2cpu", null ],
          [ "Mapping of Clocks to Peripherals", "ug_mcu_top.html#ug_mcu_functional_clocks_clockmapping2peripheral", null ]
        ] ],
        [ "Configuration", "ug_mcu_top.html#ug_mcu_functional_cfg", [
          [ "Variance / Deviation from the specification", "ug_mcu_top.html#ug_mcu_functional_cfg_v", [
            [ "APIs with restricted functionality", "ug_mcu_top.html#ug_mcu_functional_cfg_v_restricted", null ]
          ] ],
          [ "Implementation Specific Configurations", "ug_mcu_top.html#ug_mcu_functional_i_cfg", [
            [ "McuCBKFunction", "ug_mcu_top.html#ug_mcu_functional_i_cfg_cbk", null ],
            [ "McuDeviceVariant", "ug_mcu_top.html#ug_mcu_functional_i_cfg_var", null ],
            [ "McuResetMode", "ug_mcu_top.html#ug_mcu_functional_i_cfg_reset", null ],
            [ "Domain", "ug_mcu_top.html#ug_mcu_functional_i_cfg_domain", null ],
            [ "McuClockModuleId", "ug_mcu_top.html#ug_mcu_functional_i_cfg_clkmodule", null ],
            [ "McuClockSourceId", "ug_mcu_top.html#ug_mcu_functional_i_cfg_clksrc", null ],
            [ "McuClockInitializationConfigFlag", "ug_mcu_top.html#ug_mcu_functional_i_cfg_clkinitflag", null ]
          ] ]
        ] ],
        [ "Build and Running the Example Application", "ug_mcu_top.html#ug_mcu_functional_cfg_build", null ],
        [ "Steps to run example application", "ug_mcu_top.html#ug_mcu_functional_run_example", null ],
        [ "Memory Mapping", "ug_mcu_top.html#ug_mcu_functional_cfg_memmap", [
          [ "Cache", "ug_mcu_top.html#ug_mcu_functional_cfg_cache", null ]
        ] ],
        [ "Dependencies on SW Modules", "ug_mcu_top.html#ug_mcu_functional_dep_top", [
          [ "DET", "ug_mcu_top.html#ug_mcu_functional_dep_det", null ],
          [ "SchM", "ug_mcu_top.html#ug_mcu_functional_dep_schm", null ]
        ] ],
        [ "File Structure", "ug_mcu_top.html#ug_mcu_functional_filestruct_top", null ]
      ] ],
      [ "Error Handling", "ug_mcu_top.html#ug_mcu_error_top", [
        [ "Development Error Reporting", "ug_mcu_top.html#ug_mcu_error_dev", null ],
        [ "Error codes", "ug_mcu_top.html#ug_mcu_error_codes", null ]
      ] ],
      [ "API Description", "ug_mcu_top.html#ug_mcu_api_top", null ],
      [ "Example Application", "ug_mcu_top.html#ug_mcu_eg_top", [
        [ "Example Log", "ug_mcu_top.html#ug_mcu_eg_log", null ]
      ] ],
      [ "Clock Source Reference", "ug_mcu_top.html#ug_mcu_note", null ],
      [ "References", "ug_mcu_top.html#ug_mcu_ref_top", null ]
    ] ]
];