# This environment variable is used to select the appropriate board in the name
# of the linux boot and root partitions for scripts in this directory.
# This sets default, but could be overwritten from environment variable.
: "${TI_DEV_BOARD:=j721s2-evm}"
: "${SOC:=j721s2}"
: "${TISDK_IMAGE:=adas}"
