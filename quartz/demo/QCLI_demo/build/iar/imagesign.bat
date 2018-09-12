@ECHO OFF
REM Copyright (c) 2016 Qualcomm Atheros, Inc.
REM All Rights Reserved.
REM Qualcomm Atheros Confidential and Proprietary.


REM Setup the paths for the build
SET PROJECT=Quartz
SET RootDir=%~dp0\..\..\..\..\..
SET OutDir=%~dp0\output\Exe
SET ScriptDir=%RootDir%\build\scripts
SET SectoolsDir=%RootDir%\sectools

IF /I "%~1" == "" (
   SET CHIPSET_VARIANT=qca4020
) ELSE IF /I "%1" == "4020" (
   SET CHIPSET_VARIANT=qca4020
) ELSE IF /I "%1" == "4024" (
   SET CHIPSET_VARIANT=qca4024
) ELSE IF /I "%1" == "4025" (
   SET CHIPSET_VARIANT=qca4025
)

echo Signing Images for %CHIPSET_VARIANT%

python %SectoolsDir%\sectools.py secimage -i %OUTDIR%\%PROJECT%.elf -c 4020_secimage.xml -sa -g m4 -o .
python %SectoolsDir%\sectools.py secimage -i %RootDir%\bin\cortex-m0\threadx\ioe_ram_m0_threadx_ipt.mbn -c 4020_secimage.xml -sa -g m0 -o .
IF /I "%CHIPSET_VARIANT%" == "qca4020" (
    python %SectoolsDir%\sectools.py secimage -i %RootDir%\bin\wlan\wlan_fw_img.bin -c 4020_secimage.xml -sa -g kf -o .
)
