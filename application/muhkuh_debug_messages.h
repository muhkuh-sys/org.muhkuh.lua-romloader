/***************************************************************************
 *   Copyright (C) 2008 by Christoph Thelen                                *
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


#ifndef __MUHKUH_DEBUG_MESSAGES_H__
#define __MUHKUH_DEBUG_MESSAGES_H__


typedef enum
{
	MUHDBG_InterpreterHalted			= 0,
	MUHDBG_CmdStepInto				= 1,
	MUHDBG_CmdStepOver				= 2,
	MUHDBG_CmdStepOut				= 3,
	MUHDBG_CmdContinue				= 4,
	MUHDBG_CmdBreak					= 5,
	MUHDBG_CmdGetStack				= 6,
	MUHDBG_CmdGetLocal				= 7,
	MUHDBG_CmdGetUpValue				= 8,
} tMUHDBG;


#endif	// __MUHKUH_DEBUG_MESSAGES_H__
