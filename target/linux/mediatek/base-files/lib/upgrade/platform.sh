#
# Copyright (C) 2012-2014 hiwifi.com
#

. /lib/mediatek.sh

PART_NAME=firmware
BOOT_NAME=u-boot
RAMFS_COPY_DATA=/lib/mediatek.sh

platform_check_image() {
	[ "$ARGC" -gt 1 ] && return 1

	rm -f "$KEXEC_UPGRADE"
	rm -f /tmp/img_has_boot

	upgrade-check $(tw_board_name) $1 0x50000
	local check_result=$?

	sysupgrade_log "sysupgrade check with $check_result"

	if [ $check_result -lt 0 -o $check_result -gt 2 ]; then
		return 1
	elif [ $check_result -eq 0 ]; then
		touch /tmp/img_has_boot
		echo "Image has uboot."
	elif [ $check_result -eq 2 ]; then
		touch $KEXEC_UPGRADE
	fi

	return 0
}

platform_do_upgrade() {
	local board=$(tw_board_name)

	case "$board" in
	*)
		platform_do_upgrade_hiwifi "$ARGV"
		;;
	esac
}

platform_pre_upgrade() {
	# disable ethernet
	ifconfig eth2 down &>/dev/null
}

disable_watchdog() {
	killall watchdog
	( ps | grep -v 'grep' | grep '/dev/watchdog' ) && {
		echo 'Could not disable watchdog'
		return 1
	}
}

append sysupgrade_pre_upgrade disable_watchdog
