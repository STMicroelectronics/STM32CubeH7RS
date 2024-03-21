/**
  ******************************************************************************
  * @file    mx25uw25645g_conf.h
  * @author  MCD Application Team
  * @brief   MX25UW25645G OctoSPI memory configuration template file.
  *          This file should be copied to the application folder and renamed
  *          to mx25uw25645g_conf.h
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX25UW25645G_CONF_H
#define MX25UW25645G_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32xxxx_hal.h"

/** @addtogroup BSP
  * @{
  */
#define CONF_OSPI_ODS                MX25UW25645G_CR_ODS_24   /* MX25UW25645G Output Driver Strength */

#define DUMMY_CYCLES_READ            8U
#define DUMMY_CYCLES_READ_OCTAL      6U
#define DUMMY_CYCLES_READ_OCTAL_DTR  6U
#define DUMMY_CYCLES_REG_OCTAL       4U
#define DUMMY_CYCLES_REG_OCTAL_DTR   4U

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* MX25UW25645G_CONF_H */
