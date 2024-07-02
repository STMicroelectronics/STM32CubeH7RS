## <b>FLASH_OBK_Program Example Description</b>

How to use the FLASH HAL API to program option bytes keys


After Reset, the Flash memory controller and option bytes access are locked. Dedicated functions
are used to enable the FLASH controller's register and option bytes access.

Then two keys are programmed for different HDP levels, a 128-bit key for the current HDP level and
a 256-bit key for the next HDP level.

After this programming, the keys are read with current HDP level parameter to show that only 128-bit
key has been provisioned. Same read is performed with next HDP level to show that only 256-bit key
has been provisioned.

In a next step, the HDP level is incremented by calling HAL API of SBS peripheral. And finally the keys are
read at this new level to show that only 256-bit key has been provisioned.


The NUCLEO-H7S3L8 board LEDs can be used to monitor the transfer status:

  - LED_GREEN toggles every 200ms when there is no error detected after option byte keys programming
  - LED_RED toggles every 200ms when there is an issue during option byte keys programming


At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals
and initialize the systick used as 1ms HAL timebase.

The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 600 MHz.

The example calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1
Core Instruction and Data Caches and MPU_Config() function in order to configure a background region.

The programmed keys have been generated randomly by calling HAL API of RNG IP.

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
      (a proposed noncacheable_buffer section is available from CMSIS Device linker template file and its size must
      be adapted to the application requirements)
    - or to ensure cache maintenance operations to ensure the cache coherence between the CPU and the DMAs.
    This is true also for any other data buffers accessed by the CPU and other masters (DMA2D, LTDC)
    The addresses and the size of cacheable buffers (shared between CPU and other masters)
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Memory, FLASH, Option Bytes, Keys

### <b>Directory contents</b>

  - FLASH/FLASH_OBK_Program/Boot/Inc/main.h                      Header for main.c module
  - FLASH/FLASH_OBK_Program/Boot/Inc/stm32h7rsxx_hal_conf.h      HAL Configuration file
  - FLASH/FLASH_OBK_Program/Boot/Inc/stm32h7rsxx_it.h            Interrupt handlers header file
  - FLASH/FLASH_OBK_Program/Boot/Inc/stm32h7rsxx_nucleo_conf.h   BSP Configuration file
  - FLASH/FLASH_OBK_Program/Boot/Src/main.c                      Main program
  - FLASH/FLASH_OBK_Program/Boot/Src/stm32h7rsxx_hal_msp.c       HAL MSP module
  - FLASH/FLASH_OBK_Program/Boot/Src/stm32h7rsxx_it.c            Interrupt handlers
  - FLASH/FLASH_OBK_Program/Boot/Src/system_stm32h7rsxx.c        STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 **CubeIDE**:
 When resorting to STM32CubeIDE: select the Debug probe: ST-LINK(OpenOCD)
