## <b>PWR_STANDBY_RTC Example Description</b>

How to enter the Standby mode and wake up from this mode by using an external 
reset or the RTC wakeup timer through the STM32H7RSxx RTC and RCC HAL, 
and LL API (LL API use for maximizing performance).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

It allows to measure the current consumption in STANDBY mode with RTC enabled.

In the associated software, the system clock is set to 600 MHz and the SysTick is 
programmed to generate an interrupt each 1 ms.

The Low Speed Internal (LSI) clock is used as RTC clock source by default.

EXTI_Line32 is internally connected to the RTC Wakeup event.

The system automatically enters STANDBY mode 5 sec. after start-up. The RTC wake-up 
is configured to generate an interrupt on rising edge about 33 sec. afterwards.

Current consumption in STANDBY mode with RTC feature enabled can be measured during that time.

More than half a minute is chosen to ensure current convergence to its lowest operating point.

Note: Due to LSI frequency variations, wake-up time is not guarantee. Adjustments need to be
done after getting the real measurement of LSI oscillator (or if available, LSE oscillator can
be used as well)

After wake-up from STANDBY mode, program execution restarts in the same way as after
a RESET.

led LD1 is used to monitor the system state as following:

 - LD1 ON: configuration failed (system will go to an infinite loop)
 - LD1 toggling: system in RUN mode
 - LD1 off : system in STANDBY mode
 - These steps are repeated in an infinite loop.
 - To measure the current consumption in STANDBY mode, remove JP5 jumper and connect an amperemeter to JP5 to measure IDD current.

#### <b>Notes</b>

  1. This example can not be used in DEBUG mode due to the fact 
     that the Cortex-M7 core is no longer clocked during low power mode 
     so debugging features are disabled.

  2. Care must be taken when HAL_RCCEx_PeriphCLKConfig() is used to select 
     the RTC clock source; in this case the Backup domain will be reset in  
     order to modify the RTC Clock source, as consequence RTC registers (including 
     the backup registers) and RCC_CSR register are set to their reset values.

  3. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
     based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
     a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
     than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
     To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
  4. The application needs to ensure that the SysTick time base is always set to 1 millisecond
     to have correct HAL operation.

  5. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Power, PWR, RTC, wake-up, standby mode, low power mode

### <b>Directory contents</b>

  - PWR/PWR_STANDBY_RTC/Boot/Inc/stm32h7rsxx_conf.h         HAL Configuration file
  - PWR/PWR_STANDBY_RTC/Boot/Inc/stm32h7rsxx_it.h           Header for stm32h7rsxx_it.c
  - PWR/PWR_STANDBY_RTC/Boot/Inc/main.h                     Header file for main.c
  - PWR/PWR_STANDBY_RTC/Boot/Src/system_stm32h7rsxx.c       STM32H7RSxx system clock configuration file
  - PWR/PWR_STANDBY_RTC/Boot/Src/stm32h7rsxx_it.c           Interrupt handlers
  - PWR/PWR_STANDBY_RTC/Boot/Src/main.c                     Main program
  - PWR/PWR_STANDBY_RTC/Boot/Src/stm32h7rsxx_hal_msp.c      HAL MSP module

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices

  - This example has been tested with STMicroelectronics NUCLEO-H7S3L8
    board and can be easily tailored to any other supported device
    and development board.

  - NUCLEO-H7S3L8 set-up:
    - Use LD1 connected to PD.10 pin

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 - Unplug then Plug STLINK connection to perform a power-on-reset
