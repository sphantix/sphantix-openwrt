#
# Copyright (C) 2013-2014 www.hiwifi.com
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

define KernelPackage/mmc-mtk-host
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Mediatek MMC host controller driver
  DEPENDS:=@TARGET_mediatek
  KCONFIG:= \
  CONFIG_MTK_MMC=y \
  CONFIG_MTK_AEE_KDUMP=n \
  CONFIG_MTK_MMC_CD_POLL=n \
  CONFIG_MTK_MMC_EMMC_8BIT=n  
  FILES:=$(LINUX_DIR)/drivers/mmc/host/mtk-mmc/mtk_sd.ko
  AUTOLOAD:=$(call AutoLoad,92,mtk_sd,1)
  $(call AddDepends/mmc)
endef

define KernelPackage/mmc-mtk-host/description
  This driver provides MTK MMC host Controller.
endef

$(eval $(call KernelPackage,mmc-mtk-host))


define KernelPackage/mtk-usb3
  TITLE:=Support for MT7621 USB3 controllers
  DEPENDS:=@TARGET_mediatek_mt7621
  KCONFIG:= \
        CONFIG_USB_XHCI_HCD \
        CONFIG_USB_XHCI_HCD_DEBUGGING=n \
        CONFIG_USB_MT7621_XHCI_HCD \
        CONFIG_PERIODIC_ENP=y \
        CONFIG_XHCI_DEV_NOTE=y
  FILES:= \
        $(LINUX_DIR)/drivers/usb/host/xhci-hcd.ko
  AUTOLOAD:=$(call AutoLoad,54,xhci-hcd,1)
  $(call AddDepends/usb)
endef

define KernelPackage/mtk-usb3/description
  support for MT7621 USB3 (XHCI) controllers
endef

$(eval $(call KernelPackage,mtk-usb3))

define KernelPackage/mtk-i2s-wm8960
  SUBMENU:=$(OTHER_MENU)
  TITLE:=Mediatek i2s-wm8960 Audio
  DEPENDS:=@TARGET_mediatek
  KCONFIG:= \
  CONFIG_RALINK_I2C=m \
  CONFIG_RALINK_I2S=m \
  CONFIG_I2S_WM8750=n \
  CONFIG_I2S_WM8960=y \
  CONFIG_I2S_IN_MCLK=y \
  CONFIG_I2S_MCLK_12MHZ=n \
  CONFIG_I2S_MCLK_12P288MHZ=y \
  CONFIG_I2S_WITH_AEC=n
  FILES:= \
      $(LINUX_DIR)/drivers/char/i2s/ralink_i2s.ko \
      $(LINUX_DIR)/drivers/char/i2c_drv.ko
  AUTOLOAD:=$(call AutoLoad,82,i2c_drv ralink_i2s,1)
endef

define KernelPackage/mtk-i2s-wm8960/description
  This driver provides MTK I2S + WM8960 Audio.
endef

$(eval $(call KernelPackage,mtk-i2s-wm8960))

define KernelPackage/mtk-hnat
  CATEGORY:=MTK Properties
  SUBMENU:=Drivers
  TITLE:=MTK Hardware NAT
  DEPENDS:=@TARGET_mediatek_mt7621
  FILES:=$(LINUX_DIR)/net/nat/hw_nat/hw_nat.ko
endef

define KernelPackage/mtk-hnat/description
  This driver support MT7621 hardware nat function.
endef

$(eval $(call KernelPackage,mtk-hnat))
