## <b>UTILS_ReadDeviceInfo Example Description</b>

This example reads the UID, Device ID and Revision ID and saves
them into a global information buffer.
This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

Then UID, Device ID and Revision ID will be saved in following global variables:

- aShowDeviceID: Device identifier
- aShowRevisionID: Revision identifier (This field indicates the revision of the device.
  For example, it is read as 0x1000 for Revision 1.0)
- aShowUIDWord0: UID Word0
- aShowUIDWord1: UID Word1
- aShowUIDWord2: UID Word2

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

System, Utils, Device info, UID, Device ID, Revision ID, Wafer, Lot number

### <b>Directory contents</b>

  - UTILS/UTILS_ReadDeviceInfo/Boot/Inc/stm32h7rsxx_it.h        Interrupt handlers header file
  - UTILS/UTILS_ReadDeviceInfo/Boot/Inc/main.h                  Header for main.c module
  - UTILS/UTILS_ReadDeviceInfo/Boot/Inc/stm32_assert.h          Template file to include assert_failed function  
  - UTILS/UTILS_ReadDeviceInfo/Boot/Src/stm32h7rsxx_it.c        Interrupt handlers
  - UTILS/UTILS_ReadDeviceInfo/Boot/Src/main.c                  Main program
  - UTILS/UTILS_ReadDeviceInfo/Boot/Src/system_stm32h7rsxx.c    STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 