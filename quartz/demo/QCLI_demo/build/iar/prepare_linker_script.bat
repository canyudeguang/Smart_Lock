@ECHO OFF
REM Copyright (c) 2017 Qualcomm Atheros, Inc.
REM All Rights Reserved.
REM Qualcomm Atheros Confidential and Proprietary.
REM $ATH_LICENSE_TARGET_MK$

REM Setup the paths for the build
SET Project=Quartz
SET RootDir=%~dp0\..\..\..\..\..
SET LibDir=%RootDir%\lib\cortex-m4IPT\%Platform%
SET LinkerScriptDir=%RootDir%\build\scripts\linkerScripts
SET LINKFILE_T="linkerCfg\v2\threadx\%Project%.icf"
SET LINKFILE_F="linkerCfg\v2\freertos\%Project%.icf"
SET LINKERLIST="LinkerLibs.txt"
SET QUARTZ_IAR_EWP="Quartz_4020_v2.ewp"


REM -------------Generate link icf file for Freertos------------------------
echo Generating linker file for freertos
SET Platform=freertos

IF EXIST %LINKFILE_F% del %LINKFILE_F%
IF EXIST %LINKERLIST% del %LINKERLIST%
IF EXIST app.placement del app.placement

REM Update application PlacementFile
python %LinkerScriptDir%\CreateAppPlacementFile.py %RootDir%\bin\cortex-m4\%Platform%\sys.placement %RootDir%\bin\cortex-m4\%Platform%\cust.placement app.config app.placement 2>dbg.CreateAppF
if %errorlevel% == 1 (
echo Failed to update application placement file. Check dbg.CreateAppF for detail... 
goto EndOfFile
) 
REM Update application list file
python %LinkerScriptDir%\CreateIarLinkLibsFile.py %QUARTZ_IAR_EWP% %Platform% %LINKERLIST% 2>dbg.CreateListF
if %errorlevel% == 1 (
echo Failed to update application list file. Check dbg.CreateListF for detail... 
goto EndOfFile
) 
REM Create a linker script
python %LinkerScriptDir%\MakeLinkerScript.py %RootDir%\bin\cortex-m4\%Platform%\DefaultTemplateLinkerScript.icf app.placement %LINKERLIST% iar app.config > %LINKFILE_F% 2>dbg.MakeF
if %errorlevel% == 1 (
echo Failed to create linker script file. Check dbg.MakeF for detail... 
goto EndOfFile
) 

REM -------------Generate link icf file for ThreadX------------------------
echo Generating linker file for ThreadX
SET Platform=threadx

IF EXIST %LINKFILE_T% del %LINKFILE_T%
IF EXIST %LINKERLIST% del %LINKERLIST%
IF EXIST app.placement del app.placement

REM Update application PlacementFile
python %LinkerScriptDir%\CreateAppPlacementFile.py %RootDir%\bin\cortex-m4\%Platform%\sys.placement %RootDir%\bin\cortex-m4\%Platform%\cust.placement app.config app.placement 2>dbg.CreateAppT
if %errorlevel% == 1 (
echo Failed to update application placement file. Check dbg.CreateAppT for detail... 
goto EndOfFile
) 
REM Update application list file
python %LinkerScriptDir%\CreateIarLinkLibsFile.py %QUARTZ_IAR_EWP% %Platform% %LINKERLIST% 2>dbg.CreateListT
if %errorlevel% == 1 (
echo Failed to update application list file. Check dbg.CreateListT for detail... 
goto EndOfFile
) 
REM Create a linker script
python %LinkerScriptDir%\MakeLinkerScript.py %RootDir%\bin\cortex-m4\%Platform%\DefaultTemplateLinkerScript.icf app.placement %LINKERLIST% iar app.config > %LINKFILE_T% 2>dbg.MakeT
if %errorlevel% == 1 (
echo Failed to create linker script file. Check dbg.MakeT for detail... 
goto EndOfFile
) 

echo Done.
GOTO:EndOfFile

:EndOfFile

