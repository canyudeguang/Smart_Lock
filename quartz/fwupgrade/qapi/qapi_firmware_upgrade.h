/* Copyright (c) 2011-2017  Qualcomm Technologies, Inc.
 * All rights reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

 /** @file qapi_firmware_upgrade.h
* @addtogroup qapi_Fw_Upgrade
 * @{
 *
 *  @details The firmware Upgrade module provides a framework for over-the-air image upgrades.
 *  The framework uses a plugin-based architecture, where a developer can implement and 
 *  insert a custom plugin or use existing reference plugins. The transport plugin interacts
 *  with the core firmware upgrade engine using clearly defined callback APIs. 
 *
 *
 * @}
*/

#ifndef _QAPI_FIRMWARE_UPGRADE_H_
#define _QAPI_FIRMWARE_UPGRADE_H_

/*----------------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------------*/

 /*----------------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------------*/ 

 /** @addtogroup qapi_Fw_Upgrade
@{ */
 
/**
 *  Definition used by the qapi_Fw_Upgrade() and qapi_Fw_Upgrade_done() APIs as a flag bit.
 *  The Fw_Upgrade and Fw_Upgrade_Done APIs automatically reboot if this flag bit is set.
 */
#define QAPI_FW_UPGRADE_FLAG_AUTO_REBOOT            (1<<0)

/**
 *  Definition used by the qapi_Fw_Upgrade() API as a flag.
 *  Fw_Upgrade copies files from an active image file system to a trial image file system 
 *  if this flag is set
 */
#define QAPI_FW_UPGRADE_FLAG_DUPLICATE_ACTIVE_FS    (1<<1) 
 
/**
 *  Definition used by the qapi_Fw_Upgrade() API as a flag.
 *  When Fw_Upgrade copies files from an active image file system to a trial image file system,
 *  Fw_Upgrade overwrites files if the files exist at the trial image file system if this flag is not set.
 *  Fw_Upgrade does not copy files if the files exist at the trial image file system if this flag is set.
 */
#define QAPI_FW_UPGRADE_FLAG_DUPLICATE_KEEP_TRIAL_FS    (1<<2) 
 
/** @name FWD Bit Definition
 *  Definition used by the qapi_Fw_Upgrade_Get_Active_FWD() API as a return 
 *  to indicate the FWD bit type.
@{ */
#define QAPI_FW_UPGRADE_FWD_BIT_GOLDEN   (0)
#define QAPI_FW_UPGRADE_FWD_BIT_CURRENT  (1)
#define QAPI_FW_UPGRADE_FWD_BIT_TRIAL    (2)
/** @} */ 
/** @name FWD Boot Type Definition
 *  Definition used by the qapi_Fw_Upgrade_Get_Active_FWD() API as a return
 *  to indicate the FWD type for booting.
@{ */
#define QAPI_FW_UPGRADE_FWD_BOOT_TYPE_GOLDEN	(1<<QAPI_FW_UPGRADE_FWD_BIT_GOLDEN)
#define QAPI_FW_UPGRADE_FWD_BOOT_TYPE_CURRENT	(1<<QAPI_FW_UPGRADE_FWD_BIT_CURRENT)
#define QAPI_FW_UPGRADE_FWD_BOOT_TYPE_TRIAL	(1<<QAPI_FW_UPGRADE_FWD_BIT_TRIAL)
/** @} */
/*----------------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------------*/


 
/**
 * @brief Defines an opaque Flash Firmware Partition Handle type. 
 *
 * @details A Partition Handle refers to some partition in a valid 
 *          or semi-valid (under construction) Firmware Descriptor.
 *
 */

typedef void *qapi_Part_Hdl_t;

/**
 *  Enumeration that represents the various states in Firmware Upgrade state machine.
 */ 
typedef enum qapi_Fw_Upgrade_State {
    QAPI_FW_UPGRADE_STATE_NOT_START_E = 0,
    /**< Firmware upgrade operation is not started. */
    QAPI_FW_UPGRADE_STATE_GET_TRIAL_INFO_E,
    /**< Get trial image information at flash. */
    QAPI_FW_UPGRADE_STATE_ERASE_FWD_E,
    /**< Erase FWD. */
    QAPI_FW_UPGRADE_STATE_ERASE_FLASH_E,
    /**< Erase the partition. */
    QAPI_FW_UPGRADE_STATE_ERASE_SECOND_FS_E,
    /**< Erase the second file system. */
    QAPI_FW_UPGRADE_STATE_PREPARE_FS_E,
    /**< Prepare the file system. */
    QAPI_FW_UPGRADE_STATE_ERASE_IMAGE_E,
    /**< Erase the subimage. */
    QAPI_FW_UPGRADE_STATE_PREPARE_CONNECT_E,
    /**< Prepare to connect to a remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_CONNECT_SERVER_E,
    /**< Connect to a remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_RESUME_SERVICE_E,
    /**< Resume the firmware upgrade service. */
    QAPI_FW_UPGRADE_STATE_RESUME_SERVER_E,
    /**< Resume connecting to the firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_RECEIVE_DATA_E,
    /**< Receive data from the remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_DISCONNECT_SERVER_E,
    /**< Disconnected from a remote firmware upgrade server. */
    QAPI_FW_UPGRADE_STATE_PROCESS_CONFIG_FILE_E,
    /**< Process firmware upgrade configuration file. */
    QAPI_FW_UPGRADE_STATE_PROCESS_IMAGE_E,
    /**< Process the image. */
    QAPI_FW_UPGRADE_STATE_DUPLICATE_IMAGES_E,
    /**< Duplicate the images from the current FWD. */
    QAPI_FW_UPGRADE_STATE_DUPLICATE_FS_E,
    /**< Duplicate the file system. */
	QAPI_FW_UPGRADE_STATE_FINISH_E,
    /**< Firmware upgrade is done. */
} /** @cond */ qapi_Fw_Upgrade_State_t /** @endcond */; 

/**
 *  Enumeration that represents the valid error codes that can be returned
 *            by the FW Upgrade APIs.  
 */
typedef enum qapi_Fw_Upgrade_Status{     
    QAPI_FW_UPGRADE_OK_E = 0,
    /**< Operation performed successfully. */
    QAPI_FW_UPGRADE_ERROR_E = 1,
    /**< Operation failed. */
    QAPI_FW_UPGRADE_ERR_INCOMPLETE_E = 1000,
    /**< Operation is incomplete. */
	QAPI_FW_UPGRADE_ERR_SESSION_IN_PROGRESS_E,
    QAPI_FW_UPGRADE_ERR_SESSION_NOT_START_E,
    /**< Firmware upgrade session is not started. */
    QAPI_FW_UPGRADE_ERR_SESSION_NOT_READY_FOR_SUSPEND_E,
    /**< Firmware upgrade session is not ready to enter the Suspend state. */
    QAPI_FW_UPGRADE_ERR_SESSION_NOT_SUSPEND_E,
    /**< Firmware upgrade session is not in the Suspend state. */
    QAPI_FW_UPGRADE_ERR_SESSION_RESUME_NOT_SUPPORT_E,
    /**< Firmware upgrade session resume is not supported by the plugin. */
    QAPI_FW_UPGRADE_ERR_SESSION_CANCELLED_E,
    /**< Firmware upgrade session was cancelled. */
    QAPI_FW_UPGRADE_ERR_SESSION_SUSPEND_E,
    /**< Firmware upgrade session was suspended. */
    QAPI_FW_UPGRADE_ERR_INTERFACE_NAME_TOO_LONG_E,
    /**< Interface name is too long. */
    QAPI_FW_UPGRADE_ERR_URL_TOO_LONG_E,
    /**< URL is too long. */
    QAPI_FW_UPGRADE_ERR_FLASH_NOT_SUPPORT_FW_UPGRADE_E,
	/**< Not supported firmware upgrade. */
    QAPI_FW_UPGRADE_ERR_FLASH_INIT_TIMEOUT_E,
    /**< Flash initialization timeout. */
    QAPI_FW_UPGRADE_ERR_FLASH_READ_FAIL_E,
    /**< Flash read failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_WRITE_FAIL_E,
    /**< Flash write failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_ERASE_FAIL_E,
    /**< Flash erase failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_NOT_ENOUGH_SPACE_E,
    /**< Not enough free space in flash. */    
    QAPI_FW_UPGRADE_ERR_FLASH_CREATE_PARTITION_E,
    /**< Partition creation failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_IMAGE_NOT_FOUND_E,
    /**< Partition image was not found. */
    QAPI_FW_UPGRADE_ERR_FLASH_ERASE_PARTITION_E,
    /**< Partition erase failure. */
    QAPI_FW_UPGRADE_ERR_FLASH_WRITE_PARTITION_E,
    /**< Partition write failure. */
    QAPI_FW_UPGRADE_ERR_IMAGE_NOT_FOUND_E, 
    /**< Image not found failure. */
    QAPI_FW_UPGRADE_ERR_IMAGE_DOWNLOAD_FAIL_E, 
    /**< Image download failure. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_CHECKSUM_E,
    /**< Incorrect image checksum failure. */    
    QAPI_FW_UPGRADE_ERR_SERVER_RSP_TIMEOUT_E,
    /**< Server communication timeout. */
    QAPI_FW_UPGRADE_ERR_INVALID_FILENAME_E,
    /**< Image file name is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_HDR_E,
    /**< Firmware upgrade image header is invalid. */
    QAPI_FW_UPGRADE_ERR_INSUFFICIENT_MEMORY_E,
    /**< Not enough memory. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_SIGNATURE_E,
    /**< Firmware upgrade image signature is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRCT_VERSION_E,
    /**< Firmware upgrade image version is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_NUM_IMAGES_E,
    /**< Firmware upgrade image number of images is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_LENGTH_E,
    /**< Firmware upgrade image length is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_HASH_TYPE_E,
    /**< Firmware upgrade image hash type is invalid. */
    QAPI_FW_UPGRADE_ERR_INCORRECT_IMAGE_ID_E, 
    /**< Firmware upgrade image ID is invalid. */
    QAPI_FW_UPGRADE_ERR_BATTERY_LEVEL_TOO_LOW_E,
    /**< Battery level is too low. */
    QAPI_FW_UPGRADE_ERR_CRYPTO_FAIL_E,
    /**< Crypto check failure. */
    QAPI_FW_UPGRADE_ERR_PLUGIN_ENTRY_EMPTY_E,  
    /**< Firmware upgrade plugin callback is empty. */
    QAPI_FW_UPGRADE_ERR_TRIAL_IS_RUNNING_E,
    /**< Trial image is running */
    QAPI_FW_UPGRADE_ERR_FILE_NOT_FOUND_E,
    /**< File was not found. */    
    QAPI_FW_UPGRADE_ERR_FILE_OPEN_ERROR_E,
    /**< Open file failure. */    
    QAPI_FW_UPGRADE_ERR_FILE_NAME_TOO_LONG_E,
    /**< File name is too long. */    
    QAPI_FW_UPGRADE_ERR_FILE_WRITE_ERROR_E,
    /**< Write file failure. */    
    QAPI_FW_UPGRADE_ERR_MOUNT_FILE_SYSTEM_ERROR_E,
    /**< Mount file system failure. */
    QAPI_FW_UPGRADE_ERR_PRESERVE_LAST_FAILED_E,
} /** @cond */ qapi_Fw_Upgrade_Status_Code_t /** @endcond */;

/**
 * Declaration of a callback function called by the firmware upgrade state machine. The application implments
 * this callback and passes it as a parameter to the qapi_Fw_Upgrade() API.
 *
 * @param[in] state       Firmware upgrade state machine state.
 * @param[in] status      Error code defined by enum #qapi_Fw_Upgrade_Status.
 *
 * @return
 * None.
 */
typedef void (*qapi_Fw_Upgrade_CB_t)(int32_t state, uint32_t status);

/**
 * Declaration of a callback function called by the firmware upgrade state machine on initalization. 
 * The plugin module implements this callback and performs all plugin related initializations.
 * The application passes this callback as a parameter to the qapi_Fw_Upgrade() API.
 *
 * @param[in] interface_Name      Network interface name (plugin dependent).
 * @param[in] url                 Server URL (plugin dependent). 
 * @param[in] init_Param          Initialization parameters (plugin dependent).
 *
 * @return 
 * Status defined by enum #qapi_Fw_Upgrade_Status.
 */
typedef qapi_Fw_Upgrade_Status_Code_t (*qapi_Fw_Upgrade_Plugin_Init_t)(const char* interface_Name, const char *url, void *init_Param);

/**
 * Declaration of a callback function called by the firmware upgrade state machine on upgrade completion. 
 * The plugin module implements this callback and performs all plugin related cleanup.
 * The application passes this callback as a parameter to the qapi_Fw_Upgrade() API.
 *
 * @param[in] interface_name      Network interface name (plugin dependent).
 * @param[in] url                 Server URL (plugin dependent). 
 *
 * @return
 * Status defined by enum #qapi_Fw_Upgrade_Status.
 */
typedef qapi_Fw_Upgrade_Status_Code_t (*qapi_Fw_Upgrade_Plugin_Fin_t)(void);

/**
 * Declaration of a callback function called by the firmware upgrade state machine to receieve a packet from the plugin. 
 * The plugin module implements this callback and fills the buffer with incoming data.
 * The application passes this callback as a parameter to the qapi_Fw_Upgrade() API.
 *
 *
 * @param[out] buffer      Receive data buffer.
 * @param[in]  buf_len     Buffer length. 
 * @param[out] ret_size    Received data size.
 *
 * @return
 * Status defined by enum #qapi_Fw_Upgrade_Status.
 */
typedef qapi_Fw_Upgrade_Status_Code_t (*qapi_Fw_Upgrade_Plugin_Recv_Data_t)(uint8_t *buffer, uint32_t buf_len, uint32_t *ret_size);

/**
 * Declaration of a callback function called by the firmware upgrade state machine to abort a plugin operation. 
 * The plugin module implements this callback and aborts connection when invoked.
 * The application passes this callback as a parameter to the qapi_Fw_Upgrade() API.
 *
 * @return
 * Status defined by enum #qapi_Fw_Upgrade_Status.
 */
typedef qapi_Fw_Upgrade_Status_Code_t (*qapi_Fw_Upgrade_Plugin_Abort_t)(void);

/**
 * Declaration of a callback function called by the firmware upgrade state machine on resume.
 * The plugin module implements this callback and performs all plugin related resumes.
 * The application passes this callback as a parameter to the qapi_Fw_Upgrade() API.
 *
 * @param[in] interface_name      Network interface name (plugin dependent).
 * @param[in] url                 Server URL (plugin dependent).
 * @param[in] offset              Offset to resume the download (plugin dependent).
 *
 * @return
 * Status defined by enum #qapi_Fw_Upgrade_Status.
 */
typedef qapi_Fw_Upgrade_Status_Code_t (*qapi_Fw_Upgrade_Plugin_Resume_t)(const char* interface_name, const char *url, const uint32_t offset);


/**
 * Represents a set of firmware upgrade plugin callbacks.
 *
 * When the application calls qpai_Fw_Upgrade(), it must fill
 * this structure and pass it to the firmware upgrade engine. The engine calls 
 * these firmware upgrade plugin callbacks during different stages of an upgrade.
 */
typedef struct {
    qapi_Fw_Upgrade_Plugin_Init_t      fw_Upgrade_Plugin_Init;
    /**< Callback to initialize a firmware upgrade. */
    qapi_Fw_Upgrade_Plugin_Recv_Data_t fw_Upgrade_Plugin_Recv_Data;
    /**< Callback to retrieve data. */
    qapi_Fw_Upgrade_Plugin_Abort_t     fw_Upgrade_Plugin_Abort;
    /**< Firmware upgrade plugin abort callback. */
    qapi_Fw_Upgrade_Plugin_Resume_t     fw_Upgrade_Plugin_Resume;
    /**< Firmware upgrade plugin resume callback. */
    qapi_Fw_Upgrade_Plugin_Fin_t       fw_Upgrade_Plugin_Fin;
    /**< Firmware upgrade plugin finish callback. */
} qapi_Fw_Upgrade_Plugin_t;

/** @} */ /* end_addtogroup qapi_Fw_Upgrade */ 

/*----------------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------------*/
 
/* Miscellaneous Firmware Upgrade APIs */
/** @addtogroup qapi_Fw_Upgrade
@{ */
 
/**
 * @brief Initializes the Firmware Upgrade library
 *
 * @details Reads Firmware Descriptors from flash and populates internal data structures.
 *          Must be called before other APIs are used.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_init(void);

/**
 * @brief Returns the physical size, in bytes, of the flash part.
 *
 * @details The first three flash blocks (4 KB) are always reserved for FWDs.
 *
 * @param[out] size    Size of the flash.
 * 
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_Flash_Size(uint32_t *size);

/**
 * @brief Returns the flash block size, in bytes, of the flash part.
 *
 * @param[out] size    Size of the flash block.
 * 
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_Flash_Block_Size(uint32_t *size);

/* Firmware Descriptor APIs  */
/**
 * @brief Erases a firmware descriptor so that an entirely new FWD can be formed in its place.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to be erased.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Erase_FWD(uint8_t FWD_idx);

/**
 * @brief Sets the magic number for a firmware descriptor.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[in] magic      Magic number to write to the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Set_FWD_Magic(uint8_t FWD_idx, uint32_t magic);

/**
 * @brief Gets the magic number from a firmware descriptor.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[out] magic     Magic number of the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_FWD_Magic(uint8_t FWD_idx, uint32_t *magic);

/**
 * @brief Sets the rank number for a firmware descriptor.
 *
 * @details   Set/set the rank field of a FWD. Standard rank semantics:
 *            - 0x00000000 -- Golden firmware
 *            - 0xFFFFFFFF -- Trial firmware
 *            - Other value -- Current firmware
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[in] rank       Rank number to write to the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Set_FWD_Rank(uint8_t FWD_idx, uint32_t rank);

/**
 * @brief Gets the rank number from a firmware descriptor.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[out] rank      Rank number at the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_FWD_Rank(uint8_t FWD_idx, uint32_t *rank);

/**
 * @brief Sets the version number for a firmware descriptor.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[in] version    Version number to write to the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Set_FWD_Version(uint8_t FWD_idx, uint32_t version);

/**
 * @brief Gets the rank number from a firmware descriptor.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[out] version   Version number of the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_FWD_Version(uint8_t FWD_idx, uint32_t *version);

/**
 * @brief Sets the status of a firmware descriptor.
 *
 * @details  Standard status semantics:
 *           - 0x01 -- Valid FWD
 *           - 0xFF -- Under construction or unused
 *           - 0x00 -- Invalid/old/unused FWD
 *
 * @param[in] FWD_idx  Firmware descriptor index number to operate.
 *
 * @param[in] status   Status value to write to the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Set_FWD_Status(uint8_t FWD_idx, uint8_t status);

/**
 * @brief Gets the status value from a firmware descriptor.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[out] status    Status value of the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_FWD_Status(uint8_t FWD_idx, uint8_t *status);

/**
 * @brief Sets the total number of images for a firmware descriptor.
 *
 * @param[in] FWD_idx     Firmware descriptor index number to operate.
 *
 * @param[in] image_nums  Image numbers to write to the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Set_FWD_Total_Images(uint8_t FWD_idx, uint8_t image_nums);

/**
 * @brief Gets the total image numbers from a firmware descriptor.
 *
 * @param[in] FWD_idx        Firmware descriptor index number to operate.
 *
 * @param[out] image_nums    Image numbers in the firmware descriptor.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_FWD_Total_Images(uint8_t FWD_idx, uint8_t *image_nums);

/**
 * @brief Gets the FWD index number that is current running.
 *
 * @details This is for the FWD that was selected by the bootloaders and is currently in use.
 *
 * @param[out] fwd_boot_type    Type of FWD used for booting.
 *
 * @param[out] valid_fwd        Informaiton about which FWDs are present (1 bit per FWD).
 *  
 *
 * @return
 * The active FWD number.
 */
uint8_t qapi_Fw_Upgrade_Get_Active_FWD(uint32_t *fwd_boot_type, uint32_t *valid_fwd);

/**
 * @brief Accepts a trial FWD.
 *
 * @details Acceptance is implemented by changing the rank field of the trial firmware to one larger than the current FWD 
 *          and then changing the status field of the former current FWD to 0.
 *
 * @note1hang  If the active FWD is not a trial FWD, this command retruns an error.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Accept_Trial_FWD(void);

/**
 * @brief Rejects a trial FWD.
 *
 * @details Rejection is implemented by changing the status field of the trial FWD to 0.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Reject_Trial_FWD(void);

/**
 * @brief Select a trial FWD.
 *
 * @details Picks one available FWD entry for trial.
 *
 * @param[out] fwd_index        Available FWD for trial.
 *
 * @param[out] start_address    Available flash start address.
 *
 * @param[out] size             Available flash size
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Select_Trial_FWD(uint8_t *fwd_index, uint32_t *start_address, uint32_t *size);


/**
 * @brief Erases an entire partition.
 *
 * @param[in] FWD_idx    Firmware descriptor index number that is to be erased.
 *
 * @param[in] size       Size of the partition to be erased.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Format_Partition(uint8_t FWD_idx, uint32_t size);


/* Partition Firmware Upgrade APIs */
/**
 * @brief  Creates/defines a partition that starts at the specified flash offset
 *         and has a specified size. 
 *
 * @details  The partition is given the specified ID and is associated with
 *           the specified FWD. 
 *           A handle to the new partition is returned. 
 *           This API does not permit overlaps with any existing partition 
 *           associated with any valid FWD or within the specified FWD. 
 *           The start and size parameters are byte offsets, which must 
 *           be QAPI_FU_FLASH_BLOCK_SZ (4 KB) aligned.
 *
 * @param[in] FWD_idx    Firmware descriptor index number to operate.
 *
 * @param[in] id         Partition ID.
 *
 * @param[in] img_ver    Image version.
 *
 * @param[in] start      Start flash offset for the partition.
 *
 * @param[in] size       Partition size.
 *
 * @param[out] hdl       Partition handle for the partition operation.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Create_Partition(uint8_t FWD_idx, uint32_t id, uint32_t img_ver, uint32_t start, uint32_t size, qapi_Part_Hdl_t *hdl);

/**
 * @brief Gets a handle for the first partition associated with the specified FWD.
 *
 * @param[in] FWD_idx    Firmware descriptor index number. 
 *
 * @param[out] hdl       Partition handle for the partition operation.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_First_Partition(uint8_t FWD_idx, qapi_Part_Hdl_t *hdl);

/**
 * @brief Gets the next partition after the current one. 
 *
 * @details Guaranteed to be in the same FWD as curr. 
 *          This function returns an error when it reaches all blank (uninitialized) partition metadata.
 *
 * @param[in] curr      Current partition hander.
 *
 * @param[out] hdl      Next partition handle.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Next_Partition(qapi_Part_Hdl_t curr, qapi_Part_Hdl_t *hdl);

/**
 * @brief   Finds a partition.
 *
 * @details  This function scans the partition table associated with the specified FWD to find 
 *           the first partition with the specified ID and get a handle to that
 *           partition.
 *
 * @param[in] FWD_idx    Firmware descriptor index number.
 *
 * @param[in] id         Partition image ID.
 *
 * @param[out] hdl       Partition handle.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Find_Partition(uint8_t FWD_idx, uint32_t id, qapi_Part_Hdl_t *hdl);

/**
 * @brief Gets the image ID associated with a partition in a FWD.
 *
 * @param[in] hdl      Partition handle.
 *
 * @param[out] id      Partition image ID.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_Image_ID(qapi_Part_Hdl_t hdl, uint32_t *id);

/**
 * @brief Gets the image version associated with a partition in a FWD.
 *
 * @param[in]  hdl      Partition handle.
 *
 * @param[out] version  Image version to retrieve.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t  qapi_Fw_Upgrade_Get_Image_Version(qapi_Part_Hdl_t hdl, uint32_t *version);

/**
 * @brief Sets the image ID associated with a partition in a FWD.
 *
 * @param[in] hdl      Partition handle.
 *
 * @param[in] id       Partition image ID.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Set_Image_ID(qapi_Part_Hdl_t *hdl, uint32_t id);

/**
 * @brief Sets the image ID associated with a partition in a FWD.
 *
 * @param[in]  hdl      Partition handle.
 *
 * @param[out] version  Image version to be set.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t  qapi_Fw_Upgrade_Set_Image_Version(qapi_Part_Hdl_t *hdl, uint32_t version);

/**
 * @brief Gets the size of a partition in a FWD.
 *
 * @param[in] hdl      Partition handle.
 *
 * @param[out] size    Partition image size.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_Partition_Size(qapi_Part_Hdl_t hdl, uint32_t *size);

/**
 * @brief Gets the start offset of a partition in a FWD.
 *
 * @param[in] hdl      Partition handle.
 *
 * @param[out] start   Start offset at flash.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_Partition_Start(qapi_Part_Hdl_t hdl, uint32_t *start);

/**
 * @brief Identifies with which FWD a partition handle is associated.
 *
 * @param[in] hdl        Partition handle.
 *
 * @param[out] FWD_idx   Firmware descriptor index number.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Get_Partition_FWD(qapi_Part_Hdl_t hdl, uint8_t *FWD_idx);

/**
 * @brief    Erases n bytes of flash starting at the specified offset from the 
 *           start of the specified partition. 
 *
 * @details  Offset and nbytes must be multiples of QAPI_FU_FLASH_BLOCK_SZ (4 KB). 
 *           This is a partition-relative byte-oriented erase operation. 
 *           As a special case, if nbytes==0, the entire partition is erased.
 *
 * @param[in] hdl       Partition handle.
 *
 * @param[in] offset    Flash offset to erase.
 *
 * @param[in] nbytes    Flash size to erase.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Erase_Partition(qapi_Part_Hdl_t hdl, uint32_t offset, uint32_t nbytes);

/**
 * @brief    Writes n bytes from a specified buffer to flash at the specified offset from the 
 *           start of the specified partition. 
 *
 * @details  This is a partition-relative byte-oriented write operation. 
 *           Either the partition must be blank (erased) before this write operation begins 
 *           or the write operation must avoid any attempt to change a 0 to a 1 bit. 
 *           If the final contents of flash do not match the original buffer, an error is raised.
 *
 * @param[in] hdl       Partition handle.
 *
 * @param[in] offset    Flash offset to write.
 *
 * @param[in] buf       Buffer point to write to flash.
 *
 * @param[in] nbytes    Flash size to write.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Write_Partition(qapi_Part_Hdl_t hdl, uint32_t offset, char *buf, uint32_t nbytes);

/**
 * @brief    Reads up to max_bytes into the specified buffer from flash at the specified offset 
 *           from the start of the specified partition. 
 *
 * @details  This is a partition-relative byte-oriented read operation.
 *
 * @param[in] hdl        Partition handle.
 *
 * @param[in] offset     Flash offset to read.
 *
 * @param[in] buf        Buffer point to store the flash data.
 *
 * @param[in] max_bytes  Size to read.
 *
 * @param[out] nbytes    Actual flash read size in buf.
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Read_Partition(qapi_Part_Hdl_t hdl, uint32_t offset, char *buf, uint32_t max_bytes, uint32_t *nbytes);

/**
 * @brief Releases a partition handle.
 *
 * @details  
 *
 * @param[in] hdl       partition handle
 *
 * @return On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */
qapi_Status_t qapi_Fw_Upgrade_Close_Partition(qapi_Part_Hdl_t hdl);

/**
 * @brief Initiates a device reboot
 *
 * @return
 * On success, QAPI_OK is returned; on error, QAPI_ERROR is returned.
 */

qapi_Status_t qapi_Fw_Upgrade_Reboot_System(void);

/**
 * Starts a firmware upgrade session.
 *
 * The caller from the application domain specifies all the required parameters, 
 * including the plugin functions, source of the image, and flags. 
 * The session automatically ends in the case of an error. 
 *
 * @param[in] interface_Name    Network interface name, e.g., wlan1.
 *
 * @param[in] plugin        Parameter of type qapi_Fw_Upgrade_Plugin_t containing 
 *                          a set of plugin callback functions.
 *                          For more details, refer to #qapi_Fw_Upgrade_Plugin_t.
 *
 * @param[in] url           Source information for a firmware upgrade. \n
 *                          For FTP: \n
 *                          [user_name]:[password]@[IPV4 address]:[port]  for IPV4 \n
 *                          [user_name]:[password]@|[IPV6 address]|:[port]  for IPV6
 *
 * @param[in] cfg_File      Image file information for a firmware upgrade.
 *
 * @param[in] flags         Flags with bits defined for a firmware upgrade. See the qapi_Fw_Upgrade flag for a definition.
 *
 * @param[in] cb            Optional callback function called by firmware upgrade engine to provide status information.
 * 
 * @param[in] init_Param    Optional init parameter passed to the firmware upgrade init function.
 * 
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 *  On error, error code defined by enum #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade(char *interface_Name, qapi_Fw_Upgrade_Plugin_t *plugin, char *url, char *cfg_File, uint32_t flags, qapi_Fw_Upgrade_CB_t cb, void *init_Param );

/**
 * Cancels a firmware upgrade session.
 *
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 *  On error, error code defined by #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Cancel(void);

/**
 * Activates or invalidates the trial image. The application calls this API after it has verified that
 * the image is valid or invalid. The criteria for image validity is defined by the application.
 *
 * @param[in] result        Result: \n
                            1: The image is valid; set trial image to active \n
 *                          0: The image is invalid; invalidate trial image  
 * @param[in] flags         Flags (bit0): \n
                            1: The device reboots after activating or invalidating the trial image \n
 *                          0: The device does not reboot after activating or invalidating the trial image \n
 *                          @note If the reboot_flag is set, the device will reboot and there is no return.
 *
 * @return
 * On success, QAPI_FW_UPGRADE_OK_E is returned. \n 
 * On error, QAPI_FW_UPGRADE_ERROR_E is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Done(uint32_t result, uint32_t flags);

/**
 * Suspends the firmware upgrade session.
 *
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n
 *  On error, error code defined by #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Suspend(void);

/**
 * Resumes the firmware upgrade session.
 *
 * @return
 *  On success, QAPI_FW_UPGRADE_OK_E is returned. \n
 *  On error, error code defined by #qapi_Fw_Upgrade_Status is returned.
 */
qapi_Fw_Upgrade_Status_Code_t qapi_Fw_Upgrade_Resume(void);

/** @} */ /* end_addtogroup qapi_Fw_Upgrade */ 

#endif /* _QAPI_FIRMWARE_UPGRADE_H_ */
