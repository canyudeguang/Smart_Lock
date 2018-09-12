/*
* Copyright (c) 2016 Qualcomm Technologies, Inc.
* All Rights Reserved.
* Confidential and Proprietary - Qualcomm Technologies, Inc.
*/
/*****< qapi_ble_hciusbt.h >***************************************************/
/*      Copyright 2015 - 2016 Qualcomm Technologies, Inc.                     */
/*      All Rights Reserved.                                                  */
/*                                                                            */
/*  QAPI_BLE_HCIUSBT - USB HCI Driver Layer Types.                            */
/*                                                                            */
/*  Author:  Tim Cook                                                         */
/*                                                                            */
/*** MODIFICATION HISTORY *****************************************************/
/*                                                                            */
/*   mm/dd/yy  F. Lastname    Description of Modification                     */
/*   --------  -----------    ------------------------------------------------*/
/*   08/28/15  T. Cook        Initial creation.                               */
/******************************************************************************/
#ifndef __QAPI_BLE_HCIUSBT_H__
#define __QAPI_BLE_HCIUSBT_H__

   /* The following constants represent the Minimum, Maximum, and Values*/
   /* that are used with the Initialization Delay member of the         */
   /* qapi_BLE_HCI_USBDriverInformation_t structure.  These Delays are  */
   /* specified in Milliseconds and represent the delay that is to be   */
   /* added between Port Initialization (Open) and the writing of any   */
   /* data to the Port.  This functionality was added because it was    */
   /* found that some USB Drivers required a delay between the time the */
   /* Device was opened and the time when the Driver was ready to accept*/
   /* data.  The default is NO Delay (0 Milliseconds).                  */
#define QAPI_BLE_QAPI_BLE_HCI_USB_INFORMATION_INITIALIZATION_DELAY_MINIMUM       0
#define QAPI_BLE_QAPI_BLE_HCI_USB_INFORMATION_INITIALIZATION_DELAY_MAXIMUM    5000
#define QAPI_BLE_QAPI_BLE_HCI_USB_INFORMATION_INITIALIZATION_DELAY_DEFAULT       0

   /* The following type declaration defines the HCI USB Driver that    */
   /* that will be used as the physical HCI Transport Driver for the USB*/
   /* Device that is to be opened.  This type declaration is used in the*/
   /* qapi_BLE_HCI_USBDriverInformation_t structure that is required    */
   /* when an HCI USB Device is opened.                                 */
typedef enum
{
   QAPI_BLE_USB_DRIVER_SS1_E,
   QAPI_BLE_USB_DRIVER_GARMIN_E
} qapi_BLE_HCI_USB_Driver_t;

   /* The following type declaration represents the structure of all    */
   /* Data that is needed to open an HCI USB Port.                      */
typedef struct qapi_BLE_HCI_USBDriverInformation_s
{
   uint32_t                  DriverInformationSize;
                                                /* Physical Size of this      */
                                                /* structure.                 */
   qapi_BLE_HCI_USB_Driver_t DriverType;
                                                /* HCI USB Driver type that   */
                                                /* will be used for           */
                                                /* communication to the       */
                                                /* Bluetooth Device.          */
   uint32_t                  InitializationDelay;
                                                /* Time (In Milliseconds) to  */
                                                /* Delay after the Port is    */
                                                /* opened before any data is  */
                                                /* sent over the Port.  This  */
                                                /* member is present because  */
                                                /* some Drivers may require   */
                                                /* a delay because the device */
                                                /* does not function for some */
                                                /* specified period of time.  */
} qapi_BLE_HCI_USBDriverInformation_t;

   /* The following constant is used with the                           */
   /* HCI_USB_Driver_Reconfigure_Data_t structure (ReconfigureCommand   */
   /* member) to specify that the Communication parameters are required */
   /* to change.  When specified, the ReconfigureData member will point */
   /* to a valid qapi_BLE_HCI_USBDriverInformation_t structure which    */
   /* holds the new parameters.                                         */
   /* * NOTE * The underlying driver may not support changing all of    */
   /*          specified parameters.                                    */
#define QAPI_BLE_HCI_USB_DRIVER_RECONFIGURE_DATA_COMMAND_CHANGE_PARAMETERS  (QAPI_BLE_HCI_DRIVER_RECONFIGURE_DATA_RECONFIGURE_COMMAND_TRANSPORT_START)

#endif
