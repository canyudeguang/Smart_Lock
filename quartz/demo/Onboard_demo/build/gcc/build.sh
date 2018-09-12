# Copyright (c) 2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

#!/bin/bash
# Script used to select the parameters,
#   1) ONBOARD_VIA - onboard the CDB20 via WIFI or BLE
#   2) RADIO - choose the CDB20 to onboard using Thread or Zigbee
#   3) MODE - choose the mode of CDB20 operation
#       a. Zigbee mode - Co-ordinator or Enddevice 
#       b. Thread mode - Border Router or Joiner

ONBOARD_VIA=$1
RADIO=$2
MODE=$3

Online_demo()
{
	echo "\nOnline demo build script help:"
	echo "\n Example sh build.sh 1 1 1"
	echo "\n Select the options for Onboarding mode, Radio, Supported mode as below"
	echo "\n Onboarding mode:\n   1.BLE 2.WIFI"
	echo "\n Radio:\n   1.ZIGBEE 2.THREAD"
	echo "\n Zigbee Supported mode:\n   1.COORDINATOR 2.ENDDEVICE\n"
	echo "\n Thread Supported mode:\n   1.BORDER ROUTER 2.JOINER"
	exit 1
}

Offline_demo()
{
        echo "\nOffline demo build script help:"
        echo "\n Example sh build.sh 1 1"
        echo "\n Onboarding mode:\n   1.BLE"
        echo "\n Radio:\n   1.ZIGBEE\n"
        exit 1
}

if [ ! "$ECOSYSTEM" ]; then
        echo " \n Please Export the ECOSYSTEM before building the demo\n"
        exit 1
fi

if [ "$#" -ne 2 ]; then
        if [ "$ECOSYSTEM" = 'offline' ]; then
                Offline_demo
                exit 1
	fi
fi

if [ "$#" -ne 3 ]; then
        if [ "$ECOSYSTEM" = 'awsiot' ]; then
                Online_demo
                exit 1
        fi
fi


if [ "$ONBOARD_VIA" -eq 1 ]; then
	export ONBOARD_VIA=BLE
	echo "\n Selected Onboarding mode is $ONBOARD_VIA"
elif [ "$ONBOARD_VIA" -eq 2 ]; then
	export ONBOARD_VIA=WIFI
	echo "\n Selected Onboarding mode is $ONBOARD_VIA"
else
	echo "\n Incorrect Onboarding mode"
	if [ "$ECOSYSTEM" = 'offline' ]; then
		Offline_demo
	else
		Online_demo
	fi
fi

if [ "$RADIO" -eq 1 ]; then
	export RADIO=ZIGBEE
	echo "\n Selected Radio is $RADIO"
elif [ "$RADIO" -eq 2 ]; then
	export RADIO=THREAD
	echo "\n Selected Radio is $RADIO"
else
	echo "\n Incorrect mode of Radio"
	if [ "$ECOSYSTEM" = 'offline' ]; then
		Offline_demo
	else
		Online_demo
	fi
	exit 1
fi

if [ "$ECOSYSTEM" = 'offline' ]; then
	if [ "$ONBOARD_VIA" = 'WIFI' ]; then
		echo "\n Offline demo supports BLE Onboarding mode Only \n"
		Offline_demo
		exit 1
	fi
	if [ "$RADIO" = 'THREAD' ]; then
		echo "\n Offline demo supports ZIGBEE RADIO Only \n"
		Offline_demo
		exit 1
	else
	        export MODE=5
	fi
	
elif [ "$ECOSYSTEM" = 'awsiot' ]; then
    if [ "$RADIO" = 'ZIGBEE' ]; then
	    if [ "$MODE" -eq 1 ]; then
		    export MODE=4
    		echo "\n Selected zigbee mode is COORDINATOR"
	    elif [ "$MODE" -eq 2 ]; then
    		export MODE=1
	    	echo "\n Selected zigbee mode is ENDDEVICE"
    	else
	    	echo "\n Online Incorrect mode of Zigbee"
		    Online_demo
		    exit 1
	    fi

    elif [ "$RADIO" = 'THREAD' ]; then
	    if [ "$MODE" -eq 1 ]; then
		    export MODE=4
    		echo "\n Selected thread mode is BORDER ROUTER"
	    elif [ "$MODE" -eq 2 ]; then
		    export MODE=1
    		echo "\n Selected thread mode is JOINER"
	    else
		    echo "\n Incorrect mode of Thread"
    		Online_demo
	    	exit 1
        fi
    fi
fi

echo "\n $ECOSYSTEM Demo Building started\n"
sleep 3
make clean all
