This directory holds all bakefile related stuff. You need bakefile 0.2.3 or
later.

Run 'bakefile_gen' in this directory to generate all makefiles and projects.

Most of the files come directly from the wxLua distribution and are specific
to one wxLua version. The version is in the file 'wxluabase.bkl'. Look for
these lines:

	<set var="WXLUA_RELEASE">4</set>

	<!-- The default version of wxWidgets which is used by wxLua;
	     if you want to use other versions, you'll have to specify it
	     using the WX_VERSION option -->
	<set var="WX_VERSION_DEFAULT">28</set>

'WXLUA_RELEASE' seems to be the micro version and 'WX_VERSION_DEFAULT' the
major and minor. The example above is from wxLua version 2.8.4 .
If you want to compile Muhkuh with a different wxLua version than the one
specified in 'wxluabase.bkl' copy the following files from your wxLua source
tree. It is very important that you do this after building wxLua as
'options.bkl' is generated during the build process.

	build/bakefiles/mac_bundles.bkl
	build/bakefiles/options.bkl
	build/bakefiles/wxluabase.bkl
	build/bakefiles/presets/wx.bkl
	build/bakefiles/presets/wx_unix.bkl
	build/bakefiles/presets/wx_win32.bkl

