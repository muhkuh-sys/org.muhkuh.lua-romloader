# ======================================================================================
# Author: Francesco Montorsi
# RCS-ID: $Id: configure.ac,v 1.46 2007/08/01 10:08:22 frm Exp $
# ======================================================================================


# WXLUA VERSION
# =============
#
# The second argument of AC_INIT must be the same value of:
# 1) build\innosetup\wxlua.iss
# 2) WXLUA_RELEASE variable in build/bakefiles/wxluabase.bkl
AC_INIT([wxLua], [0], [frm@users.sourceforge.net])

# ENABLES/DISABLES THE DEBUG MODE FOR THIS CONFIGURE SCRIPT
#DBG_CONFIGURE=1

m4_include(utils.m4)
m4_include(wxcodecheck.m4)

AC_PREREQ([2.57])
AC_CONFIG_AUX_DIR([build/autoconf])
AC_LANG(C++)





## CONFIGURE OPTIONS
#####################################################################

# standard wxWidgets options:
WX_CONFIG_OPTIONS

# the --enable-* and --with-* wx-related options
WX_STANDARD_OPTIONS([debug,unicode,shared,toolkit,wxshared,wxversion])

# we want to always have DEBUG==WX_DEBUG and UNICODE==WX_UNICODE
WX_DEBUG=$DEBUG
WX_UNICODE=$UNICODE

AC_ARG_ENABLE([muhkuh],
                [AC_HELP_STRING([--enable-romloader-usb], [Compiles the romloader usb plugin (default is no)])],
                [], [enable_romloader_usb=no])

AC_ARG_ENABLE([muhkuh],
                [AC_HELP_STRING([--enable-romloader-dummy], [Compiles the romloader dummy plugin (default is yes)])],
                [], [enable_romloader_dummy=yes])


AC_ARG_WITH([lua-prefix],
                [AC_HELP_STRING([--with-lua-prefix], [Prefix where Lua is installed (optional)])],
                [lua_dir="$withval"], [lua_dir=""])


UTILS_YESNO_OPTCHECK([BUILD_PLUGIN_ROMLOADER_USB], [enable_romloader_usb], [--enable-romloader-usb])
UTILS_YESNO_OPTCHECK([BUILD_PLUGIN_ROMLOADER_BAKA], [enable_romloader_dummy], [--enable-romloader-dummy])



AC_MSG_CHECKING([for the --with-lua-prefix option])
if test ! $lua_dir = ""; then

    # user explicitely provided a path to its own lua, check that it's an absolute path
    # (or it won't work for out-of-tree builds)
    case $lua_dir in
    /*) continue;;
    *)
        AC_MSG_ERROR([
    You must provide an absolute path for the --with-lua-prefix option.
        ])
    esac
fi
AC_MSG_RESULT([$lua_dir])


## CONFIGURE CHECKS
#####################################################################

# NB: to avoid getting "-g -02" automatically added to C*FLAGS we need
#     to explicitely initialize CFLAGS and CXXFLAGS to empty
CFLAGS=
CXXFLAGS=

AC_CANONICAL_SYSTEM
AC_PROG_CC
AC_PROG_CXX

WXLIBS_REQUIRED="aui,xrc,html,adv,net,xml,core,base"
AC_SUBST(WXLIBS_REQUIRED)

# standard wxWidgets checks:
WX_CONVERT_STANDARD_OPTIONS_TO_WXCONFIG_FLAGS
WX_CONFIG_CHECK([2.8.0], [wxWin=1], , [$WXLIBS_REQUIRED], [$WXCONFIG_FLAGS])
WX_DETECT_STANDARD_OPTION_VALUES


# check for Lua
#
# NB: by default we try to use the system-wide lua 5.1; if it does not
#     exist then we use our built-in lua

if test "$lua_dir" = ""; then

    UTILS_SAVE_COREVAR()

    LIBS="$LIBS -llua5.1"
    AC_LINK_IFELSE([
                    AC_LANG_PROGRAM([#include <lua.h>],
                                    [lua_newtable(NULL);])
                    ],
                    [LUA_PRESENCE=1], [LUA_PRESENCE=0])

    # NOTE: put this AC_MSG_CHECKING macro call immediately before
    #       the AC_MSG_RESULT macro calls as otherwise messages coming
    #       from other macros called by AC_LINK_IFELSE may interfere
    #       in the final output
    AC_MSG_CHECKING([if Lua 5.1 is installed])
    if test "$LUA_PRESENCE" = "1"; then
        AC_MSG_RESULT([yes])
        USE_SYSTEM_LUA=1
    else
        AC_MSG_RESULT([not found, falling back to the built-in lua])
        USE_SYSTEM_LUA=0

        # LUA_DIR must always be an absolute path!
        LUA_DIR="`pwd`/$srcdir/modules/lua"
        AC_SUBST([LUA_DIR])
    fi

    UTILS_RESTORE_COREVAR()        # remove the -llua5.1 piece from $LIBS

else

    USE_SYSTEM_LUA=1
    LUA_DIR="$lua_dir"
fi


if test "$USE_SYSTEM_LUA" = "0"; then

    # Checks used to determine right LUA flags (see modules/lua/src/Makefile)
    luadefs=
    lualibs=

    # these do not require additional libs
    AC_CHECK_FUNC([isatty], [luadefs="$luadefs -DLUA_USE_ISATTY"])
    AC_CHECK_FUNC([mkstemp], [luadefs="$luadefs -DLUA_USE_MKSTEMP"])
    AC_CHECK_FUNC([popen], [luadefs="$luadefs -DLUA_USE_POPEN"])
    AC_CHECK_FUNC([_longjmp], [luadefs="$luadefs -DLUA_USE_ULONGJMP"])

    # these require addlibs
    AC_CHECK_LIB([dl], [dlopen], [
                    AC_CHECK_HEADER([dlfcn.h], [
                                luadefs="$luadefs -DLUA_USE_DLOPEN"
                                lualibs="$lualibs -ldl"
                            ])
                ])
    AC_CHECK_LIB([readline], [readline], [
                    luadefs="$luadefs -DLUA_USE_READLINE"
                    lualibs="$lualibs -lreadline"
                ])

    LUA_PLATFORM_SPECIFIC_CFLAGS=$luadefs
    LUA_PLATFORM_SPECIFIC_LDFLAGS=$lualibs
    AC_SUBST(LUA_PLATFORM_SPECIFIC_CFLAGS)
    AC_SUBST(LUA_PLATFORM_SPECIFIC_LDFLAGS)

fi




dnl ---------------------------------------------------------------------------
dnl WX_STC_CHECK(
dnl         [action if found],
dnl         [action if not found])
dnl ---------------------------------------------------------------------------
AC_DEFUN([WX_STC_CHECK],
        [
            dnl save original flags into support variables
            UTILS_SAVE_COREVAR()

            dnl add to the compilation & link flags the wxWidgets flags
            dnl
            dnl NOTE: this is very important since the test program
            dnl       we will compile needs to be linked against wx
            dnl       and the configure script will use these *FLAGS
            dnl       variables and it cannot be told to use other flags.
            dnl
            dnl NOTE2: we don't want to put into *FLAGS variables the
            dnl        WX_* variable values since the Makefile.in for
            dnl        a wxpresets-based application already merges
            dnl        the *FLAGS contents with WX_* vars
            dnl
            dnl NOTE3: the CXXFLAGS are merged with CPPFLAGS and
            dnl        thus we don't need to add the WX_CPPFLAGS to both
            CPPFLAGS="$CPPFLAGS $WX_CPPFLAGS"
            CFLAGS="$CFLAGS $WX_CFLAGS_ONLY"

            dnl before WX_LIBS we want the lib of STC
            WX_LIKE_LIBNAME([_WX_STC_LIBNAME], [wx], [stc])
            LIBS="$LIBS -l$_WX_STC_LIBNAME $WX_LIBS"

            dnl check for this component presence
            AC_MSG_CHECKING([if wxSTC contrib is available])
            AC_LINK_IFELSE([
                            AC_LANG_PROGRAM([#include <wx/stc/stc.h>],
                                            [wxStyledTextCtrl dummy;])
                            ],
                            [WXSTC_PRESENCE=1], [WXSTC_PRESENCE=0])

            if test "$WXSTC_PRESENCE" = "1"; then
                AC_MSG_RESULT([yes])
                $1
            else
                AC_MSG_RESULT([no])
                $2
            fi

            dnl restore the original flags
            UTILS_RESTORE_COREVAR()
        ])


# check for STC wrapper for wxWidgets
if [[ "$USE_WXBINDSTC" = "1" ]]; then
    AC_CACHE_VAL([ac_cv_prog_stc_set],
                 [WX_STC_CHECK([ac_cv_prog_stc_set=1], [ac_cv_prog_stc_set=0])])

    if test $ac_cv_prog_stc_set = "1"; then
        # add to the library to link to the wxSTC lib
        WX_LIKE_LIBNAME([_WX_STC_LIBNAME], [wx], [stc])
        WX_LIBS="-l$_WX_STC_LIBNAME $WX_LIBS"
    else
        WXBUILD_STR="$($WX_CONFIG_WITH_ARGS --selected_config)"
        AC_MSG_NOTICE([
    The wxWidgets wrappers for Scintilla text control could not be found
    (at least not for the '$WXBUILD_STR' wxWidgets build).
    Please be sure that you compiled and installed it since they are
    not built together with wxWidgets by default.
    E.g. from wxWidgets build directory type:

        cd contrib/src/stc && make && sudo make install
        cd contrib/samples/stc && make

    If you still get this error message, look at config.log for more details
    or report this problem to $PACKAGE_BUGREPORT.
    NOTE: build is proceeding without Scintilla wrappers, but you will miss
          the wxLuaApp and wxLuaEdit applications!
        ])
        USE_WXBINDSTC=0
        USE_WXLUAAPP=0
        USE_WXLUAEDITAPP=0
    fi
else

    # these depends from wxSTC !
    USE_WXLUAAPP=0
    USE_WXLUAEDITAPP=0
fi

# Produce some useful info for pkg-config:
AC_DEFUN([AC_ADDMODULE],
    [
        dnl the set of the module names, comma separed
        WXLUA_MODNAMES="$WXLUA_MODNAMES $1"
        TMP="wxlua_$WX_PORT""$WXLIBPOSTFIX""_$1""-$WX_VERSION_MAJOR"".$WX_VERSION_MINOR"

        dnl the set of the wxLua library names
        WXLUA_LIBNAMES="$WXLUA_LIBNAMES $TMP"

        dnl the set of the wxLua library names prefixed with -l
        WXLUA_LDFLAGS="$WXLUA_LDFLAGS -l$TMP"
    ])

AC_ADDMODULE([wxbindadv])
AC_ADDMODULE([wxbindaui])
AC_ADDMODULE([wxbindbase])
AC_ADDMODULE([wxbindcore])
AC_ADDMODULE([wxbindhtml])
AC_ADDMODULE([wxbindmedia])
AC_ADDMODULE([wxbindnet])
AC_ADDMODULE([wxbindrichtext])
AC_ADDMODULE([wxbindstc])
AC_ADDMODULE([wxbindxml])
AC_ADDMODULE([wxbindxrc])

AC_ADDMODULE([wxluasocket])
AC_ADDMODULE([wxluadebug])
AC_ADDMODULE([wxlua])

WXLUA_LIBNAMES="$WXLUA_LIBNAMES lua5.1"
WXLUA_LDFLAGS="$WXLUA_LDFLAGS -llua5.1"

AC_SUBST([WXLUA_MODNAMES])
AC_SUBST([WXLUA_LIBNAMES])
AC_SUBST([WXLUA_LDFLAGS])

# now that we know the wxWidgets' version, we can update the PACKAGE_VERSION var
PACKAGE_VERSION="$WX_VERSION_FULL.$PACKAGE_VERSION"






## END OF CONFIGURE SCRIPT
#####################################################################

AC_BAKEFILE([m4_include(autoconf_inc.m4)])
AC_CONFIG_FILES([Makefile build/autoconf/wxlua.pc])

AC_OUTPUT
