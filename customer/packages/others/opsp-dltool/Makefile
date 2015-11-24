#
# Copyright (C) 2015 OpenWrt.io
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
include $(TOPDIR)/rules.mk
PKG_NAME:=opsp-dltool
PKG_VERSION:=1.0
PKG_RELEASE=1

PKG_LICENSE:=GPL-2.0
PKG_MAINTAINER:=sphantix <sphantix@gmail.com>

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SUBMENU:=Remote Control
  CATEGORY:=Network
  DEPENDS:=+curl +opkg
  TITLE:=A tool for downloading opsp-client
endef

define Package/$(PKG_NAME)/description
  A tool for downloading opsp-client depending on architecture & cpuinfo
endef

define Package/$(PKG_NAME)/conffiles
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	$(CP) -rf ./files/* $(PKG_BUILD_DIR)/
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/opsp-dltool.sh $(1)/usr/bin/opsp-dltool
	$(INSTALL_DIR) $(1)/etc/crontabs
	$(INSTALL_DATA) $(PKG_BUILD_DIR)/root $(1)/etc/crontabs
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/opsp-dltool.init $(1)/etc/init.d/opsp-dltool
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
touch /etc/crontabs/root
r=`grep '/usr/bin/opsp-dltool' /etc/crontabs/root`
if [ -z "$$r" ]; then
echo '*/1 * * * * /usr/bin/opsp-dltool' >> /etc/crontabs/root
fi
/etc/init.d/cron restart
exit 0
endef

define Package/$(PKG_NAME)/prerm
#!/bin/sh
sed -i '/\/usr\/bin\/opsp-dltool/d' /etc/crontabs/root
exit 0
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
