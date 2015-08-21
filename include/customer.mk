# Makefile for OpenWrt
#
# Copyright (C) 2015 Hyman hang
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# This file is customer define included by other Makefile

ifneq ($(__customer_inc),1)
__customer_inc=1

LAST_PROJECT_COOKIE := $(TOPDIR)/.last_project
LAST_PROJECT        := $(strip $(shell cat $(LAST_PROJECT_COOKIE) 2>/dev/null))
ifeq ($(strip $(PROJECT)),)
PROJECT=$(LAST_PROJECT)
export PROJECT
endif

CUSTOMER_DIR:=$(TOPDIR)/customer
CUSTOMER_COMMON_DIR:=$(CUSTOMER_DIR)/common
CUSTOMER_PROJECT_DIR:=$(CUSTOMER_DIR)/project

endif
