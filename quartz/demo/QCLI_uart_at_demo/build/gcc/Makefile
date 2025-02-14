# Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
# 2016 Qualcomm Atheros, Inc.
# All Rights Reserved
# Confidential and Proprietary - Qualcomm Technologies, Inc.

# Allowed CHIPSET_VERSION - v1, v2
ifeq ($(CHIPSET_VERSION),)
   CHIPSET_VERSION = v2
endif

# Allowed CHIPSET_VARIANT - qca4020, qca4024, qca4025
ifeq ($(CHIPSET_VARIANT),)
   CHIPSET_VARIANT = qca4020
endif

ifeq ($(CHIPSET_VARIANT),qca4024)
   CHIPSET_VARIANT_CDB24 = true
endif

# Allowed RTOS - threadx, freertos
ifeq ($(RTOS),)
   RTOS = threadx
endif

# Allowed BOARD_VARIANT - carrier, dut, cdb
ifeq ($(BOARD_VARIANT),)
   BOARD_VARIANT = carrier
endif

# Allowed CHIPSET_REVISION - 1p2, 2p0
ifeq ($(CHIPSET_REVISION),)
    ifeq ($(CHIPSET_VERSION),v1)
        CHIPSET_REVISION = 1p2
    else
        CHIPSET_REVISION = 2p0
    endif
endif

# Setup the paths for the build
PROJECT          = Quartz
ROOTDIR          = ../../../../..
SRCDIR           = ../../src
NVMDIR           = $(ROOTDIR)/quartz/nvm
OUTDIR           = output
OBJDIR           = $(OUTDIR)/objs
SECTOOLSDIR      = $(ROOTDIR)/sectools
SECTOOLSQDNDIR   = $(ROOTDIR)/sectools/qdn
SECTOOLSCERTSDIR = $(SECTOOLSQDNDIR)/resources/data_prov_assets/Signing/Local/qc_presigned_certs-key2048_exp257
SECBOOT          = false
LIBDIR           = $(ROOTDIR)/lib/cortex-m4IPT/$(RTOS)
SYMFILE          = $(ROOTDIR)/bin/cortex-m4/IOE_ROM_IPT_IMG_ARNNRI_gcc.sym
SYMFILEUNPATCHED = $(ROOTDIR)/bin/cortex-m4/IOE_ROM_IPT_IMG_ARNNRI_orig_fcns_gcc.sym
SCRIPTDIR        = $(ROOTDIR)/build/scripts
LINKERSCRIPTDIR  = $(ROOTDIR)/build/scripts/linkerScripts
LINKFILE         = $(OUTDIR)/$(PROJECT).ld
LIBSFILE         = $(OUTDIR)/LinkerLibs.txt

ifeq ($(RTOS),threadx)
   OSLIB = threadx.lib
   LIBS  = "$(LIBDIR)/$(OSLIB)" \
           "$(LIBDIR)/mom_patch_table_ARNTRI_qcm.o" \
           "$(LIBDIR)/fom_patch_table_ARNTRI_qcm.o" \
           "$(LIBDIR)/som_patch_table_ARNTRI_qcm.o"
else
   OSLIB = free_rtos.lib
   LIBS  = "$(LIBDIR)/$(OSLIB)" \
           "$(LIBDIR)/mom_patch_table_ARNFRI_qcm.o" \
           "$(LIBDIR)/fom_patch_table_ARNFRI_qcm.o" \
           "$(LIBDIR)/som_patch_table_ARNFRI_qcm.o"
endif

ASSEMBLY_SRCS :=

# Sources to compile
CSRCS := qc_utils/src/qc_sbrk.c \
	qc_api/src/qc_api_main.c \
	qc_drv/src/qc_drv_main.c \
	qc_drv/src/qc_drv_ble.c \
	qc_utils/src/qc_util.c \
	qc_at/src/qc_at_ble.c \
	qc_at/src/qc_at_ble_service.c \
	qc_at/src/qc_at_zigbee.c \
	qc_at/src/qc_at_zcl.c \
	qc_at/src/clusters/qc_at_zcl_alarms_demo.c \
	qc_at/src/clusters/qc_at_zcl_basic_demo.c \
	qc_at/src/clusters/qc_at_zcl_colorcontrol_demo.c \
	qc_at/src/clusters/qc_at_zcl_custom_demo.c \
	qc_at/src/clusters/qc_at_zcl_devicetemp_demo.c \
	qc_at/src/clusters/qc_at_zcl_groups_demo.c \
	qc_at/src/clusters/qc_at_zcl_identify_demo.c \
	qc_at/src/clusters/qc_at_zcl_levelcontrol_demo.c \
	qc_at/src/clusters/qc_at_zcl_onoff_demo.c \
	qc_at/src/clusters/qc_at_zcl_powerconfig_demo.c \
	qc_at/src/clusters/qc_at_zcl_scenes_demo.c \
	qc_at/src/clusters/qc_at_zcl_time_demo.c \
	qc_at/src/clusters/qc_at_zcl_touchlink_demo.c \
	qc_at/src/clusters/qc_at_zcl_ota_demo.c \
	qc_at/src/qc_at_thread.c \
	qc_api/src/qc_api_thread.c \
	qc_drv/src/qc_drv_thread.c \
	qc_api/src/qc_api_zigbee.c \
	qc_drv/src/qc_drv_zigbee.c \
	qc_at/src/net/multi_sock_test.c \
	qc_at/src/net/net_ssl_demo.c \
	qc_at/src/net/net_bench_ssl.c \
	qc_at/src/net/net_utils.c \
	qc_at/src/net/net_sock.c \
	qc_at/src/net/net_bench.c \
	qc_at/src/net/net_bench_raw.c \
	qc_at/src/net/net_bench_tcp.c \
	qc_at/src/net/net_bench_udp.c \
	qc_at/src/net/net_bench_uapsd.c \
	qc_at/src/net/net_cert.c \
	qc_at/src/net/net_iperf.c \
	qc_at/src/net/net_http_client.c \
	qc_at/src/net/httpsvr/cgi/net_htmldata.c \
	qc_at/src/net/httpsvr/cgi/net_cgi_showintf.c \
	qc_at/src/net/httpsvr/cgi/net_cgi_demo.c \
	qc_at/src/qc_at_net.c \
	qc_api/src/qc_api_net.c \
	qc_drv/src/qc_drv_net.c \
	drivers/src/driver.c \
	qosa/src/qosa_util.c \
	lp/lp_demo.c \
	lp/fom_lp_test.c \
	lp/som_lp_test.c \
	lp/mom_lp_test.c \
	qcli/qcli.c \
	qcli/qcli_util.c \
	qcli/pal.c

ifeq ($(CHIPSET_VARIANT),qca4020)
CSRCS += qc_at/src/ota/qc_at_wifi_ota.c \
	qc_at/src/qc_at_wifi_demo.c \
	qc_api/src/qc_api_wifi.c \
	qc_at/src/qc_at_mqtt.c \
	qc_drv/src/qc_drv_mqtt.c \
	qc_drv/src/qc_drv_wifi.c \
	qc_drv/src/qc_drv_p2p.c \
	qc_at/src/ota/plugins/qc_ftp/qc_at_ota_ftp.c \
	qc_at/src/ota/plugins/qc_http/qc_at_ota_http.c \
	qc_api/src/ota/qc_api_wifi_ota.c \
	qc_drv/src/qc_drv_wifi_ota.c \
	qc_api/src/qc_api_mqtt.c
endif

CSRCS += export/platform_oem.c \
         export/DALConfig_devcfg.c \
         export/platform_oem_som.c \
         export/platform_oem_mom.c \
         export/DALConfig_fom.c \
         export/devcfg_devcfg_data.c \
         export/devcfg_fom_data.c \
         export/UsrEDL.c

# Include directories
INCLUDES := -I"$(ROOTDIR)/include" \
	-I"$(ROOTDIR)/include/qapi" \
	-I"$(ROOTDIR)/include/bsp" \
	-I"$(SRCDIR)" \
	-I"$(SRCDIR)/drivers/include" \
	-I"$(SRCDIR)/qcli" \
	-I"$(SRCDIR)/qc_at/include" \
	-I"$(SRCDIR)/qc_at/include/ota" \
	-I"$(SRCDIR)/qc_at/include/net" \
	-I"$(SRCDIR)/qc_at/include/clusters" \
	-I"$(SRCDIR)/qc_api/include" \
	-I"$(SRCDIR)/qc_drv/include" \
	-I"$(SRCDIR)/qc_utils/include" \
	-I"$(SRCDIR)/qosa/include" \

# External objects and libraries
LIBS += "$(LIBDIR)/core.lib" \
        "$(LIBDIR)/qurt.lib" \
        "$(LIBDIR)/quartzplatform.lib" \
        "$(LIBDIR)/quartzplatform_xip.lib" \
        "$(LIBDIR)/WLAN.lib" \
        "$(LIBDIR)/WLAN_PROFILER.lib" \
        "$(LIBDIR)/net.lib" \
        "$(LIBDIR)/net_ram.lib" \
        "$(LIBDIR)/dhcpv6c.lib" \
        "$(LIBDIR)/sntpc.lib" \
        "$(LIBDIR)/dnssrvr.lib" \
        "$(LIBDIR)/sharkssl.lib" \
        "$(LIBDIR)/cryptolib.lib" \
        "$(LIBDIR)/httpsvr.lib" \
        "$(LIBDIR)/httpc.lib" \
        "$(LIBDIR)/mqttc.lib" \
        "$(LIBDIR)/vfs.lib" \
        "$(LIBDIR)/userpass.lib" \
        "$(LIBDIR)/i2s.lib" \
        "$(LIBDIR)/qapi_ed25519.lib" \
        "$(LIBDIR)/qapi_securefs.lib" \
        "$(LIBDIR)/pka_port.lib" \
        "$(LIBDIR)/fs_helper.lib" \
        "$(LIBDIR)/quartz_crypto_qapi.lib" \
        "$(LIBDIR)/zigbee.lib" \
        "$(LIBDIR)/quartz_zigbee.lib" \
        "$(LIBDIR)/mdns.lib" \
        "$(LIBDIR)/BLUETOPIA_SERVICES.lib" \
        "$(LIBDIR)/BLUETOPIA_QAPI_SERVICES.lib"   \
        "$(LIBDIR)/dnssd.lib" \
        "$(LIBDIR)/otp_tlv.lib" \
        "$(LIBDIR)/base64.lib" \
        "$(LIBDIR)/PERSIST_M4.lib" \
        "$(LIBDIR)/json.lib" \
        "$(LIBDIR)/json_qapi.lib" \
		"$(LIBDIR)/nichestack.lib" \
		"$(LIBDIR)/master_sdcc.lib" \
		"$(LIBDIR)/fwup.lib" \
		"$(LIBDIR)/fwup_engine.lib" \
		"$(LIBDIR)/csr.lib" \
		"$(LIBDIR)/thread.lib" \
		"$(LIBDIR)/qapi_thread.lib" \

ifeq ($(CHIPSET_VARIANT),qca4020)
    LIBS += "$(LIBDIR)/WLAN_QAPI.lib"\
	        "$(LIBDIR)/CUST_IPSTACK_INICHE.lib"
endif

PATCHOBJS :=

ifeq ($(CHIPSET_VERSION),v1)
   LIBS += "$(LIBDIR)/pka.lib" \
           "$(LIBDIR)/homekit.lib" \
           "$(LIBDIR)/v1core.lib" \
           "$(LIBDIR)/net_v1.lib" \
           "$(LIBDIR)/i15p4_hmi.lib" \
           "$(LIBDIR)/OSAL_M4.lib" \

   PATCHOBJS += "$(LIBDIR)/patch.lib"
   INCLUDES  += -I"$(ROOTDIR)/core/v1"
else
   # In v1 these are in ROM, moved to XIP for v2
   LIBS += "$(LIBDIR)/tlv_transport.lib" \
           "$(LIBDIR)/crypto_port.lib" \
           "$(LIBDIR)/tee_master.lib" \
           "$(LIBDIR)/dnsclient.lib" \
           "$(LIBDIR)/securefs.lib" \
           "$(LIBDIR)/securefs_port.lib" \
           "$(LIBDIR)/v2core.lib" \
           "$(LIBDIR)/EDLManager.lib" \

   PATCHOBJS += "$(LIBDIR)/patch.lib"
   INCLUDES  += -I"$(ROOTDIR)/core/v2"
endif

ifeq ($(QMESH),true)
   LIBS += "$(LIBDIR)/qmesh.lib"
endif

# Setup the build variables
CC = arm-none-eabi-gcc
AR = arm-none-eabi-ar
LD = arm-none-eabi-ld

COPTS := -c -g -mcpu=cortex-m4 -mthumb -fno-short-enums -O2 -ffunction-sections -Wall
DEFINES :=-D WLAN_DEBUG
DEFINES :=-D ENABLE_P2P_MODE

DEFINES += "-D ENABLE_LOG"

ifeq ($(BOARD_VARIANT),cdb)
DEFINES += "-D CONFIG_CDB_PLATFORM"
endif

ifeq ($(CHIPSET_VARIANT_CDB24),true)
DEFINES += "-D CONFIG_PLATFORM_CDB24"
endif

ifeq ($(CHIPSET_VERSION),v1)
   DEFINES += "-D V1"
else
   DEFINES += "-D V2" \
              "-D qurt_mutex_init(x)=qurt_mutex_create(x)" \
              "-D qurt_mutex_destroy(x)=qurt_mutex_delete(x)" \
              "-D qurt_signal_init(x)=qurt_signal_create(x)" \
              "-D qurt_signal_destroy(x)=qurt_signal_delete(x)" \
              "-D FEATURE_QUARTZ_V2"
endif

# Add the correct NVM library and user NVM file.
LIBS += $(LIBDIR)/QCA$(subst qca,,$(CHIPSET_VARIANT))_$(CHIPSET_REVISION)_$(subst dut,D,$(subst carrier,C,$(subst cdb,CDB,$(BOARD_VARIANT))))_NVM.lib

ifndef NVM_FILE
   NVM_FILE = $(NVMDIR)/config/$(subst p,.,$(CHIPSET_REVISION))/$(subst qca,,$(CHIPSET_VARIANT))/$(subst dut,DUT,$(subst carrier,Carrier,$(subst cdb,CDB,$(BOARD_VARIANT))))/QCA$(subst qca,,$(CHIPSET_VARIANT))_$(CHIPSET_REVISION).nvm
endif

CFLAGS         = $(COPTS) $(DEFINES) $(INCLUDES) -D_WANT_IO_C99_FORMATS
LDOpts        := -eSBL_Entry -no-wchar-size-warning --no-warn-mismatch -R"$(SYMFILE)" -R"$(SYMFILEUNPATCHED)" -T"$(LINKFILE)" -Map="$(OUTDIR)/$(PROJECT).map" -n --gc-sections
LDFLAGS        = $(LDOpts) --start-group @$(LIBSFILE) --end-group
OBJS          := $(CSRCS:%.c=$(OBJDIR)/%.o)
OBJS          += $(CThirdPartySrcs:%.c=$(OBJDIR)/%.thirdparty_o)
	ASSEMBLY_OBJS := $(ASSEMBLY_SRCS:%.S=$(OBJDIR)/%.asm_o)
	DEPS          := $(CSRCS:%.c=%.d)
	ExtraLibs     :=


all: prepare propgen nvm $(OBJS) $(ASSEMBLY_OBJS)
	-rm -rf $(LIBSFILE)
	-rm -rf $(LINKFILE)


	$(foreach f,$(LIBS),echo $f>>$(LIBSFILE);)
	$(foreach f,$(PATCHOBJS),echo $f>>$(LIBSFILE);)
	$(foreach f,$(OBJS),echo $(OBJDIR)/$(notdir $(f))>>$(LIBSFILE);)
	$(foreach f,$(ASSEMBLY_OBJS),echo $(OBJDIR)/$(notdir $(f))>>$(LIBSFILE);)

# Update application PlacementFile
	python $(LINKERSCRIPTDIR)/CreateAppPlacementFile.py $(ROOTDIR)/bin/cortex-m4/$(RTOS)/sys.placement $(ROOTDIR)/bin/cortex-m4/$(RTOS)/cust.placement app.config app.placement 2>dbg.CreateApp

# Create a Quartz.ld linker script
	python $(LINKERSCRIPTDIR)/MakeLinkerScript.py $(ROOTDIR)/bin/cortex-m4/$(RTOS)/DefaultTemplateLinkerScript.ld app.placement $(LIBSFILE) > $(LINKFILE) 2>dbg.Make

	mkdir -p $(OBJDIR)
	@echo Linking....
ifeq ($(CHIPSET_VERSION),v1)
	$(LD) $(LDFLAGS) -o"$(OUTDIR)/$(PROJECT).elf"
else
	$(LD) $(LDFLAGS) -o"$(OUTDIR)/$(PROJECT)_nocompact.elf"
	# Run the diag compaction script to generate the final ELF
	@echo DIAG Message Compaction...
	python $(SCRIPTDIR)/diagMsgCompact.py $(OUTDIR)/$(PROJECT).elf $(ROOTDIR)/bin/cortex-m4/diag_msg_QCLI_demo.strdb $(OUTDIR)/$(PROJECT)_nocompact.elf $(ROOTDIR)/bin/cortex-m4/diag_msg.pkl Final > dictLog
endif

	@echo Hashing...
	python $(SCRIPTDIR)/createxbl.py -f$(OUTDIR)/$(PROJECT).elf -a32 -o$(OUTDIR)/$(PROJECT)_HASHED.elf
ifeq ($(SECBOOT),true)
	@echo Signing Images...
ifeq ($(BOARD_VARIANT),cdb)
	python $(SECTOOLSQDNDIR)/sectools.py iot -p 4020 -g m4 -i $(OUTDIR)/$(PROJECT).elf -k $(SECTOOLSCERTSDIR)/qpsa_rootca.key -c $(SECTOOLSCERTSDIR)/qpsa_rootca.cer --cfg_oem_id=0x0000 --cfg_model_id=0x0000 -o . -s
	python $(SECTOOLSQDNDIR)/sectools.py iot -p 4020 -g m0 -i $(ROOTDIR)/bin/cortex-m0/threadx/ioe_ram_m0_threadx_ipt.mbn -k $(SECTOOLSCERTSDIR)/qpsa_rootca.key -c $(SECTOOLSCERTSDIR)/qpsa_rootca.cer --cfg_oem_id=0x0000 --cfg_model_id=0x0000 -o . -s
	python $(SECTOOLSQDNDIR)/sectools.py iot -p 4020 -g kf -i $(ROOTDIR)/bin/wlan/wlan_fw_img.bin -k $(SECTOOLSCERTSDIR)/qpsa_rootca.key -c $(SECTOOLSCERTSDIR)/qpsa_rootca.cer --cfg_oem_id=0x0000 --cfg_model_id=0x0000 -o . -s
else
	python $(SECTOOLSDIR)/sectools.py secimage -i $(OUTDIR)/$(PROJECT).elf -c 4020_secimage.xml -sa -g m4 -o .
	python $(SECTOOLSDIR)/sectools.py secimage -i $(ROOTDIR)/bin/cortex-m0/threadx/ioe_ram_m0_threadx_ipt.mbn -c 4020_secimage.xml -sa -g m0 -o .
	python $(SECTOOLSDIR)/sectools.py secimage -i $(ROOTDIR)/bin/wlan/wlan_fw_img.bin -c 4020_secimage.xml -sa -g kf -o .
endif
endif

	@echo Build Completed...

nvm:
	-rm -rf $(SRCDIR)/export/UsrEDL.c
	python $(NVMDIR)/tool/NVM2C.py -o $(SRCDIR)/export/UsrEDL.c -i $(NVM_FILE)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo Compiling $< $@
	$(CC) $(CFLAGS) -D __FILENAME__=\"$<\" $< -o $(OBJDIR)/$(@F)

$(OBJDIR)/%.thirdparty_o: $(SRCDIR)/%.c
	@mkdir -p $(dir $@)
	@echo Compiling $< $@
	$(CC) $(CThirdPartyFlags) -D __FILENAME__=\"$<\" $< -o $(OBJDIR)/$(@F)

$(OBJDIR)/%.asm_o: $(SRCDIR)/%.S
	@mkdir -p $(dir $@)
	@echo Compiling $< $@
	$(CC) $(CFLAGS) -D __FILENAME__=\"$<\" $< -o $(OBJDIR)/$(@F)

clean:
	rm -rf $(OUTDIR)

clobber:
	rm -rf $(SRCDIR)/export
	rm -rf $(SRCDIR)/UsrEDL.c
	rm -rf $(OUTDIR)

prepare:
ifeq ($(CHIPSET_VERSION),v1)
	@echo \##############################################################################
	@echo    Starting build for Quartz UART AT Command Application for v1 Chipset
	@echo                  To debug v2, set CHIPSET_VERSION=v2
	@echo \##############################################################################
else
	@echo \##############################################################################
	@echo    Starting build for Quartz UART AT Command Application for v2 Chipset
	@echo                  To debug v1, set CHIPSET_VERSION=v1
	@echo \##############################################################################
endif

	@echo Build Parameters:
	@echo RTOS		= $(RTOS)
	@echo Variant	= $(CHIPSET_VARIANT)
	@echo Board		= $(BOARD_VARIANT)
	@echo Revision	= $(CHIPSET_REVISION)
	@echo \##############################################################################

	@mkdir -p $(SRCDIR)/export
	cp -u -v $(ROOTDIR)/quartz/platform/export/platform_oem.h $(SRCDIR)/export
	cp -u -v $(ROOTDIR)/quartz/platform/export/platform_oem.c $(SRCDIR)/export
	cp -u -v $(ROOTDIR)/quartz/platform/export/platform_oem_mom.c $(SRCDIR)/export
	cp -u -v $(ROOTDIR)/quartz/platform/export/platform_oem_som.c $(SRCDIR)/export

ifeq ($(BOARD_VARIANT),cdb)
	cp -u -v $(ROOTDIR)/build/tools/devcfg/$(RTOS)/DevCfg_master_devcfg_out_cdb.xml $(SRCDIR)/export
	cp -u -v $(ROOTDIR)/build/tools/devcfg/$(RTOS)/DevCfg_master_fom_out_cdb.xml $(SRCDIR)/export
else
	cp -u -v $(ROOTDIR)/build/tools/devcfg/$(RTOS)/DevCfg_master_devcfg_out.xml $(SRCDIR)/export
	cp -u -v $(ROOTDIR)/build/tools/devcfg/$(RTOS)/DevCfg_master_fom_out.xml $(SRCDIR)/export
endif

propgen:
	@echo GENERATING DEVCFG....
	rm -f $(SRCDIR)/export/DALConfig_devcfg.c
	rm -f $(SRCDIR)/export/devcfg_devcfg_data.c
	rm -f $(SRCDIR)/export/DALConfig_fom.c
	rm -f $(SRCDIR)/export/devcfg_fom_data.c
	python $(ROOTDIR)/build/tools/devcfg/propgen.py --XmlFile=$(SRCDIR)/export/DevCfg_master_devcfg_out$(subst dut,,$(subst carrier,,$(subst cdb,_cdb,$(BOARD_VARIANT)))).xml --DirName=$(SRCDIR)/export --ConfigFile=$(SRCDIR)/export/DALConfig_devcfg.c --DevcfgDataFile=$(SRCDIR)/export/devcfg_devcfg_data.c --ConfigType=$(CHIPSET_VARIANT)_devcfg_xml
	python $(ROOTDIR)/build/tools/devcfg/propgen.py --XmlFile=$(SRCDIR)/export/DevCfg_master_fom_out$(subst dut,,$(subst carrier,,$(subst cdb,_cdb,$(BOARD_VARIANT)))).xml --DirName=$(SRCDIR)/export --ConfigFile=$(SRCDIR)/export/DALConfig_fom.c --DevcfgDataFile=$(SRCDIR)/export/devcfg_fom_data.c --ConfigType=$(CHIPSET_VARIANT)_fom_xml
