#
# Copyright (C) 2006-2011 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=srelay
PKG_VERSION:=0.4.8b3
PKG_RELEASE:=2

PKG_SOURCE:=$(PKG_NAME)-$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=@SF/socks-relay
PKG_MD5SUM:=a6d9521594172710ffa308d6b5dbece4

include $(INCLUDE_DIR)/package.mk

define Package/srelay
  SECTION:=net
  CATEGORY:=Network
  DEPENDS:=+libwrap
  SUBMENU:=Web Servers/Proxies
  TITLE:=A socks 4/5 proxy server
  URL:=http://www.c-wind.com/srelay/
endef

define Package/srelay/conffiles
/etc/srelay.conf
endef

CONFIGURE_ARGS += \
	--disable-thread \
	--with-libwrap="$(STAGING_DIR)/usr" \

CONFIGURE_VARS += \
	CPPFLAGS="-DLINUX $$$$CPPFLAGS" \

define Package/srelay/install	
	$(INSTALL_DIR) $(1)/usr/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(PKG_NAME) $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc
	$(INSTALL_DATA) files/$(PKG_NAME).conf $(1)/etc/
	$(INSTALL_DIR) $(1)/etc/init.d
	$(INSTALL_BIN) files/$(PKG_NAME).init $(1)/etc/init.d/$(PKG_NAME)
endef

define Package/$(PKG_NAME)/postinst
#!/bin/sh
uci set firewall.srelay=rule
uci set firewall.srelay.name=Allow-Wan-Srelay
uci set firewall.srelay.src=wan
uci set firewall.srelay.target=ACCEPT
uci set firewall.srelay.proto=tcpudp
uci set firewall.srelay.dest_port=54216
uci commit
/etc/init.d/firewall restart

/etc/init.d/srelay enable
/etc/init.d/srelay restart
exit 0
endef

define Package/$(PKG_NAME)/prerm
#!/bin/sh
/etc/init.d/srelay stop
/etc/init.d/srelay disable

uci delete firewall.srelay
uci commit

exit 0
endef

$(eval $(call BuildPackage,srelay))
