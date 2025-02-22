#!/usr/bin/python
#
# Copyright (c) 2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

# qca402x_dictionary.py
import os
import sys

# This class is used to build dictionary objects from given files.
# APPS=index0, CNSS=index1, WLAN=index2

class qca402x_dictionary:
    def __init__(self):

        self.dictList = [None]*3
        self.dictFiles = ['../../../bin/cortex-m4/diag_msg_ARNFRI.strdb',
                          '../../../bin/cortex-m0/threadx/diag_msg_BRNTRI.strdb',
                          '../../../bin/wlan/wlan_fw_dictionary.msc']

    # Update the dictonary files by the ones given from CLI
    def update(self,key,val):
        self.dictFiles[key] = val

    # Create dictionary objects by parsing files
    def create_dictionary(self):
        if self.create_apps_cnss_proc_dictionary(0) != 0:
            return -1

        if self.create_apps_cnss_proc_dictionary(1) != 0:
            return -1

        if self.create_wlan_fw_dictionary(2) != 0:
            return -1

        return 0

    # Create APPS and CNSS dictionaries
    def create_apps_cnss_proc_dictionary(self,p_id):
        if (p_id != 0 and p_id != 1):
            return -1

        if os.path.exists(self.dictFiles[p_id]):
            with open(self.dictFiles[p_id], 'r') as infile:
                lines = infile.readlines()
                self.dictList[p_id] = {}

                i = 0
                while True:
                    if (i < len(lines)):
                        debug_string = ""
                        fields = lines[i].split(':')
                        if (len(fields) > 0):
                            try:
                                key = int(fields[0],10)
                                if (len(fields) >= 2) and ord(fields[1][0]) >= 32:
                                    debug_string = ':'.join(fields[j] for j in range(1,len(fields),1))
                                    self.dictList[p_id][key] = debug_string

                                else:
                                    fields_temp = lines[i+1].split(':')
                                    if (len(fields_temp) == 0):
                                        debug_string = lines[i+1]
                                        self.dictList[p_id][key] = debug_string
                                        i = i + 1

                                    else:
                                        try:
                                            another_key = int(fields_temp[0],10)
                                            self.dictList[p_id][key] = debug_string
                                        except:
                                            debug_string = ':'.join(fields_temp[k] for k in range(0,len(fields_temp),1))
                                            self.dictList[p_id][key] = debug_string
                                            i = i + 1
                            except:
                                pass

                        i = i + 1

                    else:
                        break
            return 0

        else:
            print "%s not found.\n" % self.dictFiles[p_id]
            return -1


    def create_wlan_fw_dictionary(self,p_id):
        if os.path.exists(self.dictFiles[p_id]):
            with open(self.dictFiles[p_id], 'r') as infile:
                lines = infile.readlines()
                self.dictList[p_id] = {}
                for line in lines:
                    fields = line.split(',')
                    if (len(fields) >= 3):
                        key = int(fields[0][1:],10)
                        debug_string = ''.join(fields[i] for i in range(2,len(fields),1))
                        self.dictList[p_id][key] = debug_string
                return 0
        else:
            print "%s not found.\n" % self.dictFiles[p_id]
        return -1

    def find_message(self,messageID,p_id):
        try:
            return self.dictList[p_id][messageID]
        except:
            return ''
