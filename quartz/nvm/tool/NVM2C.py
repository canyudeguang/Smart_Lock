# =============================================================================
# Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.
# =============================================================================

import os
import sys
import re
from optparse import OptionParser

NVMFileName    = ""
OutputFileName = ""

def main():
   global NVMFileName
   global OutputFileName

   parser = OptionParser()

   # Setup the options.
   parser.add_option("-i", "--input",
      action="store", type="string", dest="input_file",
      help="which input NVM file to parse.")

   parser.add_option("-o", "--output",
      action="store", type="string", dest="output_file",
      help="which C file generate as output.")

   # Get the options.
   (options, args) = parser.parse_args()

   if (options.input_file):
      NVMFileName = os.path.join(os.getcwd(), options.input_file)
   else:
      parser.print_help()
      parser.error('Input NVM file not provided.')
      sys.exit(1)

   if (options.output_file):
      OutputFileName = os.path.join(os.getcwd(), options.output_file)
   else:
      parser.print_help()
      parser.error('Output C file not provided.')
      sys.exit(1)

   if not os.path.exists(os.path.dirname(OutputFileName)):
      os.makedirs(os.path.dirname(OutputFileName))

   # Parse the NVM file and write the output.
   parseNVM()

def parseNVM():
   if os.path.isfile(NVMFileName):
      nvm = open(NVMFileName, 'r')

      state = 0
      edlLength = 0

      EDLFile = open(OutputFileName, 'wb')

      # Write the value prefix, which contains the length.
      Prefix = 'unsigned char UsrEDLFileData[] = {\n   0x02, 0x'

      EDLFile.write(Prefix + '00, 0x00, 0x00')

      # Read the NVM file tags/lengths/value and write them to the output array.
      for text in nvm.readlines():
         tagNumMatch = re.match( r'TagNum\s\=\s([0-9]*)', text, re.M | re.I)
         tagLenMatch = re.match( r'TagLength\s\=\s([0-9]*)', text, re.M | re.I)
         tagValMatch = re.match( r'TagValue\s\=((\s[A-Z0-9\s][A-Z0-9\s])*)$', text.rstrip(), re.M | re.I)
         if tagNumMatch:
            # Write the tag number.
            if state == 0:
               tag = tagNumMatch.group(1)
               EDLFile.write(',\n   ' + ('0x%02x' % ((int(tag) >> 0) & 0xFF)))
               EDLFile.write(', '     + ('0x%02x' % ((int(tag) >> 8) & 0xFF)))
               edlLength += 2
               state = 1
            else:
               print 'NVM parsing error: unexpected tag.'
               sys.exit(1)
         if tagLenMatch:
            # Write the tag length.
            if state == 1:
               EDLFile.write(', ' + ('0x%02x' % ((int(tagLenMatch.group(1)) >> 0) & 0xFF)))
               EDLFile.write(', ' + ('0x%02x' % ((int(tagLenMatch.group(1)) >> 8) & 0xFF)))
               EDLFile.write(', 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00')
               edlLength += 10
               state = 2
            else:
               print 'NVM parsing error: unexpected tag length.'
               sys.exit(1)
         if tagValMatch:
            # Write the tag value.
            if state == 2:
               EDLFile.write(tagValMatch.group(1).rstrip().replace(' ', ', 0x').lower())
               edlLength += (int(len(tagValMatch.group(1))))/3
               state = 0
            else:
               print 'NVM parsing error: unexpected tag value.'
               sys.exit(1)

      # Write the array size variable.
      EDLFile.write('\n};\n\nunsigned int UsrEDLFileDataLength = sizeof(UsrEDLFileData);\n');

      # Seek to the beginning and write the size now that it has been calculated.
      EDLFile.seek(len(Prefix));
      EDLFile.write(('%02x' % (edlLength & 0xFF)) + (', 0x%02x' % ((edlLength >> 8) & 0xFF)) + (', 0x%02x' % ((edlLength >> 16) & 0xFF)))
      EDLFile.close()

      return 0
   else:
      print 'NVM parsing error: file ' + NVMFileName + ' does not exist.'
      sys.exit(1)

main()
