/**
  ******************************************************************************
  * @file    stm32_sal_SD_type.h
  * @author  MCD Application Team
  * @brief   This file contains the software adaptation layer SD functions
  *          prototypes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SAL_SD_TYPE_H
#define __SAL_SD_TYPE_H

#ifdef __cplusplus
 extern "C" {
#endif

/** @addtogroup SAL_SD
  * @ingroup EXTMEM_SAL
  * @{
  */

/** @defgroup SAL_SD
  * @{
  */

/* Includes ------------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_types SAL SD exported types
  * @{
  */

typedef struct {
   SD_HandleTypeDef     *hSD;            /*!< handle on the SD instance */
} SAL_SD_ObjectTypeDef;


/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SAL_SD_Exported_Functions SAL XSP Exported Functions
  * @{
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __SAL_SD_TYPE_H */
