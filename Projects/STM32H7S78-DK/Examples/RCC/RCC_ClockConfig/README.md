## <b>RCC_ClockConfig Example Description</b>

Configuration of the system clock (SYSCLK) and modification of the clock settings in Run mode, using the RCC HAL API.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example, after startup SYSCLK is configured to the max frequency using the PLL with
HSI as clock source, the USER push-button (connected to External line 13) will be
used to change the PLL source:

- from CSI to HSE
- from HSE to HSI
- from HSI to CSI

Each time the USER push-button is pressed External line 13 interrupt is generated and in the ISR
the PLL oscillator source is checked using __HAL_RCC_GET_PLL_OSCSOURCE() macro:

- If the HSE oscillator is selected as PLL source, the following steps will be followed to switch 
   the PLL source to HSI oscillator:

     a. Switch the system clock source to HSE to allow modification of the PLL configuration
     b. Enable HSI Oscillator, select it as PLL source and finally activate the PLL
     c. Select the PLL as system clock source and configure bus clocks dividers
     d. Disable the HSE oscillator 

- If the CSI oscillator is selected as PLL source, the following steps will be followed to switch 
   the PLL source to HSE oscillator:

     a. Switch the system clock source to CSI to allow modification of the PLL configuration
     b. Enable HSE Oscillator, select it as PLL source and finally activate the PLL
     c. Select the PLL as system clock source and configure bus clocks dividers
     d. Disable the CSI oscillator

- If the HSI oscillator is selected as PLL source, the following steps will be followed to switch 
   the PLL source to CSI oscillator:
   
     a. Switch the system clock source to HSI to allow modification of the PLL configuration
     b. Enable CSI Oscillator, select it as PLL source and finally activate the PLL
     c. Select the PLL as system clock source and configure bus clocks dividers
     d. Disable the HSI oscillator

LED1 is toggled with a timing defined by the HAL_Delay() API.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

 3. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
    by the Cortex M7 and the GPDMA/HPDMA), there is no need for cache maintenance.
    If the application needs to put DMA buffers in AXI SRAM (starting from @0x24000000), the user has to:
    - either define a non-cacheable region in the MPU and linker configuration file to locate DMA buffers
      (a proposed dma_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.

	This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"	

### <b>Keywords</b>
RCC, System, Clock Configuration, HSE bypass mode, HSI, System clock, Oscillator, PLL
### <b>Directory contents</b>

  - RCC/RCC_ClockConfig/Boot/Inc/stm32h7s78_discovery_conf.h   BSP configuration file
  - RCC/RCC_ClockConfig/Boot/Inc/stm32h7rsxx_hal_conf.h        HAL configuration file
  - RCC/RCC_ClockConfig/Boot/Inc/stm32h7rsxx_it.h              Interrupt handlers header file
  - RCC/RCC_ClockConfig/Boot/Inc/main.h                        Header for main.c module
  - RCC/RCC_ClockConfig/Boot/Src/stm32h7rsxx_it.c              Interrupt handlers
  - RCC/RCC_ClockConfig/Boot/Src/main.c                        Main program
  - RCC/RCC_ClockConfig/Boot/Src/system_stm32h7rsxx.c          STM32H7RSxx system source file
  - RCC/RCC_ClockConfig/Boot/Src/stm32h7rsxx_hal_msp.c         HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3 devices.

  - This example has been tested with NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.
	
  - On STM32H7S78-DK board, the BOOT0 mechanical slide switch must be set to SW1.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1     I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0     I/O speed optimized to operate at 3.3V	

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
