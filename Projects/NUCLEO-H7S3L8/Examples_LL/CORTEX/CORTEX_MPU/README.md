## <b>CORTEX_MPU example Description</b>

Presentation of the MPU features. This example configures MPU attributes of different
MPU regions then configures a memory area as privileged read-only, and attempts to
perform read and write operations in different modes.

If the access is permitted LD1 is toggling. If the access is not permitted,
a memory management fault is generated and LD1 is ON.

To generate an MPU memory fault exception due to an access right error, press
user button.

3 MPU regions are declared in this example:

- a background region (region 0) to prevent speculative accesses on Normal type memory
- a non-cacheable data region (region 1) dedicated to store for instance buffer used by a DMA
- a privileged read-only data (region 2) used to demonstrate the use of MPU on read and write accesses.

### <b>Notes</b>

For more details about the MPU configuration and use, please refer to AN4838 “Managing memory protection unit (MPU) in STM32 MCUs”

Using STM32CubeIDE debugger, when hard fault is generated user shall press the debugger "Run" button to continue the execution

### <b>Keywords</b>

MPU, interrupt, MPU memory fault exception,

### <b>Directory contents</b>

  - CORTEX/CORTEX_MPU/Appli/Inc/stm32h7rsxx_it.h         Interrupt handlers header file
  - CORTEX/CORTEX_MPU/Appli/Inc/main.h                  Header for main.c module
  - CORTEX/CORTEX_MPU/Appli/Inc/stm32_assert.h          Template file to include assert_failed function
  - CORTEX/CORTEX_MPU/Appli/Src/stm32h7rsxx_it.c         Interrupt handlers
  - CORTEX/CORTEX_MPU/Appli/Src/main.c                  Main program
  - CORTEX/CORTEX_MPU/Appli/Src/system_stm32h7rsxx.c     STM32H7RSxx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with a STM32H7S3L8Hx embedded on an
    NUCLEO-H7S3L8 board and can be easily tailored to any other supported
    device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

