#!/bin/bash

MY_BOARD=$1
if [ "${MY_BOARD}" = "j721e_evm" ]
then
  MY_SOC=j721e
  OUTPUT_IMAGE_PATH=../binary/bin/j721e_evm
elif [ "${MY_BOARD}" = "j7200_evm" ]
then
  MY_SOC=j7200
  OUTPUT_IMAGE_PATH=../binary/bin/j7200_evm

fi
if [ "${MY_SOC}" == "j7200" ] || [ "${MY_SOC}" == "j721e" ]
then
  MY_OS=freertos
  MY_APP=mcusw_ex01_multicore_demo_app
  MY_BAREMETAL_APP=mcusw_baremetal_ex01_multicore_demo_app
  MY_CORE=all
  MY_PROFILE=release
  MY_CLEAN=
  MY_UART_PORT=1
  MY_BUILDDIR=../../../../build
  RUN_BUILD_STEP=1

  SDK_INSTALL_PATH=${PWD}/../../../../..
  PDK_INSTALL_PATH="${SDK_INSTALL_PATH}/pdk_j721s2_09_02_00_30/packages"
  MULTICORE_GEN_TOOL_PATH="${PDK_INSTALL_PATH}/ti/boot/sbl/tools/multicoreImageGen/bin"

  declare -a cores_j721e=("mcu2_0" "mcu2_1" "mcu3_0" "mcu3_1" "c66xdsp_1" "c66xdsp_2" "c7x_1" "mpu1_0")
  declare -a cores_exten_names_j721e=( "xer5f" "xer5f" "xer5f" "xer5f" "xe66" "xe66" "xe71" "xa72fg")
  declare -a cores_j7200=("mcu2_0" "mcu2_1" "mpu1_0")
  declare -a cores_exten_names_j7200=("xer5f" "xer5f" "xa72fg")
  USAGE="$0 [-a <app name : mcusw_ex01_multicore_demo_app> -b <board : j721e_evm, j7200_evm> -c <core> -d <PDK install directory> -r <run build step : 0 to skip> -p <build profile> -s <SDK install directory>]"

  while getopts 'a:b:c:p:r:s:t:u:d:' opt
  do
      case $opt in
          a) MY_APP=$OPTARG;;
          b) MY_BOARD=$OPTARG;;
          c) MY_CORE=$OPTARG;;
          d) PDK_INSTALL_DIR=$OPTARG;;
          r) RUN_BUILD_STEP=$OPTARG;;
          p) MY_PROFILE=$OPTARG;;
          s) SDK_INSTALL_DIR=$OPTARG;;
          t) MY_CLEAN=$OPTARG;;
          u) MY_UART_PORT=$OPTARG;;
      esac
  done

  #MY_BLD="-s -j ${MY_APP} BOARD=${MY_BOARD} SOC=${MY_SOC} BUILD_PROFILE=${MY_PROFILE} BUILD_OS_TYPE=${MY_OS}"
  MY_BLD="-s -j ${MY_APP}${MY_CLEAN} BOARD=${MY_BOARD} SOC=${MY_SOC} BUILD_PROFILE=${MY_PROFILE} BUILD_OS_TYPE=${MY_OS} UART_PORT=${MY_UART_PORT}"
  MY_BAREMETAL_BLD="-s -j ${MY_BAREMETAL_APP}${MY_CLEAN} BOARD=${MY_BOARD} SOC=${MY_SOC} BUILD_PROFILE=${MY_PROFILE} BUILD_OS_TYPE=baremetal UART_PORT=${MY_UART_PORT}"
  pushd $PWD
  cd ${MY_BUILDDIR}

  if [ "${MY_CORE}" = "all" ]
  then
      if [ ${RUN_BUILD_STEP} == 1 ]
      then
          # iterate all cores of J721e
          if [ "${MY_BOARD}" = "j721e_evm" ]
          then
            for i in "${cores_j721e[@]}"
            do
                if [ "$i" = "mpu1_0" ]
                then
                    echo make ${MY_BAREMETAL_APP} - ${MY_BOARD} "$i"
                    make ${MY_BAREMETAL_BLD} CORE="$i"
                    cd -
                    cp ${OUTPUT_IMAGE_PATH}/../${MY_SOC}/* ${OUTPUT_IMAGE_PATH}
                    rm -rf ${OUTPUT_IMAGE_PATH}/../${MY_SOC}
                    cd -
                else
                  echo make ${MY_APP} - ${MY_BOARD} "$i"
                  make ${MY_BLD} CORE="$i"
                fi
            done
          fi
          if [ "${MY_BOARD}" = "j7200_evm" ]
          then
              for i in "${cores_j7200[@]}"
              do
                if [ "$i" = "mpu1_0" ]
                then
                    echo make ${MY_BAREMETAL_APP} - ${MY_BOARD} "$i"
                    make ${MY_BAREMETAL_BLD} CORE="$i"
                    cd -
                    echo ${PWD}
                    cp ${OUTPUT_IMAGE_PATH}/../${MY_SOC}/* ${OUTPUT_IMAGE_PATH}
                    rm -rf ${OUTPUT_IMAGE_PATH}/../${MY_SOC}
                    cd -
                else
                  echo make ${MY_APP} - ${MY_BOARD} "$i"
                  make ${MY_BLD} CORE="$i"
                fi
              done
          fi
      fi
  else
      make ${MY_BLD} CORE=${MY_CORE}
  fi

  popd


  if [ "${MY_CORE}" = "all" ]
  then
      # for each of the cores, ensure that the image was properly created
      ALL_CORE_IMAGES_CREATED=1
      if [ "${MY_BOARD}" = "j721e_evm" ]
      then
        for i in "${!cores_j721e[@]}"
        do
            if [ "${cores_j721e[i]}" = "mpu1_0" ]
            then
                FILE=${OUTPUT_IMAGE_PATH}/${MY_BAREMETAL_APP}_${cores_j721e[i]}_${MY_PROFILE}.${cores_exten_names_j721e[i]}
            else
                FILE=${OUTPUT_IMAGE_PATH}/${MY_APP}_${cores_j721e[i]}_${MY_PROFILE}.${cores_exten_names_j721e[i]}
            fi
            if [ ! -f "$FILE" ]; then
                echo "Error - $FILE does not exist"
                ALL_CORE_IMAGES_CREATED=0
                break
            fi
        done
      fi
      if [ "${MY_BOARD}" = "j7200_evm" ]
      then
        for i in "${!cores_j7200[@]}"
        do
            if [ "${cores_j7200[i]}" = "mpu1_0" ]
            then
                FILE=${OUTPUT_IMAGE_PATH}/${MY_BAREMETAL_APP}_${cores_j7200[i]}_${MY_PROFILE}.${cores_exten_names_j7200[i]}
            else
                FILE=${OUTPUT_IMAGE_PATH}/${MY_APP}_${cores_j7200[i]}_${MY_PROFILE}.${cores_exten_names_j7200[i]}
            fi
            if [ ! -f "$FILE" ]; then
                echo "Error - $FILE does not exist"
                ALL_CORE_IMAGES_CREATED=0
                break
            fi
        done
      fi

      if [ $ALL_CORE_IMAGES_CREATED == 1 ]
      then
          # Since all core images were created, we can create the
          # RPRC multicore appimage now
          echo "All the Core images have been created"
          ./constructappimage_multistage_${MY_SOC}.sh
      fi

  fi
fi
