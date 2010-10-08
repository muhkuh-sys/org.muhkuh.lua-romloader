/***************************************************************************
 *   Copyright (C) 2010 by Hilscher GmbH                                   *
 *                                                                         *
 *   Author: Christoph Thelen (cthelen@hilscher.com)                       *
 *                                                                         *
 *   Redistribution or unauthorized use without expressed written          *
 *   agreement from the Hilscher GmbH is forbidden.                        *
 ***************************************************************************/


#ifndef __USB_H__
#define __USB_H__


typedef enum
{
	USBMON_COMMAND_Read_Byte		= 0x00,
	USBMON_COMMAND_Read_Word		= 0x01,
	USBMON_COMMAND_Read_Long		= 0x02,
	USBMON_COMMAND_Execute			= 0x03,
	USBMON_COMMAND_Write_Byte		= 0x04,
	USBMON_COMMAND_Write_Word		= 0x05,
	USBMON_COMMAND_Write_Long		= 0x06,
	USBMON_COMMAND_Reserved			= 0x07
} USBMON_COMMAND_T;

typedef enum
{
	USBMON_STATUS_Ok			= 0x00,
	USBMON_STATUS_CallFinished		= 0x01,
	USBMON_STATUS_InvalidPacketSize		= 0x02,
	USBMON_STATUS_InvalidSizeParameter	= 0x03
} USBMON_STATUS_T;

typedef enum
{
	USBMON_ACCESSSIZE_Byte			= 0,
	USBMON_ACCESSSIZE_Word			= 1,
	USBMON_ACCESSSIZE_Long			= 2
} USBMON_ACCESSSIZE_T;


void usb_deinit(void);
int usb_init(void);
void usb_loop(void);

#endif	/* __USB_H__ */
