

# ONE-TIME when starting gdb

target remote localhost:2331
#target extended-remote ubuntu-dsl04:3333
monitor speed auto
monitor endian little

set history filename output/gdb_history.log
set history save on
set print pretty on
set print object on
set print vtbl on
set pagination off
set output-radix 16

#Reset the chip
#set *(int*)0x50742094=4
#monitor reset 0
source v2/v2_substitute_path_m4.gdb



monitor jtagconf 4 1


#Load RAM symbols
symbol-file output/Quartz.elf
b SBL_Entry
#Load ROM symbols
add-symbol-file ../../../../../bin/cortex-m4/IOE_ROM_IPT_IMG_ARNNRI_patched_syms.elf 0x10000


b sbl1_main_ctl
b *jettison_core

#Set the cookie to allow boot past SBL
set *(int*)0x10000484=0xDADBEDAD


