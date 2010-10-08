# -*- coding: utf-8 -*-

import imp
import sys
import os

from SCons.Script import *


#----------------------------------------------------------------------------
#
# Accept 'clean' target like make as an alternative to '-c'. This makes it
# much easier to work with an IDE like KDevelop.
#
if 'clean' in COMMAND_LINE_TARGETS:
	Alias('clean', '.')
	SetOption('clean', 1)


#----------------------------------------------------------------------------
#
# Display the complete command line if any command failed.
#
def display_build_status():
	from SCons.Script import GetBuildFailures
	bf_all = GetBuildFailures()
	if bf_all:
		# iterate over all failures and print the command
		for bf in bf_all:
			if bf and bf.node and bf.command:
				print 'Failed command for ' + str(bf.node) + ":\n" + ' '.join(bf.command)
		print "!!!!!!!!!!!!!!!"
		print "!!! FAILED !!!!"
		print "!!!!!!!!!!!!!!!"
	else:
		print "Build succeeded."

import atexit
atexit.register(display_build_status)


def get_tool(strToolName):
	global TOOLS
	
	tMod = None
	try:
		strPath = TOOLS[strToolName]
		strModulName = strToolName.replace('-','_').replace('.','_')
		fp,pathname,description = imp.find_module(strModulName, [strPath])
		try:
			tMod = imp.load_module(strModulName, fp, pathname, description)
		finally:
			# Since we may exit via an exception, close fp explicitly.
			if fp:
				fp.close()
	except KeyError:
		pass
	
	if tMod==None:
		raise Exception(strToolName, 'The requested tool is not part of the configuration. Add it to setup.xml and rerun setup.py')
	
	return tMod


def set_build_path(env, build_path, source_path, sources):
	env.VariantDir(build_path, source_path, duplicate=0)
	return [src.replace(source_path, build_path) for src in sources]

