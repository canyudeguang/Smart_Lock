# Copyright (c) 2016 Qualcomm Atheros, Inc.
# All Rights Reserved.
# Qualcomm Atheros Confidential and Proprietary.
# $ATH_LICENSE_TARGET_MK$
#

# Allowed CHIPSET_VERSION - v2
ifeq ($(CHIPSET_VERSION),)
   CHIPSET_VERSION = v2
endif

# Setup the paths for the build
ROOTDIR          = ../../../..
SRCDIR           = ../..
OUTDIR           = output
OBJDIR           = $(OUTDIR)/objs
LIBFILE         = $(OUTDIR)/fwup_engine.lib

# Sources to compile
CSRCS := fw_upgrade/fw_upgrade.c

# Include directories
INCLUDES := -I"$(ROOTDIR)/include" \
            -I"$(ROOTDIR)/include/qapi" \
            -I"$(ROOTDIR)/include/bsp" \
            -I"$(SRCDIR)/fw_upgrade"

# Setup the build variables
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar

COPTS := -c -g -mcpu=cortex-m4 -mthumb -fno-short-enums -O2 -Wall

ifeq ($(CHIPSET_VERSION),v2)
   DEFINES = "-D V2" \
              "-D qurt_mutex_init(x)=qurt_mutex_create(x)" \
              "-D qurt_mutex_destroy(x)=qurt_mutex_delete(x)" \
              "-D qurt_signal_init(x)=qurt_signal_create(x)" \
              "-D qurt_signal_destroy(x)=qurt_signal_delete(x)" \
              "-D FEATURE_QUARTZ_V2"
endif

CFLAGS        = $(COPTS) $(DEFINES) $(INCLUDES)
OBJS          := $(CSRCS:%.c=$(OBJDIR)/%.o)
DEPS          := $(CSRCS:%.c=%.d)

all: $(OBJS) 
	-rm -rf $(LIBFILE)
	mkdir -p $(OBJDIR)
	$(AR) rcs $(LIBFILE) $(OBJS)

	@echo Build Completed...

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo Compiling $< $@
	$(CC) $(CFLAGS) -D __FILENAME__=\"$<\" $< -o $@

clean:
	rm -rf $(OUTDIR)




