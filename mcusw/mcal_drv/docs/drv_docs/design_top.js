var design_top =
[
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
          [ "Data Reception", "design_eth_top.html#design_eth_seq_rx", null ]
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
          [ "Eth_SetControllerMode", "design_eth_top.html#design_eth_low_level_api_set_ctrl_mode", null ],
          [ "Eth_GetControllerMode", "design_eth_top.html#design_eth_low_level_api_get_ctrl_mode", null ],
          [ "Eth_GetPhysAddr", "design_eth_top.html#design_eth_low_level_api_get_phy_addr", null ],
          [ "Eth_SetPhysAddr", "design_eth_top.html#design_eth_low_level_api_set_phy_addr", null ],
          [ "Eth_UpdatePhysAddrFilter", "design_eth_top.html#design_eth_low_level_api_up_phy_addr", null ],
          [ "Eth_WriteMii", "design_eth_top.html#design_eth_low_level_api_wr_mii", null ],
          [ "Eth_ReadMii", "design_eth_top.html#design_eth_low_level_api_rd_mii", null ],
          [ "Eth_GetCounterValues", "design_eth_top.html#design_eth_low_level_api_get_counter_values", null ],
          [ "Eth_GetRxStats", "design_eth_top.html#design_eth_low_level_api_get_rx_stats", null ],
          [ "Eth_GetTxStats", "design_eth_top.html#design_eth_low_level_api_get_tx_stats", null ],
          [ "Eth_GetTxErrorCounterValues", "design_eth_top.html#design_eth_low_level_api_get_tx_error_counter_values", null ],
          [ "Eth_GetCurrentTime", "design_eth_top.html#design_eth_low_level_api_get_curr_time", null ],
          [ "Eth_EnableEgressTimeStamp", "design_eth_top.html#design_eth_low_level_api_en_egress_time", null ],
          [ "Eth_GetEgressTimeStamp", "design_eth_top.html#design_eth_low_level_api_get_egress_time", null ],
          [ "Eth_GetIngressTimeStamp", "design_eth_top.html#design_eth_low_level_api_get_ingress_time", null ],
          [ "Eth_ProvideTxBuffer", "design_eth_top.html#design_eth_low_level_api_give_tx", null ],
          [ "Eth_Transmit", "design_eth_top.html#design_eth_low_level_api_tx", null ],
          [ "Eth_Receive", "design_eth_top.html#design_eth_low_level_api_rx", null ],
          [ "Eth_TxConfirmation", "design_eth_top.html#design_eth_low_level_api_tx_confirm", null ],
          [ "Eth_GetVersionInfo", "design_eth_top.html#design_eth_low_level_api_get_ver", null ],
          [ "Eth_MainFunction", "design_eth_top.html#design_eth_low_level_api_main_function", null ]
        ] ],
        [ "Ethernet Interface Callbacks", "design_eth_top.html#design_eth_ethif_callbacks", null ],
        [ "Ethernet Switch Callbacks", "design_eth_top.html#design_eth_ethswt_callbacks", null ],
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
    [ "Module Design Documents", "design_common_top.html", null ]
];