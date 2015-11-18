#include <stddef.h>
#include <stdint.h>


#ifndef __ROMLOADER_DPM_MEMORYLAYOUT_H__
#define __ROMLOADER_DPM_MEMORYLAYOUT_H__



#define NETX56_NETX_VERSION_MASK               0x000000ff
#define NETX56_NETX_VERSION_STEP               0x00000043

#define NETX56_DPM_NETX_TO_HOST_BUFFERSIZE     0x0200
#define NETX56_DPM_HOST_TO_NETX_BUFFERSIZE     0x0400

#define NETX56_BOOT_ID_ROM                     0x4c42584e


typedef struct NX56_DPM_AREA_Ttag
{
	volatile uint32_t  ulDpm_cfg0x0;
	volatile uint32_t  ulDpm_if_cfg;
	volatile uint32_t aulDpm_pio_cfg[2];
	volatile uint32_t  ulDpm_addr_cfg;
	volatile uint32_t  ulDpm_timing_cfg;
	volatile uint32_t  ulDpm_rdy_cfg;
	volatile uint32_t  ulDpm_status;
	volatile uint32_t  ulDpm_status_err_reset;
	volatile uint32_t  ulDpm_status_err_addr;
	volatile uint32_t  ulDpm_misc_cfg;
	volatile uint32_t  ulDpm_io_cfg_misc;
	volatile uint32_t aulReserved0[2];
	volatile uint32_t  ulDpm_tunnel_cfg;
	volatile uint32_t  ulDpm_itbaddr;
	struct
	{
		volatile uint32_t  ulEnd;
		volatile uint32_t  ulMap;
	} asDpm_win[4];
	volatile uint32_t aulReserved1[8];
	volatile uint32_t  ulDpm_irq_raw;
	volatile uint32_t  ulDpm_irq_arm_mask_set;
	volatile uint32_t  ulDpm_irq_arm_mask_reset;
	volatile uint32_t  ulDpm_irq_arm_masked;
	volatile uint32_t  ulDpm_irq_xpic_mask_set;
	volatile uint32_t  ulDpm_irq_xpic_mask_reset;
	volatile uint32_t  ulDpm_irq_xpic_masked;
	volatile uint32_t  ulDpm_irq_fiq_mask_set;
	volatile uint32_t  ulDpm_irq_fiq_mask_reset;
	volatile uint32_t  ulDpm_irq_fiq_masked;
	volatile uint32_t  ulDpm_irq_irq_mask_set;
	volatile uint32_t  ulDpm_irq_irq_mask_reset;
	volatile uint32_t  ulDpm_irq_irq_masked;
	volatile uint32_t aulReserved2[1];
	volatile uint32_t  ulDpm_sw_irq;
	volatile uint32_t aulReserved3[1];
	volatile uint32_t  ulDpm_reserved_netx50_wgd_host_timeout;
	volatile uint32_t  ulDpm_reserved_netx50_wgd_host_trigger;
	volatile uint32_t  ulDpm_reserved_netx50_wgd_netx_timeout;
	volatile uint32_t  ulDpm_sys_sta_bigend16;
	volatile uint32_t  ulDpm_reserved_netx50_timer_ctrl;
	volatile uint32_t  ulDpm_reserved_netx50_timer_start_val;
	volatile uint32_t  ulDpm_sys_sta;
	volatile uint32_t  ulDpm_reset_request;
	volatile uint32_t  ulDpm_firmware_irq_raw;
	volatile uint32_t aulReserved4[1];
	volatile uint32_t  ulDpm_firmware_irq_raw2;
	volatile uint32_t aulReserved5[1];
	volatile uint32_t  ulDpm_firmware_irq_mask;
	volatile uint32_t  ulDpm_netx_version_bigend16;
	volatile uint32_t  ulDpm_firmware_irq_mask2;
	volatile uint32_t  ulDpm_netx_version;
} NX56_DPM_AREA_T;



typedef struct HBOOT_DPM_NETX56_STRUCT
{
	volatile uint32_t ulDpmBootId;
	volatile uint32_t ulDpmByteSize;
	volatile uint32_t ulSdramExtGeneralCtrl;
	volatile uint32_t ulSdramExtTimingCtrl;
	volatile uint32_t ulSdramExtByteSize;
	volatile uint32_t ulSdramHifGeneralCtrl;
	volatile uint32_t ulSdramHifTimingCtrl;
	volatile uint32_t ulSdramHifByteSize;
	volatile uint32_t aulReserved_20[22];
	volatile uint32_t ulNetxToHostDataSize;
	volatile uint32_t ulHostToNetxDataSize;
	volatile uint32_t ulHandshake;
	volatile uint32_t aulReserved_84[31];
	volatile uint8_t aucNetxToHostData[NETX56_DPM_NETX_TO_HOST_BUFFERSIZE];
	volatile uint8_t aucHostToNetxData[NETX56_DPM_HOST_TO_NETX_BUFFERSIZE];
} HBOOT_DPM_NETX56_T;



typedef struct DPM_REGISTER_NETX56_STRUCT
{
	NX56_DPM_AREA_T tCtrl;
	HBOOT_DPM_NETX56_T tHboot;
	volatile uint8_t aucBitflipImage[65536-sizeof(NX56_DPM_AREA_T)-sizeof(HBOOT_DPM_NETX56_T)];
} DPM_REGISTER_NETX56_T;



typedef union NETX_LAYOUT_UNION
{
	DPM_REGISTER_NETX56_T tNetx56;
} NETX_LAYOUT_T;



typedef union NETX_DPM_MEMORYLAYOUT_UNION
{
	volatile uint8_t aucRaw[65536];
	volatile uint16_t ausRaw[65536/sizeof(uint16_t)];
	volatile uint32_t aulRaw[65536/sizeof(uint32_t)];
	NETX_LAYOUT_T uReg;
} NETX_DPM_MEMORYLAYOUT_T;


#endif  /* __ROMLOADER_DPM_MEMORYLAYOUT_H__ */

