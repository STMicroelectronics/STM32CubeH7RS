## <b>FreeRTOS_Semaphore_LowPower Application Description</b>

This application demonstrates the use of FreeRTOS tickless low power mode and semaphores.

This application runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

The system relies on a low power timer to wake up from its low power state.

The thread blocks on a semaphore.
This causes the system to start the low power timer and enter STOP mode.

Once the timer expires, an interrupt is triggered and the CPU returns to RUN mode.
The semaphore is released in the timer's interrupt handler. The thread resumes execution and
toggles the green LED.

#### <b>Expected success behavior</b>

- Successful operation is marked by the green LED toggling every second.

#### <b>Expected error behavior</b>

- On failure, the red LED toggles.

#### <b>Assumptions if any</b>
None

#### <b>Known limitations</b>
None

### <b>Notes</b>

To measure the current consumption remove JP2 jumper and connect an amperemeter to JP2 to measure IDD current.

This example can not be used in DEBUG mode due to the fact that the Cortex-M7 core is no longer clocked during low power mode so debugging features are disabled.

It is possible to reduce power consumption even further by configuring all pins as analog pins and disabling their clocks.

FreeRTOS uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.

#### <b>FreeRTOS usage hints</b>

The FreeRTOS heap size "configTOTAL_HEAP_SIZE" defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

### <b>Keywords</b>

FreeRTOS, RTOS, Semaphore, LowPower, LPTIM

### <b>Hardware and Software environment</b>

  - This application runs on NUCLEO-H7S3L8 device.
  - This application has been tested with STMicroelectronics NUCLEO-H7S3L8 board MB1737 Rev. B01
    and can be easily tailored to any other supported device and development board.

  - This application uses USART3 to display output, the hyperterminal configuration is as follows:

      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = none
      - Flow control = None


### <b>How to use it ?</b>

This application runs from the external Flash memory (FreeRTOS_Semaphore_LowPower Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX25UW25645G_STM32H7R38-NUCLEO.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the application
