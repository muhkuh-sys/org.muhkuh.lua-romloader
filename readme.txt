Muhkuh 1.0.228
-----------------------------------------------------------------------------

Muhkuh is a powerful and flexible test tool for hardware designs. Plugins are used to access the device under test or the testing equipment. Testcases are written in Lua and can be easily extended by the plugins.

This is a beta-release. Many things are still missing - like documentation, so I guess this one only for my brave testers. Thanks guys, you are doing a great job!



Changes:
--------

Please see "changes.txt".



Download:
---------

Source and binaries can be downloaded from the Sourceforge page:
http://sourceforge.net/project/showfiles.php?group_id=207719

The version under development is available from SVN. Please check this page for more information:
http://sourceforge.net/svn/?group_id=207719


Firmware patch files
--------------------

 python mbs/mbs
 ls plugins/romloader/uart/netx/targets/uartmon_firmware.diff
 ls plugins/romloader/usb/netx/targets/usbmon_firmware.diff


Source distribution
-------------------

 hg archive --prefix muhkuh-HG%h --type tbz2 --subrepos muhkuh-HG%h-source.tar.bz2


How to build muhkuh:
--------------------

Linux with configure:

 You will need the GNU autoconf, automake and maketools.

 1. Grab the latest stable release of the wxAll package. The ".tar.bz2" achive is the best choice.
 http://sourceforge.net/project/showfiles.php?group_id=9863&package_id=58517


 2. Unpack the archive to a temp directory:

 % tar fxj wxWidgets-2.8.5.tar.bz2


 3. Build the wxWidgets release libraries:

 % mkdir build_gtk_release
 % cd build_gtk_release
 % ../configure --disable-debug --disable-unicode --with-gtk --enable-shared --with-odbc
 % make
 % su
 % make install
 % ldconfig /usr/local/lib
 (type ctrl-d or exit to leave root mode)
 % cd contrib/src/stc
 % make
 % su
 % make install
 % ldconfig /usr/local/lib
 (type ctrl-d or exit to leave root mode

 NOTE: The libraries must be compiled in shared mode. Static libraries will not work with the Muhkuh application.


 4. Build the wxWidgets debug libraries:
 NOTE: This step is only necessary if you want to develop or debug Muhkuh. The debug libraries are not needed for normal use of the Muhkuh application.

 % mkdir build_gtk_debug
 % cd build_gtk_debug
 % ../configure --enable-debug --disable-unicode --with-gtk --enable-shared --with-odbc
 % make
 % su
 % make install
 % ldconfig /usr/local/lib
 (type ctrl-d or exit to leave root mode)
 % cd contrib/src/stc
 % make
 % su
 % make install
 % ldconfig /usr/local/lib
 (type ctrl-d or exit to leave root mode)

 NOTE: The libraries must be compiled in shared mode. Static libraries will not work with the Muhkuh application.


 5. Grab the ".tar.gz" archive of the latest stable wxLua release and unpack it to a temp directory.
 http://sourceforge.net/project/showfiles.php?group_id=140042

 They seem to be a bit sloppy with the source code releases. If the source is missing for the latest stable, get the latest nightly snapshot instead:

 http://wxlua.sourceforge.net/download/


 6. Build the wxLua release libraries:

 % mkdir build_gtk_release
 % cd build_gtk_release
 % ../configure --disable-debug --disable-unicode --enable-shared --enable-systemlua
 % make
 % su
 % make install
 % ldconfig /usr/local/lib
 % cp ../modules/wxbind/include/wxbinddefs.h /usr/local/include/wxbind/include/
 (type ctrl-d or exit to leave root mode)

 NOTE: The libraries must be compiled in shared mode. Static libraries will not work with the Muhkuh application.


 7. Build the wxLua debug libraries:
 NOTE: This step is only necessary if you want to develop or debug Muhkuh. The debug libraries are not needed for normal use of the Muhkuh application.

 % mkdir build_gtk_debug
 % cd build_gtk_debug
 % ../configure --enable-debug --disable-unicode --enable-shared --enable-systemlua
 % make
 % su
 % make install
 % ldconfig /usr/local/lib
 % cp ../modules/wxbind/include/wxbinddefs.h /usr/local/include/wxbind/include/
 (type ctrl-d or exit to leave root mode)

 NOTE: The libraries must be compiled in shared mode. Static libraries will not work with the Muhkuh application.


 8. Building the release version of Muhkuh:

 % ./bootstrap
 % make -f makefile.unx



Windows with MSVS2007:

 Get the prerequisites:
 
 * libiconv, libintl, m4 and diffutils from http://sourceforge.net/projects/gnuwin32/
 You will just need 'libiconv2.dll' from the libiconv package, 'libintl3.dll' from the libintl package, 'm4.exe' from the m4 package and 'cmp.exe' from the diffutils, it's safe to delete the rest. Copy the dll and exe files to the build directory (where bootstrap.bat is).
 
 * the subversion commandline-binaries from http://subversion.tigris.org/
 
 There are 2 ways to compile Muhkuh with Microsoft Visual Studio 2007: the easy way using the precompiled wxPack, and the annoying way where you do it all yourself.
 So you might ask why compiling wxWidgets can be annoying with all the provided build scripts - there are even complete Visual Studio project files. The problem is: all my compile boxes build the wx_*_gl.dll, but not the import lib and I have no idea why. The wxLua build env depends on wx_*_gl.dll and there is no way to get it out of the build, but to edit the makefiles. That's the annoying part.


 The easy way with wxPack:

 1. Get the latest wxPack from http://wxpack.sf.net .


 2. Install wxPack. Make sure to choose the component

 "wxWidgets Compiled By:" -> "Visual C++" -> "Dll's"

 All other parts are optional for the compilation of Muhkuh.


 3. Open the Visual Studio Command Prompt. Move to the new directory containing the wxPack files, then change to the "contrib\build\stc" subdirectory.
 Compile the stc libs in Release mode:

 nmake -f makefile.vc SHARED=1 UNICODE=0 BUILD=release

 Compile the stc libs in Debug mode:
 NOTE: This step is only necessary if you want to develop or debug Muhkuh. The debug libraries are not needed for normal use of the Muhkuh application.

 nmake -f makefile.vc SHARED=1 UNICODE=0 BUILD=debug


 4. Get the latest wxLua sourcecode version from http://wxlua.sf.net .
 NOTE: Get the source package even if the download site claims that "you won't need anything else" than the binary or dll package. Only the source package contains the header files.


 5. Extract the wxLua source somewhere. If you wonder how to handle "tar.gz", get 7zip .


 6. Open the Visual Studio Command Prompt. Move to the extracted wxLua source, then change to the "build\msw" subdirectory.
 Compile wxLua in release mode:

 nmake -f makefile.vc BUILD=release UNICODE=0 SHARED=1 WX_SHARED=1

 Compile wxLua in debug mode:
 NOTE: This step is only necessary if you want to develop or debug Muhkuh. The debug libraries are not needed for normal use of the Muhkuh application.

 nmake -f makefile.vc BUILD=debug UNICODE=0 SHARED=1 WX_SHARED=1


 7. Create a new environment variable with the name "WXLUA" which contains the path to the extracted wxLua Sources.
 Example:

 I extracted the wxLua snapshot to "C:\SourceCode\wxLua". I set the environment variable "WXLUA" to "C:\SourceCode\wxLua", so that a "dir %WXLUA% shows the following result:

 C:\>dir /B %WXLUA%
 apps
 art
 bin
 bindings
 build
 configure
 CVS
 distrib
 docs
 lib
 Makefile.in
 modules
 samples
 util


 8. Open the Visual Studio Command Prompt, then move to the Muhkuh source.
 Compile Muhkuh in release mode:

 nmake -f makefile.vc BUILD=release

 Compile Muhkuh in debug mode:
 NOTE: This step is only necessary if you want to develop or debug Muhkuh.

 nmake -f makefile.vc BUILD=debug



 The annoying way without wxPack:

 Coming soon...



Contact:
--------

Feature requests, Bugreports and Patches are most welcome. The recommended way to submit them is the tracker on Sourceforge:
http://sourceforge.net/tracker/?group_id=207719

There are also forums for general discussion and help requests:
http://sourceforge.net/forum/?group_id=207719

--
The Muhkuh team
