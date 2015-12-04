#
# Copyright (C) 2012-2014 www.hiwifi.com
#

SUBTARGET:=mt7628
BOARDNAME:=MT7628 based boards
CFLAGS:=-Os -pipe -mips32r2 -mtune=24kec -msoft-float -fno-caller-saves

define Target/Description
	Build firmware images for MediaTek MT7628 based boards.
endef

