## <b>MCE_ExecuteNoekeonCryptedCode Example Description</b>

- This project provides a description of how to run encrypted application code from external flash (0x70000000-0x7FFFFFFF). The decryption is performed on the fly using the MCE IP(Noekeon encryption algorithm). 
- It boots from internal flash and jumps to the encrypted application code in external flash. The user can provide their own application and encrypt it using the Nokeon algorithm. 
   In this case, the encryption key must be updated in the main.c file. The encrypted code consists of a basic application that toggles the user LD1 each 100 ms.
- This project is targeted to run on STM32H7S7xx device on STM32H7S78-DK board from STMicroelectronics.
At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals and initialize the systick used as 1ms HAL timebase.
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [regions 0, 1 and 2 here], system clock at 600 MHz and external memory interface at the highest speed).
- This project also calls SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches. This is provided as template implementation that the User may
integrate in his application in order to enhance the performance.

This example performs these tasks:

	Initializes the XSPI external memory in memory-mapped mode to enable data transfer and storage.
	Calls the HAL_MCE_Init() function.
	Configures the Noekeon key type (Master or Fast master key).
	Configures the MCE region (mode, start, and end address) to ensure proper data transfer and storage within the specified memory range.


STM32H7S78-DK board's LED can be used to monitor the execution status:

 - [**LD1**]{style="color: GREEN"} toggles when the the encrypted code is successfully executed.
 - [**LD2**]{style="color: RED"}  Toggles as soon as an error occurs. 


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

MCE, Cryptography, Security, Noekeon, Basics 


### <b>Directory contents</b>

File                                                                     | Description
 ---                                                                     | ---
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Inc/main.h                      |  Header for main.c module
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Inc/NoekeonEncryptedAppli_bin.h |  Encrypted application code
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Inc/stm32h7rsxx_hal_conf.h      |  BSP Configuration file
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Inc/stm32h7rsxx_it.h            |  Interrupt handlers header file
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Inc/stm32h7s78_discovery_conf.h |  HAL configuration file
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Src/main.c                      |  Main program
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Src/stm32h7rsxx_hal_msp.c       |  HAL MSP module
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Src/stm32h7rsxx_it.c            |  Interrupt handlers
  MCE/MCE_ExecuteNoekeonCryptedCode/Boot/Src/system_stm32h7rsxx.c        |  STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - In this example, the clock is set to 600 MHz.

  - This example has been tested with STMicroelectronics STM32H7S78-DK (MB1736) board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool):

    - XSPI1_HSLV=1 I/O XSPIM_P1 High speed option enabled
    - XSPI2_HSLV=1 I/O XSPIM_P2 High speed option enabled
    - VDDIO_HSLV=0 I/O speed optimized to operate at 3.3V

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Ensure **SW1** switch is set to **BOOT1**
 - Open Project.eww with your preferred toolchain (IAR only so far)
 - Rebuild all files and load your image into target memory.
 - Run the example
