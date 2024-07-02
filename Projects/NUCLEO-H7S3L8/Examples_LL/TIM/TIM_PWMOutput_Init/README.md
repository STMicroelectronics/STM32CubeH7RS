## <b>TIM_PWMOutput_Init Example Description</b>

Use of a timer peripheral to generate a 
PWM output signal and update the PWM duty cycle. This example is based on the 
STM32H7RSxx TIM LL API. The peripheral initialization uses 
LL initialization function to demonstrate LL Init.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

In this example TIM3 input clock TIM3CLK is set to APB clock (PCLK),
since APB pre-scaler is equal to 2.
    TIM3CLK = PCLK
    PCLK = HCLK
    => TIM3CLK = HCLK = SystemCoreClock/2 (300 MHz)

To set the TIM3 counter clock frequency to 10 KHz, the pre-scaler (PSC) is
calculated as follows:

	PSC = (TIM3CLK / TIM3 counter clock) - 1
	PSC = ((SystemCoreClock/2) /10 KHz) - 1

SystemCoreClock is set to 600 MHz for STM32H7RSxx Devices.

Auto-reload (ARR) is calculated to get a time base period of 10 ms,
meaning a time base frequency of 100 Hz.

	ARR = (TIM3 counter clock / time base frequency) - 1
	ARR = (TIM3 counter clock / 100) - 1

Initially, the capture/compare register (CCR1) of the output channel is set to
half the auto-reload value meaning a initial duty cycle of 50%.
Generally speaking this duty cycle is calculated as follows:

	Duty cycle = (CCR1 / ARR) * 100

The timer output channel must be connected to TIM3_CH1 on board NUCLEO-H7S3L8.
Thus TIM3_CH1 status (on/off) mirrors the timer output level (active v.s. inactive).

USER push-button can be used to change the duty cycle from 0% up to 100% by
steps of 10%. Duty cycle is periodically measured. It can be observed through
the debugger by watching the variable uwMeasuredDutyCycle.

Initially the output channel is configured in output compare toggle mode.

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

Timers, TIM, Output, signal, PWM, Oscilloscope, Frequency, Duty cycle, Waveform

### <b>Directory contents</b>

  - TIM/TIM_PWMOutput_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - TIM/TIM_PWMOutput_Init/Boot/Inc/main.h                  Header for main.c module
  - TIM/TIM_PWMOutput_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_PWMOutput_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - TIM/TIM_PWMOutput_Init/Boot/Src/main.c                  Main program
  - TIM/TIM_PWMOutput_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.
    
  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.
  - NUCLEO-H7S3L8 Set-up:
      - TIM3_CH1  PB4: connected to pin 19 of CN7 connector 

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


