#! /bin/sh
version='1.0'

. /lib/functions/network.sh
. /usr/share/libubox/jshn.sh

server='opsp.hi-wifi.cn'
port='80'
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
kernel_version=`opkg info kernel | grep "Version:" | awk '{print $NF}'`

r=`curl -s "$server":"$port"/ap/info/report/"$lan_mac"/"$arch"/"${cpuinfo// /%20}"/"$kernel_version"`

json_load "$r"
if [ $? -ne 0 ]; then
    echo 'error: json_load error'
    exit 1
fi

json_select urls 
if [ $? -ne 0 ]; then
    echo 'error: json_select error'
    exit 1
fi
local i=1
urls=''
while json_get_type type $i && [ "$type" = string ]; do
    json_get_var tmp "$((i++))"
    urls=`echo $urls $tmp`
done
echo "urls: "
echo $urls

if [ -n "$urls" ]; then
    for url in $urls; do
        ipk_name=`echo $url | awk -F '/' '{print $NF}'`
        rm -rf $path/$ipk_name
        wget -q -P $path $url
        if [ $? -ne 0 ]; then
            echo "wget error!"
            exit 1
        else
            opkg install $path/$ipk_name
        fi
    done
else
    echo 'error: urls is empty'
    exit 1
fi
