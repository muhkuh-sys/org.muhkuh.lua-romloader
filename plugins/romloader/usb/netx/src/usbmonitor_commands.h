/***************************************************************************
 *   Copyright (C) 2010 by Christoph Thelen                                *
 *   doc_bacardi@users.sourceforge.net                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef __USBMONITOR_COMMANDS_H__
#define __USBMONITOR_COMMANDS_H__


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
	USBMON_STATUS_CallMessage		= 0x01,
	USBMON_STATUS_CallFinished		= 0x02,
	USBMON_STATUS_InvalidPacketSize		= 0x03,
	USBMON_STATUS_InvalidSizeParameter	= 0x04,
} USBMON_STATUS_T;

typedef enum
{
	USBMON_ACCESSSIZE_Byte			= 0,
	USBMON_ACCESSSIZE_Word			= 1,
	USBMON_ACCESSSIZE_Long			= 2
} USBMON_ACCESSSIZE_T;


#endif  /* __USBMONITOR_COMMANDS_H__ */
