@REM Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
@REM All Rights Reserved.
@REM Confidential and Proprietary - Qualcomm Technologies, Inc.

@ECHO OFF


IF EXIST "env.config" (
	for /f "eol=# delims=" %%i in (env.config) do (	
		@set %%i
		@echo Setting %%i

	)
) ELSE (
	echo ERROR: File env.config does not exist
)