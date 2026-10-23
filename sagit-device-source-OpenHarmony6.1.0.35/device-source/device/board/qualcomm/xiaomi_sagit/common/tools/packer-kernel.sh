# Copyright (c) 2023 AlgoIdeas <https://gitee.com/algoideas>.
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

#!/usr/bin/env sh

root_src_dir=$(pwd)

pushd ${root_src_dir}

if [ -f ${root_src_dir}/out/kernel/OBJ/linux-5.10/arch/arm64/boot/dts/qcom/msm8998-xiaomi-sagit.dtb ]; then
	cp ${root_src_dir}/out/kernel/OBJ/linux-5.10/arch/arm64/boot/dts/qcom/msm8998-xiaomi-sagit.dtb ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/
fi

if [ -f ${root_src_dir}/out/kernel/src_tmp/linux-5.10/xiaomi_sagit/Image.gz ]; then
	cat ${root_src_dir}/out/kernel/src_tmp/linux-5.10/xiaomi_sagit/Image.gz ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/msm8998-xiaomi-sagit.dtb > ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/zImage
	cp -rf ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/zImage ${root_src_dir}/device/board/qualcomm/xiaomi_sagit/kernel/boot/
else
rm -rf ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/zImage
cp -rf ${root_src_dir}/device/board/qualcomm/xiaomi_sagit/kernel/boot/zImage ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/
fi

rm -rf ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/bootimg.cfg
cp -rf ${root_src_dir}/device/board/qualcomm/xiaomi_sagit/kernel/boot/bootimg.cfg ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/

# initrd.img
rm -rf ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img
cp ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img.orig
mv ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img.gz
gzip -d ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img.gz
mv ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img
gzip ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img
mv ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img.gz ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img

# boot.img
rm -rf ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/boot.img
${root_src_dir}/device/board/qualcomm/xiaomi_sagit/common/tools/linux/abootimg --create ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/boot.img -k ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/zImage -r ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img -f ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/bootimg.cfg
rm -rf ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/initrd.img

mv ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img.orig ${root_src_dir}/out/xiaomi_sagit/packages/phone/images/ramdisk.img

popd
