#!/bin/bash


pushd () {
    command pushd "$@" > /dev/null
}

popd () {
    command popd "$@" > /dev/null
}

export THIS_SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" >/dev/null && pwd)"

export REPO_WORK_DIR=${THIS_SCRIPT_DIR}/../
# declare -a kernels_target=($(find ${REPO_WORK_DIR}/src/*/* -maxdepth 0 -type d ! \( -iname "common" \) | grep -o -P '.{0}src/.*' | cut -c 5-))

declare -a kernels_target=($(find ${REPO_WORK_DIR}/src/fft_c7x/* \
   -maxdepth 0 -type d | grep -o "src/.*" | cut -c 5-))

kernels_target+=($(find ${REPO_WORK_DIR}/src/fft_c7xmma/* \
   -maxdepth 0 -type d | grep -o "src/.*" | cut -c 5-))

echo "${kernels_target[@]}"


# for i in "${kernels_target[@]}"; do
gen_test_case(){
    i=${1} # get kernel name
    mkdir -p ${THIS_SCRIPT_DIR}/../test/${i}/test_data
    if [ -d "${THIS_SCRIPT_DIR}/../test/FFTLIB_idat_gen/${i}" ]; then

	echo ""
	echo "##############################################################"
	echo ""
	echo " Generating test case headers for ${i}"
	
	echo "##############################################################"
	echo ""
	
	pushd ${THIS_SCRIPT_DIR}/../test/FFTLIB_idat_gen/${i}
	python3 gen_data.py -a
	popd
	echo ""
    fi
}
# done

export -f gen_test_case

parallel --gnu -j0  gen_test_case ::: ${kernels_target[@]}


