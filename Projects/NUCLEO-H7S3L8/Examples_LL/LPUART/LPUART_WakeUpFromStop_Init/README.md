## <b>LPUART_WakeUpFromStop_Init Example Description</b>

Configuration of GPIO and LPUART peripherals to allow characters
received on LPUART_RX pin to wake up the MCU from low-power mode. This example is based
on the LPUART LL API. The peripheral initialization uses LL
initialization function to demonstrate LL init usage.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

LPUART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
LPUART Clock is based on HSI.

Example execution:

After startup from reset and system configuration, LD1 is blinking quickly during 3 sec,
then MCU  enters "Stop" mode (LD1 off).
On first  character reception by the LPUART from PC Com port (ex: using HyperTerminal)
after "Stop" Mode period, MCU wakes up from "Stop" Mode.

Received character value is checked :

- On a specific value ('S' or 's'), LD1 is turned On and program ends.
- If different from 'S' or 's', program performs a quick LD1 blinks during 3 sec and 
  enters again "Stop" mode, waiting for next character to wake up.

In case of errors, LD1 is slowly blinking (1 sec period).

#### <b>Notes</b>

 1. the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Connectivity, LPUART, baud rate, RS-232, HyperTerminal, full-duplex,
Transmitter, Receiver, Asynchronous, Low Power

### <b>Directory contents</b>

  - LPUART/LPUART_WakeUpFromStop_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - LPUART/LPUART_WakeUpFromStop_Init/Boot/Inc/main.h                  Header for main.c module
  - LPUART/LPUART_WakeUpFromStop_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - LPUART/LPUART_WakeUpFromStop_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - LPUART/LPUART_WakeUpFromStop_Init/Boot/Src/main.c                  Main program
  - LPUART/LPUART_WakeUpFromStop_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect STM32 MCU board LPUART1 TX pin (GPIO PA9 connected to pin 21 of CN12 connector)
      to PC COM port RX signal
    - Connect STM32 MCU board LPUART1 RX pin (GPIO PA10 connected to pin 33 of CN12 connector)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

    - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration
      (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control).

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

