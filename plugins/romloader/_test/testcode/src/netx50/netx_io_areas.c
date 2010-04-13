
#include "netx_io_areas.h"

const PI2C_AREA_T ptNetXI2CArea = (PI2C_AREA_T)Addr_i2c;

const PSPI_AREA_T ptNetXSpi0Area = (PSPI_AREA_T)Addr_spi0;

const PUART_AREA_T ptNetXUartArea = (PUART_AREA_T)Addr_uart0;

const PGPIO_AREA_T ptNetXGpioArea = (PGPIO_AREA_T)Addr_gpio;

const PUSB_AREA_T ptNetXUsbArea = (PUSB_AREA_T)Addr_usb_core;

const PASIC_CTRL_AREA_T ptNetXAsicCtrlArea = (PASIC_CTRL_AREA_T)Addr_asic_ctrl;

const PNETX_GLOBAL_REG_BLOCK_1_AREA_T ptNetXGlobalRegBlock1Area = (PNETX_GLOBAL_REG_BLOCK_1_AREA_T)Adr_pci_window_low0;

const PNETX_HANDSHAKE_REG_BLOCK_AREA_T ptNetxHandshakeRegBlockArea = (PNETX_HANDSHAKE_REG_BLOCK_AREA_T)Addr_netx_controlled_handshake_register_block;

const PHOST_GLOBAL_REG_BLOCK_AREA_T ptHostGlobalRegBlockArea = (PHOST_GLOBAL_REG_BLOCK_AREA_T)Adr_pci_base;

const PHOST_HANDSHAKE_REG_BLOCK_AREA_T ptHostHandshakeRegBlockArea = (PHOST_HANDSHAKE_REG_BLOCK_AREA_T)Addr_host_controlled_handshake_register_block;

const PXMAC_AREA_T ptNetXXMac0Area = (PXMAC_AREA_T)Addr_xmac0;

const PXPEC_AREA_T ptNetXXPec0Area = (PXPEC_AREA_T)Addr_xpec0;

const PNETX_GLOBAL_REG_BLOCK_2_AREA_T ptNetXGlobalRegBlock2Area = (PNETX_GLOBAL_REG_BLOCK_2_AREA_T)Addr_netx_controlled_global_register_block_2;

const PEXT_ASYNC_MEM_CTRL_AREA_T ptNetXExtAsyncMemCtrlArea = (PEXT_ASYNC_MEM_CTRL_AREA_T)Addr_ext_asyncmem_ctrl;

const PEXT_SDRAM_CTRL_AREA_T ptNetXExtSdramCtrlArea = (PEXT_SDRAM_CTRL_AREA_T)Addr_ext_sdram_ctrl;

const PPCI_SHADOW_CONFIG_AREA_T ptNetXPciShadowConfigArea = (PPCI_SHADOW_CONFIG_AREA_T)Addr_netx_controlled_pci_configuration_shadow_register_block;

const PRNG_AREA_T ptNetXRngArea = (PRNG_AREA_T)Adr_dmac_rng_ctrl;
