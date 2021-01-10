#! /bin/sh
PKG_NAME="bandwidth"

function get_all_bandwidth() {
    local i=0
    while true
    do
        local status=`uci get $PKG_NAME.@userif[$i].status`

        if [ $? -ne 0 ]
        then
            break
        fi

        if [ $status -eq "off" ]
        then
            let i++
            continue
        fi

        local userif=`uci get $PKG_NAME.@userif[$i].ifname`
        local up=`uci get $PKG_NAME.@userif[$i].up`
        local down=`uci get $PKG_NAME.@userif[$i].down`

        printf "%s\t%s\t%s\n" $userif $up $down

        i++
    done
}
