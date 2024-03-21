/**
  ******************************************************************************
  * @file    boot_hal_iloader.h
  * @author  MCD Application Team
  * @brief   Header for iloader life-cycle
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
#ifndef BOOT_HAL_ILOADER_H
#define BOOT_HAL_ILOADER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/

/**
  * @brief Indexes of the different slots
  */
#define ILOADER_CODE_SLOT (0U) /* Index of the code slot*/
#define ILOADER_DATA_SLOT (1U) /* Index of the data slot*/

/**
  * @brief RssCmd values set by STiROT-Loader
  */
#define ILOADER_RSS_CMD_NO_LOAD                   (0U)      /*!< RssCmd value "noLoad" */
#define ILOADER_RSS_CMD_LOAD_EMPTY                (0x3ec4U) /*!< RssCmd value "empty load" */
#define ILOADER_RSS_CMD_LOAD_PC                   (0x3401U) /*!< RssCmd value "primary code is loaded" */
#define ILOADER_RSS_CMD_LOAD_SC                   (0xd242U) /*!< RssCmd value "secondary code is loaded" */
#define ILOADER_RSS_CMD_ERROR                     (0x6641U) /*!< RssCmd value "Error" */

#define HAS_ILOADER_UPDATE_SLOTS(RSSCmd, img) ((img == 0) ? (RSSCmd == ILOADER_RSS_CMD_LOAD_SC) ||\
                                               (RSSCmd == ILOADER_RSS_CMD_LOAD_SC_SD) :\
                                               (img == 1) ? (RSSCmd == ILOADER_RSS_CMD_LOAD_SD) ||\
                                               (RSSCmd == ILOADER_RSS_CMD_LOAD_SC_SD) ||\
                                               (RSSCmd == ILOADER_RSS_CMD_LOAD_PC_SD) :\
                                               0)

#define HAS_ILOADER_UPDATE_SLOTS_MASK(RSSCmd) ((RSSCmd == ILOADER_RSS_CMD_LOAD_SC) ||\
                                               (RSSCmd == ILOADER_RSS_CMD_LOAD_SC_SD) || \
                                               (RSSCmd == ILOADER_RSS_CMD_LOAD_SD) ||\
                                               (RSSCmd == ILOADER_RSS_CMD_LOAD_PC_SD))

/**
  * @brief Operations that can be performed by the STiROT-Loader
  */
#define ILOADER_OPERATION_LOAD   (0x0005c37aU) /*!< Copy available slots from the external NVM to the internal VM */
#define ILOADER_OPERATION_UPDATE (0x00146ac1U) /*!< Update slots from the internal VM to the external NVM (combined with slot suboperations) */

/**
  * @brief Suboperations that can be performed by the STiROT-Loader on the update operation
  * Suboperation is a bit field, (1 bit per suboperation, 3 bits per slots)
  * @param  slot is equal to ILOADER_CODE_SLOT or ILOADER_DATA_SLOT
  * @retval value of suboperation for the right slot
  */
#define ILOADER_SUBOPERATION_NONE(slot)      (0U << (slot * 3U)) /*!< No suboperation on this slot */
#define ILOADER_SUBOPERATION_SAVE(slot)      (1U << (slot * 3U)) /*!< save this slot */
#define ILOADER_SUBOPERATION_ERASE(slot)     (2U << (slot * 3U)) /*!< erase this slot */

/**
  * @brief Mask of available slot specific suboperations
  * not all operations can be done on each slot
  * a code slot can only be erased or saved
  * a data slot can only be erased
  */
#define ILOADER_SUBOPERATION_MASK (ILOADER_SUBOPERATION_ERASE(ILOADER_CODE_SLOT) | \
                                   ILOADER_SUBOPERATION_SAVE(ILOADER_CODE_SLOT) | \
                                   ILOADER_SUBOPERATION_ERASE(ILOADER_DATA_SLOT))

/* External variables --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/** @brief  Check the STiROT-iLoader operation
  * @param  __OPERATION__ Operation ID
  * @retval TRUE (__OPERATION__ is valid) or FALSE (__OPERATION__ is invalid)
  */
#define IS_ILOADER_OPERATION(__OPERATION__) ((__OPERATION__ == ILOADER_OPERATION_LOAD) || \
                                             (__OPERATION__ == ILOADER_OPERATION_UPDATE))

/** @brief  Check the STiROT-iLoader suboperation
  * @param  __SUBOPERATION__ Suboperation ID
  * @retval TRUE (__SUBOPERATION__ is valid) or FALSE (__SUBOPERATION__ is invalid)
  */
#define IS_ILOADER_SUBOPERATION(__OPERATION__, __SUBOPERATION__) \
  ((__OPERATION__ != ILOADER_OPERATION_UPDATE) || \
   (((__SUBOPERATION__ & ILOADER_SUBOPERATION_MASK) != 0U) && \
    ((__SUBOPERATION__ & (!ILOADER_SUBOPERATION_MASK)) == 0U)))

/** @brief  Check the value of RSS CMD
  * @param  __RSSCMD__ Suboperation ID
  * @retval TRUE (__RSSCMD__ is valid) or FALSE (__RSSCMD__ is invalid)
  */
#define IS_ILOADER_RSS_CMD(__RSSCMD__) ((__RSSCMD__ == ILOADER_RSS_CMD_NO_LOAD) \
                                        || (__RSSCMD__ == ILOADER_RSS_CMD_ERROR))

#ifdef __cplusplus
}
#endif

#endif /* BOOT_HAL_ILOADER_H */
