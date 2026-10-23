#!/bin/bash

# Copyright (c) 2024 AlgoIdeas <https://gitee.com/algoideas>.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#../../device/board/qualcomm/xiaomi_sagit/kernel/build/rebuild_kernel.sh ../../kernel/linux/linux-6.3 /mnt/minjie.yu/openharmony/OpenHarmony-6.0-Release/out /mnt/minjie.yu/openharmony/OpenHarmony-6.0-Release/out/xiaomi_sagit/packages/phone/images /mnt/minjie.yu/openharmony/OpenHarmony-6.0-Release/device/board/qualcomm/xiaomi_sagit vendor/qualcomm/xiaomi_sagit true

echo "Build kernel..."
set -e
export PATH=$(realpath ${4}/../../../../)/prebuilts/clang/ohos/linux-x86_64/llvm/bin/:$PATH
export PRODUCT_COMPANY=qualcomm
export DEVICE_NAME=xiaomi_sagit
export KERNEL_OBJ_PATH=$2
export IMAGES_PATH=$3
export DEVICE_PATH=$4
export PRODUCT_PATH=$5
export RAMDISK_ENABLE=$6
export KBUILD_OUTPUT=${2}/kernel/OBJ/linux-5.10
mkdir -p ${2}/kernel/OBJ/linux-5.10
mkdir -p ${3}
make -f ${4}/kernel/build/rekernel.mk

if [ -f ${2}/kernel/OBJ/linux-5.10/arch/arm64/boot/Image.gz ]; then
	mkdir -p ${2}/kernel/src_tmp/linux-5.10/xiaomi_sagit
	cp -rf ${2}/kernel/OBJ/linux-5.10/arch/arm64/boot/Image.gz ${2}/kernel/src_tmp/linux-5.10/xiaomi_sagit/
fi
