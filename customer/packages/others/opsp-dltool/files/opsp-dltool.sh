#! /bin/sh
version='1.0'

. /lib/functions/network.sh
. /usr/share/libubox/jshn.sh

server='192.168.3.46'
port='9004'
path='/var'

network_get_mac() {
    local iface="$1"
    network_get_device ifname $iface
    local cmd="ifconfig "$ifname" | grep -o -E '([[:xdigit:]]{1,2}:){5}[[:xdigit:]]{1,2}'"
    local tmp=`eval $cmd`
    echo ${tmp//:/}
    return $?
}

lan_mac=`network_get_mac lan`
arch=`opkg print-architecture | sed -n '3p' | awk '{print $2}'`
cpuinfo=`cat /proc/cpuinfo | grep '^system type' | awk '{print $4" "$5}'`
kernel_version=`uname -r`

r=`curl -s "$server":"$port"/ap/info/report/"$lan_mac"/"$arch"/"${cpuinfo// /%20}"/"$kernel_version"`

json_load "$r"
if [ $? -ne 0 ]; then
    echo 'error: json_load error'
    exit 1
fi

json_get_var url url

if [ -n "$url" ]; then
    ipk_name=`echo $url | awk -F '/' '{print $NF}'`
    rm -rf $path/$ipk_name
    wget -q -P $path $url
    opkg install $ipk_name
else
    echo 'error: url is empty'
    exit 1
fi
