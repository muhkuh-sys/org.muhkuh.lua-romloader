##############################################################################
##   Copyright (C) 2008 by Christoph Thelen                                 ##
##   doc_bacardi@users.sourceforge.net                                      ##
##                                                                          ##
##   This program is free software; you can redistribute it and/or modify   ##
##   it under the terms of the GNU General Public License as published by   ##
##   the Free Software Foundation; either version 2 of the License, or      ##
##   (at your option) any later version.                                    ##
##                                                                          ##
##   This program is distributed in the hope that it will be useful,        ##
##   but WITHOUT ANY WARRANTY; without even the implied warranty of         ##
##   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          ##
##   GNU General Public License for more details.                           ##
##                                                                          ##
##   You should have received a copy of the GNU General Public License      ##
##   along with this program; if not, write to the                          ##
##   Free Software Foundation, Inc.,                                        ##
##   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.              ##
##############################################################################

#
#  Please Note:
#
#  All testpatterns for the "... pattern check" tests are taken from the
#  mhash 0.9.9 testsuite.
#


import mhash

import binascii


# define some helper functions for the pattern test



def hexdump(strData, iBytesPerRow):
	if iBytesPerRow==None:
		iBytesPerRow = 16

	iByteCnt = 0
	for iCnt in range(0, strData.len()):
		if iByteCnt==0:
			strDump = "%08X :" % iCnt-1
		
		strByte = " %02X" % strData[iCnt]
		strDump = strDump + strByte
		iByteCnt = iByteCnt + 1
		if iByteCnt==iBytesPerRow:
			iByteCnt = 0
			print strDump
	
	if iByteCnt!=0:
		print strDump


def hash_show_error(input, hash, expected):
	print "the hash does not match the expected hash!"
	print "input data:"
	hexdump(input)
	print "result:"
	hexdump(hash)
	print "expected result:"
	hexdump(expected)


#def check_hash(hash_id, patterns):
#	# expect success
#	fResult = true
#
#	# create a state
#	mh = _mhash.mhash_state()
#
#	# loop over all testpatterns
#	for (i,p) in values(patterns):
#		print "Testing pattern %s" % i
#		# convert the hexdump string to binary data
#		expected_hash = ""
#		#for i=1,p.hash:len(),2 do
#			#num = tonumber(p.hash:sub(i,i+1), 16)
			#if not num then
				#print("Failed to convert expected hash value to binary: " .. p.hash)
				#fResult = false
				#break
			#end
			#expected_hash = expected_hash .. string.char(num)
		#end

		#if not fResult then
			#break
		#else
			#mh:init(hash_id)
			#mh:hash(p.data)
			#hash = mh:hash_end()
			#if hash~=expected_hash then
				#hash_show_error(p.data, hash, expected_hash)
				#fResult = false
				#break
			#end
		#end
	#end

	#return fResult


##############################################################################

print "Binding and Version check"

# check for mhash namespace
strMhashVersion = mhash.get_version()
print "Found mhash version '%s'" % strMhashVersion

# print some info
hash_cnt = mhash.count()
print "this version of mhash knows %d hash types." % hash_cnt
for t in range(0, hash_cnt+1):
	name = mhash.get_hash_name(t)
	if name:
		print "\t%d: %s (%d)" % (t, name, mhash.get_block_size(t))


##############################################################################


print "Simple CRC32B check"

# Expected hash for the string.
expected_hash = binascii.unhexlify("33F0C468")

# Create a mhash_state.
mh = mhash.mhash_state()

# Calculate the hash for a string.
data = "0123456789abcdef"
mh.init(mhash.MHASH_CRC32B)

mh.hash(data)
hash = mh.hash_end()

# Delete the state.
mh = None

# Check the result.
if hash!=expected_hash:
	print "the hash does not match the expected hash!"
	print "input data:"
	hexdump(data)
	print "result:"
	hexdump(hash)
	print "expected result:"
	hexdump(expected_hash)
	
	error("test failed!")

