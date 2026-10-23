#!/system/bin/sh
# Interactive shell on USB gadget serial port 2 (/dev/ttyGS1)
# Logs stay on ttyGS0 @115200; this port uses BAUD below.
# CDC-ACM gadget serial ignores the real baud rate, this is cosmetic.
BAUD=9600

[ -c /dev/ttyGS1 ] || { echo "ttyGS1 not present"; exit 1; }

if [ -x /system/bin/toybox ]; then
    /system/bin/toybox stty -F /dev/ttyGS1 $BAUD sane -crtscts 2>/dev/null
fi

export PATH=/usr/local/bin:/bin:/usr/bin:/usr/sbin:/vendor/bin
export HOME=/data/local/tmp
export TMPDIR=/data/local/tmp
cd $HOME 2>/dev/null

exec setsid /system/bin/sh < /dev/ttyGS1 > /dev/ttyGS1 2>&1
