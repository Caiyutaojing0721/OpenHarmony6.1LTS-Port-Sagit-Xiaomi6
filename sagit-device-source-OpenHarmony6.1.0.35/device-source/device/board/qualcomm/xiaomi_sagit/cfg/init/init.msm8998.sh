#!/system/bin/sh

/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/soc/qcom/mdt_loader.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/gpu/drm/display/drm_dp_aux_bus.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/gpu/drm/scheduler/gpu-sched.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/cfbfillrect.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/syscopyarea.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/cfbimgblt.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/sysfillrect.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/sysimgblt.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/fb_sys_fops.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/video/fbdev/core/cfbcopyarea.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/gpu/drm/drm_kms_helper.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/gpu/drm/display/drm_display_helper.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/gpu/drm/msm/msm.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/i2c/busses/i2c-qup.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/input/rmi4/rmi_core.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/input/rmi4/rmi_i2c.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/input/misc/qcom-spmi-haptics.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/md/dm-mod.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/soc/qcom/rmtfs_mem.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/input/evdev.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/hwspinlock/qcom_hwspinlock.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/soc/qcom/smp2p.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/remoteproc/qcom_pil_info.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/soc/qcom/qmi_helpers.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/remoteproc/qcom_sysmon.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/rpmsg/qcom_smd.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/remoteproc/qcom_common.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/remoteproc/qcom_q6v5.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/soc/qcom/qcom_stats.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/rfkill/rfkill.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/remoteproc/qcom_q6v5_pas.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/rtc/rtc-pm8xxx.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/remoteproc/qcom_q6v5_mss.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/power/supply/qcom_fg.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/leds/led-class.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/leds/led-class-multicolor.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/qrtr/qrtr.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/wireless/cfg80211.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/leds/rgb/leds-qcom-lpg.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/net/wireless/ath/ath.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/lib/crypto/libarc4.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/mac80211/mac80211.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/tty/serial/serial_core.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/sha256-arm64.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/sha2-ce.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/sha1_generic.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/tty/serial/msm_serial.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/sha1-ce.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/gf128mul.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/polyval-generic.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/polyval-ce.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/net/wireless/ath/ath10k/ath10k_core.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/lib/crypto/libaes.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/aes-ce-cipher.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/ghash-ce.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/net/wireless/ath/ath10k/ath10k_snoc.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/arch/arm64/crypto/aes-ce-blk.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/qrtr/qrtr-smd.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/ecc.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/ecdh_generic.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/bluetooth/bluetooth.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/bluetooth/btqca.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/bluetooth/hci_uart.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/ipv6/ipv6.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/bluetooth/bnep/bnep.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/af_alg.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/algif_skcipher.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/crypto/algif_hash.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/netfilter/nfnetlink.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/lib/libcrc32c.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/netfilter/nf_tables.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/ipv6/netfilter/nf_defrag_ipv6.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/ipv4/netfilter/nf_defrag_ipv4.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/netfilter/nf_conntrack.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/netfilter/nft_ct.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/netfilter/nft_reject.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/ipv6/netfilter/nf_reject_ipv6.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/ipv4/netfilter/nf_reject_ipv4.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/net/netfilter/nft_reject_inet.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/mm/zsmalloc.ko
/system/bin/insmod /system/lib/modules/6.0.0/kernel/drivers/block/zram/zram.ko

/system/bin/chmod 666 /sys/class/backlight/ae94000.dsi.0/brightness

# prepared dir for docker: /var /run /tmp /opt /usr
# mkdir -p /data/docker/root
# mkdir -p /data/var
# mkdir -p /data/run
# ln -s /data/run /data/var/run || true
# mkdir -p /data/run/docker/containerd
# mkdir -p /data/tmp
# mkdir -p /data/opt
# mkdir -p /data/usr

