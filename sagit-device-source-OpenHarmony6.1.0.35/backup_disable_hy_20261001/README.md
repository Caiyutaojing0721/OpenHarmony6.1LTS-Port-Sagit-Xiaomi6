# 暂时关闭合一框架(SceneBoard) — 2026-10-01

本次改动(在 `tag-OpenHarmony6.1-lts/codes` 下):

1. `vendor/qualcomm/xiaomi_sagit/config.json`
   删除最后一个 subsystem 块:
   ```json
   {
     "subsystem": "window",
     "components": [
       {
         "component": "window_manager",
         "features": [ "window_manager_use_sceneboard = true" ],
         "syscap": [ "SystemCapability.Window.SessionManager = true" ]
       }
     ]
   }
   ```
2. `device/board/qualcomm/xiaomi_sagit/cfg/preinstall/install_list.json`
   删除 `"app_dir" : "/system/app/SceneBoard"` 条目。
3. `device/board/qualcomm/xiaomi_sagit/cfg/preinstall/install_list_capability.json`
   删除 `"bundleName": "com.ohos.sceneboard"` 条目。
4. `device/board/qualcomm/xiaomi_sagit/cfg/preinstall/install_list_permissions.json`
   删除 `"bundleName": "com.ohos.sceneboard"` 权限块。

改前的原文件都在本目录:
- `config.json.bak`
- `board_preinstall/install_list.json`
- `board_preinstall/install_list_capability.json`
- `board_preinstall/install_list_permissions.json`

## 恢复合一框架
```bash
T=~/OpenHarmony6.1-lts/tag-OpenHarmony6.1-lts/codes
S=~/OpenHarmony6.1-lts/sagit-device-source-OpenHarmony6.1.0.35/backup_disable_hy_20261001
cp -p $S/config.json.bak $T/vendor/qualcomm/xiaomi_sagit/config.json
cp -p $S/board_preinstall/*.json $T/device/board/qualcomm/xiaomi_sagit/cfg/preinstall/
```
然后重新编译即可。

## 说明
- `window_manager_use_sceneboard` 在 `build/ohos_var.gni` 默认就是 false,
  所以删掉这个块 = 回到默认(旧桌面 launcher 模式)。
- 两处黑屏修复(skia/graphic_2d 标志、bootanimation 装到 system)保持不变。
- `vendor/qualcomm/xiaomi_sagit/preinstall-config/` 里也有 SceneBoard 条目,
  但整个目录没有任何 BUILD.gn 引用,不参与编译,故未改动。
