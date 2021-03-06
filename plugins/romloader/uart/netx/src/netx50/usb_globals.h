/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#include "netx_io_areas.h"
#include "usb_requests_common.h"
#include "usb_requests_cdc.h"


#ifndef __usb_globals__
#define __usb_globals__

//-------------------------------------

#define VendorID                0x1939
#define ProductID               0x0002
#define ProductRel              0x0001

//-------------------------------------

// max packet size for endpoints
#define Usb_Ep0_PacketSize      0x40
#define Usb_Ep1_PacketSize      0x40
#define Usb_Ep2_PacketSize      0x40
#define Usb_Ep3_PacketSize      0x40

// enpoint buffers
#define Usb_Ep0_Buffer          0x0000
#define Usb_Ep1_Buffer          0x0080
#define Usb_Ep2_Buffer          0x0100

//-------------------------------------

typedef enum
{
	USB_State_Powered				= 0,
	USB_State_Default				= 1,
	USB_State_AddressChangePending			= 2,
	USB_State_Address				= 3,
	USB_State_Configured				= 4
} USB_State_t;

typedef enum
{
	USB_DescriptorType_Device			= 0x01,
	USB_DescriptorType_Configuration		= 0x02,
	USB_DescriptorType_String			= 0x03,
	USB_DescriptorType_Interface			= 0x04,
	USB_DescriptorType_Endpoint			= 0x05,
	USB_DescriptorType_DeviceQualifier		= 0x06,
	USB_DescriptorType_OtherSpeedConfiguration	= 0x07,
	USB_DescriptorType_InterfacePower		= 0x08
} USB_DescriptorType_t;

typedef enum
{
	USB_EndpointState_Unconfigured			= 0x00,
	USB_EndpointState_Running,
	USB_EndpointState_Halted
} USB_EndpointState_t;

typedef enum
{
        USB_SetupTransaction_NoOutTransaction		= 0,
        USB_SetupTransaction_OutTransaction		= 1
} USB_SetupTransaction_t;

typedef enum
{
        USB_ReceiveEndpoint_Running			= 0,
        USB_ReceiveEndpoint_Blocked			= 1
} USB_ReceiveEndpoint_t;

typedef enum
{
	USB_CDC_ConnectionState_Idle			= 0,
	USB_CDC_ConnectionState_Connected		= 1
} USB_CDC_ConnectionState_t;

//-------------------------------------

extern USB_State_t globalState;

extern unsigned int currentConfig;

// buffer for setup packets
extern unsigned char setupBuffer[Usb_Ep0_PacketSize];

// decoded packet
extern setupPacket_t tSetupPkt;
// out transaction needed
extern USB_SetupTransaction_t tOutTransactionNeeded;

extern USB_ReceiveEndpoint_t tReceiveEpState;

// new address for pending address change
extern unsigned int uiNewAddress;

// CDC Connection State
extern USB_CDC_ConnectionState_t tCdcConnectionState;

//-------------------------------------

void usb_pingpong(void);
void usb_activateInputPipe(void);

//-------------------------------------

// bit definitions for Pipe Total Bytes
#define	MSK_USB_PIPE_DATA_TBYTES_DBV		0x80000000

// bit definitions for Pipe Control
#define	MSK_USB_PIPE_CTRL_TPID			0x00000003
#define	DEF_USB_PIPE_CTRL_TPID_OUT		0
#define	DEF_USB_PIPE_CTRL_TPID_IN		1
#define	DEF_USB_PIPE_CTRL_TPID_SETUP		2
#define	MSK_USB_PIPE_CTRL_ACT			0x00000004

// bit definitions for Pipe Configuration
#define	MSK_USB_PIPE_CFG_MPS			0x000003ff
#define	MSK_USB_PIPE_CFG_ET			0x00000c00
#define MSK_USB_PIPE_CFG_ET_CTRL		(0<<10)
#define MSK_USB_PIPE_CFG_ET_ISO			(1<<10)
#define MSK_USB_PIPE_CFG_ET_BULK		(2<<10)
#define MSK_USB_PIPE_CFG_ET_INT			(3<<10)
#define	MSK_USB_PIPE_CFG_STRM			0x00001000
#define	MSK_USB_PIPE_CFG_EPS			0x00002000
#define	MSK_USB_PIPE_CFG_ACID			0x00004000
#define	MSK_USB_PIPE_CFG_STALL			0x00008000
#define	MSK_USB_PIPE_CFG_POFF			0x00ff0000
#define	MSK_USB_PIPE_CFG_PI			0x0f000000
#define	MSK_USB_PIPE_CFG_SKIPISO		0x10000000
#define	MSK_USB_PIPE_CFG_HIDBE			0x20000000
#define	MSK_USB_PIPE_CFG_IOT			0x40000000

// bit definitions for Port Control
#define	MSK_USB_PORT_CTRL_PTESTC		0x00000003
#define	DEF_USB_PTESTC_DIS			0
#define	DEF_USB_PTESTC_JST			1
#define	DEF_USB_PTESTC_KST			2
#define	DEF_USB_PTESTC_SE0			3
#define	MSK_USB_PORT_CTRL_URESET		0x00000004
#define	MSK_USB_PORT_CTRL_FPRESU		0x00000008
#define	MSK_USB_PORT_CTRL_PENA			0x00000010
#define	MSK_USB_PORT_CTRL_PSUSP			0x00000020
#define	MSK_USB_PORT_CTRL_VB_ON			0x00000040
#define	MSK_USB_PORT_CTRL_TERM_SEL		0x00000080
#define	MSK_USB_PORT_CTRL_TERM_ENA		0x00000100
#define	MSK_USB_PORT_CTRL_DCHRG			0x00000200
#define	MSK_USB_PORT_CTRL_VBS_ON		0x00000400
#define	MSK_USB_PORT_CTRL_ID_PU			0x00001000
#define	MSK_USB_PORT_CTRL_P_LEN			0x00ff0000

// bit definitions for Main Event
#define		MSK_USB_MAIN_EV_FRM_EV		0x00000001
#define		MSK_USB_MAIN_EV_FRM32_EV	0x00000002
#define		MSK_USB_MAIN_EV_GPORT_EV	0x00000004
#define		MSK_USB_MAIN_EV_GPIPE_EV	0x00000008
#define		MSK_USB_MAIN_EV_HCHA_EV		0x00000010
#define		MSK_USB_MAIN_EV_BWERR_EV	0x00000020

// bit definitions for Pipe Status
#define		MSK_USB_PIPE_STAT_DBOFF		0x00000003
#define		MSK_USB_PIPE_STAT_DT		0x00000004
#define		MSK_USB_PIPE_STAT_DBSEL		0x00000008
#define		MSK_USB_PIPE_STAT_BBL		0x00000010
#define		MSK_USB_PIPE_STAT_HALT		0x00000020
#define		MSK_USB_PIPE_STAT_ACTS		0x00000040
#define		MSK_USB_PIPE_STAT_DBERR		0x00000080
#define		MSK_USB_PIPE_STAT_CERR		0x00000300

// bit definitions for Port Status Change Event
#define		MSK_USB_PSC_EV_OCU_EV		0x00000001
#define		MSK_USB_PSC_EV_BERR_EV		0x00000002
#define		MSK_USB_PSC_EV_RSU_EV		0x00000004
#define		MSK_USB_PSC_EV_RSUC_EV		0x00000008
#define		MSK_USB_PSC_EV_SUSP_EV		0x00000010
#define		MSK_USB_PSC_EV_URES_EV		0x00000020
#define		MSK_USB_PSC_EV_CDC_EV		0x00000040
#define		MSK_USB_PSC_EV_PWRSC_EV		0x00000080
#define		MSK_USB_PSC_EV_P_END_EV		0x00000100

//-------------------------------------

#endif  // __usb_globals__

