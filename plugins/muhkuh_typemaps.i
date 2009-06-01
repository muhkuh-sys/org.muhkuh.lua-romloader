
/* This typemap adds "SWIGTYPE_" to the name of the input parameter to
 * construct the swig typename. The parameter name must match the definition
 * in the wrapper.
 */
%typemap(in, numinputs=0) swig_type_info *
%{
	$1 = SWIGTYPE_$1_name;
%}

/* This typemap expects a table as input and replaces it with the Lua state.
 * This allows the function to add elements to the table without the overhead
 * of creating and deleting a C array.
 */
%typemap(in,checkfn="lua_istable") lua_State *ptLuaStateForTableAccess
%{
	$1 = L;
%}

/* This typemap passes the Lua state to the function. This allows the function
 * to call functions of the Swig Runtime API and the Lua C API.
 */
%typemap(in, numinputs=0) lua_State *
%{
	$1 = L;
%}

/* This typemap converts the output of the plugin reference's "Create"
 * function from the general "muhkuh_plugin" type to the type of this
 * interface. It transfers the ownership of the object to Lua (this is the
 * last parameter in the call to "SWIG_NewPointerObj").
 */
%typemap(out) muhkuh_plugin *
%{
	SWIG_NewPointerObj(L,result,((muhkuh_plugin_reference const *)arg1)->GetTypeInfo(),1); SWIG_arg++;
%}


%typemap(in, numinputs=0) (unsigned char **ppucOutputData, unsigned long *pulOutputData)
%{
	unsigned char *pucOutputData;
	unsigned long ulOutputData;
	$1 = &pucOutputData;
	$2 = &ulOutputData;
%}

%typemap(argout) (unsigned char **ppucOutputData, unsigned long *pulOutputData)
%{
	if( pucOutputData!=NULL && ulOutputData!=0 )
	{
		lua_pushlstring(L, (char*)pucOutputData, ulOutputData);
		delete[] pucOutputData;
	}
	else
	{
		lua_pushnil(L);
	}
	++SWIG_arg;
%}

%typemap(in) (const unsigned char *pucInputData, unsigned long ulInputData)
{
	size_t sizInputData;
	$1 = (unsigned char*)lua_tolstring(L, $argnum, &sizInputData);
	$2 = (unsigned long)sizInputData;
}

