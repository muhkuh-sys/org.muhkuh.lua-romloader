#! /usr/bin/python2.7

from jonchki import cli_args
from jonchki import filter
from jonchki import jonchkihere
from jonchki import vcs_id

import os
import shutil
import subprocess
import sys


tPlatform = cli_args.parse()
print('Building for %s' % tPlatform['platform_id'])
strCfg_jonchkiVerbose = 'info'

# --------------------------------------------------------------------------
# -
# - Configuration
# -

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
strCfg_jonchkiVersion = '0.0.3.1'
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

print(tPlatform)

astrCMAKE_COMPILER = None
astrCMAKE_PLATFORM = None
astrJONCHKI_SYSTEM = None
strMake = None

if tPlatform['host_distribution_id'] == 'ubuntu':
    if (tPlatform['platform_id'] == 'local') or (tPlatform['distribution_id'] == 'ubuntu'):
        # Build on linux for linux.
        # It is currently not possible to build for another version or CPU architecture.
        if(
            (tPlatform['platform_id'] != 'local') and
            ((tPlatform['distribution_version'] != tPlatform['host_distribution_version']) or
            (tPlatform['cpu_architecture'] != tPlatform['host_cpu_architecture']))
        ):
            raise Exception('The target Ubuntu platform must match the build host.')

        # Check for all system dependencies.
        astrDeb = [
            'libudev-dev'
        ]
        astrInstall = []
        for strDeb in astrDeb:
            strDpkgStatus = subprocess.check_output("dpkg-query -W -f='${Status}' %s || echo 'unknown'" % strDeb, shell=True)
            print('Check for %s = %s' % (strDeb, strDpkgStatus))
            if strDpkgStatus != 'install ok installed':
                astrInstall.append(strDeb)
        if len(astrInstall) != 0:
            subprocess.check_call('sudo apt-get update --assume-yes', shell=True)
            subprocess.check_call('sudo apt-get install --assume-yes %s' % ' '.join(astrInstall), shell=True)

        astrCMAKE_COMPILER = []
        astrCMAKE_PLATFORM = []
        astrJONCHKI_SYSTEM = []
        strMake = 'make'

    elif tPlatform['distribution_id'] == 'windows':
        # Cross build on linux for windows.
        if tPlatform['cpu_architecture'] == 'x86':
            # Build for 32bit windows.
            strCompilerPath = '/usr/mingw-w64-i686/bin'
            astrCMAKE_COMPILER = [
                '-DCMAKE_C_FLAGS=-m32',
                '-DCMAKE_CXX_FLAGS=-m32',
                '-DCMAKE_SYSTEM_NAME=Windows',
                '-DCMAKE_C_COMPILER=%s/i686-w64-mingw32-gcc' % strCompilerPath,
                '-DCMAKE_CXX_COMPILER=%s/i686-w64-mingw32-g++' % strCompilerPath,
                '-DCMAKE_RC_COMPILER=%s/i686-w64-mingw32-windres' % strCompilerPath
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
            strCompilerPath = '/usr/mingw-w64-x86_64/bin'
            astrCMAKE_COMPILER = [
                '-DCMAKE_C_FLAGS=-m64',
                '-DCMAKE_CXX_FLAGS=-m64',
                '-DCMAKE_SYSTEM_NAME=Windows',
                '-DCMAKE_C_COMPILER=%s/x86_64-w64-mingw32-gcc' % strCompilerPath,
                '-DCMAKE_CXX_COMPILER=%s/x86_64-w64-mingw32-g++' % strCompilerPath,
                '-DCMAKE_RC_COMPILER=%s/x86_64-w64-mingw32-windres' % strCompilerPath
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


# Create the folders if they do not exist yet.
astrFolders = [
    strCfg_workingFolder,
    os.path.join(strCfg_workingFolder, 'external'),
    os.path.join(strCfg_workingFolder, 'lua5.1'),
    os.path.join(strCfg_workingFolder, 'lua5.1', 'build_requirements'),
    os.path.join(strCfg_workingFolder, 'lua5.2'),
    os.path.join(strCfg_workingFolder, 'lua5.2', 'build_requirements'),
    os.path.join(strCfg_workingFolder, 'lua5.3'),
    os.path.join(strCfg_workingFolder, 'lua5.3', 'build_requirements')
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

# Try to get the VCS ID.
strProjectVersionVcs, strProjectVersionVcsLong = vcs_id.get(
    strCfg_projectFolder
)
print(strProjectVersionVcs, strProjectVersionVcsLong)


# ---------------------------------------------------------------------------
#
# Build the externals.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.append(os.path.join(strCfg_projectFolder, 'external'))
strCwd = os.path.join(strCfg_workingFolder, 'external')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call(strMake, shell=True, cwd=strCwd)

# ---------------------------------------------------------------------------
#
# Get the build requirements for LUA5.1.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DBUILDCFG_ONLY_JONCHKI_CFG="ON"',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.1"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.1', 'build_requirements')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call(strMake, shell=True, cwd=strCwd)

astrCmd = [
    strJonchki,
    'install-dependencies',
    '--verbose', strCfg_jonchkiVerbose,
    '--syscfg', strCfg_jonchkiSystemConfiguration,
    '--prjcfg', strCfg_jonchkiProjectConfiguration
]
astrCmd.extend(astrJONCHKI_SYSTEM)
astrCmd.append('--build-dependencies')
astrCmd.append('lua5.1-romloader-*.xml')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)


# ---------------------------------------------------------------------------
#
# Build the LUA5.1 version.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.1"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.1')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call('%s pack' % strMake, shell=True, cwd=strCwd)

# ---------------------------------------------------------------------------
#
# Get the build requirements for LUA5.2.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DBUILDCFG_ONLY_JONCHKI_CFG="ON"',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.2"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.2', 'build_requirements')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call(strMake, shell=True, cwd=strCwd)

astrCmd = [
    strJonchki,
    'install-dependencies',
    '--verbose', strCfg_jonchkiVerbose,
    '--syscfg', strCfg_jonchkiSystemConfiguration,
    '--prjcfg', strCfg_jonchkiProjectConfiguration
]
astrCmd.extend(astrJONCHKI_SYSTEM)
astrCmd.append('--build-dependencies')
astrCmd.append('lua5.2-romloader-*.xml')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)


# ---------------------------------------------------------------------------
#
# Build the LUA5.2 version.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.2"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.2')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call('%s pack' % strMake, shell=True, cwd=strCwd)

# ---------------------------------------------------------------------------
#
# Get the build requirements for LUA5.3.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DBUILDCFG_ONLY_JONCHKI_CFG="ON"',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.3"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.3', 'build_requirements')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call(strMake, shell=True, cwd=strCwd)

astrCmd = [
    strJonchki,
    'install-dependencies',
    '--verbose', strCfg_jonchkiVerbose,
    '--syscfg', strCfg_jonchkiSystemConfiguration,
    '--prjcfg', strCfg_jonchkiProjectConfiguration
]
astrCmd.extend(astrJONCHKI_SYSTEM)
astrCmd.append('--build-dependencies')
astrCmd.append('lua5.3-romloader-*.xml')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)


# ---------------------------------------------------------------------------
#
# Build the LUA5.3 version.
#
astrCmd = [
    'cmake',
    '-DCMAKE_INSTALL_PREFIX=""',
    '-DBUILDCFG_LUA_USE_SYSTEM="OFF"',
    '-DBUILDCFG_LUA_VERSION="5.3"'
]
astrCmd.extend(astrCMAKE_COMPILER)
astrCmd.extend(astrCMAKE_PLATFORM)
astrCmd.append(strCfg_projectFolder)
strCwd = os.path.join(strCfg_workingFolder, 'lua5.3')
subprocess.check_call(' '.join(astrCmd), shell=True, cwd=strCwd)
subprocess.check_call('%s pack' % strMake, shell=True, cwd=strCwd)
