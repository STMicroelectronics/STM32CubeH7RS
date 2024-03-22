/**
  ******************************************************************************
  * @file    optionbytes_interface.h
  * @author  MCD Application Team
  * @brief   Header for optionbytes_interface.c module
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
#ifndef OPTIONBYTES_INTERFACE_H
#define OPTIONBYTES_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
uint8_t OPENBL_OB_Read(uint32_t address);
void OPENBL_OB_Write(uint32_t address, uint8_t *p_data, uint32_t data_length);
void OPENBL_OB_Launch(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPTIONBYTES_INTERFACE_H */
