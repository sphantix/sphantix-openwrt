#
# Copyright (C) 2013-2014 www.hiwifi.com
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/image.mk

BIN_DATE_DIR:=$(shell date '+%y%m%d-%H%M%S')-$(REVISION)

define imgname
$(BIN_DIR)/$(IMG_PREFIX)-$(2)-$(1)
endef

define imgstore
$(BIN_DIR)/$(BIN_DATE_DIR)/$(IMG_PREFIX)-$(2)-$(patsubst jffs2-%,jffs2,$(patsubst squashfs-%,squashfs,$(1)))
endef

define sysupname
$(call imgname,$(1),$(2))-sysupgrade.bin
endef

define signname
$(call imgname,$(1),$(2))-sign.bin
endef

define signtmpname
$(call imgname,$(1),$(2))-sign-tmp.bin
endef

define sysupstore
$(call imgstore,$(1),$(2))-sysupgrade.bin
endef

define flashsmtname
$(call imgname,$(1),$(2))-flashsmt.bin
endef

define flashsmtstore
$(call imgstore,$(1),$(2))-flashsmt.bin
endef

define HiwifiSignImg
	$(STAGING_DIR_HOST)/bin/hwf-new-sign -b $(1) -s $(2) -o $(3) -k $(TOPDIR)/hwf-keys/hiwifi.key -c $(TOPDIR)/hwf-keys/hiwifi.crt
endef

define FlashSMT
	(dd if=$(BIN_DIR)/$(IMG_PREFIX)-$(2)-uboot.bin bs=196608 conv=sync;) >$(BIN_DIR)/$(IMG_PREFIX)-$(2)-uboot-pad.bin
	$(call HiwifiSignImg,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-uboot-pad.bin, $(call sysupname,$(1),$(2)), $(call signname,$(1),$(2)))
	$(call CatFiles,$(BIN_DIR)/$(IMG_PREFIX)-$(2)-uboot.bin,196608,$(call signname,$(1),$(2)),65536,$(call signtmpname,$(1),$(2)))
	$(call CatFiles,$(call signtmpname,$(1),$(2)),327680,$(call sysupname,$(1),$(2)),20000000,$(call flashsmtname,$(1),$(2)))
endef

define mkcmdline
board=$(1) console=$(2),$(3)
endef

ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)
	VMLINUX:=$(IMG_PREFIX)-vmlinux-initramfs
	UIMAGE:=$(IMG_PREFIX)-uImage-initramfs
endif

ifeq ($(CONFIG_RALINK_MT7621),y)
define kernel_entry
-a 0x80001000 -e 0x80001000
endef
else
define kernel_entry
-a 0x80000000 -e 0x80000000
endef
endif

define CompressLzma
	$(STAGING_DIR_HOST)/bin/lzma e $(1) -lc1 -lp2 -pb2 $(2)
endef

define PatchKernelLzma
	cp $(KDIR)/vmlinux $(KDIR)/vmlinux-$(1)
	$(STAGING_DIR_HOST)/bin/patch-cmdline $(KDIR)/vmlinux-$(1) '$(strip $(2))'
	$(call CompressLzma,$(KDIR)/vmlinux-$(1),$(KDIR)/vmlinux-$(1).bin.lzma)
endef

define MkImage
	$(eval imagename=$(if $(4),$(4),$(4) Linux-$(LINUX_VERSION)))
	mkimage -A mips -O linux -T kernel -C $(1) $(call kernel_entry)\
		-n "$(imagename)" \
		-d $(2) $(3)
endef

define MkImageLzma
	$(call PatchKernelLzma,$(1),$(2))
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(1).bin.lzma,$(KDIR)/vmlinux-$(1).uImage,$(3))
endef

define MkImageLzma/initramfs
	$(call PatchKernelLzma,$(1)-initramfs,$(2))
	$(call MkImage,lzma,$(KDIR)/vmlinux-$(1)-initramfs.bin.lzma,$(KDIR)/vmlinux-$(1)-initramfs.uImage.bin,$(3))
endef

define CatFiles
	if [ `stat -c%s "$(1)"` -gt $(2) ]; then \
		echo "Warning: $(1) is too big"; \
	else if [ `stat -c%s $(3)` -gt $(4) ]; then \
		echo "Warning: $(3) is too big"; \
	else \
		( dd if=$(1) bs=$(2) conv=sync; dd if=$(3) ) > $(5); \
	fi; fi
endef

define AppendFiles
	( \
		dd if=$(1) bs=64k conv=sync; \
		dd if=$(2); \
	) > $(4); \
	if [ `stat -c%s $(4)` -gt $(3) ]; then \
		echo "Warning: sysup img is too big"; \
		rm -rf $(4); \
	fi
endef

define Sysupgrade/KRuImage
	$(call AppendFiles,$(KDIR)/vmlinux-$(2).uImage,$(KDIR)/root.$(1),$(3),$(call sysupname,$(1),$(2)))
endef

define Image/BuildKernel
	cp $(KDIR)/vmlinux.elf $(BIN_DIR)/$(VMLINUX).elf
	cp $(KDIR)/vmlinux $(BIN_DIR)/$(VMLINUX).bin
	$(call CompressLzma,$(KDIR)/vmlinux,$(KDIR)/vmlinux.bin.lzma)
	$(call MkImage,lzma,$(KDIR)/vmlinux.bin.lzma,$(KDIR)/uImage.lzma)
	cp $(KDIR)/uImage.lzma $(BIN_DIR)/$(UIMAGE).bin
ifeq ($(CONFIG_TARGET_ROOTFS_INITRAMFS),y)	
	$(call Image/Build/Initramfs)
endif	
endef

#$(1)squashfs $(2)HC5761 $(3)board=HC5761 console=ttyS1, 115200 $(4)16318464 $(5)HC5761
define BuildFirmware/hiwifi
	$(call MkImageLzma,$(2),$(3),$(5))
	$(call Sysupgrade/KRuImage,$(1),$(2),$(4))
	$(call FlashSMT,$(1),$(2))
	@mkdir -p $(BIN_DIR)/$(BIN_DATE_DIR)
	@mv $(call sysupname,$(1),$(2)) $(call sysupstore,$(1),$(2))
	@mv $(call flashsmtname,$(1),$(2)) $(call flashsmtstore,$(1),$(2))
	@cp -f $(BIN_DIR)/$(IMG_PREFIX)-$(2)-uboot.bin $(BIN_DIR)/$(BIN_DATE_DIR)/$(IMG_PREFIX)-$(2)-uboot.bin
	(cd $(DEBUG_TARGET_DIR); tar -czf $(BIN_DIR)/$(BIN_DATE_DIR)/debug-root-$(REVISION)-$(BOARD)-$(2).tgz *)
	(mkdir -p $(BIN_DIR)/debug-root)
	(cp -rf $(BIN_DIR)/$(BIN_DATE_DIR)/debug-root-$(REVISION)-$(BOARD)-$(2).tgz $(BIN_DIR)/debug-root/debug-root-$(REVISION)-$(BOARD)-$(2).tgz)
endef

define BuildFirmware/hiwifi/initramfs
	$(call MkImageLzma/initramfs,$(2),$(3),$(5))
	@cp -f $(KDIR)/vmlinux-$(2)-initramfs.uImage.bin $(BIN_DIR)/$(IMG_PREFIX)-$(2)-initramfs.uImage.bin
endef

define BuildFirmware/hiwifi_8M
	$(call BuildFirmware/hiwifi,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)),7929856,$(6))
endef

define BuildFirmware/hiwifi_16M
	$(call BuildFirmware/hiwifi,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)),16318464,$(6))
endef

define BuildFirmware/hiwifi_8M/initramfs
	$(call BuildFirmware/hiwifi/initramfs,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)),7929856,$(6))
endef

define BuildFirmware/hiwifi_16M/initramfs
	$(call BuildFirmware/hiwifi/initramfs,$(1),$(2),$(call mkcmdline,$(3),$(4),$(5)),16318464,$(6))
endef

define Image/Build/squashfs
	# Align and append a JFFS marker
	$(call prepare_generic_squashfs,$(KDIR)/root.$(1))
endef

define Image/Build/Profile/HC5961
	$(call BuildFirmware/hiwifi_16M,$(1),HC5961,HC5961,ttyS1,115200,HC5961)
endef

define Image/Build/Profile/HC5961/initramfs
	$(call BuildFirmware/hiwifi_16M/initramfs,$(1),HC5961,HC5961,ttyS1,115200,HC5961)
endef

define Image/Build/Profile/HC5662
	$(call BuildFirmware/hiwifi_16M,$(1),HC5662,HC5662,ttyS1,115200,HC5662)
endef

define Image/Build/Profile/HC5662/initramfs
	$(call BuildFirmware/hiwifi_16M/initramfs,$(1),HC5662,HC5662,ttyS1,115200,HC5662)
endef

define Image/Build/Profile/HC5642
	$(call BuildFirmware/hiwifi_8M,$(1),HC5642,HC5642,ttyS1,115200,HC5642)
endef

define Image/Build/Profile/HC5642/initramfs
	$(call BuildFirmware/hiwifi_8M/initramfs,$(1),HC5642,HC5642,ttyS1,115200,HC5642)
endef

define Image/Build/Profile/HC5661s
	$(call BuildFirmware/hiwifi_16M,$(1),HC5661A,HC5661A,ttyS1,115200,HC5661s)
endef

define Image/Build/Profile/HC5661s/initramfs
	$(call BuildFirmware/hiwifi_16M/initramfs,$(1),HC5661A,HC5661A,ttyS1,115200,HC5661s)
endef

define Image/Build/Profile/HC5761s
	$(call BuildFirmware/hiwifi_16M,$(1),HC5761A,HC5761A,ttyS1,115200,HC5761s)
endef

define Image/Build/Profile/HC5761s/initramfs
	$(call BuildFirmware/hiwifi_16M/initramfs,$(1),HC5761A,HC5761A,ttyS1,115200,HC5761s)
endef

define Image/Build/Profile/HB5801
	$(call BuildFirmware/hiwifi_8M,$(1),HB5801,HB5801,ttyS1,115200,HB5801)
endef

define Image/Build/Profile/HB5801/initramfs
	$(call BuildFirmware/hiwifi_8M/initramfs,$(1),HB5801,HB5801,ttyS1,115200,HB5801)
endef

define Image/Build/Initramfs
	$(call Image/Build/Profile/$(PROFILE)/initramfs,$(1))
endef

define Image/Build
	$(call Image/Build/$(1),$(1))
	dd if=$(KDIR)/root.$(1) of=$(BIN_DIR)/$(IMG_PREFIX)-root.$(1) bs=64k conv=sync
	$(call Image/Build/Profile/$(PROFILE),$(1))
endef

$(eval $(call BuildImage))
