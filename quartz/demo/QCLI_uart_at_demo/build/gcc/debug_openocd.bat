@ECHO OFF

REM Note to use this debug script, the JlinkGDBServer and arm-non-eabi-gdb executables
REM needs to be in the system path. If either of these isn't in the system path,
REM JLINK_PATH and/or GCC_BIN_PATH must be defined with their location.

IF /I "%CHIPSET_VERSION%" == "v1" (SET CHIPSET=v1) ELSE (SET CHIPSET=v2)

IF /I "%CHIPSET%" == "v1" (
    @ECHO ****************************************************************************
    @ECHO    Starting M4 Debug session for Quartz QCLI Application for v1 Chipset
    @ECHO                  To debug v2, set CHIPSET_VERSION=v2                  
    @ECHO *****************************************************************************
) ELSE (
    @ECHO ****************************************************************************
    @ECHO    Starting M4 Debug session for Quartz QCLI Application for v2 Chipset
    @ECHO                  To debug v1, set CHIPSET_VERSION=v1                  
    @ECHO *****************************************************************************
)

REM Get the paths from the environment variables if they are set.
SET SERVER_PATH=
SET CLIENT_PATH=
if NOT "%GCC_BIN_PATH%" == "" SET CLIENT_PATH=%GCC_BIN_PATH:"=%\

REM Start the GDB Server.

Taskkill /IM openocd.exe /F 2> nul
START /B openocd.exe -f qca402x_openocd.cfg

REM Start the GDB Client
IF /I "%1" == "asic" (
    @ECHO Running ASIC gdb script
    "arm-none-eabi-gdb.exe" -x "%CHIPSET%\m4asic.gdbinit"
) ELSE IF  /I "%1" == "m4" (
    @ECHO Running ASIC gdb script for m4-only configuration
    "arm-none-eabi-gdb.exe" -x "%CHIPSET%\quartzcdb.gdbinit"
) ELSE (
    @ECHO Running Emulation gdb script for v2 configuration
    "arm-none-eabi-gdb.exe" -x "%CHIPSET%\Quartz_m4.gdbinit"
)

Taskkill /IM openocd.exe /F 2> nul