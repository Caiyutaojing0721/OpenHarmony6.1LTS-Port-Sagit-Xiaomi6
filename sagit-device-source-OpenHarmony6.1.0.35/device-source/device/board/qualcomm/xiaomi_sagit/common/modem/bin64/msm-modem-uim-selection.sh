#!/system/bin/sh

AID=$(/vendor/bin/qmicli -d qrtr://0 -p --uim-get-card-status | grep "usim (2)" -A3 | grep -E "([A-Z0-9]{2}:){15}[A-Z0-9]{2}" | xargs)
SLOT=$(/vendor/bin/qmicli -d qrtr://0 -p --uim-get-card-status | grep "Card state: 'present'" -m1 -B1 | head -n1 | cut -c7-7)

# The modem might not have come up yet, keep trying until it appears
count=0
while [ -z "$AID" ] && [ "$count" -lt "6" ]; do
	((count=count+1))
	sleep 2
	AID=$(/vendor/bin/qmicli -d qrtr://0 -p --uim-get-card-status | grep "usim (2)" -A3 | grep -E "([A-Z0-9]{2}:){15}[A-Z0-9]{2}" | xargs)
	SLOT=$(/vendor/bin/qmicli -d qrtr://0 -p --uim-get-card-status | grep "Card state: 'present'" -m1 -B1 | head -n1 | cut -c7-7)
done

if [ "$count" = "6" ]; then
	echo "Timed out waiting for modem to come up" >> /data/qmicli.log
	exit 255
fi

/vendor/bin/qmicli -p -d qrtr://0 --uim-change-provisioning-session="slot=$SLOT,activate=yes,session-type=primary-gw-provisioning,aid=$AID"
/vendor/bin/qmicli -d qrtr://0 -p --dpm-open-port="hw-data-ep-type=embedded,hw-data-ep-iface-number=1,hw-data-rx-id=2,hw-data-tx-id=10"
