/**
  ******************************************************************************
  * @file    test_versions.c
  * @author  MCD Application Team
  * @brief   image versions extraction
  *
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
#include <stdio.h>
#include "stm32h7rsxx_hal.h"
#include "test_versions.h"
#include "main.h"


/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Display the image Versions on HyperTerminal
  * @param  None.
  * @retval None.
  */
void TEST_VERSIONS_PrintVersions(void)
{
  struct image_header_s *AppliCodeHeader;

  /* Read appli code header */
  AppliCodeHeader = (struct image_header_s *)(APPLI_CODE_HEADER);

  /* Print version with same format than mcuboot log */
  printf("\r\n Appli Code version: %d.%d.%d\r\n", AppliCodeHeader->ih_ver.iv_major,
         AppliCodeHeader->ih_ver.iv_minor,
         AppliCodeHeader->ih_ver.iv_revision);
}

/**
  * @brief  Display the S Data on HyperTerminal
  * @param  None.
  * @retval None.
  */
