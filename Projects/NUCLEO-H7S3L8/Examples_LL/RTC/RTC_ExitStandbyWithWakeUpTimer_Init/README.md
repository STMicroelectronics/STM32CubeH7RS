## <b>RTC_ExitStandbyWithWakeUpTimer_Init Example Description</b>

How to periodically enter and wake up from STANDBY mode thanks to the RTC Wakeup Timer (WUT).
The peripheral initialization uses LL unitary service functions for optimization purposes (performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example, after start-up, SYSCLK is configured to the max frequency using
the PLL with HSI as clock source.

   Note :

      LSE oscillator clock is used as RTC clock source by default.
      The user can use also LSI as RTC clock source. The user uncomment the adequate
      line on the main.c file.
      @code
        #define RTC_CLOCK_SOURCE_LSE
        /* #define RTC_CLOCK_SOURCE_LSI */
      @endcode
      LSI oscillator clock is delivered by a 32 kHz RC.
      LSE (when available on board) is delivered by a 32.768 kHz crystal.

   Note: The EXTI Line 19 is connected internally to the RTC Wakeup Timer (WUT) event.

Example execution:
  The RTC Wakeup Timer (WUT) is configured to periodically generate an event every 3 seconds
  and the system enters STANDBY mode. After the WUT event is generated, the system wakes up. The system enters again in STANDBY mode after each wakeup.
  After 5 times the system wakes up (5 * 3 seconds = 15 seconds), the program checks that the RTC seconds value is also 15 seconds.
  It means that the test is OK.

One of the below scenario can occur :

   - LD1 (pin PD.10) toggles 2 times : The system woke up from standby.
   - LD1 (pin PD.10) is ON after 15 seconds : Test is over, result is OK.
   - LD1 (pin PD.10) is toggling in infinite loop at a frequency of 5Hz: An error occurred.

#### <b>Notes</b>

 1. This example can not be used in DEBUG mode, this is due to the fact
    that the core is no longer clocked during low power mode
    so debugging features are disabled.
    To use the DEBUG mode uncomment __DBGMCU_EnableDBGStandbyMode in main() but
    current consumption in STANDBY mode will increase a lot.

 2. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

System, RTC, RTC Wakeup timer, Standby mode, LSI, LSE, Interrupt,

### <b>Directory contents</b>

  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Boot/Inc/main.h                       Header for main.c module
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Boot/Inc/stm32_assert.h               Template file to include assert_failed function
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Boot/Src/main.c                        Main program
  - RTC/RTC_ExitStandbyWithWakeUpTimer_Init/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
