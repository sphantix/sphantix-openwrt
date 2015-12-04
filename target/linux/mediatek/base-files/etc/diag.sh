#!/bin/sh
#
# Copyright (C) 2012-2014 www.hiwifi.com
#
#

. /lib/platform.sh

status_led=""

led_set_attr() {
	[ -f "/sys/class/leds/$1/$2" ] && echo "$3" > "/sys/class/leds/$1/$2"
}

status_led_set_timer() {
	led_set_attr $status_led "trigger" "timer"
	led_set_attr $status_led "delay_on" "$1"
	led_set_attr $status_led "delay_off" "$2"
}

status_led_on() {
	led_set_attr $status_led "trigger" "none"
	led_set_attr $status_led "brightness" 255
}

status_led_off() {
	led_set_attr $status_led "trigger" "none"
	led_set_attr $status_led "brightness" 0
}

get_status_led() {
	case $(tw_board_name) in
	*)
		status_led="mtk:green:system"
		;;
	esac
}

switch_leds() {
	# old api
	if [ -e /etc/config/led_disable ]; then
		uci set hiwifi.led.state=0
		uci commit hiwifi
		rm -f /etc/config/led_disable
	fi

	led_state=$(uci get hiwifi.led.state)

	if [ $led_state -eq 0 ]; then
		setled all off
	fi
}

set_state() {
	get_status_led

	case "$1" in
	preinit)
		insmod leds-gpio
		insmod ledtrig-default-on
		insmod ledtring-timer
		status_led_set_timer 200 200
		;;
	failsafe)
		status_led_set_timer 50 50
		;;
	done)
		status_led_set_timer 1000 1000
		switch_leds
		;;
	esac
}
