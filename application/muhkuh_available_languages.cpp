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


#include "muhkuh_available_languages.h"
#include "muhkuh_flags.h"


const LANGUAGE_INFO_T atAvailaleApplicationLanguages[4] =
{
	{ wxLANGUAGE_DEFAULT,           "System default",               NULL },
	{ wxLANGUAGE_GERMAN,            "German",                       icon_famfamfam_flags_de },
	{ wxLANGUAGE_ENGLISH,           "English",                      icon_famfamfam_flags_gb },
	{ wxLANGUAGE_ENGLISH_US,	"English (U.S.)",               icon_famfamfam_flags_us }
};
