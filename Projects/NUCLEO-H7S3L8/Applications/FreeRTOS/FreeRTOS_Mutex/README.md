
## <b>FreeRTOS_Mutex Application Description</b>

This application demonstrates the use of mutexes to serialize access to a shared resource.

This application runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

The application has two threads:

    -Thread1 : Prints messages counting from 1 to 10 ("Thread1 : This is message number x")
               Blinks the green LED every second

    -Thread2 : Prints messages counting from 1 to 10 ("Thread2 : This is message number x")
               Blinks the yellow LED every second

The mutex is created before the threads using it are scheduled. Each thread will try to acquire it
before outputting characters to the serial port. Once the mutex is acquired, the thread will proceed
to printing its message, and then immediately releases it.

If the mutex is not used, the two threads will try to use "printf" at almost the same time. This will
result in either thread winning over the other or both threads interrupting each other.

The user can observe the effect of not using a mutex to protect printf by setting the
"EXAMPLE_USES_MUTEX" define in Appli/Src/main.c to 0.

#### <b>Expected success behavior</b>

- If the "EXAMPLE_USES_MUTEX" is set to 0, some messages will not be printed, or may be corrupted.
- If the "EXAMPLE_USES_MUTEX" is set to 1, each thread will display 10 messages identified by the thread name and the message number , for a total of 20 messages. Additional messages will be printed to displayed when the mutex is acquired or released.
- The yellow and green LEDs will toggle every second.

#### <b>Expected error behavior</b>

- "Critical Error occurred" message will be printed to the serial port.
- On failure, the red LED toggles.

#### <b>Assumptions if any</b>

None

#### <b>Known limitations</b>

None

### <b>Notes</b>

  - FreeRTOS uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.

#### <b>FreeRTOS usage hints</b>

The FreeRTOS heap size "configTOTAL_HEAP_SIZE" defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.

For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

### <b>Keywords</b>

FreeRTOS, Mutex

### <b>Hardware and Software environment</b>

  - This application runs on NUCLEO-H7S3L8 device.
  - This application has been tested with STMicroelectronics NUCLEO-H7S3L8 board MB1737 Rev. B02
    and can be easily tailored to any other supported device and development board.

  - This application uses USART3 to display output, the hyperterminal configuration is as follows:

    - BaudRate = 115200 baud
    - Word Length = 8 Bits
    - Stop Bit = 1
    - Parity = None
    - Flow control = None


### <b>How to use it ?</b>

This application runs from the external Flash memory (FreeRTOS_Mutex Appli).
It boots from internal Flash (Template_XIP_Boot) then jumps to the application code in external Flash.

To configure STM32CubeIDE Debug Configuration, you must do the following :

    1. Upload the template XIP
    2. Add the adequate external loader (MX25UW25645G_STM32H7S3L8-NUCLEO.stldr file) in Project->Debugger Configuration
    3. Add in the startup the template_XIP_Boot in Project->Debugger Configuration
    4. Move up the application in the startup

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the application
