## <b>FreeRTOS_MPU Application Description</b>

This application demonstrates the use of the MPU with FreeRTOS to control memory/peripheral access for tasks.
It implements simple recovery mechanisms from prohibited memory accesses.
The application creates one Privileged mode task using xTaskCreate():

  - 'MainTask': Has higher priority than the unprivileged tasks.
                Blocks while waiting for a fault to occur.
                Displays information about the faults and takes corrective actions.
It also has User mode tasks created using the xTaskCreateRestricted() API:

  - 'Task1': Has read only access to shared memory
             Attempts an illegal write to the shared memory only once
             Toggles the green LED
  - 'Task2': Has read only access to shared memory
             Attempts an illegal write to the shared memory periodically
             Toggles the yellow LED
The application will try to recover from MemManage fault exception for each task until EXAMPLE_FAULT_COUNT_THRESHOLD is reached for that task. At that point the task will be suspended to
avoid further exceptions.

- This application boots from internal Flash and jumps to the application code in external Flash (Sub-project Boot).
- This project runs from the external Flash memory. It is launched from a first boot stage and inherits from this boot project
configuration (caches, MPU regions [region 0 and 1], system clock at 600 MHz and external memory interface at the highest speed).
Note that the boot part is automatically downloaded from the IDE environment via the board project Templates/Template_XIP/Binary/Boot_XIP.hex file.
- The template project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

#### <b>Expected success behavior</b>

- Successful operation is marked by a toggling green LED light.
- The yellow LED will toggle "EXAMPLE_FAULT_COUNT_THRESHOLD" times.
- Information about the faults will be printed to the serial port.

#### <b>Expected error behavior</b>

- "Critical Error occurred" message will be printed to the serial port.
- On failure, the red LED toggles.

#### <b>Assumptions if any</b>
None

#### <b>Known limitations</b>
None

### <b>Notes</b>

  - When used with the MPU, FreeRTOS requires various regions to be defined in the linker file.
    For more information, refer to the linker files in the example.

  - MPU regions are required to have a size which is a multiple of 32 bytes and to be aligned on the 32 byte boundary.
    In particular, unprivileged task stacks must comply with these requirements.

  - FreeRTOS uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.

#### <b>FreeRTOS usage hints</b>
The FreeRTOS heap size "configTOTAL_HEAP_SIZE" defined in FreeRTOSConfig.h is set accordingly to the
OS resources memory requirements of the application with +10% margin and rounded to the upper Kbyte boundary.
For more details about FreeRTOS implementation on STM32Cube, please refer to UM1722 "Developing Applications
on STM32Cube with RTOS".

### <b>Keywords</b>
FreeRTOS, RTOS, MPU, Threading

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

This application runs from the external Flash memory (FreeRTOS_MPU Appli).
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
