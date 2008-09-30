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
{
}


mhash_state::mhash_state(hashid type)
{
	m_hMHash = mhash_init(type);
}


mhash_state::mhash_state(mhash_state *ptMHash)
{
	m_hMHash = mhash_cp(ptMHash->m_hMHash);
}


mhash_state::~mhash_state()
{
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


	pc = (const char*)mhash_get_hash_name(type);
	return wxString::FromAscii(pc);
}


//void mhash_state::free(void *ptr);

//wxString mhash_state::get_hash_name_static(hashid type);

void mhash_state::init(hashid type)
{
	m_hMHash = mhash_init(type);
}

/* copy prototypes */

// void mhash_state::mhash_cp(MHASH from)
// {
// 	m_hMHash = mhash_cp(from);
// }

/* update prototype */

bool mhash_state::mhash(const char *plaintext, double size)
{
	mutils_boolean mb;


	mb = mhash(plaintext, size);
	return (mb==MUTILS_TRUE);
}


bool mhash_state::mhash_o(const char *plaintext, double offset, double size)
{
	mutils_boolean mb;
	size_t sizOffset;


	sizOffset = (size_t)offset;
	mb = mhash(plaintext+sizOffset, size);
	return (mb==MUTILS_TRUE);
}


