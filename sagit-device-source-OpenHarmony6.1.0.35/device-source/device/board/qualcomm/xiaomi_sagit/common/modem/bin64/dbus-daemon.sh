#!/system/bin/sh

rm -rf /data/run/dbus/dbus.pid
rm -rf /data/var/run/dbus/system_bus_socket
/vendor/bin/dbus-daemon --config-file=/usr/share/dbus-1/system.conf
