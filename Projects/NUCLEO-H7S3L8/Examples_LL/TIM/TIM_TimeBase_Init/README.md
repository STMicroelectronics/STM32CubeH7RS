## <b>TIM_TimeBase_Init Example Description</b>

Configuration of the TIM peripheral to generate a timebase. This 
example is based on the STM32H7RSxx TIM LL API. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size). 

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example TIM1 input clock TIM1CLK is set to APB2 clock PCLK2 (TIM1CLK = PCLK2).
      As APB2 pre-scaler is equal to 2 PCLK2 = HCLK, and since AHB pre-scaler is equal to 2,
      => TIM1CLK = SystemCoreClock/2 (300 MHz)

To set the TIM1 counter clock frequency to 10 KHz, the pre-scaler (PSC) is calculated as follows:

	PSC = (TIM1CLK / TIM1 counter clock) - 1
	PSC = ((SystemCoreClock/2) /10 KHz) - 1

SystemCoreClock is set to 600 MHz for STM32H7RSxx Devices.

The auto-reload (ARR) is calculated to get a timebase period of 100ms,
meaning that initial timebase frequency is 10 Hz.

	ARR = (TIM1 counter clock / timebase frequency) - 1
	ARR = (TIM1 counter clock / 10) - 1

Update interrupts are enabled. Within the update interrupt service routine, pin PD10
(connected to LD1 on board NUCLEO-H7S3L8) is toggled. So the period of
blinking of LD1 = 2 * timebase period.

USER push-button can be used to modify the timebase period from 100 ms
to 1 s in 100 ms steps. To do so, every time USER push-button is pressed, the
autoreload register (ARR) is updated. In up-counting update event is generated 
at each counter overflow (when the counter reaches the auto-reload value). 

Finally the timebase frequency is calculated as follows:
timebase frequency = TIM1 counter clock /((PSC + 1)*(ARR + 1)*(RCR + 1))

#### <b>Notes</b>

 1. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Timer, TIM, timebase, Interrupt, Clock source

### <b>Directory contents</b>

  - TIM/TIM_TimeBase_Init/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - TIM/TIM_TimeBase_Init/Boot/Inc/main.h                  Header for main.c module
  - TIM/TIM_TimeBase_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_TimeBase_Init/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - TIM/TIM_TimeBase_Init/Boot/Src/main.c                  Main program
  - TIM/TIM_TimeBase_Init/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
    
  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


