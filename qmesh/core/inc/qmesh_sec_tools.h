/******************************************************************************
 Copyright (c) 2016-2017 Qualcomm Technologies International, Ltd.
 All Rights Reserved.
 Qualcomm Technologies International, Ltd. Confidential and Proprietary.
 ******************************************************************************/

#ifndef __QMESH_SEC_TOOLS_H__
#define __QMESH_SEC_TOOLS_H__

/*============================================================================*
 *  Local Header Files
 *===========================================================================*/
#include "qmesh_types.h"
#include "qmesh_security_ifce.h"

/*============================================================================*
 *  Public Definitions
 *============================================================================*/
typedef struct
{
    uint8_t      T1;
    uint16_t    *T2;
    uint16_t    *T3;
} QMESH_SEC_K2_NP_T;

/*============================================================================*
 *  Public Function Prototypes
 *============================================================================*/
extern void s1(const uint8_t *M, uint16_t M_len, uint16_t *salt);

extern void K1(const uint16_t *N, uint16_t N_len, const uint16_t *salt,
               const uint16_t *P, uint16_t P_len, uint16_t *k1_out);

extern void k2(const uint16_t *N, const uint8_t *P, uint16_t P_len, QMESH_SEC_K2_NP_T *K2);

extern void k3(const uint16_t *N, uint16_t *k3_out);

extern uint8_t k4(const uint16_t *N);

extern void QmeshGetPrivEncKeys(const uint16_t *net_key, QMESH_SEC_K2_NP_T *K2_data);

extern void QmeshGetNetworkID(const uint16_t *net_key, uint16_t *network_id);

extern void QmeshGetBeaconKey(const uint16_t *net_key, uint16_t *beacon_key);

extern void QmeshGetIdentityKey(const uint16_t *net_key, uint16_t *identity_key);


/*----------------------------------------------------------------------------*
 * generateVirtualAddress
 */
/*! \brief Generate the virtual address from Label UUID.
 *
 *  \param labelUUID 128 bits Label UUID.
 *
 *  \returns 16 bits Virtual Address
 */
/*----------------------------------------------------------------------------*/
extern uint16_t generateVirtualAddress(const QMESH_LABEL_UUID_T labelUUID);

#endif /* __QMESH_SEC_TOOLS_H__ */

