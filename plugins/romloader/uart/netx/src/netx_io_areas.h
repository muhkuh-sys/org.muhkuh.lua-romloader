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


#ifndef __NETX_IO_AREAS_H__
#define __NETX_IO_AREAS_H__

#define DO_CONCAT(a,b,c) a##_##b##_##c
#define CONCAT(a,b,c) DO_CONCAT(a,b,c)

#if ASIC_TYP==500
	#define HOST NX500
	#define HOSTNAME "netx500"
	#include "netx500_io_areas.h"
#elif ASIC_TYP==50
	#define HOST NX50
	#define HOSTNAME "netx50"
	#include "netx50_io_areas.h"
#elif ASIC_TYP==10
	#define HOST NX10
	#define HOSTNAME "netx10"
	#include "netx10_io_areas.h"
#else
	#error "no host define set!"
#endif

#define HOSTADR(a)  CONCAT(Adr,HOST,a)
#define HOSTADDR(a) CONCAT(Addr,HOST,a)
#define HOSTMSK(a)  CONCAT(MSK,HOST,a)
#define HOSTSRT(a)  CONCAT(SRT,HOST,a)
#define HOSTADEF(a) CONCAT(HOST,a,AREA_T)
#define HOSTBFW(a)  CONCAT(BFW,HOST,a)

#endif	/* __NETX_IO_AREAS_H__ */
