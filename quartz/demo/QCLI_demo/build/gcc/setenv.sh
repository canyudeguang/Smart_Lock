#!/bin/bash
# Copyright (c) 2016-2017 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

FILE=$1
if [ -f "$FILE" ]; then
   echo Using $FILE
else
   FILE=env.config
   echo Using default configuration file $FILE
fi

while read LINE; do
	if [ "$LINE" ]; then

		if [[ "$LINE" =~ ^[[:space:]]+$ ]]; then
			continue
		fi
		if [[ "${LINE:0:1}" != "#" ]]; then
			export $LINE
			echo $LINE
		fi
	fi	
done < <(cat $FILE)

