/*---------------------------------------------------------------------------
  Author : Christoph Thelen

           Hilscher GmbH, Copyright (c) 2006, All Rights Reserved

           Redistribution or unauthorized use without expressed written 
           agreement from the Hilscher GmbH is forbidden
---------------------------------------------------------------------------*/

#include "usb_requests_common.h"

#ifndef __usb_requests_cdc_h__
#define __usb_requests_cdc_h__

//-------------------------------------

typedef enum {
//	SETUP_CDC_REQID_Send_Encapsulated_Command                       = 0x00,
//	SETUP_CDC_REQID_Get_Encapsulated_Response                       = 0x01,
//	SETUP_CDC_REQID_Set_Comm_Feature                                = 0x02,
//	SETUP_CDC_REQID_Get_Comm_Feature                                = 0x03,
//	SETUP_CDC_REQID_Clear_Comm_Feature                              = 0x04,

//	SETUP_CDC_REQID_Set_Aux_Line_State                              = 0x10,
//	SETUP_CDC_REQID_Set_Hook_State                                  = 0x11,
//	SETUP_CDC_REQID_Pulse_Setup                                     = 0x12,
//	SETUP_CDC_REQID_Send_Pulse                                      = 0x13,
//	SETUP_CDC_REQID_Set_Pulse_Time                                  = 0x14,
//	SETUP_CDC_REQID_Ring_Aux_Jack                                   = 0x15,

	SETUP_CDC_REQID_Set_Line_Coding                                 = 0x20,
	SETUP_CDC_REQID_Get_Line_Coding                                 = 0x21,
	SETUP_CDC_REQID_Set_Control_Line_State                          = 0x22
//	SETUP_CDC_REQID_Send_Break                                      = 0x23,

//	SETUP_CDC_REQID_Set_Ringer_Parms                                = 0x30,
//	SETUP_CDC_REQID_Get_Ringer_Parms                                = 0x31,
//	SETUP_CDC_REQID_Set_Operation_Parms                             = 0x32,
//	SETUP_CDC_REQID_Get_Operation_Parms                             = 0x33,
//	SETUP_CDC_REQID_Set_Line_Parms                                  = 0x34,
//	SETUP_CDC_REQID_Get_Line_Parms                                  = 0x35,
//	SETUP_CDC_REQID_Dial_Digits                                     = 0x36,
//	SETUP_CDC_REQID_Set_Unit_Parameter                              = 0x37,
//	SETUP_CDC_REQID_Get_Unit_Parameter                              = 0x38,
//	SETUP_CDC_REQID_Clear_Unit_Parameter                            = 0x39,
//	SETUP_CDC_REQID_Get_Profile                                     = 0x3a,

//	SETUP_CDC_REQID_Set_Multicast_Filters                           = 0x40,
//	SETUP_CDC_REQID_Set_Ethernet_Powermanagement_Pattern_Filter     = 0x41,
//	SETUP_CDC_REQID_Get_Ethernet_Powermanagement_Pattern_Filter     = 0x42,
//	SETUP_CDC_REQID_Set_Ethernet_Packet_Filter                      = 0x43,
//	SETUP_CDC_REQID_Get_Ethernet_Statistic                          = 0x44,

//	SETUP_CDC_REQID_Set_Atm_Data_Format                             = 0x50,
//	SETUP_CDC_REQID_Get_Atm_Devcice_Statistics                      = 0x51,
//	SETUP_CDC_REQID_Set_Atm_Default_Vc                              = 0x52,
//	SETUP_CDC_REQID_Get_Atm_Vc_Statistics                           = 0x53
} setup_cdc_requestId_t;

//-------------------------------------

typedef enum {
	tUsbCdc_StopBits_1      = 0,
	tUsbCdc_StopBits_1_5    = 1,
	tUsbCdc_StopBits_2      = 2
} tUsbCdc_StopBits;

typedef enum {
	tUsbCdc_Parity_None     = 0,
	tUsbCdc_Parity_Odd      = 1,
	tUsbCdc_Parity_Even     = 2,
	tUsbCdc_Parity_Mark     = 3,
	tUsbCdc_Parity_Space    = 4
} tUsbCdc_Parity;


typedef enum {
	tUsbCdc_BufferState_Ok          = 0,            // the packet was successfully put into the buffer
	tUsbCdc_BufferState_Full        = 1,            // the packet was put into the buffer and afterwards the buffer is full
	tUsbCdc_BufferState_Rejected    = 2             // the packet could not be put into the buffer, it's full
} tUsbCdc_BufferState;

//-------------------------------------

extern unsigned int usb_cdc_buf_rec_rpos;              // read position
extern unsigned int usb_cdc_buf_rec_wpos;              // write position
extern unsigned int usb_cdc_buf_rec_fill;              // fill level

extern unsigned int usb_cdc_buf_send_rpos;             // read position
extern unsigned int usb_cdc_buf_send_wpos;             // write position
extern unsigned int usb_cdc_buf_send_fill;             // fill level

//-------------------------------------

unsigned int usb_cdc_buf_rec_peek(void);
unsigned char usb_cdc_buf_rec_get(void);
tUsbCdc_BufferState usb_cdc_buf_rec_put(unsigned char *pbBuffer, unsigned int uiBufferLen, unsigned int uiMaxPacketSize);

unsigned int usb_cdc_buf_send_get(unsigned char *pbBuffer, unsigned int uiMaxPacketSize);
void usb_cdc_buf_send_put(unsigned int uiValue);
void usb_cdc_buf_send_flush(void);

//-------------------------------------

#endif  // __usb_requests_cdc_h__
