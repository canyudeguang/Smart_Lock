@ECHO OFF

REM @ 2016-2018 Qualcomm Technologies, Inc.
REM 2016 Qualcomm Atheros, Inc.
REM All Rights Reserved
REM Confidential and Proprietary - Qualcomm Technologies, Inc.

SETLOCAL EnableDelayedExpansion

REM Default CHIPSET_VERSION is v2
IF /I "%CHIPSET_VERSION%" == "" (SET CHIPSET_VERSION=v2)

REM Determine the RTOS to build. Default to freertos.
IF /I "%~1" == "" (
   SET RTOS=freertos
) ELSE IF /I "%~1" == "t" (
   SET RTOS=threadx
) ELSE IF /I "%~1" == "f" (
   SET RTOS=freertos
) ELSE IF /I "%~1" == "prepare" (
REM special command, will export devcfg files
   SET RTOS=freertos
) ELSE IF /I "%~1" == "clobber" (
REM special command, will delete devcfg files
   SET RTOS=freertos
) ELSE (
   ECHO Invalid RTOS: %1
   GOTO:Usage
)

REM Validate the chipset variant. Default: 4020
IF /I "%~2" == "" (
   SET CHIPSET_VARIANT=qca4020
) ELSE IF /I "%2" == "4020" (
   SET CHIPSET_VARIANT=qca4020
) ELSE IF /I "%2" == "4024" (
   SET CHIPSET_VARIANT=qca4024
) ELSE IF /I "%2" == "4025" (
   SET CHIPSET_VARIANT=qca4025
) ELSE (
   ECHO Invalid chipset variant: %2%
   GOTO:Usage
)

SET CFG_FEATURE_WLAN=false

REM Validate the board variant  -- carrier, dut and CDB. Default: carrier
IF /I "%~3" == "" (
   SET BOARD_VARIANT=carrier
) ELSE IF /I "%3" == "c" (
   SET BOARD_VARIANT=carrier
) ELSE IF /I "%3" == "d" (
   SET BOARD_VARIANT=DUT
) ELSE IF /I "%3" == "cdb" (
   SET BOARD_VARIANT=CDB
) ELSE (
   ECHO Invalid board variant: %3%
   GOTO:Usage
)

REM Validate the chipset revision. Default: 1.2 for v1, 2.0 for v2
IF /I "%~4" == "" (
   IF /I "%CHIPSET_VERSION%" == "v2" (
      SET CHIPSET_REVISION=2p0
   ) ELSE (
      SET CHIPSET_REVISION=1p2
   )
) ELSE IF /I "%4" == "1p2" (
   SET CHIPSET_REVISION=1p2
) ELSE IF /I "%4" == "2p0" (
   SET CHIPSET_REVISION=2p0
) ELSE (
   ECHO Invalid chipset revision: %4%
   GOTO:Usage
)

REM Get the path of the optional custom NVM file.
IF /I "%~5" == "" (
   SET NVM_FILE=
) ELSE (
   IF EXIST "%~5" (
      SET NVM_FILE=%5
   ) ELSE (
      ECHO Invalid NVM file path: %5%
      GOTO:Usage
   )
)

@ECHO ****************************************************************************
@ECHO                      Building QCA402X QCLI Application for %CHIPSET_VERSION% Chipset
@ECHO                      RTOS             %RTOS%
@ECHO                      CHIPSET VARIANT  %CHIPSET_VARIANT%
@ECHO *****************************************************************************

REM Setup the paths for the build
SET Project=Quartz
SET RootDir=..\..\..\..\..
SET SrcDir=..\..\src
SET QmeshDir=..\..\QCLI_demo\src
SET QmeshDirAbs=..\ ..\..\QCLI_demo\src
SET NvmDir=%RootDir%\quartz\nvm
SET OutDir=output
SET ObjDir=%OutDir%\objs
SET SectoolsDir=%RootDir%\sectools
SET SECBOOT=false
SET LibDir=%RootDir%\lib\cortex-m4IPT\%RTOS%
SET SymFile="%RootDir%\bin\cortex-m4\IOE_ROM_IPT_IMG_ARNNRI_gcc.sym"
SET SymFileUnpatched="%RootDir%\bin\cortex-m4\IOE_ROM_IPT_IMG_ARNNRI_orig_fcns_gcc.sym"
SET ScriptDir=%RootDir%\build\scripts
SET LinkerScriptDir=%RootDir%\build\scripts\linkerScripts
SET LINKFILE="%OutDir%\%Project%.ld"
SET LIBSFILE="%OutDir%\LinkerLibs.txt"
SET MeshModelsCommonCode=..\..\..\..\qmesh\models\common
SET MeshClientModels=..\..\..\..\qmesh\models\client
SET MeshServerModels=..\..\..\..\qmesh\models\server

REM Prepare Command, copy device config files to export directory
IF /I "%1" == "prepare" (
   SET exitflag=true
   GOTO Prepare
)

REM Clobber command, delete object files and export directory
IF /I "%1" == "clobber" (
   SET exitflag=true
   GOTO Clobber
)

IF /I "%RTOS%" == "threadx" (
   SET Libs="%LibDir%\threadx.lib"
   SET Libs=!Libs! "%LibDir%\mom_patch_table_ARNTRI_qcm.o"
   SET Libs=!Libs! "%LibDir%\fom_patch_table_ARNTRI_qcm.o"
   SET Libs=!Libs! "%LibDir%\som_patch_table_ARNTRI_qcm.o"
) ELSE (
   SET Libs="%LibDir%\free_rtos.lib"
   SET Libs=!Libs! "%LibDir%\mom_patch_table_ARNFRI_qcm.o"
   SET Libs=!Libs! "%LibDir%\fom_patch_table_ARNFRI_qcm.o"
   SET Libs=!Libs! "%LibDir%\som_patch_table_ARNFRI_qcm.o"
)

REM Sources to compile
SET CWallSrcs=
SET CSrcs=%CSrcs% common\platform\sbrk.c
SET CSrcs=%CSrcs% common\platform\qcli\qcli.c
SET CSrcs=%CSrcs% common\platform\qcli\qcli_util.c
SET CSrcs=%CSrcs% common\platform\qcli\pal.c
SET CSrcs=%CSrcs% common\platform\qcli\lp\fom_lp_test.c
SET CSrcs=%CSrcs% common\platform\qcli\lp\som_lp_test.c
SET CSrcs=%CSrcs% common\platform\qcli\lp\mom_lp_test.c

SET CSrcs=%CSrcs% export\platform_oem.c
SET CSrcs=%CSrcs% export\platform_oem_som.c
SET CSrcs=%CSrcs% export\platform_oem_mom.c
SET CSrcs=%CSrcs% export\DALConfig_devcfg.c
SET CSrcs=%CSrcs% export\DALConfig_fom.c
SET CSrcs=%CSrcs% export\devcfg_devcfg_data.c
SET CSrcs=%CSrcs% export\devcfg_fom_data.c
SET CSrcs=%CSrcs% export\UsrEDL.c

SET CSrcs=%CSrcs% common\qmesh_demo_platform.c
SET CSrcs=%CSrcs% common\qmesh_demo_utilities.c
SET CSrcs=%CSrcs% common\qmesh_demo_nvm_utilities.c

IF /I "%QMESH_PROVISIONER_DEMO%"=="true" (
	SET CWallSrcs=!CWallSrcs! provisioner\qmesh_demo_provisioner_main.c
	SET CWallSrcs=!CWallSrcs! provisioner\qmesh_demo_provisioner.c
	SET CWallSrcs=!CWallSrcs! provisioner\qmesh_demo_composition.c
)

IF /I "%QMESH_CLIENT_DEMO%"=="true" (
	SET CWallSrcs=!CWallSrcs! client\qmesh_demo_client.c
	SET CWallSrcs=!CWallSrcs! client\qmesh_demo_client_main.c
	SET CWallSrcs=!CWallSrcs! client\qmesh_demo_composition.c
)

IF /I "%QMESH_SERVER_DEMO%"=="true" (
	SET CSrcs=%CSrcs% server\qmesh_demo_server.c
	SET CWallSrcs=!CWallSrcs! server\qmesh_demo_server_main.c
	SET CWallSrcs=!CWallSrcs! server\qmesh_demo_composition.c
)

SET CSrcs=%CSrcs% %QmeshDir%\qmesh\models\client_handler\model_client_menu.c

SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\generic_onoff_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\generic_power_onoff_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\generic_default_transition_time_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\generic_level_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\generic_power_level_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\light_lightness_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\light_hsl_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\config_client_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\models\client_handler\model_client_event_handler.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\gatt_bearer\src\qmesh_ble_gap.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\gatt_bearer\src\qmesh_ble_gatt.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\gatt_bearer\src\qmesh_ble_gatt_client.c
SET CWallSrcs=!CWallSrcs! !QmeshDir!\qmesh\gatt_bearer\src\qmesh_ble_gatt_server.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\generic_onoff_client.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\generic_power_onoff_client.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\generic_default_transition_time_client.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\generic_level_client.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\generic_power_level_client.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\light_lightness_client.c
SET CWallSrcs=!CWallSrcs! !MeshClientModels!\light_hsl_client.c
SET CWallSrcs=!CWallSrcs! !MeshModelsCommonCode!\qmesh_cache_mgmt.c
SET CWallSrcs=!CWallSrcs! !MeshModelsCommonCode!\qmesh_delay_cache.c
SET CWallSrcs=!CWallSrcs! !MeshModelsCommonCode!\qmesh_model_common.c
SET CWallSrcs=!CWallSrcs! !MeshModelsCommonCode!\qmesh_model_debug.c
SET CWallSrcs=!CWallSrcs! !MeshModelsCommonCode!\qmesh_model_nvm.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_default_transition_time_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_poweronoff_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_poweronoff_setup_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_level_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_powerlevel_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_powerlevel_setup_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_generic_onoff_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_light_hsl_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_light_hsl_hue_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_light_hsl_saturation_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_light_hsl_setup_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_light_lightness_handler.c
SET CWallSrcs=!CWallSrcs! !MeshServerModels!\qmesh_light_lightness_setup_handler.c

REM Include directories
SET Includes=-I"%RootDir%\include"
SET Includes=%Includes% -I"%RootDir%\include\qapi"
SET Includes=%Includes% -I"%RootDir%\include\bsp"
SET Includes=%Includes% -I"%RootDir%\qmesh\include"
SET Includes=%Includes% -I"%RootDir%\qmesh\platform\inc"
SET Includes=%Includes% -I"%RootDir%\qmesh\platform\qca402x\inc"
SET Includes=%Includes% -I"%RootDir%\qmesh\models\client"
SET Includes=%Includes% -I"%RootDir%\qmesh\models\include"
SET Includes=%Includes% -I"%RootDir%\qmesh\core\inc"

SET Includes=%Includes% -I"%SrcDir%\common"
SET Includes=%Includes% -I"%SrcDir%\common\platform\qcli"

IF /I "%QMESH_PROVISIONER_DEMO%"=="true" (
SET Includes=%Includes% -I"%SrcDir%\provisioner"
)

IF /I "%QMESH_CLIENT_DEMO%"=="true" (
SET Includes=%Includes% -I"%SrcDir%\client"
)

IF /I "%QMESH_SERVER_DEMO%"=="true" (
SET Includes=%Includes% -I"%SrcDir%\server"
)

SET Includes=%Includes% -I"%RootDir%\quartz\demo\QCLI_demo\src\qmesh"
SET Includes=%Includes% -I"%RootDir%\quartz\demo\QCLI_demo\src\qmesh\models\include"
SET Includes=%Includes% -I"%RootDir%\quartz\demo\QCLI_demo\src\qmesh\models\client_handler"
SET Includes=%Includes% -I"%RootDir%\quartz\demo\QCLI_demo\src\qmesh\gatt_bearer\inc"


IF /I "%ENABLE_CPU_PROFILER%" == "1" (
   SET Includes=%Includes% -I"%SrcDir%\cpu_profiler"
)

REM External objects and libraries
SET Libs=%Libs% "%LibDir%\core.lib"
SET Libs=%Libs% "%LibDir%\qurt.lib"
SET Libs=%Libs% "%LibDir%\quartzplatform.lib"
SET Libs=%Libs% "%LibDir%\quartzplatform_xip.lib"
SET Libs=%Libs% "%LibDir%\WLAN.lib"

SET Libs=!Libs! "%LibDir%\WLAN_QAPI.lib"
SET Libs=!Libs! "%LibDir%\CUST_IPSTACK_INICHE.lib"

SET Libs=%Libs% "%LibDir%\WLAN_PROFILER.lib"
SET Libs=%Libs% "%LibDir%\net.lib"
SET Libs=%Libs% "%LibDir%\net_ram.lib"
SET Libs=%Libs% "%LibDir%\dhcpv6c.lib"
SET Libs=%Libs% "%LibDir%\sntpc.lib"
SET Libs=%Libs% "%LibDir%\dnssrvr.lib"
SET Libs=%Libs% "%LibDir%\sharkssl.lib"
SET Libs=%Libs% "%LibDir%\csr.lib"
SET Libs=%Libs% "%LibDir%\cryptolib.lib"
SET Libs=%Libs% "%LibDir%\httpsvr.lib"
SET Libs=%Libs% "%LibDir%\httpc.lib"
SET Libs=%Libs% "%LibDir%\mqttc.lib"
SET Libs=%Libs% "%LibDir%\vfs.lib"
SET Libs=%Libs% "%LibDir%\userpass.lib"
SET Libs=%Libs% "%LibDir%\i2s.lib"
SET Libs=%Libs% "%LibDir%\master_sdcc.lib"
SET Libs=%Libs% "%LibDir%\fwup.lib"
SET Libs=%Libs% "%LibDir%\fwup_engine.lib"
SET Libs=%Libs% "%LibDir%\qapi_ed25519.lib"
SET Libs=%Libs% "%LibDir%\qapi_securefs.lib"
SET Libs=%Libs% "%LibDir%\pka_port.lib"
SET Libs=%Libs% "%LibDir%\fs_helper.lib"
SET Libs=%Libs% "%LibDir%\quartz_crypto_qapi.lib"
SET Libs=%Libs% "%LibDir%\zigbee.lib"
SET Libs=%Libs% "%LibDir%\quartz_zigbee.lib"
SET Libs=%Libs% "%LibDir%\thread.lib"
SET Libs=%Libs% "%LibDir%\qapi_thread.lib"
SET Libs=%Libs% "%LibDir%\BLUETOPIA_SERVICES.lib"
SET Libs=%Libs% "%LibDir%\BLUETOPIA_QAPI_SERVICES.lib"
SET Libs=%Libs% "%LibDir%\mdns.lib"
SET Libs=%Libs% "%LibDir%\dnssd.lib"
SET Libs=%Libs% "%LibDir%\otp_tlv.lib"
SET Libs=%Libs% "%LibDir%\base64.lib"
SET Libs=%Libs% "%LibDir%\PERSIST_M4.lib"
SET Libs=%Libs% "%LibDir%\json.lib"
SET Libs=%Libs% "%LibDir%\json_qapi.lib"
SET Libs=%Libs% "%LibDir%\master_sdcc.lib"
SET Libs=%Libs% "%LibDir%\nichestack.lib"
SET Libs=%Libs% "%LibDir%\EDLManager.lib"
SET Libs=%Libs% "%LibDir%\tlv_transport.lib"
SET Libs=%Libs% "%LibDir%\crypto_port.lib"
SET Libs=%Libs% "%LibDir%\tee_master.lib"
SET Libs=%Libs% "%LibDir%\dnsclient.lib"
SET Libs=%Libs% "%LibDir%\securefs.lib"
SET Libs=%Libs% "%LibDir%\securefs_port.lib"
SET Libs=%Libs% "%LibDir%\v2core.lib"
REM Place all v2 patches here
SET PatchObjs=!PatchObjs! "%LibDir%\patch.lib"


SET Libs=!Libs! "%LibDir%\qmesh.lib"


REM Setup the build variables
SET Compiler=arm-none-eabi-gcc
SET ObjCopy=arm-none-eabi-objcopy
SET Archiver=arm-none-eabi-ar
SET Linker=arm-none-eabi-ld
SET ARM_OBJDUMP=arm-none-eabi-objdump

SET COpts=-c -g -mcpu=cortex-m4 -mthumb -fno-short-enums -fno-exceptions -O2 -ffunction-sections  -Wall

SET Defines=
SET Defines=!Defines! "-D HTC_SYNC"
SET Defines=!Defines! "-D DEBUG"

SET Defines=!Defines! "-D PLATFORM_QUARTZ" "-D PLATFORM_MULTITHREAD_SUPPORT"
SET Defines=!Defines! "-D ENABLE_PROVISIONING" 	


IF /I "%I2S_REG_TEST%" == "1" (
    SET Defines=!Defines! "-D I2S_REG_TEST"
)

SET Defines=%Defines% "-D V2"
SET Defines=%Defines% "-D qurt_mutex_init(x)=qurt_mutex_create(x)"
SET Defines=%Defines% "-D qurt_mutex_destroy(x)=qurt_mutex_delete(x)"
SET Defines=%Defines% "-D qurt_signal_init(x)=qurt_signal_create(x)"
SET Defines=%Defines% "-D qurt_signal_destroy(x)=qurt_signal_delete(x)"
SET Defines=%Defines% "-D qurt_mutex_destroy(x)=qurt_mutex_delete(x)"
SET Defines=%Defines% "-D FEATURE_QUARTZ_V2"

IF /I "%ENABLE_CPU_PROFILER%" == "1" (
   SET Defines=!Defines! "-D ENABLE_CPU_PROFILER"
)

IF /I "%ENABLE_DBGCALL%"=="1" (
   SET Includes=%Includes% -I"%RootDir%\quartz\sys\dbgcall\include"
   SET Defines=!Defines! "-D ENABLE_DBGCALL"
   SET Libs=!Libs! "%LibDir%\dbgcall.lib"
   SET Libs=!Libs! "%LibDir%\swintr.lib"
)

IF /I "%Ecosystem%" == "awsiot" (
   SET Defines=!Defines! "-D AWS_IOT"
)

IF /I "%Ecosystem%" == "azure" (
   echo Building Azure IOT SDK
   SET Defines=!Defines! "-D AZURE_IOT"
   SET Defines=!Defines! "-D DONT_USE_UPLOADTOBLOB"
   SET Defines=!Defines! "-D NO_LOGGING"
)

IF /I "%QMESH_PROVISIONER_DEMO%"=="true" (
	SET Defines=!Defines! "-D CONFIG_QMESH_PROVISIONER_DEMO"
)

IF /I "%QMESH_CLIENT_DEMO%"=="true" (
	SET Defines=!Defines! "-D CONFIG_QMESH_CLIENT_DEMO"
)

IF /I "%QMESH_SERVER_DEMO%"=="true" (
	SET Defines=!Defines! "-D CONFIG_QMESH_SERVER_DEMO"
)

SET ExtraLibs=

SET CFlags=%COpts% %Defines% %Includes% -D_WANT_IO_C99_FORMATS

SET LDFlags=-eSBL_Entry -no-wchar-size-warning --no-warn-mismatch -R"%SymFile%" -R"%SymFileUnpatched%" -T"%LINKFILE%" -Map="%OutDir%\%Project%.map" -n --gc-sections

IF /I "%Ecosystem%" == "awsiot" (
   echo "Building for AWS"
   SET LDFlags=!LDFlags! -L"%NEWLIBPATH%" -L"%TOOLLIBPATH%"
   SET ExtraLibs=!ExtraLibs! -lc_nano -lnosys -lgcc -u _scanf_float
) ELSE IF /I "%Ecosystem%" == "azure" (
   SET LDFlags=!LDFlags! -L"%NEWLIBPATH%" -L"%TOOLLIBPATH%" -u tlsio_template_interface_description -u tlsio_template_get_interface_description
   SET ExtraLibs=!ExtraLibs! -u _scanf_float -u _printf_float -lc -lnosys -lgcc -lm
   SET CThirdPartyOpts=-c -g -mcpu=cortex-m4 -mthumb -fno-short-enums -fno-exceptions -ffunction-sections -w -O2
   SET CThirdPartyFlags=!CThirdPartyOpts! !Defines! !Includes! -D_WANT_IO_C99_FORMATS
) 

REM Clean the output directory. Note the delay is to give Windows an opportunity to remove the old directory tree
RMDIR /s/q "%OutDir%" >NUL 2>&1
TIMEOUT /t 1          >NUL 2>&1
RMDIR /s/q "4020" >NUL 2>&1
MKDIR "%OutDir%"
MKDIR "%ObjDir%"

REM Clean up EDL files.
DEL /F "%SrcDir%\export\UsrEDL.c" >NUL 2>&1

SET Libs=!Libs! "%LibDir%\%CHIPSET_VARIANT%_%CHIPSET_REVISION%_%BOARD_VARIANT:~0,1%_NVM.lib"

IF "%NVM_FILE%" == "" (
   SET NVM_FILE=%NvmDir%\config\%CHIPSET_REVISION:~0,1%.%CHIPSET_REVISION:~2,1%\%CHIPSET_VARIANT:~3,4%\%BOARD_VARIANT%\%CHIPSET_VARIANT%_%CHIPSET_REVISION%.nvm
)

python %NvmDir%\tool\NVM2C.py -o %SrcDir%\export\UsrEDL.c -i %NVM_FILE%

if errorlevel 1 goto EndOfFile

:Prepare

ECHO Exporting Device config files....

MKDIR %SrcDir%\export >NUL 2>&1

robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem.h /XO >NUL 2>&1
robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem.c /XO >NUL 2>&1
robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem_som.c /XO >NUL 2>&1
robocopy %RootDir%\quartz\platform\export\ %SrcDir%\export platform_oem_mom.c /XO >NUL 2>&1

IF /I "%RTOS%" == "threadx" (
   robocopy %RootDir%\build\tools\devcfg\threadx\ %SrcDir%\export *.* /XO >NUL 2>&1
) else (
   robocopy %RootDir%\build\tools\devcfg\freertos\ %SrcDir%\export *.* /XO >NUL 2>&1
)

IF /I "%exitflag%" == "true" ( GOTO EndOfFile )

:Propgen
ECHO GENERATING DEVCFG....
DEL /F %SrcDir%\export\DALConfig_devcfg.c >NUL 2>&1
DEL /F %SrcDir%\export\devcfg_devcfg_data.c >NUL 2>&1
DEL /F %SrcDir%\export\DALConfig_fom.c >NUL 2>&1
DEL /F %SrcDir%\export\devcfg_fom_data.c >NUL 2>&1

IF /I "%BOARD_VARIANT%" == "CDB" (
REM CDB board uses "CDB" as BOARD_VARIANT. CDB board and carrier board use different DevCfg XML files.  
   python %RootDir%/build/tools/devcfg/propgen.py --XmlFile=%SrcDir%/export/DevCfg_master_devcfg_out_cdb.xml --DirName=%SrcDir%/export --ConfigFile=%SrcDir%/export/DALConfig_devcfg.c --DevcfgDataFile=%SrcDir%/export/devcfg_devcfg_data.c --ConfigType=%CHIPSET_VARIANT%_devcfg_xml
   python %RootDir%/build/tools/devcfg/propgen.py --XmlFile=%SrcDir%/export/DevCfg_master_fom_out_cdb.xml --DirName=%SrcDir%/export --ConfigFile=%SrcDir%/export/DALConfig_fom.c --DevcfgDataFile=%SrcDir%/export/devcfg_fom_data.c --ConfigType=%CHIPSET_VARIANT%_fom_xml
) ELSE (
python %RootDir%/build/tools/devcfg/propgen.py --XmlFile=%SrcDir%/export/DevCfg_master_devcfg_out.xml --DirName=%SrcDir%/export --ConfigFile=%SrcDir%/export/DALConfig_devcfg.c --DevcfgDataFile=%SrcDir%/export/devcfg_devcfg_data.c --ConfigType=%CHIPSET_VARIANT%_devcfg_xml
python %RootDir%/build/tools/devcfg/propgen.py --XmlFile=%SrcDir%/export/DevCfg_master_fom_out.xml --DirName=%SrcDir%/export --ConfigFile=%SrcDir%/export/DALConfig_fom.c --DevcfgDataFile=%SrcDir%/export/devcfg_fom_data.c --ConfigType=%CHIPSET_VARIANT%_fom_xml
)

REM Compile the source
FOR %%F IN (%CSrcs%) DO (
   ECHO Building %%F
   "%Compiler%" %CFlags% "-D __FILENAME__=\"%%~nF.c\"" -o"%ObjDir%\%%~nF.o" "%SrcDir%\%%F"
   if errorlevel 1 goto EndOfFile
   SET Libs=!Libs! "%ObjDir%\%%~nF.o"
)

FOR %%F IN (%CWallSrcs%) DO (
   ECHO Building %%F
   "%Compiler%" %CFlags% -Werror "-D __FILENAME__=\"%%~nF.c\"" -o"%ObjDir%\%%~nF.o" "%SrcDir%\%%F"
   if errorlevel 1 goto EndOfFile
   SET Libs=!Libs! "%ObjDir%\%%~nF.o"
)

REM Compile thirdparty source
FOR %%F IN (%CThirdPartySrcs%) DO (
   ECHO Building %%F
   "%Compiler%" %CThirdPartyFlags% "-D __FILENAME__=\"%%~nF.c\"" -o"%ObjDir%\%%~nF.o" "%SrcDir%\%%F"
   if errorlevel 1 goto EndOfFile
   SET Libs=!Libs! "%ObjDir%\%%~nF.o"
)

REM Generate the file with the libraries to link
IF EXIST %LIBSFILE% del %LIBSFILE%
IF EXIST %LINKFILE% del %LINKFILE%

FOR %%F IN (%Libs%) DO (
   ECHO %%F >>%LIBSFILE%
)

FOR %%F IN (%PatchObjs%) DO (
   ECHO %%F >>%LIBSFILE%
)

REM Convert paths to unix style
python fixpaths.py %LIBSFILE%

echo Generating Linker Scripts...
REM Update application PlacementFile
python %LinkerScriptDir%\CreateAppPlacementFile.py %RootDir%\bin\cortex-m4\%RTOS%\sys.placement %RootDir%\bin\cortex-m4\%RTOS%\cust.placement app.config app.placement 2>dbg.CreateApp
if %errorlevel% == 1 (
echo Failed to update application placement file. Check dbg.CreateApp for detail...
goto EndOfFile
)

REM Create a Quartz.ld linker script
python %LinkerScriptDir%\MakeLinkerScript.py %RootDir%\bin\cortex-m4\%RTOS%\DefaultTemplateLinkerScript.ld app.placement %LIBSFILE% > %LINKFILE% 2>dbg.Make
if %errorlevel% == 1 (
echo Failed to create linker script file. Check dbg.Make for detail...
goto EndOfFile
)


ECHO Linking...
   REM link the image
   "%Linker%" %LDFlags% --start-group @%LIBSFILE%  --end-group %ExtraLibs%  -o"%OutDir%\%Project%_nocompact.elf"

   REM Run the diag compaction script to generate the final ELF
   python %ScriptDir%\diagMsgCompact.py %OutDir%\%Project%.elf %RootDir%\bin\cortex-m4\diag_msg_QCLI_demo.strdb %OutDir%\%Project%_nocompact.elf %RootDir%/bin/cortex-m4/diag_msg.pkl Final > dictLog

if %errorlevel% == 1 goto EndOfFile

IF "%ENABLE_CPU_PROFILER%"=="1" (
    copy /y ..\..\..\..\net\tools\per_fn_cpu_profiling\update_uint32_symbol_value_by_name.py %ScriptDir%\update_uint32_symbol_value_by_name.py
    copy /y %OutDir%\%Project%.elf %OutDir%\%Project%_orig.elf
    del %OutDir%\%Project%.elf
    arm-none-eabi-nm -P %OutDir%\%Project%_orig.elf > %OutDir%\%Project%.nm.output
    findstr /i /r "\<t\>" %OutDir%\%Project%.nm.output > %OutDir%\%Project%.nm.output.functions
    for /f "usebackq" %%A in (`TYPE %OutDir%\%Project%.nm.output.functions ^| find /v /c "" `) do set demo_numlines=%%A
    for /f "usebackq" %%A in (`TYPE %SymFileUnpatched% ^| find /v /c "" `) do set libs_numlines=%%A
    set /a "numlines=%demo_numlines%+%libs_numlines%"
    python %ScriptDir%\update_uint32_symbol_value_by_name.py %OutDir%\%Project%_orig.elf %OutDir%\%Project%.elf g_cpu_profiler_number_of_functions %numlines%
)


REM Hash
ECHO Hashing...
python %SCRIPTDIR%\createxbl.py -f%OUTDIR%\%PROJECT%.elf -a32 -o%OUTDIR%\%PROJECT%_HASHED.elf
if /I "%SECBOOT%" == "true" (
   python %SectoolsDir%\sectools.py secimage -i %OUTDIR%\%PROJECT%.elf -c 4020_secimage.xml -sa -g m4 -o .
   python %SectoolsDir%\sectools.py secimage -i %RootDir%\bin\cortex-m0\threadx\ioe_ram_m0_threadx_ipt.mbn -c 4020_secimage.xml -sa -g m0 -o .
   python %SectoolsDir%\sectools.py secimage -i %RootDir%\bin\wlan\wlan_fw_img.bin -c 4020_secimage.xml -sa -g kf -o .
)
GOTO:EndOfFile

:Clobber
ECHO Deleting all temporary files ...
RMDIR /s/q "%OutDir%" >NUL 2>&1
RMDIR /s/q %SrcDir%\export >NUL 2>&1
GOTO:EndOfFile

:Usage
ECHO.
ECHO Usage: build.bat [RTOS (FreeRTOS = f, ThreadX = t)] [chipset variant (4020, 4024, 4025)] [board variant (Carrier = c, DUT = d, CDB = cdb)] [chipset revision (1.2 = 1p2, 2.0 = 2p0)] [NVM file]
GOTO:EndOfFile

:EndOfFile
