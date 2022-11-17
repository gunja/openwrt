include $(TOPDIR)/rules.mk

PKG_NAME:=chirpstack-gateway-bridge
PKG_VERSION:=4.0.0
PKG_RELEASE:=1

PKG_SOURCE:=v$(PKG_VERSION).tar.gz
PKG_SOURCE_URL:=https://github.com/chirpstack/chirpstack-gateway-bridge/archive/refs/tags/

PKG_HASH:=fba1969c878c73b42cb935aa511bfc357e461f6c3ef31fe61b1dec832c70b177

PKG_MAINTAINER:=Sergey Moryakov <sergey@nqnet.org>
PKG_LICENSE:=MIT
PKG_LICENSE_FILES:=LICENSE

PKG_BUILD_DIR:=$(BUILD_DIR)/chirpstack-gateway-bridge-$(PKG_VERSION)
PKG_BUILD_DEPENDS:=golang/host
PKG_BUILD_PARALLEL:=1
PKG_USE_MIPS16:=0

GO_PKG_LDFLAGS_X:=\
	main.version=v$(PKG_VERSION) \
	main.commit=$(PKG_VERSION)  \
	main.builtBy=openwrt \

include $(INCLUDE_DIR)/package.mk
include ../../../feeds/packages/lang/golang/golang-package.mk

define Package/chirpstack-gateway-bridge/Default
  TITLE:=chirpstack gateway bridge
  URL:=https://github.com/chirpstack/chirpstack-gateway-bridge
endef

define Package/chirpstack-gateway-bridge
$(call Package/chirpstack-gateway-bridge/Default)
  SECTION:=LORA
  CATEGORY:=LoraWAN Packages
  SUBMENU:=LoRaWAN
  DEPENDS:=$(GO_ARCH_DEPENDS)
endef

define Package/chirpstack-gateway-bridge/description
  chirpstack-gateway-bridge is GateWay Client for ChirpStack LoRA server.
endef
ifeq ($(ARCH),aarch64)
	GOARCH:=arm64
endif

ifeq ($(ARCH),arc)
	GOARCH:=risc
endif

ifeq ($(ARCH),arc)
	GOARCH:=risc
endif

ifeq ($(ARCH),arm)
	GOARCH:=arm
endif

ifeq ($(ARCH),armeb)
	GOARCH:=armbe
endif

ifeq ($(ARCH),i386)
	GOARCH:=386
endif

ifeq ($(ARCH),mips)
	GOARCH:=mips
endif

ifeq ($(ARCH),mips64)
	GOARCH:=mips64
endif

ifeq ($(ARCH),mipsel)
	GOARCH:=mipsle
endif

ifeq ($(ARCH),powerpc)
	GOARCH:=ppc64
endif

define Build/Compile
  cd $(PKG_BUILD_DIR) && \
  GOOS="linux" \
  GOARCH="$(GOARCH)" \
  PKGVER="$(PKG_VERSION)" \
  PKGNAME="chrpstack-gateway-bridge" \
  go build  -ldflags "-s -w -X main.version=$(PKGVER)" -o chirpstack-gateway-bridge cmd/chirpstack-gateway-bridge/main.go
endef

define Package/chirpstack-gateway-bridge/install
	# $(call GoPackage/Package/Install/Bin,$(PKG_INSTALL_DIR))

	$(INSTALL_DIR) $(1)/usr/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/chirpstack-gateway-bridge $(1)/usr/bin/
	$(INSTALL_DIR) $(1)/etc/chirpstack-gateway-bridge/
	$(INSTALL_DATA) ./files/chirpstack-gateway-bridge.toml $(1)/etc/chirpstack-gateway-bridge/
	$(INSTALL_DIR) $(1)/etc/init.d/
	$(INSTALL_BIN) ./files/cs-gateway.init $(1)/etc/init.d/cs-gateway
endef

$(eval $(call BuildPackage,chirpstack-gateway-bridge))
