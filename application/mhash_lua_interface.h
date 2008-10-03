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


#include "mhash.h"

#include <wx/wx.h>

#include <wxlua/include/wxlua.h>


#ifndef __MHASH_LUA_INTERFACE_H__
#define __MHASH_LUA_INTERFACE_H__


class mhash_state : public wxObject
{
public:
	mhash_state(void);
	mhash_state(hashid type);
	mhash_state(mhash_state *ptMHash);

	~mhash_state();

	static wxString get_version(void);

	static double count(void);
	static double get_block_size(hashid type);
	static wxString get_hash_name(hashid type);

	void init(hashid type);
	void hash(lua_State *L, const char *pcPlaintext, double dSize, double dOffset);
	wxString hash_end(void);

	static double get_hash_pblock(hashid type);
	static hashid get_mhash_algo(MHASH tmp);

	/* HMAC */

//	MHASH mhash_hmac_init(const hashid type, void *key, mutils_word32 keysize, mutils_word32 block);
	void hmac_init(const hashid type, void *key, mutils_word32 keysize, mutils_word32 block);
//	void *mhash_hmac_end_m(void *(*hash_malloc) (mutils_word32));
	void *hmac_end(void);
	bool hmac_deinit(void *result);

	/* Save state functions */

//	mutils_boolean mhash_save_state_mem(MHASH thread, void *mem, mutils_word32 *mem_size);
//	MHASH mhash_restore_state_mem(void *mem);

	/* Key generation functions */
/*
	mutils_error keygen(keygenid algorithm, hashid opt_algorithm,
			  mutils_word64 count,
			  void *keyword, mutils_word32 keysize,
			  void *salt, mutils_word32 saltsize,
			  mutils_word8 *password, mutils_word32 passwordlen);

mutils_error mhash_keygen_ext(keygenid algorithm, KEYGEN data,
			      void *keyword, mutils_word32 keysize,
			      mutils_word8 *password, mutils_word32 passwordlen);

mutils_word8 *mhash_get_keygen_name(keygenid type);

mutils_word32 mhash_get_keygen_salt_size(keygenid type);
mutils_word32 mhash_get_keygen_max_key_size(keygenid type);
mutils_word32 mhash_keygen_count(void);

mutils_boolean mhash_keygen_uses_salt(keygenid type);
mutils_boolean mhash_keygen_uses_count(keygenid type);
mutils_boolean mhash_keygen_uses_hash_algorithm(keygenid type);
*/

private:
	void deinit(void);

	MHASH m_hMHash;
};


#endif  // __MHASH_LUA_INTERFACE_H__

