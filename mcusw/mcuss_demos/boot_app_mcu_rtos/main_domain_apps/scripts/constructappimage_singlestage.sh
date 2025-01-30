#!/bin/bash

SDK_INSTALL_PATH=${PWD}/../../../../..
PDK_INSTALL_PATH="${SDK_INSTALL_PATH}/pdk_j721s2_09_02_00_30/packages"
SBL_REPO_PATH="${PDK_INSTALL_PATH}/ti/boot/sbl"
MULTICORE_APPIMAGE_GEN_TOOL_PATH="${SBL_REPO_PATH}/tools/multicoreImageGen/bin"
SBL_OUT2RPRC_GEN_TOOL_PATH="${SBL_REPO_PATH}/tools/out2rprc/bin"
MULTICOREAPP_BIN_PATH=../binary/bin/j721e_evm
TEST_NAME=ex01_multicore_demo_app

devId=55

split_mode=1
dsp_binaries_included=1
mpu_rtos_enabled=1

if [ $split_mode = 0 ] && [ $dsp_binaries_included = 1 ]; then
    appImageName="multicore_lockstep_with_DSPs.appimage"
elif [ $split_mode = 1 ] && [ $dsp_binaries_included = 1 ]; then
    appImageName="multicore_split_with_DSPs.appimage"
elif [ $split_mode = 0 ] && [ $dsp_binaries_included = 0 ]; then
    appImageName="multicore_lockstep.appimage"
else
    appImageName="multicore_split.appimage"
fi

ElfImages=()

if [ $mpu_rtos_enabled = 1 ]; then
    # TODO - Add mpu1_1 elf to the appimage
    ElfImages+=("mcusw_${TEST_NAME}_mpu1_0_release.xa72fg" \
             #  "mcusw_${TEST_NAME}_mpu1_1_release.xa72fg" \
               );
fi

ElfImages+=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_mcu2_1_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_mcu3_0_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_mcu3_1_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_c66xdsp_1_release_strip.xe66" \
            "mcusw_${TEST_NAME}_c66xdsp_2_release_strip.xe66" \
            "mcusw_${TEST_NAME}_c7x_1_release_strip.xe71" \
           );

if [ $split_mode = 1 ]; then
    CoreRprcFiles=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f.rprc" \
                   "mcusw_${TEST_NAME}_mcu2_1_release_strip.xer5f.rprc" \
                   "mcusw_${TEST_NAME}_mcu3_0_release_strip.xer5f.rprc" \
                   "mcusw_${TEST_NAME}_mcu3_1_release_strip.xer5f.rprc" \
                   );
    CoreIds=(6 7 8 9);
    if [ $mpu_rtos_enabled = 1 ]; then
        # TODO - Add mpu1_1 elf to the appimage
        CoreRprcFiles+=("mcusw_${TEST_NAME}_mpu1_0_release.xa72fg.rprc" \
               #        "mcusw_${TEST_NAME}_mpu1_1_release.xa72fg.rprc" \
                       );
        CoreIds+=(0);
        #CoreIds+=(0 1);
    fi
else
    CoreRprcFiles=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f.rprc" \
                   "mcusw_${TEST_NAME}_mcu3_0_release_strip.xer5f.rprc" \
                  );
    CoreIds=(6 8);
    if [ $mpu_rtos_enabled = 1 ]; then
        CoreRprcFiles+=("mcusw_${TEST_NAME}_mpu1_0_release.xa72fg.rprc");
        CoreIds+=(0);
    fi
fi

if [ $dsp_binaries_included = 1 ]; then
    DspCoreRprcFiles=("mcusw_${TEST_NAME}_c66xdsp_1_release_strip.xe66.rprc" \
                      "mcusw_${TEST_NAME}_c66xdsp_2_release_strip.xe66.rprc" \
                      "mcusw_${TEST_NAME}_c7x_1_release_strip.xe71.rprc" \
                     );


    DspCoreIds=(10 11 12);
fi

# Add all main domain ARM binaries to appimage

arrayLength=${#CoreIds[@]}

echo "length is $arrayLength"

for (( i = 0; i < $arrayLength; i++ )); do
    output_args+="${CoreIds[i]} ${CoreRprcFiles[i]} "
#    echo "${CoreIds[i]} ${CoreRprcFiles[i]}"
done

# Add DSP binaries to appimage


if [ $dsp_binaries_included = 1 ]; then
    dspArrayLength=${#DspCoreIds[@]}
    echo "dsp array length is $dspArrayLength"

    for (( i = 0; i < $dspArrayLength; i++ )); do
        output_args+="${DspCoreIds[i]} ${DspCoreRprcFiles[i]} "
    done
fi

# Preserve original working directory
pushd $PWD > /dev/null

cd $MULTICOREAPP_BIN_PATH
MULTICOREAPP_BIN_ABS_PATH=$PWD

if [ -f $appImageName ]; then
    rm $appImageName
fi

for i in ${ElfImages[@]}; do
    $SBL_OUT2RPRC_GEN_TOOL_PATH/out2rprc.exe $i "$i.rprc"
done

$MULTICORE_APPIMAGE_GEN_TOOL_PATH/MulticoreImageGen LE $devId $appImageName $output_args
$PDK_INSTALL_PATH/ti/build/makerules/x509CertificateGen.sh -b $MULTICOREAPP_BIN_ABS_PATH/$appImageName -o $MULTICOREAPP_BIN_ABS_PATH/$appImageName.signed -c R5 -l 0x0 -k $PDK_INSTALL_PATH/ti/build/makerules/k3_dev_mpk.pem

echo "Wrote output appImage to $appImageName"
echo "Wrote signed output appImage to $appImageName.signed"

# Restore shell to original working directory
popd > /dev/null
