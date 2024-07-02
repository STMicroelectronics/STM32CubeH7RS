## <b>FLASH_ECC_Error_Generation Example Description</b>

How to use the FLASH HAL API to manage ECC errors.

After reset, the Flash memory controller is locked. Dedicated function is used
to enable the FLASH controller's register access.

Then Flash ECC detection interrupt is enabled.

An ECC detection error will be generated and an interrupt will occur.
To generate this error, an access to the Read-Only area with the cache activated
will be performed. The programmed data in this area don't fill a full cache line,
thus when the cache fills a complete line it accesses to a non-programmed area
where both data and ECC are all '1' and an double ECC error detection occurs.
The user can monitor the detected address and area with the structure
Flash_EccDetectionInfo in the watch area. In case of correct behavior LD2 will
toggle every 250 ms.

If no ECC detection occurs LD1 will toggle every 250 ms.

In case of error during the execution LD3 will toggle every 250 ms.


At the beginning of the main program, the HAL_Init() function is called to reset all the peripherals
and initialize the systick used as 1ms HAL timebase.

The SystemClock_Config() function is used to configure the system clock (SYSCLK) to run at 600 MHz.

The example calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable the Layer 1
Core Instruction and Data Caches and MPU_Config() function in order to configure a background region.

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
    must be properly defined to be aligned to data cache line size (32 bytes) and of a size of being multiple
    of this cache line size.
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Memory, FLASH, ECC, Interrupts

### <b>Directory contents</b>

File | Description
 --- | ---
  FLASH/FLASH_ECC_Error_Generation/Boot/Inc/main.h                      |  Header for main.c module
  FLASH/FLASH_ECC_Error_Generation/Boot/Inc/stm32h7rsxx_hal_conf.h      |  HAL Configuration file
  FLASH/FLASH_ECC_Error_Generation/Boot/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  FLASH/FLASH_ECC_Error_Generation/Boot/Inc/stm32h7rsxx_nucleo_conf.h   |  BSP Configuration file
  FLASH/FLASH_ECC_Error_Generation/Boot/Src/main.c                      |  Main program
  FLASH/FLASH_ECC_Error_Generation/Boot/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  FLASH/FLASH_ECC_Error_Generation/Boot/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  FLASH/FLASH_ECC_Error_Generation/Boot/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

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
