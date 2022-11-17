#!/bin/sh
. /usr/share/libubox/jshn.sh
json_init
let _rc++
logger -t wmon "Wireless link  monitoring started for '${1}'"

while [ ${_rc} -le 20 ]
    do
        json_load $(ubus -S call network.interface.wwan status)
        json_get_var status up
        logger -t wmon "STA - '${_rc}' STATUS: '${status}'"

        if [ ${status} = '1' ]; then
              logger -t wmon "STA: - got carrier!"
              break
        fi
        sleep 1
        let ++_rc
    done
if [ ${status} = '0' ]; then
    logger -t wmon "Timeout for STA association, reconfiguring for AP mode"
    uci set wireless.sta.disabled=1
    uci commit wireless
    wifi reconf
    uci set wireless.sta.disabled=0
    uci commit wireless
fi
json_cleanup
