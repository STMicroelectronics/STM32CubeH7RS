/* USER CODE BEGIN Header */
/**
******************************************************************************
* @file           : main.h
* @brief          : Header for main.c file.
*                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32h7s78_discovery.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
  
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
  /* Definition for HSPI clock resources */
#define XSPI1_CLK_ENABLE()                 __HAL_RCC_XSPI1_CLK_ENABLE()
#define XSPI1_CLK_DISABLE()                __HAL_RCC_XSPI1_CLK_DISABLE()
  
#define XSPI1_CLK_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOO_CLK_ENABLE()
#define XSPI1_DQS0_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOO_CLK_ENABLE()
#define XSPI1_DQS1_GPIO_CLK_ENABLE()       __HAL_RCC_GPIOO_CLK_ENABLE()
#define XSPI1_CS_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOO_CLK_ENABLE()
#define XSPI1_D0_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D1_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D2_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D3_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D4_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D5_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D6_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D7_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D8_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D9_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D10_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D11_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D12_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D13_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D14_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOP_CLK_ENABLE()
#define XSPI1_D15_GPIO_CLK_ENABLE()        __HAL_RCC_GPIOP_CLK_ENABLE()
  
#define XSPI1_CLK_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_DQS0_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_DQS1_GPIO_CLK_DISABLE()       __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_CS_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D0_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D1_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D2_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D3_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D4_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D5_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D6_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D7_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D8_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D9_GPIO_CLK_DISABLE()         __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D10_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D11_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D12_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D13_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D14_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
#define XSPI1_D15_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOP_CLK_DISABLE()
  
#define XSPI1_FORCE_RESET()                __HAL_RCC_XSPI1_FORCE_RESET()
#define XSPI1_RELEASE_RESET()              __HAL_RCC_XSPI1_RELEASE_RESET()
  
  /* Definition for HSPI Pins */
  /* HSPI_CLK */
#define XSPI1_CLK_PIN                      GPIO_PIN_4
#define XSPI1_CLK_GPIO_PORT                GPIOO
#define XSPI1_CLK_PIN_AF                   GPIO_AF9_XSPIM_P1
  /* HSPI_DQS0 */
#define XSPI1_DQS0_PIN                     GPIO_PIN_2
#define XSPI1_DQS0_GPIO_PORT               GPIOO
#define XSPI1_DQS0_PIN_AF                  GPIO_AF9_XSPIM_P1
  /* HSPI_DQS1 */
#define XSPI1_DQS1_PIN                     GPIO_PIN_3
#define XSPI1_DQS1_GPIO_PORT               GPIOO
#define XSPI1_DQS1_PIN_AF                  GPIO_AF9_XSPIM_P1
  /* HSPI_CS */
#define XSPI1_CS_PIN                       GPIO_PIN_0
#define XSPI1_CS_GPIO_PORT                 GPIOO
#define XSPI1_CS_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D0 */
#define XSPI1_D0_PIN                       GPIO_PIN_0
#define XSPI1_D0_GPIO_PORT                 GPIOP
#define XSPI1_D0_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D1 */
#define XSPI1_D1_PIN                       GPIO_PIN_1
#define XSPI1_D1_GPIO_PORT                 GPIOP
#define XSPI1_D1_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D2 */
#define XSPI1_D2_PIN                       GPIO_PIN_2
#define XSPI1_D2_GPIO_PORT                 GPIOP
#define XSPI1_D2_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D3 */
#define XSPI1_D3_PIN                       GPIO_PIN_3
#define XSPI1_D3_GPIO_PORT                 GPIOP
#define XSPI1_D3_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D4 */
#define XSPI1_D4_PIN                       GPIO_PIN_4
#define XSPI1_D4_GPIO_PORT                 GPIOP
#define XSPI1_D4_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D5 */
#define XSPI1_D5_PIN                       GPIO_PIN_5
#define XSPI1_D5_GPIO_PORT                 GPIOP
#define XSPI1_D5_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D6 */
#define XSPI1_D6_PIN                       GPIO_PIN_6
#define XSPI1_D6_GPIO_PORT                 GPIOP
#define XSPI1_D6_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D7 */
#define XSPI1_D7_PIN                       GPIO_PIN_7
#define XSPI1_D7_GPIO_PORT                 GPIOP
#define XSPI1_D7_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D8 */
#define XSPI1_D8_PIN                       GPIO_PIN_8
#define XSPI1_D8_GPIO_PORT                 GPIOP
#define XSPI1_D8_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D9 */
#define XSPI1_D9_PIN                       GPIO_PIN_9
#define XSPI1_D9_GPIO_PORT                 GPIOP
#define XSPI1_D9_PIN_AF                    GPIO_AF9_XSPIM_P1
  /* HSPI_D10 */
#define XSPI1_D10_PIN                      GPIO_PIN_10
#define XSPI1_D10_GPIO_PORT                GPIOP
#define XSPI1_D10_PIN_AF                   GPIO_AF9_XSPIM_P1
  /* HSPI_D11 */
#define XSPI1_D11_PIN                      GPIO_PIN_11
#define XSPI1_D11_GPIO_PORT                GPIOP
#define XSPI1_D11_PIN_AF                   GPIO_AF9_XSPIM_P1
  /* HSPI_D12 */
#define XSPI1_D12_PIN                      GPIO_PIN_12
#define XSPI1_D12_GPIO_PORT                GPIOP
#define XSPI1_D12_PIN_AF                   GPIO_AF9_XSPIM_P1
  /* HSPI_D13 */
#define XSPI1_D13_PIN                      GPIO_PIN_13
#define XSPI1_D13_GPIO_PORT                GPIOP
#define XSPI1_D13_PIN_AF                   GPIO_AF9_XSPIM_P1
  /* HSPI_D14 */
#define XSPI1_D14_PIN                      GPIO_PIN_14
#define XSPI1_D14_GPIO_PORT                GPIOP
#define XSPI1_D14_PIN_AF                   GPIO_AF9_XSPIM_P1
  /* HSPI_D15 */
#define XSPI1_D15_PIN                      GPIO_PIN_15
#define XSPI1_D15_GPIO_PORT                GPIOP
#define XSPI1_D15_PIN_AF                   GPIO_AF9_XSPIM_P1
  
  /* Aps256xx APMemory memory */
  
  /* Read Operations */
#define READ_CMD                                0x00
#define READ_LINEAR_BURST_CMD                   0x20
#define READ_HYBRID_BURST_CMD                   0x3F
  
  /* Write Operations */
#define WRITE_CMD                               0x80
#define WRITE_LINEAR_BURST_CMD                  0xA0
#define WRITE_HYBRID_BURST_CMD                  0xBF
  
  /* Reset Operations */
#define RESET_CMD                               0xFF
  
  /* Registers definition */
#define MR0                                     0x00000000
#define MR1                                     0x00000001
#define MR2                                     0x00000002
#define MR3                                     0x00000003
#define MR4                                     0x00000004
#define MR8                                     0x00000008
  
  /* Register Operations */
#define READ_REG_CMD                            0x40
#define WRITE_REG_CMD                           0xC0
  
  /* Default dummy clocks cycles */
#define DUMMY_CLOCK_CYCLES_READ                 4
#define DUMMY_CLOCK_CYCLES_WRITE                4
  
  /* Size of buffers */
#define BUFFERSIZE                              10240
#define KByte                                   1024
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
  
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
  
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
  
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
