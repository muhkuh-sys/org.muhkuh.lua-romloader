/***************************************************************************
 *   Copyright (C) 2011 by Christoph Thelen                                *
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


#include <wx/wx.h>

/* Platform specific includes. */
#include <unistd.h>


#ifndef SWIGRUNTIME
#include <swigluarun.h>

/* swigluarun does not include the lua specific defines. Add them here. */
typedef struct
{
	lua_State* L; /* the state */
	int ref;      /* a ref in the lua global index */
}SWIGLUA_REF;
#endif



#ifndef __MUHKUH_CAPTURE_STD_H__
#define __MUHKUH_CAPTURE_STD_H__


class capture_std
{
public:
	capture_std(void);
	~capture_std(void);

	int run(const char *pcCommand, lua_State *ptLuaStateForTableAccess);


private:
	char **get_strings_from_table(int iIndex, lua_State *ptLuaState) const;
	int free_string_table(char **ppcTable) const;

	static const unsigned int m_uiMaxLinesPerLoop = 256;

	pid_t m_tCaptureThread;
	pid_t m_tExecThread;
};


#endif  /* __MUHKUH_CAPTURE_STD_H__ */
