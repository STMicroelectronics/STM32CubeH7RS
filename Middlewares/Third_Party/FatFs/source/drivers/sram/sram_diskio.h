/**
  ******************************************************************************
  * @file    sram_diskio.h
  * @author  MCD Application Team
  * @brief   Header for sram_diskio.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the st_license.txt
  * file in the root directory of this software component.
  * If no st_license.txt file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SRAM_DISKIO_H
#define __SRAM_DISKIO_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
  /*
   * The sram_diskio_config.h is under application project
   * and contain the config parameters for SRAM diskio
   *
   */
#include "sram_diskio_config.h"
#include "ff_gen_drv.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern const Diskio_drvTypeDef  SRAMDISK_Driver;

#ifdef __cplusplus
}
#endif

#endif /* __SRAM_DISKIO_H */

