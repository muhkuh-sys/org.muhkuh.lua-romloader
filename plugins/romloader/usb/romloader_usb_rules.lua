-------------------------------------------------------------------------------
-- Rules to build the plugin bindings for wxLua
--  load using : $lua -e"rulesFilename=\"PLUGINNAME_rules.lua\"" genwxbind.lua
-------------------------------------------------------------------------------

-------------------------------------------------------------------------------
-- Set the root directory of the wxLua distribution, used only in this file
plugin_dir =  "./"

--=============================================================================
-- Set the lua namespace (lua table) that the bindings will be placed into.
--   eg. wx.wxWindow(...)
hook_lua_namespace = "muhkuh"

-- Set the C++ "namespace" that the bindings will be placed.
--   eg. cpp function names contain this to prevent duplicate function names
hook_cpp_namespace = "romloader_usb_lua"

--=============================================================================
-- Set the directory to output the bindings to, both C++ header and source files
output_cpp_header_filepath = "_luaif"
output_cpp_filepath        = "_luaif"
output_lua_filepath        = "_luaif"

--=============================================================================
-- Set the DLLIMPEXP macros for compiling these bindings into a DLL
--  Use "WXLUA_NO_DLLIMPEXP" and "WXLUA_NO_DLLIMPEXP_DATA" for no IMPEXP macros

output_cpp_impexpsymbol     = "WXLUA_NO_DLLIMPEXP"
output_cpp_impexpdatasymbol = "WXLUA_NO_DLLIMPEXP_DATA"

-------------------------------------------------------------------------------
-- Set the name of the header file that will have the #includes from the
--   bindings in it. This will be used as #include "hook_cpp_header_filename" in
--   the C++ wrapper files, so it must include the proper #include path.
hook_cpp_header_filename = "romloader_usb_wxlua_bindings.h"

-------------------------------------------------------------------------------
-- Set the name of the main binding file that will have the glue code for the
--   bindings in it. This file along with the output from the *.i files will be
--   placed in the "output_cpp_filepath".
hook_cpp_binding_filename = hook_cpp_namespace.."_bind.cpp"

-------------------------------------------------------------------------------
-- Set the name of the subclassed wxLuaBinding class
hook_cpp_binding_classname = "wxLuaBinding_"..hook_cpp_namespace

-------------------------------------------------------------------------------
-- Set the function names that wrap the output structs of defined values,
--   objects, events, functions, and classes.
hook_cpp_define_funcname   = "wxLuaGetDefineList_"..hook_cpp_namespace
hook_cpp_string_funcname   = "wxLuaGetStringList_"..hook_cpp_namespace
hook_cpp_object_funcname   = "wxLuaGetObjectList_"..hook_cpp_namespace
hook_cpp_event_funcname    = "wxLuaGetEventList_"..hook_cpp_namespace
hook_cpp_function_funcname = "wxLuaGetFunctionList_"..hook_cpp_namespace
hook_cpp_class_funcname    = "wxLuaGetClassList_"..hook_cpp_namespace

-------------------------------------------------------------------------------
-- Set any #includes or other C++ code to be placed verbatim at the top of
--   every generated cpp file or "" for none
hook_cpp_binding_includes = ""

-------------------------------------------------------------------------------
-- Set any #includes or other C++ code to be placed verbatim below the
--   #includes of every generated cpp file or "" for none
hook_cpp_binding_post_includes = ""

-------------------------------------------------------------------------------
-- Add additional include information or C++ code for the binding header file.
--  This code will be place directly after any #includes at the top of the file
hook_cpp_binding_header_includes = ""

-------------------------------------------------------------------------------
-- Set any #includes or other C++ code to be placed verbatim at the top of
--   the single hook_cpp_binding_filename generated cpp file or "" for none
hook_cpp_binding_source_includes = ""

--=============================================================================
-- Set the bindings directory that contains the *.i interface files
interface_filepath = "."

-------------------------------------------------------------------------------
-- A list of interface files to use to make the bindings. These files will be
--   converted into *.cpp and placed in the output_cpp_filepath directory.
--   The files are loaded from the interface_filepath.
interface_fileTable =
{
	"romloader_usb_lua.i"
}

-------------------------------------------------------------------------------
-- A list of files that contain bindings that need to be overridden or empty
--   table {} for none.
--   The files are loaded from the interface_filepath.
override_fileTable =
{
	"../romloader_lua_override.h"
}

--=============================================================================
-- A table containing filenames of XXX_datatype.lua from other wrappers to
--  to define classes and data types used in this wrapper
--  NOTE: for the base wxWidgets wrappers we don't load the cache since they
--        don't depend on other wrappers and can cause problems when interface
--        files are updated. Make sure you delete or have updated any cache file
--        that changes any data types used by this binding.

datatype_cache_input_fileTable = { "../../wx_datatypes.lua" }

-------------------------------------------------------------------------------
-- The file to output the data type cache for later use with a binding that
--   makes use of data types (classes, enums, etc) that are declared in this
--   binding. The file will be generated in the interface_filepath.

datatypes_cache_output_filename = output_lua_filepath.."/"..hook_cpp_namespace.."_datatypes.lua"

--=============================================================================
-- virtual void wxLuaBinding::PreRegister function body for the
--   hook_cpp_binding_classname. You can initialize data here.
--   Typically this is not necessary and you can rem this out.
-- wxLuaBinding_PreRegister = nothing to do here

--=============================================================================
-- virtual void wxLuaBinding::PostRegister function body for the
--   hook_cpp_binding_classname. You can load any other custom bindings here.
--   Typically this is not necessary and you can rem this out.
-- wxLuaBinding_PostRegister = nothing to do here

--=============================================================================
-- Add additional conditions here
-- example: conditions["DOXYGEN_INCLUDE"] = "defined(DOXYGEN_INCLUDE)"

-------------------------------------------------------------------------------
-- Add additional data types here
-- example: AllocDataType("wxArrayInt", "class",false)

--=============================================================================
-- Generate comments into binding C++ code
comment_cpp_binding_code = true
