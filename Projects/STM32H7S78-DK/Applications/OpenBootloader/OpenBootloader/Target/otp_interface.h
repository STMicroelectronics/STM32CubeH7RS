/**
  ******************************************************************************
  * @file    otp_interface.h
  * @author  MCD Application Team
  * @brief   Header for otp_interface.c module
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
#ifndef OTP_INTERFACE_H
#define OTP_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
uint8_t OPENBL_OTP_Read(uint32_t address);
void OPENBL_OTP_Write(uint32_t address, uint8_t *p_data, uint32_t data_length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* OPTIONBYTES_INTERFACE_H */
