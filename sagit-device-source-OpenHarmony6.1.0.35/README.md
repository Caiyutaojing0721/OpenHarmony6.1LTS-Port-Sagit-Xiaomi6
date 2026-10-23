# 小米6 (xiaomi_sagit) OpenHarmony 6.1.0.35 设备源码与修复

> 备份一套**可以直接拷回源码树重新编译**的小米6（sagit）OpenHarmony 设备侧源码，
> 并包含修复「无开机动画 + 系统 UI 黑屏」两处关键补丁。

| 项目 | 值 |
|---|---|
| 机型 | Xiaomi Mi 6 (`xiaomi_sagit`, SoC `msm8998`) |
| 系统 | OpenHarmony **6.1.0.35**（`OpenHarmony 6.1.0.35.Midori_Momoi`） |
| manifest | `refs/tags/OpenHarmony-v6.1-LTS` |
| 备份体积 | 885 MB（2548 个文件） |
| 上次编译 | 2026-09-26，耗时 `6:14:18`，构建成功 |

---

## 1. 这个仓库是什么

上游 OpenHarmony 源码树里**不属于**这台设备适配层的部分（`foundation/`、`base/`、`third_party/` …）用 `repo sync` 就能恢复；
真正会丢、必须单独备份的是下面这些：

| 内容 | 说明 |
|---|---|
| `device-source/` | 3 个设备仓的**完整工作树**（含 git 历史）：产品定义、板级配置、SoC/驱动/GPU/firmware |
| `local-patches/` | 设备仓**之外**的 22 个仓的本地修改补丁（其中 8 个仓 11 个文件是编译必需） |
| `extra-untracked/` | 没被 git 跟踪、但编译确实要用的文件（如 `third_party/libnl/libnl/`） |
| `build/` `manifest/` | 构建命令、完整 gn args、manifest 版本，保证可复现 |
| `backup_35fix/` | 每处改动**之前**的原始文件（`.bak`），方便对照/回滚 |

---

## 2. 本仓库修复了什么

原版 35 镜像有两个症状：**开机无动画**、**进系统后黑屏（无 SystemUI）**。

### 修复一：开机动画配置装错分区 → 无开机动画

```
path not find etc/bootanimation/bootanimation_custom_config.json
→ 走默认配置 PlayVideo → file://system/etc/graphic/bootvideo.mp4
→ [StartClient]Cannot find session 100000 → StopBootAnimation（启动后 1.2s 就退出）
```

配置被装进了 `sys_prod` 分区，而该设备**从不挂载 `/sys_prod`**（`fstab` 里没有这条）。

**修复**：`device/board/qualcomm/xiaomi_sagit/cfg/BUILD.gn:479`

```gn
-  install_images = [ sys_prod_base_dir ]
+  install_images = [ "system" ]
```

配置落到 `/system/etc/bootanimation/bootanimation_custom_config.json`，
其中 `"video": ""` → 走 `PlayPicture()` 用 `bootpic.zip`，完全绕开音频问题。
改动前原件：`backup_35fix/cfg_BUILD.gn.bak`

### 修复二：缺 gn 特性导致图形栈降级编译 → 系统 UI 黑屏

35 树的 `vendor/qualcomm/xiaomi_sagit/config.json` 比能正常工作的 42 版本**少了 skia / graphic_2d 的 gn 特性**，
导致 `graphic_config.gni` 走默认值，最终 `rs_system_properties.cpp` 把 GPU 类型**强制成 `OPENGL`** 而非 Vulkan / uni-render：

| 缺失的 gn 特性（已补回） | 后果 |
|---|---|
| `graphic_2d_feature_enable_vulkan = true` | 不编译 Vulkan 路径 |
| `graphic_2d_feature_rs_enable_uni_render = true` | 关闭 uni-render |
| `graphic_2d_feature_enable_opengl` / `ace_enable_gpu` / `use_igraphics_extend_hooks` | 组合降级 |
| `skia_feature_use_vulkan = true` / `skia_feature_product = phone` | `skia_use_vulkan=false`、非手机 profile |

运行时表现：`render_service` CPU 占用≈0、日志刷 `E EGLDislay is invalid.`、
RS `layer map is empty` → 从未创建 RSLayer → 从未 queue buffer → **纯黑屏**。

**修复**：`vendor/qualcomm/xiaomi_sagit/config.json`（已与 42 版本逐字节一致）
- `librender_service.z.so`：6,392,472 → **6,539,872**（42 参照 6,540,920）
- 备份：`backup_35fix/config.json.bak`

### 还没做（可选）

`audio_policy_config.xml` 仍装在 `chip_prod`（设备不挂载）。
如需修音频，把 `vendor/qualcomm/xiaomi_sagit/hals/audio/BUILD.gn:61,70` 的
`chip_prod_base_dir` 改成 `"vendor"` 即可 —— 解析器本身会回退到 `/vendor/etc/audio/`。

---

## 3. 目录结构

```
sagit-device-source-OpenHarmony6.1.0.35/
├── README.md               ← 本文
├── apply-patches.sh        一键打补丁（读 local-patches/MAPPING.txt 自动对位）
├── device-source/          863M  三个设备仓完整工作树（含 .git）
│   ├── vendor/qualcomm/          xiaomi_sagit 产品定义、config.json、hals、hdf_config
│   ├── device/board/qualcomm/    xiaomi_sagit 板级：cfg/、kernel/、modules/(预编译 .ko)、resource/
│   └── device/soc/qualcomm/      msm8998：DTS、GPU(Mesa/EGL)、display、wifi firmware + common/
├── local-patches/          4.4M   22 个仓的 git diff + MAPPING.txt（补丁↔仓库对应表）
├── extra-untracked/        12M    未被 git 跟踪但编译要用的文件
│   ├── third_party/libnl/libnl/        ← 构建日志引用 155 次
│   ├── third_party/freetype/include/
│   └── third_party/sane-airscan/*.rej
├── backup_35fix/           244K   13 个改动前的 .bak 原件
├── build/                  new_sagit_build.sh + ohos_config.json + gn_args.txt
├── manifest/               manifest.xml + default.xml（锁定 OpenHarmony-v6.1-LTS）
└── notes/                  42↔35 对比记录、编译日志、全仓 git status、被排除文件清单
```

---

## 4. 环境要求

- 一台 64 位 Linux（验证环境：Ubuntu，16 核 / 27G 内存 / 磁盘 ≥ 300G）
- OpenHarmony 官方依赖（`repo`、JDK、Node、Python 等，见官方《快速入门》）
- 全量编译一次约 **6 小时**（`-j16` + ccache 命中后增量会快很多）

---

## 5. 使用教程

### 5.1 恢复一棵同版本源码树

```bash
repo init -u <你们的 manifest 地址> -b <分支>    # revision: refs/tags/OpenHarmony-v6.1-LTS
repo sync
```

### 5.2 拷回三个设备仓

```bash
DST=~/sagit-device-source-OpenHarmony6.1.0.35
SRC=<你的源码树>

rsync -a $DST/device-source/vendor/qualcomm/       $SRC/vendor/qualcomm/
rsync -a $DST/device-source/device/board/qualcomm/ $SRC/device/board/qualcomm/
rsync -a $DST/device-source/device/soc/qualcomm/   $SRC/device/soc/qualcomm/
```

> 这三个仓的工作树里只有 `xiaomi_sagit/`、`msm8998/`、`common/`，
> 上游的 `oneplus_fajita/`、`sdm845/` 在原树里就是删除状态，rsync 覆盖即可。

### 5.3 应用其余仓的补丁

```bash
$DST/apply-patches.sh $SRC
```

脚本按 `local-patches/MAPPING.txt` 自动把每个 `.patch` 送到对应仓库执行 `git apply`，
逐仓打印 `OK` / `FAIL`；FAIL 的会提示你手工处理。
所有 22 个补丁都已通过 `git apply --reverse --check` 反向校验。

**8 个仓 / 11 个文件是编译必需的**，缺一不可：

| 补丁 | 关键文件 |
|---|---|
| `build.patch` | `compile_standard_whitelist.json`、`check_build_target.py` |
| `arkcompiler_runtime_core.patch` | `stdlib/stdconfig.json` |
| `arkcompiler_ets_runtime.patch` | `mpl_logging.h`、`namemangler.h`、`utils.h` |
| `base_startup_appspawn.patch` | `appspawn_adapter.cpp` |
| `base_web_webview.patch` | `ohos_adapter/BUILD.gn` |
| `developtools_profiler.patch` | `ebpf_std_type.h` |
| `third_party_toybox.patch` | `mdev.c` |
| `third_party_icu.patch` | `lunar_calendar.h` |

改动前原件都在 `backup_35fix/`，可 `diff` 对照；其余 14 个补丁来源不明但无害，建议一并应用。

### 5.4 拷回未跟踪但必需的文件

```bash
rsync -a $DST/extra-untracked/third_party/ $SRC/third_party/
```

### 5.5 编译

```bash
cd $SRC
cp $DST/build/new_sagit_build.sh .
env -u LC_ALL LC_ALL=C.utf8 ./new_sagit_build.sh
```

等价于：

```bash
./build.sh --product-name xiaomi_sagit --ccache --no-prebuilt-sdk \
           --gn-args allow_sanitize_debug=true --ninja-args="-j 16" --deps-guard=false
```

> ⚠️ **必须 `env -u LC_ALL`**：`LC_ALL=C` 会让 Ruby 的 `irtoc.rb` 报错。
> 需要 `LC_ALL=C` 的场景（diff/grep 输出）请临时单独设置。

### 5.6 验收

```bash
IMG=$SRC/out/xiaomi_sagit/packages/phone/images
sudo mount -o ro,loop $IMG/system.img /mnt

# 三个必须为真
ls /mnt/system/etc/bootanimation/bootanimation_custom_config.json
grep -o '"graphic_2d_feature_enable_vulkan": *[a-z]*' /mnt/system/etc/features.json
ls -l /mnt/system/lib64/librender_service.z.so     # 期望 ≈ 6,539,872 字节

sudo umount /mnt
```

上次成功编译的镜像 md5：

| 镜像 | md5 |
|---|---|
| `system.img` | `5e7b34728ddd67f4e7088b3f1139ef2b` |
| `vendor.img`  | `c22ef1a7dbd227be53ff684943bea8d8` |

### 5.7 刷机（fastboot）

```bash
cd $IMG
fastboot flash system system.img
fastboot flash cust   vendor.img      # 本机 vendor 分区名是 cust
fastboot reboot
```

- 若报 `cannot flash logical partition` → 先 `fastboot reboot fastboot` 进 fastbootd 再刷，最后 `fastboot reboot`
- 只需刷 `system` + `cust`；`chip_prod` / `sys_prod` / `userdata` / `boot` 都不需要刷

刷完应看到开机动画（`bootpic.zip`），随后进入 SystemUI。
串口日志里**不应再出现**：

```
path not find etc/bootanimation/bootanimation_custom_config.json
[StartClient]Cannot find session 100000
E C01400/OpenGLWrapper: EGLDislay is invalid.
```

---

## 6. 常见问题

**Q：黑屏问题的诊断依据是什么？**
SceneBoard 的 JS 正常运行、但 `render_service` 2.5 小时只用了 1.1s CPU、
RS 日志 `PreProcessLayersComp: layer map is empty, drop this frame` →
从未产生任何 RSLayer → 从未 queue buffer。根因是 `config.json` 缺 gn 特性导致 GPU 路径降级。

**Q：为什么不直接修音频，要跳过 `audio_policy_config`？**
开机动画配置修复后 `"video": ""` 会让 `IsBootVideoEnabled()` 返回 false，
直接走 `PlayPicture()`，音频路径根本不参与，因此无需处理。

**Q：为什么备份里没有 `out/` 和 `prebuilts/`？**
`out/` 是构建产物，`prebuilts/` 由 `repo sync` 恢复，都不该进版本库。

**Q：`extra-untracked/third_party/libnl/libnl/` 是什么？**
原树里这个目录**没有被 git 跟踪**，但构建日志引用了 155 次 —— 属于“不备份就编不过”的东西，所以单独收在这里。

**Q：编译报 Ruby / `irtoc.rb` 错误？**
`LC_ALL` 没清掉，按 5.5 的 `env -u LC_ALL LC_ALL=C.utf8` 重来。

---

## 7. 上传 GitHub 前必读

1. **内嵌 `.git` 会吞掉内容** —— `device-source/` 下有 3 个 `.git` 目录，
   直接 `git add device-source/` 只会记录成 gitlink（子仓指针），**文件内容不会上传**。二选一：
   - 想传内容：先 `find device-source -name .git -type d -exec rm -rf {} +`（约省 499M，但会丢失上游 git 历史）
   - 想保留历史：改成 git submodule，或用 `git filter-repo` 把三个仓合并进本仓库
2. **大文件**：单文件 GitHub 硬限 **100MB**。本仓库工作区最大两个文件是
   `device/soc/qualcomm/msm8998/hardware/gpu/lib64/libgallium_dri.so`（67M）和
   `hardware/firmware/qcom/msm8998/xiaomi/modem.mbn`（54M）——
   未超限但 >50MB，建议用 [Git LFS](https://git-lfs.com/) 管理：
   ```bash
   git lfs install
   git lfs track "*.so" "*.mbn"
   ```
3. **体积**：去掉内嵌 `.git` 后约 **386M**，推 GitHub 没问题（上限 5G/次）。
4. `notes/` 里含完整编译日志与设备串口脚本，若不想公开请先删除再提交。

---

## 8. 声明

- 本仓库只包含**设备适配层与本地补丁**，不含 OpenHarmony 主体代码，
  主体代码请按 `manifest/` 里的 `refs/tags/OpenHarmony-v6.1-LTS` 从官方渠道获取（Apache-2.0）。
- `device-source/` 中的 `.git` 历史来自 `gitee.com/openharmony-dg/*` 三个仓库。
- 仓库内的 prebuilt 二进制（`.so` / `.ko` / `.mbn` / `.hap`）版权归原作者所有，
  仅用于复现编译，请勿商用。
