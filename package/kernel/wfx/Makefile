#
# Copyright (C) 2008-2012 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk

PKG_NAME:=wfx
PKG_RELEASE:=1
PKG_BUILD_PARALLEL:=1

#added backports
STAMP_CONFIGURED_DEPENDS := $(STAGING_DIR)/usr/include/mac80211-backport/backport/autoconf.h

include $(INCLUDE_DIR)/kernel.mk
include $(INCLUDE_DIR)/package.mk

define KernelPackage/wfx
 $(call KernelPackage/mac80211/Default)
  SUBMENU:=Wireless Drivers
  TITLE:=Silabs WF200 Wireless Driver
  DEPENDS:= \
	+wf200-firmware +kmod-mac80211 +kmod-cfg80211 +kmod-mmc +@DRIVER_11N_SUPPORT
  FILES:=$(PKG_BUILD_DIR)/wfx.ko
  AUTOLOAD:=$(call AutoProbe,mac80211 wfx)
  PROVIDES:=kmod-wfx
endef

define KernelPackage/wfx/description
 Silabs WF200 Wireless Driver
endef

NOSTDINC_FLAGS = \
	$(KERNEL_NOSTDINC_FLAGS) \
	-I$(PKG_BUILD_DIR) \
	-I$(STAGING_DIR)/usr/include/mac80211-backport/uapi \
	-I$(STAGING_DIR)/usr/include/mac80211-backport \
	-I$(STAGING_DIR)/usr/include/mac80211/uapi \
	-I$(STAGING_DIR)/usr/include/net/mac80211 \
	-I$(STAGING_DIR)/usr/include/mac80211 \
	-include backport/autoconf.h \
	-include backport/backport.h

PKG_MAKE_FLAGS:= CONFIG_WFX=m
NOSTDINC_FLAGS+= -DCONFIG_WFX

define Build/Compile
	echo "Called Build/Compile"
	$(MAKE)  -C "$(LINUX_DIR)" \
		$(KERNEL_MAKE_FLAGS) \
		PATH="$(TARGET_PATH)" \
		$(PKG_MAKE_FLAGS) \
		SUBDIRS="$(PKG_BUILD_DIR)" \
		NOSTDINC_FLAGS="$(NOSTDINC_FLAGS)" \
		EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
		$(PKG_EXTRA_KCONFIG) \
		M="$(PKG_BUILD_DIR)" \
		modules
#	$(KERNEL_MAKE) M="$(PKG_BUILD_DIR)" \
#        EXTRA_CFLAGS="$(EXTRA_CFLAGS)" \
#        $(PKG_EXTRA_KCONFIG) \
#        CONFIG_WFX=m \
#        modules
endef

$(eval $(call KernelPackage,wfx))
