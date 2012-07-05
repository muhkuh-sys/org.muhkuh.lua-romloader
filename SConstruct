# -*- coding: utf-8 -*-
#-------------------------------------------------------------------------#
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
#-------------------------------------------------------------------------#


#----------------------------------------------------------------------------
#
# Set up the Muhkuh Build System.
#
SConscript('mbs/SConscript')
Import('env_default')


#----------------------------------------------------------------------------
#
# Create the compiler environments.
#
env_netx500_default = env_default.CreateCompilerEnv('500', ['cpu=arm926ej-s'])
env_netx56_default = env_default.CreateCompilerEnv('56', ['cpu=arm966e-s'])
env_netx50_default = env_default.CreateCompilerEnv('50', ['cpu=arm966e-s'])
env_netx10_default = env_default.CreateCompilerEnv('10', ['cpu=arm966e-s'])
Export('env_netx500_default', 'env_netx56_default', 'env_netx50_default', 'env_netx10_default')


#----------------------------------------------------------------------------
#
# Build the platform libraries.
#
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

