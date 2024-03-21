/**
  ******************************************************************************
  * @file    spi_interface.c
  * @author  MCD Application Team
  * @brief   Contains SPI HW configuration
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "platform.h"
#include "interfaces_conf.h"

#include "openbl_core.h"
#include "openbl_spi_cmd.h"

#include "app_openbootloader.h"
#include "spi_interface.h"
#include "iwdg_interface.h"
#include "external_memory_interface.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define SPI_DUMMY_BYTE                    0x00U  /* Dummy byte */
#define SPI_SYNC_BYTE                     0x5AU  /* Synchronization byte */
#define SPI_BUSY_BYTE                     0xA5U  /* Busy byte */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint8_t SpiRxValue;
static __IO uint8_t SpiRxNotEmpty = 0U;
static uint8_t SpiDetected        = 0U;

/* Exported variables --------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void OPENBL_SPI_Init(void);
#if defined (__ICCARM__)
__ramfunc void OPENBL_SPI_ClearFlag_OVR(void);
#else
__attribute__((section(".ramfunc"))) void OPENBL_SPI_ClearFlag_OVR(void);
#endif /* (__ICCARM__) */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  This function is used to initialize the SPI peripheral
  * @retval None.
  */
static void OPENBL_SPI_Init(void)
{
  LL_SPI_InitTypeDef spi_init_struct = {0U};

  /* Configure the SPIx IP:
      - Mode               : Slave.
      - Transfer Direction : Full Duplex.
      - Data width         : 8-bit.
      - Bit Order          : MSB first.
      - Polarity           : CPOL Low, CPHA Low.
      - NSS                : Software.
  */
  spi_init_struct.TransferDirection = LL_SPI_FULL_DUPLEX;
  spi_init_struct.Mode              = LL_SPI_MODE_SLAVE;
  spi_init_struct.DataWidth         = LL_SPI_DATAWIDTH_8BIT;
  spi_init_struct.ClockPolarity     = LL_SPI_POLARITY_LOW;
  spi_init_struct.ClockPhase        = LL_SPI_PHASE_1EDGE;
  spi_init_struct.NSS               = LL_SPI_NSS_SOFT;
  spi_init_struct.BaudRate          = LL_SPI_BAUDRATEPRESCALER_DIV2;
  spi_init_struct.BitOrder          = LL_SPI_MSB_FIRST;
  spi_init_struct.CRCCalculation    = LL_SPI_CRCCALCULATION_DISABLE;
  spi_init_struct.CRCPoly           = 7U;

  if (LL_SPI_Init(SPIx, &spi_init_struct) != SUCCESS)
  {
    Error_Handler();
  }

  LL_SPI_SetFIFOThreshold(SPIx, LL_SPI_FIFO_TH_01DATA);

  /* In case the underrun flag is set, we send a busy byte */
  LL_SPI_SetUDRConfiguration(SPIx, LL_SPI_UDR_CONFIG_REGISTER_PATTERN);
  LL_SPI_SetUDRPattern(SPIx, SPI_BUSY_BYTE);

  HAL_NVIC_SetPriority(SPIx_IRQ, 3U, 0U);
  HAL_NVIC_EnableIRQ(SPIx_IRQ);

  LL_SPI_Enable(SPIx);
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  This function is used to configure SPI pins and then initialize the used SPI instance.
  * @retval None.
  */
void OPENBL_SPI_Configuration(void)
{
  GPIO_InitTypeDef gpio_init_struct = {0U};

  /* Enable all resources clocks ---------------------------------------------*/
  /* Enable used GPIOx clocks */
  SPIx_GPIO_CLK_SCK_ENABLE();
  SPIx_GPIO_CLK_MISO_ENABLE();
  SPIx_GPIO_CLK_MOSI_ENABLE();
  SPIx_GPIO_CLK_NSS_ENABLE();

  /* Enable SPI clock */
  SPIx_CLK_ENABLE();

  /* SPI4 pins configuration -------------------------------------------------*/
  /*
           +----------+
           |   SPI4   |
     +-----+----------+
     | MOSI|   PE14   |
     +-----+----------+
     | MISO|   PE13   |
     +-----+----------+
     | SCK |   PE12   |
     +-----+----------+
     | NSS |   PE10   |
     +-----+----------+ */

  /* Common configuration for SPIx PINs:
      Mode        : Alternate function.
      Output type : Push-Pull (No pull).
      Speed       : High speed.
  */
  gpio_init_struct.Mode      = GPIO_MODE_AF_PP;
  gpio_init_struct.Pull      = GPIO_NOPULL;
  gpio_init_struct.Speed     = GPIO_SPEED_FREQ_HIGH;
  gpio_init_struct.Alternate = SPIx_ALTERNATE;

  /* SPI MOSI pin configuration */
  gpio_init_struct.Pin = SPIx_MOSI_PIN;
  HAL_GPIO_Init(SPIx_MOSI_PIN_PORT, &gpio_init_struct);

  /* SPI MISO pin configuration */
  gpio_init_struct.Pin = SPIx_MISO_PIN;
  HAL_GPIO_Init(SPIx_MISO_PIN_PORT, &gpio_init_struct);

  /* SPI SCK pin configuration */
  gpio_init_struct.Pin = SPIx_SCK_PIN;
  HAL_GPIO_Init(SPIx_SCK_PIN_PORT, &gpio_init_struct);

  /* SPI NSS pin configuration */
  gpio_init_struct.Pin = SPIx_NSS_PIN;
  HAL_GPIO_Init(SPIx_NSS_PIN_PORT, &gpio_init_struct);

  OPENBL_SPI_Init();
}

/**
  * @brief  This function is used to De-initialize the SPI pins and instance.
  * @retval None.
  */
void OPENBL_SPI_DeInit(void)
{
  /* Only de-initialize the SPI if it is not the current detected interface */
  if (SpiDetected == 0U)
  {
    LL_SPI_Disable(SPIx);

    SPIx_CLK_DISABLE();
  }
}

/**
  * @brief  This function is used to detect if there is any activity on SPI protocol.
  * @retval Returns 1 if interface is detected else 0.
  */
uint8_t OPENBL_SPI_ProtocolDetection(void)
{
  /* Check if there is any activity on SPI */
  if (LL_SPI_IsActiveFlag_RXP(SPIx) != 0U)
  {
    /* Check that Synchronization byte has been received on SPI */
    if (LL_SPI_ReceiveData8(SPIx) == SPI_SYNC_BYTE)
    {
      SpiDetected = 1U;

      /* Enable the interrupt of Rx not empty buffer */
      LL_SPI_EnableIT_RXP(SPIx);

      /* Send synchronization byte */
      OPENBL_SPI_SendByte(SYNC_BYTE);

      OPENBL_SPI_SendAcknowledgeByte(ACK_BYTE);
    }
    else
    {
      SpiDetected = 0U;
    }
  }
  else
  {
    SpiDetected = 0U;
  }

  return SpiDetected;
}

/**
  * @brief  This function is used to get the command opcode from the host.
  * @retval Returns the command.
  */
uint8_t OPENBL_SPI_GetCommandOpcode(void)
{
  uint8_t command_opc;

  /* Check if there is any activity on SPI */
  while (OPENBL_SPI_ReadByte() != SPI_SYNC_BYTE)
  {}

  /* Get the command opcode */
  command_opc = OPENBL_SPI_ReadByte();

  /* Check the data integrity */
  if ((command_opc ^ OPENBL_SPI_ReadByte()) != 0xFFU)
  {
    command_opc = ERROR_COMMAND;
  }

  return command_opc;
}

/**
  * @brief  This function is used to read one byte from SPI pipe.
  *         Read operation is synchronized on SPI Rx buffer not empty interrupt.
  * @retval Returns the read byte.
  */
#if defined (__ICCARM__)
__ramfunc uint8_t OPENBL_SPI_ReadByte(void)
#else
__attribute__((section(".ramfunc"))) uint8_t OPENBL_SPI_ReadByte(void)
#endif /* (__ICCARM__) */
{
  uint8_t data;

  /* Wait until SPI Rx buffer not empty interrupt */
  while (SpiRxNotEmpty == 0U)
  {
    /* Refresh IWDG: reload counter */
    IWDG->KR = IWDG_KEY_RELOAD;
  }

  /* Reset the Rx not empty token */
  SpiRxNotEmpty = 0U;

  /* Read the SPI data register */
  data = SpiRxValue;

  /* Enable the interrupt of Rx not empty buffer */
  SPIx->IER |= SPI_IER_RXPIE;

  return data;
}

/**
  * @brief  This function is used to send one busy byte each receive interrupt through SPI pipe.
  *         Read operation is synchronized on SPI Rx buffer not empty interrupt.
  * @retval Returns the read byte.
  */
#if defined (__ICCARM__)
__ramfunc void OPENBL_SPI_SendBusyByte(void)
#else
__attribute__((section(".ramfunc"))) void OPENBL_SPI_SendBusyByte(void)
#endif /* (__ICCARM__) */
{
  /* Wait until SPI Rx buffer not empty interrupt */
  while (SpiRxNotEmpty == 0U)
  {
    /* Refresh IWDG: reload counter */
    IWDG->KR = IWDG_KEY_RELOAD;
  }

  /* Reset the Rx not empty token */
  SpiRxNotEmpty = 0U;

  /* Transmit the busy byte */
  *((__IO uint8_t *)&SPIx->TXDR) = SPI_BUSY_BYTE;

  /* Read bytes from the host to avoid the overrun */
  OPENBL_SPI_ClearFlag_OVR();

  /* Enable the interrupt of Rx not empty buffer */
  SPIx->IER |= SPI_IER_RXPIE;
}

/**
  * @brief  This function is used to send one byte through SPI pipe.
  * @param  byte The byte to be sent.
  * @retval None.
  */
#if defined (__ICCARM__)
__ramfunc void OPENBL_SPI_SendByte(uint8_t byte)
#else
__attribute__((section(".ramfunc"))) void OPENBL_SPI_SendByte(uint8_t byte)
#endif /* (__ICCARM__) */
{
  /* Wait until SPI transmit buffer is empty */
  while ((SPIx->SR & SPI_SR_TXP) == 0U)
  {}

  /* Transmit the data */
  *((__IO uint8_t *)&SPIx->TXDR) = byte;

  /* Clear underrun flag */
  SET_BIT(SPIx->IFCR, SPI_IFCR_UDRC);
}

/**
  * @brief  This function is used to send acknowledge byte through SPI pipe.
  * @param  acknowledge The acknowledge byte to be sent.
  * @retval None.
  */
void OPENBL_SPI_SendAcknowledgeByte(uint8_t acknowledge)
{
  /* Check the AN4286 for the acknowledge procedure */
  if (acknowledge == ACK_BYTE)
  {
    /* Send dummy byte */
    OPENBL_SPI_SendByte(SPI_DUMMY_BYTE);
  }

  OPENBL_SPI_SendByte(acknowledge);

  /* Wait for the host to send ACK synchronization byte */
  while (OPENBL_SPI_ReadByte() != ACK_BYTE)
  {}
}

/**
  * @brief  Handle SPI interrupt request.
  * @retval None.
  */
#if defined (__ICCARM__)
__ramfunc void OPENBL_SPI_IRQHandler(void)
#else
__attribute__((section(".ramfunc"))) void OPENBL_SPI_IRQHandler(void)
#endif /* (__ICCARM__) */
{
  uint32_t overrun_flag;
  uint32_t read_packet_available;
  uint32_t read_packet_interrupt_enable;

  /* Read SPIx Status Register flags */
  overrun_flag                = SPIx->SR & SPI_SR_OVR;
  read_packet_available        = SPIx->SR & SPI_SR_RXP;
  read_packet_interrupt_enable = SPIx->IER & SPI_IER_RXPIE;

  /* Check that SPIx Rx buffer not empty interrupt has been raised */
  if ((overrun_flag                   != SPI_SR_OVR)
      && (read_packet_available        == SPI_SR_RXP)
      && (read_packet_interrupt_enable == SPI_IER_RXPIE))
  {
    /* Set the Rx not empty token */
    SpiRxNotEmpty = 1U;

    /* Read SPI Rx data register value */
    SpiRxValue = SPIx->RXDR;

    /* Disable the interrupt of Rx not empty buffer */
    SPIx->IER &= ~SPI_IER_RXPIE;
  }

  if (((SPIx->SR & SPI_SR_OVR)        != RESET)
      && ((SPIx->SR & SPI_SR_RXP)     != RESET)
      && ((SPIx->IER & SPI_IER_RXPIE) != RESET))
  {
    /* Read bytes from the host to avoid the overrun */
    OPENBL_SPI_ClearFlag_OVR();
  }
}

/**
  * @brief  This function enables the send of busy state.
  * @retval None.
  */
void OPENBL_SPI_EnableBusyState(void)
{
  /* Since we are using the underrun configuration, we don't need to enable the busy state */
}

/**
  * @brief  This function disables the send of busy state.
  * @retval None.
  */
void OPENBL_SPI_DisableBusyState(void)
{
  /*
    NOTE: In this product, we use the LL_SPI_UDR_CONFIG_REGISTER_PATTERN configuration,
    where we send a busy byte in case an underrun error occurs.
    This is why we don't need to send the busy byte manually.
  */
}

/**
  * @brief  Clear overrun error flag
  * @note   Clearing this flag is done by a read access to the SPIx_DR
  *         register followed by a read access to the SPIx_SR register
  * @retval None
  */
#if defined (__ICCARM__)
__ramfunc void OPENBL_SPI_ClearFlag_OVR(void)
#else
__attribute__((section(".ramfunc"))) void OPENBL_SPI_ClearFlag_OVR(void)
#endif /* (__ICCARM__) */
{
  SET_BIT(SPIx->IFCR, SPI_IFCR_OVRC);
}

/**
  * @brief  This function is used to process and execute the special commands.
  *         The user must define the special commands routine here.
  * @param  p_special_cmd Pointer to the OPENBL_SpecialCmdTypeDef structure.
  * @retval Returns NACK status in case of error else returns ACK status.
  */
void OPENBL_SPI_SpecialCommandProcess(OPENBL_SpecialCmdTypeDef *p_special_cmd)
{
  ErrorStatus status;
  uint32_t address;

  switch (p_special_cmd->OpCode)
  {
    case SPECIAL_CMD_EXT_MEM_INIT:
      address = (((uint32_t)p_special_cmd->Buffer1[0] << 24) | ((uint32_t)p_special_cmd->Buffer1[1] << 16) \
                 | ((uint32_t)p_special_cmd->Buffer1[2] << 8) | ((uint32_t)p_special_cmd->Buffer1[3]));

      status = OPENBL_ExtMem_Init(address);

      /* Send data packet size (coded on 2 bytes with MSB sent first) */
      OPENBL_SPI_SendByte(0x00U);
      OPENBL_SPI_SendByte(0x00U);

      /* Send status packet size (coded on 2 bytes with MSB sent first) */
      OPENBL_SPI_SendByte(0x00U);
      OPENBL_SPI_SendByte(0x01U);

      if (status == SUCCESS)
      {
        OPENBL_SPI_SendByte(SPECIAL_CMD_SUCCESS);
      }
      else
      {
        OPENBL_SPI_SendByte(SPECIAL_CMD_ERROR);
      }

      break;

    case SPECIAL_CMD_EXT_MEM_ERASE:
      status = OPENBL_ExtMem_Erase(p_special_cmd);

      /* Send data packet size (coded on 2 bytes with MSB sent first) */
      OPENBL_SPI_SendByte(0x00U);
      OPENBL_SPI_SendByte(0x00U);

      /* Send status packet size (coded on 2 bytes with MSB sent first) */
      OPENBL_SPI_SendByte(0x00U);
      OPENBL_SPI_SendByte(0x01U);

      if (status == SUCCESS)
      {
        OPENBL_SPI_SendByte(SPECIAL_CMD_SUCCESS);
      }
      else
      {
        OPENBL_SPI_SendByte(SPECIAL_CMD_ERROR);
      }

      break;

    default:
      if (p_special_cmd->CmdType == OPENBL_SPECIAL_CMD)
      {
        /* Send NULL data size */
        OPENBL_SPI_SendByte(0x00U);
        OPENBL_SPI_SendByte(0x00U);

        /* Send NULL status size */
        OPENBL_SPI_SendByte(0x00U);
        OPENBL_SPI_SendByte(0x00U);

        /* NOTE: In case of any operation that prevents the code from returning to Middleware (reset operation...),
           to be compatible with the OpenBL protocol, the user must ensure sending the last ACK from here.
        */
      }
      else if (p_special_cmd->CmdType == OPENBL_EXTENDED_SPECIAL_CMD)
      {
        /* Send NULL status size */
        OPENBL_SPI_SendByte(0x00U);
        OPENBL_SPI_SendByte(0x00U);

        /* NOTE: In case of any operation that prevents the code from returning to Middleware (reset operation...),
           to be compatible with the OpenBL protocol, the user must ensure sending the last ACK from here.
        */
      }
      else
      {
        /* Nothing to do */
      }

      break;
  }
}
