#
# Copyright (C) 2015 Sphantix 
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=ocsi
PKG_VERSION:=1.0
PKG_RELEASE=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=sphantix <sphantix@gmail.com>

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Remote Control
  CATEGORY:=Network
  DEPENDS:=+libsol-util
  TITLE:=Openwrt Package Store Platform Client
endef

define Package/$(PKG_NAME)/description
  A client for openwrt package store platform
endef

define Package/$(PKG_NAME)/conffiles
  /etc/opsp-client.conf
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
	$(CP) ./files $(PKG_BUILD_DIR)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/ocsi $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/ocsi.conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/ocsi.init $(1)/etc/init.d/ocsi
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
sed -i '/\/usr\/bin\/opsp-dltool/d' /etc/crontabs/root
exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
