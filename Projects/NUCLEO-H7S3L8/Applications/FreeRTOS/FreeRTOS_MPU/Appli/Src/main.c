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
#include "FreeRTOS.h"
#include "cmsis_os2.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "task.h"
#include "queue.h"
#include "stdio.h"
#if defined(__ICCARM__)
#include <LowLevelIOInterface.h>
#endif /* __ICCARM__ */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct
{
  uint32_t address;
  uint32_t handle;
  uint32_t psp;
} FaultInfo_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define NUMBER_OF_TASKS (2)

/* Return address offset within stack frame */
#define RETURN_ADDRESS_OFFSET (6U)

/* Mask used to identify whether instruction is 16 bits or 32 bits */
#define INSTRUCTION_32BIT_Msk (0xE000)


/* Example config */
/* Number of faults a task can cause before it gets deleted */
#define EXAMPLE_FAULT_COUNT_THRESHOLD (3)

#define EXAMPLE_SHARED_MEMORY_SIZE (512U)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

UART_HandleTypeDef huart3;

/* Definitions for ProducerThread */
/* USER CODE BEGIN PV */

#if defined ( __CC_ARM ) || defined(__ARMCC_VERSION)

extern uint32_t Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Base;
extern uint32_t Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Limit;

extern uint32_t Image$$ER_IROM_UNPRIVILEGED$$Base;
extern uint32_t Image$$ER_IROM_UNPRIVILEGED$$Limit;

/* Memory map needed for MPU setup. Must match the one defined in
 * the scatter-loading file (Project.sct). */
const uint32_t * __FLASH_segment_start__ = ( uint32_t * ) 0x70000000;
const uint32_t * __FLASH_segment_end__ = ( uint32_t * ) 0x7FFFFFFF;
const uint32_t * __SRAM_segment_start__ = ( uint32_t * ) 0x24000000;
const uint32_t * __SRAM_segment_end__ = ( uint32_t * ) 0x2401FFFF;

const uint32_t * __privileged_functions_start__ = ( uint32_t * ) 0x70000000;
const uint32_t * __privileged_functions_end__ = ( uint32_t * ) 0x70007FFF;
const uint32_t * __privileged_data_start__ = ( uint32_t * ) 0x24000000;
const uint32_t * __privileged_data_end__ = ( uint32_t * ) 0x24007FFF;

const uint32_t * __unprivileged_flash_start__ = ( uint32_t * ) &( Image$$ER_IROM_UNPRIVILEGED$$Base );
const uint32_t * __unprivileged_flash_end__ = ( uint32_t * ) &( Image$$ER_IROM_UNPRIVILEGED$$Limit );

const uint32_t * __syscalls_flash_start__ = ( uint32_t * ) &( Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Base );
const uint32_t * __syscalls_flash_end__ = ( uint32_t * ) &( Image$$ER_IROM_FREERTOS_SYSTEM_CALLS$$Limit );
#endif

/* A queue to hold fault information */
static QueueHandle_t fault_queue;

/* Define an array to track fault count */
static uint8_t fault_count[NUMBER_OF_TASKS];

/* Define stack spaces for unprivileged tasks.
 * These must meet MPU alignment and size requirements. */
static StackType_t Task1Stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( configMINIMAL_STACK_SIZE * sizeof( StackType_t ) ) ) );
static StackType_t Task2Stack[ configMINIMAL_STACK_SIZE ] __attribute__( ( aligned( configMINIMAL_STACK_SIZE * sizeof( StackType_t ) ) ) );

/* Define shared memory */
static uint8_t sharedMemory[ EXAMPLE_SHARED_MEMORY_SIZE ] __attribute__( ( aligned( EXAMPLE_SHARED_MEMORY_SIZE ) ) );


static void Task1(void *argument);
static void Task2(void *argument);

static TaskHandle_t MainTaskHandle;
static TaskHandle_t TaskHandles[NUMBER_OF_TASKS];

/* Task parameters for unprivileged tasks */
static const TaskParameters_t TaskParameters[NUMBER_OF_TASKS] =
{
  {
      .pvTaskCode     = Task1,
      .pcName         = "Task1",
      .usStackDepth   = configMINIMAL_STACK_SIZE,
      .pvParameters   = NULL,
      .uxPriority     = tskIDLE_PRIORITY,
      .puxStackBuffer = Task1Stack,
      .xRegions       =   {
                              { sharedMemory,         EXAMPLE_SHARED_MEMORY_SIZE,	portMPU_REGION_PRIVILEGED_READ_WRITE_UNPRIV_READ_ONLY | portMPU_REGION_EXECUTE_NEVER },
                              { LED_GREEN_GPIO_Port,  EXAMPLE_SHARED_MEMORY_SIZE,	portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER },

                              { 0 ,  0,  0 },
                          }
  },
  {
      .pvTaskCode     = Task2,
      .pcName         = "Task2",
      .usStackDepth   = configMINIMAL_STACK_SIZE,
      .pvParameters   = NULL,
      .uxPriority     = tskIDLE_PRIORITY,
      .puxStackBuffer = Task2Stack,
      .xRegions       =   {
                              { sharedMemory,         EXAMPLE_SHARED_MEMORY_SIZE,	portMPU_REGION_PRIVILEGED_READ_WRITE_UNPRIV_READ_ONLY | portMPU_REGION_EXECUTE_NEVER },
                              { LED_YELLOW_GPIO_Port, EXAMPLE_SHARED_MEMORY_SIZE,	portMPU_REGION_READ_WRITE | portMPU_REGION_EXECUTE_NEVER },

                              { 0 ,  0,  0 },
                          }
  }
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
void Producer_Thread(void *argument);

/* USER CODE BEGIN PFP */
static void MainTask(void *argument);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#if defined(__ICCARM__)
/* New definition from EWARM V9, compatible with EWARM8 */
int iar_fputc(int ch);
#define PUTCHAR_PROTOTYPE int iar_fputc(int ch)
#elif defined ( __CC_ARM ) || defined(__ARMCC_VERSION)
/* ARM Compiler 5/6*/
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#elif defined(__GNUC__)
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#endif /* __ICCARM__ */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

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

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
  printf("FreeRTOS MPU Application\r\n");
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  fault_queue = xQueueCreate(5U, sizeof(FaultInfo_t));
  if(fault_queue == NULL)
  {
     Error_Handler();
  }
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */

  /* USER CODE BEGIN RTOS_THREADS */
  if(xTaskCreate(MainTask, "MainTask", configMINIMAL_STACK_SIZE, NULL,
                 (configMAX_PRIORITIES -1) |portPRIVILEGE_BIT  ,
                 &MainTaskHandle) != pdPASS)
  {
     Error_Handler();
  }

  /* Create unprivileged threads */
  if(xTaskCreateRestricted(&TaskParameters[0], &TaskHandles[0] )!= pdPASS)
  {
   Error_Handler();
  }
  if(xTaskCreateRestricted(&TaskParameters[1], &TaskHandles[1] )!= pdPASS)
  {
   Error_Handler();
  }
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LED_YELLOW_Pin|LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_RED_Pin */
  GPIO_InitStruct.Pin = LED_RED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(LED_RED_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_YELLOW_Pin LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_YELLOW_Pin|LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
#if defined(__ICCARM__)
size_t __write(int file, unsigned char const *ptr, size_t len)
{
  size_t idx;
  unsigned char const *pdata = ptr;

  for (idx = 0; idx < len; idx++)
  {
    iar_fputc((int)*pdata);
    pdata++;
  }
  return len;
}
#endif /* __ICCARM__ */

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART3 and Loop until the end of transmission */
  HAL_UART_Transmit(&huart3, (uint8_t *)&ch, 1, 0xFFFF);

  return ch;
}


/**
  * @brief Function implementing the MainTask thread.
  *        It is responsible for montitoring other threads.
  * @param argument: Not used
  * @retval None
  */
void MainTask(void *argument)
{
  UNUSED(argument);

  FaultInfo_t fault;

  for(;;)
  {
    xQueueReceive(fault_queue, &fault, portMAX_DELAY);

    printf("\tTask name: %s\n", pcTaskGetName((TaskHandle_t) fault.handle));

    printf("------------------------------\n");

    /* Handle MPU exceptions */
    for(int i = 0; i < NUMBER_OF_TASKS; ++i)
    {
       /* Delete task if it has caused too many faults*/
      if(fault_count[i] == EXAMPLE_FAULT_COUNT_THRESHOLD)
      {
        vTaskDelete((TaskHandle_t)fault.handle);

        printf("%s has caused %d faults. It was deleted.\r\n",
                pcTaskGetName(TaskHandles[i]),
                EXAMPLE_FAULT_COUNT_THRESHOLD);
      }
    }
  }
}

/**
  * @brief Function implementing the Task1 thread.
  * @param argument: Not used
  * @retval None
  */
void Task1(void *argument)
{
  UNUSED(argument);

  uint8_t isFirstTime = 1;

  for(;;)
  {
    HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
    vTaskDelay(pdMS_TO_TICKS(1000));
    if(isFirstTime)
    {
      isFirstTime = 0;

      /* Try to perform an illegal write */
      sharedMemory[EXAMPLE_SHARED_MEMORY_SIZE - 1] = 4U;
    }
  }
}

/**
  * @brief Function implementing the Task2 thread.
  * @param argument: Not used
  * @retval None
  */
void Task2(void *argument)
{
  UNUSED(argument);

  vTaskDelay(pdMS_TO_TICKS(1000));
  for(;;)
  {
    HAL_GPIO_TogglePin(LED_YELLOW_GPIO_Port, LED_YELLOW_Pin);
    vTaskDelay(pdMS_TO_TICKS(2000));

    /* Try to perform an illegal write */
    sharedMemory[EXAMPLE_SHARED_MEMORY_SIZE/2] = 4U;
  }
}

/* We need to use the original FreeRTOS xTaskGetCurrentTaskHandle
 * function instead of the one defined in mpu_wrapper.c */
#undef xTaskGetCurrentTaskHandle
TaskHandle_t xTaskGetCurrentTaskHandle( void );

/**
  * @brief MemManage fault recovery
  * @retval None
  */
void MemManage_Recover(void)
{

  FaultInfo_t current_fault;
  uint8_t nextInstructionOffset = 2U;
  uint32_t* returnAddress;
  uint16_t instruction;

  /* Log the offending thread */
  current_fault.address  = SCB->MMFAR;
  current_fault.handle =  (uint32_t)xTaskGetCurrentTaskHandle();
  current_fault.psp = (uint32_t)__get_PSP();

  /*
   * Clear DACCVIOL sticky bit in the MMFSR subregister.
   * If not cleared, subsequent fault addresses will not be stored in
   * the MMFAR register.*/
  SET_BIT(SCB->CFSR,SCB_CFSR_DACCVIOL_Msk);

  for(int i = 0; i < NUMBER_OF_TASKS; ++i)
  {
    if((TaskHandle_t)current_fault.handle == TaskHandles[i])
    {
       /* Increment fault count for task */
       ++fault_count[i];

       /* Display info about fault */
       printf("MemManage fault occurred\r\n");
       printf("\tAddress: 0x%X\r\n", (unsigned int) current_fault.address);
       printf("\tCurrent Task Handle: %p\n", xTaskGetCurrentTaskHandle());

       returnAddress = (uint32_t*)current_fault.psp + RETURN_ADDRESS_OFFSET;

       /* Identify if the instruction which caused the issue
        *is 16 or 32 bit wide */
       instruction = *(uint16_t*)(*(uint32_t*)returnAddress);
       if((instruction & INSTRUCTION_32BIT_Msk) == INSTRUCTION_32BIT_Msk)
       {
          nextInstructionOffset = 4U;
       }
       else
       {
          nextInstructionOffset = 2U;
       }

       /* Move to next instruction */
       *returnAddress += nextInstructionOffset;

       break;
    }
  }
  /* Send Fault info to Main task */
  xQueueSendToBackFromISR(fault_queue, &current_fault, NULL);

  /* Current task yields. Set PENDSVSET bit to set the PendSV exception state
   * to pending, this forces a context switch and guarantees that
   * the Main thread will be able to take action if needed before the current
   * thread continues.*/
  portYIELD_WITHIN_API();
}

/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM6 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  printf("Critical Error occurred \r\n");
  while (1)
  {
    HAL_GPIO_TogglePin(LED_RED_GPIO_Port, LED_RED_Pin);
    HAL_Delay(500);
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
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
