## <b>SPI_OneBoard_HalfDuplex_IT_Init Example Description</b>

Configuration of GPIO and SPI peripherals to transmit bytes
from an SPI Master device to an SPI Slave device in Interrupt mode. This example
is based on the STM32H7RSxx SPI LL API. The peripheral initialization uses
LL unitary service functions for optimization purposes (performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

This example works with only one NUCLEO-H7S3L8.

SPI1 Peripheral is configured in Master mode Half-Duplex Tx.
SPI3 Peripheral is configured in Slave mode Half-Duplex Rx.
GPIO associated to User push-button is linked with EXTI.

Example execution:

LD1 is blinking Fast (200ms) and wait USER push-button action.
Press USER push-button on BOARD start a Half-Duplex communication through IT.
On MASTER side (SPI1), Clock will be generated on SCK line, Transmission done on MOSI Line.
On SLAVE side (SPI3) reception is done through the MISO Line.

LD1 is On if data is well received.

In case of errors, LD1 is blinking Slowly (1s).

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

Connectivity, SPI, Half-duplex, Transmission, Reception, Master, Slave, MISO, MOSI, Interrupt

### <b>Directory contents</b>

  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Boot/Inc/main.h                  Header for main.c module
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Boot/Src/main.c                  Main program
  - SPI/SPI_OneBoard_HalfDuplex_IT_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect Master SCK  PA5 to Slave SCK  PB3
    - Connect Master MOSI PB5 to Slave MISO PB4

Relation with Board connector:
  PA5  is connected to pin 10 of CN7 connector
  PB5  is connected to pin 14 of CN7 connector
  PB3 is connected to pin 15 of CN7 connector
  PB4 is connected to pin 19 of CN7 connector

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

