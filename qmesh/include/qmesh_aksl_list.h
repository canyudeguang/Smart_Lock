/******************************************************************************
 Copyright (c) 2017-2018 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/
/*! \file qmesh_aksl_list.h
 *
 *  \brief Qmesh Application Key Index List and Subscription List Management API
 *
 *   This file contains the API for Application Key Index List and Subscription
 *   List Management
 */
 /*****************************************************************************/

#ifndef _QMESH_AKSL_LIST_H_
#define _QMESH_AKSL_LIST_H_

/*! \addtogroup AppKeyIndex_Subscription_List
 * @{
 */

/*============================================================================*
    Macro Definitions
 *============================================================================*/
/*! \brief Offset of the list type \ref QMESH_LIST_TYPE_T in list data structure. */
#define LIST_TYPE_OFFSET                    (0u)

/*! \brief Offset of the maximum capacity of the list. */
#define LIST_MAXNUM_OFFSET                  (1u)

/*! \brief Offset of the current number of valid entries in the list. */
#define LIST_CURRNUM_OFFSET                 (2u)

/*! \brief Offset of the actual data in the list. */
#define LIST_DATA_OFFSET                    (3u)

/*! \brief Size of list header in uint16_t half words. */
#define LIST_HEADER_SIZE                    (LIST_DATA_OFFSET)

/*! \brief Self Device Key index identifier. */
#define QMESH_SELF_DEVICE_KEY_IDX           (0x1000u)

/*! \brief Peer Device Key index identifier. */
#define QMESH_PEER_DEVICE_KEY_IDX           (0x2000u)

/*! \brief Default invalid AppKey Index list value. */
#define QMESH_INVALID_APPKEY_VALUE          (0xFFFFu)

/*! \brief Invalid Subscription list value. */
#define QMESH_INVALID_SUBLIST_VALUE         (QMESH_UNASSIGNED_ADDRESS)

/*! \brief Invalid AppKey Index or Subscription list handle. */
#define QMESH_LIST_HANDLE_INVALID           ((QMESH_LIST_HANDLE_T)NULL)

/*! \brief Macro to create a AppKey Index list statically.
 *
 *  \param name     Name to identify the list.
 *                  Actual name of the list variable will be akl_<name>
 *
 *  \param max_num  Maximum number of valid entries to be stored
 *                  in the list.
 *  \warning \ref QmeshDeleteList shall not be used on statically
 *           created lists.
 */
#define CREATE_STATIC_APPKEY_LIST(name, max_num)   \
    static uint16_t akl_##name[LIST_HEADER_SIZE + (max_num)] =  \
            {(uint16_t)QMESH_LIST_TYPE_APPKEY, (max_num), 0};   \

/*! \brief Macro to get the list handle for a statically allocated
 *         AppKey Index list.
 *
 *  \param name Name to identify the list.
 */
#define GET_STATIC_APPKEY_LIST(name)    ((QMESH_LIST_HANDLE_T)(akl_##name))

/*! \brief Macro to initialize the statically allocated
 *         AppKey Index list.
 *
 *  \param name Name of the list.
 */
#define INIT_STATIC_APPKEY_LIST(name)   \
    do { QmeshMemSet16(&akl_##name[LIST_DATA_OFFSET], \
                       QMESH_INVALID_APPKEY_VALUE,    \
                       akl_##name[LIST_MAXNUM_OFFSET]); } while(0)

/*! \brief Macro to create a Subscription list statically.
 *
 *  \param name     Name to identify the list.
 *                  Actual name of the list variable will be subl_<name>
 *
 *  \param max_num  Maximum number of valid entries to be stored
 *                  in the list.
 *
 *  \warning \ref QmeshDeleteList shall not be used on statically
 *           created lists.
 */
#define CREATE_STATIC_SUBS_LIST(name, max_num)   \
    static uint16_t subl_##name[LIST_HEADER_SIZE + (max_num)] =     \
            {(uint16_t)QMESH_LIST_TYPE_SUBSCRIPTION, (max_num), 0}; \

/*! \brief Macro to get the list handle for a statically allocated
 *         Subscription list.
 *
 *  \param name Name of the list.
 */
#define GET_STATIC_SUBS_LIST(name)    ((QMESH_LIST_HANDLE_T)(subl_##name))

/*! \brief Macro to initialize the statically allocated
 *         Subscription list.
 *
 *  \param name Name of the list.
 */
#define INIT_STATIC_SUBS_LIST(name)   \
    do { QmeshMemSet16(&subl_##name[LIST_DATA_OFFSET], \
                       QMESH_INVALID_SUBLIST_VALUE,    \
                       subl_##name[LIST_MAXNUM_OFFSET]); } while(0)

/*============================================================================*
    Enumeration Definitions
 *============================================================================*/

/*! \brief Type of the list. */
typedef enum
{
    QMESH_LIST_TYPE_APPKEY       = 0,   /*!< Appkey Index list. */
    QMESH_LIST_TYPE_SUBSCRIPTION = 1    /*!< Subscription list. */
} QMESH_LIST_TYPE_T;

/*============================================================================*
    Data Type Definitions
 *============================================================================*/

/*! \brief AppKey Index or Subscription List handle. */
typedef void* QMESH_LIST_HANDLE_T;

/*! \brief AppKey Index or Subscription List Data. */
typedef struct
{
    uint16_t            num_data;       /*!< Number of elements in the list. */
    const uint16_t     *data;           /*!< Pointer to the array of list members. */
} QMESH_LIST_DATA_T;

/*============================================================================*
    API Definitions
 *============================================================================*/

/*============Qmesh AppKey Index list or Subscription list management APIs=====*/

#ifdef __cplusplus
 extern "C" {
#endif

/*----------------------------------------------------------------------------*
 * QmeshCreateList
 */
/*! \brief Create an AppKey Index or Subscription list dynamically.
 *
 *  On successful execution, creates an AppKey or Subscription list dynamically,
 *  initializes them with invalid values (see \ref QMESH_INVALID_APPKEY_VALUE or
 *  \ref QMESH_INVALID_SUBLIST_VALUE) and returns a valid handle \ref QMESH_LIST_HANDLE_T.
 *  On failure returns invalid handle value \ref QMESH_LIST_HANDLE_INVALID.
 *
 *  \note1hang To delete a dynamically allocated list use \ref QmeshDeleteList
 *
 *  \param[in] list_type  Type of the list to be created. See \ref QMESH_LIST_TYPE_T
 *  \param[in] max_num    Maximum number of valid members to be stored in the list.
 *
 *  \returns See \ref QMESH_LIST_HANDLE_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_LIST_HANDLE_T QmeshCreateList(QMESH_LIST_TYPE_T list_type, uint16_t max_num);

/*----------------------------------------------------------------------------*
 * QmeshAddToList
 */
/*! \brief Adds a new value to the AppKey Index or Subscription list.
 *
 *  \note1hang
 *  If the list is already full, returns \ref QMESH_RESULT_INSUFFICIENT_RESOURCES.<br>
 *  If the value is already present, returns \ref QMESH_RESULT_SUCCESS
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *  \param[in] value        Value to be added to the list.
 *
 *  \returns See \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshAddToList(QMESH_LIST_HANDLE_T list_handle, uint16_t value);

/*----------------------------------------------------------------------------*
 * QmeshDeleteFromList
 */
/*! \brief Deletes a value from AppKey Index or Subscription list.
 *
 *  \note If the value is to be deleted is not present in the list,
 *        return value will be \ref QMESH_RESULT_SUCCESS
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *  \param[in] value        Value to be deleted from the list.
 *
 *  \returns See \ref QMESH_RESULT_T.
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshDeleteFromList(QMESH_LIST_HANDLE_T list_handle, uint16_t value);

/*----------------------------------------------------------------------------*
 * QmeshFindInList
 */
/*! \brief Search if a value is present in the list.
 *
 *  Returns TRUE if the value is present in the list. If the list handle or
 *  the value is invalid, then returns FALSE.
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *  \param[in] value        Value to be found in the list.
 *
 *  \returns TRUE if value is found in the list, otherwise FALSE.
 */
/*---------------------------------------------------------------------------*/
bool QmeshFindInList(QMESH_LIST_HANDLE_T list_handle, uint16_t value);

/*----------------------------------------------------------------------------*
 * QmeshGetListData
 */
/*! \brief This function gets the contents of the list.
 *
 *  This API fills the number of valid entries in the list and pointer to
 *  array of values, in the list_data \ref QMESH_LIST_DATA_T.
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *  \param[in] list_data    Pointer to the list data variable to be filled.
 *                          See \ref QMESH_LIST_DATA_T
 *
 *  \returns See \ref QMESH_RESULT_T
 */
/*---------------------------------------------------------------------------*/
QMESH_RESULT_T QmeshGetListData(QMESH_LIST_HANDLE_T list_handle, QMESH_LIST_DATA_T *list_data);

/*----------------------------------------------------------------------------*
 * QmeshIsListFull
 */
/*! \brief Checks if the AppKey Index or Subscription list is full.
 *
 *  Returns TRUE if the list is empty. If the list handle is an invalid value,
 *  then returns FALSE.
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *
 *  \returns TRUE if the list is full, otherwise FALSE.
 */
/*---------------------------------------------------------------------------*/
bool QmeshIsListFull(QMESH_LIST_HANDLE_T list_handle);

/*----------------------------------------------------------------------------*
 * QmeshIsListEmpty
 */
/*! \brief Checks if the AppKey Index or Subscription list is empty.
 *
 *  Returns TRUE if the list is empty. If the list handle is an invalid value,
 *  then returns FALSE.
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *
 *  \returns TRUE if the list is empty, otherwise FALSE.
 */
/*---------------------------------------------------------------------------*/
bool QmeshIsListEmpty(QMESH_LIST_HANDLE_T list_handle);

/*----------------------------------------------------------------------------*
 * QmeshClearList
 */
/*! \brief Clears the AppKey Index or Subscription list.
 *
 *  Clears the AppKey Index or Subscription list by setting the list
 *  contents to the invalid values. See \ref QMESH_INVALID_APPKEY_VALUE or
 *  \ref QMESH_INVALID_SUBLIST_VALUE
 *
 *  \param[in] list_handle  AppKey Index or Subscription list handle.
 *
 *  \returns None
 */
/*---------------------------------------------------------------------------*/
void QmeshClearList(QMESH_LIST_HANDLE_T list_handle);

/*----------------------------------------------------------------------------*
 * QmeshDeleteList
 */
/*! \brief Deletes the AppKey Index or Subscription list.
 *
 *  This API deletes the AppKey Index or Subscription list created dynamically
 *  using the API \ref QmeshCreateList
 *
 *  \param[in] list_handle  Pointer to the list handle. This will be set to
 *                          \ref QMESH_LIST_HANDLE_INVALID after deleting the list.
 *
 *  \returns None
 */
/*---------------------------------------------------------------------------*/
void QmeshDeleteList(QMESH_LIST_HANDLE_T *list_handle);

/*! @} */

#ifdef __cplusplus
 }
#endif

#endif /* _QMESH_AKSL_LIST_H_ */


