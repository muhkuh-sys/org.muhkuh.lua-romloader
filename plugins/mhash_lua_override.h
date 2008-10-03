
%override wxLua_mhash_state_hash
static int LUACALL wxLua_mhash_state_hash(lua_State *L)
{
	size_t sizLen;
	const char *pcBuf;
	double dData_Size;
	mhash_state *ptSelf;


	// get the 'strData' parameter and it's size
	pcBuf = lua_tolstring(L, 2, &sizLen);
	dData_Size = (double)sizLen;

	// get this
	ptSelf = (mhash_state *)wxluaT_getuserdatatype(L, 1, wxluatype_mhash_state);

	// call hash
	ptSelf->hash(L, pcBuf, dData_Size, 0);

	return 0;
}
%end


%override wxLua_mhash_state_hash1
static int LUACALL wxLua_mhash_state_hash1(lua_State *L)
{
	size_t sizLen;
	double dData_Size;
	const char *pcBuf;
	double dParam_Size;
	mhash_state *ptSelf;


	// get the 'strData' parameter and it's size
	pcBuf = lua_tolstring(L, 2, &sizLen);
	dData_Size = (double)sizLen;

	// get the 'size' parameter
	dParam_Size = (double)wxlua_getnumbertype(L, 3);

	// check if the requested size exceeds the string
	if( dParam_Size>dData_Size )
	{
		dParam_Size = dData_Size;
	}

	// get this
	ptSelf = (mhash_state *)wxluaT_getuserdatatype(L, 1, wxluatype_mhash_state);

	// call hash
	ptSelf->hash(L, pcBuf, dParam_Size, 0);

	return 0;
}
%end


%override wxLua_mhash_state_hash2
static int LUACALL wxLua_mhash_state_hash2(lua_State *L)
{
	size_t sizLen;
	double dData_Size;
	const char *pcBuf;
	double dParam_Size;
	double dParam_Offset;
	mhash_state *ptSelf;


	// get the 'strData' parameter and it's size
	pcBuf = lua_tolstring(L, 2, &sizLen);
	dData_Size = (double)sizLen;

	// get the 'size' parameter
	dParam_Size = (double)wxlua_getnumbertype(L, 3);

	// get the 'offset' parameter
	dParam_Offset = (double)wxlua_getnumbertype(L, 4);

	// check if the requested offset exceeds the data
	if( dParam_Offset<dData_Size )
	{
		// offset is within data

		// check if the requested size exceeds the string
		if( dParam_Offset+dParam_Size>dData_Size )
		{
			dParam_Size = dData_Size - dParam_Offset;
		}

		// get this
		ptSelf = (mhash_state *)wxluaT_getuserdatatype(L, 1, wxluatype_mhash_state);

		// call hash
		ptSelf->hash(L, pcBuf, dParam_Size, dParam_Offset);
	}

	return 0;
}
%end


%override wxLua_mhash_state_hash_end
static int LUACALL wxLua_mhash_state_hash_end(lua_State *L)
{
	wxLuaState wxlState(L);
	mhash_state *ptSelf;
	wxString strHash;

	// get this
	ptSelf = (mhash_state *)wxluaT_getuserdatatype(L, 1, wxluatype_mhash_state);

	// call hash_end
	strHash = ptSelf->hash_end();

	// push the result string
	wxlState.lua_PushLString(strHash, strHash.Len());

	return 1;
}
%end

