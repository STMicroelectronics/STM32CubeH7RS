## <b>FLASH_EraseProgram Example Description</b>

How to configure and use the FLASH HAL API to erase and program the internal

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system clock (SYSCLK)
to run at 600 MHz.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

After Reset, the Flash memory Program/Erase Controller is locked. A dedicated function
is used to enable the FLASH control register access.
Before programming the desired addresses, an erase operation is performed using
the flash erase sector feature. The erase procedure is done by filling the erase init
structure giving the starting erase sector and the number of sectors to erase.
At this stage, all these sectors will be erased one by one separately.


**Note:** If problem occurs on a sector, erase will be stopped and faulty sector will
be returned to user (through variable 'SectorError').

Once this operation is finished, sector quad-word programming operation will be performed
in the Flash memory. The written data is then read back and checked.

The NUCLEO-H7S3L8 board LEDs can be used to monitor the transfer status:  
  - LD1 is ON when there are no errors detected after data programming  
  - LD2 is ON when there are errors detected after data programming  
  - LD3 is ON when there is an issue during erase or program procedure  

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

Memory, FLASH, Erase, Program, Sector, Mass Erase

### <b>Directory contents</b>

        - FLASH/FLASH_EraseProgram/Boot/Inc/stm32h7rsxx_nucleo_conf.h   BSP configuration file
        - FLASH/FLASH_EraseProgram/Boot/Inc/stm32h7rsxx_hal_conf.h      HAL Configuration file
        - FLASH/FLASH_EraseProgram/Boot/Inc/stm32h7rsxx_it.h            Header for stm32h7rsxx_it.c
        - FLASH/FLASH_EraseProgram/Boot/Inc/main.h                      Header for main.c module
        - FLASH/FLASH_EraseProgram/Boot/Src/stm32h7rsxx_it.c            Interrupt handlers
        - FLASH/FLASH_EraseProgram/Boot/Src/main.c                      Main program
        - FLASH/FLASH_EraseProgram/Boot/Src/stm32h7rsxx_hal_msp.c       MSP initialization and de-initialization
        - FLASH/FLASH_EraseProgram/Boot/Src/system_stm32h7rsxx.c        STM32H7RSxx system clock configuration file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
