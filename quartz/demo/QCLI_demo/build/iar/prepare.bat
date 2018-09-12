@ECHO OFF
REM Copyright (c) 2016 Qualcomm Atheros, Inc.
REM All Rights Reserved.
REM Qualcomm Atheros Confidential and Proprietary.

REM Setup the paths for the build
SET Project=Quartz
SET RootDir=%~dp0\..\..\..\..\..
SET SrcDir=%~dp0\..\..\src
SET NvmDir=%RootDir%\quartz\nvm
SET CHIPSET_VARIANT=qca4020
SET CHIPSET_REVISION=1p2

REM Validate the chipset variant. Default: 4020
IF /I "%~1" == "" (
   SET CHIPSET_VARIANT=qca4020
) ELSE IF /I "%1" == "4020" (
   SET CHIPSET_VARIANT=qca4020
) ELSE IF /I "%1" == "4024" (
   SET CHIPSET_VARIANT=qca4024
) ELSE IF /I "%1" == "4025" (
   SET CHIPSET_VARIANT=qca4025
) ELSE (
   ECHO Invalid chipset variant: %1%
   GOTO:Usage
)

IF /I "%~2" == "" (
SET CHIPSET_VERSION=v2
SET NVM_CONFIG=2.0
SET NVM=2p0
) ELSE IF /I "%2" == "v2" (
SET CHIPSET_VERSION=v2
SET NVM_CONFIG=2.0
SET NVM=2p0
) ELSE IF /I "%2" == "v1" (
SET CHIPSET_VERSION=v1
SET NVM_CONFIG=1.2
SET NVM=1p2
) ELSE (
   ECHO Invalid chipset version: %2%
   GOTO:Usage
)

MKDIR %SrcDir%\export >NUL 2>&1

robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem.h /XO >NUL 2>&1
robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem.c /XO >NUL 2>&1
robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem_som.c /XO >NUL 2>&1
robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem_mom.c /XO >NUL 2>&1

IF /I "%Platform%" == "threadx" (
   robocopy %RootDir%\build\tools\devcfg\threadx\ %SrcDir%\export *.* /XO >NUL 2>&1
) else (
   robocopy %RootDir%\build\tools\devcfg\freertos\ %SrcDir%\export *.* /XO >NUL 2>&1
)

DEL /F %SrcDir%\export\DALConfig_devcfg.c >NUL 2>&1
DEL /F %SrcDir%\export\devcfg_devcfg_data.c >NUL 2>&1
DEL /F %SrcDir%\export\DALConfig_fom.c >NUL 2>&1
DEL /F %SrcDir%\export\devcfg_fom_data.c >NUL 2>&1
python %RootDir%/build/tools/devcfg/propgen.py --XmlFile=%SrcDir%/export/DevCfg_master_devcfg_out.xml --DirName=%SrcDir%/export --ConfigFile=%SrcDir%/export/DALConfig_devcfg.c --DevcfgDataFile=%SrcDir%/export/devcfg_devcfg_data.c --ConfigType=%CHIPSET_VARIANT%_devcfg_xml
python %RootDir%/build/tools/devcfg/propgen.py --XmlFile=%SrcDir%/export/DevCfg_master_fom_out.xml --DirName=%SrcDir%/export --ConfigFile=%SrcDir%/export/DALConfig_fom.c --DevcfgDataFile=%SrcDir%/export/devcfg_fom_data.c --ConfigType=%CHIPSET_VARIANT%_fom_xml

REM NVM config

REM Clean up EDL files.
DEL /F "%SrcDir%\export\UsrEDL.c" >NUL 2>&1

REM Link the appropriate NVM library and generate the UserEDL C array to be built.

ECHO Generating sysEDL.c for %CHIPSET_VARIANT%


IF "%CHIPSET_VARIANT%" == "qca4020" (
         
    python %NvmDir%\tool\NVM2C.py -o %SrcDir%\export\UsrEDL.c -i %NvmDir%\config\%NVM_CONFIG%\4020\Carrier\QCA4020_%NVM%.nvm )

IF "%CHIPSET_VARIANT%" == "qca4024" (
         
    python %NvmDir%\tool\NVM2C.py -o %SrcDir%\export\UsrEDL.c -i %NvmDir%\config\%NVM_CONFIG%\4024\Carrier\QCA4024_%NVM%.nvm )

IF "%CHIPSET_VARIANT%" == "qca4025" (
       
    python %NvmDir%\tool\NVM2C.py -o %SrcDir%\export\UsrEDL.c -i %NvmDir%\config\%NVM_CONFIG%\4025\Carrier\QCA4025_%NVM%.nvm )

REM exit