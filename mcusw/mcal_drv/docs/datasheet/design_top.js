var design_top =
[
    [ "Can Design Document", "design_can_top.html", [
      [ "Introduction", "design_can_top.html#design_can_intro", [
        [ "Overview", "design_can_top.html#design_can_intro_overview", null ],
        [ "Can (FD) Overview", "design_can_top.html#design_can_fd_overview", null ],
        [ "References", "design_can_top.html#design_can_references", null ]
      ] ],
      [ "Requirements", "design_can_top.html#design_can_req", [
        [ "Features Supported", "design_can_top.html#design_can_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_can_top.html#design_can_features_not_supported", null ],
        [ "Assumptions", "design_can_top.html#design_can_assumptions", null ],
        [ "Constraints", "design_can_top.html#design_can_constraints", null ]
      ] ],
      [ "Design Description", "design_can_top.html#design_can_description", [
        [ "Fundamental Operation", "design_can_top.html#design_can_desc_funda", [
          [ "Classic Can (Normal Operation)", "design_can_top.html#design_can_desc_funda_classic_op", null ],
          [ "CAN FD", "design_can_top.html#design_can_desc_funda_fd_op", null ]
        ] ],
        [ "Dynamic Behavior", "design_can_top.html#design_can_desc_dynamic", [
          [ "States", "design_can_top.html#design_can_desc_dynamic_state", null ]
        ] ],
        [ "Directory Structure", "design_can_top.html#design_can_desc_deter_dir", null ],
        [ "Interrupt Service Routine", "design_can_top.html#design_can_desc_int", null ],
        [ "Configurator", "design_can_top.html#design_can_desc_cfg", [
          [ "NON Standard configurable parameters", "design_can_top.html#design_can_desc_cfg_ti", null ],
          [ "Implementation specific parameters (computed)", "design_can_top.html#design_can_desc_cfg_ti_compute", null ],
          [ "Variant Support", "design_can_top.html#design_can_desc_cfg_variant", null ]
        ] ],
        [ "Dependency on other modules", "design_can_top.html#design_can_desc_cfg_dependency", [
          [ "SBL", "design_can_top.html#design_can_desc_cfg_dependency_sbl", null ]
        ] ],
        [ "Error Classification", "design_can_top.html#design_can_desc_error", [
          [ "Development Errors", "design_can_top.html#design_can_desc_error_dev", null ],
          [ "Error Detection", "design_can_top.html#design_can_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_can_top.html#design_can_desc_error_dev_notify", null ],
          [ "Runtime Errors", "design_can_top.html#design_can_desc_error_runtime", null ],
          [ "Error notification (DEM)", "design_can_top.html#design_can_desc_error_dev_notify_dem", null ]
        ] ]
      ] ],
      [ "Low Level Definitions", "design_can_top.html#design_can_low_level", [
        [ "MACROS, Data Types & Structures", "design_can_top.html#design_can_low_level_dtypes", [
          [ "Can Controller Pre-Compile Configuration(Can_ControllerType_PC)", "design_can_top.html#design_can_low_c_type", null ],
          [ "Mailbox Configuration(Can_MailboxType)", "design_can_top.html#design_can_low_mb_cfg_type", null ],
          [ "Can Mailbox Pre-Compile Configuration(Can_MailboxType_PC)", "design_can_top.html#design_can_low_mb_cfg_pc_type", null ],
          [ "Can Configuration(Can_ConfigType)", "design_can_top.html#design_can_low_can_cfg_type", null ]
        ] ],
        [ "API's", "design_can_top.html#design_can_low_level_api", [
          [ "Can_Init", "design_can_top.html#design_can_low_level_api_init", null ],
          [ "Can_SetControllerMode", "design_can_top.html#design_can_low_level_api_set_mode", null ],
          [ "Can_Write", "design_can_top.html#design_can_low_level_api_can_write", null ],
          [ "Can_DisableControllerInterrupts", "design_can_top.html#design_can_low_level_api_can_dis_ctrl_int", null ],
          [ "Can_EnableControllerInterrupts", "design_can_top.html#design_can_low_level_api_can_en_ctrl_int", null ],
          [ "Can_MainFunction_Write", "design_can_top.html#design_can_low_level_api_can_main_write", null ],
          [ "Can_MainFunction_BusOff", "design_can_top.html#design_can_low_level_api_can_main_busoff", null ],
          [ "Can_MainFunction_Read", "design_can_top.html#design_can_low_level_api_can_main_read", null ],
          [ "Can_MainFunction_Wakeup", "design_can_top.html#design_can_low_level_api_can_main_wakeup", null ],
          [ "Can_GetVersionInfo", "design_can_top.html#design_can_low_level_api_can_get_ver", null ],
          [ "Can_MainFunction_Mode", "design_can_top.html#design_can_low_level_api_main_fun_mode", null ],
          [ "Can_ChangeBaudrate", "design_can_top.html#design_can_low_level_api_change_baud", null ],
          [ "Can_CheckBaudrate", "design_can_top.html#design_can_low_level_api_check_baud", null ],
          [ "Can_TestLoopBackModeEnable", "design_can_top.html#design_can_low_level_api_en_loopback", null ],
          [ "Can_TestLoopBackModeDisable", "design_can_top.html#design_can_low_level_api_dis_loopback", null ],
          [ "Can_RegisterReadback", "design_can_top.html#design_can_low_level_api_rb", null ],
          [ "Can_mcanProcessISRRx", "design_can_top.html#design_can_low_level_api_isr_rx", null ],
          [ "Can_X_IntXISR OR Can_X_IntXISR_Fun", "design_can_top.html#design_can_low_level_api_main_isr", null ],
          [ "Can_SetBaudrate", "design_can_top.html#design_can_low_level_api_set_baud", null ],
          [ "Can_DeInit", "design_can_top.html#design_can_low_level_api_de_init", null ]
        ] ],
        [ "Global Variables", "design_can_top.html#design_can_low_level_globals", null ],
        [ "Resource Behavior", "design_can_top.html#design_can_desc_res_behave", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_can_top.html#design_can_dar_top", [
        [ "DAR Criteria", "design_can_top.html#design_can_dar_criteria_1", null ],
        [ "Available Alternatives", "design_can_top.html#design_can_dar_alternatives_1", null ],
        [ "Decision", "design_can_top.html#design_can_dar_decision_1", null ],
        [ "DAR Criteria", "design_can_top.html#design_can_dar_criteria_2", null ],
        [ "Available Alternatives", "design_can_top.html#design_can_dar_alternatives_2", null ],
        [ "Decision", "design_can_top.html#design_can_dar_decision_2", null ]
      ] ],
      [ "Test Criteria", "design_can_top.html#design_can_test_top", null ],
      [ "Document Revision History", "design_can_top.html#design_can_rev_hist", null ]
    ] ],
    [ "Dio Design Document", "design_dio_top.html", [
      [ "Introduction", "design_dio_top.html#design_dio_intro", [
        [ "Overview", "design_dio_top.html#design_dio_intro_overview", [
          [ "Dio Overview", "design_dio_top.html#design_dio_intro_dio_overview", null ]
        ] ],
        [ "References", "design_dio_top.html#design_dio_references", null ]
      ] ],
      [ "Requirements", "design_dio_top.html#design_dio_req", [
        [ "Features Supported", "design_dio_top.html#design_dio_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_dio_top.html#design_dio_features_not_supported", null ],
        [ "Assumptions", "design_dio_top.html#design_dio_assumptions", null ],
        [ "Constraints", "design_dio_top.html#design_dio_constraints", null ],
        [ "Dependencies", "design_dio_top.html#design_dio_depend", null ]
      ] ],
      [ "Design Description", "design_dio_top.html#design_dio_description", [
        [ "DIO Channel, Port And Channel Group", "design_dio_top.html#design_dio_desc_ch_p_chgrp", null ],
        [ "Input/Output Functionality", "design_dio_top.html#design_dio_desc_io", null ],
        [ "Dynamic Behavior", "design_dio_top.html#design_dio_dynamic_behaviour", null ],
        [ "Resource Behavior", "design_dio_top.html#design_dio_desc_res_behave", null ],
        [ "Supporting DIO for AUTOSAR versioned 4.3.1", "design_dio_top.html#design_dio_desc_upgrade_431", null ],
        [ "Directory Structure", "design_dio_top.html#design_dio_desc_deter_dir", null ],
        [ "Configurator", "design_dio_top.html#design_dio_desc_cfg", [
          [ "NON Standard configurable parameters", "design_dio_top.html#design_dio_desc_cfg_ti", null ],
          [ "Variant Support", "design_dio_top.html#design_dio_desc_cfg_variant", null ]
        ] ],
        [ "Error Classification", "design_dio_top.html#design_dio_desc_error", [
          [ "Error Detection", "design_dio_top.html#design_dio_desc_error_dev_detect", null ],
          [ "Development Errors", "design_dio_top.html#design_dio_desc_error_dev", null ],
          [ "Error notification (DET)", "design_dio_top.html#design_dio_desc_error_dev_notify", null ]
        ] ]
      ] ],
      [ "Low Level Definitions", "design_dio_top.html#design_dio_low_level", [
        [ "MACROS, Data Types & Structures", "design_dio_top.html#design_dio_low_level_dtypes", null ],
        [ "APIs", "design_dio_top.html#design_dio_low_level_api", [
          [ "Dio_ReadChannel", "design_dio_top.html#design_dio_low_level_api_read_ch", null ],
          [ "Dio_WriteChannel", "design_dio_top.html#design_dio_low_level_api_write_ch", null ],
          [ "Dio_ReadPort", "design_dio_top.html#design_dio_low_level_api_read_port", null ],
          [ "Dio_WritePort", "design_dio_top.html#design_dio_low_level_api_wr_port", null ],
          [ "Dio_ReadChannelGroup", "design_dio_top.html#design_dio_low_level_api_rd_ch_grp", null ],
          [ "Dio_WriteChannelGroup", "design_dio_top.html#design_dio_low_level_api_wr_ch_grp", null ],
          [ "Dio_FlipChannel", "design_dio_top.html#design_dio_low_level_api_flip_ch", null ],
          [ "Dio_GetVersionInfo", "design_dio_top.html#design_dio_low_level_api_get_ver", null ],
          [ "Dio_RegisterReadback", "design_dio_top.html#design_dio_low_level_api_rb", null ]
        ] ],
        [ "Global Variables", "design_dio_top.html#design_dio_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_dio_top.html#design_dio_dar_top", [
        [ "Supporting Different SoCs", "design_dio_top.html#design_dio_dar_soc", [
          [ "DAR Criteria", "design_dio_top.html#design_dio_dar_criteria_soc", null ],
          [ "Available Alternatives", "design_dio_top.html#design_dio_dar_alternatives_soc", null ],
          [ "Decision", "design_dio_top.html#design_dio_dar_decision_soc", null ]
        ] ],
        [ "Width of Port", "design_dio_top.html#design_dio_dar_port", [
          [ "DAR Criteria", "design_dio_top.html#design_dio_dar_criteria_port", null ],
          [ "Available Alternatives", "design_dio_top.html#design_dio_dar_alternatives_port", null ],
          [ "Decision", "design_dio_top.html#design_dio_dar_decision", null ]
        ] ]
      ] ],
      [ "Test Criteria", "design_dio_top.html#design_dio_test_top", null ],
      [ "Document Revision History", "design_dio_top.html#design_dio_rev_hist", null ]
    ] ],
    [ "Eth Design Document", "design_eth_top.html", [
      [ "Introduction", "design_eth_top.html#design_eth_intro", [
        [ "Overview", "design_eth_top.html#design_eth_intro_overview", null ],
        [ "Eth Overview", "design_eth_top.html#design_eth_eth_overview", null ],
        [ "References", "design_eth_top.html#design_eth_references", null ]
      ] ],
      [ "Requirements", "design_eth_top.html#design_eth_req", [
        [ "Features Supported", "design_eth_top.html#design_eth_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_eth_top.html#design_eth_features_not_supported", null ],
        [ "Assumptions", "design_eth_top.html#design_eth_assumptions", null ]
      ] ],
      [ "Design Description", "design_eth_top.html#design_eth_description", [
        [ "Dynamic Behavior", "design_eth_top.html#design_eth_desc_dynamic", null ],
        [ "Sequence Diagrams", "design_eth_top.html#design_eth_seq_dia", [
          [ "Data Transmission", "design_eth_top.html#design_eth_seq_tx", null ],
          [ "Data Reception", "design_eth_top.html#design_eth_seq_rx", null ],
          [ "Data Transmission", "design_eth_top.html#design_eth_seq_tx_opt", null ],
          [ "Data Reception", "design_eth_top.html#design_eth_seq_rx_opt", null ],
          [ "Recommended Methods for Tx & Rx", "design_eth_top.html#design_eth_seq_rx_rec", null ]
        ] ],
        [ "Directory Structure", "design_eth_top.html#design_eth_desc_deter_dir", null ],
        [ "Interrupt Service Routines", "design_eth_top.html#design_eth_desc_isr", null ],
        [ "Configurator", "design_eth_top.html#design_eth_desc_cfg", [
          [ "NON Standard configurable parameters", "design_eth_top.html#design_eth_desc_cfg_ti", null ]
        ] ],
        [ "Debug Information", "design_eth_top.html#design_eth_desc_dbg", null ],
        [ "Error Classification", "design_eth_top.html#design_eth_desc_error", [
          [ "Development Errors", "design_eth_top.html#design_eth_desc_error_dev", null ],
          [ "Error Detection", "design_eth_top.html#design_eth_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_eth_top.html#design_eth_desc_error_dev_notify", null ],
          [ "Runtime Errors", "design_eth_top.html#design_eth_desc_error_runtime", null ],
          [ "Extended Production error", "design_eth_top.html#design_eth_desc_error_dem_extended", null ]
        ] ],
        [ "Resource Behavior", "design_eth_top.html#design_eth_desc_res_behave", null ],
        [ "API's", "design_eth_top.html#design_eth_low_level_api", [
          [ "Eth_Init", "design_eth_top.html#design_eth_low_level_api_init", null ],
          [ "Eth_ControllerInit", "design_eth_top.html#design_eth_low_level_api_ctrl_init", null ],
          [ "Eth_SetControllerMode", "design_eth_top.html#design_eth_low_level_api_set_ctrl_mode", null ],
          [ "Eth_GetControllerMode", "design_eth_top.html#design_eth_low_level_api_get_ctrl_mode", null ],
          [ "Eth_WriteMii", "design_eth_top.html#design_eth_low_level_api_wr_mii", null ],
          [ "Eth_ReadMii", "design_eth_top.html#design_eth_low_level_api_rd_mii", null ],
          [ "Eth_GetPhysAddr", "design_eth_top.html#design_eth_low_level_api_get_phy_addr", null ],
          [ "Eth_ProvideTxBuffer", "design_eth_top.html#design_eth_low_level_api_give_tx", null ],
          [ "Eth_Transmit", "design_eth_top.html#design_eth_low_level_api_tx", null ],
          [ "Eth_Receive", "design_eth_top.html#design_eth_low_level_api_rx", null ],
          [ "Eth_TxConfirmation", "design_eth_top.html#design_eth_low_level_api_tx_confirm", null ],
          [ "Eth_GetVersionInfo", "design_eth_top.html#design_eth_low_level_api_get_ver", null ],
          [ "Eth_RxIrqHdlr_<CtrlIdx>", "design_eth_top.html#design_eth_low_level_api_rx_irq", null ],
          [ "Eth_TxIrqHdlr_<CtrlIdx>", "design_eth_top.html#design_eth_low_level_api_tx_irq", null ],
          [ "Eth_UpdatePhysAddrFilter", "design_eth_top.html#design_eth_low_level_api_up_phy_addr", null ],
          [ "Eth_SetPhysAddr", "design_eth_top.html#design_eth_low_level_api_set_phy_addr", null ],
          [ "Eth_GetDropCount", "design_eth_top.html#design_eth_low_level_api_get_drop_count", null ],
          [ "Eth_GetEtherStats", "design_eth_top.html#design_eth_low_level_api_get_eth_stats", null ],
          [ "Eth_GetCurrentTime", "design_eth_top.html#design_eth_low_level_api_get_curr_time", null ],
          [ "Eth_EnableEgressTimeStamp", "design_eth_top.html#design_eth_low_level_api_en_egress_time", null ],
          [ "Eth_GetEgressTimeStamp", "design_eth_top.html#design_eth_low_level_api_get_egress_time", null ],
          [ "Eth_GetIngressTimeStamp", "design_eth_top.html#design_eth_low_level_api_get_ingress_time", null ],
          [ "Eth_SetCorrectionTime", "design_eth_top.html#design_eth_low_level_api_set_correction_time", null ],
          [ "Eth_SetGlobalTime", "design_eth_top.html#design_eth_low_level_api_set_global_time", null ],
          [ "Eth_MainFunction", "design_eth_top.html#design_eth_low_level_api_main_function", null ]
        ] ],
        [ "Types", "design_eth_top.html#design_eth_low_level_types", null ],
        [ "Global Variables", "design_eth_top.html#design_eth_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_eth_top.html#design_eth_dar_top", [
        [ "Packet Submission & Retrieval to CPSW: Single or Queue", "design_eth_top.html#design_eth_dar_q_or_s", [
          [ "DAR Criteria", "design_eth_top.html#design_eth_dar_q_or_s_criteria", null ],
          [ "Available Alternatives", "design_eth_top.html#design_eth_dar_alternatives_1", null ],
          [ "Decision", "design_eth_top.html#design_eth_dar_decision_1", null ]
        ] ],
        [ "Buffers Per Packet", "design_eth_top.html#design_eth_dar_buf_pack", [
          [ "DAR Criteria", "design_eth_top.html#design_eth_dar_buf_pack_criteria", null ],
          [ "Available Alternatives", "design_eth_top.html#design_eth_dar_alternatives_2", null ],
          [ "Decision", "design_eth_top.html#design_eth_dar_decision_2", null ]
        ] ]
      ] ],
      [ "Test Criteria", "design_eth_top.html#design_eth_test_top", null ],
      [ "Document Revision History", "design_eth_top.html#design_eth_rev_hist", null ]
    ] ],
    [ "Gpt Design Document", "design_gpt_top.html", [
      [ "Introduction", "design_gpt_top.html#design_gpt_intro", [
        [ "Overview", "design_gpt_top.html#design_gpt_intro_overview", null ],
        [ "GPT Overview", "design_gpt_top.html#design_gpt_gpt_overview", null ],
        [ "References", "design_gpt_top.html#design_gpt_references", null ]
      ] ],
      [ "Requirements", "design_gpt_top.html#design_gpt_req", [
        [ "Features Supported", "design_gpt_top.html#design_gpt_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_gpt_top.html#design_gpt_features_not_supported", null ],
        [ "Assumptions", "design_gpt_top.html#design_gpt_assumptions", null ],
        [ "Constraints", "design_gpt_top.html#design_gpt_constraints", null ]
      ] ],
      [ "Design Description", "design_gpt_top.html#design_gpt_description", [
        [ "Fundamental Operation", "design_gpt_top.html#design_gpt_desc_funda", null ],
        [ "Dynamic Behavior", "design_gpt_top.html#design_gpt_desc_dynamic", [
          [ "States", "design_gpt_top.html#design_gpt_desc_dynamic_state", null ]
        ] ],
        [ "Modes of Timer", "design_gpt_top.html#design_gpt_desc_modes", [
          [ "Continuous Mode", "design_gpt_top.html#design_gpt_desc_modes_conti", null ],
          [ "One Shot Mode", "design_gpt_top.html#design_gpt_desc_modes_one_shot", null ]
        ] ],
        [ "Determination of time elapsed", "design_gpt_top.html#design_gpt_desc_deter_time_elapsed", null ],
        [ "Determination of time remaining", "design_gpt_top.html#design_gpt_desc_deter_time_remain", null ],
        [ "Directory Structure", "design_gpt_top.html#design_gpt_desc_deter_dir", null ],
        [ "Configurator", "design_gpt_top.html#design_gpt_desc_cfg", [
          [ "NON Standard configurable parameters", "design_gpt_top.html#design_gpt_desc_cfg_ti", null ],
          [ "Implementation specific parameters (computed)", "design_gpt_top.html#design_gpt_desc_cfg_ti_compute", null ],
          [ "Variant Support", "design_gpt_top.html#design_gpt_desc_cfg_variant", null ]
        ] ],
        [ "Dependency on other modules", "design_gpt_top.html#design_gpt_desc_cfg_dependency", [
          [ "SBL", "design_gpt_top.html#design_gpt_desc_cfg_dependency_sbl", null ]
        ] ],
        [ "Error Classification", "design_gpt_top.html#design_gpt_desc_error", [
          [ "Development Errors", "design_gpt_top.html#design_gpt_desc_error_dev", null ],
          [ "Error Detection", "design_gpt_top.html#design_gpt_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_gpt_top.html#design_gpt_desc_error_dev_notify", null ],
          [ "Runtime Errors", "design_gpt_top.html#design_gpt_desc_error_runtime", null ],
          [ "Error notification (DEM)", "design_gpt_top.html#design_gpt_desc_error_dev_notify_dem", null ]
        ] ],
        [ "Resource Behavior", "design_gpt_top.html#design_gpt_desc_res_behave", null ]
      ] ],
      [ "Low Level Definitions", "design_gpt_top.html#design_gpt_low_level", [
        [ "MACROS, Data Types & Structures", "design_gpt_top.html#design_gpt_low_level_dtypes", [
          [ "Gpt_RegisterReadbackType", "design_gpt_top.html#design_gpt_low_level_readback_type", null ],
          [ "Gpt_ChannelConfigType", "design_gpt_top.html#design_gpt_low_level_channel_cfg", null ],
          [ "Gpt_ConfigType", "design_gpt_top.html#design_gpt_low_level_cfg", null ]
        ] ],
        [ "API's", "design_gpt_top.html#design_gpt_low_level_api", [
          [ "Gpt_Init", "design_gpt_top.html#design_gpt_low_level_api_init", null ],
          [ "Gpt_DeInit", "design_gpt_top.html#design_gpt_low_level_api_deinit", null ],
          [ "Gpt_GetTimeElapsed", "design_gpt_top.html#design_gpt_low_level_api_gte", null ],
          [ "Gpt_GetTimeRemaining", "design_gpt_top.html#design_gpt_low_level_api_gtr", null ],
          [ "Gpt_StartTimer", "design_gpt_top.html#design_gpt_low_level_api_st", null ],
          [ "Gpt_StopTimer", "design_gpt_top.html#design_gpt_low_level_api_stopT", null ],
          [ "Gpt_EnableNotification", "design_gpt_top.html#design_gpt_low_level_api_en", null ],
          [ "Gpt_DisableNotification", "design_gpt_top.html#design_gpt_low_level_api_dn", null ],
          [ "Gpt_SetMode", "design_gpt_top.html#design_gpt_low_level_api_sm", null ],
          [ "Gpt_DisableWakeup", "design_gpt_top.html#design_gpt_low_level_api_dw", null ],
          [ "Gpt_EnableWakeup", "design_gpt_top.html#design_gpt_low_level_api_ew", null ],
          [ "Gpt_CheckWakeup", "design_gpt_top.html#design_gpt_low_level_api_cw", null ],
          [ "Gpt_GetVersionInfo", "design_gpt_top.html#design_gpt_low_level_api_gvi", null ],
          [ "Gpt_RegisterReadback", "design_gpt_top.html#design_gpt_low_level_api_rb", null ]
        ] ],
        [ "Notify ISR", "design_gpt_top.html#design_gpt_low_level_api_isr", null ],
        [ "Wakeup ISR", "design_gpt_top.html#design_gpt_low_level_api_wakeup_isr", null ],
        [ "Global Variables", "design_gpt_top.html#design_gpt_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_gpt_top.html#design_gpt_dar_top", [
        [ "Use of DM Timer Auto-Reload mode for GPT continuous mode", "design_gpt_top.html#design_gpt_dar_reload", null ],
        [ "DAR Criteria", "design_gpt_top.html#design_gpt_dar_criteria", null ],
        [ "Available Alternatives", "design_gpt_top.html#design_gpt_dar_alternatives", null ],
        [ "Decision", "design_gpt_top.html#design_gpt_dar_decision", null ]
      ] ],
      [ "Test Criteria", "design_gpt_top.html#design_gpt_test_top", null ],
      [ "Document Revision History", "design_gpt_top.html#design_gpt_rev_hist", null ]
    ] ],
    [ "Spi Design Document", "design_spi_top.html", [
      [ "Introduction", "design_spi_top.html#design_spi_intro", [
        [ "Overview", "design_spi_top.html#design_spi_intro_overview", [
          [ "Spi Overview", "design_spi_top.html#design_spi_intro_spi_overview", null ],
          [ "References", "design_spi_top.html#design_spi_references", null ]
        ] ]
      ] ],
      [ "Requirements", "design_spi_top.html#design_spi_req", [
        [ "Features Supported", "design_spi_top.html#design_spi_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_spi_top.html#design_spi_features_not_supported", null ],
        [ "Assumptions", "design_spi_top.html#design_spi_assumptions", null ],
        [ "Constraints", "design_spi_top.html#design_spi_constraints", null ],
        [ "Dependencies to other modules", "design_spi_top.html#design_spi_depend", null ]
      ] ],
      [ "Design Description", "design_spi_top.html#design_spi_description", [
        [ "Priority Handling and Job Queuing Operations", "design_spi_top.html#design_spi_desc_prio_jobqueue", null ],
        [ "Interrupt Service Routines", "design_spi_top.html#design_spi_desc_ISR", null ],
        [ "Dynamic Behavior", "design_spi_top.html#design_spi_dynamic_behaviour", null ],
        [ "Resource Behavior", "design_spi_top.html#design_spi_desc_res_behave", null ],
        [ "Directory Structure", "design_spi_top.html#design_spi_desc_deter_dir", null ],
        [ "Configurator", "design_spi_top.html#design_spi_desc_cfg", [
          [ "NON Standard configurable parameters", "design_spi_top.html#design_spi_desc_cfg_ti", null ],
          [ "Variant Support", "design_spi_top.html#design_spi_desc_cfg_variant", null ]
        ] ],
        [ "Error Classification", "design_spi_top.html#design_spi_desc_error", [
          [ "Error Detection", "design_spi_top.html#design_spi_desc_error_dev_detect", null ],
          [ "Development Errors", "design_spi_top.html#design_spi_desc_error_dev", null ],
          [ "Parameter Checking", "design_spi_top.html#design_spi_desc_error_param_check", null ]
        ] ]
      ] ],
      [ "Low Level Definitions", "design_spi_top.html#design_spi_low_level", [
        [ "MACROS, Data Types & Structures", "design_spi_top.html#design_spi_low_level_dtypes", null ],
        [ "APIs", "design_spi_top.html#design_spi_low_level_api", [
          [ "Spi_Init", "design_spi_top.html#design_spi_low_level_api_init", null ],
          [ "Spi_DeInit", "design_spi_top.html#design_spi_low_level_api_deinit", null ],
          [ "Spi_GetStatus", "design_spi_top.html#design_spi_low_level_api_getstatus", null ],
          [ "Spi_GetJobResult", "design_spi_top.html#design_spi_low_level_api_getjobres", null ],
          [ "Spi_GetSequenceResult", "design_spi_top.html#design_spi_low_level_api_getseqres", null ],
          [ "Spi_GetHWUnitStatus", "design_spi_top.html#design_spi_low_level_api_gethwunitstat", null ],
          [ "Spi_SetupEB", "design_spi_top.html#design_spi_low_level_api_setupeb", null ],
          [ "Spi_AsyncTransmit", "design_spi_top.html#design_spi_low_level_api_asynctransmit", null ],
          [ "Spi_Cancel", "design_spi_top.html#design_spi_low_level_api_cancel", null ],
          [ "Spi_SyncTransmit", "design_spi_top.html#design_spi_low_level_api_synctransmit", null ],
          [ "Spi_SetAsyncMode", "design_spi_top.html#design_spi_low_level_api_setasyncmode", null ],
          [ "Spi_MainFunction_Handling", "design_spi_top.html#design_spi_low_level_api_mainfunchandling", null ],
          [ "Spi_GetVersionInfo", "design_spi_top.html#design_spi_low_level_api_getversioninfo", null ],
          [ "Spi_WriteIB", "design_spi_top.html#design_spi_low_level_api_writeib", null ],
          [ "Spi_ReadIB", "design_spi_top.html#design_spi_low_level_api_readib", null ],
          [ "Spi_RegisterReadback", "design_spi_top.html#design_spi_low_level_api_rb", null ],
          [ "Spi_SetLoopbackMode", "design_spi_top.html#design_spi_low_level_api_lpbck", null ]
        ] ],
        [ "Global Variables", "design_spi_top.html#design_spi_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_spi_top.html#design_spi_dar_top", [
        [ "Data transfer mode (MCSPI)", "design_spi_top.html#design_spi_dar_datatransfermode", [
          [ "DAR Criteria", "design_spi_top.html#design_spi_dar_criteria_datatransfermode", null ],
          [ "Available Alternatives", "design_spi_top.html#design_spi_dar_alternatives_datatransfermode", null ],
          [ "Decision", "design_spi_top.html#design_spi_dar_decision_soc", null ]
        ] ],
        [ "Selecting SPI Instances in Configurator", "design_spi_top.html#design_spi_dar_instanceselect", [
          [ "DAR Criteria", "design_spi_top.html#design_spi_dar_criteria_instanceselect", null ],
          [ "Available Alternatives", "design_spi_top.html#design_spi_dar_alternatives_instanceselect", null ],
          [ "Decision", "design_spi_top.html#design_spi_dar_decision", null ]
        ] ]
      ] ],
      [ "Test Criteria", "design_spi_top.html#design_spi_test_top", null ],
      [ "Document Revision History", "design_spi_top.html#design_spi_rev_hist", null ]
    ] ],
    [ "Wdg Design Document", "design_wdg_top.html", [
      [ "Introduction", "design_wdg_top.html#design_wdg_intro", [
        [ "Overview", "design_wdg_top.html#design_wdg_intro_overview", null ],
        [ "WDG Overview", "design_wdg_top.html#design_wdg_wdg_overview", null ],
        [ "References", "design_wdg_top.html#design_wdg_references", null ]
      ] ],
      [ "Requirements", "design_wdg_top.html#design_wdg_req", [
        [ "Features Supported", "design_wdg_top.html#design_wdg_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_wdg_top.html#design_wdg_features_not_supported", null ],
        [ "Key Notes", "design_wdg_top.html#design_wdg_key_notes", null ],
        [ "Assumptions", "design_wdg_top.html#design_wdg_assumptions", null ],
        [ "Constraints", "design_wdg_top.html#design_wdg_constraints", null ]
      ] ],
      [ "Design Description", "design_wdg_top.html#design_wdg_description", [
        [ "Fundamental Operation", "design_wdg_top.html#design_wdg_desc_funda", null ],
        [ "WDG Configuration Sequence", "design_wdg_top.html#design_wdg_desc_Sequence", null ],
        [ "Dynamic Behavior", "design_wdg_top.html#design_wdg_desc_dynamic", [
          [ "States", "design_wdg_top.html#design_wdg_desc_dynamic_state", null ]
        ] ],
        [ "Directory Structure", "design_wdg_top.html#design_wdg_desc_deter_dir", null ],
        [ "Configurator", "design_wdg_top.html#design_wdg_desc_cfg", [
          [ "NON Standard configurable parameters", "design_wdg_top.html#design_wdg_desc_cfg_ti", null ],
          [ "Implementation specific parameters (computed)", "design_wdg_top.html#design_wdg_desc_cfg_ti_compute", null ],
          [ "Variant Support", "design_wdg_top.html#design_wdg_desc_cfg_variant", null ]
        ] ],
        [ "Dependency on other modules", "design_wdg_top.html#design_wdg_desc_cfg_dependency", [
          [ "SBL", "design_wdg_top.html#design_wdg_desc_cfg_dependency_sbl", null ],
          [ "ESM", "design_wdg_top.html#design_wdg_desc_cfg_dependency_esm", null ]
        ] ],
        [ "Debug Information", "design_wdg_top.html#design_wdg_desc_dbg", null ],
        [ "Error Classification", "design_wdg_top.html#design_wdg_desc_error", [
          [ "Development Errors", "design_wdg_top.html#design_wdg_desc_error_dev", null ],
          [ "Error Detection", "design_wdg_top.html#design_wdg_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_wdg_top.html#design_wdg_desc_error_dev_notify", null ],
          [ "Runtime Errors", "design_wdg_top.html#design_wdg_desc_error_runtime", null ],
          [ "Error notification (DEM)", "design_wdg_top.html#design_wdg_desc_error_dev_notify_dem", null ]
        ] ],
        [ "Resource Behavior", "design_wdg_top.html#design_wdg_desc_res_behave", null ],
        [ "Supporting WDG for AUTOSAR versioned 4.3.1", "design_wdg_top.html#design_wdg_desc_upgrade_431", null ]
      ] ],
      [ "Low Level Definitions", "design_wdg_top.html#design_wdg_low_level", [
        [ "MACROS, Data Types & Structures", "design_wdg_top.html#design_wdg_low_level_dtypes", [
          [ "Wdg_ModeInfoType", "design_wdg_top.html#design_wdg_low_level_modeinfo_cfg", null ],
          [ "Wdg_ConfigType", "design_wdg_top.html#design_wdg_low_level_cfg", null ],
          [ "Wdg_RegisterReadbackType", "design_wdg_top.html#design_wdg_low_level_readback_type", null ]
        ] ],
        [ "API's", "design_wdg_top.html#design_wdg_low_level_api", [
          [ "Wdg_Init", "design_wdg_top.html#design_wdg_low_level_api_init", null ],
          [ "Wdg_SetTriggerCondition", "design_wdg_top.html#design_wdg_low_level_api_settriggercondition", null ],
          [ "Wdg_GetVersionInfo", "design_wdg_top.html#design_wdg_low_level_api_getversioninfo", null ],
          [ "Wdg_Trigger", "design_wdg_top.html#design_wdg_low_level_api_trigger", null ],
          [ "Wdg_RegisterReadback", "design_wdg_top.html#design_wdg_low_level_api_rb", null ]
        ] ],
        [ "Global Variables", "design_wdg_top.html#design_wdg_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_wdg_top.html#design_wdg_dar_top", [
        [ "Watchdog SOC Reset Functionality", "design_wdg_top.html#design_wdg_dar_reset_functionality", null ],
        [ "DAR Criteria", "design_wdg_top.html#design_wdg_dar_criteria_reset_output", null ],
        [ "Available Alternatives", "design_wdg_top.html#design_wdg_dar_alternatives_reset_output", null ],
        [ "Decision", "design_wdg_top.html#design_wdg_dar_decision_reset_output", null ],
        [ "Watchdog Service Routine", "design_wdg_top.html#design_wdg_dar_service_routine", null ],
        [ "DAR Criteria", "design_wdg_top.html#design_wdg_dar_criteria_service_routine", null ],
        [ "Available Alternatives", "design_wdg_top.html#design_wdg_dar_alternatives_service_routine", null ],
        [ "Decision", "design_wdg_top.html#design_wdg_dar_decision_service_routine", null ]
      ] ],
      [ "Test Criteria", "design_wdg_top.html#design_wdg_test_top", null ],
      [ "Document Revision History", "design_wdg_top.html#design_wdg_rev_hist", null ]
    ] ],
    [ "Adc Design Document", "design_adc_top.html", [
      [ "Introduction", "design_adc_top.html#design_adc_intro", [
        [ "Overview", "design_adc_top.html#design_adc_intro_overview", null ],
        [ "ADC Overview", "design_adc_top.html#design_adc_hw_overview", null ],
        [ "References", "design_adc_top.html#design_adc_references", null ]
      ] ],
      [ "Requirements", "design_adc_top.html#design_adc_req", [
        [ "Features Supported", "design_adc_top.html#design_adc_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_adc_top.html#design_adc_features_not_supported", null ],
        [ "Assumptions", "design_adc_top.html#design_adc_assumptions", null ],
        [ "Constraints", "design_adc_top.html#design_adc_constraints", null ]
      ] ],
      [ "Design Description", "design_adc_top.html#design_adc_description", [
        [ "Fundamental Operation", "design_adc_top.html#design_adc_desc_funda", null ],
        [ "Different Input Values", "design_adc_top.html#design_adc_diff_input_values", [
          [ "Single-ended input", "design_adc_top.html#design_adc_diff_input_values_single_ended", null ],
          [ "Differential input", "design_adc_top.html#design_adc_diff_input_values_diff_input", null ]
        ] ],
        [ "Dynamic Behavior", "design_adc_top.html#design_adc_desc_dynamic", [
          [ "States", "design_adc_top.html#design_adc_desc_dynamic_state", null ]
        ] ],
        [ "Conversion Modes of Adc", "design_adc_top.html#design_adc_desc_modes", [
          [ "One Shot Mode", "design_adc_top.html#design_adc_desc_modes_one_shot", null ],
          [ "Continuous Mode", "design_adc_top.html#design_adc_desc_modes_conti", null ]
        ] ],
        [ "Trigger source", "design_adc_top.html#design_adc_desc_trig_src", [
          [ "Software API Call", "design_adc_top.html#design_adc_desc_sw_api_call", null ],
          [ "Hardware Event", "design_adc_top.html#design_adc_desc_hw_event", null ]
        ] ],
        [ "Access Modes", "design_adc_top.html#design_adc_desc_access_mode", [
          [ "Adc_GetStreamLastPointer", "design_adc_top.html#design_adc_desc_adc_get_stream_last_pointer", null ],
          [ "Adc_ReadGroup", "design_adc_top.html#design_adc_desc_adc_read_group", null ]
        ] ],
        [ "Priority Handling and Queuing Operations", "design_adc_top.html#design_adc_desc_prio_hand_que", null ],
        [ "Interrupt Service Routines", "design_adc_top.html#design_adc_desc_intr_serv_rout", null ],
        [ "Conversion Mode Example", "design_adc_top.html#design_adc_desc_conv_mode_examp", null ],
        [ "Directory Structure", "design_adc_top.html#design_adc_desc_deter_dir", null ],
        [ "Configurator", "design_adc_top.html#design_adc_desc_cfg", [
          [ "NON Standard configurable parameters", "design_adc_top.html#design_adc_desc_cfg_ti", null ]
        ] ],
        [ "Dependency on other modules", "design_adc_top.html#design_adc_desc_cfg_dependency", [
          [ "SBL", "design_adc_top.html#design_adc_desc_cfg_dependency_sbl", null ]
        ] ],
        [ "Debug Information", "design_adc_top.html#design_adc_desc_dbg", null ],
        [ "Error Classification", "design_adc_top.html#design_adc_desc_error", [
          [ "Development Errors", "design_adc_top.html#design_adc_desc_error_dev", null ],
          [ "Error Detection", "design_adc_top.html#design_adc_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_adc_top.html#design_adc_desc_error_dev_notify", null ],
          [ "Runtime Errors", "design_adc_top.html#design_adc_desc_error_runtime", null ],
          [ "Error notification (DEM)", "design_adc_top.html#design_adc_desc_error_dev_notify_dem", null ]
        ] ],
        [ "Resource Behavior", "design_adc_top.html#design_adc_desc_res_behave", null ],
        [ "Supporting ADC for AUTOSAR versioned 4.4", "design_adc_top.html#design_adc_desc_upgrade_44", null ]
      ] ],
      [ "Low Level Definitions", "design_adc_top.html#design_adc_low_level", [
        [ "MACROS, Data Types & Structures", "design_adc_top.html#design_adc_low_level_dtypes", [
          [ "Adc_RegisterReadbackType", "design_adc_top.html#design_adc_low_level_readback_type", null ],
          [ "Adc_ChannelConfigType", "design_adc_top.html#design_adc_low_level_channel_cfg", null ],
          [ "Adc_GroupConfigType", "design_adc_top.html#design_adc_low_level_grp_cfg", null ],
          [ "Adc_HwUnitConfigType", "design_adc_top.html#design_adc_low_level_hw_unit_cfg", null ],
          [ "Adc_ConfigType", "design_adc_top.html#design_adc_low_level_cfg", null ]
        ] ],
        [ "API's", "design_adc_top.html#design_adc_low_level_api", [
          [ "Adc_Init", "design_adc_top.html#design_adc_low_level_api_init", null ],
          [ "Adc_SetupResultBuffer", "design_adc_top.html#design_adc_low_level_api_setupresultbuffer", null ],
          [ "Adc_DeInit", "design_adc_top.html#design_adc_low_level_api_deinit", null ],
          [ "Adc_StartGroupConversion", "design_adc_top.html#design_adc_low_level_api_startgrpconv", null ],
          [ "Adc_StopGroupConversion", "design_adc_top.html#design_adc_low_level_api_stopgrpconv", null ],
          [ "Adc_ReadGroup", "design_adc_top.html#design_adc_low_level_api_readgrp", null ],
          [ "Adc_EnableGroupNotification", "design_adc_top.html#design_adc_low_level_api_engrpntf", null ],
          [ "Adc_DisableGroupNotification", "design_adc_top.html#design_adc_low_level_api_disgrpntf", null ],
          [ "Adc_GetGroupStatus", "design_adc_top.html#design_adc_low_level_api_grpsts", null ],
          [ "Adc_GetStreamLastPointer", "design_adc_top.html#design_adc_low_level_api_getstreamlastptr", null ],
          [ "Adc_GetVersionInfo", "design_adc_top.html#design_adc_low_level_api_getverinfo", null ],
          [ "Adc_RegisterReadback", "design_adc_top.html#design_adc_low_level_api_rb", null ]
        ] ],
        [ "Global Variables", "design_adc_top.html#design_adc_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_adc_top.html#design_adc_dar_top", [
        [ "ADC Conversion Mode", "design_adc_top.html#design_adc_dar_conv_mode", null ],
        [ "DAR Criteria", "design_adc_top.html#design_adc_dar_criteria", null ],
        [ "Available Alternatives", "design_adc_top.html#design_adc_dar_alternatives", null ],
        [ "Decision", "design_adc_top.html#design_adc_dar_decision", null ]
      ] ],
      [ "Test Criteria", "design_adc_top.html#design_adc_test_top", null ],
      [ "Document Revision History", "design_adc_top.html#design_adc_rev_hist", null ]
    ] ],
    [ "Pwm Design Document", "design_pwm_top.html", [
      [ "Introduction", "design_pwm_top.html#design_pwm_intro", [
        [ "Overview", "design_pwm_top.html#design_pwm_intro_overview", null ],
        [ "PWM Overview", "design_pwm_top.html#design_pwm_overview", null ],
        [ "References", "design_pwm_top.html#design_pwm_references", null ]
      ] ],
      [ "Requirements", "design_pwm_top.html#design_pwm_req", [
        [ "Features Supported", "design_pwm_top.html#design_pwm_features_supported", null ],
        [ "Features Not Supported / NON Compliance", "design_pwm_top.html#design_pwm_features_not_supported", null ],
        [ "Assumptions", "design_pwm_top.html#design_pwm_assumptions", null ],
        [ "Constraints", "design_pwm_top.html#design_pwm_constraints", null ]
      ] ],
      [ "Design Description", "design_pwm_top.html#design_pwm_description", [
        [ "Fundamental Operation", "design_pwm_top.html#design_pwm_desc_fundamental", null ],
        [ "Dynamic Behavior", "design_pwm_top.html#design_pwm_desc_dynamic", [
          [ "States", "design_pwm_top.html#design_pwm_desc_dynamic_state", null ]
        ] ],
        [ "Time Unit Ticks", "design_pwm_top.html#design_pwm_ticks", null ],
        [ "Duty Cycle Resolution and Scaling", "design_pwm_top.html#design_pwm_resolution", null ],
        [ "Directory Structure", "design_pwm_top.html#design_pwm_desc_deter_dir", null ],
        [ "Configurator", "design_pwm_top.html#design_pwm_desc_cfg", [
          [ "NON Standard configurable parameters", "design_pwm_top.html#design_pwm_desc_cfg_ti", null ],
          [ "Variant Support", "design_pwm_top.html#design_pwm_desc_cfg_variant", null ]
        ] ],
        [ "Dependency on other modules", "design_pwm_top.html#design_pwm_desc_cfg_dependency", [
          [ "SBL", "design_pwm_top.html#design_pwm_desc_cfg_dependency_sbl", null ]
        ] ],
        [ "Error Classification", "design_pwm_top.html#design_pwm_desc_error", [
          [ "Development Errors", "design_pwm_top.html#design_pwm_desc_error_dev", null ],
          [ "Error Detection", "design_pwm_top.html#design_pwm_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_pwm_top.html#design_pwm_desc_error_dev_notify", null ]
        ] ],
        [ "Resource Behavior", "design_pwm_top.html#design_pwm_desc_res_behave", null ],
        [ "Supporting PWM for AUTOSAR versioned 4.4.0", "design_pwm_top.html#design_pwm_desc_upgrade_44", null ]
      ] ],
      [ "Low Level Definitions", "design_pwm_top.html#design_pwm_low_level", [
        [ "MACROS, Data Types & Structures", "design_pwm_top.html#design_pwm_low_level_dtypes", [
          [ "Pwm_RegisterReadbackType", "design_pwm_top.html#design_pwm_low_level_readback_type", null ]
        ] ],
        [ "APIs", "design_pwm_top.html#design_pwm_low_level_api", [
          [ "Pwm_Init", "design_pwm_top.html#design_pwm_low_level_api_init", null ],
          [ "Pwm_DeInit", "design_pwm_top.html#design_pwm_low_level_api_deinit", null ],
          [ "Pwm_SetDutyCycle", "design_pwm_top.html#design_pwm_low_level_api_setduty", null ],
          [ "Pwm_SetPeriodAndDuty", "design_pwm_top.html#design_pwm_low_level_api_setperiodduty", null ],
          [ "Pwm_SetOutputToIdle", "design_pwm_top.html#design_pwm_low_level_api_setoptoidle", null ],
          [ "Pwm_DisableNotification", "design_pwm_top.html#design_pwm_low_level_api_disablenotification", null ],
          [ "Pwm_EnableNotification", "design_pwm_top.html#design_pwm_low_level_api_enablenotification", null ],
          [ "Pwm_GetVersionInfo", "design_pwm_top.html#design_pwm_low_level_api_versioninfo", null ],
          [ "Pwm_RegisterReadback", "design_pwm_top.html#design_pwm_low_level_api_rb", null ]
        ] ],
        [ "Global Variables", "design_pwm_top.html#design_pwm_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_pwm_top.html#design_pwm_dar_top", [
        [ "Timer Mode configuration in Overflow Only Mode for Duty cycle of 50%", "design_pwm_top.html#design_pwm_dar_timerdutycycle", null ],
        [ "DAR Criteria", "design_pwm_top.html#design_pwm_dar_criteria", null ],
        [ "Available Alternatives", "design_pwm_top.html#design_pwm_dar_alternatives", null ],
        [ "Decision", "design_pwm_top.html#design_pwm_dar_decision", null ]
      ] ],
      [ "Test Criteria", "design_pwm_top.html#design_pwm_test_top", null ],
      [ "Document Revision History", "design_pwm_top.html#design_pwm_rev_hist", null ]
    ] ],
    [ "Cdd IPC Design Document", "design_ipc_top.html", [
      [ "Introduction", "design_ipc_top.html#design_cdd_ipc_intro", [
        [ "Overview", "design_ipc_top.html#design_cdd_ipc_intro_overview", null ],
        [ "Cdd IPC Overview", "design_ipc_top.html#design_cdd_ipc_overview", [
          [ "Basic Working Principle", "design_ipc_top.html#design_cdd_ipc_basic_arch", null ],
          [ "Rational for IPC as an CDD", "design_ipc_top.html#design_cdd_ipc_rational", null ]
        ] ],
        [ "References", "design_ipc_top.html#design_cdd_ipc_references", null ]
      ] ],
      [ "Requirements", "design_ipc_top.html#design_cdd_ipc_req", [
        [ "Features Supported", "design_ipc_top.html#design_cdd_ipc_features_supported", null ],
        [ "Features Not Supported", "design_ipc_top.html#design_cdd_ipc_features_not_supported", null ],
        [ "Assumptions", "design_ipc_top.html#design_cdd_ipc_assumptions", null ],
        [ "Constraints", "design_ipc_top.html#design_cdd_ipc_constraints", null ]
      ] ],
      [ "Design Description", "design_ipc_top.html#design_cdd_ipc_description", [
        [ "Processor Identifiers", "design_ipc_top.html#design_cdd_ipc_proc_id", null ],
        [ "End Point", "design_ipc_top.html#design_cdd_ipc_ep_id", null ],
        [ "Message Buffer", "design_ipc_top.html#design_cdd_ipc_rpmsg_id", null ],
        [ "Queue in shared memory", "design_ipc_top.html#design_cdd_ipc_virtio_obj_id", null ],
        [ "Multiple End Point / Communication Channels", "design_ipc_top.html#design_cdd_ipc_multi_ep_id", null ],
        [ "Control End Point", "design_ipc_top.html#design_cdd_ipc_ctrl_ep_id", null ],
        [ "Dynamic Behavior", "design_ipc_top.html#design_cdd_ipc_desc_dynamic", [
          [ "States", "design_ipc_top.html#design_cdd_ipc_desc_dynamic_state", null ]
        ] ],
        [ "Directory Structure", "design_ipc_top.html#design_cdd_ipc_desc_deter_dir", null ],
        [ "Configurator", "design_ipc_top.html#design_cdd_ipc_desc_cfg", [
          [ "CDD IPC General Configuration parameters", "design_ipc_top.html#design_cdd_ipc_desc_cfg_gen", null ],
          [ "CDD IPC Shared Memory Configuration parameters", "design_ipc_top.html#design_cdd_ipc_desc_shared_mem", null ],
          [ "CDD IPC Processor Identifier", "design_ipc_top.html#design_cdd_ipc_desc_ownProc", null ],
          [ "CDD IPC Processor Identifier Remote", "design_ipc_top.html#design_cdd_ipc_desc_remote_proc", null ],
          [ "CDD IPC Communication Channels", "design_ipc_top.html#design_cdd_ipc_desc_com_chs", null ],
          [ "Variant Support", "design_ipc_top.html#design_cdd_ipc_desc_cfg_variant", null ]
        ] ],
        [ "Dependency on other modules", "design_ipc_top.html#design_cdd_ipc_desc_cfg_dependency", null ],
        [ "Error Classification", "design_ipc_top.html#design_cdd_ipc_desc_error", [
          [ "Development Errors", "design_ipc_top.html#design_cdd_ipc_desc_error_dev", null ],
          [ "Error Detection", "design_ipc_top.html#design_cdd_ipc_desc_error_dev_detect", null ],
          [ "Error notification (DET)", "design_ipc_top.html#design_cdd_ipc_desc_error_dev_notify", null ],
          [ "Runtime Errors", "design_ipc_top.html#design_cdd_ipc_desc_error_runtime", null ],
          [ "Error notification (DEM)", "design_ipc_top.html#design_cdd_ipc_desc_error_dev_notify_dem", null ]
        ] ],
        [ "Resource Behavior", "design_ipc_top.html#design_cdd_ipc_desc_res_behave", null ]
      ] ],
      [ "Low Level Definitions", "design_ipc_top.html#design_cdd_ipc_low_level", [
        [ "MACROS, Data Types & Structures", "design_ipc_top.html#design_cdd_ipc_low_level_dtypes", [
          [ "Maximum number of channels", "design_ipc_top.html#design_cdd_ipc_max_numchannels", null ],
          [ "Cdd_IpcMpType", "design_ipc_top.html#design_cdd_ipc_Cdd_IpcMpType", null ],
          [ "Cdd_IpcVertIoType", "design_ipc_top.html#design_cdd_ipc_Cdd_IpcVertIoType", null ],
          [ "Cdd_IpcChannelType", "design_ipc_top.html#design_cdd_ipc_Cdd_IpcChannelType", null ],
          [ "Cdd_IpcConfigType", "design_ipc_top.html#design_cdd_ipc_Cdd_IpcConfigType", null ],
          [ "Cdd_IpcRegRbValues", "design_ipc_top.html#design_cdd_ipc_low_level_readback_type", null ]
        ] ],
        [ "API's", "design_ipc_top.html#design_cdd_ipc_low_level_api", [
          [ "Cdd_IpcNewMessageNotify", "design_ipc_top.html#design_cdd_ipc_Cdd_IpcNewMessageNotify", null ],
          [ "Cdd_IpcNewCtrlMessageNotify", "design_ipc_top.html#design_cdd_ipc_Cdd_IpcCtrlNewMessageNotify", null ],
          [ "Cdd_IpcInit", "design_ipc_top.html#design_cdd_ipc_low_level_api_init", null ],
          [ "Cdd_IpcDeinit", "design_ipc_top.html#design_cdd_ipc_low_level_Cdd_IpcDeinit", null ],
          [ "Cdd_IpcSendMsg", "design_ipc_top.html#design_cdd_ipc_low_level_api_Cdd_IpcSendMsg", null ],
          [ "Cdd_IpcReceiveMsg", "design_ipc_top.html#design_cdd_ipc_low_level_api_Cdd_IpcReceiveMsg", null ],
          [ "Cdd_IpcAnnounce", "design_ipc_top.html#design_cdd_ipc_low_level_Cdd_IpcAnnounce", null ],
          [ "Cdd_IpcGetVersionInfo", "design_ipc_top.html#design_cdd_ipc_low_level_api_gver", null ],
          [ "Cdd_IpcRegisterReadBack", "design_ipc_top.html#design_cdd_ipc_low_level_api_rb", null ],
          [ "Cdd_IpcReceiveCtrlMsg", "design_ipc_top.html#design_cdd_ipc_low_level_api_Cdd_IpcReceiveCtrlMsg", null ],
          [ "Cdd_IpcIsInitDone", "design_ipc_top.html#design_cdd_ipc_low_level_api_Cdd_IpcIsInitDone", null ],
          [ "Cdd_IpcGetMaxMsgSize", "design_ipc_top.html#design_cdd_ipc_low_level_api_Cdd_IpcGetMaxMsgSize", null ]
        ] ],
        [ "Cdd_IpcNewMessageNotify ISR", "design_ipc_top.html#design_cdd_ipc_low_level_api_isr", null ],
        [ "Global Variables", "design_ipc_top.html#design_cdd_ipc_low_level_globals", null ]
      ] ],
      [ "Decision Analysis & Resolution (DAR)", "design_ipc_top.html#design_cdd_ipc_dar_top", [
        [ "Allocation of memory for LocalQ", "design_ipc_top.html#design_cdd_ipc_dar_reload", null ],
        [ "DAR Criteria", "design_ipc_top.html#design_cdd_ipc_dar_criteria", null ],
        [ "Available Alternatives", "design_ipc_top.html#design_cdd_ipc_dar_alternatives", null ],
        [ "Decision", "design_ipc_top.html#design_cdd_ipc_dar_decision", null ]
      ] ],
      [ "Test Criteria", "design_ipc_top.html#design_cdd_ipc_test_top", null ],
      [ "Document Revision History", "design_ipc_top.html#design_cdd_ipc_rev_hist", null ]
    ] ]
];