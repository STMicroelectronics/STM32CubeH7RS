/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* The size of the plaintext is in words */
#define PLAINTEXT_SIZE    16 /* Plaintext size in Words */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RNG_HandleTypeDef hrng;

CRYP_HandleTypeDef hcryp;
uint32_t pKeySAES[4] = {0x2B7E1516,0x28AED2A6,0xABF71588,0x09CF4F3C};

/* USER CODE BEGIN PV */
static CRYP_ConfigTypeDef Conf;
uint32_t pKeySAES256[8] = {0x603DEB10 ,0x15CA71BE ,0x2B73AEF0 ,0x857D7781 ,0x1F352C07 ,0x3B6108D7 ,0x2D9810A3 ,0x0914DFF4};
uint32_t pInitVectSAES[4] = {0x00010203 , 0x04050607 , 0x08090A0B , 0x0C0D0E0F};

uint32_t Plaintext[16] = { 0x6BC1BEE2 ,0x2E409F96 ,0xE93D7E11 ,0x7393172A ,

                          0xAE2D8A57 ,0x1E03AC9C ,0x9EB76FAC ,0x45AF8E51 ,
                          0x30C81C46 ,0xA35CE411 ,0xE5FBC119 ,0x1A0A52EF ,
                          0xF69F2445 ,0xDF4F9B17 ,0xAD2B417B ,0xE66C3710};

uint32_t CiphertextECB128[16] = {0x3AD77BB4 ,0x0D7A3660 ,0xA89ECAF3 ,0x2466EF97 ,
                           0xF5D3D585 ,0x03B9699D ,0xE785895A ,0x96FDBAAF ,
                           0x43B1CD7F ,0x598ECE23 ,0x881B00E3 ,0xED030688 ,
                           0x7B0C785E ,0x27E8AD3F ,0x82232071 ,0x04725DD4};
uint32_t CiphertextECB256[16] = {0xF3EED1BD ,0xB5D2A03C ,0x064B5A7E ,0x3DB181F8 ,
                                    0x591CCB10 ,0xD410ED26 ,0xDC5BA74A ,0x31362870 ,
                                    0xB6ED21B9 ,0x9CA6F4F9 ,0xF153E7B1 ,0xBEAFED1D ,
                                    0x23304B7A ,0x39F9F3FF ,0x067D8D8F ,0x9E24ECC7};
uint32_t CiphertextCBC128[16] = {0x7649ABAC ,0x8119B246 ,0xCEE98E9B ,0x12E9197D ,
                           0x5086CB9B ,0x507219EE ,0x95DB113A ,0x917678B2 ,
                           0x73BED6B8 ,0xE3C1743B ,0x7116E69E ,0x22229516 ,
                           0x3FF1CAA1 ,0x681FAC09 ,0x120ECA30 ,0x7586E1A7};
uint32_t CiphertextCBC256[16] = {0xF58C4C04 ,0xD6E5F1BA ,0x779EABFB ,0x5F7BFBD6 ,
                          0x9CFC4E96 ,0x7EDB808D ,0x679F777B ,0xC6702C7D ,
                          0x39F23369 ,0xA9D9BACF ,0xA530E263 ,0x04231461 ,
                          0xB2EB05E2 ,0xC39BE9FC ,0xDA6C1907 ,0x8C6A9D1B};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_RNG_Init(void);
static void MX_SAES_CRYP_Init(void);
/* USER CODE BEGIN PFP */
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit);
static void MPU_Config(void);
static uint32_t FLASH_KEY_PROG(uint32_t index);
static uint32_t FLASH_KEY_LOAD(uint32_t index);
/* Used for storing the encrypted text */
uint32_t EncryptedText[PLAINTEXT_SIZE]={0};

/* Used for storing the decrypted text */
uint32_t DecryptedText[PLAINTEXT_SIZE]={0};

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* STM32H7RSxx HAL library initialization:
       - Systick timer is configured by default as source of time base, but user
             can eventually implement his proper time base source (a general purpose
             timer for example or other time source), keeping in mind that Time base
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and
             handled in milliseconds basis.
       - Set NVIC Group Priority to 4
       - Low Level Initialization
     */
  MPU_Config();
  /* USER CODE END 1 */

  /* Enable the CPU Cache */

  /* Enable I-Cache---------------------------------------------------------*/
  SCB_EnableICache();

  /* Enable D-Cache---------------------------------------------------------*/
  SCB_EnableDCache();

  /* MCU Configuration--------------------------------------------------------*/

  /* Update SystemCoreClock variable according to RCC registers values. */
  SystemCoreClockUpdate();

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN SysInit */
  /* Configure LD3 and LD1 */
  BSP_LED_Init(LD3);
  BSP_LED_Init(LD1);
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_RNG_Init();
  MX_SAES_CRYP_Init();
  /* USER CODE BEGIN 2 */
  /*=======================================ECB  Key128========================================*/
  /* Program an OB key into flash */
  if (FLASH_KEY_PROG(0) != 0) /* 128bits AHK key */
  {
   Error_Handler();
  }
  /* Option byte key loading */
  if (FLASH_KEY_LOAD(0) != 0) /* 128bits AHK key */
  {
  Error_Handler();
  }
  /*##-1- Encryption Phase #################################################*/
  if (HAL_CRYP_Encrypt(&hcryp, Plaintext, PLAINTEXT_SIZE, EncryptedText, TIMEOUT_VALUE) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }
  /*Compare results with expected buffer*/
  if(memcmp(EncryptedText, CiphertextECB128, PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*##-1- Decryption Phase #################################################*/
  if (HAL_CRYP_Decrypt(&hcryp,CiphertextECB128 , PLAINTEXT_SIZE, DecryptedText, TIMEOUT_VALUE) != HAL_OK)
  {
    /* Processing Error */
    Error_Handler();
  }
  /*Compare results with expected buffer*/
  if(memcmp(DecryptedText, Plaintext, PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }
  /*=======================================ECB  Key256========================================*/
  /* Program an OB key into flash */
  if (FLASH_KEY_PROG(2) != 0) /* 256bits AHK key */
  {
  Error_Handler();
  }
  /* Get the Secure AES parameters */
  HAL_CRYP_GetConfig(&hcryp, &Conf);
  /* Set the SAES parameters */
  Conf.KeySize = CRYP_KEYSIZE_256B;
  Conf.pKey    = pKeySAES256;
  /* Set the SAES Configuration*/
  HAL_CRYP_SetConfig(&hcryp, &Conf);
  /* Option byte key loading */
  if (FLASH_KEY_LOAD(2) != 0) /* 256bits AHK key */
  {
    /* Processing Error */
  Error_Handler();
  }
  /*##-1.1- Encryption Phase #################################################*/
  HAL_CRYP_Encrypt(&hcryp, Plaintext, 16, EncryptedText, TIMEOUT_VALUE);

  if(memcmp(EncryptedText, CiphertextECB256, PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*##-1.1- Decryption Phase #################################################*/
  HAL_CRYP_Decrypt(&hcryp,CiphertextECB256 , 16, DecryptedText, TIMEOUT_VALUE);
  /*Compare results with expected buffer*/
  if(memcmp(DecryptedText, Plaintext, PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }
  /*=======================================CBC  Key128========================================*/
  if (FLASH_KEY_PROG(0) != 0) /* 128bits AHK key */
  {
    /* Processing Error */
    Error_Handler();
  }
  /* Get the SAES parameters */
  HAL_CRYP_GetConfig(&hcryp, &Conf);
  /* Set the SAES parameters */
  Conf.KeySize = CRYP_KEYSIZE_128B;
  Conf.pKey = pKeySAES;
  Conf.Algorithm = CRYP_AES_CBC;
  Conf.pInitVect = pInitVectSAES;
  /* Set the SAES Configuration*/
  HAL_CRYP_SetConfig(&hcryp, &Conf);
  if (FLASH_KEY_LOAD(0) != 0) /* 128bits AHK key */
  {
    /* Processing Error */
    Error_Handler();
  }
  /*##-2- Encryption Phase #################################################*/
  HAL_CRYP_Encrypt(&hcryp, Plaintext, 16, EncryptedText, TIMEOUT_VALUE);
   /*Compare results with expected buffer*/
  if(memcmp(EncryptedText, CiphertextCBC128, PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*##-2- Decryption Phase #################################################*/
  HAL_CRYP_Decrypt(&hcryp,CiphertextCBC128 , 16, DecryptedText, TIMEOUT_VALUE);
  /*Compare results with expected buffer*/
  if(memcmp(DecryptedText, Plaintext, PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }
  /*=======================================CBC  Key256========================================*/
  if (FLASH_KEY_PROG(2) != 0) /* 256bits AHK key */
  {
    /* Processing Error */
    Error_Handler();
  }
  /* Get the SAES parameters */
  HAL_CRYP_GetConfig(&hcryp, &Conf);
  /* Set the SAES parameters */
  Conf.KeySize = CRYP_KEYSIZE_256B;
  Conf.pKey    = pKeySAES256;
  /* Set the SAES Configuration*/
  HAL_CRYP_SetConfig(&hcryp, &Conf);
  if (FLASH_KEY_LOAD(2) != 0) /* 128bits AHK key */
  {
    /* Processing Error */
    Error_Handler();
  }
  /*##-2.1- Encryption Phase #################################################*/
  HAL_CRYP_Encrypt(&hcryp, Plaintext, 16, EncryptedText, TIMEOUT_VALUE);
    /*Compare results with expected buffer*/
  if(memcmp(EncryptedText, CiphertextCBC256, 64) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }

  /*##-2.1- Decryption Phase #################################################*/
  HAL_CRYP_Decrypt(&hcryp,CiphertextCBC256 , 16, DecryptedText, TIMEOUT_VALUE);
  /*Compare results with expected buffer*/
  if(memcmp(DecryptedText, Plaintext,PLAINTEXT_SIZE) != 0)
  {
    /* Processing Error */
    Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* Encrypted/Decrypted text and computed Tag are correct : Turn LD1 on */
    BSP_LED_On(LD1);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief SAES Initialization Function
  * @param None
  * @retval None
  */
static void MX_SAES_CRYP_Init(void)
{

  /* USER CODE BEGIN SAES_Init 0 */

  /* USER CODE END SAES_Init 0 */

  /* USER CODE BEGIN SAES_Init 1 */

  /* USER CODE END SAES_Init 1 */
  hcryp.Instance = SAES;
  hcryp.Init.DataType = CRYP_DATATYPE_32B;
  hcryp.Init.KeySize = CRYP_KEYSIZE_128B;
  hcryp.Init.pKey = (uint32_t *)pKeySAES;
  hcryp.Init.Algorithm = CRYP_AES_ECB;
  hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
  hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
  hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
  hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;
  hcryp.Init.KeySelect = CRYP_KEYSEL_AHK;
  if (HAL_CRYP_Init(&hcryp) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SAES_Init 2 */

  /* USER CODE END SAES_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
* @brief  This function configures the MPU context of the application.
* @retval None
*/
static void MPU_Config(void)
{
  MPU_Region_InitTypeDef MPU_InitStruct = {0};
  uint32_t index = MPU_REGION_NUMBER0;
  uint32_t address;
  uint32_t size;

  /* Disable the MPU */
  HAL_MPU_Disable();

  /* Initialize the background region */
  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = index;
  MPU_InitStruct.BaseAddress = 0x0;
  MPU_InitStruct.Size = MPU_REGION_SIZE_4GB;
  MPU_InitStruct.SubRegionDisable = 0x87;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL0;
  MPU_InitStruct.AccessPermission = MPU_REGION_NO_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  index++;

  /* Initialize the non cacheable region */
#if defined ( __ICCARM__ )
  /* get the region attribute form the icf file */
  extern uint32_t NONCACHEABLEBUFFER_start;
  extern uint32_t NONCACHEABLEBUFFER_size;

  address = (uint32_t)&NONCACHEABLEBUFFER_start;
  size = (uint32_t)&NONCACHEABLEBUFFER_size;

#elif defined (__CC_ARM) || defined(__ARMCC_VERSION)
  extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$Base;
  extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$Length;
  extern uint32_t Image$$RW_NONCACHEABLEBUFFER$$ZI$$Length;

  address = (uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$Base;
  size  = (uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$Length + (uint32_t)&Image$$RW_NONCACHEABLEBUFFER$$ZI$$Length;
#elif defined ( __GNUC__ )
  extern int __NONCACHEABLEBUFFER_BEGIN;
  extern int __NONCACHEABLEBUFFER_END;

  address = (uint32_t)&__NONCACHEABLEBUFFER_BEGIN;
  size  = (uint32_t)&__NONCACHEABLEBUFFER_END - (uint32_t)&__NONCACHEABLEBUFFER_BEGIN;
#else
#error "Compiler toolchain is unsupported"
#endif

  if (size != 0)
  {
    /* Configure the MPU attributes as Normal Non Cacheable */
    MPU_InitStruct.Enable = MPU_REGION_ENABLE;
    MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
    MPU_InitStruct.IsBufferable = MPU_ACCESS_NOT_BUFFERABLE;
    MPU_InitStruct.IsCacheable = MPU_ACCESS_NOT_CACHEABLE;
    MPU_InitStruct.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
    MPU_InitStruct.Number = index;
    MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
    MPU_InitStruct.SubRegionDisable = 0x00;
    MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
    MPU_AdjustRegionAddressSize(address, size, &MPU_InitStruct);
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
    index++;
  }

  /* Initialize the region corresponding to the execution area
     (external or internal flash or external or internal RAM
     depending on scatter file definition) */
#if defined ( __ICCARM__ )
  extern uint32_t __ICFEDIT_region_ROM_start__;
  extern uint32_t __ICFEDIT_region_ROM_end__;
  address = (uint32_t)&__ICFEDIT_region_ROM_start__;
  size = (uint32_t)&__ICFEDIT_region_ROM_end__ - (uint32_t)&__ICFEDIT_region_ROM_start__ + 1;
#elif defined (__CC_ARM) || defined(__ARMCC_VERSION)
  extern uint32_t Image$$ER_ROM$$Base;
  extern uint32_t Image$$ER_ROM$$Limit;
  address = (uint32_t)&Image$$ER_ROM$$Base;
  size    = (uint32_t)&Image$$ER_ROM$$Limit-(uint32_t)&Image$$ER_ROM$$Base;
#elif defined ( __GNUC__ )
  extern uint32_t __FLASH_BEGIN;
  extern uint32_t __FLASH_SIZE;
  address = (uint32_t)&__FLASH_BEGIN;
  size  = (uint32_t)&__FLASH_SIZE;
#else
#error "Compiler toolchain is unsupported"
#endif

  MPU_InitStruct.Enable = MPU_REGION_ENABLE;
  MPU_InitStruct.Number = index;
  MPU_InitStruct.SubRegionDisable = 0u;
  MPU_InitStruct.TypeExtField = MPU_TEX_LEVEL1;
  MPU_InitStruct.AccessPermission = MPU_REGION_FULL_ACCESS;
  MPU_InitStruct.DisableExec = MPU_INSTRUCTION_ACCESS_ENABLE;
  MPU_InitStruct.IsShareable = MPU_ACCESS_SHAREABLE;
  MPU_InitStruct.IsCacheable = MPU_ACCESS_CACHEABLE;
  MPU_InitStruct.IsBufferable = MPU_ACCESS_BUFFERABLE;
  MPU_AdjustRegionAddressSize(address, size, &MPU_InitStruct);
  HAL_MPU_ConfigRegion(&MPU_InitStruct);
  index++;

  /* Reset unused MPU regions */
  for(; index < __MPU_REGIONCOUNT ; index++)
  {
    /* All unused regions disabled */
    MPU_InitStruct.Enable = MPU_REGION_DISABLE;
    MPU_InitStruct.Number = index;
    HAL_MPU_ConfigRegion(&MPU_InitStruct);
  }

  /* Enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);
}

/**
  * @brief This function adjusts the MPU region Address and Size within an MPU configuration.
  * @param Address memory address
  * @param Size memory size
  * @param pInit pointer to an MPU initialization structure
  * @retval None
  */
static void MPU_AdjustRegionAddressSize(uint32_t Address, uint32_t Size, MPU_Region_InitTypeDef* pInit)
{
  /* Compute the MPU region size */
  pInit->Size = ((31 - __CLZ(Size)) - 1);
  if (Size > (1 << (pInit->Size + 1)))
  {
    pInit->Size++;
  }
  uint32_t Modulo = Address % (1 << (pInit->Size - 1));
  if (0 != Modulo)
  {
    /* Align address with MPU region size considering there is no need to increase the size */
    pInit->BaseAddress = Address - Modulo;
  }
  else
  {
    pInit->BaseAddress = Address;
  }
}
/**
  * @brief Program an OB key into flash
  * @param Index   0: 128bits key,  2: 256bits key
  * @retval 0 in case of success
  */

static uint32_t FLASH_KEY_PROG(uint32_t index)
{
  uint32_t key[8];
  uint32_t i;
  FLASH_KeyConfigTypeDef keyConfig;

  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();

  /* Set PG_OPT Bit */
  SET_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OPT);

  /* Make sure the KTEF and KVEF bits are cleared */
  if (READ_BIT(FLASH->OPTISR, FLASH_OPTISR_KTEF) != 0) while(1);
  if (READ_BIT(FLASH->OPTISR, FLASH_OPTISR_KVEF) != 0) while(1);


  switch(index)
  {
  case 0:  /* Index 0 used for 128bits key */
     /* Program value of Key for current TIL */
    keyConfig.Index     = index;
    keyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;
    keyConfig.Size = FLASH_KEY_128_BITS;
    /* Write keys  */
    for (i = 0; i<4; i++)
    {
      key[i] = pKeySAES[3-i];
    }

    if (HAL_FLASHEx_KeyConfig(&keyConfig, key) != HAL_OK) return 2;
    break;

  case 2:  /* Index 2 used for 256bits key */
    /* Program value of Key for current TIL */
    keyConfig.Index     = index;
    keyConfig.HDPLLevel = FLASH_KEY_LEVEL_NEXT;
    keyConfig.Size = FLASH_KEY_256_BITS;

    /* Write keys  */
    for (i = 0; i<8; i++)
    {
      key[i] = pKeySAES256[7-i];
    }
    if (HAL_FLASHEx_KeyConfig(&keyConfig, key) != HAL_OK) return 1;
    break;

  default:
    return 3;

  }

  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();

  return 0;
}

/**
  * @brief Option byte key loading
  * @param Index   0: 128bits key,  2: 256bits key
  * @retval 0 in case of success
  */
static uint32_t FLASH_KEY_LOAD(uint32_t index)
{

  uint32_t key_out[8];
  FLASH_KeyConfigTypeDef keyConfig;

  HAL_FLASH_Unlock();
  HAL_FLASH_OB_Unlock();
    /* Set PG_OPT Bit */
  SET_BIT(FLASH->OPTCR, FLASH_OPTCR_PG_OPT);

  /* Make sure the KTEF and KVEF bits are cleared */
  if (READ_BIT(FLASH->OPTISR, FLASH_OPTISR_KTEF) != 0) while(1);
  if (READ_BIT(FLASH->OPTISR, FLASH_OPTISR_KVEF) != 0) while(1);

  switch(index)
  {
  case 0:  /* Index 0 used for 128bits key */
    /* Program value of Key for current TIL */
    keyConfig.Index     = index;
    keyConfig.HDPLLevel = FLASH_KEY_LEVEL_CURRENT;
    keyConfig.Size = FLASH_KEY_128_BITS;
    if (  HAL_FLASHEx_GetKey(&keyConfig, key_out) != HAL_OK) return 2;
    break;

  case 2:  /* Index 2 used for 256bits key */
    /* Program value of Key for current TIL */
    keyConfig.Index     = index;
    keyConfig.Size = FLASH_KEY_256_BITS;
    keyConfig.HDPLLevel = FLASH_KEY_LEVEL_NEXT;
    if (  HAL_FLASHEx_GetKey(&keyConfig, key_out) != HAL_OK) return 1;
    break;

  default:
    return 3;
  }

  HAL_FLASH_OB_Lock();
  HAL_FLASH_Lock();


  return 0;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  /* Turn LD3 on */
  BSP_LED_On(LD3);
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* Infinite loop */
  while (1)
  {
  }
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
