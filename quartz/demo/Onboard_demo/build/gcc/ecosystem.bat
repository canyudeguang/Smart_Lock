@ECHO OFF
rem
rem Copyright (c) 2018 Qualcomm Technologies, Inc.
rem All Rights Reserved.
rem Confidential and Proprietary - Qualcomm Technologies, Inc.
rem
 
IF /I "%1" == "" (
	set Ecosystem=
	GOTO Usage
) ELSE IF /I "%1" == "1" (
	ECHO ecosystem set to awsiot
	set Ecosystem=awsiot
	GOTO EndOfFile
) ELSE IF /I "%1" == "2" (
	ECHO ecosystem set to iotivity
	set Ecosystem=iotivity
	GOTO EndOfFile
) ELSE IF /I "%1" == "3" (
	ECHO ecosystem set to coap
	set Ecosystem=coap
	GOTO EndOfFile
) ELSE IF /I "%1" == "4" (
	ECHO ecosystem set to Azure
	set Ecosystem=azure
	GOTO EndOfFile
) ELSE IF /I "%1" == "5" (
	ECHO ecosystem set to offline
	set Ecosystem=offline
	GOTO EndOfFile
)

:Usage
ECHO Usage: ecosystem (1 = awsiot, 2 = iotivity, 3 = coap, 4 = azure, 5 = offline)

:EndOfFile
