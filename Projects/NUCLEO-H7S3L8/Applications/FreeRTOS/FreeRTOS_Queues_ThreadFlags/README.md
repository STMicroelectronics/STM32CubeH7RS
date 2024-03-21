
## <b>FreeRTOS_Queues_ThreadFlags Application Description</b>

This application demonstrates the use of message queues, and thread flags with CMSIS_RTOS2 API.

This application runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.

The application creates 4 Tasks and 1 Queue:

  - 'ProducerThread': Sends message (incrementing value "ProducerValue") to the queue.

  - 'ConsumerThread': Gets message from the queue
                      Checks if it's the correct message (ProducerValue == ConsumerValue)
                      LED_GREEN ON

  - 'GenThread': Waits for the message on the queue to reach a specific "GenerationValue"
                 Sets ReceiveThread' flag to 1

  - 'ReceiveThread': Waits for the thread flag settings
                     LED_YELLOW ON

#### <b>Expected success behavior</b>

- Successful operation is marked by a turning ON the Green LED then the Yellow LED.
- Information about the application will be printed to the serial port.

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

FreeRTOS, Threading, Message, Queues, ThreadFlags

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

This application runs from the external Flash memory (FreeRTOS_Queues_ThreadFlags Appli).
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
