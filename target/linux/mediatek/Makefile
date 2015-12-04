#
# Copyright (C) 2012-2014 www.hiwifi.com
#
# This is free software. licensed under the GNU General Public License v2.
# See /LICENSE for more information
#
include $(TOPDIR)/rules.mk

ARCH:=mipsel
BOARD:=mediatek
BOARDNAME:=Mediatek Soc
SUBTARGETS:=mt7621 mt7628
FEATURES:=squashfs

LINUX_VERSION:=3.3.8

include $(INCLUDE_DIR)/target.mk

DEFAULT_PACKAGES += wireless-tools \
		    kmod-leds-gpio kmod-ledtrig-gpio kmod-ledtrig-timer kmod-ledtring-netdev \
		    kmode-ledtrig-default-on kmod-gpio-button-hotplug util-switch upgrade-check

define Target/Description
	Build firmware images for Mediatek Soc(MT7621A/MT7628) Boards
endef

$(eval $(call BuildTarget))
