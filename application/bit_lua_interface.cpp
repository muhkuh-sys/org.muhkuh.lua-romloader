/***************************************************************************
 *   Copyright (C) 2007 by Christoph Thelen                                *
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

#include "bit_lua_interface.h"


unsigned long band(unsigned long a, unsigned long b)
{
	return a&b;
}


unsigned long bor(unsigned long a, unsigned long b)
{
	return a|b;
}


unsigned long bxor(unsigned long a, unsigned long b)
{
	return a^b;
}


unsigned long bshl(unsigned long a, unsigned long b)
{
	return a<<b;
}


unsigned long bshr(unsigned long a, unsigned long b)
{
	return a>>b;
}


unsigned long bnot(unsigned long a)
{
	return ~a;
}

