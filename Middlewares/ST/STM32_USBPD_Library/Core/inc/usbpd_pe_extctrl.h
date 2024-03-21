/**
  ******************************************************************************
  * @file    usbpd_pe_extctrl.h
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
#ifndef USBPD_PE_EXTCTRL_H_
#define USBPD_PE_EXTCTRL_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

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
/* Private macros ------------------------------------------------------------*/
/* Private variable ----------------------------------------------------------*/
#if defined(USBPDCORE_SNK) || defined(USBPDCORE_DRP)
extern const PE_GENMACH_TypeDef _Generic_AMS_Send_EPRKeepAlive_SNK;
extern const PE_GENMACH_TypeDef _Generic_AMS_Send_EPRSNKCapa;
extern const PE_GENMACH_TypeDef _Generic_AMS_Send_EPRGetSRCCapa;
#endif /* USBPDCORE_SNK || USBPDCORE_DRP */

#if defined(USBPDCORE_SNK) || defined(USBPDCORE_DRP)
extern const PE_GENMACH_TypeDef _Generic_AMS_Send_EPRKeepAliveAck_SRC;
extern const PE_GENMACH_TypeDef _Generic_AMS_Send_EPRGetSNKCapa;
extern const PE_GENMACH_TypeDef _Generic_AMS_Send_EPRSRCCapa;
#endif /* USBPDCORE_SNK || USBPDCORE_DRP */

/* Exported Private function prototypes -----------------------------------------------*/
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* USBPD_PE_EXTCTRL_H_ */
