/**
  ******************************************************************************
  * @file    interfaces_conf.h
  * @author  MCD Application Team
  * @brief   Contains Interfaces configuration
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
#ifndef INTERFACES_CONF_H
#define INTERFACES_CONF_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_ll_usart.h"
#include "stm32h7rsxx_ll_i2c.h"
#include "stm32h7rsxx_ll_i3c.h"
#include "stm32h7rsxx_ll_spi.h"
#include "stm32h7rsxx_ll_gpio.h"
#include "stm32h7rsxx_ll_rcc.h"
#include "stm32h7rsxx_hal_fdcan.h"

#define MEMORIES_SUPPORTED                8U

/*-------------------------- Definitions for USART ---------------------------*/
#define USARTx                            UART4
#define USARTx_CLK_ENABLE()               __HAL_RCC_UART4_CLK_ENABLE()
#define USARTx_CLK_DISABLE()              __HAL_RCC_UART4_CLK_DISABLE()
#define USARTx_GPIO_CLK_TX_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_GPIO_CLK_RX_ENABLE()       __HAL_RCC_GPIOD_CLK_ENABLE()
#define USARTx_DEINIT()                   LL_USART_DeInit(USARTx)

#define USARTx_TX_PIN                     GPIO_PIN_1
#define USARTx_TX_GPIO_PORT               GPIOD
#define USARTx_RX_PIN                     GPIO_PIN_0
#define USARTx_RX_GPIO_PORT               GPIOD
#define USARTx_ALTERNATE                  GPIO_AF8_UART4

/*-------------------------- Definitions for I2C -----------------------------*/
#define I2Cx                              I2C1
#define I2Cx_CLK_ENABLE()                 __HAL_RCC_I2C1_CLK_ENABLE()
#define I2Cx_CLK_DISABLE()                __HAL_RCC_I2C1_CLK_DISABLE()
#define I2Cx_GPIO_CLK_SCL_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_GPIO_CLK_SDA_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I2Cx_DEINIT()                     LL_I2C_DeInit(I2Cx)

#define I2Cx_SCL_PIN                      GPIO_PIN_6
#define I2Cx_SCL_PIN_PORT                 GPIOB
#define I2Cx_SDA_PIN                      GPIO_PIN_9
#define I2Cx_SDA_PIN_PORT                 GPIOB
#define I2Cx_ALTERNATE                    GPIO_AF4_I2C1

#define I2C_ADDRESS                       (0x000000B0U << 0x01U)
#define OPENBL_I2C_TIMEOUT                0x01D00000U
#define I2C_TIMING                        0x00800000U

/*--------------------------- Definitions for SPI ----------------------------*/
#define SPIx                              SPI4
#define SPIx_CLK_ENABLE()                 __HAL_RCC_SPI4_CLK_ENABLE()
#define SPIx_CLK_DISABLE()                __HAL_RCC_SPI4_CLK_DISABLE()
#define SPIx_GPIO_CLK_SCK_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_GPIO_CLK_MISO_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_GPIO_CLK_MOSI_ENABLE()       __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_GPIO_CLK_NSS_ENABLE()        __HAL_RCC_GPIOE_CLK_ENABLE()
#define SPIx_DEINIT()                     LL_SPI_DeInit(SPIx)
#define SPIx_IRQ                          SPI4_IRQn

#define SPIx_MOSI_PIN                     GPIO_PIN_14
#define SPIx_MOSI_PIN_PORT                GPIOE
#define SPIx_MISO_PIN                     GPIO_PIN_13
#define SPIx_MISO_PIN_PORT                GPIOE
#define SPIx_SCK_PIN                      GPIO_PIN_12
#define SPIx_SCK_PIN_PORT                 GPIOE
#define SPIx_NSS_PIN                      GPIO_PIN_10
#define SPIx_NSS_PIN_PORT                 GPIOE
#define SPIx_ALTERNATE                    GPIO_AF5_SPI4

/*-------------------------- Definitions for FDCAN ---------------------------*/
#define FDCANx                            FDCAN2
#define FDCANx_CLK_ENABLE()               __HAL_RCC_FDCAN_CLK_ENABLE()
#define FDCANx_CLK_DISABLE()              __HAL_RCC_FDCAN_CLK_DISABLE()
#define FDCANx_GPIO_CLK_TX_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()
#define FDCANx_GPIO_CLK_RX_ENABLE()       __HAL_RCC_GPIOB_CLK_ENABLE()

#define FDCANx_TX_PIN                     GPIO_PIN_13
#define FDCANx_TX_GPIO_PORT               GPIOB
#define FDCANx_TX_AF                      GPIO_AF9_FDCAN2
#define FDCANx_RX_PIN                     GPIO_PIN_12
#define FDCANx_RX_GPIO_PORT               GPIOB
#define FDCANx_RX_AF                      GPIO_AF9_FDCAN2
#define FDCANx_DEINIT()                   do {                                   \
                                                __HAL_RCC_FDCAN_FORCE_RESET();   \
                                                __HAL_RCC_FDCAN_RELEASE_RESET(); \
                                              } while(0)

#define FDCANx_FORCE_RESET()              __HAL_RCC_FDCAN_FORCE_RESET()
#define FDCANx_RELEASE_RESET()            __HAL_RCC_FDCAN_RELEASE_RESET()

/*-------------------------- Definitions for I3C -----------------------------*/
#define I3Cx                              I3C1
#define I3Cx_CLK_ENABLE()                 __HAL_RCC_I3C1_CLK_ENABLE()
#define I3Cx_CLK_DISABLE()                __HAL_RCC_I3C1_CLK_DISABLE()
#define I3Cx_GPIO_CLK_SCL_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I3Cx_GPIO_CLK_SDA_ENABLE()        __HAL_RCC_GPIOB_CLK_ENABLE()
#define I3Cx_DEINIT()                     LL_I3C_DeInit(I3Cx)
#define I3Cx_EV_IRQ                       I3C1_EV_IRQn

#define I3Cx_SCL_PIN                      LL_GPIO_PIN_6
#define I3Cx_SCL_PORT                     GPIOB
#define I3Cx_SDA_PIN                      LL_GPIO_PIN_9
#define I3Cx_SDA_PORT                     GPIOB
#define I3Cx_ALTERNATE                    LL_GPIO_AF_4

#define OPENBL_I3C_TIMEOUT                0x000FFFFFU

/*-------------------------- Definitions for USB -----------------------------*/
#define USB_CLK_ENABLE()                  __HAL_RCC_USB_OTG_FS_CLK_ENABLE();
#define USB_CLK_DISABLE()                 __HAL_RCC_USB_OTG_FS_CLK_DISABLE();
#define USB_GPIO_CLK_ENABLE()             __HAL_RCC_GPIOM_CLK_ENABLE();
#define USB_DEINIT()                      do {                                        \
                                                __HAL_RCC_USB_OTG_FS_FORCE_RESET();   \
                                                __HAL_RCC_USB_OTG_FS_RELEASE_RESET(); \
                                              } while(0)

#define USB_FORCE_RESET()                 __HAL_RCC_USB_OTG_FS_FORCE_RESET();
#define USB_RELEASE_RESET()               __HAL_RCC_USB_OTG_FS_RELEASE_RESET();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* INTERFACES_CONF_H */
