#!/bin/bash
# Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
# All Rights Reserved.
# Confidential and Proprietary - Qualcomm Technologies, Inc.

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
done < <(cat env.config)

