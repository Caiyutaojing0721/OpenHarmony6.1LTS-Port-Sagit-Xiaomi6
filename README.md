# OpenHarmony 6.1 LTS Port for Xiaomi 6 (sagit)

将 **OpenHarmony 6.1 LTS**（基于 OpenHarmony 6.1.0.35）移植到 **Xiaomi 6（代号 sagit）** 的个人移植项目。

本仓库包含设备适配源码、内核源码、本地补丁、构建脚本与移植笔记，目标是让 Xiaomi 6 能够编译并运行 OpenHarmony 6.1 LTS。

> ⚠️ **状态说明**：本项目为个人移植项目，非官方支持，可能存在稳定性问题。刷机有风险，请自行备份数据。

---

## 项目特性

- 基于 OpenHarmony 6.1 LTS（6.1.0.35）进行设备适配
- 适配 Xiaomi 6（sagit）的设备树、SoC 与 vendor 配置
- 集成 Linux 6.0.0 内核源码（`xiaomi6-linux6.0.0`）
- 提供完整本地补丁集（`local-patches/`），便于复现构建
- 提供构建脚本（`build/new_sagit_build.sh`）与 GN 参数（`build/gn_args.txt`）
- 保留移植过程中的笔记与日志（`notes/`），便于排查问题

---

## 目录结构

```text
OpenHarmony6.1-lts/
├── README.md
├── notes/                                    # 移植笔记、日志、嵌入式仓库来源记录
│   ├── embedded-repos-origin.txt             # 嵌入式仓库的原始远程地址与 HEAD
│   └── ...
├── sagit-device-source-OpenHarmony6.1.0.35/  # 设备适配源码（OpenHarmony 6.1.0.35）
│   ├── device-source/                        # 设备、SoC、vendor 源码
│   │   ├── device/board/qualcomm/            # 板级配置
│   │   ├── device/soc/qualcomm/              # SoC 适配
│   │   └── vendor/qualcomm/                  # vendor 配置
│   ├── local-patches/                        # 本地补丁集（按仓库名分类）
│   │   ├── MAPPING.txt                       # 补丁与仓库的对应关系
│   │   ├── arkcompiler_*.patch
│   │   ├── base_*.patch
│   │   ├── third_party_*.patch
│   │   └── ...
│   ├── manifest/                             # repo manifest
│   │   ├── default.xml
│   │   ├── manifest.xml
│   │   └── codecheck.xml
│   ├── build/                                # 构建脚本与配置
│   │   ├── new_sagit_build.sh                # 构建入口脚本
│   │   ├── ohos_config.json
│   │   └── gn_args.txt
│   ├── backup_35fix/                         # 6.1.0.35 修复前的文件备份
│   ├── backup_disable_hy_20261001/           # 禁用某项功能的备份配置
│   ├── extra-untracked/                      # 未纳入上游仓库的第三方源码
│   ├── apply-patches.sh                      # 应用本地补丁的脚本
│   └── README.md
└── xiaomi6-linux6.0.0/                       # Linux 6.0.0 内核源码
```

> 说明：`device/board/qualcomm`、`device/soc/qualcomm`、`vendor/qualcomm`、`xiaomi6-linux6.0.0` 原本为独立 Git 仓库，现已作为普通目录纳入本仓库，便于一次性 `git clone` 获得完整源码。原始远程地址与 HEAD 记录在 `notes/embedded-repos-origin.txt`。

---

## 完整编译教程

### 第 0 步：环境准备

推荐 **Ubuntu 20.04 + 16G 内存**，磁盘至少 200 GB。

```bash
sudo apt-get update
sudo apt-get install -y binutils git git-lfs gnupg flex bison gperf build-essential \
  zip curl zlib1g-dev gcc-multilib g++-multilib libc6-dev-i386 lib32ncurses5-dev \
  x11proto-core-dev libx11-dev lib32z1-dev ccache libgl1-mesa-dev libxml2-utils \
  xsltproc unzip m4 bc gnutls-bin python3.8 python3-pip ruby default-jdk \
  libssl-dev libtinfo5
```

安装 repo 工具：

```bash
curl -s https://gitee.com/oschina/repo/raw/fork_flow/repo-py3 > /usr/local/bin/repo
chmod a+x /usr/local/bin/repo
pip3 install -i https://repo.huaweicloud.com/repository/pypi/simple requests
```

配置 Git 身份：

```bash
git config --global user.name "你的名字"
git config --global user.email "你的邮箱"
git config --global credential.helper store
```

---

### 第 1 步：拉取 OpenHarmony 6.1 LTS 完整源码

本仓库只包含设备适配代码、内核和补丁，完整系统源码需要用 `repo` 拉取。

```bash
export WORK_SPACE=$HOME/ohos-build
mkdir -p $WORK_SPACE/sagit-6.1
cd $WORK_SPACE/sagit-6.1

repo init -u https://gitcode.com/ohos-porting-communities/manifest.git \
          -b OpenHarmony-6.1-LTS --no-repo-verify
repo sync -c -j8
repo forall -c 'git lfs pull'
```

---

### 第 2 步：合并适配代码与补丁

#### 2.1 应用 `local-patches/` 中的补丁

```bash
cd $WORK_SPACE/sagit-6.1
cat $HOME/OpenHarmony6.1-lts/sagit-device-source-OpenHarmony6.1.0.35/local-patches/MAPPING.txt
```

按 `MAPPING.txt` 逐个进入对应子仓库执行 `git apply`：

```bash
# 示例
cd arkcompiler/ets_frontend
git apply /path/to/OpenHarmony6.1-lts/sagit-device-source-OpenHarmony6.1.0.35/local-patches/arkcompiler_ets_frontend.patch
cd ../..
```

#### 2.2 合并设备适配源码

```bash
SRC=$HOME/OpenHarmony6.1-lts/sagit-device-source-OpenHarmony6.1.0.35/device-source
DST=$WORK_SPACE/sagit-6.1

rsync -a --exclude='.git' $SRC/device/board/qualcomm/  $DST/device/board/qualcomm/
rsync -a --exclude='.git' $SRC/device/soc/qualcomm/    $DST/device/soc/qualcomm/
rsync -a --exclude='.git' $SRC/vendor/qualcomm/        $DST/vendor/qualcomm/
```

#### 2.3 放入内核源码

先检查 sagit 设备配置期望的内核路径：

```bash
grep -r "kernel" $WORK_SPACE/sagit-6.1/device/board/qualcomm/sagit/ \
  --include="*.gni" --include="*.json" -n | head -20
```

按实际配置放入内核源码：

```bash
cp -a $HOME/OpenHarmony6.1-lts/xiaomi6-linux6.0.0 $WORK_SPACE/sagit-6.1/kernel/linux/
```

---

### 第 3 步：安装 `hb` 编译工具

方式 A（推荐）：

```bash
cd $WORK_SPACE/sagit-6.1
python3 -m pip install --user build/hb
hb help
```

方式 B（Docker，避免污染宿主机）：

```bash
docker pull swr.cn-south-1.myhuaweicloud.com/openharmony-docker/openharmony-docker:1.0.0
cd $WORK_SPACE/sagit-6.1
docker run -it -v $(pwd):/home/openharmony \
  swr.cn-south-1.myhuaweicloud.com/openharmony-docker/openharmony-docker:1.0.0
```

---

### 第 4 步：选择产品并编译

```bash
cd $WORK_SPACE/sagit-6.1
hb set
```

在菜单中用上下键选择 **sagit / Xiaomi 6**，回车确认。

```bash
hb build -f
```

`-f` 表示全量编译，第一次必须加。

编译产物位于：

```bash
out/sagit/packages/phone/images/
```

---

### 第 5 步：刷入小米 6

手机进入 fastboot 模式（关机后按住音量下 + 电源键）：

```bash
fastboot flash boot out/sagit/packages/phone/images/boot.img
# 按产品实际镜像列表刷入 system / vendor 等
# fastboot flash system out/sagit/packages/phone/images/system.img
# fastboot flash vendor out/sagit/packages/phone/images/vendor.img
fastboot reboot
```

---

## 常见问题

**1. `repo sync` 卡住或报错**
确认 `repo --version` 正常，尝试 `repo sync -c --no-tags -j4` 降低并发，支持断点续传。

**2. `hb set` 里找不到 sagit**
检查完整源码中是否存在：

```bash
ls $WORK_SPACE/sagit-6.1/device/board/qualcomm/sagit/
ls $WORK_SPACE/sagit-6.1/vendor/qualcomm/sagit/
```

不存在则说明 manifest 分支或设备代码合并有误。

**3. 补丁打不上（`git apply` 失败）**
通常是子仓库版本与补丁基线不一致，可尝试 `git apply --3way` 或 `patch -p1` 手动解决冲突。

**4. 编译报工具链路径错误**
优先改用第 3 步的 Docker 方式编译，官方镜像已预装对应工具链。

---

## 移植笔记

`notes/` 目录下保留了移植过程中的日志与记录，包括：

- `embedded-repos-origin.txt`：嵌入式仓库原始来源
- `sagit35_fix_build.log`：6.1.0.35 构建修复日志
- `BINARY_EXCLUDED.txt`：未纳入版本管理的二进制文件说明
- 其他 `*.txt`：各阶段状态记录

> 注意：部分二进制文件未纳入本仓库。若编译依赖这些文件，请参考 `notes/BINARY_EXCLUDED.txt` 自行获取并放置到对应路径。

---

## 贡献

欢迎提交 Issue 或 Pull Request。提交前请说明问题现象、复现步骤、编译环境；如涉及补丁，请附上对应仓库名与补丁说明。

---

## 致谢

- [OpenHarmony](https://gitee.com/openharmony) 官方项目
- 各上游仓库的维护者

---

## 许可证

本项目中的原创部分采用 MIT 许可证。仓库中包含的第三方代码（如 `extra-untracked/third_party/`、内核源码等）遵循其各自原始许可证，详见对应目录下的 LICENSE 文件。-- 待补充 -->

首次启动可能需要 <!-- 待补充 --> 分钟，请耐心等待。

已知问题
<!-- 待补充：列出当前已知问题，例如某项功能不工作、编译告警等 -->
<!-- 示例：某项硬件功能暂未适配 -->
<!-- 示例：编译时可能出现某类告警，不影响产物 -->
