
/* This typemap adds "SWIGTYPE_" to the name of the input parameter to
 * construct the swig typename. The parameter name must match the definition
 * in the wrapper.
 */
%typemap(in, numinputs=0) swig_type_info *
%{
	$1 = SWIGTYPE_$1_name;
%}


/* Swig 1.3.40 has no lua implementation of the cstring library. The following
 * typemaps are a subset of the library.
 */
#ifdef SWIGLUA
%typemap(in) (const char *pcBUFFER_IN, size_t sizBUFFER_IN)
{
	size_t sizBuffer;
	$1 = (char*)lua_tolstring(L, $argnum, &sizBuffer);
	$2 = sizBuffer;
}

%typemap(in, numinputs=0) (char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT)
%{
	char *pcOutputData;
	size_t sizOutputData;
	$1 = &pcOutputData;
	$2 = &sizOutputData;
%}

/* NOTE: This "argout" typemap can only be used in combination with the above "in" typemap. */
%typemap(argout) (char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT)
%{
	if( pcOutputData!=NULL && sizOutputData!=0 )
	{
		lua_pushlstring(L, pcOutputData, sizOutputData);
	}
	else
	{
		lua_pushnil(L);
	}
	++SWIG_arg;
%}
#else
%include cstring.i
%apply (char *STRING, int LENGTH) { (const char *pcBUFFER_IN, size_t sizBUFFER_IN) };
%cstring_output_allocate_size(char **ppcBUFFER_OUT, size_t *psizBUFFER_OUT, free(*$1));
#endif


/* This typemap expects a table as input and replaces it with the Lua state.
 * This allows the function to add elements to the table without the overhead
 * of creating and deleting a C array.
 */
#ifdef SWIGLUA
%typemap(in,checkfn="lua_istable") lua_State *ptLuaStateForTableAccess
%{
	$1 = L;
%}
#elseif SWIGPYTHON
#endif


/* This typemap passes Lua state to the function. The function must create one
 * lua object on the stack. This is passes as the return value to lua.
 * No further checks are done!
 */
#ifdef SWIGLUA
%typemap(in, numinputs=0) lua_State *MUHKUH_SWIG_OUTPUT_CUSTOM_OBJECT
%{
	/* Hooray, this is my typemap. */
	$1 = L;
	++SWIG_arg;
%}
#elseif SWIGPYTHON
#endif


/* This typemap passes the Lua state to the function. This allows the function
 * to call functions of the Swig Runtime API and the Lua C API.
 */
#ifdef SWIGLUA
%typemap(in, numinputs=0) lua_State *
%{
	$1 = L;
%}
#elseif SWIGPYTHON
#endif


/* This typemap converts the output of the plugin reference's "Create"
 * function from the general "muhkuh_plugin" type to the type of this
 * interface. It transfers the ownership of the object to Lua (this is the
 * last parameter in the call to "SWIG_NewPointerObj").
 */
#ifdef SWIGLUA
%typemap(out) muhkuh_plugin *
%{
	SWIG_NewPointerObj(L,result,((muhkuh_plugin_reference const *)arg1)->GetTypeInfo(),1); SWIG_arg++;
%}
#elseif SWIGPYTHON
#endif

