/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
 /*! \file qmesh.h
  *  \brief Qmesh API
  *
  *   This file contains the API for Qmesh.
  *
  */
/******************************************************************************/

#ifndef __QMESH_H__
#define __QMESH_H__

#include "qmesh_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*============================================================================*
    Public Function Implementations
 *============================================================================*/

/*! \addtogroup Core_General */
/*! @{ */

/*=========================Qmesh Generic APIs==============================*/
/*----------------------------------------------------------------------------*
 * QmeshInit
 */
/*! \brief Initializes the core Mesh stack.
 *
 *  Initializes the core stack internal layers and reads all persistent data
 *  from PS.
 *
 *  \param[in] max_networks  Maximum number of provisioned networks.
 *  \param[in] callback      Application callback for reporting the events.
 *                           See QMESH_APP_CB_T.
 *
 *  \returns See \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshInit(uint16_t max_networks, QMESH_APP_CB_T callback);

/*----------------------------------------------------------------------------*
 * QmeshCreateDeviceInstance
 */
/*! \brief Creates a new device instance. A new device instance can be
 *         provisioned or act as a provisioner.
 *
 *  \param[in] device_instance Device instance that needs to be provisioned or act
 *                         as provisioner.
 *  \param[in] device_data  Pointer to the device identifier data.
 *                          See \ref QMESH_DEVICE_IDENTIFIER_DATA_T.
 *  \param[in] comp_data    Pointer to the device composition data.
 *                          See \ref QMESH_DEVICE_COMPOSITION_DATA_T.

 *
 *  \return \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshCreateDeviceInstance(uint8_t device_instance, const QMESH_DEVICE_IDENTIFIER_DATA_T *device_data,
                                                const QMESH_DEVICE_COMPOSITION_DATA_T *comp_data);

/*----------------------------------------------------------------------------*
 * QmeshAddBearer
 */
/*! \brief Adds a new Mesh bearer type.
 *
 *   The stack sends messages on all enabled bearer types. At least one bearer
 *   type must be enabled for the stack to send messages.
 *
 *  \param[in] bearer_type  Bearer type to be added. See \ref QMESH_BEARER_TYPE_T.
 *  \param[in] context      Pointer to the information passed to the platform
 *                          bearer API. The stack does not interpret this information.
 *  \param[in] mtu_size     MTU size to be used. This is used for segmentation and
 *                          reassembly of bearer packets in the stack. For an advertisement
 *                          bearer, this parameter is ignored.
 *  \param[out] bearer_handle Returns the bearer handle if the operation is successful.
 *                          See \ref QMESH_BEARER_HANDLE_T.
 *
 *  \return QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshAddBearer(QMESH_BEARER_TYPE_T bearer_type,
                              QMESH_BEARER_CONTEXT_T context,
                              uint16_t mtu_size,
                              QMESH_BEARER_HANDLE_T *bearer_handle);

/*----------------------------------------------------------------------------*
 * QmeshDeleteBearer
 */
/*! \brief Deletes an existing bearer. Sets the handle to \ref QMESH_BEARER_HANDLE_INVALID.
 *
 *  \param[in] bearer_handle  Pointer to the bearer handle. See \ref QMESH_BEARER_HANDLE_T.
 *
 *  \return QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshDeleteBearer(QMESH_BEARER_HANDLE_T bearer_handle);

/*----------------------------------------------------------------------------*
 * QmeshGetAdvBearerHandle
 */
/*! \brief Retrieves bearer handle for advert bearer.
 *
 *  \param[in] bearer_type  Type of advert bearer.
 *
 *  \return \ref QMESH_BEARER_HANDLE_T.
 *          QMESH_BEARER_HANDLE_INVALID if not enabled.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_BEARER_HANDLE_T QmeshGetAdvBearerHandle(QMESH_BEARER_TYPE_T bearer_type);

/*----------------------------------------------------------------------------*
 * QmeshGetProxyBearerHandle
 */
/*! \brief Retrieves bearer handle for proxy bearer based on context information.
 *
 *  \param[in] ctx  Bearer context.
 *
 *  \return \ref QMESH_BEARER_HANDLE_T.
 *          QMESH_BEARER_HANDLE_INVALID if not enabled.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_BEARER_HANDLE_T QmeshGetProxyBearerHandle(QMESH_BEARER_CONTEXT_T ctx);

/*----------------------------------------------------------------------------*
 * QmeshProxySetFilterType
 */
/*! \brief Sets the proxy filter type on the peer proxy server device.
 *         The filter status is notified with the \ref QMESH_PROXY_FILTER_STATUS_EVENT.
 *
 *  \param[in] bearer_handle  Bearer handle. See \ref QMESH_BEARER_HANDLE_T.
 *  \param[in] filter_type    White-list or blacklist filter types to use.
 *                            See \ref QMESH_PROXY_FILTER_TYPE_T.
 *  \param[in] net_key_idx    Network ID on which the proxy filter is to be used.
 *                            See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshProxySetFilterType(QMESH_BEARER_HANDLE_T bearer_handle,
                                              QMESH_PROXY_FILTER_TYPE_T filter_type,
                                              QMESH_SUBNET_KEY_IDX_INFO_T net_key_idx);

/*----------------------------------------------------------------------------*
 * QmeshProxyFilterAddRemoveAddr
 */
/*! \brief Adds or removes destination addresses to the proxy filter list on
 *         the peer proxy server device. The filter status is notified with the
 *         \ref QMESH_PROXY_FILTER_STATUS_EVENT.
 *
 *  \param[in] bearer_handle  Bearer handle. See \ref QMESH_BEARER_HANDLE_T.
 *  \param[in] net_key_idx    Network ID on which the proxy filter is to be used.
 *                            See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] add            Add to the list if TRUE, otherwise remove from the list.
 *  \param[in] addr_list      Pointer to the list of addresses to be added/removed
 *                            to/from the proxy filter address list.
 *  \param[in] num_addr       Number of addresses in the list.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshProxyFilterAddRemoveAddr(QMESH_BEARER_HANDLE_T bearer_handle,
                                                    QMESH_SUBNET_KEY_IDX_INFO_T net_key_idx,
                                                    bool add, const uint16_t *addr_list,
                                                    uint8_t num_addr);

/*----------------------------------------------------------------------------*
 * QmeshGetProxyAdvServiceData
 */
/*! \brief Returns the service data used in the connectable advertisements when the
 *  Mesh proxy service is supported.
 *
 *  Any provisioned node that supports the GATT bearer advertises with the Mesh
 *  proxy service UUID data.
 *
 *  The service data used for the Mesh proxy service UUID can be one of two types:
 *  - Network ID type -- 64-bit network ID of the network key. \n
 *    Svc Data: [Hdr:0x0C, 0x16, 0xd2, 0x7F, Data:0x00, Nw ID 8 octets]
 *  - Node identity type -- 64-bit hash followed by a 64-bit random number. \n
 *    Svc Data: [Hdr:0x14, 0x16, 0xd2, 0x7F, Data:0x01, Hash 8 Octets, Random 8 Octets]
 *
 *  \note1hang The function returns only the data part of the service data.
 *
 *  All nodes provisioned to a network and that support GATT proxy service will
 *  advertise with the same service data if the service data type is based on the
 *  network ID. This can be used to identify the advertisements from nodes
 *  belonging to one network.
 *
 *  When an individual node must be identified by its element address for
 *  connection, the proxy service data must be a node identity type. This can be
 *  enabled on a node by setting the node identity state of a device using the
 *  config message QMESH_CONFIG_CLI_NODE_IDENTITY_SET: \n
 *  - The proxy client calls this function to get the proxy service data for a network
 *  to identify advertisements from the proxy servers belonging to that network.
 *  - The proxy server calls this function to get the proxy service data to be used
 *  in the connectable advertisements.
 *
 *  \param[in] net_key_info Provisioned network and Subnetwork Information.
 *                          See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] type         Service data type. See \ref QMESH_PROXY_ADV_ID_TYPE_T.
 *  \param[in] elm_addr     Element address for which the node identity type serivce
 *                          data is requested. This is ignored for a network ID type.
 *  \param[in] random       64-bit random data used in node identity type service data.
 *                          This is ignored for a network ID type.
 *  \param[out] svc_data    Pointer to a buffer to receive the service data.
 *                          9 Octets for a network ID type.
 *                          17 Octets for a node identity type.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGetProxyAdvServiceData(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                                  QMESH_PROXY_ADV_ID_TYPE_T type,
                                                  uint16_t elm_addr,
                                                  uint8_t *random,
                                                  uint8_t *svc_data);

/*!@} */

/*=========================Qmesh Provisioning APIs===========================*/
/*! \addtogroup Core_Provisioning
 * @{
 */

/*----------------------------------------------------------------------------*
 * QmeshSetProvisioningRole
 */
/*! \brief Sets the provisioner or device role. In the case of a node role,
 *         a public key is generated in this call and the application references
 *         it using a pointer passed in the structure \ref QMESH_DEVICE_IDENTIFIER_DATA_T
 *         passed in QmeshCreateDeviceInstance().
 *
 *  \param[in] device_instance  Device instance for which the role needs to be set.
 *                              Uses the Device Information and composition data for
 *                              this device instance.
 *  \param[in] provisioner      Enables the provisioner role if TRUE.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 *           If result code is \ref QMESH_RESULT_INPROGRESS, then application needs
 *           to wait for event \ref QMESH_PUBLIC_KEY_AVAILABLE_EVENT before starting
 *           provisioning (This is applicable only in case role is DEVICE).
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetProvisioningRole(uint8_t device_instance, bool provisioner);

/*----------------------------------------------------------------------------*
 * QmeshSendUnProvDeviceBeacon
 */
/*! \brief Sends an unprovisioned device beacon.
 *
 *  \param[in] device_instance  Device instance on which the unprovisioned beacon needs
 *                          to be set. Uses the Device Information and composition data for
 *                          this device instance.
 *  \param[in] use_urihash   Uses a URI hash in the unprovisioned device beacon.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSendUnProvDeviceBeacon(uint8_t device_instance, bool use_urihash);

/*----------------------------------------------------------------------------*
 * QmeshGenerateURIHash
 */
/*! \brief Generates the URI hash from the URI.
 *
 *  \param[in] uri       URI input string. See \ref QMESH_URI_T.
 *  \param[in] uri_len   URI input string length. See \ref QMESH_URI_T.
 *  \param[out] uri_hash  Pointer to a variable to which to copy the URI hash.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGenerateURIHash(QMESH_URI_T uri, uint16_t uri_len, uint32_t *uri_hash);

/*----------------------------------------------------------------------------*
 * QmeshStartDeviceDiscovery
 */
/*! \brief Starts discovery of unprovisioned devices beacons. Optionally, filters
 *  the beacons as per the parameters.
 *
 *  This function enables processing of the unprovisioned device beacon
 *  messages. Every beacon message received will be notified to the application
 *  with the \ref QMESH_UNPROVISIONED_DEVICE_EVENT after filtering, if enabled.
 *  The discovery procedure can be stopped by calling QmeshStopDeviceDiscovery().
 *
 *   \note1hang This API is only for discovering devices over a PB-ADV bearer.
 *         This version of the code does not support device filtering and ignoring
 *         duplicate devices.
 *
 *  \param[in] uuid_list      Array of UUIDs to be filtered. NULL if no filtering
 *                            is needed.
 *  \param[in] uuid_list_len  Number of UUIDs in the UUID list.
 *  \param[in] ignore_dup     Ignore duplicated beacons.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshStartDeviceDiscovery(QMESH_DEVICE_UUID_T *const uuid_list,
                                                uint16_t uuid_list_len, bool ignore_dup);

/*----------------------------------------------------------------------------*
 * QmeshStopDeviceDiscovery
 */
/*! \brief Stops discovery of unprovisioned devices beacons.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshStopDeviceDiscovery(void);

/*----------------------------------------------------------------------------*
 * QmeshProvisionDevice
 */
/*! \brief Starts provisioning the device selected by the UUID.
 *
 *  This function starts provisioning an unprovisioned device. During the
 *  provisioning procedure, both the provisioner and the nodes
 *  receive a \ref QMESH_OOB_ACTION_EVENT if out-of-band authentication
 *  is supported. The provisioner receives a \ref QMESH_DEVICE_CAPABILITY_EVENT
 *  that provides the device provisioning capabilities. The
 *  \ref QMESH_PROVISION_COMPLETE_EVENT indicates that the procedure is complete.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] pri_elm_addr  Address to be assigned to the primary element.
 *  \param[in] uuid          128-bit UUID of the device, in big endian format.
 *                           See \ref QMESH_UUID_T
 *  \param[in] attn_time     Attention duration during provisioning.
 *
 *  \return QMESH_RESULT_T: Result of operation
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshProvisionDevice(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info, uint16_t pri_elm_addr,
                                           const QMESH_DEVICE_UUID_T uuid, uint8_t attn_time);

/*----------------------------------------------------------------------------*
 * QmeshSelectCapability
 */
/*! \brief Selects the capability of the peer device for OOB action.
 *
 *  \param[in] sel_cap  Pointer to the selected capability.
 *                      See \ref QMESH_PROV_SELECTED_CAPS_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSelectCapability(const QMESH_PROV_SELECTED_CAPS_T *sel_cap);

/*----------------------------------------------------------------------------*
 * QmeshSetDevicePublicKey
 */
/*! \brief Sets the public key of the peer device. This needs to be called only
 *         if OOB is selected for public key exchange.
 *
 *  \param[in] oob_pub_key Pointer to the peer device's public key.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetDevicePublicKey(const uint16_t *oob_pub_key);

/*----------------------------------------------------------------------------*
 * QmeshSetOobData
 */
/*! \brief Sets the data for the selected OOB action.
 *
 *  The stack sends a \ref QMESH_OOB_ACTION_EVENT to the application if any of
 *  the OOB actions is selected.
 *
 *  The application must call this function upon receiving QMESH_OOB_ACTION_EVENT
 *  to provide the stack with authentication data to be used.
 *
 *  \param[in] oob_data Pointer to the OOB authentication data.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetOobData(const uint8_t *oob_data);

/*----------------------------------------------------------------------------*
 * QmeshStopProvision
 */
/*! \brief Stops ongoing provisioning procedure
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshStopProvision(void);

/*! @} */

/*! \addtogroup Core_Network
 * @{
 */

/*----------------------------------------------------------------------------*
 * QmeshAddNetwork
 */
/*! \brief Adds a new network on the provisioning device.
 *
 *  This function is only available when the device is in a provisioner role.
 *  It returns an error otherwise.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] net_key       Network key in little endian format.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAddNetwork(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                      const QMESH_NETWORK_KEY_T net_key);

/*----------------------------------------------------------------------------*
 * QmeshUpdateNetKey
 */
/*! \brief Updates the existing key at the specified key index.
 *
 *  This function updates an existing key and starts the key refresh procedure.
 *  It continues to use the existing key until the key refresh procedure is
 *  completed and all required devices in the network are updated with the
 *  new key.
 *
 *  This function is only available when the device is in a provisioner role.
 *  It returns an error otherwise.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] net_key       Network key in little endian format.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshUpdateNetKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                        const QMESH_NETWORK_KEY_T net_key);

/*----------------------------------------------------------------------------*
 * QmeshRemoveNetKey
 */
/*! \brief Removes a network key from the global network key list on the provisioner.
 *
 *  This function is only available when the device is in a provisioner role.
 *  It returns an error otherwise.
 *
 *  \note1hang This needs to be called only after removing the network key from the devices.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshRemoveNetKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info);

/*----------------------------------------------------------------------------*
 * QmeshAddAppKey
 */
/*! \brief Adds a new application key and binds that to a specified net key.
 *
 *  This function is only available when the device is in a provisioner role.
 *  It returns an error otherwise.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] app_key_idx   Global application key index for the provisioned network.
 *  \param[in] app_key       Application key in little endian format.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshAddAppKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                     uint16_t app_key_idx,
                                     const QMESH_APPLICATION_KEY_T app_key);

/*----------------------------------------------------------------------------*
 * QmeshUpdateAppKey
 */
/*! \brief Updates the existing application key at the specified key index.
 *
 *  This function updates an existing key and starts the key refresh procedure.
 *  It continues to use the existing key until the key refresh procedure is
 *  completed and all the required devices in the network are updated with the
 *  new key.
 *
 *  This function is only available when the device is in a provisioner role.
 *  It returns an error otherwise.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] app_key_idx   12-bit global application key index.
 *  \param[in] app_key       Application key in little endian format.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshUpdateAppKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                        uint16_t app_key_idx,
                                        const QMESH_APPLICATION_KEY_T app_key);

/*----------------------------------------------------------------------------*
 * QmeshRemoveAppKey
 */
/*! \brief Removes an application key from the global network key list on the provisioner.
 *
 *  This function is only available when the device is in a provisioner role.
 *  It returns an error otherwise.
 *
 *  \note1hang This needs to be called only after removing the application key from the devices.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] app_key_idx   12-bit global application key index.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \returns \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshRemoveAppKey(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                        uint16_t app_key_idx);

/*----------------------------------------------------------------------------*
 * QmeshUpdateKeyRefreshPhase
 */
/*! \brief Updates the key refresh phase.
 *
 *  The provisioner calls this function to update the key refresh phase. \n
 *  This function must be used along with the \ref QmeshUpdateNetKey and
 *  \ref QMESH_CONFIG_CLI_NETWORK_KEY_UPDATE to update the network on all devices in
 *  the network to complete a key refresh phase.
 *
 *  The provisioner calls the \ref QmeshUpdateNetKey() to update the key locally,
 *  then it calls \ref QMESH_CONFIG_CLI_NETWORK_KEY_UPDATE individually to update the
 *  key on each device. It then calls this function with the appropriate
 *  phase to indicate the current key refresh phase. Refer to the Mesh profile spec sec. 4.2.14.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] new_phase     New key refresh phase. See \ref QMESH_KEY_REFRESH_PHASE_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshUpdateKeyRefreshPhase(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                                 QMESH_KEY_REFRESH_PHASE_T new_phase);

/*----------------------------------------------------------------------------*
 * QmeshIncrementIV
 */
/*! \brief Increments the current IV index by one and triggers the IV index update
 *  procedure. After a key refresh, the provisioner can optionally initiate
 *  an IV update using this API.
 *
 * \param[in] prov_net_idx Provisioned network index.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshIncrementIV(uint8_t prov_net_idx);

/*----------------------------------------------------------------------------*
 * QmeshSetIV
 */
/*! \brief Sets the current IV index.
 *
 * \param[in] prov_net_idx Provisioned network index.
 * \param[in] iv_index 32-bit IV index in big endian format.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetIV(uint8_t prov_net_idx, uint32_t iv_index);

/*----------------------------------------------------------------------------*
 * QmeshSetIVRecoveryState
 */
/*! \brief Enables/Disables IV Index recovery mode. After Enabling IV recovery mode
 *           stack recovers IV and triggers QMESH_IV_RECOVERY_COMPLETE_EVENT to
 *           application, upon which application needs to disable IV recovery mode.
 *
 * \param[in] prov_net_idx Provisioned Network Index.
 * \param[in] enable Flag to enable or disable recovery mode.
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetIVRecoveryState(uint8_t prov_net_idx, bool enable);

/*----------------------------------------------------------------------------*
 * QmeshSendSecureNetworkBeacon
 */
/*! \brief Transmits a secure network beacon.
 *
 *  The application uses this API to send a secure network beacon.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSendSecureNetworkBeacon(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info);

/*! @} */

/*! \addtogroup Core_Transport
 * @{
 */

/*----------------------------------------------------------------------------*
 * QmeshSendMessage
 */
/*! \brief Sends an access layer message.
 *
 *  @note1hang
 *  This API queues the message for transmission, and the status it returns
 *  indicates whether the queueing was successful. \n
 *  A separate event QMESH_APP_PAYLOAD_EVENT with the appropriate status is
 *  sent when the message is actually attempted for transmission.
 *
 *  \param[in] key_info  Key used to secure the application payload \ref QMESH_ACCESS_PAYLOAD_KEY_INFO_T.
 *  \param[in] payload_info  Data information about the message, such as TTL
 *                       destination address, self element ID from which the message
 *                       is to be sent, etc. See \ref QMESH_ACCESS_PAYLOAD_INFO_T.
 *
 *  \returns \ref QMESH_RESULT_T
 *
 *  NOTE:
 *  1) This API queues the message for transmission, and the status it returns
 *     indicates whether the queueing was successful or not.
 *  2) A separate event QMESH_APP_PAYLOAD_EVENT with the appropriate status will
 *     be sent when the message is actually attempted for transmission.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSendMessage(const QMESH_ACCESS_PAYLOAD_KEY_INFO_T *key_info,
                                       const QMESH_ACCESS_PAYLOAD_INFO_T *payload_info);

/*----------------------------------------------------------------------------*
 * QmeshHeartBeatSetSubscriptionState
 */
/*! \brief Sets the heartbeat subscription state on the provisioner. On the
 *         device, this is done by the provisioner using configuration client
 *         commands.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                           See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[in] src           Source address.
 *  \param[in] dst           Destination address.
 *  \param[in] period_log    Duration to collect the logs.
 *
 *  \returns \ref QMESH_RESULT_T
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHeartBeatSetSubscriptionState(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                                         uint16_t src,
                                                         uint16_t dst,
                                                         uint8_t period_log);

/*----------------------------------------------------------------------------*
 * QmeshHeartBeatGetSubscriptionState
 */
/*! \brief Gets the heartbeat subscription state on the provisioner. On the
 *         device, this is done by the provisioner using configuration client
 *         commands.
 *
 *  \param[in] net_key_info  Provisioned network and subnetwork information.
 *                       See \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *  \param[out] subs_state    Pointer to the subscription state.
 *                       See \ref QMESH_HEARTBEAT_SUBSCRIPTION_STATE_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHeartBeatGetSubscriptionState(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                                         QMESH_HEARTBEAT_SUBSCRIPTION_STATE_T *subs_state);

/*----------------------------------------------------------------------------*
 * QmeshHeartBeatSetPublishState
 */
/*! \brief Sets the heartbeat publication state on the provisioner. On the
 *         device, this is done by the provisioner using configuration client
 *         commands.
 *
 *  \param[in] prov_net_id   Provisioned Network Index.
 *  \param[in] pub_state     Pointer to the heartbeat publication state.
 *                           See \ref QMESH_HEARTBEAT_PUBLICATION_STATE_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHeartBeatSetPublishState(uint8_t prov_net_id,
                          const QMESH_HEARTBEAT_PUBLICATION_STATE_T *pub_state);

/*----------------------------------------------------------------------------*
 * QmeshHeartBeatGetPublishState
 */
/*! \brief Gets the heartbeat publication state on the provisioner. On the
 *         device, this is done by the provisioner using configuration client
 *         commands.
 *
 *  \param[in] prov_net_id   Provisioned Network Index.
 *  \param[out] pub_state     Pointer to the heartbeat publication state.
 *                           See \ref QMESH_HEARTBEAT_PUBLICATION_STATE_T.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshHeartBeatGetPublishState(uint8_t prov_net_id,
                                QMESH_HEARTBEAT_PUBLICATION_STATE_T *pub_state);

/*! @} */

/*----------------------------------------------------------------------------*
 * QmeshSetModelPublicationState
 */
/*! \brief Configures the virtual address publication state. The provisioner must use
 *         this API to set its own publication state for the group/virtual address. In the device
 *         role, only a virtual address can be set using this API.
 *
 * \param[in] prov_net_id Provisioned net ID for which the publication state is to be configured.
 * \param[in] pub_config_t Publication configuration information.
 * \param[in] encoded_vr_addr In the case of a virtual address, the SIGMESH stack returns the encoded
 *        virtual address in this field provided the operation was successful. Otherwise, an unassigned
 *        address is returned. The application can use this encoded virtual address
 *        with \ref QmeshGetVirtualAddressUuid to get the actual generated virtual address
 *        and label UUID from which it was generated.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetModelPublicationState(uint8_t prov_net_id,
                                                    QMESH_PUBLICATION_CONFIG_T *pub_config,
                                                    uint16_t *encoded_vr_addr);

/*----------------------------------------------------------------------------*
 * QmeshSetModelSubscriptionState
 */
/*! \brief Configures the virtual address subscription state. The provisioner must use
 *         this API to set its own subscription data for the group/virtual address. In the device
 *         role, only a virtual address can be set using this API.
 *
 * \param[in] prov_net_id Provisioned net ID for which the publication state is to be configured.
 * \param[in] sub_config_t Subscription configuration information.
 * \param[in] op_code Operation on the subscription list. Refer to \ref QMESH_SUBSCRIBE_OPCODE_T.
 * \param[out] encoded_vr_addr In the case of a virtual address, the SIGMESH stack returns the encoded
 *        virtual address in this field for subscription add and overwrite provided the
 *        operation was successful. Otherwise, an unassigned address is returned. The application
 *        can use this encoded virtual address with \ref QmeshGetVirtualAddressUuid to get
 *        the actual generated virtual address and label UUID from which it was generated.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetModelSubscriptionState(uint8_t prov_net_id,
                                                     QMESH_SUBSCRIPTION_CONFIG_T *sub_config,
                                                     QMESH_SUBSCRIBE_OPCODE_T op_code,
                                                     uint16_t *encoded_vr_addr);

/*----------------------------------------------------------------------------*
 * QmeshGetVirtualAddressUuid
 */
/*! \brief Gets the virtual address and the corresponding UUID. The application can use
 *   this API when it needs to resolve the subscription/publication address only in the case
 *   of a virtual address.
 *
 * \param[in] prov_net      Provisioned Network Index.
 * \param[in] addr          Encoded virtual address.
 * \param[out] vr_addr       Resolved virtual address.
 * \param[out] label_uuid    128-bit corresponding UUID.
 *
 * \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGetVirtualAddressUuid(uint16_t addr,
                                                 uint16_t *vr_addr,
                                                 const uint16_t **label_uuid);

/*----------------------------------------------------------------------------*
 * QmeshSetVirtualAddress
 */
/*! \brief Adds/deletes a virtual address into the device's virtual table.
 *
 *  \param[in] prov_net     Provisioned Network Index.
 *  \param[in] label_uuid   16-octet label UUID.
 *  \param[in] add          If TRUE, add the virtual address to the table; otherwise delete the entry.
 *  \param[out] encoded_vr_addr Index into the virtual table at which the virtual address
 *                         is added. This field should be ignored while deleting from
 *                         the table.
 *
 *  \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetVirtualAddress(uint8_t prov_net_id,
                                             const QMESH_LABEL_UUID_T label_uuid,
                                             bool add,
                                             uint16_t *encoded_vr_addr);

/*----------------------------------------------------------------------------*
 * QmeshSetRelayState
 */
/*! \brief Sets the relay state of the device for a particular provisioned network.
 *
 * \param[in] prov_net          Provisioned network index.
 * \param[in] relay_state       Relay state to be set. See \ref QMESH_RELAY_STATE_T.
 * \param[in] repeat_count      Relay repeat count.
 * \param[in] repeat_interval   Relay repeat interval.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetRelayState(uint8_t prov_net, QMESH_RELAY_STATE_T relay_state,
                                         uint8_t repeat_count, uint8_t repeat_interval);

/*----------------------------------------------------------------------------*
 * QmeshSetNetworkTransmitState
 */
/*! \brief Sets the network transmit state.
 *
 * \param[in] prov_net    Provisioned network index.
 *
 * \param[in] nw_tx_state Network transmit state to be set.
 *                        See \ref QMESH_NETWORK_TRANSMIT_STATE_T.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetNetworkTransmitState(uint8_t prov_net,
                                                   QMESH_NETWORK_TRANSMIT_STATE_T nw_tx_state);

/*----------------------------------------------------------------------------*
 * QmeshGetNetworkTransmitState
 */
/*! \brief Gets the current network transmit state.
 *
 * \param[in] prov_net    Provisioned network index.
 *
 * \param[out] nw_tx_state Pointer to the variable to hold the network transmit state.
 *                        See \ref QMESH_NETWORK_TRANSMIT_STATE_T.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshGetNetworkTransmitState(uint8_t prov_net,
                                                   QMESH_NETWORK_TRANSMIT_STATE_T *nw_tx_state);

/*----------------------------------------------------------------------------*
 * QmeshClearRPLEntry
 */
/*! \brief Clears the replay protection entries for all the elements of the peer
 *         device for the specified provisioned network.
 *         NOTE: The application has to use this API after resetting a node by issuing
 *         QMESH_CONFIG_CLI_NODE_RESET (see \ref QMESH_CONFIG_CLIENT_CMD_T)
 *         via \ref QmeshConfigSendClientCmd.
 *
 *  \param[in] prov_net_id     Provisioned Network Index.
 *
 *  \param[in] elem_addr       Primary element address of the peer device for which
 *                             the replay protection entries needs to be cleared.
 *
 *  \param[in] num_elments     Number of elements in the peer device.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshClearRPLEntry(uint8_t prov_net_id, uint16_t elem_addr,
                                         uint8_t num_elements);

/*----------------------------------------------------------------------------*
 * QmeshNodeReset
 */
/*! \brief Does a Soft Reset for the specified provisioned network index.
 *
 *  \param[in] prov_net_id     Provisioned Network Index.
 *
 *  \return \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshNodeReset(uint8_t prov_net_id);

/*----------------------------------------------------------------------------*
* QmeshSetSecureNetworkBeaconState
*/
/*! \brief  Changes the secure network beacon broadcast state.
 *  \note1hang  This API can only be called after QmeshSendSecureNetworkBeacon()
 *              has been called.
 *
 * \param[in] prov_net_id   Provisioned Network Index.
 * \param[in] beacon_state  Secure network beacon broadcast state.
 *
 * \return \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshSetSecureNetworkBeaconState(uint8_t prov_net_id,
                                   QMESH_SECURE_NW_BEACON_STATE_T beacon_state);

/*! @} */

#ifdef __cplusplus
 }
#endif
#endif /* __QMESH_H__ */

