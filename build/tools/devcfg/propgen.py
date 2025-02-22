#==============================================================================
#
# Propgen.py
#
# GENERAL DESCRIPTION
#    Generate and Pack Properties expressed in xml in a binary format
#
#  Copyright (c) 2010-2014 Qualcomm Technologies, Inc . 
#  All Rights Reserved.
#  Qualcomm Confidential and Proprietary
#==============================================================================
#==============================================================================
#                              EDIT HISTORY
#
#  $Header: //components/rel/core.ioe/1.0/dal/tools/propgen.py#2 $
#  $DateTime: 2017/01/03 09:46:02 $
#  $Author: pwbldsvc $
#
# when       who     what, where, why
# --------   ---     ----------------------------------------------------------
# 01/13/15   aa     Added support for storing indicator for prop id/name
# 03/03/15   aa     Fixed user defined struct array parsing from xml
# 03/03/15   aa     Removed struct section, not used
# 02/11/15   sg     Add support for enums as propid and uint32 sequences
# 10/27/14   aa     DALModDir_loader.c is now #included in DALConfig_loader.c
# 08/27/14   aa     Fix warning
# 07/29/14   aa     Added support for string based devices
# 07/29/14   aa     Updated Indentations.
# 07/29/14   aa     Add history
#
#==============================================================================
import xml.dom.minidom
import sys
import re
import os
from struct import *
from types import *
from optparse import OptionParser

#------------------------------------------------------------------------------
# Globals
#------------------------------------------------------------------------------
global NameSection, StringSection, ByteSection,  SegmentOffset, UintSection, DevSection
global DeviceID
UintSection = []
ByteSection = []
StringSection =[]
NameSection = []
DevSection = []
DeviceID = []
SegmentOffset = []
StructPtrs = []
StructIncFiles = []
PropNames = []
StringDeviceIdx = 0
StringDeviceNames = {}
ConfigStructList = {}
HashCollisionOffset = {}
CollisionDictionary = {}
StructDef = {}
device_offset = 0

#------------------------------------------------------------------------------
# AlignSectionAddr : Make Section word aligned
#------------------------------------------------------------------------------
def AlignSectionAddr():
    NameSectionLen = len(NameSection)
    StringSectionLen = len(StringSection)
    ByteSectionLen = len(ByteSection)

    if NameSectionLen%4 != 0:
        for tmpBytes in range(0,4-(NameSectionLen%4)):
            NameSection.append(0)

    if StringSectionLen%4 != 0:
        for tmpBytes in range(0,4-(StringSectionLen%4)):
            StringSection.append(0)

    if ByteSectionLen%4 != 0:
        for tmpBytes in range(0,4-(ByteSectionLen%4)):
            ByteSection.append(0)
    return

#------------------------------------------------------------------------------
# ComputeCollisionIndex : Return index where collision info for a hash is stored 
#------------------------------------------------------------------------------
def ComputeCollisionIndex(Myhash,device):
   if len(HashCollisionOffset[Myhash])==1:
      return "NULL"
   else:
      CollisionDictionary[device] = HashCollisionOffset[Myhash]
      return device
      #StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx,DriverName]

#------------------------------------------------------------------------------
# PrintStringDeviceNameStruct : Populate Meta deta for string based devices
#------------------------------------------------------------------------------
def PrintStringDeviceNameStruct(putnull, ConfigType):
   #{/dev/buses/i2c1, 1234, DALI2C_DriverInfo,  {2,5,8 },
   for device in StringDeviceNames:
      # ignore null Driver names     
      if putnull ==0 and (StringDeviceNames[device][0]=="NULL" or StringDeviceNames[device][0]=="null"):
         ConfigStructList[StringDeviceNames[device][3]] = "{0,0,0,0,0,0}"
         continue                  
      ConfigStructList[StringDeviceNames[device][3]] = "{\""+device #String Name
      ConfigStructList[StringDeviceNames[device][3]] +="\","+str(StringDeviceNames[device][2])+"u"# hash     
      if (putnull==0):
         # The offset is not used in DALModDir.c. Hence, setting it to 0
         ConfigStructList[StringDeviceNames[device][3]] +=", 0"# offset
         ConfigStructList[StringDeviceNames[device][3]] +=", &DAL"+StringDeviceNames[device][0]+"_DriverInfo"#Driver
      else:
         ConfigStructList[StringDeviceNames[device][3]] +=", "+str(StringDeviceNames[device][1])# offset
         ConfigStructList[StringDeviceNames[device][3]] +=", NULL"#Struct Pointer
      # Getting Collision list 
      MyHash = StringDeviceNames[device][2]
      if ConfigType == '':
         collision_str_var = "Collision_"+str(StringDeviceNames[device][2])
      else:
         collision_str_var = "Collision_" + str(StringDeviceNames[device][2]) + "_" + ConfigType
      Collisions = ComputeCollisionIndex(MyHash, collision_str_var)
      if(len(HashCollisionOffset[MyHash])==1):
         ConfigStructList[StringDeviceNames[device][3]] +=", 0"
      else:
         ConfigStructList[StringDeviceNames[device][3]] +=", "+ str(len(HashCollisionOffset[MyHash]))
      ConfigStructList[StringDeviceNames[device][3]] +=", "+Collisions
      ConfigStructList[StringDeviceNames[device][3]] +=" "+"}"
      
#------------------------------------------------------------------------------
# PrintConfig : Generate DALModConfig and pack all sections
#------------------------------------------------------------------------------      
def PrintConfig(ModName, CONFIG, devcfgdatafilename, ConfigType ):
   #Header Size is 24 i.e. Total Len (4) + Name Section Len(4) + String Sec Len(4) + Byte Sec Len (4) + Uint Sec Len (4) + Num Devices (4)
   ConfigHeaderLen = 24 + 4*len(DeviceID)
   global UintSection, DevSection
   #UintSection = PackData(UintSection, 3)

   DevSection = PackData(DevSection, 3)
   SegmentOffset = [ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection) + len(DevSection),
         ConfigHeaderLen,
         ConfigHeaderLen + len(NameSection),
         ConfigHeaderLen + len(NameSection) + len(StringSection),
         ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection),
         len(DeviceID) / 2]
   for i in range(len(DeviceID)):
      if i%2 == 1:
         DeviceID[i] += SegmentOffset[4] + len(UintSection)
   SegmentOffset.extend(DeviceID)
   SegmentOffset = PackData(SegmentOffset, 3)
   PropBin = SegmentOffset + NameSection + StringSection + ByteSection + UintSection + DevSection

   #define image name
   if ModName == "modem":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_MODEM \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_MODEM \n")
      CONFIG.write("#endif \n")
   elif ModName == "dsp":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_DSP \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_DSP \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot_wm6":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT_WM_6 \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT_WM_6 \n")
      CONFIG.write("#endif \n")
   elif ModName == "boot_wm7":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_BOOT_WM_7 \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_BOOT_WM_7 \n")
      CONFIG.write("#endif \n")
   elif ModName == "tz":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_TZ \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_TZ \n")
      CONFIG.write("#endif \n")
   elif ModName == "wcn":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_WCN \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_WCN \n")
      CONFIG.write("#endif \n")
   elif ModName == "sps":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_SPS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_SPS \n")
      CONFIG.write("#endif \n")
   elif ModName == "rpm":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_RPM \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_RPM \n")
      CONFIG.write("#endif \n")
   elif ModName == "uefi":
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_UEFI \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_UEFI \n")
      CONFIG.write("#endif \n")
   else:
      CONFIG.write("#ifndef DAL_CONFIG_IMAGE_APPS \n")
      CONFIG.write("#define DAL_CONFIG_IMAGE_APPS \n")
      CONFIG.write("#endif \n")

   #include struct files
   for inc_files in StructIncFiles:
      CONFIG.write ("#include \"%s\"\n" %inc_files)
      
   #struct ptrs
   for struct_ptrs in StructPtrs:
      local_struct =0
      for struct in StructDef:
         name = StructDef[struct][0]
         if name == struct_ptrs :
            local_struct =1
            break
      if local_struct == 1 :
         continue
      if(len(ConfigType) !=0):
        struct_qual_name = struct_ptrs+'_'+ConfigType
      else:
        struct_qual_name = struct_ptrs
      CONFIG.write ('extern void * %s;\n' %struct_qual_name)

   #structure
   for index in StructDef:
      name = StructDef[index][0]
      data_type = StructDef[index][1]
      value = StructDef[index][2]
      CONFIG.write ("\n\nstatic " +  data_type + "\t" + name + "=" + value + ";\n")
   
   #[asmitp] : Incorporating Struct ptr table to accomodate for 64 bit:
   #1] Structure references will be in Struct ptr table
   #2] Type of Prop_bin changed from (void *) to uint32
   
   if(len(ConfigType) !=0):
      dalprop_structptrs_str = "DALPROP_StructPtrs_" + ConfigType
   else :
      dalprop_structptrs_str = "DALPROP_StructPtrs"
      
   CONFIG.write ("\nconst void * " + dalprop_structptrs_str + "[" + str(len(StructPtrs)+1) + "] =  {\n")
   for struct_ptrs in StructPtrs:
      local_struct =0
      for struct in StructDef:
         name = StructDef[struct][0]
         if name == struct_ptrs :
            local_struct =1
            break
      if(local_struct == 1) :
         array_symbol_index = struct_ptrs.find('[')
         if(array_symbol_index > 0):
            name_split = struct_ptrs.split('[')
            struct_ptrs = name_split[0]
         CONFIG.write ('\t &'+ struct_ptrs +",\n")
      else:
        if(len(ConfigType) !=0):
            CONFIG.write ('\t &' + struct_ptrs +'_' + ConfigType + ',\n')
        else:
            CONFIG.write ('\t {sizeof(void *), &' +struct_ptrs +'},\n')
        
   CONFIG.write ("\t NULL \n };")
   # CONFIG.write ("\n}\n\n")
   #binary props
   if(len(ConfigType) !=0):
      dalprop_propbin_str = "DALPROP_PropBin_" + ConfigType
   else :
      dalprop_propbin_str = "DALPROP_PropBin"
      
   CONFIG.write ("\nconst uint32 " + dalprop_propbin_str + "[] = {\n")
   for i in range(0, len(PropBin), 4):
      if i%20 == 0:
         CONFIG.write ("\n\t\t\t")
      if type(PropBin[i]) is StringType:
         CONFIG.write ("%s" %PropBin[i])
      else:
         if i+3<len(PropBin):
            CONFIG.write ("0x%.2x" %PropBin[i+3] + "%.2x" %PropBin[i+2] + "%.2x" %PropBin[i+1] + "%.2x" %PropBin[i])
         elif i+2<len(PropBin):
            CONFIG.write ("0x%.4x" %PropBin[i+2] + "%.2x" %PropBin[i+1] + "%.2x" %PropBin[i])
         elif i+1<len(PropBin):
            CONFIG.write ("0x%.6x" %PropBin[i+1] + "%.2x" %PropBin[i])
         else:
            CONFIG.write ("0x%.8x" %PropBin[i])
      if i != len(PropBin) - 4:
         CONFIG.write (", ")
      else:
         CONFIG.write (" };\n")
    
   #driver_entry driver_list[] = {{ /dev/buses/i2c1, 1234, {2,5,8 }, }
   PrintStringDeviceNameStruct(1, ConfigType)
   # Create the devcfg_data.c
   CONFIG_DATA = open(devcfgdatafilename, 'w')
   CONFIG_DATA.write ("#include \"DALSysTypes.h\" \n")
   CONFIG_DATA.write ("\nextern const void * " + dalprop_structptrs_str + "[];\n")
   CONFIG_DATA.write ("\nextern const uint32 " + dalprop_propbin_str + "[];\n")
   
   if(len(ConfigType) !=0):
      dalprop_propsinfo_str = "DALPROP_PropsInfo_" + ConfigType
   else :
      dalprop_propsinfo_str = "DALPROP_PropsInfo"
      
   if len(ConfigStructList)!=0:
      if(len(ConfigType) !=0):
         driver_list_str = "driver_list_" + ConfigType
      else:
         driver_list_str = "driver_list"
      CONFIG_DATA.write ("\nextern const StringDevice " + driver_list_str +"[];\n")
      for Col_array in CollisionDictionary:
         CONFIG.write ("\nstatic uint32 " + Col_array + " [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      CONFIG.write("\n\n\nconst StringDevice " + driver_list_str + "[] = ")
      CONFIG.write("{\n\t\t\t")
      for list in range(len(ConfigStructList)):
         CONFIG.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            CONFIG.write (",\n\t\t\t")
      CONFIG.write ("\n};\n")
      CONFIG_DATA.write("\n\nconst DALProps " + dalprop_propsinfo_str + " = {(const byte*)" + dalprop_propbin_str +", " + dalprop_structptrs_str + ", " + str(len(ConfigStructList)) + ", " + driver_list_str + "};\n")
   else:
      CONFIG_DATA.write("\n\nconst DALProps " + dalprop_propsinfo_str + " = {(const byte*)" + dalprop_propbin_str + ", " + dalprop_structptrs_str + " , 0, NULL};\n")

   CONFIG_DATA.close()
# PackData : Pack data as a byte array
#------------------------------------------------------------------------------      
def PackData(value, type1):
   if type1 == 1: 		#String
      value = list(value)
      for index in range (len(value)):
         value[index] = ord(value[index])
      value.append(0)
   elif type1 == 3:		#UINT32
      value_temp = []
      value=list(value)
      for index in range (len(value)):
         if type(value[index]) is StringType:
            value_temp.append(value[index])
            # Since this is a enum which is 4-bytes long and the PropBin list
            # is handled as 4-bytes at a time, appending 3 0's at the end of
            # the string.
            for j in range(3):
               
                value_temp.append(0)
         elif type(value[index]) is ListType:
            for idx in range(len(value[index])):
               value_temp.append(value[index][idx])
            if len(value[index]) % 4 != 0:
               for idx2 in range(4 - (len(value[index]) % 4)):
                  value_temp.append(0)
         else:
            for j in (unpack('BBBB',pack('I', value[index]))):
               value_temp.append(j)
      value = value_temp
      
   return value

#------------------------------------------------------------------------------
# GenerateGlobalDef : Read & Parse the Global Section for driver and return 
# data in a dictionary(GlobalDef)
#------------------------------------------------------------------------------   
def GenerateGlobalDef(pDriver):
   GlobalDef = {}
   for global_defs in pDriver.getElementsByTagName("global_def"):
      global_defs.normalize()
      for node in global_defs.childNodes:
         if node.nodeType == node.TEXT_NODE:
            continue
         name = node.getAttribute("name")
         type = node.getAttribute("type")
         type = int(type,0)
         value = node.firstChild.data
         if type == 1: #DALPROP_DATA_TYPE_STRING
            value = value.strip()
         elif (type == 2 or type ==3):
            re.sub("\s*", '', value)
            value = value.split(',')
            if(len(value) < 2) :
               raise ValueError('sequence cant be empty, for property ' + name + '\n')
            for index in range (len(value)-1):
               try:
                  value[index] = eval(value[index].lstrip())
               except NameError:
                  value[index] = str(value[index].strip())
               #value[index] = int(value[index], 16)
            del value[-1]
            value.insert(0, len(value)-1)
         elif (type == 4):
            #insert the include file if not already in our list
            value = value.strip()
            try:
               StructIncFiles.index(value)
            except ValueError:
               StructIncFiles.append(value)
         value = PackData(value, type)
         GlobalDef[name] = value
   return GlobalDef

#------------------------------------------------------------------------------
# Implement DJB2 hash 
#------------------------------------------------------------------------------          
def HashDJB2(key):
   myhash = 5381;
   for c in key:
      myhash = ((myhash << 5) + myhash) + ord(c);
   import ctypes;
   return myhash & 0xFFFFFFFF
   

#------------------------------------------------------------------------------
# Generate the array DevSection based on properties . The last element of the 
# array returned has the len of the array  
#------------------------------------------------------------------------------   
def GenerateArray(DriverName, node, GlobalDef, GlobalOffset, remove_dup_names):
   Supported_char = 'abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890%()!#$*+,-./:;=?@[]^_`{|}~'  
   global StringDeviceIdx
   global device_offset
   if len(DevSection):
      # poping the last element 
      device_offset += DevSection.pop()
   
   if node.getAttribute("id")[:2] =="0x":
      if DriverName == "NULL" or DriverName == "null":
         raise ValueError("You may not have a NULL Driver and uint32 Device ID")
      try :
         nod_id = int(node.getAttribute("id"),16)
         if nod_id > 0xFFFFFFFF:
            raise ValueError
      except:
         raise ValueError(node.getAttribute("id")+" is invalid uint32 Device Name. Remove \"0x\" if it is a String ID")
      DeviceID.append(nod_id)
      DeviceID.append(device_offset)
   else:
      if len(node.getAttribute("id"))==0:
         print "ERROR *** Empty Device Name"
         raise ValueError("Empty Device Name")
      if StringDeviceNames.has_key(node.getAttribute("id")):
         if remove_dup_names is False:
            #Raise error if duplicate device names
            print "ERROR *** Duplicate Device Name '%s'" %node.getAttribute("id")
            raise ValueError("Duplicate Device Name")
         else:
            DevSection.append(0)
            return
      if not all(L in list(Supported_char)  for L in node.getAttribute("id")):
         print "ERROR *** Device Name '%s' has NOT-Supported Characters" %node.getAttribute("id")
         print "Supported Charactors are :"
         print str(list(Supported_char))[1:-1]
         raise ValueError("NOT-Supported Characters Device Name")
      else:
         #device name is string and not uint32 number
         hash = HashDJB2(node.getAttribute("id"))
         StringDeviceNames[node.getAttribute("id")]= [DriverName,device_offset, hash,StringDeviceIdx]
         if HashCollisionOffset.has_key(hash):
            HashCollisionOffset[hash].append(StringDeviceIdx)
         else:
            HashCollisionOffset[hash] = [StringDeviceIdx]
         StringDeviceIdx +=1
   
   #Every property is stored in 2 words(header[type,nameoffset], value)
   valLen = 8* len(node.getElementsByTagName("props"))
   
   for Props in node.getElementsByTagName("props"):
      
      # if the prop is a string then store in namesection
      name = Props.getAttribute("name")
      if(len(name) != 0):
         name = name.encode('ascii', 'ignore')
         # make sure the prop name is unique in the NameSection
         if name not in PropNames:
            PropNames.append(name)
            nameoffset = len(NameSection)
            charname = PackData(name, 1)
            NameSection.extend(charname)
         else:
            nameoffset = 0;
            loc = PropNames.index(name)
            for idx in range(loc):
               nameoffset += len(PropNames[idx]) + 1
         nameoffset = nameoffset | 0x800000; # turn on 23rd bit to signify its name offset
      else:
         nameoffset = Props.getAttribute("id")
         if(nameoffset.isdigit() is True):
            nameoffset = eval(nameoffset)
      
      type1 = Props.getAttribute("type").strip()
      #if the prop is primitive type
      if type1[:2] =="0x":
         type1 = int(type1, 16)
      
      # if the prop is userdefined struct array
      instance_is_array = 0
      array = Props.getAttribute("array")
      if(len(array) and "True".lower() in array.lower() ):
         instance_is_array =1
         re.sub("(\[\.*\])", '', type1) # get rid of '[]' if client mistakenly added in type
      
      # Support adding comments within device tag <device>...</device>
      char_data = ""
      for child in Props.childNodes:
         if child.nodeType == child.COMMENT_NODE: # skip, if its is a comment node
            continue
         char_data = char_data + re.sub("(\s*|\n)", '', child.data)
      
      if type1 == 0x02:   #DALPROP_ATTR_TYPE_UINT32
         try:
            value = eval(char_data)
         except NameError:
            value= str(char_data)

         if type(value) == int:
             if len(Props.getAttribute('min')) > 0:
                min = int(Props.getAttribute('min'))
                if value < min:
                    if len(name) > 0:
                        raise ValueError('Property value {} is less than minimum {} for device id {}, property name {}'.format(value, min, node.getAttribute("id"), name))
                    else:
                        raise ValueError('Property value {} is less than minimum {} for device id {}, property id {}'.format(value, min, node.getAttribute("id"), Props.getAttribute("id")))

             if len(Props.getAttribute('max')) > 0:
                max = int(Props.getAttribute('max'))
                if value > max:
                    if len(name) > 0:
                        raise ValueError('Property value {} is greater than maximum {} for device id {}, property name {}'.format(value, max, node.getAttribute("id"), name))
                    else:
                        raise ValueError('Property value {} is greater than maximum {} for device id {}, property id {}'.format(value, max, node.getAttribute("id"), Props.getAttribute("id")))
      
      elif type1 == 0x08: # DALPROP_ATTR_TYPE_BYTE_SEQ
         value = char_data.split(',')
         if(len(value) < 2) :
             raise ValueError('sequence cant be empty, for property ' + name + '\n')
         for index in range(len(value) - 1):
            value[index] = value[index].lstrip()
            value[index] = eval(value[index])   
         value.insert(0, len(value) - 1)
         # fix the length and pop the 'end'
         value[0] -= 1
         if "end" == value[len(value) - 1]:
            value.pop()
         while len(value)%4 != 0:
            value.append(0)
         valLen +=len(value)-4
      
      elif type1 == 0x12:   #DALPROP_ATTR_TYPE_STRUCT_PTR
         try:
            StructPtrs.index(char_data)
         except ValueError:
            StructPtrs.append(char_data)
         value = StructPtrs.index(char_data)
         #len_struct = "sizeof(" + str(char_data) + ")" 
         name_addr = value
      
      # if its a user defined type, doesnot have pre-defined hex type
      elif (re.match( r'\D', str(type1))):  
         data_type = str(type1) 
         type1 = 0x12          # DALPROP_ATTR_TYPE_STRUCT_PTR    
         struct_index = len(StructPtrs)
         if(instance_is_array > 0): # if array, add [] syntax for C compiler
            struct_name = "devcfg_" + str(struct_index) + "[]"
         else:
            struct_name = "devcfg_" + str(struct_index)   
         StructPtrs.append(struct_name)
         value = StructPtrs.index(struct_name)
         name_addr = value
         StructDef.setdefault(value, [struct_name, data_type, char_data])
         
      else:
         if char_data in GlobalOffset:
            value = GlobalOffset[char_data]
         else:
            if type1 == 0x18:
               value = len(ByteSection)
               ByteSection.extend(GlobalDef[char_data])
            elif type1 == 0x11:
               value = len(StringSection)
               StringSection.extend(GlobalDef[char_data])
            elif type1 == 0x14:
               value = len(UintSection)
               UintSection.extend(GlobalDef[char_data])
         GlobalOffset[char_data] = value
      
      #logic to store type and nameoffset in 32 bit, higher 8 bits for type
      str_nameoffset = str(nameoffset)
      if(str_nameoffset.isdigit() is True) : 
         type1 = type1 & 0x000000FF #discard all higher 28 bits
         nameoffset = nameoffset & 0x00FFFFFF #discard all higher 8 bits
         type_nameoffset = (type1<<24) | nameoffset #123456780000...| 00000000xxxxx.....= 12345678xxxxxxx
         if(type1 == 0x12):
            DevSection.extend([type_nameoffset, name_addr])
         else:
            DevSection.extend([type_nameoffset, value])
      else:
         #str_type = str(type1) + " & 0x000000FF" #discard all higher 28 bits
         type1 = (type1 & 0x000000FF) << 24 #discard all higher 28 bits and store on higher 8 bit in word
         str_nameoffset = str(nameoffset) +  " & 0x00FFFFFF" #discard all higher 8 bits
         #type1 = type1 & 0x000000FF #discard all higher 28 bits
         type_nameoffset = "(" + str(type1) + "| (" + str_nameoffset + "))"
         if(type1 == 0x12):
            DevSection.extend([type_nameoffset, name_addr])
         else:
            DevSection.extend([type_nameoffset, value])
        
   DevSection.append(0xFF00FF00)
   # The length of the End marker
   valLen+=4
   DevSection.append(valLen)

def AdjustStringDeviceOffsets():
# This function will print DALMod file
   ConfigHeaderLen = 24 + 4*len(DeviceID)
   global UintSection, DevSection
   # Looping through all String devices
   for i in StringDeviceNames:
      StringDeviceNames[i][1] +=ConfigHeaderLen + len(NameSection) + len(StringSection) + len(ByteSection) + len(UintSection)

#------------------------------------------------------------------------------
# This function will print DALMod file
#------------------------------------------------------------------------------
def PrintModDir(ModDirFileName, DriverList):

   MODDIR = open(ModDirFileName, 'w')
   MODDIR.write ("#include \"DALStdDef.h\" \n")
   MODDIR.write ("#include \"DALReg.h\" \n\n")
   MODDIR.write ("#include\"DALSysTypes.h\"\n")
   for drivers in DriverList:
      if drivers == "NULL" or drivers == "null":
         continue 
      MODDIR.write ("extern DALREG_DriverInfo DAL%s_DriverInfo;\n" %drivers)
   MODDIR.write ("\nstatic DALREG_DriverInfo * DALDriverInfoArr[] = {\n")
   driver_device_id_len =0
   for drivers in DriverList:
      if drivers == "NULL" or drivers == "null":
         continue 
      driver_device_id_len += 1  
      MODDIR.write ("\t& DAL%s_DriverInfo" %drivers)
      MODDIR.write (",\n")
   MODDIR.write ("};\n\n")
   MODDIR.write ("DALREG_DriverInfoList gDALModDriverInfoList = {"+str(driver_device_id_len) + ", DALDriverInfoArr}; \n") 
   PrintStringDeviceNameStruct(0, '')
   if(len(ConfigStructList)!=0):
      for Col_array in CollisionDictionary:
         MODDIR.write ("\nstatic uint32 "+Col_array+" [] = {"+str(CollisionDictionary[Col_array])[1:-1]+"};")
      MODDIR.write("\n\n\nStringDevice DAL_Mod_driver_list[] = ")
      MODDIR.write("{\n\t\t\t")  
      driver_string_device_len = 0;   
      for list in range(len(ConfigStructList)):
         if(ConfigStructList[list] == "{0,0,0,0,0,0}"): 
            continue
         driver_string_device_len+=1 
         MODDIR.write(ConfigStructList[list])
         if(list <len(ConfigStructList)-1):
            MODDIR.write (",\n\t\t\t")
      if driver_string_device_len ==0:
         MODDIR.write ("{NULL}\n")
      MODDIR.write ("\n};\n")
      MODDIR.write ("\nDALProps DAL_Mod_Info = {NULL, 0 ,"+str(driver_string_device_len)+", DAL_Mod_driver_list};\n")
   else:
      MODDIR.write ("\nDALProps DAL_Mod_Info = {NULL, 0 ,0 , NULL};\n")
      
#------------------------------------------------------------------------------
# CheckDupDriverID : This function will check for duplicate drivers
#------------------------------------------------------------------------------      
def CheckDupDriverID(ModName, DriverList, remove_dup_dr_names):
   # check if 'driver' is duplicated in the DriverList
   idx = 1
   DriverList_mod = []
   for driver in DriverList:
      if remove_dup_dr_names is False:
         if driver in DriverList[idx:] and driver!= "NULL" and driver!= "null":
            # found a duplicate driver and no need to proceed any further
            print "ERROR *** DAL Driver '%s'" % driver, "has been included more than once for SW image '%s'" % ModName
            print "ERROR *** Please check XML files for '%s'" % ModName
            raise ValueError(driver)
      else:
         if driver not in DriverList[idx:] and driver!= "NULL" and driver!= "null":
            DriverList_mod.append(driver)
         
      idx += 1
   return DriverList_mod

#------------------------------------------------------------------------------
# CheckDupDeviceID : This function will check for duplicate deviceid
#------------------------------------------------------------------------------
def CheckDupDeviceID(ModName, remove_dup_names):
   # build the device ID list
   devIDList = []
   for i in range(0, len(DeviceID), 2):
      devIDList.append(DeviceID[i])
   # check if there are any duplicated device IDs
   idx = 1
   if remove_dup_names is False:
      for dev in devIDList:
         if dev in devIDList[idx:]:
            # find a duplicate device ID and no need to proceed any further
            print "ERROR *** DAL Device ID 0x%.8x" % dev, "has been included more than once for SW image '%s'" % ModName
            print "ERROR *** Please check XML files for '%s'" % ModName
            raise ValueError(hex(dev))
         idx += 1

#------------------------------------------------------------------------------
# Print_Debug : Print Debug Information
#------------------------------------------------------------------------------
def Print_Debug ():
    print "\nSegmentOffset: ", SegmentOffset, "Len: ", len(SegmentOffset)
    print "\nName Section:", NameSection, "Len: ",len(NameSection)
    print "\nByte Section: ", ByteSection, "Len: ",len(ByteSection)
    print "\nString Section:" , StringSection, "Len: ",len(StringSection)
    print "\nUINT Section:", UintSection, "Len: ",len(UintSection)
    print "\nDevice Section: ", DevSection

# Main function for Propgen
def main():
   parser = OptionParser("usage: %prog [options]")
   parser.add_option("-x", "--XmlFile", dest="xml_filename", help="XML Filename, INPUT, REQUIRED")
   parser.add_option("-t", "--ConfigType", dest="config_type", help="Config Type, INPUT")
   parser.add_option("-d", "--DirName", dest="output_dir", help="Directory name, OUTPUT")
   parser.add_option("-m", "--ModDirFile", dest="mod_dir_filename", help="DALModDir filename, OUTPUT")
   parser.add_option("-c", "--ConfigFile", dest="cfg_dir_filename", help="DALConfig filename, OUTPUT")
   parser.add_option("-e", "--DevcfgDataFile", dest="devcfgdata_filename", help="Devcfg Data filename, OUTPUT")
   # RemoveDups is set to True for DALModDir.c creation as this will be a superset of all the drivers present in the system
   parser.add_option("-r", "--RemoveDups", dest="remove_duplicate_drivers", help="Option to remove duplicate drivers, INPUT")
   parser.add_option("-s", "--MasterCFile", dest="master_cfile", help="C file, INPUT")
   
   
   (options, args) = parser.parse_args()
   DirName = None
   ModDirFileName = None
   ConfigFileName = None
   DevcfgDataFileName = None
   MasterCFileName = None 
   
   if options.xml_filename is None:
      parser.print_help()
      exit()
   else:
      xml_file = options.xml_filename

   if options.config_type is not None:
      ConfigType = options.config_type
   if options.output_dir is not None:
      DirName = options.output_dir
   if options.mod_dir_filename is not None:
      ModDirFileName = options.mod_dir_filename
   if options.cfg_dir_filename is not None:
      ConfigFileName = options.cfg_dir_filename
   if options.devcfgdata_filename is not None:
      DevcfgDataFileName = options.devcfgdata_filename
   if options.remove_duplicate_drivers is not None:
      remove_dup_names =  options.remove_duplicate_drivers
   else:
      # If nothing specified, then assume this is for DALConfig .c file creation 
      # and check for duplicates and error out in case of duplicates
      remove_dup_names = False
   
   if not os.path.isdir(DirName):
      os.mkdir( DirName )
   ModList = xml.dom.minidom.parse(xml_file)   
   
   # open config file if provided
   if ConfigFileName != None:
      CONFIG = open(ConfigFileName, 'a')
      CONFIG.write ("#include \"DALStdDef.h\" \n")
      CONFIG.write ("#include \"DALSysTypes.h\" \n\n")
   
   # Read the XML file and generate the array
   for Mod in ModList.getElementsByTagName("module"):
      DriverList = []
      ModName = Mod.getAttribute("name")   
      for Driver in Mod.getElementsByTagName("driver"):
         GlobalDef = {}
         DriverList.append(Driver.getAttribute("name"))
         GlobalDef = GenerateGlobalDef(Driver)
         GlobalOffset = {}
         for Device in Driver.getElementsByTagName("device"):
            GenerateArray(Driver.getAttribute("name"),Device, GlobalDef, GlobalOffset, remove_dup_names)
      # Pop the last element of the array, since it is nothing but the len
      if len(DevSection):
         DevSection.pop()
      AlignSectionAddr()
      # check for duplicated driver IDs and device IDs
      modDriverList = CheckDupDriverID(ModName, DriverList, remove_dup_names)
      CheckDupDeviceID(ModName, remove_dup_names)
      AdjustStringDeviceOffsets()
      if ModDirFileName != None:
         PrintModDir(ModDirFileName, modDriverList)

      if ConfigFileName != None:
         PrintConfig(ModName, CONFIG, DevcfgDataFileName, ConfigType)
         CONFIG.close()
         
if __name__ == "__main__":
   main()

