/* Copyright (c) 2011-2015.  Qualcomm Atheros, Inc.
All rights reserved.

Qualcomm Atheros Confidential and Proprietary.

*/

#ifndef __COEX_DEMO_H__ // [
#define __COEX_DEMO_H__

/**
   @brief Register coex commands with QCLI.
*/
void Initialize_Coex_Demo(qbool_t IsColdBoot);

/**
   @brief Prepares the Coex demo for a mode change.

   @return true if the mode transition can proceed or false if it should be
           aborted.
*/
qbool_t Coex_Prepare_Mode_Switch(Operating_Mode_t Next_Mode);

/**
   @brief Undoes preparation of the Coex demo for a mode change.
*/
void Coex_Cancel_Mode_Switch(void);

/**
   @brief Finalizes the mode change for the Coex demo.
*/
void Coex_Exit_Mode(Operating_Mode_t Next_Mode);

#endif // ] ifndef __COEX_DEMO_H__
