#
# Copyright (C) 2015 Sphantix 
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=libsol_util
PKG_VERSION:=1.0
PKG_RELEASE=$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/libsol_util
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=A lib which contains kinds of utilities
  URL:=http://www.wifidog.org
endef

define Package/libsol_util/description
	This is a dynamic library which implies utilities of string,
	timer, memory and so on.
endef

define Package/libsol_util/conffiles
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -rf ./src/* $(PKG_BUILD_DIR)
endef

define Package/libsol_util/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libsol-util.so* $(1)/usr/lib/
endef

$(eval $(call BuildPackage,libsol_util))
