/**
  ******************************************************************************
  * @file    stm32_sfdp_driver.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for SFPD support
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
#include "stm32_extmem_conf.h"

#if EXTMEM_DRIVER_NOR_SFDP == 1
#include "stm32_sfdp_driver_type.h"
#include "stm32_sfdp_driver_api.h"
#include "stm32_sfdp_data.h"
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG) */
#include <string.h>

/** @defgroup NOR_SFDP NOR SFDP driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */



/* Private Macros ------------------------------------------------------------*/
/** @defgroup NOR_SFDP_Private_Macro DRIVER NOR SFDP Private Macro
  * @{
  */
/**
 * @brief erase command
 */
#define ERASE_COMMAND 0x60

/**
 * @brief MIN macro
 */
#define MIN(_A_,_B_)   ((_A_) > (_B_))?(_B_):(_A_);

/**
 * @brief default timeout
 */
#define DRIVER_DEFAULT_TIMEOUT 300

/**
 * @brief default clock value to read SFDP data
 */
#define DRIVER_SFDP_DEFAULT_CLOCK 50000000u

/**
 * @brief DEBUG macro
 */
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 1 && defined(EXTMEM_MACRO_DEBUG)
/**
 * @brief debug macro for a string
 * @param _STR_ string
 */
#define SFDP_DEBUG_STR(_STR_)  {           \
           EXTMEM_MACRO_DEBUG("\tSFDP::"); \
           EXTMEM_MACRO_DEBUG(_STR_);      \
           EXTMEM_MACRO_DEBUG("\n\r");     \
           }

/**
 * @brief debug macro for lowlevel
 */
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 2
#define DEBUG_DRIVER(_STR_)  {                     \
           EXTMEM_MACRO_DEBUG("\tSFDP::DRIVER::"); \
           EXTMEM_MACRO_DEBUG(_STR_);              \
           EXTMEM_MACRO_DEBUG("\n");               \
           }
#else
#define DEBUG_DRIVER(_STR_)
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 2 */

/**
 * @brief debug macro for an integer
 */
#define DEBUG_ID(_ID_)  {                                              \
                           char StrID[40];                             \
                           EXTMEM_MACRO_DEBUG("\tSFDP:: Flash ID(");   \
                           (void)sprintf(StrID, "0x%x:0x%x:0x%x:0x%x", \
                                   _ID_[0],_ID_[1],_ID_[2], _ID_[3]);  \
                           EXTMEM_MACRO_DEBUG(StrID);                  \
                           EXTMEM_MACRO_DEBUG(")\n");                  \
                        }


/**
 * @brief debug macro for error
 */
#define DEBUG_DRIVER_ERROR(_STR_)   SFDP_DEBUG_STR(_STR_)
#else
#define DEBUG_ID(_ID_)
#define SFDP_DEBUG_STR(_STR_)
#define DEBUG_DRIVER(_STR_)
#define DEBUG_DRIVER_ERROR(_STR_)
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 1 && defined(EXTMEM_MACRO_DEBUG) */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DRIVER_SFDP_Private_Functions DRIVER SFDP Private Functions
  * @{
  */
static EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef driver_set_FlagWEL(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Timeout);
__weak void EXTMEM_MemCopy( uint32_t* destination_Address, const uint8_t* ptrData, uint32_t DataSize);

/**
  * @}
  */

/** @defgroup DRIVER_SFDP_Exported_Functions DRIVER SFDP Exported Functions
  * @{
  */

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Init(void *Peripheral, EXTMEM_LinkConfig_TypeDef Config, uint32_t ClockInput, EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;
  SFPD_HeaderTypeDef JEDEC_SFDP_Header;
  uint8_t FreqUpdate = 0u;
  uint8_t DataID[6];
  uint32_t ClockOut;

  /* reset data of SFDPObject to zero */
  SFDP_DEBUG_STR("1 - reset data SFDPObject to zero")
  (void)memset((void *)&SFDPObject->sfpd_private, 0x0, sizeof(SFDPObject->sfpd_private));

  /* initialize the SFDPObject */
  SFDP_DEBUG_STR("2 - initialize the SFDPObject")
  SFDPObject->sfpd_private.Config = Config;
  SFDPObject->sfpd_private.DriverInfo.SpiPhyLink = PHY_LINK_1S1S1S;
  SFDPObject->sfpd_private.DriverInfo.ClockIn = ClockInput;
  SAL_XSPI_SET_SFDPDUMMYCYLE(SFDPObject->sfpd_private.SALObject, 8);

  /* set memory speed to 50Mhz maximum */
  SFDP_DEBUG_STR("3 - set memory link and speed to 50Mhz maximum")
  (void)SAL_XSPI_Init(&SFDPObject->sfpd_private.SALObject, Peripheral);
  (void)SAL_XSPI_SetClock(&SFDPObject->sfpd_private.SALObject, ClockInput, DRIVER_SFDP_DEFAULT_CLOCK, &ClockOut);

  /* Abort any ongoing XSPI action */
  (void)SAL_XSPI_DisableMapMode(&SFDPObject->sfpd_private.SALObject);

  /* analyse the SFPD structure to get driver information */
  SFDP_DEBUG_STR("4 - analyse the SFPD structure to get driver information")
  if(EXTMEM_SFDP_OK != SFDP_GetHeader(SFDPObject, &JEDEC_SFDP_Header))
  {
    /*
     *  for the future, we can try to get SFDP by using different mode
     *  the SFDP read is only performed in 1S1S1S mode
     */
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP;
    goto error;
  }

  /* Reset the memory */
  SFDP_DEBUG_STR("5 - reset the memory")
  if(EXTMEM_SFDP_OK != SFDP_MemoryReset(SFDPObject))
  {
    /*
     *  for the future, we can try to get SFDP by using different mode
     *  the SFDP read is only performed in 1S1S1S mode
     */
    SFDP_DEBUG_STR("ERROR::on the call of SFDP_MemoryReset but no error returned")
  }

  /* wait few ms after the reset operation, this is done to avoid issue on SFDP read */
  HAL_Delay(10);

  /* analyse the SFPD structure to get driver information after the reset */
  SFDP_DEBUG_STR("6 - analyse the SFPD structure to get driver information")
  if(EXTMEM_SFDP_OK != SFDP_GetHeader(SFDPObject, &JEDEC_SFDP_Header))
  {
    /*
     *  for the future, we can try to get SFDP by using different mode
     *  the SFDP read is only perform in 1S1S1S mode
     */
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP;
    goto error;
  }

  /* Save information from the SFDP table */
  SFDPObject->sfpd_private.Sfdp_param_number = JEDEC_SFDP_Header.param_number;
  SFDPObject->sfpd_private.Sfdp_AccessProtocol = JEDEC_SFDP_Header.AccessProtocol;

  /* read the flash ID */
  SFDP_DEBUG_STR("7 - read the flash ID")
  (void)SAL_XSPI_GetId(&SFDPObject->sfpd_private.SALObject, DataID, 4);
  DEBUG_ID(DataID);

  /* keep manufacturer information, it could be used to help in
     building of consistent driver */
  SFDPObject->sfpd_private.ManuID = DataID[0];

  /* get the SFDP data */
  SFDP_DEBUG_STR("8 - collect the SFDP data")
  if(EXTMEM_SFDP_OK != SFDP_CollectData(SFDPObject))
  {
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP;
    goto error;
  }

  /* setup the generic driver information and prepare the physical layer */
  SFDP_DEBUG_STR("9 - build the generic driver information and prepare the physical layer")
  if(EXTMEM_SFDP_OK !=  SFDP_BuildGenericDriver(SFDPObject, &FreqUpdate))
  {
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_BUILD")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_BUILD;
    goto error;
  }

  SFDP_DEBUG_STR("10 - adjust the frequency if required")
  if ((FreqUpdate == 0u) && (SFDPObject->sfdp_public.MaxFreq != 0u))
  {
    (void)SAL_XSPI_SetClock(&SFDPObject->sfpd_private.SALObject, ClockInput, SFDPObject->sfdp_public.MaxFreq, &ClockOut);
    SFDP_DEBUG_STR("--> new freq configured");
  }

  SFDP_DEBUG_STR("11 - read again the SFDP header to adjust memory type if necessary")
  if(EXTMEM_SFDP_OK != SFDP_ReadHeader(SFDPObject, &JEDEC_SFDP_Header))
  {
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_MEMTYPE_CHECK")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_MEMTYPE_CHECK;
    goto error;
  }

  (void)memset(DataID, 0xAA, sizeof(DataID));
  (void)SAL_XSPI_GetId(&SFDPObject->sfpd_private.SALObject, DataID, 4);
  DEBUG_ID(DataID);

error:
  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_DeInit(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  SFDPObject->sfpd_private.FlashSize = 0;
  return EXTMEM_DRIVER_NOR_SFDP_OK;
}

void EXTMEM_DRIVER_NOR_SFDP_GetFlashInfo(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, EXTMEM_NOR_SFDP_FlashInfoTypeDef *FlashInfo)
{
  DEBUG_DRIVER((uint8_t *)__func__)
  /* Format the info */
  FlashInfo->FlashSize      = SFDPObject->sfpd_private.FlashSize;
  FlashInfo->PageSize       = SFDPObject->sfpd_private.PageSize;
  FlashInfo->EraseType1Size = (SFDPObject->sfpd_private.DriverInfo.EraseType1Size == 0u) ? 0u:
                              ((uint32_t)1u << SFDPObject->sfpd_private.DriverInfo.EraseType1Size);
  FlashInfo->EraseType2Size = (SFDPObject->sfpd_private.DriverInfo.EraseType2Size == 0u) ? 0u:
                              ((uint32_t)1u << SFDPObject->sfpd_private.DriverInfo.EraseType2Size);
  FlashInfo->EraseType3Size = (SFDPObject->sfpd_private.DriverInfo.EraseType3Size == 0u) ? 0u:
                              ((uint32_t)1u << SFDPObject->sfpd_private.DriverInfo.EraseType3Size);
  FlashInfo->EraseType4Size = (SFDPObject->sfpd_private.DriverInfo.EraseType4Size == 0u) ? 0u:
                              ((uint32_t)1u << SFDPObject->sfpd_private.DriverInfo.EraseType4Size);
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Write(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Address, const uint8_t* Data, uint32_t Size)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  uint32_t size_write;
  uint32_t local_size = Size;
  uint32_t local_Address = Address;
  uint32_t local_Data = (uint32_t)Data;
  uint32_t misalignment = 0u;

  if (0u != (local_Address % SFDPObject->sfpd_private.PageSize))
  {
    misalignment = 1u;
  }

  DEBUG_DRIVER((uint8_t *)__func__)
  while(local_size != 0u) {

    if (misalignment == 1u)
    {
      size_write = SFDPObject->sfpd_private.PageSize - (local_Address % SFDPObject->sfpd_private.PageSize);
      size_write = MIN(local_size, size_write);
      misalignment = 0u;
    }
    else
    {
      size_write = MIN(local_size, SFDPObject->sfpd_private.PageSize);
    }

    /* check busy flag */
    retr = driver_check_FlagBUSY(SFDPObject, 5000u);
    if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_CHECK_BUSY")
      goto error;
    }

    /* wait for write enable flag */
    retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
    if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_CHECK_WEL")
      goto error;
    }

    /* Write the data */
    if (HAL_OK != SAL_XSPI_Write(&SFDPObject->sfpd_private.SALObject, SFDPObject->sfpd_private.DriverInfo.PageProgramInstruction, local_Address, (uint8_t *)local_Data, size_write))
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_WRITE")
      retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_WRITE;
      goto error;
    }

    local_size = local_size - size_write;
    local_Address = local_Address + size_write;
    local_Data = local_Data + size_write;
  }

  /* check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 5000);
#if EXTMEM_MACRO_DEBUG
  if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_CHECK_BUSY_ON_EXIT")
  }
#endif /* EXTMEM_MACRO_DEBUG */

error:
  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_WriteInMappedMode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Address, const uint8_t* Data, uint32_t Size)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;
  uint32_t size_write;
  uint32_t local_size = Size;
  uint32_t local_Address = Address;
  const uint8_t *local_Data = Data;
  uint32_t size;
  uint32_t misalignment = 0u;

  DEBUG_DRIVER((uint8_t *)__func__)

  /* check if the input address is 32bit aligned */
  if (0u != (local_Address % 4u))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_ADDRESS_ALIGNMENT;
    goto error;
  }

  if (0u != (local_Address % SFDPObject->sfpd_private.PageSize))
  {
    misalignment = 1u;
  }

  while(local_size != 0u) {

    if (misalignment == 1u)
    {
      size_write = SFDPObject->sfpd_private.PageSize - (local_Address % SFDPObject->sfpd_private.PageSize);
      size_write = MIN(local_size, size_write);
      misalignment = 0u;
    }
    else
    {
      if (local_size > SFDPObject->sfpd_private.PageSize)
      {
        size_write = SFDPObject->sfpd_private.PageSize;
      }
      else
      {
        size_write = local_size;
        if (0u != (local_size % sizeof(uint32_t)))
        {
          size_write = size_write + sizeof(uint32_t) - (local_size % sizeof(uint32_t));
        }
      }
    }

    /* wait for Write enable flag */
    retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
    if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_write::ERROR_CHECK_WEL")
      goto error;
    }

    /* enter the mapped mode */
    retr = EXTMEM_DRIVER_NOR_SFDP_Enable_MemoryMappedMode(SFDPObject);
    if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      goto error;
    }

    size = size_write;
    /* Execute the copy */
    EXTMEM_MemCopy((uint32_t *)local_Address, local_Data, size_write);

    /* enter the mapped mode */
    retr = EXTMEM_DRIVER_NOR_SFDP_Disable_MemoryMappedMode(SFDPObject);
    if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      goto error;
    }

    /* check busy flag */
    retr = driver_check_FlagBUSY(SFDPObject, 5000);
    if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_write::ERROR_CHECK_BUSY_ON_EXIT")
      goto error;
    }

    /* decrement the transfer size */
    if (local_size > size )
    {
      local_size = local_size - size;
      local_Data = &local_Data[size];
      local_Address = local_Address + size;
    }
    else
    {
      local_size = 0;
    }
  }

error:
  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Read(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Address, uint8_t* Data, uint32_t Size)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  DEBUG_DRIVER((uint8_t *)__func__)
  /* check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 5000);
  if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Read::ERROR_CHECK_BUSY")
    goto error;
  }

  if (HAL_OK != SAL_XSPI_Read(&SFDPObject->sfpd_private.SALObject, SFDPObject->sfpd_private.DriverInfo.ReadInstruction, Address, Data, Size))
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Read::ERROR_READ")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_READ;
  }
error :
  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_SectorErase(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Address, EXTMEM_DRIVER_NOR_SFDP_SectorTypeTypeDef SectorType)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  uint8_t command, size;
  uint32_t timeout;
  DEBUG_DRIVER((uint8_t *)__func__)

  /* check if the selected sector type is available */
  switch(SectorType)
  {
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE1:
        command = SFDPObject->sfpd_private.DriverInfo.EraseType1Command;
        size = SFDPObject->sfpd_private.DriverInfo.EraseType1Size;
        timeout = SFDPObject->sfpd_private.DriverInfo.EraseType1Timing;
      break;
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE2:
        command = SFDPObject->sfpd_private.DriverInfo.EraseType2Command;
        size = SFDPObject->sfpd_private.DriverInfo.EraseType2Size;
        timeout = SFDPObject->sfpd_private.DriverInfo.EraseType2Timing;
      break;
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE3:
        command = SFDPObject->sfpd_private.DriverInfo.EraseType3Command;
        size = SFDPObject->sfpd_private.DriverInfo.EraseType3Size;
        timeout = SFDPObject->sfpd_private.DriverInfo.EraseType3Timing;
      break;
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE4:
        command = SFDPObject->sfpd_private.DriverInfo.EraseType4Command;
        size = SFDPObject->sfpd_private.DriverInfo.EraseType4Size;
        timeout = SFDPObject->sfpd_private.DriverInfo.EraseType4Timing;
      break;
    default :
      retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SECTORTYPE;
      goto error;
      break;
  }

  /* check if the command for this sector size is available */
  if ( 0x0u == command )
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SECTORTYPE_UNAVAILABLE;
    goto error;
  }

  /* check @ alignment */
  if (0x0u != (Address % ((uint32_t)1u << size)))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_ADDRESS_ALIGNMENT;
    goto error;
  }

  /* check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 5000u);
  if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    goto error;
  }

  /* wait for write enable flag */
  retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr )
  {
    goto error;
  }

  /* launch erase command */
  (void)SAL_XSPI_CommandSendAddress(&SFDPObject->sfpd_private.SALObject, command, Address);

  /* check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, timeout); /* the timeout is set according the memory characteristic */
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    goto error;
  }


error:
  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_MassErase(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  DEBUG_DRIVER((uint8_t *)__func__)

  /* check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 1000);
  if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_read::ERROR_CHECK_BUSY")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_FLASHBUSY;
    goto error;
  }

  /* wait for write enable flag */
  retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_read::ERROR_CHECK_WEL")
    goto error;
  }

  /* launch mass erase command */
  (void)SAL_XSPI_CommandSendData(&SFDPObject->sfpd_private.SALObject, ERASE_COMMAND, NULL, 0);


  /* check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, SFDPObject->sfpd_private.DriverInfo.EraseChipTiming); /* time to used should be set according the memory characteristic */
  if ( EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_MassErase::ERROR_CHECK_BUSY_ON_EXIT")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_ERASE_TIMEOUT;
    goto error;
  }

error:
  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Enable_MemoryMappedMode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;

  /* enter the mapped mode */
  if (HAL_OK != SAL_XSPI_EnableMapMode(&SFDPObject->sfpd_private.SALObject, SFDPObject->sfpd_private.DriverInfo.ReadInstruction,
                                        (uint8_t)SFDPObject->sfpd_private.SALObject.Commandbase.DummyCycles,
                                        SFDPObject->sfpd_private.DriverInfo.PageProgramInstruction, 0))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_MAP_ENABLE;
  }

  return retr;
}

EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Disable_MemoryMappedMode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;

  /* exit the mapped mode */
  if (HAL_OK != SAL_XSPI_DisableMapMode(&SFDPObject->sfpd_private.SALObject))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_MAP_ENABLE;
  }

  return retr;
}


/**
  * @}
  */

/** @addtogroup DRIVER_SFDP_Internal_Functions DRIVER SFDP Internal Functions
  * @{
  */



/**
  * @}
  */

/** @addtogroup DRIVER_SFDP_Private_Functions DRIVER SFDP Private Functions
  * @{
  */

/**
 * @brief this function enables the WEL Flag and checks its activation
 *
 * @param SFDPObject memory object
 * @param timeout timeout value
 * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
 **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef driver_set_FlagWEL(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject, uint32_t Timeout)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_WRITEENABLE;
  DEBUG_DRIVER((uint8_t *)__func__)
  /* send the command write enable */
  (void)SAL_XSPI_CommandSendData(&SFDPObject->sfpd_private.SALObject, SFDPObject->sfpd_private.DriverInfo.WriteWELCommand, NULL, 0);

  /* wait for write enable status */
  if (0u != SFDPObject->sfpd_private.DriverInfo.ReadWELCommand)
  {
    /* check if flag write enable is enabled */
    if (HAL_OK == SAL_XSPI_CheckStatusRegister(&SFDPObject->sfpd_private.SALObject,
                                               SFDPObject->sfpd_private.DriverInfo.ReadWELCommand,
                                               SFDPObject->sfpd_private.DriverInfo.WELAddress,
                                               ((SFDPObject->sfpd_private.DriverInfo.WELBusyPolarity == 0u) ? 1u: 0u) << SFDPObject->sfpd_private.DriverInfo.WELPosition,
                                               1u << SFDPObject->sfpd_private.DriverInfo.WELPosition,
                                               Timeout))
    {
      retr = EXTMEM_DRIVER_NOR_SFDP_OK;
    }
  }
  return retr;
}

__weak void EXTMEM_MemCopy(uint32_t* destination_Address, const uint8_t* ptrData, uint32_t DataSize)
{
  uint32_t *ptrDest = destination_Address;
  /* Write the data */
  for (uint32_t index = 0u; index < DataSize; index = index + 4u){
    *ptrDest = ((uint32_t)ptrData[index] | ((uint32_t)ptrData[index+1u] << 8u) |
                ( (uint32_t)ptrData[index+2u] << 16u) | ((uint32_t)ptrData[index+3u] << 24u));
    ptrDest++;
  }
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
