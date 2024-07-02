## <b>PWR_STOP Example Description</b>

How to enter the STOP mode and wake up from this mode by using external 
reset or wakeup interrupt (all the RCC function calls use RCC LL API 
for minimizing footprint and maximizing performance).

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In the associated software, the system clock is set to 600 MHz, an EXTI line
is connected to the user button through PC.13 and configured to generate an 
interrupt on falling edge upon key press.

The SysTick is programmed to generate an interrupt each 1 ms and in the SysTick 
interrupt handler, LD1 is toggled in order to indicate whether the MCU is in STOP mode 
or RUN mode.

5 seconds after start-up, the system automatically enters STOP mode and 
LD1 stops toggling.

The USER push-button can be pressed at any time to wake-up the system. 

The software then comes back in RUN mode for 5 sec. before automatically entering STOP mode again. 

Two leds LD1 and LD2 are used to monitor the system state as following:
 - LD1 toggling: system in RUN mode
 - LD1 off : system in STOP mode
 - LD1 and LD2 ON: configuration failed (system will go to an infinite loop)

These steps are repeated in an infinite loop.

#### <b>Notes</b>

  1. This example can not be used in DEBUG mode due to the fact 
     that the Cortex-M7 core is no longer clocked during low power mode 
     so debugging features are disabled.

  2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
     based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
     a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
     than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
     To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
  3. The application needs to ensure that the SysTick time base is always set to 1 millisecond
     to have correct HAL operation.

  4. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Power, PWR, stop mode, wake-up, external reset, Interrupt, low power mode

### <b>Directory contents</b>

  - PWR/PWR_STOP/Boot/Inc/stm32h7rsxx_conf.h         HAL Configuration file
  - PWR/PWR_STOP/Boot/Inc/stm32h7rsxx_it.h           Header for stm32h7rsxx_it.c
  - PWR/PWR_STOP/Boot/Inc/main.h                     Header file for main.c
  - PWR/PWR_STOP/Boot/Src/system_stm32h7rsxx.c       STM32H7RSxx system clock configuration file
  - PWR/PWR_STOP/Boot/Src/stm32h7rsxx_it.c           Interrupt handlers
  - PWR/PWR_STOP/Boot/Src/main.c                     Main program
  - PWR/PWR_STOP/Boot/Src/stm32h7rsxx_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-H7S3L8 set-up:
    - Use LD1 and LD2 connected respectively to PD.10 and PD.13 pins
    - USER push-button connected to pin PC.13 (External line 13)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Unplug then Plug STLINK connection to perform a power-on-reset

