#
# PTK release package script
# ============================
#
# Steps to use,
#
# Prerequistes,
# - Make sure all required PTK changes including documentation updates
#   are merged to GIT "master" branch
# - Make sure all required tools are installed according to build/ptk_tools_path.mak
# - Copy this file from ti-perception-toolkit/internal_docs/scripts
#   to <vsdk_install_path>/ti_components/open_compute
#
# Edit this file,
# - Modify $makeoptions to add, remove options depending on your build environment
# - uncomment or comment the functions at the end of the file
#   you want to run to make the release package
#   - this is mainly to debug individual steps if something goes wrong during the packaging
#
# Run this script.
#
#

build_libs() {
  cd ti-perception-toolkit
  make clean
  make BUILD_TARGET_MODE=yes PSDK_PATH=${PSDK_PATH} -j 2
  make release
  doxygen internal_docs/api_guide/ptk_api_guide.cfg 2> internal_docs/api_guide/doxy_warnings.txt
  doxygen internal_docs/algo_guide/ptk_algo_guide.cfg 2> internal_docs/algo_guide/doxy_warnings.txt
  cd -
}

copy_files() {
  cp -r ti-perception-toolkit/ ptk_release/
  if [ -d ptk_release]
  then
    cd ptk_release
    mkdir -p docs/content_pages/supporting_docs/
    cp ./internal_docs/algo_guide/content_pages/supporting_docs/*.pdf ./docs/content_pages/supporting_docs
    rm -rf .git out external_lib internal_docs
    cd -
  fi
}

make_tar_gz()
{
  tar zcvf ptk_release.tar.gz ptk_release/*
  rm -rf ptk_release/
}

build_libs
copy_files
make_tar_gz
