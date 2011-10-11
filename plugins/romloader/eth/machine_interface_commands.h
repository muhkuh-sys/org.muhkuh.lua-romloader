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


#ifndef __MACHINE_INTERFACE_COMMANDS_H__
#define __MACHINE_INTERFACE_COMMANDS_H__


#define MI_VERSION_MAJOR 1
#define MI_VERSION_MINOR 0

#define MI_ETH_MAX_PACKET_SIZE 1024

typedef enum
{
	MI_COMMAND_Read                    = 0x00,
	MI_COMMAND_Write                   = 0x01,
	MI_COMMAND_Execute                 = 0x02
} MI_COMMAND_T;

typedef enum
{
	MI_STATUS_Ok                        = 0x00,
	MI_STATUS_CallMessage               = 0x01,
	MI_STATUS_CallFinished              = 0x02,
	MI_STATUS_InvalidCommand            = 0x03,
	MI_STATUS_InvalidPacketSize         = 0x04,
	MI_STATUS_InvalidSizeParameter      = 0x05
} MI_STATUS_T;

typedef enum
{
	MI_ACCESSSIZE_Byte                  = 0,
	MI_ACCESSSIZE_Word                  = 1,
	MI_ACCESSSIZE_Long                  = 2
} MI_ACCESSSIZE_T;


extern const unsigned char aucMachineInterface_magic[8];


#endif  /* __MACHINE_INTERFACE_COMMANDS_H__ */
