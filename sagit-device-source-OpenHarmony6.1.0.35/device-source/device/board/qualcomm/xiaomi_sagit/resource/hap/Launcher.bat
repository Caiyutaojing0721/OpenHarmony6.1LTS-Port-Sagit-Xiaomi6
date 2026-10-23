hdc shell mount -o remount,rw /
hdc file send Launcher.hap /system/app/com.ohos.launcher
hdc file send Launcher_Settings.hap /system/app/com.ohos.launcher
hdc shell rm -rf /data/*
hdc shell sync
hdc shell reboot