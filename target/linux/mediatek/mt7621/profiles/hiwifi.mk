#
# Copyright (C) 2012-2013 hiwifi.com
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define Profile/HC5961
	NAME:=Hiwifi Wireless HC5961 Board
endef

define Profile/HC5961/Description
	Default package set compatible with Hiwifi Wireless HC5961 Board.
endef
$(eval $(call Profile,HC5961))
