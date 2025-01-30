#!/bin/bash

#####################
## User-defined paths
#####################

SDK_INSTALL_PATH=${PWD}/../../../../..
PDK_INSTALL_PATH="${SDK_INSTALL_PATH}/pdk_j721s2_09_02_00_30/packages"
SBL_REPO_PATH="${PDK_INSTALL_PATH}/ti/boot/sbl"
MULTICORE_APPIMAGE_GEN_TOOL_PATH="${SBL_REPO_PATH}/tools/multicoreImageGen/bin"
SBL_OUT2RPRC_GEN_TOOL_PATH="${SBL_REPO_PATH}/tools/out2rprc/bin"
MULTICOREAPP_BIN_PATH=../binary/bin/j721e_evm
TEST_NAME=ex01_multicore_demo_app

devId=55

###################################
## Image definitions for each stage
###################################

numStages=3

appImageName1=multicore_MCU2_0_MCU2_1_stage1.appimage
appImageName2=multicore_DSPs_MCU3_0_MCU3_1_stage2.appimage
appImageName3=multicore_MPU1_0_stage3.appimage

ElfImages1=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_mcu2_1_release_strip.xer5f" \
           )
ElfImages2=("mcusw_${TEST_NAME}_mcu3_0_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_mcu3_1_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_c66xdsp_1_release_strip.xe66" \
            "mcusw_${TEST_NAME}_c66xdsp_2_release_strip.xe66" \
            "mcusw_${TEST_NAME}_c7x_1_release_strip.xe71" \
           )
ElfImages3=("mcusw_baremetal_ex01_multicore_demo_app_mpu1_0_release.xa72fg" \
           )

CoreRprcFiles1=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f.rprc" \
                "mcusw_${TEST_NAME}_mcu2_1_release_strip.xer5f.rprc" \
               )
CoreRprcFiles2=("mcusw_${TEST_NAME}_mcu3_0_release_strip.xer5f.rprc" \
                "mcusw_${TEST_NAME}_mcu3_1_release_strip.xer5f.rprc" \
                "mcusw_${TEST_NAME}_c66xdsp_1_release_strip.xe66.rprc" \
                "mcusw_${TEST_NAME}_c66xdsp_2_release_strip.xe66.rprc" \
                "mcusw_${TEST_NAME}_c7x_1_release_strip.xe71.rprc" \
               )
CoreRprcFiles3=("mcusw_baremetal_ex01_multicore_demo_app_mpu1_0_release.xa72fg.rprc" \
               )

CoreIds1=(10 11);
CoreIds2=(12 13 16 17 18);
CoreIds3=(0);

############################################################

## Create all output arguments for each of the stages

# Create output arguments for Stage 1 appimage
arrayLength=${#CoreIds1[@]}
echo "Stage 1 length is $arrayLength"

for (( i = 0; i < $arrayLength; i++ )); do
    output_args1+="${CoreIds1[i]} ${CoreRprcFiles1[i]} "
done

# Create output arguments for Stage 2 appimage
arrayLength=${#CoreIds2[@]}
echo "Stage 2 length is $arrayLength"
for (( i = 0; i < $arrayLength; i++ )); do
    output_args2+="${CoreIds2[i]} ${CoreRprcFiles2[i]} "
done

# Create output arguments for Stage 3 appimage
arrayLength=${#CoreIds3[@]}
echo "Stage 3 length is $arrayLength"
for (( i = 0; i < $arrayLength; i++ )); do
    output_args3+="${CoreIds3[i]} ${CoreRprcFiles3[i]} "
done

# Preserve original working directory
pushd $PWD > /dev/null

cd $MULTICOREAPP_BIN_PATH
MULTICOREAPP_BIN_ABS_PATH=$PWD

## Remove all current appimages with the same names
if [ -f $appImageName1 ]; then
    rm $appImageName1
fi
if [ -f $appImageName2 ]; then
    rm $appImageName2
fi
if [ -f $appImageName3 ]; then
    rm $appImageName3
fi

## Create all rprc files for each of the stages
for i in ${ElfImages1[@]}; do
    $SBL_OUT2RPRC_GEN_TOOL_PATH/out2rprc.exe $i "$i.rprc"
done
for i in ${ElfImages2[@]}; do
    $SBL_OUT2RPRC_GEN_TOOL_PATH/out2rprc.exe $i "$i.rprc"
done
for i in ${ElfImages3[@]}; do
    $SBL_OUT2RPRC_GEN_TOOL_PATH/out2rprc.exe $i "$i.rprc"
done

## Generate the appimage (both unsigned and signed) for Stage 1
$MULTICORE_APPIMAGE_GEN_TOOL_PATH/MulticoreImageGen LE $devId $appImageName1 $output_args1
$PDK_INSTALL_PATH/ti/build/makerules/x509CertificateGen.sh -b $MULTICOREAPP_BIN_ABS_PATH/$appImageName1 -o $MULTICOREAPP_BIN_ABS_PATH/$appImageName1.signed -c R5 -l 0x0 -k $PDK_INSTALL_PATH/ti/build/makerules/k3_dev_mpk.pem

echo "output_args is $output_args1"
echo "Wrote output appImage to $appImageName1"
echo "Wrote signed output appImage to $appImageName1.signed"

## Generate the appimage (both unsigned and signed) for Stage 2
$MULTICORE_APPIMAGE_GEN_TOOL_PATH/MulticoreImageGen LE $devId $appImageName2 $output_args2
$PDK_INSTALL_PATH/ti/build/makerules/x509CertificateGen.sh -b $MULTICOREAPP_BIN_ABS_PATH/$appImageName2 -o $MULTICOREAPP_BIN_ABS_PATH/$appImageName2.signed -c R5 -l 0x0 -k $PDK_INSTALL_PATH/ti/build/makerules/k3_dev_mpk.pem

echo "output_args is $output_args2"
echo "Wrote output appImage to $appImageName2"
echo "Wrote signed output appImage to $appImageName2.signed"

## Generate the appimage (both unsigned and signed) for Stage 3
$MULTICORE_APPIMAGE_GEN_TOOL_PATH/MulticoreImageGen LE $devId $appImageName3 $output_args3
$PDK_INSTALL_PATH/ti/build/makerules/x509CertificateGen.sh -b $MULTICOREAPP_BIN_ABS_PATH/$appImageName3 -o $MULTICOREAPP_BIN_ABS_PATH/$appImageName3.signed -c R5 -l 0x0 -k $PDK_INSTALL_PATH/ti/build/makerules/k3_dev_mpk.pem

echo "output_args is $output_args3"
echo "Wrote output appImage to $appImageName3"
echo "Wrote signed output appImage to $appImageName3.signed"

# Restore shell to original working directory
popd > /dev/null
