/**
  ******************************************************************************
  * @file    common_interface.h
  * @author  MCD Application Team
  * @brief   Header for common_interface.c module
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
#ifndef COMMON_INTERFACE_H
#define COMMON_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
typedef void (*Function_Pointer)(void);

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

/* Get I2C_NI3C field from option byte OBW2SR register */
#define GET_FLASH_OBW2SR_I2C_NI3C()       READ_BIT(FLASH->OBW2SR, FLASH_OBW2SR_I2C_NI3C)

/* Exported functions --------------------------------------------------------*/
void Common_SetMsp(uint32_t TopOfMainStack);
void Common_EnableIrq(void);
void Common_DisableIrq(void);
FlagStatus Common_GetProtectionStatus(void);
void Common_SetPostProcessingCallback(Function_Pointer Callback);
void Common_StartPostProcessing(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* COMMON_INTERFACE_H */
