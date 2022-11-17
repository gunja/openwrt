#
# Copyright (C) 2013-2016 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_VERSION:=2020.10
PKG_HASH:=0d481bbdc05c0ee74908ec2f56a6daa53166cc6a78a0e4fac2ac5d025770a622

include $(INCLUDE_DIR)/u-boot.mk
include $(INCLUDE_DIR)/package.mk
include $(INCLUDE_DIR)/host-build.mk


define U-Boot/Default
  BUILD_TARGET:=imx6ull
  UBOOT_IMAGE:=u-boot.imx
  DEFAULT:=y
endef

#define U-Boot/wirelessroad_gw-imx6ull
#  NAME:=WirelessRoad GW-IMX6ULL
#  BUILD_DEVICES:=wirelessroad_gw-imx6ull
#endef

#define U-Boot/wirelessroad_stream-imx6ull
#  NAME:=WirelessRoad STREAM-IMX6ULL
#  BUILD_DEVICES:=wirelessroad_stream-imx6ull
#endef

#define U-Boot/wirelessroad_stream_wifi-imx6ull
#  NAME:=WirelessRoad STREAM-IMX6ULL
#  BUILD_DEVICES:=wirelessroad_stream_wifi-imx6ull
#endef


UBOOT_TARGETS := wirelessroad_ecspi3

UBOOT_MAKE_FLAGS += $(UBOOT_IMAGE)

Build/Exports:=$(Host/Exports)

define Build/InstallDev
	$(INSTALL_DIR) $(STAGING_DIR_IMAGE)
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/$(UBOOT_IMAGE) $(STAGING_DIR_IMAGE)/$(BUILD_VARIANT)-$(UBOOT_IMAGE)
endef

$(eval $(call BuildPackage/U-Boot))
