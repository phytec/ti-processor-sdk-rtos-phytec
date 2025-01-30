var BUILD_AND_RUN =
[
    [ "Build Environment Setup", "ENVIRONMENT_SETUP.html", [
      [ "IMPORTANT NOTES", "ENVIRONMENT_SETUP.html#autotoc_md2", null ],
      [ "Step 1a: Download and Install PSDK RTOS", "ENVIRONMENT_SETUP.html#autotoc_md3", null ],
      [ "Step 1b: Download and Install PSDK RTOS Add-on to run in PC Emulation Mode (Optional, only needed for PC emulation mode)", "ENVIRONMENT_SETUP.html#autotoc_md4", null ],
      [ "Step 2a: Download and install PSDK Linux (Needed for Linux+RTOS mode or for QNX SPL mode)", "ENVIRONMENT_SETUP.html#ENVIRONMENT_SETUP_STEP2", null ],
      [ "Step 2b: Download and Install PSDK QNX addon to run in QNX+RTOS mode (Optional, only needed for QNX+RTOS mode)", "ENVIRONMENT_SETUP.html#ENVIRONMENT_SETUP_QNX", null ],
      [ "Step 3: Proxy Setup", "ENVIRONMENT_SETUP.html#PROXY_SETUP", [
        [ "Proxy for wget", "ENVIRONMENT_SETUP.html#autotoc_md5", null ],
        [ "Proxy for http access", "ENVIRONMENT_SETUP.html#autotoc_md6", null ],
        [ "Proxy for \"apt-get\"", "ENVIRONMENT_SETUP.html#autotoc_md7", null ],
        [ "Proxy for \"git\"", "ENVIRONMENT_SETUP.html#autotoc_md8", null ],
        [ "Proxy for \"curl\"", "ENVIRONMENT_SETUP.html#autotoc_md9", null ]
      ] ],
      [ "Step 4: Download and install additional dependencies", "ENVIRONMENT_SETUP.html#ADDITIONAL_DOWNLOADS", null ]
    ] ],
    [ "Build Instructions", "BUILD_INSTRUCTIONS.html", [
      [ "IMPORTANT NOTES", "BUILD_INSTRUCTIONS.html#autotoc_md10", null ],
      [ "Quick Steps to build vision apps for selected configuration", "BUILD_INSTRUCTIONS.html#QUICK_SETUP_STEPS", [
        [ "Linux+RTOS mode", "BUILD_INSTRUCTIONS.html#QUICK_SETUP_STEPS_LINUX_TI_RTOS", null ],
        [ "QNX+RTOS mode", "BUILD_INSTRUCTIONS.html#QUICK_SETUP_STEPS_QNX_TI_RTOS", null ],
        [ "PC emulation mode", "BUILD_INSTRUCTIONS.html#QUICK_SETUP_STEPS_PC_EMULATION", null ]
      ] ],
      [ "Quick steps to clean vision apps generated files", "BUILD_INSTRUCTIONS.html#BUILD_CLEAN", [
        [ "SafeRTOS Support", "BUILD_INSTRUCTIONS.html#QUICK_SETUP_STEPS_SAFERTOS", null ]
      ] ],
      [ "Detailed steps to configure and build vision apps", "BUILD_INSTRUCTIONS.html#BUILD_DETAILED", [
        [ "Makefile and configuration options", "BUILD_INSTRUCTIONS.html#MAKEFILE_OPTIONS", null ],
        [ "Build vision apps without prebuilt libraries", "BUILD_INSTRUCTIONS.html#BUILD_SOURCE", null ]
      ] ]
    ] ],
    [ "Run Instructions", "RUN_INSTRUCTIONS.html", [
      [ "IMPORTANT NOTES", "RUN_INSTRUCTIONS.html#autotoc_md11", null ],
      [ "Run vision apps on EVM in Linux+RTOS mode (via SD card boot)", "RUN_INSTRUCTIONS.html#run_steps_linux", [
        [ "Step 1: Prepare SD card for boot (one time only)", "RUN_INSTRUCTIONS.html#run_step1_linux", null ],
        [ "Step 2: Copy test data to SD card (one time only)", "RUN_INSTRUCTIONS.html#run_step_copy_test_data_linux", null ],
        [ "Step 3: Copy executable files to SD card (first time and each time you want to run updated applications)", "RUN_INSTRUCTIONS.html#run_step_copy_files_linux", null ],
        [ "Step 4: Run on EVM", "RUN_INSTRUCTIONS.html#run_step_run_on_evm_linux", null ],
        [ "Sample logs", "RUN_INSTRUCTIONS.html#autotoc_md12", null ]
      ] ],
      [ "Run vision apps on EVM in Linux+RTOS mode (via NFS boot - For Debug only)", "RUN_INSTRUCTIONS.html#run_steps_linux_nfs", null ],
      [ "Run vision apps on EVM in QNX+RTOS mode (via SD card boot)", "RUN_INSTRUCTIONS.html#run_steps_qnx", [
        [ "Step 1: Prepare SD card for boot (one time only)", "RUN_INSTRUCTIONS.html#run_step1_qnx", null ],
        [ "Step 2a: Copy executable files to SD card for boot using SPL+Uboot(first time and each time you want to run updated applications)", "RUN_INSTRUCTIONS.html#run_step_copy_files_qnx_a", null ],
        [ "Step 2b: Copy executable files to SD card for boot using MMCSD+SBL(first time and each time you want to run updated applications)", "RUN_INSTRUCTIONS.html#run_step_copy_files_qnx_b", null ],
        [ "Step 2c: Copy executable files to SD card for boot using OSPI+SBL(first time and each time you want to run updated applications)", "RUN_INSTRUCTIONS.html#run_step_copy_files_qnx_c", null ],
        [ "Step 3: Copy test data to SD card (one time only)", "RUN_INSTRUCTIONS.html#run_step_copy_test_data_qnx", null ],
        [ "Step 4: Run on EVM", "RUN_INSTRUCTIONS.html#run_step_run_on_evm_qnx", null ]
      ] ],
      [ "Run vision apps on PC in PC emulation mode", "RUN_INSTRUCTIONS.html#run_in_pc_mode", [
        [ "Running the TIOVX demos in PC emulation mode", "RUN_INSTRUCTIONS.html#build_instruction_sec3", null ],
        [ "Running the vision apps demos in PC emulation mode", "RUN_INSTRUCTIONS.html#build_instruction_sec4", null ]
      ] ]
    ] ],
    [ "Incremental Update and Debug", "INCREMENTAL_UPDATE_AND_DEBUG.html", [
      [ "Steps for incremental update and debug", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_steps", [
        [ "Step 1 : Change Code", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_change_code", null ],
        [ "Step 2 : Recompile & Build the code", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_recompile_build", null ],
        [ "Step 3a : Copy executable files to SD card (Linux+RTOS mode)", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_copy_files", null ],
        [ "Step 3b : Copy executable files to SD card (QNX+RTOS mode)", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_copy_files_qnx", null ],
        [ "Step 4a: Run on EVM (Linux+RTOS mode)", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_run_on_evm_linux", null ],
        [ "Step 4b: Run on EVM (QNX+RTOS mode)", "INCREMENTAL_UPDATE_AND_DEBUG.html#incremental_update_run_on_evm_qnx", null ]
      ] ]
    ] ],
    [ "Trouble shooting build and run errors", "BUILD_TROUBLE_SHOOTING.html", [
      [ "Trouble shooting build errors", "BUILD_TROUBLE_SHOOTING.html#autotoc_md13", [
        [ "I see the error \"ERROR: /home/<user>/ti-processor-sdk-rtos-${SOC}-evm-xx_xx_xx_xx/targetfs//usr/include not found !!!\" when building vision_apps", "BUILD_TROUBLE_SHOOTING.html#autotoc_md14", null ],
        [ "How do I install GCC tools for ARM MPU ?", "BUILD_TROUBLE_SHOOTING.html#FAQ_BUILD_GCC", null ],
        [ "What does setup_psdk_rtos.sh do and can I skip executing this ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md15", null ]
      ] ],
      [ "Trouble shooting run time errors", "BUILD_TROUBLE_SHOOTING.html#autotoc_md16", [
        [ "I dont see any print on UART terminal ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md17", null ],
        [ "What is the purpose of different files in bootfs partition ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md18", null ],
        [ "What are the filesystem changes done by vision apps on top of default PSDK Linux filesystem", "BUILD_TROUBLE_SHOOTING.html#autotoc_md19", null ],
        [ "What is DMA heap and why I dont see CMEM or ION anymore ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md20", null ],
        [ "How do I know the remote cores like C6x, C7x, R5F booted correctly ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md21", null ],
        [ "How can I confirm the memory carve outs for various CPUs and remote cores are applied correctly by Linux ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md22", null ],
        [ "Where can I find sample logs for different applications ?", "BUILD_TROUBLE_SHOOTING.html#autotoc_md23", null ]
      ] ]
    ] ]
];