## <b>CORTEXM_ModePrivilege Example Description</b>

How to modify the Thread mode privilege access and stack. Thread mode is entered
on reset or when returning from an exception.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.

This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).

Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The associated program is used to:

1. Switch the Thread mode stack from Main stack to Process stack

2. Switch the Thread mode from Privileged to Unprivileged

3. Switch the Thread mode from Unprivileged back to Privileged

To monitor the stack used and the privileged or unprivileged access level of code
in Thread mode, a set of variables is available within the program. It is also
possible to use the 'Cortex register' window of the debugger.

LD1 Turns ON when the test is finished successfully.

#### <b>Notes</b>

  1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
  2. The application need to ensure that the SysTick time base is always set to 1 millisecond
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
      - Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
      - Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

System, Cortex, Mode Privilege, Privileged access, thread mode, main stack, process stack

### <b>Directory contents</b>

  - CORTEX/CORTEXM_ModePrivilege/Boot/Inc/stm32h7rsxx_nucleo_conf.h     BSP configuration file
  - CORTEX/CORTEXM_ModePrivilege/Boot/Inc/stm32h7rsxx_hal_conf.h        HAL configuration file
  - CORTEX/CORTEXM_ModePrivilege/Boot/Inc/stm32h7rsxx_it.h              Interrupt handlers header file
  - CORTEX/CORTEXM_ModePrivilege/Boot/Inc/main.h                        Header for main.c module
  - CORTEX/CORTEXM_ModePrivilege/Boot/Src/stm32h7rsxx_it.c              Interrupt handlers
  - CORTEX/CORTEXM_ModePrivilege/Boot/Src//stm32h7rsxx_hal_msp.c        HAL MSP file
  - CORTEX/CORTEXM_ModePrivilege/Boot/Src/main.c                        Main program
  - CORTEX/CORTEXM_ModePrivilege/Boot/Src/system_stm32h7rsxx.c          STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
    
  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.      

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

- Open your preferred toolchain 
- Rebuild all files and load your image into target memory
- Run the example

