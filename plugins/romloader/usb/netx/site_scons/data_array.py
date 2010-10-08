# -*- coding: utf-8 -*-

from SCons.Script import *


def dataarray_action(target, source, env):
	file_target = open(target[0].get_path(), 'w')
	
	aucSourceData = source[0].get_contents()
	sizSourceData = len(aucSourceData)
	
	strArrayName = env['DATAARRAY_NAME']
	sizBytesPerLine = env['DATAARRAY_BYTES_PER_LINE']
	
	file_target.write("const unsigned char %s[%d] =\n"%(strArrayName,sizSourceData))
	file_target.write("{\n");
	for iCnt in xrange(0, sizSourceData, sizBytesPerLine):
		aucChunk = aucSourceData[iCnt:iCnt+sizBytesPerLine]
		file_target.write("\t");
		file_target.write(', '.join(['0x%02X'%ord(ucByte) for ucByte in aucChunk]))
		if( iCnt+sizBytesPerLine<sizSourceData ):
			file_target.write(',')
		file_target.write("\n")
	file_target.write("};\n");
	file_target.close()
	return 0


def dataarray_emitter(target, source, env):
	# Make the target depend on the parameter.
	Depends(target, SCons.Node.Python.Value(env['DATAARRAY_NAME']))
	Depends(target, SCons.Node.Python.Value(env['DATAARRAY_BYTES_PER_LINE']))
	
	return target, source


def dataarray_string(target, source, env):
	return 'DataArray %s' % target[0].get_path()


def ApplyToEnv(env):
	#----------------------------------------------------------------------------
	#
	# Add dataarray builder.
	#
	env['DATAARRAY_NAME'] = 'aucData'
	env['DATAARRAY_BYTES_PER_LINE'] = 16

	dataarray_act = SCons.Action.Action(dataarray_action, dataarray_string)
	dataarray_bld = Builder(action=dataarray_act, emitter=dataarray_emitter, suffix='.c', single_source=1)
	env['BUILDERS']['DataArray'] = dataarray_bld

