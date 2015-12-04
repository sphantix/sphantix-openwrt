#!/bin/sh
# mediatek.sh
# controlling script of mediatek wireless driver for openwrt system
# (c)2013 Beijing Geek-Geek Technology Co., Ltd.

append DRIVERS "mediatek"

. /lib/platform.sh
. /usr/share/libubox/jshn.sh

ubus_wireless_cmd() {
	local cmd="$1"
	local dev="$2"

	json_init
	[ -n "$2" ] && json_add_string device "$2"
	ubus call wireless "$1" "$(json_dump)"
}

start_mediatek_vif() {
	local vif="$1"
	local ifname="$2"

	local net_cfg
	net_cfg="$(find_net_config "$vif")"
	[ -z "$net_cfg" ] || start_net "$ifname" "$net_cfg"

	set_wifi_up "$vif" "$ifname"
}

find_mediatek_phy() {
	local device="$1"

	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled = 0 ] || continue
		return 0
	done

	return 1
}

sync_mediatek() {
	local devdst="$1"
	local devsrc="$2"

	config_get disabled "$devsrc" disabled
	config_get txpwr "$devsrc" txpwr
	config_get vifs "$devsrc" vifs

	uci set wireless.${devdst}.disabled="$disabled"
	#uci set wireless.${devdst}.txpwr="$txpwr"

	for vif in $vifs; do
		config_get ifname "$vif" ifname
		config_get ssid "$vif" ssid
		config_get hidden "$vif" hidden
		config_get macfilter "$vif" macfilter
		config_get encryption "$vif" encryption
		config_get key "$vif" key
		config_get rekey "$vif" rekey
		config_get ssidprefix "$vif" ssidprefix

		config_get_bool disabled "$vif" disabled 0
		config_get_bool isolate "$vif" isolate 0

		[ "$ifname" = "ra0" ] || break
		maclist=$(uci -q get wireless.master.maclist)
		uci -q delete wireless.masterac.maclist
		for mac in $maclist; do
			uci add_list wireless.masterac.maclist="$mac"
		done
		uci set wireless.masterac.ssid="$ssidprefix""$ssid""_5G"
		[ -n "$hidden" ] && uci set wireless.masterac.hidden="$hidden"
		[ -n "$macfilter" ] && uci set wireless.masterac.macfilter="$macfilter"
		[ -n "$encryption" ] && uci set wireless.masterac.encryption="$encryption"
		[ -n "$key" ] && uci set wireless.masterac.key="$key"
		[ -n "$rekey" ] && uci set wireless.masterac.rekey="$rekey"
		[ -n "$disabled" ] && uci set wireless.masterac.disabled="$disabled"
		[ -n "$isolate" ] && uci set wireless.masterac.isolate="$isolate"
	done
	uci commit wireless
}

scan_mediatek() {
	local device="$1"
	config_get syncdev "$device" syncdev
	config_get vifs "$device" vifs

	[ "$2" = "disable" ] || return 

	[ "$device" = "radio0" ] && {
		for vif in $vifs; do
			config_get ifname "$vif" ifname
			[ "$ifname" = "ra0" ] || break
			config_get ssidprefix "$vif" ssidprefix
			config_get ssid "$vif" ssid
			uci set wireless.@wifi-iface[0].ssid="$ssidprefix""${ssid##$ssidprefix}"
		done
		return 0
	}

	[ -n "$syncdev" ] && sync_mediatek "$device" "$syncdev"
	return 0
}

enable_iface() {
	local device="$1"
	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled -eq 0 ] || continue

		config_get ifname "$vif" ifname
		[ "$ifname" = "$2" ] && {
			config_get network "$vif" network

			echo "$(date '+%F %X') start $2" >> /tmp/wifi.log

			ubus_wireless_cmd start $ifname
			[ "$network" = "wan" ] || {
				start_mediatek_vif "$vif" "$ifname"
			}
		}
	done
}

disable_iface() {
	local device="$1"
	include /lib/network
	config_get vifs "$device" vifs
	for vif in $vifs; do
		config_get ifname "$vif" ifname
		[ "$ifname" = "$2" ] && {
			config_get network "$vif" network

			echo "$(date '+%F %X') stop $2" >> /tmp/wifi.log

			ubus_wireless_cmd stop $ifname
			brctl show | grep -qs "$ifname"
			if [ $? -eq 0 ]; then
				unbridge1 $ifname $network 2>/dev/null
			fi
		}
	done
}

enable_mediatek() {
	local device="$1"
	config_get vifs "$device" vifs
	config_get_bool repeater "$device" repeater 0
	config_get_bool smt "$device" smt 0

	find_mediatek_phy "$device" || return 0

	echo "$(date '+%F %X') $device enable" >> /tmp/wifi.log

	ubus_wireless_cmd up $device

	for vif in $vifs; do
		config_get_bool disabled "$vif" disabled 0
		[ $disabled -eq 0 ] || continue

		config_get ifname "$vif" ifname
		config_get network "$vif" network

		[ "$network" = "wan" ] || {
			start_mediatek_vif "$vif" "$ifname"
		}
	done
	
	[ $repeater -eq 1 ] || ubus_wireless_cmd sync

	return 0
}

disable_mediatek() {
	local device="$1"
	config_get vifs "$device" vifs
	
	set_wifi_down "$device"
	ubus_wireless_cmd down $device

	echo "$(date '+%F %X') $device disable" >> /tmp/wifi.log

	include /lib/network
	for vif in $vifs; do
		config_get ifname "$vif" ifname
		config_get network "$vif" network

		brctl show | grep -qs "$ifname"
		if [ $? -eq 0 ]; then
			unbridge1 $ifname $network 2>/dev/null
		fi
	done

	return 0
}

detect_mediatek_ra() {
	local device="$1"

	mac_suffix=$(tw_get_mac)
	mac_suffix=${mac_suffix:6:6}

	cat <<EOF
config wifi-device $device
	option type	mediatek
	option channel	0
	option txpwr	max

config wifi-iface	master
	option device	$device
	option ifname	ra0
	option network	lan
	option mode	ap
	option ssid	HiWiFi_${mac_suffix}
	option encryption none

config wifi-iface	slave
	option device	$device
	option ifname	apcli0
	option network	lan
	option mode	sta
	option ssid	HiWiFi_${mac_suffix}
	option encryption none
	option disabled 1

config wifi-iface	guest
	option device	$device
	option ifname	ra1
	option network	lan
	option mode	ap
	option ssid	HiWiFi_guest
	option encryption none
	option isolate 	1
	option disabled 1

EOF
}

detect_mediatek_rai() {
	local device="$1"

	mac_suffix=$(tw_get_mac)
	mac_suffix=${mac_suffix:6:6}

	cat <<EOF
config wifi-device $device
	option type	mediatek
	option channel	149
	option txpwr	max
	option syncdev	radio0

config wifi-iface	masterac
	option device	$device
	option ifname	rai0
	option network	lan
	option mode	ap
	option ssid	HiWiFi_${mac_suffix}
	option encryption none

config wifi-iface	slaveac
	option device	$device
	option ifname	apclii0
	option network	lan
	option mode	sta
	option ssid	HiWiFi_${mac_suffix}
	option encryption none
	option disabled 1

config wifi-iface	guestac
	option device	$device
	option ifname	rai1
	option network	lan
	option mode	ap
	option ssid	HiWiFi_guest_5G
	option encryption none
	option isolate 	1
	option disabled 1
EOF
}

detect_mediatek() {
	config_load wireless
	config_get type "radio0" type
	[ -n "$type" ] || detect_mediatek_ra "radio0"
	config_get type "radio1" type
	[ -n "$type" ] || {
		grep -qs "^ *rai0:" /proc/net/dev && detect_mediatek_rai "radio1"
	}
}
