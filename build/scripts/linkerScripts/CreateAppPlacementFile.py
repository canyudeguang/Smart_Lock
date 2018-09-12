#===============================================================================
# Copyright (c) 2015 Qualcomm Atheros, Inc.
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
#===============================================================================

#===============================================================================
# Create a Application PlacementFile for Applicatio modules which can be used as input
# to MakeLinkerScript. This runs as part of the App build; it is
#
# Inputs:SysPlacementFile AppConfigFile AppPlacementFile
#   1) Name of "sys.placement" file, specified on the command line.
#      The sys.placement file is created by CreateSysPlacementFile.py. It includes
#      all sys placement info
#   2) Name of "app.config" file, specified on the command line.
#      The app.config file is defined at application. It includes all 
#      application placement info
#   3) Name of "app.placement" file, specified on the command line.
#      This is the output file name which store application placement info.
# Output:
#   1) Contents of application placement file. The file is used to generate ld file.
#   2) Debug output (if enabled)
#===============================================================================

import sys
import os
import logging
import subprocess
import re
import string
import glob
from collections import defaultdict

#
# generate application placement file 
#
def create_application_placement_file():
   global SysPlacementFileName
   global CustPlacementFileName   
   global AppConfigFileName
   global AppPlacementFileName

   logging.debug("Create Application Placement File")
   app_placement_file = open(AppPlacementFileName, 'w')
   
   sys_placement_file = open(SysPlacementFileName, 'r')
   lineno = 0
   # copy sys placement file to application placement file
   for line in sys_placement_file:
      lineno = lineno + 1
      logging.debug("Sys placement file line %d: %s", lineno, line)
      if len(line.split()) < 3:
         logging.debug("In SysPlacementFile skipping %s", line)
         continue
      app_placement_file.write(line)
   
   # close sys placement file 
   sys_placement_file.close()

   cust_placement_file = open(CustPlacementFileName, 'r')
   lineno = 0
   # copy cust placement file to application placement file
   for line in cust_placement_file:
      lineno = lineno + 1
      logging.debug("Cust placement file line %d: %s", lineno, line)
      if len(line.split()) < 3:
         logging.debug("In CustPlacementFile skipping %s", line)
         continue
      app_placement_file.write(line)
   
   # close sys placement file 
   cust_placement_file.close()

   lineno = 0   
   # append application config to application placement file 
   app_config_file = open(AppConfigFileName, 'r')
   for line in app_config_file:
      lineno = lineno + 1
      logging.debug("app config file line %d: %s", lineno, line)
      if len(line.split()) < 3:
         logging.debug("In AppConfigFile skipping %s", line)
         continue      
      app_placement_file.write(line)

   # close app config file 
   app_config_file.close()
   # close app placement file 
   app_placement_file.close()

      
#
# Parse and validate the command line
#
def process_command_line():
   global SysPlacementFileName
   global CustPlacementFileName
   global AppConfigFileName
   global AppPlacementFileName
   
   argv_num = len(sys.argv)
   if not (argv_num == 5 ):
      print "Usage: ", str(sys.argv[0]), "SysPlacementFile CustPlacementFile AppConfigFile AppPlacementFile"
      sys.exit(1)

   SysPlacementFileName = str(sys.argv[1])
   logging.debug("SysPlacementFile: %s", SysPlacementFileName)

   if not os.path.exists(SysPlacementFileName):
      logging.debug("SysPlacementFile doesn't exist....")
      sys.exit(1)

   CustPlacementFileName = str(sys.argv[2])
   if not os.path.exists(CustPlacementFileName):
      logging.debug("CustPlacementFile doesn't exist....")
      sys.exit(1)
      
   AppConfigFileName = str(sys.argv[3])
   logging.debug("AppConfigFile: %s", AppConfigFileName)

   if not os.path.exists(AppConfigFileName):
      logging.debug("AppConfigFile doesn't exist....")
      sys.exit(1)
      
   AppPlacementFileName = str(sys.argv[4])      
   logging.debug("AppPlacementFile: %s", AppPlacementFileName)
   
# main
if os.getenv("DEBUG") or os.getenv("VERBOSE"):
   logging.basicConfig(stream=sys.stderr, level=logging.DEBUG)
else:
   logging.basicConfig(stream=sys.stderr, level=logging.INFO)


logging.debug("Argument List: %s", str(sys.argv))
process_command_line()
create_application_placement_file()
