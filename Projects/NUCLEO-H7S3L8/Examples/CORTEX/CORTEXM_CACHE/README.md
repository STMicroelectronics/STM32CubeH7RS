## <b>CORTEXM_CACHE Example Description</b>

This project provides a CORTEXM cache example based on the CMSIS API that can be used
to build any firmware application to execute from internal Flash.

This project is targeted to run on STM32H7S3xx device on NUCLEO-H7S3L8 board from STMicroelectronics.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The example shows the consequence of the cache on data located in the cacheable area when
the memory is updated by a DMA operation and how to counteract this effect.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The example needs to ensure that the SysTick time base is always set to 1 millisecond
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

CORTEXM, CACHE, DMA, SRAM

### <b>Directory contents</b>

File | Description
 --- | ---
  CORTEX/CTORTEXM_CACHE/Boot/Inc/main.h                     | Header for main.c module
  CORTEX/CTORTEXM_CACHE/Boot/Inc/stm32h7rsxx_hal_conf.h     | HAL Configuration file
  CORTEX/CTORTEXM_CACHE/Boot/Inc/stm32h7rsxx_it.h           | Interrupt handlers header file
  CORTEX/CTORTEXM_CACHE/Boot/Inc/stm32h7rsxx_nucleo_conf.h  | BSP Configuration file
  CORTEX/CTORTEXM_CACHE/Boot/Src/main.c                     |  Main program
  CORTEX/CTORTEXM_CACHE/Boot/Src/stm32h7rsxx_hal_msp.c      | HAL MSP module
  CORTEX/CTORTEXM_CACHE/Boot/Src/stm32h7rsxx_it.c           | Interrupt handlers
  CORTEX/CTORTEXM_CACHE/Boot/Src/system_stm32h7rsxx.c       | STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

    - XSPI2_HSLV=1     I/O XSPIM_P2 High speed option enabled

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
