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


%module muhkuh_components_lua

%{
	#include "muhkuh_components.h"
%}

%include muhkuh_typemaps.i

/*
typedef struct
{
	const char *pcPluginName;                                                                                                                                                                                      
	const char *pcPluginId;
	unsigned int uiVersionMajor;                                                                                                                                                                                                     
	unsigned int uiVersionMinor;                                                                                                                                                                                                     
	unsigned int uiVersionSub;
} MUHKUH_PLUGIN_DESCRIPTION_T;
*/

const char *get_version();

void get_plugins(lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT);
