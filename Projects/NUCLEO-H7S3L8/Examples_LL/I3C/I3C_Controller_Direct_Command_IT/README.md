## <b>I3C_Controller_Direct_Command_IT Example Description</b>

How to handle a Direct Command procedure between an I3C Controller and an I3C Target,
using IT.

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
The project is split in two workspaces:
the Controller Board (I3C_Controller_Direct_Command_IT) and the Target Board (I3C_Target_Direct_Command_IT)

- Controller Board
    The Handle_ENTDAA_Controller() function allow the Controller to
  manage a Dynamic Address Assignment procedure to Target connected on the bus.
  This communication is done at 12.5Mhz based on I3C source clock which is at 150MHz.

  The MX_I3C1_Init(), the Prepare_GET_CCC_Transfer(), Prepare_SET_CCC_Transfer and the Handle_CCC_Controller() functions
  allow respectively the configuration of the different I3C hardware settings like timing, internal hardware FIFOs,
  the preparation of the message buffer which contain the different caracteristics of array aGET_CCCList,
  the preparation of the message buffer which contain the different caracteristics of array aSET_CCCList,
  and the treatment of associated Command value using IT mode at 12.5Mhz during Push-pull phase
  based on I3C source clock which is at 150MHz.

For this example the aSET_CCCList and aGET_CCCList are predefined related to Common Command Code descriptor
treated in this example.

In a first step after the user press the USER push-button on the Controller Board,
I3C Controller starts the communication by initiate the Dynamic Address Assignment
procedure through Handle_ENTDAA_Controller().
This procedure allows the Controller to associate Dynamic Address to the Target
connected on the Bus.
User can verify through debug the payload value by watch the content of aTargetDesc
in the field TARGET_BCR_DCR_PID.

Then controller waiting user action.

The user press the USER push-button on the Controller Board to start the communication by sending the first
then all Get CCC element of the aGET_CCCList through Handle_CCC_Controller()
to I3C Targets which receive the Command and treat it by sending the associated data.

At the end, Controller is informed at fully reception of Get CCC element through Controller_Complete_Callback().

At this step, Controller compute the data receive through aRxBuffer and print it through Terminal I/O through DisplayCCCValue().

Terminal I/O watch the list of Get Command Code sent by Controller and associated Target data with IDE in debug mode.
Depending of IDE, to watch content of Terminal I/O note that
 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.
   In Debug configuration : 
   - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
   - Window\Startup,add the command "monitor arm semihosting enable"

At next USER push-button press, the Controller switch to sending the first then all Set CCC element of the aSet_CCCList
through Handle_CCC_Controller() to I3C Target which receive the Command and treat it by receiving the associated data.
At the end, Controller is informed at fully transmission of Set CCC element through Controller_Complete_Callback().

NUCLEO-H7S3L8's LEDs can be used to monitor the transfer status:
 - LD1 is toggle quickly to wait for USER push-button press.
 - LD1 is toggle at ENTDAA reception and each time the Command Code process is completed.
 - LD1 is toggle slowly when there is an error in Command Code process.  

Terminal I/O watch the list of Get Command Code sent by Controller and associated Target data with IDE in debug mode.
Depending of IDE, to watch content of Terminal I/O note that
 - When resorting to EWARM IAR IDE:
   Command Code is displayed on debugger as follows: View --> Terminal I/O

 - When resorting to MDK-ARM KEIL IDE:
   Command Code is displayed on debugger as follows: View --> Serial Viewer --> Debug (printf) Viewer

 - When resorting to STM32CubeIDE:
   Command Code is displayed on debugger as follows: Window--> Show View--> Console.
   In Debug configuration : 
   - Window\Debugger, select the Debug probe : ST-LINK(OpenOCD)
   - Window\Startup,add the command "monitor arm semihosting enable"

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

Connectivity, Communication, I3C, Interrupt, Controller, Target, Dynamic Address Assignment, Direct Command,
Transmission, Reception

### <b>Directory contents</b>

  - I3C/I3C_Controller_Direct_Command_IT/Boot/Inc/desc_target1.h            Target Descriptor
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Inc/desc_target2.h            Target Descriptor
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Inc/stm32h7rsxx_it.h            I3C interrupt handlers header file
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Inc/main.h                    Header for main.c module
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Inc/stm32_assert.h            Template file to include assert_failed function
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Src/stm32h7rsxx_it.c            I3C interrupt handlers
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Src/main.c                    Main program
  - I3C/I3C_Controller_Direct_Command_IT/Boot/Src/system_stm32h7rsxx.c        STM32H7RSxx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H7S3L8Hx devices.

  - This example has been tested with NUCLEO-H7S3L8 board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H7S3L8 Set-up

    - Use short wire as possible between the boards or twist an independent ground wire on each I3C lines
      mean one GND twist around SCL and one GND twist around SDA to help communication at 12.5Mhz.
    - Connect I3C_SCL line of Controller board (PB8 Arduino SCL/D15 CN7 pin 2, Morpho CN12 pin3) to I3C_SCL line of
      Target Board (PB8 Arduino SCL/D15 CN7 pin 2, Morpho CN12 pin3).
    - Connect I3C_SDA line of Controller board (PB9 Arduino SDA/D14 CN7 pin 4, Morpho CN12 pin5) to I3C_SDA line of
      Target Board (PB9 Arduino SDA/D14 CN7 pin 4, Morpho CN12 pin5).
    - Connect GND of Controller board to GND of Target Board.

  - Launch the program in debug mode on Controller board side, and in normal mode on Target side
    to benefit of Terminal I/O information on Controller side.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into Controller memory (I3C_Controller_Direct_Command_IT)
 - Then rebuild all files and load your image into Target memory (I3C_Target_Direct_Command_IT)
 - Run the Controller in debug mode before run the Target in normal mode.
 This sequence will prevent a false startup phase on Target side
 as there is no high level on the bus, if the Target is started before the Controller.
