## <b>I3C_Controller_Private_Command_IT Example Description</b>

How to handle I3C as Controller data buffer transmission/reception between two boards, using interrupt.

      - Board: NUCLEO-H7S3L8 (embeds a STM32H7S3L8Hx device)
      - SCL Pin: PB8 (Arduino SCL/D15 CN7 pin 2, Morpho CN12 pin3) to PB8 (Arduino SCL/D15 CN7 pin 2, Morpho CN12 pin3)
      - SDA Pin: PB9 (Arduino SDA/D14 CN7 pin 4, Morpho CN12 pin5) to PB9 (Arduino SDA/D14 CN7 pin 4, Morpho CN12 pin5)
      Connect GND between each board
      Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.

At the beginning of the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 600 MHz.

This project calls also SCB_EnableICache() and SCB_EnableDCache() functions in order to enable
the Layer 1 Core Instruction and Data Caches.

The I3C peripheral configuration and I3C resources (CLOCK, GPIO and NVIC) are ensured by the MX_I3C1_Init() function.
User may update this function to change I3C configuration.

To have a better signal startup, the user must adapt the parameter BusFreeDuration
depends on its hardware constraint. The value of BusFreeDuration set on this example
is link to Nucleo hardware environment.

The I3C communication is then initiated.
The project is split in two workspace:
the Controller Board (I3C_Controller_Private_Command_IT) and the Target Board (I3C_Target_Private_Command_IT)

- Controller Board
    The Handle_ENTDAA_Controller() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 150MHz.

  The MX_I3C1_Init(), the Prepare_Private_Transfer(), and Handle_Private_Controller() functions
  allow respectively the configuration of the different I3C hardware settings like timing, internal hardware FIFOs,
  the preparation of the message buffer which contain the different caracteristics of array aPrivateDescriptor,
  and the start of private message using IT mode at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 150MHz.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through Handle_ENTDAA_Controller().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of TargetDesc1
in the field TARGET_BCR_DCR_PID.

The second step starts when the user press the USER push-button on the Controller Board,
the I3C Controller sends aTxBuffer through Handle_Private_Controller() to I3C Target.

The third step starts when the user press the USER push-button on the Controller Board,
the I3C Target sends aTxBuffer to the I3C Controller which receives aRxBuffer through Handle_Private_Controller().

The end of this two steps are monitored through the Controller_Complete_Callback() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to
check buffers correctness.

NUCLEO-H7S3L8's LEDs can be used to monitor the transfer status:

 - LD1 is toggle quickly to wait for USER push-button press.
 - LD1 is ON when the transmission process is complete.
 - LD1 is OFF when the reception process is complete.
 - LD1 toggle slowly when there is an error in transmission/reception process.

#### <b>Notes</b>
 1. Whenever the application is using ITCM/DTCM memories (@0x0000000 / @0x20000000: not cacheable and only accessible
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
    Please refer to the AN4838 "Managing memory protection unit (MPU) in STM32 MCUs"
    Please refer to the AN4839 "Level 1 cache on STM32F7 Series"

### <b>Keywords</b>

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, Private,
Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_Private_Command_IT/Boot/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_Private_Command_IT/Boot/Inc/stm32h7rsxx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_Private_Command_IT/Boot/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_Private_Command_IT/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Controller_Private_Command_IT/Boot/Src/stm32h7rsxx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_Private_Command_IT/Boot/Src/main.c                    Main program
  - I3C/I3C_Controller_Private_Command_IT/Boot/Src/system_stm32h7rsxx.c        STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB8 Arduino SCL/D15 CN7 pin 2, Morpho CN12 pin3) to I3C_SCL line of Target Board (PB8 Arduino SCL/D15 CN7 pin 2, Morpho CN12 pin3).
    - Connect I3C_SDA line of Controller board (PB9 Arduino SDA/D14 CN7 pin 4, Morpho CN12 pin5) to I3C_SDA line of Target Board (PB9 Arduino SDA/D14 CN7 pin 4, Morpho CN12 pin5).
    - Connect GND of Controller board to GND of Target Board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Private_Command_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_Private_Command_IT)
 - Run the Controller before run the Target, this will let a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
