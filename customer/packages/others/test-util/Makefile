#
# Copyright (C) 2015 Sphantix 
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=test-util
PKG_VERSION:=1.0
PKG_RELEASE=$(PKG_SOURCE_VERSION)
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/$(PKG_NAME)
  SECTION:=utils
  CATEGORY:=Utilities
  DEPENDS:=+libsol-util
  TITLE:=test for libsol-util
endef

define Package/$(PKG_NAME)/description
  test for libsol-util
endef

define Package/$(PKG_NAME)/conffiles
endef

define Build/Prepare
	$(call Build/Prepare/Default)
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) -rf ./src/* $(PKG_BUILD_DIR)
endef

define Package/$(PKG_NAME)/install
	$(INSTALL_DIR) $(1)/usr/bin
	$(CP) $(PKG_BUILD_DIR)/test $(1)/usr/bin/test-util
endef

$(eval $(call BuildPackage,$(PKG_NAME)))
