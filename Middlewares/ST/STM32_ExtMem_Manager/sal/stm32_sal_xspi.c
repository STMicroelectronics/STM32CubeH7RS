/**
  ******************************************************************************
  * @file   stm32_sal_xspi.c
  * @author  MCD Application Team
  * @brief   This file is the software adaptation layer for XSPI
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if EXTMEM_SAL_XSPI_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /*EXTMEM_SAL_XSPI_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)*/

#if EXTMEM_SAL_XSPI == 1
#include "stm32_sal_xspi_type.h"
#include "stm32_sal_xspi_api.h"

/** @defgroup SAL_XSPI SAL_XSPI : Software adaptation layer for XSPI
  * @ingroup EXTMEM_SAL
  * @{
  */

/* Private Macros ------------------------------------------------------------*/
/** @defgroup SAL_XSPI_Private_Macros SAL XSPI Private Macros
  * @{
  */

#if EXTMEM_SAL_XSPI_DEBUG_LEVEL == 0 || !defined(EXTMEM_MACRO_DEBUG)
#define DEBUG_PARAM_BEGIN()
#define DEBUG_PARAM_DATA(_STR_)
#define DEBUG_PARAM_INT(_INT_ )
#define DEBUG_PARAM_END()
#define DEBUG_AUTOPOLLING(_DR_,_MVAL_,_MMASK_)
#define STR_PHY_LINK(_PHY_)
#else
/**
  * @brief trace header macro
  */
#define DEBUG_PARAM_BEGIN()     EXTMEM_MACRO_DEBUG("\t\tSALXSPI::");

const uint8_t phylink_string[][17] = {"PHY_LINK_1S1S1S",
                                      "PHY_LINK_1S1S2S",
                                      "PHY_LINK_1S2S2S",
                                      "PHY_LINK_1S1D1D",
                                      "PHY_LINK_4S4S4S",
                                      "PHY_LINK_4S4D4D",
                                      "PHY_LINK_4D4D4D",
                                      "PHY_LINK_1S8S8S",
                                      "PHY_LINK_8S8D8D",
                                      "PHY_LINK_8D8D8D",
                                      "PHY_LINK_RAM8"
#if defined(HAL_XSPI_DATA_16_LINES)									  
                                      ,"PHY_LINK_RAM16"
#endif /* defined(HAL_XSPI_DATA_16_LINES) */
									  };

#define STR_PHY_LINK(_PHY_)  phylink_string[_PHY_]

/**
  * @brief trace data string macro
  */
#define DEBUG_PARAM_DATA(_STR_) EXTMEM_MACRO_DEBUG(_STR_);

/**
  * @brief trace data integer macro
  */
#define DEBUG_PARAM_INT(_INT_ ) {                                                 \
                                  char str[10];                                   \
                                  (void)snprintf(str, sizeof(str), "0x%x", _INT_);\
                                  EXTMEM_MACRO_DEBUG(str);                        \
                                }
/**
  * @brief trace close macro
  */
#define DEBUG_PARAM_END()       EXTMEM_MACRO_DEBUG("\n");

#if EXTMEM_SAL_XSPI_DEBUG_LEVEL == 2
#define DEBUG_AUTOPOLLING(_DR_,_MVAL_,_MMASK_)                                               \
  {                                                                                          \
    char str[50];                                                                            \
    (void)snprintf(str, sizeof(str),"DR:0x%x::Ma:0x%x::MM:0x%x\n\r", _DR_, _MVAL_, _MMASK_); \
    DEBUG_PARAM_BEGIN()                                                                      \
    DEBUG_PARAM_DATA(str)                                                                    \
    DEBUG_PARAM_END()                                                                        \
  }
#else
#define DEBUG_AUTOPOLLING(_DR_,_MVAL_,_MMASK_)
#endif /* EXTMEM_SAL_XSPI_DEBUG_LEVEL == 2 */
#endif /* EXTMEM_SAL_XSPI_DEBUG_LEVEL == 0 || !defined(EXTMEM_MACRO_DEBUG) */

/**
  * @brief default SAL timeout (100 ms)
  */

#define SAL_XSPI_TIMEOUT_DEFAULT_VALUE (100U) 

/**
  * @}
  */

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
/** @defgroup SAL_XSPI_Private_Transfer SAL XSPI Dma transfer management definition
  * @{
  */
/**
  * @brief state of the transfer status
  */
typedef enum {
  SALXSPI_TRANSFER_NONE, /*!< */
  SALXSPI_TRANSFER_OK,
  SALXSPI_TRANSFER_ERROR
}SAL_XSPI_TRANSFER_STATUS;

/**
  * @brief variable of the transfer status
  */
volatile SAL_XSPI_TRANSFER_STATUS salXSPI_status = SALXSPI_TRANSFER_NONE;

/**
  * @}
  */
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

/* Private typedefs ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup SAL_XSPI_Private_Functions SAL XSP Private Functions
  * @{
  */
uint16_t XSPI_FormatCommand(uint8_t CommandExtension, uint32_t InstructionWidth, uint8_t Command);
HAL_StatusTypeDef XSPI_Transmit(SAL_XSPI_ObjectTypeDef *SalXspi, const uint8_t *Data);
HAL_StatusTypeDef XSPI_Receive(SAL_XSPI_ObjectTypeDef *SalXspi,  uint8_t *Data);
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
void SAL_XSPI_ErrorCallback(struct __XSPI_HandleTypeDef *hxspi);
void SAL_XSPI_CompleteCallback(struct __XSPI_HandleTypeDef *hxspi);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

/**
  * @}
  */

/* Exported variables ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/** @defgroup SAL_XSPI_Exported_Functions SAL XSP Exported Functions
  * @{
  */
HAL_StatusTypeDef SAL_XSPI_SetClock(SAL_XSPI_ObjectTypeDef *SalXspi, uint32_t ClockIn, uint32_t ClockRequested, uint32_t *ClockReal)
{
  HAL_StatusTypeDef retr = HAL_OK;
  uint32_t divider;

  if (ClockRequested == 0u)
  {
    retr = HAL_ERROR;
  }
  else
  {
    divider = (ClockIn / ClockRequested);
    if (divider >= 1u)
    {
      *ClockReal = ClockIn / divider;
      if (*ClockReal <= ClockRequested)
      {
        divider--;
      }
    }

#if 0  /* Only used for debug purpose */
    divider=+5;
    divider++;
    divider++;
    divider++;
    divider++;
    divider++;
    divider++;
    divider++;
    divider++;
#endif

    /* real clock calculation */
    *ClockReal = ClockIn / (divider + 1u);

    DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::CLOCK::"); DEBUG_PARAM_INT(divider); DEBUG_PARAM_END();
    MODIFY_REG(SalXspi->hxspi->Instance->DCR2, XSPI_DCR2_PRESCALER, (uint32_t)divider << XSPI_DCR2_PRESCALER_Pos);
  }

  return retr;
}

/*
* This function is used to configure the way to discuss with the memory
*
*/
HAL_StatusTypeDef SAL_XSPI_Init(SAL_XSPI_ObjectTypeDef *SalXspi, void *HALHandle)
{
  XSPI_RegularCmdTypeDef s_commandbase = {
      .OperationType = HAL_XSPI_OPTYPE_COMMON_CFG,
      .IOSelect = HAL_XSPI_SELECT_IO_7_0,
      .InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE,
      .Instruction = 0x5A,
      .InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS,
      .InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE,
      .AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE,
      .AddressMode = HAL_XSPI_ADDRESS_1_LINE,
      .AddressWidth = HAL_XSPI_ADDRESS_24_BITS,
      .Address = 0x0,
      .AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE,
      .DataMode = HAL_XSPI_DATA_1_LINE,
      .DataLength = 0x0,
      .DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE,
      .DummyCycles = 8,
      .DQSMode = HAL_XSPI_DQS_DISABLE,
#if defined(XSPI_CCR_SIOO)
      .SIOOMode = HAL_XSPI_SIOO_INST_EVERY_CMD,
#endif /* HAL_XSPI_SIOO_INST_EVERY_CMD */
  };

  SalXspi->hxspi = (XSPI_HandleTypeDef *)HALHandle;
  SalXspi->Commandbase = s_commandbase;
  SalXspi->CommandExtension = 0;
  SalXspi->PhyLink = PHY_LINK_1S1S1S;

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
  /* set completion call back */
  HAL_XSPI_RegisterCallback(SalXspi->hxspi,HAL_XSPI_RX_CPLT_CB_ID, SAL_XSPI_CompleteCallback);
  HAL_XSPI_RegisterCallback(SalXspi->hxspi,HAL_XSPI_TX_CPLT_CB_ID, SAL_XSPI_CompleteCallback);
  /* set the error callback */
  HAL_XSPI_RegisterCallback(SalXspi->hxspi,HAL_XSPI_ERROR_CB_ID, SAL_XSPI_ErrorCallback);
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

  return HAL_OK;
}

HAL_StatusTypeDef SAL_XSPI_MemoryConfig(SAL_XSPI_ObjectTypeDef *SalXspi, SAL_XSPI_MemParamTypeTypeDef ParametersType, void *ParamVal)
{
  HAL_StatusTypeDef retr = HAL_OK;
  XSPI_RegularCmdTypeDef s_commandbase = SalXspi->Commandbase;

  switch (ParametersType) {
  case PARAM_PHY_LINK:{
    SalXspi->PhyLink = *((SAL_XSPI_PhysicalLinkTypeDef *)ParamVal);
    DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::PARAM_PHY_LINK::");DEBUG_PARAM_DATA(STR_PHY_LINK(SalXspi->PhyLink));
    switch (SalXspi->PhyLink)
    {
    case PHY_LINK_1S1D1D:
    case PHY_LINK_1S2S2S:
    case PHY_LINK_1S1S2S:
    case PHY_LINK_1S1S1S: {
      s_commandbase.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
      s_commandbase.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      s_commandbase.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
      s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
      s_commandbase.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
      s_commandbase.DataMode = HAL_XSPI_DATA_1_LINE;
      s_commandbase.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
      s_commandbase.DummyCycles = 8;
      s_commandbase.DQSMode = HAL_XSPI_DQS_DISABLE;
      break;
    }

    case PHY_LINK_4S4D4D:
    case PHY_LINK_4S4S4S: {
      s_commandbase.InstructionMode = HAL_XSPI_INSTRUCTION_4_LINES;
      s_commandbase.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      s_commandbase.AddressMode = HAL_XSPI_ADDRESS_4_LINES;
      s_commandbase.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
      s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
      s_commandbase.DataMode = HAL_XSPI_DATA_4_LINES;
      s_commandbase.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
      s_commandbase.DummyCycles = 6;
      s_commandbase.DQSMode = HAL_XSPI_DQS_DISABLE;
      break;
      }
    case PHY_LINK_4D4D4D: {
      s_commandbase.InstructionMode = HAL_XSPI_INSTRUCTION_4_LINES;
      s_commandbase.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
      s_commandbase.AddressMode = HAL_XSPI_ADDRESS_4_LINES;
      s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
      s_commandbase.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_ENABLE;
      s_commandbase.DataMode = HAL_XSPI_DATA_4_LINES;
      s_commandbase.DataDTRMode = HAL_XSPI_DATA_DTR_ENABLE;
      s_commandbase.DummyCycles = 6;
      s_commandbase.DQSMode = HAL_XSPI_DQS_DISABLE;
      break;
    }
    case PHY_LINK_1S8S8S: {
      s_commandbase.InstructionMode = HAL_XSPI_INSTRUCTION_1_LINE;
      s_commandbase.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      s_commandbase.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
      s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
      s_commandbase.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_DISABLE;
      s_commandbase.DataMode = HAL_XSPI_DATA_8_LINES;
      s_commandbase.DataDTRMode = HAL_XSPI_DATA_DTR_DISABLE;
      s_commandbase.DummyCycles = 8;
      s_commandbase.DQSMode = HAL_XSPI_DQS_DISABLE;
      break;
    }
    case PHY_LINK_8S8D8D: {
      s_commandbase.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
      s_commandbase.InstructionWidth = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      s_commandbase.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
      s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
      s_commandbase.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_ENABLE;
      s_commandbase.DataMode = HAL_XSPI_DATA_8_LINES;
      s_commandbase.DataDTRMode = HAL_XSPI_DATA_DTR_ENABLE;
      s_commandbase.DummyCycles = 8;
      s_commandbase.DQSMode = HAL_XSPI_DQS_ENABLE;
      break;
    }

    case PHY_LINK_8D8D8D: {
      s_commandbase.InstructionMode = HAL_XSPI_INSTRUCTION_8_LINES;
      s_commandbase.InstructionWidth = HAL_XSPI_INSTRUCTION_16_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_ENABLE;
      s_commandbase.AddressMode = HAL_XSPI_ADDRESS_8_LINES;
      s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
      s_commandbase.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_ENABLE;
      s_commandbase.DataMode = HAL_XSPI_DATA_8_LINES;
      s_commandbase.DataDTRMode = HAL_XSPI_DATA_DTR_ENABLE;
      s_commandbase.DummyCycles = 20;
      s_commandbase.DQSMode = HAL_XSPI_DQS_ENABLE;
      break;
    }
    case PHY_LINK_RAM8:{
      s_commandbase.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
      s_commandbase.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      s_commandbase.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
      s_commandbase.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
      s_commandbase.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
      s_commandbase.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
      s_commandbase.DataMode           = HAL_XSPI_DATA_8_LINES;
      s_commandbase.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
      s_commandbase.DummyCycles        = 10;
      s_commandbase.DQSMode            = HAL_XSPI_DQS_ENABLE;
      break;
    }
#if defined(HAL_XSPI_DATA_16_LINES)
    case PHY_LINK_RAM16 :{
      s_commandbase.InstructionMode    = HAL_XSPI_INSTRUCTION_8_LINES;
      s_commandbase.InstructionWidth   = HAL_XSPI_INSTRUCTION_8_BITS;
      s_commandbase.InstructionDTRMode = HAL_XSPI_INSTRUCTION_DTR_DISABLE;
      s_commandbase.AddressMode        = HAL_XSPI_ADDRESS_8_LINES;
      s_commandbase.AddressWidth       = HAL_XSPI_ADDRESS_32_BITS;
      s_commandbase.AddressDTRMode     = HAL_XSPI_ADDRESS_DTR_ENABLE;
      s_commandbase.AlternateBytesMode = HAL_XSPI_ALT_BYTES_NONE;
      s_commandbase.DataMode           = HAL_XSPI_DATA_16_LINES;
      s_commandbase.DataDTRMode        = HAL_XSPI_DATA_DTR_ENABLE;
      s_commandbase.DummyCycles        = 10;
      s_commandbase.DQSMode            = HAL_XSPI_DQS_ENABLE;
      break;
    }
#endif /* defined(HAL_XSPI_DATA_16_LINES) */
    default:
      retr = HAL_ERROR;
      break;
    }
    DEBUG_PARAM_END();
    break;
  }
  case PARAM_ADDRESS_4BITS: {
    DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::PARAM_ADDRESS_4BITS"); DEBUG_PARAM_END();
    s_commandbase.AddressWidth = HAL_XSPI_ADDRESS_32_BITS;
    break;
  }
  case PARAM_FLASHSIZE:{
    uint8_t valParam = *((uint8_t *)ParamVal);
    DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::PARAM_FLASHSIZE::"); DEBUG_PARAM_INT(valParam); DEBUG_PARAM_END();
    MODIFY_REG(SalXspi->hxspi->Instance->DCR1, XSPI_DCR1_DEVSIZE, ((uint32_t)valParam) << XSPI_DCR1_DEVSIZE_Pos);
    break;
  }
  case PARAM_DUMMY_CYCLES:{
    uint8_t valParam = *((uint8_t *)ParamVal);
    DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::PARAM_DUMMY_CYCLES::"); DEBUG_PARAM_INT(valParam); DEBUG_PARAM_END();
    s_commandbase.DummyCycles = valParam;
    break;
  }
  default:
    DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::SAL_XSPI_MemoryConfig::ERROR"); DEBUG_PARAM_END();
    retr = HAL_ERROR;
    break;
  }
  SalXspi->Commandbase = s_commandbase;
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_GetSFDP(SAL_XSPI_ObjectTypeDef *SalXspi, uint32_t Address, uint8_t *Data, uint32_t DataSize)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;

  /* Initialize the read ID command */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, 0x5A);

  s_command.Address     = Address;
  s_command.DataLength  = DataSize;
  s_command.DummyCycles = SalXspi->SFDPDummyCycle;

  if (s_command.AddressMode == HAL_XSPI_ADDRESS_1_LINE)
  {
    s_command.AddressWidth = HAL_XSPI_ADDRESS_24_BITS;
  }

  if (s_command.DataDTRMode == HAL_XSPI_DATA_DTR_ENABLE)
  {
    s_command.DQSMode = HAL_XSPI_DQS_ENABLE;
  }
  else
  {
    s_command.DQSMode = HAL_XSPI_DQS_DISABLE;
  }

  /* Configure the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr  != HAL_OK)
  {
    goto error;
  }

  /* Reception of the data */
  retr = HAL_XSPI_Receive(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);

error:
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_GetId(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t *Data, uint32_t DataSize)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;

  /* Initialize the read ID command */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, 0x9F);

  s_command.DataLength  = DataSize;
  s_command.AddressMode = HAL_XSPI_ADDRESS_NONE;

  if  (s_command.InstructionMode == HAL_XSPI_INSTRUCTION_1_LINE)
  {
    s_command.DummyCycles       = 0;
    /* this behavior is linked with micron memory to read ID in 1S8S8S */
    s_command.DataMode = HAL_XSPI_DATA_1_LINE;
  }
  else
  {
    /* this behavior is valid for macromix and must be confirmed on the other memories */
    s_command.Address = 0;
    s_command.DummyCycles = 8;
  }

  /* Configure the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr  != HAL_OK)
  {
    goto error;
  }

  /* Reception of the data */
  retr = HAL_XSPI_Receive(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);

error:
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_Read(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command, uint32_t Address, uint8_t *Data, uint32_t DataSize)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;

  /* Initialize the read ID command */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  s_command.Address           = Address;
  s_command.DataLength        = DataSize;

  /* DTR management for single/dual/quad */
  switch(SalXspi->PhyLink)
  {
   case PHY_LINK_4S4D4D :{
     s_command.AddressDTRMode = HAL_XSPI_ADDRESS_DTR_ENABLE;
     s_command.DataDTRMode    = HAL_XSPI_DATA_DTR_ENABLE;
     s_command.DummyCycles = SalXspi->DTRDummyCycle;
   break;
   }
   case PHY_LINK_1S2S2S :{
     s_command.AddressMode = HAL_XSPI_ADDRESS_2_LINES;
     s_command.DataMode = HAL_XSPI_DATA_2_LINES;
   break;
   }
   case PHY_LINK_1S1S2S :{
     s_command.DataMode = HAL_XSPI_DATA_2_LINES;
   break;
   }
   default :{
     /* keep default parameters */
   break;
   }
  }

  /* Configure the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr  != HAL_OK)
  {
    goto error;
  }

  /* read the data */
  retr = XSPI_Receive(SalXspi, Data);

error:
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_Write(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command, uint32_t Address, const uint8_t *Data, uint32_t DataSize)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;

  /* Initialize the read ID command */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  s_command.Address           = Address;
  s_command.DataLength        = DataSize;
  s_command.DummyCycles       = 0u;
  s_command.DQSMode           = HAL_XSPI_DQS_DISABLE;

  /* Configure the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if (HAL_OK != retr)
  {
    goto error;
  }

  /* transmit data */
  retr = XSPI_Transmit(SalXspi, Data);

error:
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_CommandSendAddress(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                       uint32_t Address)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;

  /* Initialize the writing of status register */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  if (s_command.InstructionMode == HAL_XSPI_INSTRUCTION_1_LINE)
  {
    s_command.AddressMode = HAL_XSPI_ADDRESS_1_LINE;
  }

  s_command.Address           = Address;
  s_command.DummyCycles       = 0U;
  s_command.DataMode          = HAL_XSPI_DATA_NONE;
  s_command.DQSMode           = HAL_XSPI_DQS_DISABLE;

  /* Send the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_CommandSendData(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                           uint8_t *Data, uint16_t DataSize)
{
  XSPI_RegularCmdTypeDef   s_command = SalXspi->Commandbase;
  HAL_StatusTypeDef retr;

  /* Initialize the writing of status register */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.DummyCycles        = 0U;
  s_command.DataLength         = DataSize;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

  if (DataSize == 0u)
  {
    s_command.DataMode         = HAL_XSPI_DATA_NONE;
  }

  /* Send the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  if (( retr == HAL_OK) && (DataSize != 0u))
  {
    retr = HAL_XSPI_Transmit(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  }

  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_SendReadCommand(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                           uint8_t *Data, uint16_t DataSize)
{
  XSPI_RegularCmdTypeDef   s_command = SalXspi->Commandbase;
  HAL_StatusTypeDef retr;

  /* Initialize the writing of status register */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  s_command.AddressMode        = HAL_XSPI_ADDRESS_NONE;
  s_command.DummyCycles        = 0u;
  s_command.DataLength         = DataSize;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

  if (DataSize == 0u)
  {
    s_command.DataMode         = HAL_XSPI_DATA_NONE;
  }

  /* Send the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  if (( retr == HAL_OK) && (DataSize != 0u))
  {
    /* Get the data */
    retr = HAL_XSPI_Receive(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  }

  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_CommandSendReadAddress(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command,
                                                  uint32_t Address, uint8_t *Data, uint16_t DataSize)
{
  XSPI_RegularCmdTypeDef   s_command = SalXspi->Commandbase;
  HAL_StatusTypeDef retr;

  /* Initialize the writing of status register */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  s_command.Address            = Address;
  s_command.DummyCycles        = SalXspi->SFDPDummyCycle;
  s_command.DataLength         = DataSize;
  s_command.DQSMode            = HAL_XSPI_DQS_DISABLE;

  /* Send the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);

  if ( retr == HAL_OK)
  {
    /* Get the data */
    retr = HAL_XSPI_Receive(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  }

  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_CheckStatusRegister(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t Command, uint32_t Address, uint8_t MatchValue, uint8_t MatchMask, uint32_t Timeout)
{
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;
  XSPI_AutoPollingTypeDef  s_config = {
                                       .MatchValue    = MatchValue,
                                       .MatchMask     = MatchMask,
                                       .MatchMode     = HAL_XSPI_MATCH_MODE_AND,
                                       .AutomaticStop = HAL_XSPI_AUTOMATIC_STOP_ENABLE,
                                       .IntervalTime  = 0x10
                                      };
  HAL_StatusTypeDef retr;

  /* Initialize the writing of status register */
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, Command);

  s_command.DataLength     = 1u;
  s_command.DQSMode        = HAL_XSPI_DQS_DISABLE;

  if (s_command.InstructionMode == HAL_XSPI_INSTRUCTION_1_LINE)
  {
    // patch cypress to force 1 line on status read
    s_command.DataMode    = HAL_XSPI_DATA_1_LINE;
    s_command.AddressMode = HAL_XSPI_DATA_NONE;
    s_command.DummyCycles = 0u;
  }

  /* @ is used only in 8 LINES format */
  if (s_command.DataMode == HAL_XSPI_DATA_8_LINES)
  {
    s_command.AddressMode    = HAL_XSPI_ADDRESS_8_LINES;
    s_command.AddressWidth   = HAL_XSPI_ADDRESS_32_BITS;
    s_command.Address        = Address;
  }

  /* Send the command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr == HAL_OK)
  {
    retr = HAL_XSPI_AutoPolling(SalXspi->hxspi, &s_config, Timeout);
    DEBUG_AUTOPOLLING(SalXspi->hxspi->Instance->DR, s_config.MatchValue, s_config.MatchMask)
  }

  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  /* return status */
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_ConfigureWrappMode(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t WrapCommand, uint8_t WrapDummy)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;

  /* Initialize the read ID command */
  s_command.OperationType = HAL_XSPI_OPTYPE_WRAP_CFG;
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, WrapCommand);
  s_command.DummyCycles = WrapDummy;
  /* Configure the read command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr  != HAL_OK)
  {
    goto error;
  }

error:
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  /* return status */
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_EnableMapMode(SAL_XSPI_ObjectTypeDef *SalXspi, uint8_t CommandRead, uint8_t DummyRead,
                                         uint8_t CommandWrite, uint8_t DummyWrite)
{
  HAL_StatusTypeDef retr;
  XSPI_RegularCmdTypeDef s_command = SalXspi->Commandbase;
  XSPI_MemoryMappedTypeDef sMemMappedCfg = {0};

  /* Initialize the read ID command */
  s_command.OperationType = HAL_XSPI_OPTYPE_READ_CFG;
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, CommandRead);
  s_command.DummyCycles = DummyRead;
  /* Configure the read command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr  != HAL_OK)
  {
    goto error;
  }

  /* Initialize the read ID command */
  s_command.OperationType     = HAL_XSPI_OPTYPE_WRITE_CFG;
  s_command.Instruction = XSPI_FormatCommand(SalXspi->CommandExtension, s_command.InstructionWidth, CommandWrite);
  s_command.DummyCycles = DummyWrite;
  /* Configure the read command */
  retr = HAL_XSPI_Command(SalXspi->hxspi, &s_command, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  if ( retr  != HAL_OK)
  {
    goto error;
  }

  /* Activation of memory-mapped mode */
  sMemMappedCfg.TimeOutActivation  = HAL_XSPI_TIMEOUT_COUNTER_DISABLE;
  sMemMappedCfg.TimeoutPeriodClock = 0x50;
  retr = HAL_XSPI_MemoryMapped(SalXspi->hxspi, &sMemMappedCfg);

error:
  if (retr != HAL_OK )
  {
    /* abort any ongoing transaction for the next action */
    (void)HAL_XSPI_Abort(SalXspi->hxspi);
  }
  /* return status */
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_DisableMapMode(SAL_XSPI_ObjectTypeDef *SalXspi)
{
  __DSB();
  return HAL_XSPI_Abort(SalXspi->hxspi);
}


HAL_StatusTypeDef SAL_XSPI_UpdateMemoryType(SAL_XSPI_ObjectTypeDef *SalXspi, SAL_XSPI_DataOrderTypeDef DataOrder)
{
HAL_StatusTypeDef retr = HAL_OK;

  /* read the memory type value */
  uint32_t memorytype = READ_REG(SalXspi->hxspi->Instance->DCR1) & XSPI_DCR1_MTYP;

  switch(DataOrder)
  {
  case SAL_XSPI_ORDERINVERTED :
    if (memorytype == HAL_XSPI_MEMTYPE_MICRON) {
      memorytype = HAL_XSPI_MEMTYPE_MACRONIX;
    } else if (memorytype == HAL_XSPI_MEMTYPE_MACRONIX) {
      memorytype = HAL_XSPI_MEMTYPE_MICRON;
    } else {
      retr = HAL_ERROR;
    }
    MODIFY_REG(SalXspi->hxspi->Instance->DCR1, XSPI_DCR1_MTYP, memorytype);
    break;
  default :
    return HAL_ERROR;
    break;
  }

  DEBUG_PARAM_BEGIN(); DEBUG_PARAM_DATA("::SAL_XSPI_UpdateMemoryType::"); DEBUG_PARAM_INT(memorytype); DEBUG_PARAM_END();
  return retr;
}

HAL_StatusTypeDef SAL_XSPI_Abort(SAL_XSPI_ObjectTypeDef *SalXspi)
{
  return HAL_XSPI_Abort(SalXspi->hxspi);
}

/**
  * @}
  */

/** @defgroup SAL_XSPI_Private_Functions SAL XSP Private Functions
  * @{
  */
/**
  * @brief This function return a formatted command
  *
  * @param CommandExtension type of the command extension 0: the complement  1 : the same
  * @param InstructionWidth instruction width
  * @param Command command
  * @return the formatted command
  */
uint16_t XSPI_FormatCommand(uint8_t CommandExtension, uint32_t InstructionWidth, uint8_t Command)
{
  uint16_t retr;
  if  (InstructionWidth == HAL_XSPI_INSTRUCTION_16_BITS)
  {
    /* 0b00 The Command Extension is the same as the Command. (The Command / Command Extension has the same value for the whole clock period.)*/
    /* 0b01 The Command Extension is the inverse of the Command. The Command Extension acts as a confirmation of the Command */
    /* 0b11 Command and Command Extension forms a 16 bit command word :: Not yes handled */
    retr = ((uint16_t)Command << 8u);
    if (CommandExtension == 1u)
    {
       retr|=  (uint8_t)(~Command & 0xFFu);
    }
    else
    {
       retr|=  (uint8_t)(Command & 0xFFu);
    }
  }
  else
  {
    retr = Command;
  }

  return retr;
}

/**
  * @brief This function trasnmits the data
  *
  * @param SalXspi handle on the XSPI IP
  * @param Data data pointer
  * @return none
  */
HAL_StatusTypeDef XSPI_Transmit(SAL_XSPI_ObjectTypeDef *SalXspi, const uint8_t *Data)
{
HAL_StatusTypeDef retr;

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
  if (SalXspi->hxspi->hdmatx == NULL)
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  {
    /* transmit data */
    retr = HAL_XSPI_Transmit(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  }
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
  else
  {
    /* set completion call back */
    salXSPI_status = SALXSPI_TRANSFER_NONE;

    /* Reception of the data */
    retr = HAL_XSPI_Transmit_DMA(SalXspi->hxspi, (uint8_t *)Data);

    if (retr ==  HAL_OK)
    {
      /* wait data completion */
      while(salXSPI_status == SALXSPI_TRANSFER_NONE);
      if (salXSPI_status == SALXSPI_TRANSFER_ERROR)
      {
        retr = HAL_ERROR;
      }
    }
  }
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */

  return retr;
}

/**
  * @brief this is called to start the data transfer in polling mode or DMA
  *
  * @param SalXspi handle on the XSPI IP
  * @param Data data pointer
  * @return none
  */
HAL_StatusTypeDef XSPI_Receive(SAL_XSPI_ObjectTypeDef *SalXspi,  uint8_t *Data)
{
HAL_StatusTypeDef retr;

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
  if (SalXspi->hxspi->hdmarx == NULL)
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  {
    /* Reception of the data */
    retr = HAL_XSPI_Receive(SalXspi->hxspi, Data, SAL_XSPI_TIMEOUT_DEFAULT_VALUE);
  }
#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
  else
  {
    /* set completion call back */
    salXSPI_status = SALXSPI_TRANSFER_NONE;

    /* Reception of the data */
    retr = HAL_XSPI_Receive_DMA(SalXspi->hxspi, Data);

    if (retr ==  HAL_OK)
    {
      /* wait data completion */
      while(salXSPI_status == SALXSPI_TRANSFER_NONE);
      if (salXSPI_status == SALXSPI_TRANSFER_ERROR)
      {
        retr = HAL_ERROR;
      }
    }
  }
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
  return retr;
}

#if defined (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U)
/**
  * @brief this is called when a DMA transfer error occurs
  *
  * @param hxspi handle on the XSPI IP
  * @return none
  */
void SAL_XSPI_ErrorCallback(struct __XSPI_HandleTypeDef *hxspi)
{
  salXSPI_status = SALXSPI_TRANSFER_ERROR;
}

/**
  * @brief this is called when a DMA transfer is complete
  *
  * @param hxspi handle on the XSPI IP
  * @return none
  */
void SAL_XSPI_CompleteCallback(struct __XSPI_HandleTypeDef *hxspi)
{
  salXSPI_status = SALXSPI_TRANSFER_OK;
}
#endif /* (USE_HAL_XSPI_REGISTER_CALLBACKS) && (USE_HAL_XSPI_REGISTER_CALLBACKS == 1U) */
/**
  * @}
  */

/**
  * @}
  */

#endif /* EXTMEM_SAL_XSPI == 1 */
