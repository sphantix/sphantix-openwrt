#!/bin/sh
#
# Copyright (C) 2012-2014 www.hiwifi.com
#

hiwifi_uboot_version() {
	dd if=/dev/mtdblock0 ibs=1 skip=196544 2>/dev/null | dd of=/tmp/sysinfo/boot_version bs=20 skip=0 count=1 2>/dev/null
	cat /tmp/sysinfo/boot_version | grep -qs "HIWIFI_BOOT"
	if [ $? -ne 0 ]; then
		echo "HIWIFI_BOOT 00000000" >/tmp/sysinfo/boot_version
	fi
}

mediatek_board_detect() {
	local machine
	local name

	machine=$(awk 'BEGIN{FS="[ \t]+:[ \t]"} /machine/ {print $2}' /proc/cpuinfo)

	case "$machine" in
	*"HiWiFi Wireless HC5961 Board")
		name="HC5961"
		;;
	*"HiWiFi Wireless HC5662 Board")
		name="HC5662"
		;;
	*"HiWiFi Wireless HC5642 Board")
		name="HC5642"
		;;
	*"HiWiFi Wireless HC5661s Board")
		name="HC5661s"
		;;
	*"HiWiFi Wireless HC5761s Board")
		name="HC5761s"
		;;
	*"HiWiFi Wireless HB5801 Board")
		name="HB5801"
		;;	
	esac

	[ -z "$name" ] && name="unkown"

	[ -e "/tmp/sysinfo/" ] || mkdir -p "/tmp/sysinfo/"

	hiwifi_uboot_version

	echo "$name" > /tmp/sysinfo/board_name
	echo "$machine" > /tmp/sysinfo/model
}

get_board_name_up() {
	get_image "$@" | dd bs=32 skip=1 count=1 2>/dev/null
}

get_board_name_smt() {
	get_image "$@" | dd bs=2k skip=160 count=1 2>/dev/null | dd bs=32 skip=1 count=1 2>/dev/null
}

get_boot_version() {
	get_image "$@" | dd ibs=1 skip=196544 2>/dev/null | dd bs=20 skip=0 count=1 2>/dev/null | awk '{print $2}'
}

tw_board_name() {
	local name

	[ -f /tmp/sysinfo/board_name ] && name=$(cat /tmp/sysinfo/board_name)
	[ -z "$name" ] && name="unknown"

	echo "$name"
}

tw_boot_version() {
	cat /tmp/sysinfo/boot_version | awk '{print $2}'
}

tw_get_mac() {
	board=$(tw_board_name)
	case "$board" in
		*)
			ifconfig eth2 | grep HWaddr | awk '{ print $5 }' | awk -F: '{printf $1$2$3$4$5$6}'
		;;
	esac
}

