# Copyright (c) 2023 AlgoIdeas <https://gitee.com/algoideas>.
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program. If not, see <http://www.gnu.org/licenses/>.
#
# ohos makefile to build kernel

KERNEL_VERSION = linux-6.3
KERNEL_ARCH = arm64
BUILD_TYPE = standard
OHOS_ROOT_PATH = $(realpath $(shell pwd)/../..)

KERNEL_SRC_TMP_PATH := $(KERNEL_OBJ_PATH)/kernel/src_tmp/linux-5.10
KERNEL_OBJ_TMP_PATH := $(KERNEL_OBJ_PATH)/kernel/OBJ/linux-5.10

KERNEL_SRC_PATH := $(OHOS_ROOT_PATH)/kernel/linux/${KERNEL_VERSION}

PREBUILTS_GCC_DIR := $(OHOS_ROOT_PATH)/prebuilts/gcc
CLANG_HOST_TOOLCHAIN := $(OHOS_ROOT_PATH)/prebuilts/clang/ohos/linux-x86_64/llvm/bin
KERNEL_HOSTCC := $(CLANG_HOST_TOOLCHAIN)/clang
KERNEL_PREBUILT_MAKE := make

ifeq ($(BUILD_TYPE), standard)
ifeq ($(KERNEL_ARCH), arm)
    KERNEL_ARCH := arm
    KERNEL_TARGET_TOOLCHAIN := $(PREBUILTS_GCC_DIR)/linux-x86/arm/gcc-linaro-7.5.0-arm-linux-gnueabi/bin
    KERNEL_TARGET_TOOLCHAIN_PREFIX := $(KERNEL_TARGET_TOOLCHAIN)/arm-linux-gnueabi-
endif
ifeq ($(KERNEL_ARCH), arm64)
    KERNEL_ARCH := arm64
    KERNEL_TARGET_TOOLCHAIN := $(PREBUILTS_GCC_DIR)/linux-x86/aarch64/gcc-linaro-7.5.0-2019.12-x86_64_aarch64-linux-gnu/bin
    KERNEL_TARGET_TOOLCHAIN_PREFIX := aarch64-linux-gnu-
endif
    CLANG_CC := $(CLANG_HOST_TOOLCHAIN)/clang
else ifeq ($(BUILD_TYPE), small)
    KERNEL_ARCH := arm64
    ifeq ($(CLANG_CC), "")
        CLANG_CC := $(CLANG_HOST_TOOLCHAIN)/clang
    endif
endif

KERNEL_PERL := /usr/bin/perl

KERNEL_CROSS_COMPILE :=
KERNEL_CROSS_COMPILE += CC="$(CLANG_CC)"
ifeq ($(BUILD_TYPE), standard)
    KERNEL_CROSS_COMPILE += HOSTCC="$(KERNEL_HOSTCC)"
    KERNEL_CROSS_COMPILE += PERL=$(KERNEL_PERL)
    KERNEL_CROSS_COMPILE += CROSS_COMPILE="$(KERNEL_TARGET_TOOLCHAIN_PREFIX)"
else ifeq ($(BUILD_TYPE), small)
    KERNEL_CROSS_COMPILE += CROSS_COMPILE="arm-linux-gnueabi-"
endif

KERNEL_MAKE := \
    PATH="$$PATH" \
    $(KERNEL_PREBUILT_MAKE)

KERNEL_CONFIG_FILE := $(DEVICE_PATH)/kernel/config/xiaomi_sagit_standard_defconfig
KERNEL_LOGO_FILE := $(DEVICE_PATH)/resource/logo/logo.ppm

ifeq ($(KERNEL_ARCH), arm)
KERNEL_IMAGE_FILE := $(KERNEL_SRC_TMP_PATH)/arch/arm/boot/uImage
else ifeq ($(KERNEL_ARCH), arm64)
KERNEL_IMAGE_FILE := $(KERNEL_SRC_TMP_PATH)/arch/arm64/boot/uImage
endif
DEFCONFIG_FILE := defconfig

$(KERNEL_IMAGE_FILE):
#@rm -rf $(KERNEL_SRC_TMP_PATH);mkdir -p $(KERNEL_SRC_TMP_PATH);cp -arf $(KERNEL_SRC_PATH)/* $(KERNEL_SRC_TMP_PATH)/
#@$(DEVICE_PATH)/kernel/build/patch_hdf.sh $(OHOS_ROOT_PATH) $(KERNEL_SRC_TMP_PATH)
	@cp -rf $(DEVICE_PATH)/kernel/drivers/input/rmi4/* $(KERNEL_SRC_TMP_PATH)/drivers/input/rmi4/
	@cp -rf $(KERNEL_CONFIG_FILE) $(KERNEL_SRC_TMP_PATH)/arch/arm64/configs/defconfig
	@$(KERNEL_MAKE) -C $(KERNEL_SRC_TMP_PATH) LLVM=1 LLVM_IAS=1 ARCH=$(KERNEL_ARCH) $(KERNEL_CROSS_COMPILE) $(DEFCONFIG_FILE)
	@$(KERNEL_MAKE) -C $(KERNEL_SRC_TMP_PATH) LLVM=1 LLVM_IAS=1 ARCH=$(KERNEL_ARCH) $(KERNEL_CROSS_COMPILE) -j90 dtbs Image Image.gz modules
	@$(KERNEL_MAKE) -C $(KERNEL_SRC_TMP_PATH) LLVM=1 LLVM_IAS=1 ARCH=$(KERNEL_ARCH) $(KERNEL_CROSS_COMPILE) -j90 INSTALL_MOD_PATH=$(OHOS_ROOT_PATH)/out/xiaomi_sagit/modules modules_install

.PHONY: build-kernel
build-kernel: $(KERNEL_IMAGE_FILE)
