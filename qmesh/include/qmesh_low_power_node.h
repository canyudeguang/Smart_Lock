/*=============================================================================
 Copyright (c) 2016-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ============================================================================*/
 /*! \file qmesh_low_power_node.h
  *  \brief Qmesh API
  *
  *   This file contains the API for Qmesh Low Power Node feature.
  *
  */
/******************************************************************************/

#ifndef __QMESH_LOW_POWER_NODE_H__
#define __QMESH_LOW_POWER_NODE_H__

#include "qmesh_types.h"
#include "qmesh_friend_lpn_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/*! \addtogroup Core_Friendship
 * @{
 */

/*============================================================================*
    Macro Definitions
 *============================================================================*/
     
/*! \brief Time in milliseconds for which LPN should wait before going to sleep 
 * after sending a message so as to allow successful transmission of message.
 */
#define QMESH_LPN_WAIT_TIME_SLEEP                          (100)
     
/*! \brief Minimum value for LPN sleep.
 */
#define QMESH_LPN_MIN_TIME_SLEEP                           (10)

/*! \brief Data type for friend offer event */
typedef struct
{
    uint8_t    rcv_window;     /*!< Supported receive window. */
    uint8_t    cache_size;     /*!< Supported cache size. */
    uint8_t    sub_list_size;  /*!< Supported subscription list size. */
    uint8_t    rssi;           /*!< Supported rssi. */
    uint16_t   frnd_counter;   /*!< Friend counter. */
    uint16_t   src;            /*!< Source address. */
    uint16_t   offer_expiry_time[3]; /*!< Expected time of offer expiry. */
    QMESH_SUBNET_KEY_IDX_INFO_T subnet_key_info; /*!< Subnet information. */
} QMESH_FRND_OFFER_INFO_T;

/*============================================================================*
    Public Function Implementations
 *============================================================================*/


/*----------------------------------------------------------------------------*
 * QmeshLPNInit
 */
/*! \brief Initializes LPN mode on the device
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNInit(void);

/*----------------------------------------------------------------------------*
 * QmeshLPNFindFriend
 */
/*! \brief Starts discovering for Friend nodes
 *
 * \param[in] net_key_info Network on which LPN node is interested to find a friend.
 *                     Refer to \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 * \param[in] criteria Criteria a friend node should support.Refer to \ref QMESH_FRND_CRITERIA_T.
 * \param[in] rcv_delay Receive delay requested by the LPN node.
 * \param[in] poll_timeout Poll timeout set by the LPN node.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNFindFriend(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                         QMESH_FRND_CRITERIA_T *criteria, uint8_t rcv_delay,
                                         uint32_t poll_timeout);


/*----------------------------------------------------------------------------*
 * QmeshLPNAcceptFriend
 */
/*! \brief Establishes Friendship with the specified Friend node
 *
 * \param[in] frnd_offer_info Friend offer information of the friend with which
 *                        LPN wants to establish friendship.Refer to \ref QMESH_FRND_OFFER_INFO_T.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNAcceptFriend(QMESH_FRND_OFFER_INFO_T *frnd_offer_info);

/*----------------------------------------------------------------------------*
 * QmeshLPNWakeupStack
 */
/*! \brief Wakes up low power node after sleeping specified amount of time
 *
 *  \return QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNWakeupStack(void);

/*----------------------------------------------------------------------------*
 * QmeshLPNRemoveFriend
 */
/*! \brief Removes friendship on the specified network
 *
 * \param[in] net_key_info Network on which active friendship exsists.
 *                     Refer to \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNRemoveFriend(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info);

/*----------------------------------------------------------------------------*
 * QmeshLPNUpdateSubcriptionList
 */
/*! \brief Updates address to friend subscription list
 *
 * \param[in] net_key_info Network info on which friendship is established.Refer to \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 * \param[in] add Add/Remove operation to be performed on address list.
 * \param[in] msg Address List containing space allocated for transaction number.
 * \param[in] msg_len Message length.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNUpdateSubcriptionList(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info,
                                                    bool add, uint8_t *msg, uint8_t msg_len);

/*----------------------------------------------------------------------------*
 * QmeshLPNSendPollRequest
 */
/*! \brief Sends poll request to friend node
 *
 *  \param[in] net_key_info Network on which poll request has to be sent.
 *                      Refer to \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNSendPollRequest(QMESH_SUBNET_KEY_IDX_INFO_T net_key_info);

/*----------------------------------------------------------------------------*
 * QmeshLPNConfigureSubnet
 */
/*! \brief Informs LPN about the newly configured subnet
 *
 *  \param[in] net_key_info Pointer to network on which subnet was configured.
 *                      Refer to \ref QMESH_SUBNET_KEY_IDX_INFO_T.
 *
 *  \returns QMESH_RESULT_T. Refer to \ref QMESH_RESULT_T.
 */
/*----------------------------------------------------------------------------*/
extern QMESH_RESULT_T QmeshLPNConfigureSubnet(QMESH_SUBNET_KEY_IDX_INFO_T *net_key_info);

/*! @} */

#ifdef __cplusplus
 }
#endif
#endif /* __QMESH_LOW_POWER_NODE_H__ */

