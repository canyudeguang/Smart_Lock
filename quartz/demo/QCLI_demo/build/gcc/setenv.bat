@REM Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
@REM All Rights Reserved.
@REM Confidential and Proprietary - Qualcomm Technologies, Inc.

@ECHO OFF

IF "%~1" == "" (
SET cfg=env.config
) ELSE (
SET cfg=%~1
)

IF EXIST "%cfg%" (
	for /f "eol=# delims=" %%i in (%cfg%) do (	
		@set %%i
		@echo Setting %%i

	)
) ELSE (
	echo ERROR: File env.config does not exist
)