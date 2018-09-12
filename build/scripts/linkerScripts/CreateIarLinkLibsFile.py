#===============================================================================
# Copyright (c) 2017 Qualcomm Atheros, Inc.
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#===============================================================================

#===============================================================================
# Create a linkerlibsFile for SYSTEM modules which can be used as input
# to MakeLinkerScript. 
#
# Inputs:
#   1) Path to system libraries (.lib) and object files (.o).
#   2) Name of "iar ewp" file
#   3) RTOS type. "freertos" or "threadx" are the input
#
# Output:
#   1) Contents of linkerlibs file.
#   2) Debug output (if enabled)
#===============================================================================

import sys
import os
import logging
import subprocess
import re
import string
import glob
import xml.etree.ElementTree as ET

#
# Create the linkerlibs file, starting with libraries and then application's
# objects
#
def create_linker_libs_file():
   linkerlibs_file = open(LinkerLibsFileName, "wb")
   for lib in lib_list:
      str = "\"%s\" " % lib
      linkerlibs_file.write(str)
   linkerlibs_file.close()   

#
# generate lib list based on ewp file
#
def consume_ewp_file():
   global lib_list
   lib_list = []
   tree = ET.parse(EwpFileName)
   root = tree.getroot()

   # search 'configuration' for freertos or threadx
   for config in root.iter('configuration'):
      if config.find('name').text.upper() == RtosName.upper():
         # search option's name is 'IlinkAdditionalLibs'   
         for option in config.iter('option'):   
            if option.find('name').text == 'IlinkAdditionalLibs':
               # find all "state" as libs
               for state in option.findall('state'):
                  proj_specification = re.search(r'\$PROJ_DIR\$\\(.*)', state.text.strip())
                  # trim "$PROJ_DIR$\" if it matches
                  if proj_specification != None:         
                     lib_list.append(proj_specification.group(1))
                  else:
                     lib_list.append(state.text)
   for lib in lib_list:
      logging.debug("%s", lib)
      
#
# Parse and validate the command line
#
def process_command_line():
   global ObjPath
   global EwpFileName
   global LinkerLibsFileName
   global RtosName
   
   argv_num = len(sys.argv)
   if not (argv_num == 4):
      print "Usage: ", str(sys.argv[0]), "[ewpFile] [freertos|threadx] [linkerLibsFile]"
      sys.exit(1)
   else:
      EwpFileName = str(sys.argv[1])
      logging.debug("EwpFile: %s", EwpFileName)
      
      RtosName = str(sys.argv[2])
      logging.debug("RTOS: %s", RtosName)

      LinkerLibsFileName = str(sys.argv[3])
      logging.debug("LinkerLibsFile: %s", LinkerLibsFileName)

      
# main
if os.getenv("DEBUG") or os.getenv("VERBOSE"):
   logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)
else:
   logging.basicConfig(stream=sys.stderr, level=logging.INFO)

logging.debug("Argument List: %s", str(sys.argv))

process_command_line()
consume_ewp_file()
create_linker_libs_file()
