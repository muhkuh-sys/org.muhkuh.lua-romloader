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


def check_hash(hash_id, patterns):
	# expect success
	fResult = True

	# create a state
	mh = mhash.mhash_state()

	# loop over all testpatterns
	for p in patterns:
		print "\tTesting pattern '%s'" % p['data']
		# convert the hexdump string to binary data
		expected_hash = binascii.unhexlify(p['hash'])
		mh.init(hash_id)
		mh.hash(p['data'])
		hash = mh.hash_end()
		if hash!=expected_hash:
			hash_show_error(p.data, hash, expected_hash)
			fResult = False
			break
	
	return fResult


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

print "Test ok!"


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

print "Test ok!"


##############################################################################


print "Simple CRC32B check with size"

# Expected hash for the string.
expected_hash = binascii.unhexlify('33F0C468')

# Create a mhash_state.
mh = mhash.mhash_state()

# Calculate the hash for a string,
# but do not include the last 4 characters.
data = "0123456789abcdef****"
mh.init(mhash.MHASH_CRC32B)
mh.hash(data, 16)
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

print "Test ok!"


##############################################################################


print "Simple CRC32B check with size and offset"

# Expected hash for the string.
expected_hash = binascii.unhexlify('33F0C468')

# Create a mhash_state.
mh = mhash.mhash_state()

# Calculate the hash for a string,
# but do not include the first and last 4 characters.
data = "****0123456789abcdef****"
mh.init(mhash.MHASH_CRC32B)
mh.hash(data, 16, 4)
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

print "Test ok!"


##############################################################################


print "CRC32 pattern check"

hash_check = [
	{
		'data': "checksum",
		'hash': "7FBEB02E"
	}
]

if check_hash(mhash.MHASH_CRC32, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "CRC32B pattern check"

hash_check = [
	{
		'data': "checksum",
		'hash': "9ADF6FDE"
	}
]

if check_hash(mhash.MHASH_CRC32B, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "MD2 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "8350E5A3E24C153DF2275C9F80692773"
	},
	{
		'data': "a",
		'hash': "32EC01EC4A6DAC72C0AB96FB34C0B5D1"
	},
	{
		'data': "abc",
		'hash': "DA853B0D3F88D99B30283A69E6DED6BB"
	},
	{
		'data': "message digest",
		'hash': "AB4F496BFB2A530B219FF33031FE06B0"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "4E8DDFF3650292AB5A4108C3AA47940B"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "DA33DEF2A42DF13975352846C30338CD"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "D5976F79D83D3A0DC9806C3C66F3EFD8"
	}
]

if check_hash(mhash.MHASH_MD2, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "MD4 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "31D6CFE0D16AE931B73C59D7E0C089C0"
	},
	{
		'data': "a",
		'hash': "BDE52CB31DE33E46245E05FBDBD6FB24"
	},
	{
		'data': "abc",
		'hash': "A448017AAF21D8525FC10AE87AA6729D"
	},
	{
		'data': "message digest",
		'hash': "D9130A8164549FE818874806E1C7014B"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "D79E1C308AA5BBCDEEA8ED63DF412DA9"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "043F8582F241DB351CE627E153E7F0E4"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "E33B4DDC9C38F2199C3E7B164FCC0536"
	}
]

if check_hash(mhash.MHASH_MD4, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "MD5 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "D41D8CD98F00B204E9800998ECF8427E"
	},
	{
		'data': "a",
		'hash': "0CC175B9C0F1B6A831C399E269772661"
	},
	{
		'data': "abc",
		'hash': "900150983CD24FB0D6963F7D28E17F72"
	},
	{
		'data': "message digest",
		'hash': "F96B697D7CB7938D525A2F31AAF161D0"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "C3FCD3D76192E4007DFB496CCA67E13B"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "57EDF4A22BE3C955AC49DA2E2107B67A"
	}
]

if check_hash(mhash.MHASH_MD5, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "HAVAL256 pattern check"

hash_check = [
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "72FAD4BDE1DA8C8332FB60561A780E7F504F21547B98686824FC33FC796AFA76"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "899397D96489281E9E76D5E65ABAB751F312E06C06C07C9C1D42ABD31BB6A404"
	}
]

if check_hash(mhash.MHASH_HAVAL256, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "HAVAL224 pattern check"

hash_check = [
	{
		'data': "0123456789",
		'hash': "EE345C97A58190BF0F38BF7CE890231AA5FCF9862BF8E7BEBBF76789"
	}
]

if check_hash(mhash.MHASH_HAVAL224, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "HAVAL192 pattern check"

hash_check = [
	{
		'data': "HAVAL",
		'hash': "8DA26DDAB4317B392B22B638998FE65B0FBE4610D345CF89"
	}
]

if check_hash(mhash.MHASH_HAVAL192, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "HAVAL160 pattern check"

hash_check = [
	{
		'data': "a",
		'hash': "4DA08F514A7275DBC4CECE4A347385983983A830"
	}
]

if check_hash(mhash.MHASH_HAVAL160, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "HAVAL128 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "C68F39913F901F3DDF44C707357A7D70"
	}
]

if check_hash(mhash.MHASH_HAVAL128, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "RIPEMD128 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "CDF26213A150DC3ECB610F18F6B38B46"
	},
	{
		'data': "a",
		'hash': "86BE7AFA339D0FC7CFC785E72F578D33"
	},
	{
		'data': "abc",
		'hash': "C14A12199C66E4BA84636B0F69144C77"
	},
	{
		'data': "message digest",
		'hash': "9E327B3D6E523062AFC1132D7DF9D1B8"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "FD2AA607F71DC8F510714922B371834E"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "A1AA0689D0FAFA2DDC22E88B49133A06"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "D1E959EB179C911FAEA4624C60C5C702"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "3F45EF194732C2DBB2C4A2C769795FA3"
	}
]

if check_hash(mhash.MHASH_RIPEMD128, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "RIPEMD160 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "9C1185A5C5E9FC54612808977EE8F548B2258D31"
	},
	{
		'data': "a",
		'hash': "0BDC9D2D256B3EE9DAAE347BE6F4DC835A467FFE"
	},
	{
		'data': "abc",
		'hash': "8EB208F7E05D987A9B044A8E98C6B087F15A0BFC"
	},
	{
		'data': "message digest",
		'hash': "5D0689EF49D2FAE572B881B123A85FFA21595F36"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "F71C27109C692C1B56BBDCEB5B9D2865B3708DBC"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "12A053384A9C0C88E405A06C27DCF49ADA62EB2B"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "B0E20B6E3116640286ED3A87A5713079B21F5189"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "9B752E45573D4B39F4DBD3323CAB82BF63326BFB"
	}
]

if check_hash(mhash.MHASH_RIPEMD160, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "RIPEMD256 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "02BA4C4E5F8ECD1877FC52D64D30E37A2D9774FB1E5D026380AE0168E3C5522D"
	},
	{
		'data': "a",
		'hash': "F9333E45D857F5D90A91BAB70A1EBA0CFB1BE4B0783C9ACFCD883A9134692925"
	},
	{
		'data': "abc",
		'hash': "AFBD6E228B9D8CBBCEF5CA2D03E6DBA10AC0BC7DCBE4680E1E42D2E975459B65"
	},
	{
		'data': "message digest",
		'hash': "87E971759A1CE47A514D5C914C392C9018C7C46BC14465554AFCDF54A5070C0E"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "649D3034751EA216776BF9A18ACC81BC7896118A5197968782DD1FD97D8D5133"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "3843045583AAC6C8C8D9128573E7A9809AFB2A0F34CCC36EA9E72F16F6368E3F"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "5740A408AC16B720B84424AE931CBB1FE363D1D0BF4017F1A89F7EA6DE77A0B8"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "06FDCC7A409548AAF91368C06A6275B553E3F099BF0EA4EDFD6778DF89A890DD"
	}
]

if check_hash(mhash.MHASH_RIPEMD256, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "RIPEMD320 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "22D65D5661536CDC75C1FDF5C6DE7B41B9F27325EBC61E8557177D705A0EC880151C3A32A00899B8"
	},
	{
		'data': "a",
		'hash': "CE78850638F92658A5A585097579926DDA667A5716562CFCF6FBE77F63542F99B04705D6970DFF5D"
	},
	{
		'data': "abc",
		'hash': "DE4C01B3054F8930A79D09AE738E92301E5A17085BEFFDC1B8D116713E74F82FA942D64CDBC4682D"
	},
	{
		'data': "message digest",
		'hash': "3A8E28502ED45D422F68844F9DD316E7B98533FA3F2A91D29F84D425C88D6B4EFF727DF66A7C0197"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "CABDB1810B92470A2093AA6BCE05952C28348CF43FF60841975166BB40ED234004B8824463E6B009"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "D034A7950CF722021BA4B84DF769A5DE2060E259DF4C9BB4A4268C0E935BBC7470A969C9D072A1AC"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "ED544940C86D67F250D232C30B7B3E5770E0C60C8CB9A4CAFE3B11388AF9920E1B99230B843C86A4"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "557888AF5F6D8ED62AB66945C6D2A0A47ECD5341E915EB8FEA1D0524955F825DC717E4A008AB2D42"
	}
]

if check_hash(mhash.MHASH_RIPEMD320, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "TIGER192 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "24F0130C63AC933216166E76B1BB925FF373DE2D49584E7A"
	},
	{
		'data': "abc",
		'hash': "F258C1E88414AB2A527AB541FFC5B8BF935F7B951C132951"
	},
	{
		'data': "Tiger",
		'hash': "9F00F599072300DD276ABB38C8EB6DEC37790C116F9D2BDF"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-",
		'hash': "87FB2A9083851CF7470D2CF810E6DF9EB586445034A5A386"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZ=abcdefghijklmnopqrstuvwxyz+0123456789",
		'hash': "467DB80863EBCE488DF1CD1261655DE957896565975F9197"
	},
	{
		'data': "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham",
		'hash': "0C410A042968868A1671DA5A3FD29A725EC1E457D3CDB303"
	},
	{
		'data': "Tiger - A Fast New Hash Function, by Ross Anderson and Eli Biham, proceedings of Fast Software Encryption 3, Cambridge.",
		'hash': "EBF591D5AFA655CE7F22894FF87F54AC89C811B6B0DA3193"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-",
		'hash': "00B83EB4E53440C576AC6AAEE0A7485825FD15E70A59FFE4"
	}
]

if check_hash(mhash.MHASH_TIGER192, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "TIGER160 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "24F0130C63AC933216166E76B1BB925FF373DE2D"
	},
	{
		'data': "abc",
		'hash': "F258C1E88414AB2A527AB541FFC5B8BF935F7B95"
	},
	{
		'data': "Tiger",
		'hash': "9F00F599072300DD276ABB38C8EB6DEC37790C11"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-",
		'hash': "87FB2A9083851CF7470D2CF810E6DF9EB5864450"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZ=abcdefghijklmnopqrstuvwxyz+0123456789",
		'hash': "467DB80863EBCE488DF1CD1261655DE957896565"
	}
]

if check_hash(mhash.MHASH_TIGER160, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "TIGER128 pattern check"

hash_check = [
	{
		'data': "",
		'hash': "24F0130C63AC933216166E76B1BB925F"
	},
	{
		'data': "abc",
		'hash': "F258C1E88414AB2A527AB541FFC5B8BF"
	},
	{
		'data': "Tiger",
		'hash': "9F00F599072300DD276ABB38C8EB6DEC"
	}
]

if check_hash(mhash.MHASH_TIGER128, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "GOST pattern check"

hash_check = [
	{
		'data': "This is message, length=32 bytes",
		'hash': "B1C466D37519B82E8319819FF32595E047A28CB6F83EFF1C6916A815A637FFFA"
	},
	{
		'data': "Suppose the original message has length = 50 bytes",
		'hash': "471ABA57A60A770D3A76130635C1FBEA4EF14DE51F78B4AE57DD893B62F55208"
	}
]

if check_hash(mhash.MHASH_GOST, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SHA1 pattern check"

hash_check = [
	{
		'data': "abc",
		'hash': "A9993E364706816ABA3E25717850C26C9CD0D89D"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "84983E441C3BD26EBAAE4AA1F95129E5E54670F1"
	}
]

if check_hash(mhash.MHASH_SHA1, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SHA256 pattern check"

hash_check = [
	{
		'data': "abc",
		'hash': "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "248D6A61D20638B8E5C026930C3E6039A33CE45964FF2167F6ECEDD419DB06C1"
	},
	{
		'data': "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		'hash': "CF5B16A778AF8380036CE59E7B0492370B249B11E8F07A51AFAC45037AFEE9D1"
	}
]

if check_hash(mhash.MHASH_SHA256, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SHA224 pattern check"


hash_check = [
	{
		'data': "abc",
		'hash': "23097D223405D8228642A477BDA255B32AADBCE4BDA0B3F7E36C9DA7"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
		'hash': "75388B16512776CC5DBA5DA1FD890150B0C6455CB4F58B1952522525"
	}
]

if check_hash(mhash.MHASH_SHA224, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SHA512 pattern check"

hash_check = [
	{
		'data': "abc",
		'hash': "DDAF35A193617ABACC417349AE20413112E6FA4E89A97EA20A9EEEE64B55D39A2192992A274FC1A836BA3C23A3FEEBBD454D4423643CE80E2A9AC94FA54CA49F"
	},
	{
		'data': "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		'hash': "8E959B75DAE313DA8CF4F72814FC143F8F7779C6EB9F7FA17299AEADB6889018501D289E4900F7E4331B99DEC4B5433AC7D329EEB6DD26545E96E55B874BE909"
	}
]

if check_hash(mhash.MHASH_SHA512, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SHA384 pattern check"

hash_check = [
	{
		'data': "abc",
		'hash': "CB00753F45A35E8BB5A03D699AC65007272C32AB0EDED1631A8B605A43FF5BED8086072BA1E7CC2358BAECA134C825A7"
	},
	{
		'data': "abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
		'hash': "09330C33F71147E83D192FC782CD1B4753111B173B3B05D22FA08086E3B0F712FCC7C71A557E2DB966C3E9FA91746039"
	}
]

if check_hash(mhash.MHASH_SHA384, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "WHIRLPOOL pattern check"

hash_check = [
	{
		'data': "",
		'hash': "19FA61D75522A4669B44E39C1D2E1726C530232130D407F89AFEE0964997F7A73E83BE698B288FEBCF88E3E03C4F0757EA8964E59B63D93708B138CC42A66EB3"
	},
	{
		'data': "a",
		'hash': "8ACA2602792AEC6F11A67206531FB7D7F0DFF59413145E6973C45001D0087B42D11BC645413AEFF63A42391A39145A591A92200D560195E53B478584FDAE231A"
	},
	{
		'data': "abc",
		'hash': "4E2448A4C6F486BB16B6562C73B4020BF3043E3A731BCE721AE1B303D97E6D4C7181EEBDB6C57E277D0E34957114CBD6C797FC9D95D8B582D225292076D4EEF5"
	},
	{
		'data': "message digest",
		'hash': "378C84A4126E2DC6E56DCC7458377AAC838D00032230F53CE1F5700C0FFB4D3B8421557659EF55C106B4B52AC5A4AAA692ED920052838F3362E86DBD37A8903E"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "F1D754662636FFE92C82EBB9212A484A8D38631EAD4238F5442EE13B8054E41B08BF2A9251C30B6A0B8AAE86177AB4A6F68F673E7207865D5D9819A3DBA4EB3B"
	},
	{
		'data': "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
		'hash': "DC37E008CF9EE69BF11F00ED9ABA26901DD7C28CDEC066CC6AF42E40F82F3A1E08EBA26629129D8FB7CB57211B9281A65517CC879D7B962142C65F5A7AF01467"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "466EF18BABB0154D25B9D38A6414F5C08784372BCCB204D6549C4AFADB6014294D5BD8DF2A6C44E538CD047B2681A51A2C60481E88C5A20B2C2A80CF3A9A083B"
	},
	{
		'data': "abcdbcdecdefdefgefghfghighijhijk",
		'hash': "2A987EA40F917061F5D6F0A0E4644F488A7A5A52DEEE656207C562F988E95C6916BDC8031BC5BE1B7B947639FE050B56939BAAA0ADFF9AE6745B7B181C3BE3FD"
	}
]

if check_hash(mhash.MHASH_WHIRLPOOL, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SNEFRU128 pattern check"

hash_check = [
	{
		'data': "\n",
		'hash': "D9FCB3171C097FBBA8C8F12AA0906BAD"
	},
	{
		'data': "1\n",
		'hash': "44EC420CE99C1F62FEB66C53C24AE453"
	},
	{
		'data': "12\n",
		'hash': "7182051AA852EF6FBA4B6C9C9B79B317"
	},
	{
		'data': "123\n",
		'hash': "BC3A50AF82BF56D6A64732BC7B050A93"
	},
	{
		'data': "1234\n",
		'hash': "C5B8A04985A8EADFB4331A8988752B77"
	},
	{
		'data': "12345\n",
		'hash': "D559A2B62F6F44111324F85208723707"
	},
	{
		'data': "123456\n",
		'hash': "6CFB5E8F1DA02BD167B01E4816686C30"
	},
	{
		'data': "1234567\n",
		'hash': "29AA48325F275A8A7A01BA1543C54BA5"
	},
	{
		'data': "12345678\n",
		'hash': "BE862A6B68B7DF887EBE00319CBC4A47"
	},
	{
		'data': "123456789\n",
		'hash': "6103721CCD8AD565D68E90B0F8906163"
	},
	{
		'data': "abc",
		'hash': "553D0648928299A0F22A275A02C83B10"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "7840148A66B91C219C36F127A0929606"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "D9204ED80BB8430C0B9C244FE485814A"
	},
	{
		'data': "Test message for buffer workflow test(47 bytes)",
		'hash': "DD0D1AB288C3C36671044F41C5077AD6"
	},
	{
		'data': "Test message for buffer workflow test(48 bytes).",
		'hash': "E7054F05BD72D7E86A052153A17C741D"
	},
	{
		'data': "Test message for buffer workflow test(49 bytes)..",
		'hash': "9B34204833422DF13C83E10A0C6D080A"
	}
]

if check_hash(mhash.MHASH_SNEFRU128, hash_check):
	print "Ok!"
else:
	error("Test failed!")



##############################################################################


print "SNEFRU256 pattern check"

hash_check = [
	{
		'data': "\n",
		'hash': "2E02687F0D45D5B9B50CB68C3F33E6843D618A1ACA2D06893D3EB4E3026B5732"
	},
	{
		'data': "1\n",
		'hash': "BFEA4A05A2A2EF15C736D114598A20B9D9BD4D66B661E6B05ECF6A7737BDC58C"
	},
	{
		'data': "12\n",
		'hash': "AC677D69761ADE3F189C7AEF106D5FE7392D324E19CC76D5DB4A2C05F2CC2CC5"
	},
	{
		'data': "123\n",
		'hash': "061C76AA1DB4A22C0E42945E26C48499B5400162E08C640BE05D3C007C44793D"
	},
	{
		'data': "1234\n",
		'hash': "1E87FE1D9C927E9E24BE85E3CC73359873541640A6261793CE5A974953113F5E"
	},
	{
		'data': "12345\n",
		'hash': "1B59927D85A9349A87796620FE2FF401A06A7BA48794498EBAB978EFC3A68912"
	},
	{
		'data': "123456\n",
		'hash': "28E9D9BC35032B68FAEDA88101ECB2524317E9DA111B0E3E7094107212D9CF72"
	},
	{
		'data': "1234567\n",
		'hash': "F7FFF4EE74FD1B8D6B3267F84E47E007F029D13B8AF7E37E34D13B469B8F248F"
	},
	{
		'data': "12345678\n",
		'hash': "EE7D64B0102B2205E98926613B200185559D08BE6AD787DA717C968744E11AF3"
	},
	{
		'data': "123456789\n",
		'hash': "4CA72639E40E9AB9C0C3F523C4449B3911632D374C124D7702192EC2E4E0B7A3"
	},
	{
		'data': "abc",
		'hash': "7D033205647A2AF3DC8339F6CB25643C33EBC622D32979C4B612B02C4903031B"
	},
	{
		'data': "abcdefghijklmnopqrstuvwxyz",
		'hash': "9304BB2F876D9C4F54546CF7EC59E0A006BEAD745F08C642F25A7C808E0BF86E"
	},
	{
		'data': "12345678901234567890123456789012345678901234567890123456789012345678901234567890",
		'hash': "D5FCE38A152A2D9B83AB44C29306EE45AB0AED0E38C957EC431DAB6ED6BB71B8"
	}
]

if check_hash(mhash.MHASH_SNEFRU256, hash_check):
	print "Ok!"
else:
	error("Test failed!")



