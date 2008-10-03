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


#include "mhash_lua_interface.h"


mhash_state::mhash_state(void)
 : m_hMHash(NULL)
{
	wxLogMessage("mhash_state(void)");
}


mhash_state::mhash_state(hashid type)
 : m_hMHash(NULL)
{
	wxLogMessage("mhash_state(hashid type)");
	m_hMHash = mhash_init(type);
}


mhash_state::mhash_state(mhash_state *ptMHash)
 : m_hMHash(NULL)
{
	wxLogMessage("mhash_state(mhash_state *ptMHash)");
	m_hMHash = mhash_cp(ptMHash->m_hMHash);
}


mhash_state::~mhash_state()
{
	wxLogMessage("~mhash_state()");

	deinit();
}


void mhash_state::deinit(void)
{
	if( m_hMHash!=NULL )
	{
		mhash_deinit(m_hMHash, NULL);
		m_hMHash = NULL;
	}
}


wxString mhash_state::get_version(void)
{
	return wxString::FromAscii(PACKAGE_STRING);
}


double mhash_state::count(void)
{
	double d;


	d = (double)mhash_count();
	return d;
}


double mhash_state::get_block_size(hashid type)
{
	double d;


	d = (double)mhash_get_block_size(type);
	return d;
}


wxString mhash_state::get_hash_name(hashid type)
{
	const char *pc;
	wxString strName;


	// NOTE: use the static version here, the other one mallocs the string
	pc = (const char*)mhash_get_hash_name_static(type);
	if( pc!=NULL )
	{
		strName = wxString::FromAscii(pc);
	}
	return strName;
}


void mhash_state::init(hashid type)
{
	// clear any existing state
	deinit();

	// init new state
	m_hMHash = mhash_init(type);
}


void mhash_state::hash(lua_State *L, const char *pcPlaintext, double dSize, double dOffset)
{
	mutils_boolean tMb;
	size_t sizSize;
	size_t sizOffset;
	wxString strErrorMsg;


	if( m_hMHash==NULL )
	{
		wxlua_error(L, _("mhash: the state was not initialized!"));
	}
	else
	{
		sizSize = (size_t)dSize;
		sizOffset = (size_t)dOffset;
		tMb = mhash(m_hMHash, pcPlaintext+sizOffset, sizSize);
		if( tMb!=MUTILS_OK )
		{
			strErrorMsg.Printf(_("mhash: failed to hash %d bytes!"), sizSize);
			wxlua_error(L, strErrorMsg);
		}
	}
}


wxString mhash_state::hash_end(void)
{
	hashid tId;
	size_t sizBlockSize;
	void *pvHash;
	const char *pcHash;
	wxString strHash;


	// get blocksize
	if( m_hMHash==NULL )
	{
		wxLogError("the mhash state was not initialized!");
	}
	else
	{
		tId = mhash_get_mhash_algo(m_hMHash);
		sizBlockSize = mhash_get_block_size(tId);
		if( sizBlockSize!=0 )
		{
			pvHash = mhash_end(m_hMHash);
			if( pvHash!=NULL )
			{
				pcHash = (const char*)pvHash;
				strHash = wxString::From8BitData(pcHash, sizBlockSize);
			}
			m_hMHash = NULL;
		}
	}

	return strHash;
}

