# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------- #
#   Copyright (C) 2011 by Christoph Thelen                                #
#   doc_bacardi@users.sourceforge.net                                     #
#                                                                         #
#   This program is free software; you can redistribute it and/or modify  #
#   it under the terms of the GNU General Public License as published by  #
#   the Free Software Foundation; either version 2 of the License, or     #
#   (at your option) any later version.                                   #
#                                                                         #
#   This program is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         #
#   GNU General Public License for more details.                          #
#                                                                         #
#   You should have received a copy of the GNU General Public License     #
#   along with this program; if not, write to the                         #
#   Free Software Foundation, Inc.,                                       #
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             #
# ----------------------------------------------------------------------- #

import os.path

# ---------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('atEnv')

# Create a build environment for the ARM9 based netX chips.
env_arm9 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.7', 'asciidoc'])
env_arm9.CreateCompilerEnv('NETX500', ['arch=armv5te'])
env_arm9.CreateCompilerEnv('NETX56', ['arch=armv5te'])
env_arm9.CreateCompilerEnv('NETX50', ['arch=armv5te'])
env_arm9.CreateCompilerEnv('NETX10', ['arch=armv5te'])

env_arm9.CreateCompilerEnv('NETX500', ['arch=armv5te', 'thumb'], name='NETX500_THUMB')
env_arm9.CreateCompilerEnv('NETX56', ['arch=armv5te', 'thumb'], name='NETX56_THUMB')
env_arm9.CreateCompilerEnv('NETX50', ['arch=armv5te', 'thumb'], name='NETX50_THUMB')
env_arm9.CreateCompilerEnv('NETX10', ['arch=armv5te', 'thumb'], name='NETX10_THUMB')

# Create a build environment for the Cortex-R7 and Cortex-A9 based netX chips.
env_cortexR7 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])
env_cortexR7.CreateCompilerEnv('NETX4000', ['arch=armv7', 'thumb'], ['arch=armv7-r', 'thumb'])

# Create a build environment for the Cortex-M4 based netX chips.
env_cortexM4 = atEnv.DEFAULT.CreateEnvironment(['gcc-arm-none-eabi-4.9', 'asciidoc'])
env_cortexM4.CreateCompilerEnv('NETX90_MPW', ['arch=armv7', 'thumb'], ['arch=armv7e-m', 'thumb'])
env_cortexM4.CreateCompilerEnv('NETX90', ['arch=armv7', 'thumb'], ['arch=armv7e-m', 'thumb'])

# Create a build environment for the RISCV based netIOL.
env_riscv = atEnv.DEFAULT.CreateEnvironment(['gcc-riscv-none-embed-7.2', 'asciidoc'])
env_riscv.CreateCompilerEnv('NETIOL', ['arch=rv32im', 'abi=ilp32'], ['arch=rv32imc', 'abi=ilp32'])


# Build the platform libraries.
SConscript('platform/SConscript')


#----------------------------------------------------------------------------
#
# Build the machine interface library.
#
SConscript('plugins/romloader/machine_interface/netx/SConscript')


#----------------------------------------------------------------------------
#
# Build the test code.
#
SConscript('plugins/romloader/_test/testcode/SConscript')


#----------------------------------------------------------------------------
#
# Build the plugin update code.
#
SConscript('plugins/romloader/uart/netx/SConscript')
SConscript('plugins/romloader/usb/netx/SConscript')


#----------------------------------------------------------------------------
#
# Build the DPM monitor code.
#
#SConscript('plugins/romloader/dpm/netx/SConscript')


#----------------------------------------------------------------------------
#
# Build the DCC output code.
#
SConscript('plugins/romloader/jtag/netx/SConscript')


#----------------------------------------------------------------------------
#
# Build an archive with all firmware parts.
#
Import(
    'USBMON_NETX500',
    'USBMON_NETX56',
    'USBMON_NETX10',
    'UARTMON_NETX500',
    'UARTMON_NETX500_SYM',
    'UARTMON_NETX56',
    'UARTMON_NETX56_SYM',
    'UARTMON_NETX50',
    'UARTMON_NETX50_SYM',
    'UARTMON_NETX50_BOOTSTRAP',
    'UARTMON_NETX50_BOOTSTRAP_SYM',
    'UARTMON_NETX10',
    'UARTMON_NETX10_SYM',
    'UARTMON_BOOTSTRAP_NETX10',
    'UARTMON_BOOTSTRAP_NETX10_SYM',
    'UARTMON_NETX4000',
    'UARTMON_NETX4000_SYM',
    'DCC_NETX500',
    'DCC_NETX10_50_56',
    'DCC_NETX4000',
    'DCC_NETX90'
)

atFirmware = [
    USBMON_NETX500,
    USBMON_NETX56,
    USBMON_NETX10,
    UARTMON_NETX500,
    UARTMON_NETX500_SYM,
    UARTMON_NETX56,
    UARTMON_NETX56_SYM,
    UARTMON_NETX50,
    UARTMON_NETX50_SYM,
    UARTMON_NETX50_BOOTSTRAP,
    UARTMON_NETX50_BOOTSTRAP_SYM,
    UARTMON_NETX10,
    UARTMON_NETX10_SYM,
    UARTMON_BOOTSTRAP_NETX10,
    UARTMON_BOOTSTRAP_NETX10_SYM,
    UARTMON_NETX4000,
    UARTMON_NETX4000_SYM,
    DCC_NETX500,
    DCC_NETX10_50_56,
    DCC_NETX4000,
    DCC_NETX90
]
atEnv.DEFAULT.Tar('targets/firmware.tar.gz', atFirmware, TARFLAGS = '-c -z')

#----------------------------------------------------------------------------
#
# Build an archive with the test script and test binaries.
#
Import('bin_netx500', 'bin_netx50', 'bin_netx10', 'bin_netx56', 'bin_netx4000', 'bin_netx90_mpw', 'bin_netx90', 'bin_netiol')

tArcList = atEnv.DEFAULT.ArchiveList('zip')

tArcList.AddFiles('./', 'plugins/romloader/_test/test_romloader.lua')

tArcList.AddFiles('netx/',
    bin_netx500, 
    bin_netx50,
    bin_netx10,
    bin_netx56,
    bin_netx4000,
    bin_netx90_mpw,
    bin_netx90,
    bin_netiol
)

strGroup = 'org.muhkuh.romloader'
strModule = 'montest'

aGroup = strGroup.split('.')
strModulePath = 'targets/jonchki/repository/%s/%s/%s' % ('/'.join(aGroup), strModule, PROJECT_VERSION)
strArtifact = 'romloader-montest'
strBasePath = os.path.join(strModulePath, '%s-%s' % (strArtifact, PROJECT_VERSION))
tArtifact = atEnv.DEFAULT.Archive('%s.zip' % strBasePath, None, ARCHIVE_CONTENTS = tArcList)
tArtifactHash = atEnv.DEFAULT.Hash('%s.hash' % tArtifact[0].get_path(), tArtifact[0].get_path(), HASH_ALGORITHM='md5,sha1,sha224,sha256,sha384,sha512', HASH_TEMPLATE='${ID_UC}:${HASH}\n')

