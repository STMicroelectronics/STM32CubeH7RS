## <b>SPI_TwoBoards_FullDuplex_IT_Master_Init Example Description</b>

Data buffer transmission and reception via SPI using Interrupt mode. This
example is based on the STM32H7RSxx SPI LL API. The peripheral
initialization uses LL unitary service functions for optimization purposes (performance and size).

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The communication is done with 2 boards through SPI.

This example shows how to configure GPIO and SPI peripherals
to use a Full-Duplex communication using IT mode through the STM32H7RSxx COM_INSTANCE1_TYPE LL API.

This example is split in two projects, Master board and Slave board:

- Master Board:
  - SPI1 Peripheral is configured in Master mode.
  - SPI1_IRQn activated.
  - RXP and TXP Interrupts SPI peripheral activated.
  - GPIO associated to USER push-button is linked with EXTI.

- Slave Board:
  - SPI1 Peripheral is configured in Slave mode.
  - SPI1_IRQn activated.
  - RXP and TXP Interrupts SPI peripheral activated.


Example execution:
On BOARD MASTER, LD1 is blinking Fast (200ms) and wait USER push-button action.
Press USER push-button on BOARD MASTER start a Full-Duplex communication through IT.
On MASTER side, Clock will be generated on SCK line, Transmission(MOSI Line) and reception (MISO Line)
will be done at the same time.
SLAVE SPI will received  the Clock (SCK Line), so Transmission(MISO Line) and reception (MOSI Line) will be done also.

LD1 is On on both boards if data is well received.

In case of errors, LD1 is blinking Slowly (1s).

### <b>Note</b>

 1. You need to perform a reset on Master board, then perform it on Slave board
    to have the correct behaviour of this example.

 2. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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

Connectivity, SPI, Full-duplex, Transmission, Reception, Master, MISO, MOSI, Interrupt

### <b>Directory contents</b>

  - SPI/SPI_TwoBoards_FullDuplex_IT_Master_Init/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - SPI/SPI_TwoBoards_FullDuplex_IT_Master_Init/Boot/Inc/main.h                  Header for main.c module
  - SPI/SPI_TwoBoards_FullDuplex_IT_Master_Init/Boot/Inc/stm32_assert.h          Template file to include assert_failed function
  - SPI/SPI_TwoBoards_FullDuplex_IT_Master_Init/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - SPI/SPI_TwoBoards_FullDuplex_IT_Master_Init/Boot/Src/main.c                  Main program
  - SPI/SPI_TwoBoards_FullDuplex_IT_Master_Init/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up
    - Connect Master board PA5 to Slave Board PA5 (connected to pin 10 of CN7 connector)
    - Connect Master board PA6 to Slave Board PA6 (connected to pin 12 of CN7 connector)
    - Connect Master board PB5 to Slave Board PB5 (connected to pin 14 of CN7 connector)
    - Connect Master board GND to Slave Board GND

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
    - Load the Master project in Master Board
    - Load the Slave project in Slave Board
 - Run the example

