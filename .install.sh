#!/bin/bash

set -euo pipefail

VERSION=$(apt show rocm-libs -a | grep Version: | xargs)
MAJOR=$(echo $VERSION | cut -d'.' -f1 | cut -d':' -f2 | xargs)
SEMIMAJOR=$(echo $VERSION | cut -d'.' -f2)
MINOR=$(echo $VERSION | cut -d'.' -f3 | cut -d'-' -f1)
CURR_DIR=$(realpath .)
GIT_BRANCH="rocm-${MAJOR}.${SEMIMAJOR}.x"
INSTALL_DIR="$(realpath .)/usr"

echo "Detected ROCm version: ${MAJOR}.${SEMIMAJOR}.${MINOR}"
echo "Git-branch: ${GIT_BRANCH}"
echo "Install Dir: ${INSTALL_DIR}"


mkdir -p ${INSTALL_DIR}

# Install ROCm-LLVM
cd ${CURR_DIR}
if [ ! -d ./llvm-project  ]; then
  git clone --depth=1 -b $GIT_BRANCH https://github.com/ROCm/llvm-project.git llvm-project
fi
cd ./llvm-project
mkdir -p build && cd build
cmake ../llvm  -DCMAKE_BUILD_TYPE=Release \
-DLLVM_ENABLE_ASSERTIONS=ON \
-DLLVM_TARGETS_TO_BUILD="host;AMDGPU" \
-DLLVM_ENABLE_PROJECTS="clang;lld;compiler-rt" \
-DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
make -j8; make install

# Install HIPCC
cd ${CURR_DIR}
cd ./llvm-project/amd/hipcc
mkdir -p build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=${INSTALL_DIR}
make -j8; make install

# Fetch HIP
cd ${CURR_DIR}
if [ ! -d ./hip ]; then
  git clone -b ${GIT_BRANCH} https://github.com/ROCm/HIP.git hip
fi
HIP_DIR=$(realpath ./hip)

# Install Common Language Runtimes
cd ${CURR_DIR}
if [ ! -d ./clr  ]; then
  git clone -b ${GIT_BRANCH} https://github.com/ROCm/clr.git
fi
cd clr
mkdir -p build && cd build
cmake .. -DCLR_BUILD_HIP=ON \
-DCMAKE_BUILD_TYPE=Debug \
-DCLR_BUILD_OCL=OFF \
-DHIP_COMMON_DIR="${HIP_DIR}" \
-DHIPCC_BIN_DIR="${INSTALL_DIR}/bin" \
-DCMAKE_PREFIX_PATH="/opt/rocm;/opt/rocm/llvm;${INSTALL_DIR}" \
-D__HIP_ENABLE_PCH=OFF \
-DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
-DCMAKE_INSTALL_PREFIX="${INSTALL_DIR}"
make -j8; make install
