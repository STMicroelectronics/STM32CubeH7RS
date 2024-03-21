/**
  ******************************************************************************
  * @file    usbpd_pe_genericmachine.h
  * @author  MCD Application Team
  * @brief   This file provides the definition used for the management of short generic state machines
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef USBPD_PE_GENERICMACHINE_H_
#define USBPD_PE_GENERICMACHINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if !defined(_SIMULATOR)
#include "cmsis_compiler.h"
#endif /* !_SIMULATOR */

/** @addtogroup STM32_USBPD_LIBRARY
  * @{
  */

/** @addtogroup USBPD_CORE
  * @{
  */

/** @addtogroup USBPD_CORE_PE_GENMACH
  * @{
  */

/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
typedef enum
{
  GENMACH_EPRMODEENTER_SNK = 1,
  GENMACH_EPRMODEENTER_SRC,
  GENMACH_EPRMODEEXIT_SNK,
  GENMACH_EPRMODEEXIT_SRC,
  GENMACH_EPRKEEPALIVE_SNK,
  GENMACH_EPR_SRCCAPA,
  GENMACH_EPR_SNKCAPA,
  GENMACH_EPR_KEEPALIVEACK,
  GENMACH_EPR_GETSRC_CAPA,
  GENMACH_EPR_GETSNK_CAPA,
} USBPD_MachineID_TypeDef;


typedef enum
{
  GENMACH_STATUS_WAIT,
  GENMACH_STATUS_OK,
  GENMACH_STATUS_KO
} GENMACH_Status_TypeDef;

typedef enum
{
  ACTION_DIRECTION_SEND_MESSAGE, /*!< the generic state machine send a message     */
  ACTION_DIRECTION_WAIT_MESSAGE, /*!< the generic state machine wait for a message */
  ACTION_DIRECTION_ROOT,         /*!< the generic state machine manage direction   */
} PE_Direction_TypeDef;

typedef enum
{
  ACTION_TYPE_CONTROL_MESSAGE,
  ACTION_TYPE_DATA_MESSAGE,
  ACTION_TYPE_EXTENDED_MESSAGE
} PE_Type_TypeDef;

typedef __PACKED_STRUCT
{
  PE_Direction_TypeDef  direction    : 2;    /*!< SEND, WAIT                                          */
  PE_Type_TypeDef       type         : 6;    /*!< DATA, CONTROL EXTENDED                              */
  uint32_t              dataID       :16;    /*!< ID to read or send the data according the direction */
  uint32_t              SOP          : 4;    /*!< SOP id                                              */
  uint32_t              IndexTable   : 4;    /*!< index to jump the state machine in case of :
                                                    - message wait timeout (the state shall be PE_STATE_GENERIC)
                                                    - message wait KO (the state shall be PE_STATE_GENERIC)
                                                    - message sending KO (the state shall be PE_STATE_GENERIC)  */
} PE_GENMACH_Action_TypeDef;


typedef __PACKED_STRUCT
{
  PE_State_TypeDef               state;                                /*!< State to display the PE state machine                         */
  PE_State_TypeDef               state_next;                           /*!< Next state if different PE_STATE_GENERIC                      */
  PE_State_TypeDef               state_error;                          /*!< Error state if different PE_STATE_GENERIC                     */
  /* SEND message used to as next step when message is sent         */
  /* WAIT message used as next step if the message is not received  */
  uint16_t                       timing;                               /*!< Timing for next operation                                    */
  uint16_t (*ptrDataFunction)(USBPD_PE_HandleTypeDef * pPEHandle, USBPD_StatusTypeDef * RxEvent); /*!< function ptr used to handle the data */
  PE_GENMACH_Action_TypeDef      action;                               /*!< Action type                                                   */
} PE_GENMACH_Table_TypeDef;

typedef __PACKED_STRUCT
{
  PE_AMSSeq_TypeDef AMS;
  PE_GENMACH_Table_TypeDef *StateTable;
} PE_GENMACH_TypeDef;

/* Private macros ------------------------------------------------------------*/
/* Private variable ----------------------------------------------------------*/
/* Exported Private function prototypes -----------------------------------------------*/
/** @defgroup USBPD_CORE_PE_GENMACH_Private_Functions USBPD CORE PE MSG Private Functions
  * @{
  */
void PE_GENMACH_StateMachine_Init(USBPD_PE_HandleTypeDef *pPEHandle, USBPD_MachineID_TypeDef MachineID);
uint32_t PE_GENMACH_StateMachine_Run(USBPD_PE_HandleTypeDef *pPEHandle, USBPD_StatusTypeDef *RxEvent);

/**
  * @}
  */
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* USBPD_PE_GENERICMACHINE_H_ */
