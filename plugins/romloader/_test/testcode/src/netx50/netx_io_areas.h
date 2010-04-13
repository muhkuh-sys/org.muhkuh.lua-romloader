
#include "netx_regdef.h"

#ifndef __NETX_IO_AREAS_H__
#define __NETX_IO_AREAS_H__

//-------------------------------------

// I2C area, netX50

typedef struct {
	volatile unsigned long ulMcr;
	volatile unsigned long ulScr;
	volatile unsigned long ulCmd;
	volatile unsigned long ulMdr;
	volatile unsigned long ulSdr;
	volatile unsigned long ulMfifoCr;
	volatile unsigned long ulSfifoCr;
	volatile unsigned long ulSr;
	volatile unsigned long ulIrqmsk;
	volatile unsigned long ulIrqsr;
	volatile unsigned long ulIrqmsked;
	volatile unsigned long ulDmacr;
} I2C_AREA_T, *PI2C_AREA_T;

extern const PI2C_AREA_T ptNetXI2CArea;

//-------------------------------------

// SPI area, netx50

typedef struct {
	volatile unsigned long ul_spi_cr0;
	volatile unsigned long ul_spi_cr1;
	volatile unsigned long ul_spi_dr;
	volatile unsigned long ul_spi_sr;
	volatile unsigned long ul_spi_cpsr;
	volatile unsigned long ul_spi_imsc;
	volatile unsigned long ul_spi_ris;
	volatile unsigned long ul_spi_mis;
	volatile unsigned long ul_spi_icr;
	volatile unsigned long ul_spi_dmacr;
	volatile unsigned long aul_reserved28[2];
	volatile unsigned long ul_spi_data_register;
	volatile unsigned long ul_spi_status_register;
	volatile unsigned long ul_spi_control_register;
	volatile unsigned long ul_spi_interrupt_control_register;
} SPI_AREA_T, *PSPI_AREA_T;

extern const PSPI_AREA_T ptNetXSpi0Area;

//-------------------------------------

// Uart area, same for netX50

typedef struct {
	volatile unsigned long ulUartdr;
	volatile unsigned long ulUartrsr;
	volatile unsigned long ulUartlcr_h;
	volatile unsigned long ulUartlcr_m;
	volatile unsigned long ulUartlcr_l;
	volatile unsigned long ulUartcr;
	volatile unsigned long ulUartfr;
	volatile unsigned long ulUartiir;
	volatile unsigned long ulUartilpr;
	volatile unsigned long ulUartrts;
	volatile unsigned long ulUartforerun;
	volatile unsigned long ulUarttrail;
	volatile unsigned long ulUartdrvout;
	volatile unsigned long ulUartcr_2;
	volatile unsigned long ulUartrxiflsel;
	volatile unsigned long ulUarttxiflsel;
} UART_AREA_T, *PUART_AREA_T;

extern const PUART_AREA_T ptNetXUartArea;

//-------------------------------------

// Gpio area, netX50

typedef struct {
	volatile unsigned long aul_gpio_cfg[32];
	volatile unsigned long aul_gpio_tc[32];
	volatile unsigned long aul_gpio_counter_ctrl[5];
	volatile unsigned long aul_gpio_counter_max[5];
	volatile unsigned long aul_gpio_counter_cnt[5];
	volatile unsigned long ul_gpio_systime_cmp;
	volatile unsigned long ul_gpio_line;
	volatile unsigned long ul_gpio_in;
	volatile unsigned long ul_gpio_irq_raw;
	volatile unsigned long ul_gpio_irq_masked;
	volatile unsigned long ul_gpio_irq_mask_set;
	volatile unsigned long ul_gpio_irq_mask_rst;
	volatile unsigned long ul_gpio_cnt_irq_raw;
	volatile unsigned long ul_gpio_cnt_irq_masked;
	volatile unsigned long ul_gpio_cnt_irq_mask_set;
	volatile unsigned long ul_gpio_cnt_irq_mask_rst;
} GPIO_AREA_T, *PGPIO_AREA_T;

extern const PGPIO_AREA_T ptNetXGpioArea;

//-------------------------------------

// USB area, same for netX50

typedef struct {
	volatile unsigned long ulID;
	volatile unsigned long ulUSB_CTRL;
	volatile unsigned long ulFRM_TIMER;
	volatile unsigned long ulMAIN_EV;
	volatile unsigned long ulMAIN_EM;
	volatile unsigned long ulPIPE_EV;
	volatile unsigned long ulPIPE_EM;
	volatile unsigned long ulReserved_1c;
	volatile unsigned long ulReserved_20;
	volatile unsigned long ulPIPE_SEL;
	volatile unsigned long ulReserved_28;
	volatile unsigned long ulPORT_STAT;
	volatile unsigned long ulPORT_CTRL;
	volatile unsigned long ulPSC_EV;
	volatile unsigned long ulPSC_EM;
	volatile unsigned long ulReserved_3c;
	volatile unsigned long ulPIPE_CTRL;
	volatile unsigned long ulPIPE_CFG;
	volatile unsigned long ulPIPE_ADDR;
	volatile unsigned long ulPIPE_STAT;
	volatile unsigned long ulPIPE_DATA_PTR;
	volatile unsigned long ulPIPE_DATA_TBYTES;
	volatile unsigned long ulPIPE_ADATA_PTR;
	volatile unsigned long ulPIPE_ADATA_TBYTES;
	volatile unsigned long ulDEBUG_CTRL;
	volatile unsigned long ulDEBUG_PID;
	volatile unsigned long ulDEBUG_STAT;
	volatile unsigned long ulTEST;
	volatile unsigned long ulReserved_70;
	volatile unsigned long ulReserved_74;
	volatile unsigned long ulReserved_78;
	volatile unsigned long ulReserved_7c;
	volatile unsigned long ulMAIN_CFG;
	volatile unsigned long ulMODE_CFG;
	volatile unsigned long ulUsb_core_ctrl;
} USB_AREA_T, *PUSB_AREA_T;

extern const PUSB_AREA_T ptNetXUsbArea;

//-------------------------------------

// asic control area, netX50

typedef struct {
	volatile unsigned long ul_io_status;
	volatile unsigned long ul_io_config;
	volatile unsigned long ul_io_config_mask;
	volatile unsigned long ul_reset_ctrl;
	volatile unsigned long ul_phy_control;
	volatile unsigned long ul_armclk_rate_mul_add;
	volatile unsigned long ul_usbclk_rate_mul_add;
	volatile unsigned long ul_fb0clk_rate_mul_add;
	volatile unsigned long ul_fb1clk_rate_mul_add;
	volatile unsigned long ul_clock_enable;
	volatile unsigned long ul_clock_enable_mask;
	volatile unsigned long ul_misc_asic_ctrl;
	volatile unsigned long ul_exmem_priority_lock;
	volatile unsigned long ul_netx_version;
	volatile unsigned long ul_rom_wdg;
	volatile unsigned long aul_reserved_3c[13];
	volatile unsigned long ul_asic_ctrl_access_key;
	volatile unsigned long ul_netx_lock_reserved00;
	volatile unsigned long ul_netx_lock_reserved01;
	volatile unsigned long ul_reserved_7c;
	volatile unsigned long aul_netx_lock_reserved_area[32];
} ASIC_CTRL_AREA_T, *PASIC_CTRL_AREA_T;

extern const PASIC_CTRL_AREA_T ptNetXAsicCtrlArea;

//-------------------------------------

// netx_global_reg_block1_area, netX50
typedef struct {
	volatile unsigned long ul_pci_window_low0;
	volatile unsigned long ul_pci_window_high0;
	volatile unsigned long ul_pci_window_low1;
	volatile unsigned long ul_pci_window_high1;
	volatile unsigned long ul_pci_window_low2;
	volatile unsigned long ul_pci_window_high2;
	volatile unsigned long ul_pci_window_low3;
	volatile unsigned long ul_pci_window_high3;
	volatile unsigned long ul_pci_io_base;
	volatile unsigned long ul_pci_rom_base;
	volatile unsigned long ul_arb_ctrl;
	volatile unsigned long ul_pci_config;
	volatile unsigned long aul_reserved_b0[2];
	volatile unsigned long ul_sfr_base_addr;
	volatile unsigned long ul_cis_base;
	volatile unsigned long ul_wdg_netx;
	volatile unsigned long ul_reserved_c4;
	volatile unsigned long ul_dpmas_wdg_arm_timeout;
	volatile unsigned long ul_dpmas_wdg_arm_trig;
	volatile unsigned long aul_reserved_d0[2];
	volatile unsigned long ul_sta_netx;
	volatile unsigned long ul_reserved_dc;
	volatile unsigned long ul_irq_status_0_netx;
	volatile unsigned long ul_irq_status_1_netx;
	volatile unsigned long ul_irq_status_2_netx;
	volatile unsigned long ul_irq_status_3_netx;
	volatile unsigned long ul_irq_mask_0_netx;
	volatile unsigned long ul_irq_mask_1_netx;
	volatile unsigned long ul_irq_mask_2_netx;
	volatile unsigned long ul_irq_mask_3_netx;
} NETX_GLOBAL_REG_BLOCK_1_AREA_T, *PNETX_GLOBAL_REG_BLOCK_1_AREA_T;

extern const PNETX_GLOBAL_REG_BLOCK_1_AREA_T ptNetXGlobalRegBlock1Area;

//-------------------------------------

// netx_handshake_reg_block, netX50

typedef struct {
	volatile unsigned long aul_handshake_netx[16];
} NETX_HANDSHAKE_REG_BLOCK_AREA_T, *PNETX_HANDSHAKE_REG_BLOCK_AREA_T;

extern const PNETX_HANDSHAKE_REG_BLOCK_AREA_T ptNetxHandshakeRegBlockArea;

//-------------------------------------

// host_global_reg_block, netX50

typedef struct {
	volatile unsigned long ul_pci_base;
	volatile unsigned long ul_wdg_timeout_host;
	volatile unsigned long ul_wdg_trigger_host;
	volatile unsigned long ul_wdg_timeout_netx;
	volatile unsigned long ul_reserved_cc;
	volatile unsigned long ul_cyclic_tmr_control;
	volatile unsigned long ul_cyclic_tmr_reload;
	volatile unsigned long ul_sta_host;
	volatile unsigned long ul_res_reg;
	volatile unsigned long aul_irq_status_host[4];
	volatile unsigned long aul_irq_mask_host[4];
} HOST_GLOBAL_REG_BLOCK_AREA_T, *PHOST_GLOBAL_REG_BLOCK_AREA_T;

extern const PHOST_GLOBAL_REG_BLOCK_AREA_T ptHostGlobalRegBlockArea;

//-------------------------------------

// host_handshake_reg_block, netX50

typedef struct {
	volatile unsigned long aul_handshake_host[16];
} HOST_HANDSHAKE_REG_BLOCK_AREA_T, *PHOST_HANDSHAKE_REG_BLOCK_AREA_T;

extern const PHOST_HANDSHAKE_REG_BLOCK_AREA_T ptHostHandshakeRegBlockArea;

//-------------------------------------

// size of ram areas in dwords (32 bits)
#define XMAC_RPU_DWORD_RAMSIZE 0x0100
#define XMAC_TPU_DWORD_RAMSIZE 0x0100
#define XPEC_DWORD_RAMSIZE 0x0800

typedef struct {
        volatile unsigned long aulRpuProgram[XMAC_RPU_DWORD_RAMSIZE];
        volatile unsigned long aulTpuProgram[XMAC_TPU_DWORD_RAMSIZE];
        volatile unsigned long aulSr[16];
        volatile unsigned long ulStatusShared0;
        volatile unsigned long ulConfigShared0;
        volatile unsigned long ulStatusShared1;
        volatile unsigned long ulConfigShared1;
        volatile unsigned long ulStatusShared2;
        volatile unsigned long ulConfigShared2;
        volatile unsigned long ulStatusShared3;
        volatile unsigned long ulConfigShared3;
        volatile unsigned long aulUrxUtx[4];
        volatile unsigned long ulUrx;
        volatile unsigned long ulUtx;
        volatile unsigned long ulRx;
        volatile unsigned long ulRxHw;
        volatile unsigned long ulRxHwCount;
        volatile unsigned long ulTx;
        volatile unsigned long ulTxHw;
        volatile unsigned long ulTxHwCount;
        volatile unsigned long ulTxSend;
        volatile unsigned long ulRpuPc;
        volatile unsigned long ulTpuPc;
        volatile unsigned long aulWr[10];
        volatile unsigned long ulSysTime;
        volatile unsigned long aulCmpStatus[4];
        volatile unsigned long ulAluFlags;
        volatile unsigned long ulStatusInt;
        volatile unsigned long ulStatBits;
        volatile unsigned long ulStatusMii;
        volatile unsigned long ulStatusMii2;
        volatile unsigned long ulConfigMii;
        volatile unsigned long ulConfigNibbleFifo;
        volatile unsigned long ulConfigSbu;
        volatile unsigned long ulSbuRateMulAdd;
        volatile unsigned long ulSbuRateMulStart;
        volatile unsigned long ulSbuRateMul;
        volatile unsigned long ulStartSamplePos;
        volatile unsigned long ulStopSamplePos;
        volatile unsigned long ulConfigObu;
        volatile unsigned long ulObuRateMulAdd;
        volatile unsigned long ulObuRateMulStart;
        volatile unsigned long ulObuRateMul;
        volatile unsigned long ulStartTransPos;
        volatile unsigned long ulStopTransPos;
        volatile unsigned long ulRpuCount1;
        volatile unsigned long ulRpuCount2;
        volatile unsigned long ulTpuCount1;
        volatile unsigned long ulTpuCount2;
        volatile unsigned long ulRxCount;
        volatile unsigned long ulTxCount;
        volatile unsigned long ulRpmMask0;
        volatile unsigned long ulRpmVal0;
        volatile unsigned long ulRpmMask1;
        volatile unsigned long ulRpmVal1;
        volatile unsigned long ulTpmMask0;
        volatile unsigned long ulTpmVal0;
        volatile unsigned long ulTpmMask1;
        volatile unsigned long ulTpmVal1;
        volatile unsigned long ulPwmConfig;
        volatile unsigned long ulPwmStatus;
        volatile unsigned long ulPwmTp;
        volatile unsigned long ulPwmTu;
        volatile unsigned long ulPwmTv;
        volatile unsigned long ulPwmTw;
        volatile unsigned long ulPwmTd;
        volatile unsigned long ulRpwmTp;
        volatile unsigned long ulRpwmTr;
        volatile unsigned long ulPwmCnt;
        volatile unsigned long ulRpwmCnt;
        volatile unsigned long ulPwmStartTime;
        volatile unsigned long ulRpwmStartTime;
        volatile unsigned long ulPosConfigEncoder;
        volatile unsigned long ulPosConfigCapture;
        volatile unsigned long ulPosCommand;
        volatile unsigned long ulPosStatus;
        volatile unsigned long ulPosEnc0Position;
        volatile unsigned long ulPosEnc0Nullposition;
        volatile unsigned long ulPosEnc1Position;
        volatile unsigned long ulPosEnc1Nullposition;
        volatile unsigned long ulPosEnc0EdgeTime;
        volatile unsigned long ulPosEnc1EdgeTime;
        volatile unsigned long aulPosCapture[4];
        volatile unsigned long aulReserved0[4];
        volatile unsigned long ulRxCrcPolynomialL;
        volatile unsigned long ulRxCrcPolynomialH;
        volatile unsigned long ulRxCrcL;
        volatile unsigned long ulRxCrcH;
        volatile unsigned long ulRxCrcCfg;
        volatile unsigned long ulTxCrcPolynomialL;
        volatile unsigned long ulTxCrcPolynomialH;
        volatile unsigned long ulTxCrcL;
        volatile unsigned long ulTxCrcH;
        volatile unsigned long ulTxCrcCfg;
        volatile unsigned long ulRpuHoldPc;
        volatile unsigned long ulTpuHoldPc;
} XMAC_AREA_T, *PXMAC_AREA_T;

typedef struct {
	union
	{
		struct
		{
			volatile unsigned long aulR[8];
			volatile unsigned long ulRange01;
			volatile unsigned long ulRange23;
			volatile unsigned long ulRange45;
			volatile unsigned long ulRange67;
			volatile unsigned long ulTimer0;
			volatile unsigned long ulTimer1;
			volatile unsigned long ulTimer2;
			volatile unsigned long ulTimer3;
			volatile unsigned long ulUrxCount;
			volatile unsigned long ulUtxCount;
			volatile unsigned long ulPc;
			volatile unsigned long ulZero;
			volatile unsigned long ulStatCfg;
			volatile unsigned long ulEcMaskA;
			volatile unsigned long ulEcMaskB;
			volatile unsigned long ulEcMask0;
			volatile unsigned long ulEcMask1;
			volatile unsigned long ulEcMask2;
			volatile unsigned long ulEcMask3;
			volatile unsigned long ulEcMask4;
			volatile unsigned long ulEcMask5;
			volatile unsigned long ulEcMask6;
			volatile unsigned long ulEcMask7;
			volatile unsigned long ulEcMask8;
			volatile unsigned long ulEcMask9;
			volatile unsigned long ulTimer4;
			volatile unsigned long ulTimer5;
			volatile unsigned long ulIrq;
			volatile unsigned long ulSystimeNs;
			volatile unsigned long ulFifoData;
			volatile unsigned long ulSystimeS;
			volatile unsigned long ulAdc;
			volatile unsigned long aulSr[16];
			volatile unsigned long aulStatCfg[4];
			volatile unsigned long aulUrtx[4];
		} s;
		volatile unsigned long aul[64];
	} uxc;
	volatile unsigned long ulXpuHoldPc;
	volatile unsigned long aulReserved0[1983];
	volatile unsigned long aulRam[XPEC_DWORD_RAMSIZE];
} XPEC_AREA_T, *PXPEC_AREA_T;

extern const PXMAC_AREA_T ptNetXXMac0Area;
extern const PXPEC_AREA_T ptNetXXPec0Area;

//-------------------------------------

// netx_global_reg_block_2, same for netX50

typedef struct {
	volatile unsigned long ul_reserved_00;
	volatile unsigned long ul_clk_reg;
	volatile unsigned long ul_if_conf1;
	volatile unsigned long ul_if_conf2;
	volatile unsigned long aul_ext_bus_config[4];
	volatile unsigned long ul_io_reg_mode0;
	volatile unsigned long ul_io_reg_drv_en0;
	volatile unsigned long ul_io_reg_data0;
	volatile unsigned long ul_io_reg_reserved_2c;
	volatile unsigned long ul_io_reg_mode1;
	volatile unsigned long ul_io_reg_drv_en1;
	volatile unsigned long ul_io_reg_data1;
	volatile unsigned long ul_io_reg_reserved_3c;
	union
	{
		struct
		{
			volatile unsigned long ul_mb_base0;
			volatile unsigned long ul_mb_mapping0;
			volatile unsigned long ul_mb_base1;
			volatile unsigned long ul_mb_mapping1;
			volatile unsigned long ul_mb_base2;
			volatile unsigned long ul_mb_mapping2;
			volatile unsigned long ul_mb_base3;
			volatile unsigned long ul_mb_mapping3;
			volatile unsigned long ul_mb_base4;
			volatile unsigned long ul_mb_mapping4;
			volatile unsigned long ul_mb_base5;
			volatile unsigned long ul_mb_mapping5;
			volatile unsigned long ul_mb_base6;
			volatile unsigned long ul_mb_mapping6;
			volatile unsigned long ul_mb_base7;
			volatile unsigned long ul_mb_mapping7;
		} s;
		volatile unsigned long aul_mb_config[16];
	} u_mb;
	volatile unsigned long aul_hs_ctrl[16];
} NETX_GLOBAL_REG_BLOCK_2_AREA_T, *PNETX_GLOBAL_REG_BLOCK_2_AREA_T;

extern const PNETX_GLOBAL_REG_BLOCK_2_AREA_T ptNetXGlobalRegBlock2Area;

//-------------------------------------

// extmem control, same for netX50

typedef struct {
	volatile unsigned long aul_extsram_ctrl[4];
} EXT_ASYNC_MEM_CTRL_AREA_T, *PEXT_ASYNC_MEM_CTRL_AREA_T;

extern const PEXT_ASYNC_MEM_CTRL_AREA_T ptNetXExtAsyncMemCtrlArea;

//-------------------------------------

// sdram control, same for netX50

typedef struct {
	volatile unsigned long ul_sdram_general_ctrl;
	volatile unsigned long ul_sdram_timing_ctrl;
	volatile unsigned long ul_sdram_mr;
	volatile unsigned long ul_sdram_emr2;
} EXT_SDRAM_CTRL_AREA_T, *PEXT_SDRAM_CTRL_AREA_T;

extern const PEXT_SDRAM_CTRL_AREA_T ptNetXExtSdramCtrlArea;

//-------------------------------------

// pci shadow config area, same for netX50

typedef struct {
	volatile unsigned long ul_pci_conf_rd_ctrl;
	volatile unsigned long ul_pci_conf_rd_data;
	volatile unsigned long ul_pci_conf_wr_ctrl;
	volatile unsigned long ul_pci_conf_wr_data;
} PCI_SHADOW_CONFIG_AREA_T, *PPCI_SHADOW_CONFIG_AREA_T;

extern const PPCI_SHADOW_CONFIG_AREA_T ptNetXPciShadowConfigArea;

//-------------------------------------

// random number generator

typedef struct {
	volatile unsigned long ul_ctrl;
	volatile unsigned long ul_seed;
	volatile unsigned long ul_num;
} RNG_AREA_T, *PRNG_AREA_T;

extern const PRNG_AREA_T ptNetXRngArea;

//-------------------------------------


#endif	// __NETX_IO_AREAS_H__
