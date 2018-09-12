@ECHO OFF
SETLOCAL EnableDelayedExpansion

REM Get the paths from the environment variables if they are set.

IF NOT "%JLINK_PATH%"=="" SET GDB_SERVER_PATH=%JLINK_PATH%\JLinkGDBServerCL.exe
IF NOT "%CLIENT_PATH%"=="" SET GDB_PATH=%CLIENT_PATH%\arm-none-eabi-gdb.exe

rmdir /S /Q gdbout
mkdir gdbout

SET ROOTDIR=..\..\..\..\..
SET SCRIPTDIR=%ROOTDIR%\build\tools\flash
SET M4_DIR=%ROOTDIR%\quartz\demo\QCLI_demo\build\iar\output\Exe
SET M4_IMAGE=%M4_DIR%\Quartz_HASHED.elf

SET M0_DIR=%ROOTDIR%\bin\cortex-m0\threadx\
SET M0_IMAGE=%M0_DIR%\ioe_ram_m0_threadx_ipt.mbn

REM Set WLAN_IMAGE=none (or any non-existent filename) to avoid programming a WLAN image.
SET WLAN_DIR=%ROOTDIR%\bin\wlan
SET WLAN_IMAGE=%WLAN_DIR%\wlan_fw_img.bin

IF "%GDB_PATH%"=="" (
	where arm-none-eabi-gdb.exe > gdbclient.txt
	SET /P GDB_PATH=<gdbclient.txt
	DEL /F gdbclient.txt
)

IF "%GDB_SERVER_PATH%"=="" (
	where JLinkGDBServerCL.exe > gdbserver.txt
	SET /P GDB_SERVER_PATH=<gdbserver.txt
	DEL /F gdbserver.txt
)

SET GDB_SERVER=localhost
SET GDB_PORT=2335

REM Enable Below if Signed images are used
REM SET M4_IMAGE=4020\m4\Quartz.elf
REM SET M0_IMAGE=4020\m0\ioe_ram_m0_threadx_ipt.mbn
REM SET WLAN_IMAGE=4020\kf\wlan_fw_img.bin


REM Create partition_table.xml
IF EXIST "%WLAN_IMAGE%" (
    python %SCRIPTDIR%\gen_part_table.py  --file=%M4_IMAGE%  --file=%M0_IMAGE%  --file=%WLAN_IMAGE%
) else (
python %SCRIPTDIR%\gen_part_table.py --file=%M4_IMAGE% --file=%M0_IMAGE%
)

IF errorlevel 1 (
    echo Abort flash.bat: gen_part_table.py failed
    goto exit
)		

REM Convert to fwd_table.xml
python %SCRIPTDIR%\gen_fwd_table.py -x generated_partition_table.xml --rawprogram generated_fwd_table.xml

if errorlevel 1 (
    echo Abort flash.bat: gen_fwd_table.py failed
    goto exit
)

echo Starting GDB Server....

REM Set the options for the Jlink GDB server
SET JLinkOptions=-scriptfile "%~dp0%Quartz.JLinkScript"
SET JLinkOptions=%JLinkOptions% -select USB
SET JLinkOptions=%JLinkOptions% -device Unspecified
SET JLinkOptions=%JLinkOptions% -endian little
SET JLinkOptions=%JLinkOptions% -if JTAG
SET JLinkOptions=%JLinkOptions% -speed 1000
SET JLinkOptions=%JLinkOptions% -noir
REM SET JLinkOptions=%JLinkOptions% -singlerun
SET JLinkOptions=%JLinkOptions% -nolocalhostonly
SET JLinkOptions=%JLinkOptions% -port 2335

REM Start the GDB Server.
START "JLinkGDBServer" /MIN "%GDB_SERVER_PATH%" %JLinkOptions%

python %SCRIPTDIR%/flash_through_gdb.py ^
	--rawprogram=generated_fwd_table.xml ^
	--verbose=1 ^
	--verify ^
	--outputdir=gdbout ^
	--gdbport=%GDB_PORT% ^
	--gdbpath="%GDB_PATH%" ^
	--jtagprgelf=%SCRIPTDIR%\JTAGPROGRAMMER_IMG_ARNTRI.elf ^
	--search_path=%M4_DIR%,%M0_DIR%,%WLAN_DIR% ^
	--gdbserver %GDB_SERVER%:%GDB_PORT%

IF NOT %errorlevel% == 0 (
    echo Abort flash.bat: flash_through_gdb.py failed
    goto exit
) else (
	echo Flash Operation Completed Successfully...
)

:exit