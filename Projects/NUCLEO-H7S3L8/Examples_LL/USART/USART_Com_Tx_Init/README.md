## <b>USART_Com_Tx_Init Example Description</b>

This example shows how to configure GPIO and USART peripherals to send characters
asynchronously to an HyperTerminal (PC) in Polling mode. If the transfer could not
be completed within the allocated time, a timeout allows to exit from the sequence
with a Timeout error code. This example is based on STM32H7RSxx USART LL API. Peripheral
initialization is done using LL unitary services functions for optimization purpose
(performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

USART Peripheral is configured in asynchronous mode (115200 bauds, 8 data bit, 1 start bit, 1 stop bit, no parity).
No HW flow control is used.
GPIO associated to User push-button is linked with EXTI.
Virtual Com port feature of STLINK could be used for UART communication between board and PC.

Example execution:

On press on push button , first character of buffer to be transmitted is written into USART Transmit Data Register (TDR)

Program then starts polling on USART TXE flag before sending next character.

On last character, program polls on TC flag to ensure transmit of last character is completed.

At completion of Tx Buffer sending, LD1 is turned On.

In case of errors, LD1 is blinking (1sec period).

Program is written so that, any new press on User push-button will lead to new transmission of complete buffer.

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

Connectivity, UART/USART, Asynchronous, RS-232, baud rate, Polling, HyperTerminal, Transmitter

### <b>Directory contents</b>

  - USART/USART_Com_Tx_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - USART/USART_Com_Tx_Init/Boot/Inc/main.h                  Header for main.c module
  - USART/USART_Com_Tx_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - USART/USART_Com_Tx_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - USART/USART_Com_Tx_Init/Boot/Src/main.c                  Main program
  - USART/USART_Com_Tx_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    Connect USART1 TX/RX to respectively RX and TX pins of PC UART (could be done through a USB to UART adapter) :
    - Connect STM32 MCU board USART1 TX pin (GPIO PB.14 connected to pin 14 of connector CN10)
      to PC COM port RX signal
    - Connect STM32 MCU board USART1 RX pin (GPIO PA.10 connected to pin 16 of connector CN10)
      to PC COM port TX signal
    - Connect STM32 MCU board GND to PC COM port GND signal

  - Launch serial communication SW on PC (as HyperTerminal or TeraTerm) with proper configuration
    (115200 bauds, 8 bits data, 1 stop bit, no parity, no HW flow control).

  - Launch the program. Press on User push button on board to initiate data transfer.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

