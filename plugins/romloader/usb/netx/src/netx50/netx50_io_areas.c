/***************************************************************************
 *   Copyright (C) 2005, 2006, 2007, 2008, 2009 by Hilscher GmbH           *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#if ASIC_TYP==50


#include "netx50_io_areas.h"


NX50_ASIC_CTRL_AREA_T * const ptAsicCtrlArea = (NX50_ASIC_CTRL_AREA_T * const)Addr_NX50_asic_ctrl;
NX50_SDRAM_AREA_T * const ptSdramArea = (NX50_SDRAM_AREA_T * const)Addr_NX50_ext_sdram_ctrl;
NX50_EXT_ASYNCMEM_CTRL_AREA_T * const ptExtAsyncmemCtrlArea = (NX50_EXT_ASYNCMEM_CTRL_AREA_T * const)Addr_NX50_ext_asyncmem_ctrl;
NX50_EXTMEM_PRIORITY_CTRL_AREA_T * const ptExtmemPriorityCtrlArea = (NX50_EXTMEM_PRIORITY_CTRL_AREA_T * const)Addr_NX50_extmem_priority_ctrl;
NX50_WATCHDOG_AREA_T * const ptWatchdogArea = (NX50_WATCHDOG_AREA_T * const)Addr_NX50_watchdog;
NX50_GPIO_AREA_T * const ptGpioArea = (NX50_GPIO_AREA_T * const)Addr_NX50_gpio;
/* NX50_IOLINK_AREA_T * const ptIolinkArea = (NX50_IOLINK_AREA_T * const)Addr_NX50_iolink; */
NX50_PIO_AREA_T * const ptPioArea = (NX50_PIO_AREA_T * const)Addr_NX50_pio;
NX50_UART_AREA_T * const ptUart0Area = (NX50_UART_AREA_T * const)Addr_NX50_uart0;
NX50_UART_AREA_T * const ptUart1Area = (NX50_UART_AREA_T * const)Addr_NX50_uart1;
NX50_UART_AREA_T * const ptUart2Area = (NX50_UART_AREA_T * const)Addr_NX50_uart2;
NX50_MIIMU_AREA_T * const ptMiimuArea = (NX50_MIIMU_AREA_T * const)Addr_NX50_miimu;
NX50_SPI_AREA_T * const ptSpi0Area = (NX50_SPI_AREA_T * const)Addr_NX50_spi0;
NX50_SPI_AREA_T * const ptSpi1Area = (NX50_SPI_AREA_T * const)Addr_NX50_spi1;
NX50_I2C_AREA_T * const ptI2cArea = (NX50_I2C_AREA_T * const)Addr_NX50_i2c;
NX50_CCDC_AREA_T * const ptCcdcArea = (NX50_CCDC_AREA_T * const)Addr_NX50_ccdc;
NX50_CRC_AREA_T * const ptCrcArea = (NX50_CRC_AREA_T * const)Addr_NX50_crc;
NX50_SYSTIME_AREA_T * const ptSystimeArea = (NX50_SYSTIME_AREA_T * const)Addr_NX50_systime;
NX50_MMIO_CTRL_AREA_T * const ptMmioCtrlArea = (NX50_MMIO_CTRL_AREA_T * const)Addr_NX50_mmio_ctrl;
NX50_HOST_CONTROLLED_DMA_REGISTER_BLOCK_AREA_T * const ptHostControlledDmaRegisterBlockArea = (NX50_HOST_CONTROLLED_DMA_REGISTER_BLOCK_AREA_T * const)Addr_NX50_host_controlled_dma_register_block;
NX50_HOST_CONTROLLED_GLOBAL_REGISTER_BLOCK_AREA_T * const ptHostControlledGlobalRegisterBlockArea = (NX50_HOST_CONTROLLED_GLOBAL_REGISTER_BLOCK_AREA_T * const)Addr_NX50_host_controlled_global_register_block;
NX50_HOST_CONTROLLED_HANDSHAKE_REGISTER_BLOCK_AREA_T * const ptHostControlledHandshakeRegisterBlockArea = (NX50_HOST_CONTROLLED_HANDSHAKE_REGISTER_BLOCK_AREA_T * const)Addr_NX50_host_controlled_handshake_register_block;
NX50_NETX_CONTROLLED_DMA_REGISTER_BLOCK_AREA_T * const ptNetxControlledDmaRegisterBlockArea = (NX50_NETX_CONTROLLED_DMA_REGISTER_BLOCK_AREA_T * const)Addr_NX50_netx_controlled_dma_register_block;
NX50_NETX_CONTROLLED_GLOBAL_REGISTER_BLOCK_1_AREA_T * const ptNetxControlledGlobalRegisterBlock1Area = (NX50_NETX_CONTROLLED_GLOBAL_REGISTER_BLOCK_1_AREA_T * const)Addr_NX50_netx_controlled_global_register_block_1;
NX50_NETX_CONTROLLED_HANDSHAKE_REGISTER_BLOCK_AREA_T * const ptNetxControlledHandshakeRegisterBlockArea = (NX50_NETX_CONTROLLED_HANDSHAKE_REGISTER_BLOCK_AREA_T * const)Addr_NX50_netx_controlled_handshake_register_block;
NX50_NETX_CONTROLLED_GLOBAL_REGISTER_BLOCK_2_AREA_T * const ptNetxControlledGlobalRegisterBlock2Area = (NX50_NETX_CONTROLLED_GLOBAL_REGISTER_BLOCK_2_AREA_T * const)Addr_NX50_netx_controlled_global_register_block_2;
NX50_DMAC_CH_AREA_T * const ptDmacCh0Area = (NX50_DMAC_CH_AREA_T * const)Addr_NX50_dmac_ch0;
NX50_DMAC_CH_AREA_T * const ptDmacCh1Area = (NX50_DMAC_CH_AREA_T * const)Addr_NX50_dmac_ch1;
NX50_DMAC_CH_AREA_T * const ptDmacCh2Area = (NX50_DMAC_CH_AREA_T * const)Addr_NX50_dmac_ch2;
NX50_DMAC_CH_AREA_T * const ptDmacCh3Area = (NX50_DMAC_CH_AREA_T * const)Addr_NX50_dmac_ch3;
NX50_DMAC_REG_AREA_T * const ptDmacRegArea = (NX50_DMAC_REG_AREA_T * const)Addr_NX50_dmac_reg;
NX50_USB_CORE_AREA_T * const ptUsbCoreArea = (NX50_USB_CORE_AREA_T * const)Addr_NX50_usb_core;
NX50_USB_FIFO_AREA_T * const ptUsbFifoArea = (NX50_USB_FIFO_AREA_T * const)Addr_NX50_usb_fifo;
NX50_XPEC_AREA_T * const ptXpec0Area = (NX50_XPEC_AREA_T * const)Addr_NX50_xpec0;
NX50_XPEC_AREA_T * const ptXpec1Area = (NX50_XPEC_AREA_T * const)Addr_NX50_xpec1;
NX50_XMAC_AREA_T * const ptXmac0Area = (NX50_XMAC_AREA_T * const)Addr_NX50_xmac0;
NX50_XMAC_AREA_T * const ptXmac1Area = (NX50_XMAC_AREA_T * const)Addr_NX50_xmac1;
NX50_POINTER_FIFO_AREA_T * const ptPointerFifoArea = (NX50_POINTER_FIFO_AREA_T * const)Addr_NX50_pointer_fifo;
NX50_XPEC_IRQ_REGISTERS_AREA_T * const ptXpecIrqRegistersArea = (NX50_XPEC_IRQ_REGISTERS_AREA_T * const)Addr_NX50_xpec_irq_registers;
NX50_XC_MEM_PROT_AREA_T * const ptXcMemProtArea = (NX50_XC_MEM_PROT_AREA_T * const)Addr_NX50_xc_mem_prot;
NX50_XC_DEBUG_AREA_T * const ptXcDebugArea = (NX50_XC_DEBUG_AREA_T * const)Addr_NX50_xc_debug;
NX50_TRIGGER_SAMPLE_UNIT_AREA_T * const ptTriggerSampleUnitArea = (NX50_TRIGGER_SAMPLE_UNIT_AREA_T * const)Addr_NX50_trigger_sample_unit;
NX50_FMMUSM_AREA_T * const ptFmmusmArea = (NX50_FMMUSM_AREA_T * const)Addr_NX50_fmmusm;
NX50_BUF_MAN_AREA_T * const ptBufManArea = (NX50_BUF_MAN_AREA_T * const)Addr_NX50_buf_man;
NX50_XC_EXTBUS_SEL_AREA_T * const ptXcExtbusSelArea = (NX50_XC_EXTBUS_SEL_AREA_T * const)Addr_NX50_xc_extbus_sel;
NX50_VIC_AREA_T * const ptVicArea = (NX50_VIC_AREA_T * const)Addr_NX50_vic;


#endif	/* ASIC_TYP==50 */

