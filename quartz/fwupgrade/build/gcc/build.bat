@ECHO OFF

REM @ 2016-2017 Qualcomm Technologies, Inc.
REM 2016 Qualcomm Atheros, Inc.
REM All Rights Reserved
REM Confidential and Proprietary - Qualcomm Technologies, Inc.

SETLOCAL EnableDelayedExpansion
REM Default CHIPSET_VERSION is v2
IF /I "%CHIPSET_VERSION%" == "" SET CHIPSET_VERSION=v2

REM Setup the paths for the build
SET RootDir=..\..\..\..
SET SrcDir=..\..
SET OutDir=output
SET ObjDir=%OutDir%\objs
SET LibFile=%OutDir%\fwup_engine.lib

REM Clobber command, delete object files and export directory
IF /I "%1" == "clobber" (
   SET exitflag=true
   GOTO Clobber
)

REM Sources to compile
SET CSrcs=fw_upgrade\fw_upgrade.c

REM Include directories
SET Includes=-I"%RootDir%\include"
SET Includes=%Includes% -I"%RootDir%\include\qapi"
SET Includes=%Includes% -I"%RootDir%\include\bsp"
SET Includes=%Includes% -I"%SrcDir%\fw_upgrade"

REM Setup the build variables
SET Compiler=arm-none-eabi-gcc
SET Archiver=arm-none-eabi-ar

SET COpts=-c -g -mcpu=cortex-m4 -mthumb -fno-short-enums -fno-exceptions -O2 -Wall

IF /I "%CHIPSET_VERSION%" == "v2" (
   SET Defines=!Defines! "-D V2"
   SET Defines=!Defines! "-D qurt_mutex_init(x)=qurt_mutex_create(x)"
   SET Defines=!Defines! "-D qurt_mutex_destroy(x)=qurt_mutex_delete(x)"
   SET Defines=!Defines! "-D qurt_signal_init(x)=qurt_signal_create(x)"
   SET Defines=!Defines! "-D qurt_signal_destroy(x)=qurt_signal_delete(x)"
   SET Defines=!Defines! "-D qurt_mutex_destroy(x)=qurt_mutex_delete(x)"
   SET Defines=!Defines! "-D FEATURE_QUARTZ_V2"
)

SET CFlags=%COpts% %Defines% %Includes%

REM Clean the output directory. Note the delay is to give Windows an opportunity to remove the old directory tree
RMDIR /s/q "%OutDir%" >NUL 2>&1
TIMEOUT /t 1          >NUL 2>&1
MKDIR "%OutDir%"
MKDIR "%ObjDir%"


REM Compile source
FOR %%F IN (%CSrcs%) DO (
   ECHO Compiling %%F
   "%Compiler%" %CFlags% -Wall -Werror "-D __FILENAME__=\"%%~nF.c\"" -o"%ObjDir%\%%~nF.o" "%SrcDir%\%%F"
   if errorlevel 1 goto EndOfFile
   SET ObjFiles=!ObjFiles! "%ObjDir%\%%~nF.o"
)

REM generate lib
ECHO Building %LibFile%
%Archiver% rcs %LibFile%  %ObjFiles%
if errorlevel 1 goto EndOfFile
goto EndOfFile


:Clobber
ECHO Deleting all temporary files ...
RMDIR /s/q "%OutDir%" >NUL 2>&1
RMDIR /s/q %SrcDir%\export >NUL 2>&1
goto EndOfFile


:EndOfFile
