#
# Copyright (C) 2015 Sphantix 
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=opsp-client
PKG_VERSION:=1.0
PKG_RELEASE=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=sphantix <sphantix@gmail.com>
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Remote Control
  CATEGORY:=Network
  DEPENDS:=+libsol-util +libstdcpp
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
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/opsp-client $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/opsp-client.init $(1)/etc/init.d/opsp-client
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/files/opsp-client.conf $(1)/etc/
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
sed -i '/\/usr\/bin\/opsp-dltool/d' /etc/crontabs/root
/etc/init.d/opsp-dltool disable
/etc/init.d/opsp-client enable
exit 0
endef

define Package/$(PKG_NAME)/prerm
#!/bin/sh
r=`grep '/usr/bin/opsp-dltool' /etc/crontabs/root`
if [ -z "$$r" ]; then
echo '*/1 * * * * /usr/bin/opsp-dltool' >> /etc/crontabs/root
fi
/etc/init.d/opsp-client disable
/etc/init.d/opsp-dltool enable
exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
