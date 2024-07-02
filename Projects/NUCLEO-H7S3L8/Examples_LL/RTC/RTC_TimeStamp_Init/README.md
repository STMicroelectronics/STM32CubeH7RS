## <b>RTC_TimeStamp_Init Example Description</b>

Configuration of the Timestamp using the RTC LL API. The peripheral initialization
uses LL unitary service functions for optimization purposes (performance and size).

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
HSI as clock source.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The RTC peripheral configuration is ensured by the Configure_RTC() function
(configure of the needed RTC resources according to the used hardware CLOCK,
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

**Note:**

    LSI oscillator clock is used as RTC clock source by default.
       The user can use also LSE as RTC clock source.
       - The user uncomment the adequate line on the main.c file.
         @code
           #define RTC_CLOCK_SOURCE_LSI
           /* #define RTC_CLOCK_SOURCE_LSE */
         @endcode
       - Open the ioc file with STM32CubeMX and select :
         LSE as "Crystal/Ceramic Resonator" in RCC configuration.
         LSE as RTC clock source in Clock configuration.
       - Generate code
       LSI oscillator clock is delivered by a 32 kHz RC.
       LSE (when available on board) is delivered by a 32.768 kHz crystal.

Configure_RTC_TimeStamp() function is then called to initialize the time stamp feature
with interrupt mode. It configures the time stamp pin to be rising edge and enables
the time stamp detection on time stamp pin.

LL_RTC_DATE_Config()and LL_RTC_TIME_Config() functions are then called to initialize the
time and the date.

The associated firmware performs the following:

1. After start-up the program configure the RTC (Time date) and enable the feature
   timeStamp.

2. When applying a rising edge on the time stamp pin (PC.13 (Pin 23, CN11)),
   a time stamp event is detected and the calendar is saved in the time stamp structures.
   The current date and time stamp are updated and displayed.
   on the debugger (live watch) in aShowTimeStamp and aShowDateStamp variables .
   The current time and date are updated and displayed on the debugger (live watch) in aShowTime, aShowDate variables.

**Note:** On this board, time stamp pin is not connected to USER push-button (pin PC.13).
Therefore, an external connection is needed:

 - to generate a time stamp event using external signal: connect PC.13 (Pin 23, CN11) to voltage level 0V or 3.3V.

LD1 is toggling : This indicates that the system generates an error.

#### <b>Notes</b>

 1. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

RTC, Timer, Timestamp, Counter, LSE, LSI, Current time, Real Time Clock

### <b>Directory contents</b>

  - RTC/RTC_TimeStamp_Init/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - RTC/RTC_TimeStamp_Init/Boot/Inc/main.h                        Header for main.c module
  - RTC/RTC_TimeStamp_Init/Boot/Inc/stm32_assert.h                Template file to include assert_failed function
  - RTC/RTC_TimeStamp_Init/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - RTC/RTC_TimeStamp_Init/Boot/Src/main.c                        Main program
  - RTC/RTC_TimeStamp_Init/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b> 

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.


### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
