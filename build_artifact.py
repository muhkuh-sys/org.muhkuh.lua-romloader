#! /usr/bin/python3

from jonchki import cli_args
from jonchki import install
from jonchki import jonchkihere

import glob
import os
import subprocess


tPlatform = cli_args.parse()
print('Building for %s' % tPlatform['platform_id'])


# --------------------------------------------------------------------------
# -
# - Configuration
# -

# Only if you are building on Ubuntu for Windows:
# Select the path of the MinGW-w64 cross-compiler running on Ubuntu and
# building for Windows 32bit.
strCfg_CompilerPath_Ubuntu_MinGw_w64_i686 = '/usr/mingw-w64-i686/bin'

# Only if you are building on Ubuntu for Windows:
# Select the path of the MinGW-w64 cross-compiler running on Ubuntu and
# building for Windows 64bit.
strCfg_CompilerPath_Ubuntu_MinGw_w64_x86_64 = '/usr/mingw-w64-x86_64/bin'

# Only if you are building on Windows:
# Select the path of the compiler for Windows 32bit.
strCfg_CompilerPath_Windows_MinGw_w64_i686 = 'C:/msys64/mingw32/bin'

# Only if you are building on Windows:
# Select the path of the compiler for Windows 64bit.
strCfg_CompilerPath_Windows_MinGw_w64_x86_64 = 'C:/msys64/mingw64/bin'

# Only if you are building on Windows:
# Select the path and name of the swig exe.
strCfg_SwigPath_Windows = 'C:/Swig/swig.exe'

# Get the project folder. This is the folder of this script.
strCfg_projectFolder = os.path.dirname(os.path.realpath(__file__))

# This is the complete path to the testbench folder. The installation will be
# written there.
strCfg_workingFolder = os.path.join(
    strCfg_projectFolder,
    'build',
    tPlatform['platform_id']
)

# Where is the jonchkihere tool?
strCfg_jonchkiHerePath = os.path.join(
    strCfg_projectFolder,
    'jonchki'
)
# This is the Jonchki version to use.
strCfg_jonchkiVersion = '0.0.11.1'
# Look in this folder for Jonchki archives before downloading them.
strCfg_jonchkiLocalArchives = os.path.join(
    strCfg_projectFolder,
    'jonchki',
    'local_archives'
)
# The target folder for the jonchki installation. A subfolder named
# "jonchki-VERSION" will be created there. "VERSION" will be replaced with
# the version number from strCfg_jonchkiVersion.
strCfg_jonchkiInstallationFolder = os.path.join(
    strCfg_projectFolder,
    'build'
)

# Select the verbose level for jonchki.
# Possible values are "debug", "info", "warning", "error" and "fatal".
strCfg_jonchkiVerbose = 'info'

strCfg_jonchkiSystemConfiguration = os.path.join(
    strCfg_projectFolder,
    'jonchki',
    'jonchkisys.cfg'
)
strCfg_jonchkiProjectConfiguration = os.path.join(
    strCfg_projectFolder,
    'jonchki',
    'jonchkicfg.xml'
)

# -
# --------------------------------------------------------------------------

astrCMAKE_COMPILER = None
astrCMAKE_PLATFORM = None
astrJONCHKI_SYSTEM = None
strMake = None
astrEnv = None

if tPlatform['host_distribution_id'] == 'ubuntu':
    if tPlatform['distribution_id'] == 'ubuntu':
        # Build on linux for linux.
        # It is currently not possible to build for another version of the OS.
        if tPlatform['distribution_version'] != tPlatform['host_distribution_version']:
            raise Exception('The target Ubuntu version must match the build host.')

        if tPlatform['cpu_architecture'] == tPlatform['host_cpu_architecture']:
            # Build for the build host.

            # Check for all system dependencies.
            astrDeb = [
                'libudev-dev'
            ]
            install.install_host_debs(astrDeb)

            astrCMAKE_COMPILER = []
            astrCMAKE_PLATFORM = []
            astrJONCHKI_SYSTEM = []
            strMake = 'make'

        elif tPlatform['cpu_architecture'] == 'arm64':
            # Build on linux for raspebrry.

            # Install the build dependencies.
            astrDeb = [
                'libudev-dev:arm64'
            ]
            install.install_foreign_debs(astrDeb, strCfg_workingFolder, strCfg_projectFolder)

            astrCMAKE_COMPILER = [
                '-DCMAKE_TOOLCHAIN_FILE=%s/cmake/toolchainfiles/toolchain_ubuntu_arm64.cmake' % strCfg_projectFolder
            ]
            astrCMAKE_PLATFORM = [
                '-DJONCHKI_PLATFORM_DIST_ID=%s' % tPlatform['distribution_id'],
                '-DJONCHKI_PLATFORM_DIST_VERSION=%s' % tPlatform['distribution_version'],
                '-DJONCHKI_PLATFORM_CPU_ARCH=%s' % tPlatform['cpu_architecture']
            ]

            astrJONCHKI_SYSTEM = [
                '--distribution-id %s' % tPlatform['distribution_id'],
                '--distribution-version %s' % tPlatform['distribution_version'],
                '--cpu-architecture %s' % tPlatform['cpu_architecture']
            ]
            strMake = 'make'

        else:
            raise Exception('Unknown CPU architecture: "%s"' % tPlatform['cpu_architecture'])

    elif tPlatform['distribution_id'] == 'windows':
        # Cross build on linux for windows.

        if tPlatform['cpu_architecture'] == 'x86':
            # Build for 32bit windows.
            astrCMAKE_COMPILER = [
                '-DCMAKE_TOOLCHAIN_FILE=%s/cmake/toolchainfiles/toolchain_windows_32.cmake' % strCfg_projectFolder
            ]
            astrCMAKE_PLATFORM = [
                '-DJONCHKI_PLATFORM_DIST_ID=windows',
                '-DJONCHKI_PLATFORM_DIST_VERSION=""',
                '-DJONCHKI_PLATFORM_CPU_ARCH=x86'
            ]
            astrJONCHKI_SYSTEM = [
                '--distribution-id windows',
                '--empty-distribution-version',
                '--cpu-architecture x86'
            ]
            strMake = 'make'

        elif tPlatform['cpu_architecture'] == 'x86_64':
            # Build for 64bit windows.
            astrCMAKE_COMPILER = [
                '-DCMAKE_TOOLCHAIN_FILE=%s/cmake/toolchainfiles/toolchain_windows_64.cmake' % strCfg_projectFolder
            ]
            astrCMAKE_PLATFORM = [
                '-DJONCHKI_PLATFORM_DIST_ID=windows',
                '-DJONCHKI_PLATFORM_DIST_VERSION=""',
                '-DJONCHKI_PLATFORM_CPU_ARCH=x86_64'
            ]
            astrJONCHKI_SYSTEM = [
                '--distribution-id windows',
                '--empty-distribution-version',
                '--cpu-architecture x86_64'
            ]
            strMake = 'make'

        else:
            raise Exception('Unknown CPU architecture: "%s"' % tPlatform['cpu_architecture'])

    else:
        raise Exception('Unknown distribution: "%s"' % tPlatform['distribution_id'])

elif tPlatform['host_distribution_id'] == 'windows':
    if tPlatform['distribution_id'] == 'windows':
        # Build on windows for windows.
        if tPlatform['cpu_architecture'] == 'x86':
            # Build for 32bit windows.
            astrCMAKE_COMPILER = [
                '-DCMAKE_C_FLAGS=-m32',
                '-DCMAKE_CXX_FLAGS=-m32',
                '-DCMAKE_SYSTEM_NAME=Windows',
                '-DCMAKE_AR=%s/ar.exe' % strCfg_CompilerPath_Windows_MinGw_w64_i686,
                '-DCMAKE_C_COMPILER=%s/i686-w64-mingw32-gcc.exe' % strCfg_CompilerPath_Windows_MinGw_w64_i686,
                '-DCMAKE_CXX_COMPILER=%s/i686-w64-mingw32-g++.exe' % strCfg_CompilerPath_Windows_MinGw_w64_i686,
                '-DCMAKE_RC_COMPILER=%s/i686-w64-mingw32-windres.exe' % strCfg_CompilerPath_Windows_MinGw_w64_i686,
                '-DSWIG_EXECUTABLE=%s' % strCfg_SwigPath_Windows,
                '-G "MinGW Makefiles"'
            ]
            astrCMAKE_PLATFORM = [
                '-DJONCHKI_PLATFORM_DIST_ID=windows',
                '-DJONCHKI_PLATFORM_DIST_VERSION=""',
                '-DJONCHKI_PLATFORM_CPU_ARCH=x86'
            ]
            astrJONCHKI_SYSTEM = [
                '--distribution-id windows',
                '--empty-distribution-version',
                '--cpu-architecture x86'
            ]
            strMake = '%s/mingw32-make.exe' % strCfg_CompilerPath_Windows_MinGw_w64_i686
            astrEnv = dict(
                os.environ,
                PATH='%s;%s' % (
                    strCfg_CompilerPath_Windows_MinGw_w64_i686,
                    os.environ['PATH']
                )
            )

        elif tPlatform['cpu_architecture'] == 'x86_64':
            # Build for 64bit windows.
            astrCMAKE_COMPILER = [
                '-DCMAKE_C_FLAGS=-m64',
                '-DCMAKE_CXX_FLAGS=-m64',
                '-DCMAKE_SYSTEM_NAME=Windows',
                '-DCMAKE_AR=%s/ar.exe' % strCfg_CompilerPath_Windows_MinGw_w64_x86_64,
                '-DCMAKE_C_COMPILER=%s/x86_64-w64-mingw32-gcc.exe' % strCfg_CompilerPath_Windows_MinGw_w64_x86_64,
                '-DCMAKE_CXX_COMPILER=%s/x86_64-w64-mingw32-g++.exe' % strCfg_CompilerPath_Windows_MinGw_w64_x86_64,
                '-DCMAKE_RC_COMPILER=%s/x86_64-w64-mingw32-windres.exe' % strCfg_CompilerPath_Windows_MinGw_w64_x86_64,
                '-DSWIG_EXECUTABLE=%s' % strCfg_SwigPath_Windows,
                '-G "MinGW Makefiles"'
            ]
            astrCMAKE_PLATFORM = [
                '-DJONCHKI_PLATFORM_DIST_ID=windows',
                '-DJONCHKI_PLATFORM_DIST_VERSION=""',
                '-DJONCHKI_PLATFORM_CPU_ARCH=x86_64'
            ]
            astrJONCHKI_SYSTEM = [
                '--distribution-id windows',
                '--empty-distribution-version',
                '--cpu-architecture x86_64'
            ]
            strMake = '%s/mingw32-make.exe' % strCfg_CompilerPath_Windows_MinGw_w64_x86_64
            astrEnv = dict(
                os.environ,
                PATH='%s;%s' % (
                    strCfg_CompilerPath_Windows_MinGw_w64_x86_64,
                    os.environ['PATH']
                )
            )

        else:
            raise Exception(
                'Unknown CPU architecture: "%s"' %
                tPlatform['cpu_architecture']
            )

else:
    raise Exception(
        'Unknown host distribution: "%s"' %
        tPlatform['host_distribution_id']
    )

# Create the folders if they do not exist yet.
astrFolders = [
    strCfg_workingFolder,
    os.path.join(strCfg_workingFolder, 'external'),
    os.path.join(strCfg_workingFolder, 'lua5.1'),
    os.path.join(strCfg_workingFolder, 'lua5.1', 'build_requirements'),
    os.path.join(strCfg_workingFolder, 'lua5.4'),
    os.path.join(strCfg_workingFolder, 'lua5.4', 'build_requirements'),
]
for strPath in astrFolders:
    if os.path.exists(strPath) is not True:
        os.makedirs(strPath)

# Install jonchki.
strJonchki = jonchkihere.install(
    strCfg_jonchkiVersion,
    strCfg_jonchkiInstallationFolder,
    LOCAL_ARCHIVES=strCfg_jonchkiLocalArchives
)


# ---------------------------------------------------------------------------
#
# Build the externals.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DPRJ_DIR=%s' % strCfg_projectFolder,
    '-DWORKING_DIR=%s' % strCfg_workingFolder
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(tPlatform['cmake_defines'])
astrCmd.append(os.path.join(strCfg_projectFolder, 'external'))
strCwd = os.path.join(strCfg_workingFolder, 'external')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)
subprocess.check_call(strMake, shell=True, cwd=strCwd, env=astrEnv)

# ---------------------------------------------------------------------------
#
# Get the build requirements for LUA5.1.
#
for strMatch in glob.iglob(os.path.join(strCwd, 'lua5.1-romloader-*.xml')):
    os.remove(strMatch)

astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DPRJ_DIR=%s' % strCfg_projectFolder,
    '-DWORKING_DIR=%s' % strCfg_workingFolder,
    '-DBUILDCFG_ONLY_JONCHKI_CFG="ON"',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.1"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.extend(tPlatform['cmake_defines'])
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.1', 'build_requirements')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)
subprocess.check_call(strMake, shell=True, cwd=strCwd, env=astrEnv)

astrMatch = glob.glob(os.path.join(strCwd, 'lua5.1-romloader-*.xml'))

for s in astrMatch:
    print(s)
# What if there are more than one matches?
if len(astrMatch) != 1:
    raise Exception('No match found for "lua5.1-romloader-*.xml".')

astrCmd = [
    strJonchki,
    'install-dependencies',
    '--verbose', strCfg_jonchkiVerbose,
    '--syscfg', strCfg_jonchkiSystemConfiguration,
    '--prjcfg', strCfg_jonchkiProjectConfiguration,
    '--logfile', os.path.join(strCwd, 'jonchki.log'),
    '--dependency-log', os.path.join(
        strCfg_projectFolder,
        'dependency-log-lua5.1.xml'
    )
]
astrCmd.extend(astrJONCHKI_SYSTEM)
astrCmd.append('--build-dependencies')
astrCmd.append(astrMatch[0])
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)

# ---------------------------------------------------------------------------
#
# Build the LUA5.1 version.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DPRJ_DIR=%s' % strCfg_projectFolder,
    '-DWORKING_DIR=%s' % strCfg_workingFolder,
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.1"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.extend(tPlatform['cmake_defines'])
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.1')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)
subprocess.check_call('%s pack' % strMake, shell=True, cwd=strCwd, env=astrEnv)

# ---------------------------------------------------------------------------
#
# Get the build requirements for LUA5.4.
#
for strMatch in glob.iglob(os.path.join(strCwd, 'lua5.4-romloader-*.xml')):
    os.remove(strMatch)

astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DPRJ_DIR=%s' % strCfg_projectFolder,
    '-DWORKING_DIR=%s' % strCfg_workingFolder,
    '-DBUILDCFG_ONLY_JONCHKI_CFG="ON"',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.4"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.extend(tPlatform['cmake_defines'])
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.4', 'build_requirements')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)
subprocess.check_call(strMake, shell=True, cwd=strCwd, env=astrEnv)

astrMatch = glob.glob(os.path.join(strCwd, 'lua5.4-romloader-*.xml'))
if len(astrMatch) != 1:
    raise Exception('No match found for "lua5.4-romloader-*.xml".')

astrCmd = [
    strJonchki,
    'install-dependencies',
    '--verbose', strCfg_jonchkiVerbose,
    '--syscfg', strCfg_jonchkiSystemConfiguration,
    '--prjcfg', strCfg_jonchkiProjectConfiguration,
    '--logfile', os.path.join(strCwd, 'jonchki.log'),
    '--dependency-log', os.path.join(
        strCfg_projectFolder,
        'dependency-log-lua5.4.xml'
    )
]
astrCmd.extend(astrJONCHKI_SYSTEM)
astrCmd.append('--build-dependencies')
astrCmd.append(astrMatch[0])
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)


# ---------------------------------------------------------------------------
#
# Build the LUA5.4 version.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DPRJ_DIR=%s' % strCfg_projectFolder,
    '-DWORKING_DIR=%s' % strCfg_workingFolder,
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.4"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.extend(tPlatform['cmake_defines'])
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.4')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd, env=astrEnv)
subprocess.check_call('%s pack' % strMake, shell=True, cwd=strCwd, env=astrEnv)
