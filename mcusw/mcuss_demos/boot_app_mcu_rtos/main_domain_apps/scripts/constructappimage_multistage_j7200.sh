#!/bin/bash

#####################
## User-defined paths
#####################

SDK_INSTALL_PATH=${PWD}/../../../../..
PDK_INSTALL_PATH="${SDK_INSTALL_PATH}/pdk_j721s2_09_02_00_30/packages"
SBL_REPO_PATH="${PDK_INSTALL_PATH}/ti/boot/sbl"
MULTICORE_APPIMAGE_GEN_TOOL_PATH="${SBL_REPO_PATH}/tools/multicoreImageGen/bin"
SBL_OUT2RPRC_GEN_TOOL_PATH="${SBL_REPO_PATH}/tools/out2rprc/bin"
MULTICOREAPP_BIN_PATH=../binary/bin/j7200_evm
TEST_NAME=ex01_multicore_demo_app

devId=55

###################################
## Image definitions for each stage
###################################

numStages=2

appImageName1=multicore_MCU2_0_MCU2_1_stage1.appimage
appImageName2=multicore_MPU1_0_stage2.appimage

ElfImages1=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f" \
            "mcusw_${TEST_NAME}_mcu2_1_release_strip.xer5f" \
           )
ElfImages2=("mcusw_baremetal_ex01_multicore_demo_app_mpu1_0_release.xa72fg" \
           )

CoreRprcFiles1=("mcusw_${TEST_NAME}_mcu2_0_release_strip.xer5f.rprc" \
                "mcusw_${TEST_NAME}_mcu2_1_release_strip.xer5f.rprc" \
               )
CoreRprcFiles2=("mcusw_baremetal_ex01_multicore_demo_app_mpu1_0_release.xa72fg.rprc" \
               )

CoreIds1=(10 11);
CoreIds2=(0);

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

## Create all rprc files for each of the stages
for i in ${ElfImages1[@]}; do
    $SBL_OUT2RPRC_GEN_TOOL_PATH/out2rprc.exe $i "$i.rprc"
done
for i in ${ElfImages2[@]}; do
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

# Restore shell to original working directory
popd > /dev/null
