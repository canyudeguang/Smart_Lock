#!/bin/bash

# Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
# 2016 Qualcomm Atheros, Inc.
# All Rights Reserved
# Confidential and Proprietary - Qualcomm Technologies, Inc.


sudo fuser -k 3333/tcp
openocd -f qca402x_openocd.cfg &                   
var="$(pgrep -f openocd)"                                                       
#echo "$var"  
cd output
mkdir -p gdbout

ROOTDIR=$(cd ../../../../../..;pwd)
SCRIPTDIR=$ROOTDIR/build/tools/flash

M4_DIR=${M4_DIR:-$ROOTDIR/quartz/demo/Helloworld_demo/build/gcc/output}
M4_IMAGE=${M4_IMAGE:-$M4_DIR/Quartz_HASHED.elf}

M0_DIR=${M0_DIR:-$ROOTDIR/bin/cortex-m0/threadx}
M0_IMAGE=${M0_IMAGE:-$M0_DIR/ioe_ram_m0_threadx_ipt.mbn}

# Set WLAN_IMAGE=none (or any non-existent filename) to avoid programming a WLAN image.
WLAN_DIR=${WLAN_DIR:-$ROOTDIR/bin/wlan}
WLAN_IMAGE=${WLAN_IMAGE:-$WLAN_DIR/wlan_fw_img.bin}

GDB_PATH=${GDB_PATH:-`which arm-none-eabi-gdb`}
#GDB_SERVER_PATH=${GDB_SERVER_PATH:-`which JLinkGDBServer`}

GDB_SERVER=${GDB_SERVER:-localhost}
GDB_PORT=${GDB_PORT:-3333}

# Create partition_table.xml
if [ -e $WLAN_IMAGE ]
then
#    python $SCRIPTDIR/gen_part_table.py \
#	    --begin=3BLK \
#	    --file=$M4_IMAGE \
#	    --file=$M0_IMAGE \
#	    --file=$WLAN_IMAGE 
    python $SCRIPTDIR/gen_part_table.py \
	    --begin=140KB --partition --file=$M4_IMAGE \
        --partition --file=$M0_IMAGE \
        --partition --file=$WLAN_IMAGE \
        --partition --id=FS1 --size=64KB --start=12KB \
        --partition --id=FS2 --size=64KB --start=76KB \
        --partition --id=UNUSED --size=8KB --start=4KB               
else
#    python $SCRIPTDIR/gen_part_table.py \
#	    --begin=3BLK \
#	    --file=$M4_IMAGE \
#	    --file=$M0_IMAGE
    python $SCRIPTDIR/gen_part_table.py \
	    --begin=140KB --partition --file=$M4_IMAGE \
        --partition --file=$M0_IMAGE \
        --partition --id=FS1 --size=64KB --start=12KB \
        --partition --id=FS2 --size=64KB --start=76KB \
        --partition --id=UNUSED --size=8KB --start=4KB               
fi

if [ $? -ne 0 ]
then
    echo Abort flash.sh: gen_part_table.py failed
    exit 1
fi

# Convert to fwd_table.xml
python $SCRIPTDIR/gen_fwd_table.py \
	-x generated_partition_table.xml \
	--rawprogram generated_fwd_table.xml

if [ $? -ne 0 ]
then
    echo Abort flash.sh: gen_fwd_table.py failed
    exit 1
fi

python $SCRIPTDIR/flash_through_gdb.py \
	--rawprogram=generated_fwd_table.xml \
	--verbose=5 \
	--verify \
	--outputdir=gdbout \
	--gdbport=$GDB_PORT \
	--gdbpath=$GDB_PATH \
	--jtagprgelf=$SCRIPTDIR/JTAGPROGRAMMER_IMG_ARNTRI.elf \
	--search_path=$M4_DIR,$M0_DIR,$WLAN_DIR \
	--gdbserver $GDB_SERVER:$GDB_PORT

if [ $? -ne 0 ]
then
    echo Abort flash.sh: flash_through_gdb.py failed
	sleep 1
	kill $var
    exit 1
fi
echo " Finished Flashing"
sleep 2
kill $var
echo "Disconnected Openocd"

exit 0
