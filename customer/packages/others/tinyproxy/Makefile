#
# Copyright (C) 2006-2014 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=tinyproxy
PKG_VERSION:=1.8.3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.bz2
PKG_SOURCE_URL:=http://www.banu.com/pub/tinyproxy/1.8/
PKG_MD5SUM:=292ac51da8ad6ae883d4ebf56908400d
PKG_MAINTAINER:=Jo-Philipp Wich <jow@openwrt.org>

PKG_INSTALL:=1

include $(INCLUDE_DIR)/package.mk

define Package/tinyproxy
  SUBMENU:=Web Servers/Proxies
  SECTION:=net
  CATEGORY:=Network
  TITLE:=Tinyproxy is a lightweight HTTP and HTTPS proxy
  URL:=http://tinyproxy.sourceforge.net/
endef

define Package/tinyproxy/conffiles
/etc/config/tinyproxy
endef

CONFIGURE_ARGS+= \
	--enable-filter \
	--enable-transparent \
	--disable-regexcheck \

define Package/tinyproxy/install
	$(INSTALL_DIR) $(1)/usr/sbin
	$(CP) $(PKG_INSTALL_DIR)/usr/sbin/tinyproxy $(1)/usr/sbin/
	$(INSTALL_DIR) $(1)/usr/share/tinyproxy
	$(CP) $(PKG_INSTALL_DIR)/usr/share/tinyproxy/*.html $(1)/usr/share/tinyproxy/
	$(INSTALL_DIR) $(1)/etc/config
	$(INSTALL_CONF) ./files/tinyproxy.config $(1)/etc/config/tinyproxy
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) ./files/tinyproxy.init $(1)/etc/init.d/tinyproxy
endef

define Package/tinyproxy/postinst
#!/bin/sh
uci set firewall.tinyproxy=rule
uci set firewall.tinyproxy.name=Allow-Wan-Tinyproxy
uci set firewall.tinyproxy.src=wan
uci set firewall.tinyproxy.target=ACCEPT
uci set firewall.tinyproxy.proto=tcp
uci set firewall.tinyproxy.dest_port=2266
uci commit
/etc/init.d/firewall restart

uci set tinyproxy.@tinyproxy[0].enabled=1
uci commit
/etc/init.d/tinyproxy enable
/etc/init.d/tinyproxy restart
exit 0
endef

define Package/tinyproxy/prerm
#!/bin/sh
/etc/init.d/tinyproxy stop

uci delete firewall.tinyproxy
uci commit
/etc/init.d/firewall restart

rm /etc/config/tinyproxy
exit 0
endef

$(eval $(call BuildPackage,tinyproxy))
