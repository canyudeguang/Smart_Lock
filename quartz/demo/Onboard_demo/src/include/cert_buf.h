/*
 * Copyright (c) 2018 Qualcomm Technologies, Inc.
 * All Rights Reserved.
 * Confidential and Proprietary - Qualcomm Technologies, Inc.
 */

#ifndef __CERT_BUF_H__
#define __CERT_BUF_H__
/**
 * @brief This buffer should contain the SSL Cert + Priv key
 *
 * This can be generated using SharkSSLParseCert <certfile> <private_key>
 * where certfile and private_key is downloaded for AWS Thing
 */
uint8_t aws_thing_privkey[] =
{
};

uint8_t aws_thing_cert[] =
{
};
 /* @brief This buffer should contain the CAList cert
 *
 * This can be generated using SharkSSLParseCAList <certfile>
 * where certfile is downloaded for AWS Thing
 */
uint8_t aws_calist[] =
{
};

#endif
