#!/bin/bash
# This script builds the project and flashes it to the device. You can set
# different parameters to configure the build and flash process.
#
# Note:
# This script was written by Phytec in order to simplify the development
# proces.

# Set DEBUG to 1 to enable debugging, 0 to disable
DEBUG=1

# Define the build APP
#APP_NAME="csl_uart_test_app"
# Test without Linux running (without RSC Table)
#APP_NAME="ipc_echo_test_freertos"
APP_NAME="ipc_echo_baremetal_test"

# debug | release
BUILD_PROFILE=debug

# mcu1_0 | mcu1_1 | mcu2_0 | mcu2_1 | mcu3_0 | mcu3_1
CORE=mcu2_0

BOARD=j721s2_evm
SOC=j721s2
BINDIR="../binary/$APP_NAME/bin/$BOARD"
BIN_NAME="${APP_NAME}_${CORE}_${BUILD_PROFILE}.xer5f"
BIN="$BINDIR/${BIN_NAME}"

export CORE=$CORE
export SOC=$SOC
export BOARD=$BOARD
export BUILD_PROFILE=$BUILD_PROFILE

declare -A core_to_rp_reg=(
    ["mcu1_0"]="41000000.r5f"
    ["mcu2_0"]="5c00000.r5f"
    ["mcu2_1"]="5d00000.r5f"
    ["mcu3_0"]="5e00000.r5f"
    ["mcu3_1"]="5f00000.r5f"
)

declare -A core_to_name=(
    ["mcu1_0"]="mcu-r5f0_0"
    ["mcu2_0"]="main-r5f0_0"
    ["mcu2_1"]="main-r5f0_1"
    ["mcu3_0"]="main-r5f1_0"
    ["mcu3_1"]="main-r5f1_1"
)

########## Target Configuration ##########
TARGET_IP=192.168.3.11
TARGET_FW_PATH=/lib/firmware
TARGET_BIN="$TARGET_FW_PATH/$BIN_NAME"
#TARGET_RP_PATH="/sys/class/remoteproc/${core_to_rp_endpoint[$CORE]}"
TARGET_RP_PATH="/sys/class/remoteproc/remoteprocx"


########## Functions ##########

# Function to print a debug message
debug_print() {
    if [ "$DEBUG" -eq 1 ]; then
        echo "D: $1"
    fi
}

# Function: get_remoteproc_path
# Description:
#   Executes a script on a remote target via SSH to find and return the path
#   of a remoteproc device that matches a specified remoteproc name (register value).
#
# Parameters:
#   $1 - target_ip: The IP address of the target device.
#   $2 - rp_reg: The remoteproc name (register value) to match against.
#
# Returns:
#   Outputs the path of the matching remoteproc device on the target system.
#
# Usage:
#   TARGET_RP_PATH=$(get_remoteproc_path "$TARGET_IP" "${core_to_rp_reg[$CORE]}")
get_remoteproc_path() {
    local target_ip="$1"
    local rp_reg="$2"

    ssh -o StrictHostKeyChecking=no \
        -o UserKnownHostsFile=/dev/null \
        "root@$target_ip" bash << EOF
for device in /sys/class/remoteproc/*; do
    if [ -f "\$device/name" ] && [ "\$(cat "\$device/name")" == "$rp_reg" ]; then
        echo "\$device"
        break
    fi
done
EOF
}
####################################


debug_print "App     $APP_NAME"
debug_print "Board   $BOARD"
debug_print "Core    $CORE"
debug_print "Bin     $BIN_NAME"
debug_print "Target  IP: $TARGET_IP"
debug_print "Target  firmware Path: $TARGET_FW_PATH"
debug_print "Target  bin: $TARGET_BIN"
TARGET_RP_PATH=$(get_remoteproc_path "$TARGET_IP" "${core_to_rp_reg[$CORE]}")
debug_print "Target remoteproc path: $TARGET_RP_PATH"

# Check if this script is run from the correct directory
if [[ "$(pwd)" != *"pdk_j721s2_09_02_00_30/packages/ti/build" ]]; then
    echo "You are not in the correct directory."
    exit 1
fi

# Clean the application
echo "Cleaning the application..."
rm -rf ../binary$APP_NAME
#
# Run the make command with the specified build profile
echo "Building the application..."
make -s $APP_NAME -j

# Send a single ping with a timeout of 0.1 seconds (100 milliseconds)
if ! ping -c 1 -W 0.1 "$TARGET_IP" > /dev/null 2>&1; then
  echo "Ping to $TARGET_IP failed, exiting."
  exit 1
fi

# Check if the binary is available in the specified directory
if [ ! -f "$BIN" ]; then
  echo "Binary file $BIN not found, exiting."
  exit 1
fi

# Copy the generated binary to the target device
echo "Copy $BIN_NAME to $TARGET_IP"
scp -o StrictHostKeyChecking=no \
    -o UserKnownHostsFile=/dev/null \
    "$BIN" root@"$TARGET_IP":"$TARGET_FW_PATH" \
    2> >(grep -i "error")

#echo $TARGET_BIN > $TARGET_RP_PATH/firmware && \
TARGET_CMD=" \
	   (echo stop > $TARGET_RP_PATH/state || true) && \
	   ln -sf /lib/firmware/$BIN_NAME /lib/firmware/$SOC-${core_to_name[$CORE]}-fw && \
	   echo start > $TARGET_RP_PATH/state && \
	   echo 'Started $BIN_NAME on $TARGET_RP_PATH' \
	   "

debug_print "Target command: $TARGET_CMD"

ssh -o StrictHostKeyChecking=no \
    -o UserKnownHostsFile=/dev/null \
    root@"$TARGET_IP" $TARGET_CMD \
    2> >(grep -i "error")
