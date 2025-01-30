var demo_top =
[
    [ "CAN Profiling Application", "demo_can_profile_top.html", [
      [ "Introduction", "demo_can_profile_top.html#demo_can_profile_intro", [
        [ "Dependencies", "demo_can_profile_top.html#demo_can_profile_depend", null ]
      ] ],
      [ "Flow Chart", "demo_can_profile_top.html#demo_can_profile_flowchart", null ],
      [ "Customizing Examples Application", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_top", null ],
      [ "Operating modes", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_modes", [
        [ "TX Only", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_tx", null ],
        [ "RX Only", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_rx", null ],
        [ "Internal Loopback (default)", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_lpbk", null ],
        [ "External Loopback", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_ext_lpbk", null ]
      ] ],
      [ "Underlying implementation", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_implementation", [
        [ "Interrupt mode (default)", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_interrupt", null ],
        [ "Polling mode", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_polling", [
          [ "Processing type for controller", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_polling_controller", null ],
          [ "Batch processing in polling", "demo_can_profile_top.html#demo_can_profile_demo_cfg_custom_polling_batch", null ]
        ] ]
      ] ],
      [ "Test Setup / Configurations used", "demo_can_profile_top.html#demo_can_profile_setup_cfg", null ],
      [ "Early CAN response", "demo_can_profile_top.html#demo_can_profile_early_can_res_top", null ],
      [ "J721E and J7200", "demo_can_profile_top.html#demo_can_profile_early_can_res_j721e_j7200", [
        [ "Steps to run with CCS", "demo_can_profile_top.html#demo_can_profile_steps_2_run", null ]
      ] ],
      [ "Document Revision History", "demo_can_profile_top.html#demo_can_profile_rev_history", null ]
    ] ],
    [ "Execute In Place (XIP) Application", "demo_xip_profile_top.html", [
      [ "Introduction", "demo_xip_profile_top.html#demo_xip_profile_intro", null ],
      [ "Flow Chart", "demo_xip_profile_top.html#demo_xip_profile_flowchart", null ],
      [ "Compile Time Configurations", "demo_xip_profile_top.html#demo_xip_profile_demo_cfg", null ],
      [ "Customizing Examples Application", "demo_xip_profile_top.html#demo_xip_profile_demo_cfg_custom_top", [
        [ "TX Only", "demo_xip_profile_top.html#demo_xip_profile_demo_cfg_custom_tx", null ],
        [ "RX Only", "demo_xip_profile_top.html#demo_xip_profile_demo_cfg_custom_rx", null ],
        [ "Loopback (default)", "demo_xip_profile_top.html#demo_xip_profile_demo_cfg_custom_lpbk", null ]
      ] ],
      [ "Test Setup / Configurations used", "demo_xip_profile_top.html#demo_xip_profile_setup_cfg", [
        [ "Steps to build", "demo_xip_profile_top.html#demo_xip_profile_build", null ],
        [ "Steps to flash", "demo_xip_profile_top.html#demo_xip_profile_flash", null ],
        [ "BIOS Care abouts to run on XIP mode", "demo_xip_profile_top.html#demo_xip_care_abouts", null ]
      ] ],
      [ "Document Revision History", "demo_xip_profile_top.html#demo_xip_profile_rev_history", null ]
    ] ],
    [ "CDD IPC Profiling Application", "demo_cdd_ipc_profile_top.html", [
      [ "Introduction", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_intro", [
        [ "Dependencies", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_depend", null ]
      ] ],
      [ "Flow Chart", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_flowchart", null ],
      [ "Compile Time Configurations", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_demo_cfg", null ],
      [ "Test Setup / Configurations used", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_setup_cfg", [
        [ "Steps to run", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_steps_2_run", null ]
      ] ],
      [ "Document Revision History", "demo_cdd_ipc_profile_top.html#demo_cdd_ipc_profile_rev_history", null ]
    ] ],
    [ "CAN Response Application", "demo_boot_app_mcu_rtos_top.html", [
      [ "Introduction", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_intro", [
        [ "Performance / Early CAN response", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_profiled", null ],
        [ "Internal Dependencies", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_depend_int", null ],
        [ "External Dependencies", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_depend_ext", null ]
      ] ],
      [ "Flow Charts", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_flowchart", null ],
      [ "Compile Time Configurations for Boot Application", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_cfg", null ],
      [ "Compile Time Configurations for CAN fast response task", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_can_cfg", null ],
      [ "Test Setup / Configurations used", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_setup_cfg", null ],
      [ "Building dependent binaries", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_prep_dep", [
        [ "Building can_boot_app_mcu_rtos", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_building_can_boot_app_mcu_rtos", null ]
      ] ],
      [ "Steps to run", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_steps_2_run", [
        [ "Sample output RTOS main domain apps, CAN Loop back mode", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_main_domain_rtos_can_output_loopback", null ]
      ] ],
      [ "Known Issues", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_known_issues", null ],
      [ "Document Revision History", "demo_boot_app_mcu_rtos_top.html#demo_boot_app_mcu_rtos_rev_history", null ]
    ] ],
    [ "Mode Switch Application", "demo_mode_switch_top.html", [
      [ "Introduction", "demo_mode_switch_top.html#demo_mode_switch_intro_1", [
        [ "Dependencies", "demo_mode_switch_top.html#demo_mode_switch_depend", null ]
      ] ],
      [ "Flow Chart", "demo_mode_switch_top.html#demo_mode_switch_flowchart", null ],
      [ "Build Instructions", "demo_mode_switch_top.html#demo_mode_switch_demo_build_top", [
        [ "Enabling the application", "demo_mode_switch_top.html#demo_mode_switch_demo_build_enable", null ],
        [ "Building", "demo_mode_switch_top.html#demo_mode_switch_demo_build_link", null ]
      ] ],
      [ "Steps to run", "demo_mode_switch_top.html#demo_mode_switch_steps_2_run", [
        [ "Check power rails", "demo_mode_switch_top.html#demo_mode_switch_steps_2_run_check_power_rails", null ],
        [ "LOGs", "demo_mode_switch_top.html#demo_mode_switch_steps_2_run_logs", null ]
      ] ],
      [ "Known Issues", "demo_mode_switch_top.html#demo_boot_app_mode_switch_mcu_rtos_known_issues", null ],
      [ "Document Revision History", "demo_mode_switch_top.html#demo_mode_switch_rev_history", null ]
    ] ],
    [ "Execute In Place (XIP) + Firmware Over The Air (FOTA) Application", "demo_xip_fota_profile_top.html", [
      [ "Introduction", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_intro", null ],
      [ "Flow Chart", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_flowchart", null ],
      [ "Compile Time Configurations", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_demo_cfg", null ],
      [ "Customizing Examples Application", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_demo_cfg_custom_top", [
        [ "TX Only", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_demo_cfg_custom_tx", null ],
        [ "RX Only", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_demo_cfg_custom_rx", null ],
        [ "Loopback (default)", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_demo_cfg_custom_lpbk", null ],
        [ "Steps to build", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_build", null ],
        [ "Steps to flash", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_flash", null ],
        [ "BIOS Care abouts to run on XIP mode", "demo_xip_fota_profile_top.html#demo_xip_fota_care_abouts", null ]
      ] ],
      [ "Document Revision History", "demo_xip_fota_profile_top.html#demo_xip_fota_profile_rev_history", null ]
    ] ]
];