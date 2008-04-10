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
#include <wx/wx.h>


double band(double a, double b)
{
  unsigned long ulA, ulB, ulResult;


  ulA = (unsigned long)a;
  ulB = (unsigned long)b;

  ulResult = ulA&ulB;
//  wxLogMessage(wxT("band: %08x & %08x = %08x"), ulA, ulB, ulResult);
	return (double)ulResult;
}


double bor(double a, double b)
{
  unsigned long ulA, ulB, ulResult;


  ulA = (unsigned long)a;
  ulB = (unsigned long)b;

  ulResult = ulA|ulB;
//  wxLogMessage(wxT("bor: %08x | %08x = %08x"), ulA, ulB, ulResult);
	return (double)ulResult;
}


double bxor(double a, double b)
{
  unsigned long ulA, ulB, ulResult;


  ulA = (unsigned long)a;
  ulB = (unsigned long)b;

  ulResult = ulA^ulB;
//  wxLogMessage(wxT("bxor: %08x ^ %08x = %08x"), ulA, ulB, ulResult);
	return (double)ulResult;
}


double lshift(double a, double b)
{
  unsigned long ulA, ulB, ulResult;


  ulA = (unsigned long)a;
  ulB = (unsigned long)b;

  ulResult = ulA<<ulB;
//  wxLogMessage(wxT("lshift: %08x << %08x = %08x"), ulA, ulB, ulResult);
	return (double)ulResult;
}


double rshift(double a, double b)
{
  unsigned long ulA, ulB, ulResult;


  ulA = (unsigned long)a;
  ulB = (unsigned long)b;

  ulResult = ulA>>ulB;
//  wxLogMessage(wxT("rshift: %08x >> %08x = %08x"), ulA, ulB, ulResult);
	return (double)ulResult;
}


double bnot(double a)
{
  unsigned long ulA, ulResult;


  ulA = (unsigned long)a;

  ulResult = ~ulA;
//  wxLogMessage(wxT("bnot: ~%08x = %08x"), ulA, ulResult);
	return (double)ulResult;
}

