import argparse
import sys
import xml.etree.ElementTree

# Parse the command line arguments.
tParser = argparse.ArgumentParser(description='Extract the version number from a MBS setup file.')
tParser.add_argument('infile', nargs='?', type=argparse.FileType('rb'), default=sys.stdin,
                     help='Read the MBS setup file from FILENAME', metavar='FILENAME')
aOptions = tParser.parse_args()

# Read the complete file to a string.
strSetupFile = aOptions.infile.read()
aOptions.infile.close()

t = xml.etree.ElementTree.fromstring(strSetupFile)
v = t.find('project_version')
print '%d.%d.%d' % (long(v.find('major').text), long(v.find('minor').text), long(v.find('micro').text))
