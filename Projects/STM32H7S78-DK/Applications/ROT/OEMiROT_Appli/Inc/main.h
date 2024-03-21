/**
  ******************************************************************************
  * @file    main.h
  * @author  MCD Application Team
  * @brief   This file contains definitions for main application file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "com.h"
#include "appli_flash_layout.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#if defined(__ICCARM__)
extern uint32_t __vector_table;
#define CODE_START ((uint32_t)& __vector_table)
#elif defined(__CC_ARM) || defined(__ARMCC_VERSION)
extern void *__Vectors;
#define CODE_START ((uint32_t) & __Vectors)
#elif defined(__GNUC__)
extern void *g_pfnVectors;
#define CODE_START ((uint32_t)& g_pfnVectors)
#endif /* __ICCARM__ */
/* Exported macros -----------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* MAIN_H */
