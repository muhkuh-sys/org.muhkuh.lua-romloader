/***************************************************************************
 *   Copyright (C) 2012 by Christoph Thelen                                *
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


#ifndef __ROMLOADER_DEF_H__
#define __ROMLOADER_DEF_H__

/*-----------------------------------*/

typedef enum
{
	ROMLOADER_CHIPTYP_UNKNOWN         = 0,
	ROMLOADER_CHIPTYP_NETX500         = 1,
	ROMLOADER_CHIPTYP_NETX100         = 2,
	ROMLOADER_CHIPTYP_NETX50          = 3,
	ROMLOADER_CHIPTYP_NETX5           = 4,
	ROMLOADER_CHIPTYP_NETX10          = 5,
	ROMLOADER_CHIPTYP_NETX56          = 6,
	ROMLOADER_CHIPTYP_NETX56B         = 7,
	ROMLOADER_CHIPTYP_NETX4000RELAXED = 8,
	ROMLOADER_CHIPTYP_RESERVED9       = 9,
	ROMLOADER_CHIPTYP_NETX90MPW       = 10
} ROMLOADER_CHIPTYP;

/*-----------------------------------*/

#endif  /* __ROMLOADER_DEF_H__ */

