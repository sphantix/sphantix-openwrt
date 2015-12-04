#!/bin/sh
#
# Copyright (C) 2013-2014 www.hiwifi.com
#

blink_led_with_num() {
	case $1 in
	1)
		setled timer green system 100 100
		;;
	2)
		setled timer green system 500 1500
		sleep 1
		setled timer green wlan-2p4 500 1500
		;;
	3)
		setled timer green system 500 2500
		sleep 1
		setled timer green internet 500 2500
		sleep 1
		setled timer green wlan-2p4 500 2500
		;;
	4)
		setled timer green system 500 3500
		sleep 1
		setled timer green internet 500 3500
		sleep 1
		setled timer green wlan-5p 500 3500
		sleep 1
		setled timer green wlan-2p4 500 3500
		;;
	5)
		setled timer green system 500 4500
		sleep 1
		setled timer green wlan-2p4 500 4500
		sleep 1
		setled timer green turbo 500 4500
		sleep 1
		setled timer green wlan-5p 500 4500
		sleep 1
		setled timer green internet 500 4500
		;;
	esac
}

do_upgrade_bootloader() {
	local loop_min=1
	local loop_max=5

	while [[ $loop_min -le $loop_max ]];
	do
		get_image "$1" | dd bs=2k count=96 conv=sync 2>/dev/null | mtd -q -l "$SYSUPGRADE_LOG_FILE" write - "${BOOT_NAME:-image}"
		get_image "$1" | dd bs=2k count=96 conv=sync 2>/dev/null | mtd -q -l "$SYSUPGRADE_LOG_FILE" verify - "${BOOT_NAME:-image}" 2>&1 | grep -qs "Success"
		if [[ "$?" -eq 0 ]]; then
			break
		fi
		loop_min=`expr $loop_min + 1`
	done
}

platform_do_upgrade_hiwifi() {
	local board=$(tw_board_name)
	local upgrade_boot=0
	sync

	case "$board" in
	HC5761s)
		blink_led_with_num 4
		;;
	HC5642 | HB5801)
		blink_led_with_num 2
		;;
	*)
		blink_led_with_num 3
		;;
	esac

	if [ -f /tmp/img_has_boot ]; then
		img_boot_version="$(get_boot_version "$1")"
		local_boot_version="$(tw_boot_version)"

		if [[ "$img_boot_version" -gt "$local_boot_version" ]]; then
			upgrade_boot=1
		fi
	fi

	sysupgrade_log "start write flash"
	sysupgrade_log "memory"

	if [ "$SAVE_CONFIG" -eq 1 -a -z "$USE_REFRESH" ]; then
		if [ -f /tmp/img_has_boot ]; then
			if [ $upgrade_boot -eq 1 ]; then
				do_upgrade_bootloader "$1"
			fi
			get_image "$1" | dd bs=2k skip=160 conv=sync 2>/dev/null | mtd -q -l "$SYSUPGRADE_LOG_FILE" -j "$CONF_TAR" write - "${PART_NAME:-image}"
		else
			get_image "$1" | mtd -q -l "$SYSUPGRADE_LOG_FILE"  -j "$CONF_TAR" write - "${PART_NAME:-image}"
		fi
	else
		if [ -f /tmp/img_has_boot ]; then
			if [ $upgrade_boot -eq 1 ]; then
				do_upgrade_bootloader "$1"
			fi
			get_image "$1" | dd bs=2k skip=160 conv=sync 2>/dev/null | mtd -q -l "$SYSUPGRADE_LOG_FILE" write - "${PART_NAME:-image}"
		else
			get_image "$1" | mtd -q -l "$SYSUPGRADE_LOG_FILE" write - "${PART_NAME:-image}"
		fi
	fi

	if [ "$?" -eq 0 ]; then
		sysupgrade_log "sysupgrade ok"
	else
		sysupgrade_log "sysupgrade failed with $?"
	fi

	sysupgrade_log "end write flash"
}
