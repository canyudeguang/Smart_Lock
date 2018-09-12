###########################################################
##### Root of Trust Management Tool #######################
###########################################################

Purpose: QCA402X modules can be provisioned with four root certificates at a given time. The purpose of this tool is to revoke or activate specific root(s) using KDF functionality.

Dependencies: 
1. The tool works in conjunction with KDF Password Generator tool provided in the SDK.
2. Secure boot must be enabled.
3. The chip should be configured for Multiple-root-of-trust (via OTP fuses). 

Usage:  To use this tool, following steps should be followed-

1. Generate KDF password using the password generator. Edit the input xml file based on your requirements (e.g. activation/revocation vector values, Op-Code- 7 (Activation), 8 (Revocation)). A different password is generated for activation and revocation operations.

2. Edit the rot_params.h file under src/app. Plug in the newly generated passwords and user input values. The "activate_input[]" and "revoc_input[]" values must match the user input used in the xml input file to the password generator.

3. Build the ROT tool. Navigate to ROT/build/gcc and run build.bat. Make sure the image is signed with a root that is currently active. This is specified by the "mrc_index" field in 4020_secimage.xml. A value of 0 indicates 1st root is used to sign the image, a value of 3 means 4th root is used to sign the image.

4. The signed elf file (ROT.elf) will be generated in build/gcc/4020/m4 directory. This file can now be:
	a. Flashed via USB or JTAG
	b. Added to firmware upgrade image bundle and downloaded over the air.
	
5. If the image is added to Firmware Upgrade bundle, then it must be assigned an ID of 10, and should be placed before the M4 ELF image. This will allow the boot loader to load the ROT image before all other images. 

6. If the image succeeds in the ROT operation, it will "invalidate" itself and trigger a reset, which will then force the boot loader to boot the original M4 application image. If the image fails (e.g. due to misconfigured parameters), the entire trial image set will be invalidated, and boot loader will load the current image set.

Please refer to QCA402X Security guide for more details.