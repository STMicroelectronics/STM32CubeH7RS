## <b>ADC_AnalogWatchdog_Init Example Description</b>

How to use an ADC peripheral with an ADC analog watchdog to monitor a channel
and detect when the corresponding conversion data is outside the window
thresholds.

This example is based on the STM32H7RSxx ADC LL API.

The peripheral initialization is done using LL unitary service functions
for optimization purposes (performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

Example configuration:

ADC is configured to convert a single channel, in continuous conversion mode,
from SW trigger.

Analog watchdog 1 is configured to monitor all channels on ADC group regular
(therefore, including the selected channel).

Analog watchdog thresholds values:

- high threshold is set to Vref+/2 (3.3V/2=1.65V on NUCLEO-H7S3L8), refer to literal ADC_AWD_THRESHOLD_HIGH.

- low threshold is set to 0V, refer to literal ADC_AWD_THRESHOLD_LOW.

ADC interruption enabled: Analog watchdog 1.

Example execution:

From the main program execution, the ADC converts the selected channel continuously.

When conversion data is out of analog watchdog window, ADC interruption occurs
and analog watchdog callback function is called to process the event.

Every 5 seconds, the ADC analog watchdog is rearmed to be ready for another trig.

LD1 is used to monitor program execution status:

- Normal operation: Voltage of ADC channel monitored versus analog watchdog window
  can be observed with LED toggle frequency:
  - Voltage within analog watchdog window: toggle frequency slow (2Hz)
  - Voltage below analog watchdog window: toggle frequency fast (10Hz)
  - Voltage above analog watchdog window: toggle frequency very fast (20Hz)
- Error: LED remaining turned on

Note: In this example, analog watchdog low threshold is set to 0V
      for test simplicity purpose, therefore case of voltage below low threshold
      is not achievable unless cutomizing low threshold value.

Note: In case of noise on voltage applied on ADC channel input (due to connectors and wires parasitics),
      ADC analog watchdog may trig at a voltage level with an uncertainty of tens of mV.

Debug: variables to monitor with debugger:

- "ubAnalogWatchdog1Status": analog watchdog state

Connection needed:

Use an external power supply to generate a voltage in range [0V; 3.3V]
and connect it to analog input pin (cf pin below).

Note: If no power supply available, this pin can be let floating (in this case
      ADC conversion data will be undetermined).

Other peripherals used:

  1 GPIO for LD1

  1 GPIO for analog input: ADC channel 10 on pin PC.00 (Arduino A1 pin 3 CN9, Morpho PIN 38 CN4)

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

ADC, analog digital converter, analog, conversion, voltage, channel, analog input, trigger, analog watchdog, threshold, window

### <b>Directory contents</b>

  - ADC/ADC_AnalogWatchdog_Init/Boot/Inc/stm32h7rsxx_it.h          Interrupt handlers header file
  - ADC/ADC_AnalogWatchdog_Init/Boot/Inc/main.h                    Header for main.c module
  - ADC/ADC_AnalogWatchdog_Init/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - ADC/ADC_AnalogWatchdog_Init/Boot/Src/stm32h7rsxx_it.c          Interrupt handlers
  - ADC/ADC_AnalogWatchdog_Init/Boot/Src/main.c                    Main program
  - ADC/ADC_AnalogWatchdog_Init/Boot/Src/system_stm32h7rsxx.c      STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

