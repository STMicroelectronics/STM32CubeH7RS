/**
  ******************************************************************************
  * @file    loader.h
  * @author  MCD Application Team
  * @brief   Header for loader.c module
  *
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
#ifndef LOADER_H
#define LOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "boot_hal_iloader.h"

/* Exported constants --------------------------------------------------------*/

/**
  * @brief STiROT-iLoader suboperations for the operation "Update"
  */
#define LOADER_SUBOPERATION_SAVE_CODE              ILOADER_SUBOPERATION_SAVE(ILOADER_CODE_SLOT)     /*!< STiROT-iLoader suboperation "Save" */
#define LOADER_SUBOPERATION_ERASE_SECONDARY_DATA   ILOADER_SUBOPERATION_ERASE(ILOADER_DATA_SLOT)    /*!< STiROT-iLoader suboperation "Erase Secondary Data" */
#define LOADER_SUBOPERATION_ERASE_SECONDARY_CODE   ILOADER_SUBOPERATION_ERASE(ILOADER_CODE_SLOT)    /*!< STiROT-iLoader suboperation "Erase Secondary Code" */

/* Exported types ------------------------------------------------------------*/
/* External variables --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
STIROT_ILOADER_ErrorStatus LOADER_Run(void);

#ifdef __cplusplus
}
#endif

#endif /* LOADER_H */
