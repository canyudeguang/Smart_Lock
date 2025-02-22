#ifndef OM_LP_TEST_APP_H
#define OM_LP_TEST_APP_H
/*===========================================================================

                                                               som_test_app.h
GENERAL DESCRIPTION
  SOM Test Application (used as reference for test teams and OEMs)

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright 2010-2015 by Qualcomm Technologies, Inc.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/qmesh.ioe/1.0/demo/qca402x/QMesh_demo/src/common/platform/qcli/lp/om_lp_test.h#1 $
  $DateTime: 2018/01/06 10:58:34 $ 
  $Author: vc03 $

============================================================================*/

/*===========================================================================

                           INCLUDE FILES

===========================================================================*/
#include "stdint.h"
#include "qapi.h"
#include "qapi_omtm.h"
#include "qcli.h"
#include "qcli_api.h"
#include "qapi_timer.h"

/*======================================================================
                          MACROS and DEFINES
 ======================================================================*/
#define TEST_INITIALIZED_ID         0xFEEDBEEF
#define SOM_TEST_GPIO_NUM           29
#define OM_SMEM_OM_TEST_CLIENT_ID (100)
#define LP_PRINTF_HANDLE qcli_LP_group

/*======================================================================
                          TYPES and ENUMS
 ======================================================================*/
extern QCLI_Group_Handle_t qcli_LP_group;

enum
{
  SOM_TEST_TIMER1,
  SOM_TEST_TIMER2,
/*  SOM_TEST_TIMER3,*/
  SOM_TEST_MAX_TIMERS  
};

enum
{
  SOM_TEST_DSR1,
  SOM_TEST_DSR2,
  SOM_TEST_DSR3,
  SOM_TEST_MAX_DSRS  
};


typedef struct
{
  uint32_t som_gpio_cnt;
  uint32_t fom_gpio_cnt;
  qbool_t gpio_event_pending; 
}gpio_test_t;


typedef struct
{
  uint32_t som_dsr_count[SOM_TEST_MAX_DSRS]; 
  uint32_t som_timer_count[SOM_TEST_MAX_TIMERS];
  qbool_t som_app_init_done;
  qbool_t som_app_exit_done;
}som_test_app_info_t;

typedef struct
{
  uint32_t dsr_cnt_idx;
  qapi_TIMER_handle_t* timer;
}som_test_ctxt_t;


enum
{
  FOM_SOM,
  FOM_MOM,  
  SOM_MOM,
  MAX_OM_TRANSITION_TYPES  
};

typedef enum
{
  FAIL,
  SUCCESS,  	
}om_transition_status_e;

typedef struct
{
  uint32_t curr_om;
  uint32_t dest_om;  
}om_test_map_t;


typedef struct
{
  uint32_t transition_count[MAX_OM_TRANSITION_TYPES];
  om_test_map_t test_map[MAX_OM_TRANSITION_TYPES];  
  uint32_t vector_cnt;
  uint32_t test_initialized;    
}om_test_info_t;

/* ToDo:Should be made QAPIs
*/
typedef void (*mom_cb_t)(uint32_t *dest_om, void *data);
void mom_wakeup_callback_register(mom_cb_t cb, void *data); 
void mom_set_wakeup_timer(uint32_t duration_ticks, uint32_t backoff_ticks);
void mom_oem_wakeup_handler(uint32_t *dest_om, void *data);

/* OMT used in SOM
*/
extern qapi_OMTM_Operating_Mode_t lp_omtm_operating_mode_tbl_sram[];

/* test counter used by Mom
*/
extern uint32_t mom_oem_counter;

/* OMTM operating mode table
*/
extern qapi_OMTM_Operating_Mode_t omtm_operating_mode_tbl_sram;

extern uint64_t timer_set_time[];
extern uint32_t omtm_get_curr_mode_id(void);
extern qapi_Status_t
qapi_OMTM_MOM_Register_Operating_Modes( qapi_OMTM_Operating_Mode_t *modes,
                                        uint32_t num_Modes, uint32_t cur_Mode );

/*======================================================================
                          FUNCTIONS
 ======================================================================*/
void som_app_init(void);
void som_app_entry(void);
void som_main(void);
void mom_main(void);
void main(void);

om_transition_status_e om_transition_test(void);
om_transition_status_e som_transition_test(void);
qbool_t reset_OM_Test_Vectors(void);
void om_transition_test_cb(uint32_t dest_mode_id, void *data);
void fom_register_operating_modes(void);

void som_timer1_cb(uint32_t data);
void som_timer2_cb(uint32_t data);
void som_timer3_cb(uint32_t data);

#endif
